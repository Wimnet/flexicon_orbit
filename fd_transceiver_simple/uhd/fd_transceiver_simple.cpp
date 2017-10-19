//
// Copyright 2010-2012,2014-2015 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "wavetable.hpp"
#include <uhd/types/tune_request.hpp>
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/static.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <boost/thread/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <csignal>

#include <Eigen/Dense>
#include <Eigen/SVD>
#include <vector>
#include <complex>
#include <stdint.h>
#include <cmath>
#include "fft.hpp"
#include <typeinfo>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>

#include "dig_sic.hpp"

namespace po = boost::program_options;

// global variable
int PREA_LEN = 500;
int DIG_CANC_PREA_LEN = 200;

int DATA_LEN = 2048;
int DIG_CANC_DATA_LEN = 1024;

int DIG_CANC_CHNL_LEN = 17;

// global buffers
int gv_tbuff_len = 10; int gv_rbuff_len = 10;
int gv_tbuff_idx = 0;  int gv_rbuff_idx = 0;
Eigen::VectorXcf gv_tbuff;
Eigen::VectorXcf gv_rbuff;
Eigen::VectorXf gv_tbuff_re = Eigen::VectorXf::Zero(20440);
Eigen::VectorXf gv_tbuff_im = Eigen::VectorXf::Zero(20440);
Eigen::VectorXf gv_rbuff_re = Eigen::VectorXf::Zero(20440);
Eigen::VectorXf gv_rbuff_im = Eigen::VectorXf::Zero(20440);

Eigen::VectorXf gv_rbuff_res_re = Eigen::VectorXf::Zero(20440);

double gv_rx_peak_dBm = 0.0;
double gv_res_peak_dBm = 0.0;

Eigen::VectorXd gv_rx_sig_spec = Eigen::VectorXd::Zero(32768);
Eigen::VectorXd gv_res_sig_spec = Eigen::VectorXd::Zero(32768);

boost::mutex mtx;

/***********************************************************************
* Signal handlers
**********************************************************************/
static bool stop_signal_called = false;
void sig_int_handler(int){stop_signal_called = true;}


volatile sig_atomic_t main_stop_signal;
void my_handler(int signum) {main_stop_signal = 1;}
/***********************************************************************
* Utilities
**********************************************************************/
//! Change to filename, e.g. from usrp_samples.dat to usrp_samples.00.dat,
//  but only if multiple names are to be generated.
std::string generate_out_filename(const std::string &base_fn, size_t n_names, size_t this_name)
{
    if (n_names == 1) {
        return base_fn;
    }

    boost::filesystem::path base_fn_fp(base_fn);
    base_fn_fp.replace_extension(
        boost::filesystem::path(
            str(boost::format("%02d%s") % this_name % base_fn_fp.extension().string())
        )
    );
    return base_fn_fp.string();
}


/***********************************************************************
* transmit_worker function
* A function to be used as a boost::thread_group thread for transmitting
**********************************************************************/
void transmit_worker(
    std::vector<std::complex<float> > buff,
    wave_table_class wave_table,
    uhd::tx_streamer::sptr tx_streamer,
    uhd::tx_metadata_t metadata,
    size_t step,
    size_t index,
    int num_channels
){
    //outfiles
    std::ofstream outfile_tx_samps("gv_tbuff.dat", ios::out | ios::binary | ios::trunc);

    std::vector<std::complex<float> *> buffs(num_channels, &buff.front());

    float tx_start_step = 0.0;
    int tx_dice = 0;
    srand (time(NULL));

    mtx.lock();
    gv_tbuff_idx = 0;
    mtx.unlock();

    //send data until the signal handler gets called
    while(not stop_signal_called){

        tx_start_step = (rand() % 2) * 0.5;

        //fill the buffer with the waveform
        /*
        for (int n = 0; n < tx_buff_size/10; n++){
        buff[n].real() = tx_start_step;
        buff[n].imag() = tx_start_step;
        mtx.lock();
        // std::cout << gv_tbuff_idx << ", " << tx_buff_size << ", " << gv_tbuff_idx*tx_buff_size + n << std::endl;
        gv_tbuff[n] = buff[n];
        gv_tbuff_re[n] = real(buff[n]);
        gv_tbuff_im[n] = imag(buff[n]);
        mtx.unlock();
    }
    */
    for (size_t n = 0; n < buff.size(); n++){
        buff[n] = wave_table(index += step);
        mtx.lock();
        gv_tbuff[n] = buff[n];
        gv_tbuff_re[n] = real(buff[n]);
        gv_tbuff_im[n] = imag(buff[n]);
        mtx.unlock();
    }

    mtx.lock();
    gv_tbuff_idx = gv_tbuff_idx + 1;
    if (gv_tbuff_idx >= gv_tbuff_len) {
        gv_tbuff_idx = 0;
    }
    mtx.unlock();

    tx_start_step += 0.05;
    if (tx_start_step >= 0.25) {
        tx_start_step = 0.25;
    }

    //send the entire contents of the buffer
    tx_streamer->send(buffs, buff.size(), metadata);

    metadata.start_of_burst = false;
    metadata.has_time_spec = false;

    // write Eigen vector to file
    /*
    mtx.lock();
    // outfile_tx_samps.write((char*) gv_tbuff.data(), buff.size()*sizeof(float)*2 );  // *2 b/c of complex value
    std::ofstream outfile_tx_samps_single("gv_tbuff_single.dat", ios::out | ios::binary | ios::trunc);
    outfile_tx_samps_single.write((char*) gv_tbuff.data(), buff.size()*sizeof(float)*2 );  // *2 b/c of complex value
    outfile_tx_samps_single.close();
    mtx.unlock();
    */
}

//send a mini EOB packet
metadata.end_of_burst = true;
tx_streamer->send("", 0, metadata);
}


/***********************************************************************
* receiver worker
**********************************************************************/
// "fc32", otw, file, spb, total_num_samps, settling, rx_channel_nums, start_tx_stream_time
void receive_worker(
    uhd::usrp::multi_usrp::sptr usrp,
    uhd::stream_args_t stream_args,
    size_t samps_per_buff,
    int num_requested_samples,
    float settling_time,
    std::vector<size_t> rx_channel_nums,
    uhd::time_spec_t start_rx_stream_time
){
    int num_total_samps = 0;

    uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);

    // Prepare buffers for received samples and metadata
    uhd::rx_metadata_t md;
    std::vector <std::vector< std::complex<float> > > buffs(
        rx_channel_nums.size(), std::vector< std::complex<float> >(samps_per_buff)
    );
    //create a vector of pointers to point to each of the channel buffers
    std::vector<std::complex<float> *> buff_ptrs;
    for (size_t i = 0; i < buffs.size(); i++) {
        buff_ptrs.push_back(&buffs[i].front());
    }

    bool overflow_message = true;
    float timeout = settling_time + 0.1f; //expected settling time + padding for first recv

    //setup streaming
    uhd::stream_cmd_t stream_cmd((num_requested_samples == 0) ? uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS:
    uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
    stream_cmd.num_samps = num_requested_samples;
    stream_cmd.stream_now = false;
    // stream_cmd.time_spec = uhd::time_spec_t(settling_time);
    stream_cmd.time_spec = start_rx_stream_time;
    rx_stream->issue_stream_cmd(stream_cmd);

    while(not stop_signal_called and (num_requested_samples > num_total_samps or num_requested_samples == 0)) {
        size_t num_rx_samps = rx_stream->recv(buff_ptrs, samps_per_buff, md, timeout);
        timeout = 0.1f; //small timeout for subsequent recv

        // copy the values to the global RX buffer
        mtx.lock();
        for (size_t i = 0; i < samps_per_buff; i++) {
            gv_rbuff[i] = buff_ptrs[0][i];
        }
        mtx.unlock();
        mtx.lock();
        for (size_t i = 0; i < samps_per_buff; i++) {
            gv_rbuff_re[i] = real(buff_ptrs[0][i]);
        }
        mtx.unlock();

        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
            std::cout << boost::format("Timeout while streaming") << std::endl;
            break;
        }
        if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW){
            if (overflow_message){
                overflow_message = false;
                std::cerr << boost::format(
                    "Got an overflow indication. Please consider the following:\n"
                    "  Your write medium must sustain a rate of %fMB/s.\n"
                    "  Dropped samples will not be written to the file.\n"
                    "  Please modify this example for your purposes.\n"
                    "  This message will not appear again.\n"
                ) % (usrp->get_rx_rate()*sizeof(std::complex<float>)/1e6);
            }
            continue;
        }
        if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE){
            throw std::runtime_error(str(boost::format(
                "Receiver error %s"
            ) % md.strerror()));
        }

        num_total_samps += num_rx_samps;
    }

    // Shut down receiver
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    rx_stream->issue_stream_cmd(stream_cmd);
}


/* Returns the power spectrum of the input signal
 * @param sig: input signal
 * @param fs: sampling rate
 * @return sig_spec: signal spectrum
 */
Eigen::VectorXd sig_power(Eigen::VectorXf &sig, double fs) 	// fs: sampling rate
{
    int sig_len = sig.size();
    int N_fft = sig_len;

    Eigen::VectorXd x = sig.cast<double>();
    Eigen::VectorXd sig_fft_real(N_fft), sig_fft_imag(N_fft);
    Eigen::VectorXd sig_spec(N_fft), sig_spec_temp(N_fft);

    // half-normalized fft
    fft(x.data(), NULL, N_fft, sig_fft_real.data(), sig_fft_imag.data());
    // renormalize since there is a factor of sqrt(N_fft) from the fft function
    Eigen::ArrayXd sig_power = ( sig_fft_real.array().square() + sig_fft_imag.array().square() ) * (double)sig_len;
    Eigen::VectorXd sig_power_normalized = sig_power.matrix() / sig_len;
    sig_spec.array() = 10 * log10(sig_power_normalized.array());
    // fft circular shift to move zero freq components to the middle
    sig_spec_temp << sig_spec.segment(N_fft/2, N_fft/2), sig_spec.segment(0, N_fft/2);
    sig_spec = sig_spec_temp;

    if (2048 == N_fft){
        std::ofstream outfile_rx_spec_single("rx_spec_single.dat", ios::out | ios::binary | ios::trunc);
        outfile_rx_spec_single.write((char*) sig_spec.data(), N_fft*sizeof(double) );
        outfile_rx_spec_single.close();

        std::ofstream outfile_rx_fft_real("rx_fft_real_single.dat", ios::out | ios::binary | ios::trunc);
        outfile_rx_fft_real.write((char*) sig_fft_real.data(), N_fft*sizeof(double) );
        outfile_rx_fft_real.close();

        std::ofstream outfile_rx_fft_imag("rx_fft_imag_single.dat", ios::out | ios::binary | ios::trunc);
        outfile_rx_fft_imag.write((char*) sig_fft_imag.data(), N_fft*sizeof(double) );
        outfile_rx_fft_imag.close();
    }
    else {
        std::ofstream outfile_res_spec_single("res_spec_single.dat", ios::out | ios::binary | ios::trunc);
        outfile_res_spec_single.write((char*) sig_spec.data(), N_fft*sizeof(double) );
        outfile_res_spec_single.close();
    }

    return sig_spec;
}

/* Digital SIC worker
 * @param sine_freq: sine wave frequency
 * @param fs: sampling rate in Sa/s
 * @param fr: refreash rate msec
 */
void dig_SIC_worker(double sine_freq, double fs, int fr)
{
    int cur_l = DIG_CANC_PREA_LEN;
    int cur_k = DIG_CANC_CHNL_LEN;

    Eigen::VectorXf cur_tx_preamble, cur_rx_preamble;
    Eigen::VectorXf cur_si_chnl_est;
    Eigen::VectorXf cur_tx_data, cur_rx_data;
    Eigen::VectorXf cur_res_data;

    Eigen::VectorXd cur_rx_sig_spec;
    Eigen::VectorXd cur_res_sig_spec;

    int cur_sig_peak_idx = 0;

    while(not stop_signal_called) {
        mtx.lock();
        cur_tx_preamble = gv_tbuff_re.segment(0, PREA_LEN); // 500
        cur_rx_preamble = gv_rbuff_re.segment(0, PREA_LEN);

        cur_tx_data = gv_tbuff_re.segment(PREA_LEN, DATA_LEN); // 2048
        cur_rx_data = gv_rbuff_re.segment(PREA_LEN, DATA_LEN);

        // compute RX signal power
        cur_rx_sig_spec = sig_power(cur_rx_data, fs);
        cur_sig_peak_idx = (int) ( cur_rx_data.size()/2 + cur_rx_data.size()*sine_freq/fs );
        // search for a small neighborhood for the peak power
        double cur_rx_sig_spec_peak = cur_rx_sig_spec.segment(cur_sig_peak_idx-5, 10).maxCoeff();
        // log RX signal power
        std::cout << boost::format("RX Signal Power: %f dBm (N_FFt = %d)")
            % cur_rx_sig_spec_peak % (int) cur_rx_data.size() << std::endl;

        // perfrom SI channel estimation
        cur_si_chnl_est = si_chnl_est(cur_tx_preamble, cur_rx_preamble, cur_l, cur_k);  // 200, 20

        // reconstruct SI signal from RX signal
        Eigen::MatrixXf A = sig_toeplitz(cur_tx_data, DIG_CANC_DATA_LEN, cur_k, 1); // 1024, 20
        cur_res_data = cur_rx_data.segment(DIG_CANC_CHNL_LEN, DIG_CANC_DATA_LEN) - A*cur_si_chnl_est;  // 1024
        // compute RX residual signal power after digital SIC
        cur_res_sig_spec = sig_power(cur_res_data, fs);
        cur_sig_peak_idx = (int) ( cur_res_data.size()/2 + cur_res_data.size()*sine_freq/fs );
        // search for a small neighborhood for the peak power
        double cur_res_sig_spec_peak = cur_res_sig_spec.segment(cur_sig_peak_idx-5, 10).maxCoeff();
        // log residual signal power
        std::cout << boost::format("RX Res Signal Power: %f dBm (N_fft = %d)")
            % cur_res_sig_spec_peak % (int) cur_res_data.size() << std::endl;

        std::cout << boost::format("Amount of Digital SIC: %f dB")
            % (cur_rx_sig_spec_peak - cur_res_sig_spec_peak) << std::endl << std::endl;

        // write data to files for offline processing and checking
        std::ofstream outfile_tx_preamble("tx_preamble.dat", ios::out | ios::binary | ios::trunc);
        outfile_tx_preamble.write((char*) cur_tx_preamble.data(), cur_tx_preamble.size()*sizeof(float) );  // *2 b/c of complex value
        outfile_tx_preamble.close();

        std::ofstream outfile_rx_preamble("rx_preamble.dat", ios::out | ios::binary | ios::trunc);
        outfile_rx_preamble.write((char*) cur_rx_preamble.data(), cur_rx_preamble.size()*sizeof(float) );  // *2 b/c of complex value
        outfile_rx_preamble.close();

        std::ofstream outfile_est_h("est_h.dat", ios::out | ios::binary | ios::trunc);
        outfile_est_h.write((char*) cur_si_chnl_est.data(), cur_si_chnl_est.size()*sizeof(float) );  // *2 b/c of complex value
        outfile_est_h.close();

        std::ofstream outfile_tx_data("tx_data.dat", ios::out | ios::binary | ios::trunc);
        outfile_tx_data.write((char*) cur_tx_data.data(), cur_tx_data.size()*sizeof(float) );  // *2 b/c of complex value
        outfile_tx_data.close();

        std::ofstream outfile_rx_data("rx_data.dat", ios::out | ios::binary | ios::trunc);
        outfile_rx_data.write((char*) cur_rx_data.data(), cur_tx_data.size()*sizeof(float) );  // *2 b/c of complex value
        outfile_rx_data.close();

        std::ofstream outfile_res_data("rx_data_res.dat", ios::out | ios::binary | ios::trunc);
        outfile_res_data.write((char*) cur_res_data.data(), cur_res_data.size()*sizeof(float) );  // *2 b/c of complex value
        outfile_res_data.close();

        mtx.unlock();

        sleep(fr);
    }
}

/***********************************************************************
* Main function
**********************************************************************/
int UHD_SAFE_MAIN(int argc, char *argv[]){

    uhd::set_thread_priority_safe();

    //transmit variables to be set by po
    std::string tx_args, wave_type, tx_ant, tx_subdev, ref, otw, tx_channels;
    double tx_rate, tx_freq, tx_gain, wave_freq, tx_bw;
    float ampl;

    //receive variables to be set by po
    std::string rx_args, file, type, rx_ant, rx_subdev, rx_channels;
    size_t total_num_samps, spb;
    double rx_rate, rx_freq, rx_gain, rx_bw;
    float settling;

    //setup the program options
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "help message")
    ("tx-args", po::value<std::string>(&tx_args)->default_value(""), "uhd transmit device address args")
    ("rx-args", po::value<std::string>(&rx_args)->default_value(""), "uhd receive device address args")
    ("file", po::value<std::string>(&file)->default_value("usrp_samples.dat"), "name of the file to write binary samples to")
    ("type", po::value<std::string>(&type)->default_value("float"), "sample type in file: double, float, or short") // default was "short"
    ("nsamps", po::value<size_t>(&total_num_samps)->default_value(0), "total number of samples to receive")
    ("settling", po::value<float>(&settling)->default_value(float(0.2)), "settling time (seconds) before receiving")
    ("spb", po::value<size_t>(&spb)->default_value(0), "samples per buffer, 0 for default")
    ("tx-rate", po::value<double>(&tx_rate), "rate of transmit outgoing samples")
    ("rx-rate", po::value<double>(&rx_rate), "rate of receive incoming samples")
    ("tx-freq", po::value<double>(&tx_freq), "transmit RF center frequency in Hz")
    ("rx-freq", po::value<double>(&rx_freq), "receive RF center frequency in Hz")
    ("ampl", po::value<float>(&ampl)->default_value(float(0.3)), "amplitude of the waveform [0 to 0.7]")
    ("tx-gain", po::value<double>(&tx_gain), "gain for the transmit RF chain")
    ("rx-gain", po::value<double>(&rx_gain), "gain for the receive RF chain")
    ("tx-ant", po::value<std::string>(&tx_ant), "transmit antenna selection")
    ("rx-ant", po::value<std::string>(&rx_ant), "receive antenna selection")
    ("tx-subdev", po::value<std::string>(&tx_subdev), "transmit subdevice specification")
    ("rx-subdev", po::value<std::string>(&rx_subdev), "receive subdevice specification")
    ("tx-bw", po::value<double>(&tx_bw), "analog transmit filter bandwidth in Hz")
    ("rx-bw", po::value<double>(&rx_bw), "analog receive filter bandwidth in Hz")
    ("wave-type", po::value<std::string>(&wave_type)->default_value("CONST"), "waveform type (CONST, SQUARE, RAMP, SINE)")
    ("wave-freq", po::value<double>(&wave_freq)->default_value(0), "waveform frequency in Hz")
    ("ref", po::value<std::string>(&ref)->default_value("internal"), "clock reference (internal, external, mimo)")
    ("otw", po::value<std::string>(&otw)->default_value("sc16"), "specify the over-the-wire sample mode")
    ("tx-channels", po::value<std::string>(&tx_channels)->default_value("0"), "which TX channel(s) to use (specify \"0\", \"1\", \"0,1\", etc)")
    ("rx-channels", po::value<std::string>(&rx_channels)->default_value("0"), "which RX channel(s) to use (specify \"0\", \"1\", \"0,1\", etc)")
    ("tx-int-n", "tune USRP TX with integer-N tuning")
    ("rx-int-n", "tune USRP RX with integer-N tuning")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help")){
        std::cout << std::endl;
        std::cout << boost::format("UHD Full-Duplex Simple Transceiver %s") % desc << std::endl;
        return ~0;
    }

    //create a usrp device
    std::cout << std::endl;
    std::cout << boost::format("Creating the transmit usrp device with: %s...") % tx_args << std::endl;
    uhd::usrp::multi_usrp::sptr tx_usrp = uhd::usrp::multi_usrp::make(tx_args);
    std::cout << std::endl;
    std::cout << boost::format("Creating the receive usrp device with: %s...") % rx_args << std::endl;
    uhd::usrp::multi_usrp::sptr rx_usrp = uhd::usrp::multi_usrp::make(rx_args);

    //detect which channels to use
    std::vector<std::string> tx_channel_strings;
    std::vector<size_t> tx_channel_nums;
    boost::split(tx_channel_strings, tx_channels, boost::is_any_of("\"',"));
    for(size_t ch = 0; ch < tx_channel_strings.size(); ch++){
        size_t chan = boost::lexical_cast<int>(tx_channel_strings[ch]);
        if(chan >= tx_usrp->get_tx_num_channels()){
            throw std::runtime_error("Invalid TX channel(s) specified.");
        }
        else tx_channel_nums.push_back(boost::lexical_cast<int>(tx_channel_strings[ch]));
    }
    std::vector<std::string> rx_channel_strings;
    std::vector<size_t> rx_channel_nums;
    boost::split(rx_channel_strings, rx_channels, boost::is_any_of("\"',"));
    for(size_t ch = 0; ch < rx_channel_strings.size(); ch++){
        size_t chan = boost::lexical_cast<int>(rx_channel_strings[ch]);
        if(chan >= rx_usrp->get_rx_num_channels()){
            throw std::runtime_error("Invalid RX channel(s) specified.");
        }
        else rx_channel_nums.push_back(boost::lexical_cast<int>(rx_channel_strings[ch]));
    }

    //Lock mboard clocks
    tx_usrp->set_clock_source(ref);
    rx_usrp->set_clock_source(ref);

    //always select the subdevice first, the channel mapping affects the other settings
    if (vm.count("tx-subdev")) tx_usrp->set_tx_subdev_spec(tx_subdev);
    if (vm.count("rx-subdev")) rx_usrp->set_rx_subdev_spec(rx_subdev);

    std::cout << boost::format("Using TX Device: %s") % tx_usrp->get_pp_string() << std::endl;
    std::cout << boost::format("Using RX Device: %s") % rx_usrp->get_pp_string() << std::endl;

    //set the transmit sample rate
    if (not vm.count("tx-rate")){
        std::cerr << "Please specify the transmit sample rate with --tx-rate" << std::endl;
        return ~0;
    }
    std::cout << boost::format("Setting TX Rate: %f Msps...") % (tx_rate/1e6) << std::endl;
    tx_usrp->set_tx_rate(tx_rate);
    std::cout << boost::format("Actual TX Rate: %f Msps...") % (tx_usrp->get_tx_rate()/1e6) << std::endl << std::endl;

    //set the receive sample rate
    if (not vm.count("rx-rate")){
        std::cerr << "Please specify the sample rate with --rx-rate" << std::endl;
        return ~0;
    }
    std::cout << boost::format("Setting RX Rate: %f Msps...") % (rx_rate/1e6) << std::endl;
    rx_usrp->set_rx_rate(rx_rate);
    std::cout << boost::format("Actual RX Rate: %f Msps...") % (rx_usrp->get_rx_rate()/1e6) << std::endl << std::endl;

    //set the transmit center frequency
    if (not vm.count("tx-freq")){
        std::cerr << "Please specify the transmit center frequency with --tx-freq" << std::endl;
        return ~0;
    }

    for(size_t ch = 0; ch < tx_channel_nums.size(); ch++) {
        size_t channel = tx_channel_nums[ch];
        if (tx_channel_nums.size() > 1) {
            std::cout << "Configuring TX Channel " << channel << std::endl;
        }
        std::cout << boost::format("Setting TX Freq: %f MHz...") % (tx_freq/1e6) << std::endl;
        uhd::tune_request_t tx_tune_request(tx_freq);
        if(vm.count("tx-int-n")) tx_tune_request.args = uhd::device_addr_t("mode_n=integer");
        tx_usrp->set_tx_freq(tx_tune_request, channel);
        std::cout << boost::format("Actual TX Freq: %f MHz...") % (tx_usrp->get_tx_freq(channel)/1e6) << std::endl << std::endl;

        //set the rf gain
        if (vm.count("tx-gain")){
            std::cout << boost::format("Setting TX Gain: %f dB...") % tx_gain << std::endl;
            tx_usrp->set_tx_gain(tx_gain, channel);
            std::cout << boost::format("Actual TX Gain: %f dB...") % tx_usrp->get_tx_gain(channel) << std::endl << std::endl;
        }

        //set the analog frontend filter bandwidth
        if (vm.count("tx-bw")){
            std::cout << boost::format("Setting TX Bandwidth: %f MHz...") % tx_bw << std::endl;
            tx_usrp->set_tx_bandwidth(tx_bw, channel);
            std::cout << boost::format("Actual TX Bandwidth: %f MHz...") % tx_usrp->get_tx_bandwidth(channel) << std::endl << std::endl;
        }

        //set the antenna
        if (vm.count("tx-ant")) tx_usrp->set_tx_antenna(tx_ant, channel);
    }

    for(size_t ch = 0; ch < rx_channel_nums.size(); ch++) {
        size_t channel = rx_channel_nums[ch];
        if (rx_channel_nums.size() > 1) {
            std::cout << "Configuring RX Channel " << channel << std::endl;
        }

        //set the receive center frequency
        if (not vm.count("rx-freq")){
            std::cerr << "Please specify the center frequency with --rx-freq" << std::endl;
            return ~0;
        }
        std::cout << boost::format("Setting RX Freq: %f MHz...") % (rx_freq/1e6) << std::endl;
        uhd::tune_request_t rx_tune_request(rx_freq);
        if(vm.count("rx-int-n")) rx_tune_request.args = uhd::device_addr_t("mode_n=integer");
        rx_usrp->set_rx_freq(rx_tune_request, channel);
        std::cout << boost::format("Actual RX Freq: %f MHz...") % (rx_usrp->get_rx_freq(channel)/1e6) << std::endl << std::endl;

        //set the receive rf gain
        if (vm.count("rx-gain")){
            std::cout << boost::format("Setting RX Gain: %f dB...") % rx_gain << std::endl;
            rx_usrp->set_rx_gain(rx_gain, channel);
            std::cout << boost::format("Actual RX Gain: %f dB...") % rx_usrp->get_rx_gain(channel) << std::endl << std::endl;
        }

        //set the receive analog frontend filter bandwidth
        if (vm.count("rx-bw")){
            std::cout << boost::format("Setting RX Bandwidth: %f MHz...") % (rx_bw/1e6) << std::endl;
            rx_usrp->set_rx_bandwidth(rx_bw, channel);
            std::cout << boost::format("Actual RX Bandwidth: %f MHz...") % (rx_usrp->get_rx_bandwidth(channel)/1e6) << std::endl << std::endl;
        }
    }
    //set the receive antenna
    if (vm.count("ant")) rx_usrp->set_rx_antenna(rx_ant);

    //for the const wave, set the wave freq for small samples per period
    if (wave_freq == 0 and wave_type == "CONST"){
        wave_freq = tx_usrp->get_tx_rate()/2;
    }

    //error when the waveform is not possible to generate
    if (std::abs(wave_freq) > tx_usrp->get_tx_rate()/2){
        throw std::runtime_error("wave freq out of Nyquist zone");
    }
    if (tx_usrp->get_tx_rate()/std::abs(wave_freq) > wave_table_len/2){
        throw std::runtime_error("wave freq too small for table");
    }

    //pre-compute the waveform values
    const wave_table_class wave_table(wave_type, ampl);
    const size_t step = boost::math::iround(wave_freq/tx_usrp->get_tx_rate() * wave_table_len);
    size_t index = 0;

    //create a transmit streamer
    //linearly map channels (index0 = channel0, index1 = channel1, ...)
    uhd::stream_args_t stream_args("fc32", otw);
    stream_args.channels = tx_channel_nums;
    uhd::tx_streamer::sptr tx_stream = tx_usrp->get_tx_stream(stream_args);

    // rx_streamer will be set up in the receive_worker
    // uhd::rx_streamer::sptr rx_stream = rx_usrp->get_rx_stream(stream_args);

    //allocate a buffer which we re-use for each channel
    if (spb == 0) spb = tx_stream->get_max_num_samps()*10;
    std::vector<std::complex<float> > buff(spb);
    int num_channels = tx_channel_nums.size();

    // init global variables
    gv_tbuff = Eigen::VectorXcf::Zero(spb);
    gv_rbuff = Eigen::VectorXcf::Zero(spb);

    //reset usrp time to prepare for transmit/receive
    std::cout << boost::format("Setting device timestamp to 0...") << std::endl;
    tx_usrp->set_time_now(uhd::time_spec_t(0.0));

    const uhd::time_spec_t start_tx_stream_time = tx_usrp->get_time_now() + uhd::time_spec_t(0.1);

    //setup the metadata flags
    uhd::tx_metadata_t md;
    md.start_of_burst = true;
    md.end_of_burst   = false;
    md.has_time_spec  = true;
    md.time_spec = start_tx_stream_time; // uhd::time_spec_t(0.1); //give us 0.1 seconds to fill the tx buffers

    //Check Ref and LO Lock detect
    std::vector<std::string> tx_sensor_names, rx_sensor_names;
    tx_sensor_names = tx_usrp->get_tx_sensor_names(0);
    if (std::find(tx_sensor_names.begin(), tx_sensor_names.end(), "lo_locked") != tx_sensor_names.end()) {
        uhd::sensor_value_t lo_locked = tx_usrp->get_tx_sensor("lo_locked",0);
        std::cout << boost::format("Checking TX: %s ...") % lo_locked.to_pp_string() << std::endl;
        UHD_ASSERT_THROW(lo_locked.to_bool());
    }
    rx_sensor_names = rx_usrp->get_rx_sensor_names(0);
    if (std::find(rx_sensor_names.begin(), rx_sensor_names.end(), "lo_locked") != rx_sensor_names.end()) {
        uhd::sensor_value_t lo_locked = rx_usrp->get_rx_sensor("lo_locked",0);
        std::cout << boost::format("Checking RX: %s ...") % lo_locked.to_pp_string() << std::endl;
        UHD_ASSERT_THROW(lo_locked.to_bool());
    }

    tx_sensor_names = tx_usrp->get_mboard_sensor_names(0);
    if ((ref == "mimo") and (std::find(tx_sensor_names.begin(), tx_sensor_names.end(), "mimo_locked") != tx_sensor_names.end())) {
        uhd::sensor_value_t mimo_locked = tx_usrp->get_mboard_sensor("mimo_locked",0);
        std::cout << boost::format("Checking TX: %s ...") % mimo_locked.to_pp_string() << std::endl;
        UHD_ASSERT_THROW(mimo_locked.to_bool());
    }
    if ((ref == "external") and (std::find(tx_sensor_names.begin(), tx_sensor_names.end(), "ref_locked") != tx_sensor_names.end())) {
        uhd::sensor_value_t ref_locked = tx_usrp->get_mboard_sensor("ref_locked",0);
        std::cout << boost::format("Checking TX: %s ...") % ref_locked.to_pp_string() << std::endl;
        UHD_ASSERT_THROW(ref_locked.to_bool());
    }

    rx_sensor_names = rx_usrp->get_mboard_sensor_names(0);
    if ((ref == "mimo") and (std::find(rx_sensor_names.begin(), rx_sensor_names.end(), "mimo_locked") != rx_sensor_names.end())) {
        uhd::sensor_value_t mimo_locked = rx_usrp->get_mboard_sensor("mimo_locked",0);
        std::cout << boost::format("Checking RX: %s ...") % mimo_locked.to_pp_string() << std::endl;
        UHD_ASSERT_THROW(mimo_locked.to_bool());
    }
    if ((ref == "external") and (std::find(rx_sensor_names.begin(), rx_sensor_names.end(), "ref_locked") != rx_sensor_names.end())) {
        uhd::sensor_value_t ref_locked = rx_usrp->get_mboard_sensor("ref_locked",0);
        std::cout << boost::format("Checking RX: %s ...") % ref_locked.to_pp_string() << std::endl;
        UHD_ASSERT_THROW(ref_locked.to_bool());
    }

    if (total_num_samps == 0){
        std::signal(SIGINT, &sig_int_handler);
        std::cout << "Press Ctrl + C to stop streaming..." << std::endl;
    }

    //reset usrp time to prepare for transmit/receive
    // std::cout << boost::format("Setting device timestamp to 0...") << std::endl;
    // tx_usrp->set_time_now(uhd::time_spec_t(0.0));

    //start transmit worker thread
    boost::thread_group main_thread;
    main_thread.create_thread(boost::bind(&transmit_worker, buff, wave_table, tx_stream, md, step, index, num_channels));

    main_thread.create_thread(boost::bind(&receive_worker, rx_usrp, stream_args, spb, total_num_samps, settling, rx_channel_nums, start_tx_stream_time));

    main_thread.create_thread(boost::bind(&dig_SIC_worker, (double) wave_freq, (double) tx_usrp->get_tx_rate(), 1));

    std::cout << boost::format(
        "TX Stream time was: %u full secs, %f frac secs"
    ) % start_tx_stream_time.get_full_secs() % start_tx_stream_time.get_frac_secs() << std::endl;

    signal(SIGINT, my_handler);

    // catch exit signal from keyboard using Ctrl + c
    while(!main_stop_signal) {
        // mtx.lock();
        // std::cout << boost::format("TX buff idx: %d, RX buff idx: %d") % gv_tbuff_idx % gv_rbuff_idx << std::endl;
        // std::cout << boost::format("RX tone peak power %f dBm") % gv_rx_sig_spec.maxCoeff() << std::endl;
        // std::cout << boost::format("Res. tone peak power %f dBm") % gv_res_sig_spec.maxCoeff() << std::endl << std::endl;
        // mtx.unlock();
        // sleep(1);
    }

//clean up all threads
stop_signal_called = true;
main_thread.join_all();

//finished
std::cout << std::endl << "Done!" << std::endl << std::endl;
return EXIT_SUCCESS;
}
