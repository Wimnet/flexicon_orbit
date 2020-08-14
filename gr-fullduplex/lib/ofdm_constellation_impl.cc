/* -*- c++ -*- */
/* 
 * Copyright 2020 gr-fullduplex author.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "ofdm_constellation_impl.h"

namespace gr {
  namespace fullduplex {

    ofdm_constellation::sptr
    ofdm_constellation::make(bool debug, int delay_tx2rx, int pad_front, int packet_len)
    {
      return gnuradio::get_initial_sptr
        (new ofdm_constellation_impl(debug, delay_tx2rx, pad_front, packet_len));
    }

    /*
     * The private constructor
     */
    ofdm_constellation_impl::ofdm_constellation_impl(bool debug, int delay_tx2rx, int pad_front, int packet_len)
      : gr::tagged_stream_block("ofdm_constellation",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(float)), "packet_len"),
              d_debug(debug),
              d_delay_tx2rx(delay_tx2rx),
              d_pad_front(pad_front),
              d_packet_len(packet_len),
              d_symbol_len(80),
              d_CP_len(16),
              d_FFT_len(64),
              d_n_data_sbcrs(48),
              d_qpsk_mag(std::sqrt(2))
    {
    	// setup the pilot symbols (known)
    	// TODO: include these as input
    	d_pilot_symbols.resize(4);
    	d_pilot_symbols[0] = gr_complex(1.0,0.0);
    	d_pilot_symbols[1] = gr_complex(1.0,0.0);
    	d_pilot_symbols[2] = gr_complex(1.0,0.0);
    	d_pilot_symbols[3] = gr_complex(-1.0,0.0);

    	// setup buffers for FFT and other FFT things
    	d_fft_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * d_FFT_len);
		  d_fft_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * d_FFT_len);
		  d_fft_plan = fftw_plan_dft_1d(d_FFT_len, d_fft_in, d_fft_out, FFTW_FORWARD, FFTW_MEASURE);

      int real_packet_len = d_packet_len - 320; // correct for "bug" of larger than expected packet size
      real_packet_len = real_packet_len - 4*d_symbol_len; // subtract pilot symbols
      d_num_symbols = int(real_packet_len/d_symbol_len); // compute real number of symbols
    }

    /*
     * Our virtual destructor.
     */
    ofdm_constellation_impl::~ofdm_constellation_impl()
    {
    	free(d_fft_in);
    	free(d_fft_out);
    	fftw_destroy_plan(d_fft_plan);
    }

    void
    ofdm_constellation_impl::set_delay_tx2rx(int delay_tx2rx)
    {
  		gr::thread::scoped_lock lock(d_mutex);
  		d_delay_tx2rx = delay_tx2rx;
    }

    int
    ofdm_constellation_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
		  int noutput_items = d_n_data_sbcrs * (d_num_symbols - 3); // subtract S&C sync words and sig
		  return noutput_items;
    }

    /*
     * Compute the average magnitude of a complex array
     */
    float
    ofdm_constellation_impl::average_mag(gr_complex* symbols, int n)
    {
    	float total = 0.0;
    	for (int idx = 0; idx < n; idx++)
    	{
    		gr_complex sym = symbols[idx];
    		float mag = std::sqrt(std::pow(sym.real(), 2) + std::pow(sym.imag(), 2));
    		total = total + mag;
    	}
    	return total / (float)n;
    }

    /*
     * Normalize the corrected QPSK samples to the normalized QPSK amplitude
     */
    void
    ofdm_constellation_impl::normalize_samples(gr_complex* rx_symbols)
    {
    	int n = d_n_data_sbcrs * (d_num_symbols - 3); // number of samples in one packet
    	float avg_mag = average_mag(rx_symbols, n);
    	float norm_factor = d_qpsk_mag / avg_mag;
    	for (int idx = 0; idx < n; idx++)
    	{
    		rx_symbols[idx] = norm_factor * rx_symbols[idx];
    	}
    }

    /*
     * Compute the EVM of a packet
     * For QPSK, we can find the error by taking the absolute value of I and Q and comparing to 1
     */
    float
    ofdm_constellation_impl::calculate_evm(const gr_complex* normalized_symbols)
    {
    	int n = d_n_data_sbcrs * (d_num_symbols - 3);
    	float I_err = 0.0;
    	float Q_err = 0.0;
    	for (int idx = 0; idx < n; idx++)
    	{
    		gr_complex sym = normalized_symbols[idx];
    		I_err += std::pow(std::fabs(sym.real()) - 1, 2);
    		Q_err += std::pow(std::fabs(sym.imag()) - 1, 2);
    	}
    	float evm = std::sqrt((1.0/float(n))*(I_err + Q_err))/d_qpsk_mag;
    	return evm * 100.0;
    }

    int
    ofdm_constellation_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
		const gr_complex* in = (const gr_complex*) input_items[0];
		gr_complex* out_symbols = (gr_complex*) output_items[0];
    float* out_evm = (float*) output_items[1];

		noutput_items = calculate_output_stream_length(ninput_items);

		// initialize memory for the output symbols
		std::memset(out_symbols, 0, sizeof(gr_complex) * noutput_items);

		// initialize vector b for storing pilot symbol phase
		Eigen::VectorXf pilot_phase_offsets(4);

		// initialize vector A for computing phase correction as a linear 
		// function of the symbol index
		// the 1s are for computing the phase x=0 intercept ( y=mx+c )
		Eigen::MatrixXf pilot_idx_mat(4,2);
		pilot_idx_mat << 1, d_pilot_subcarriers[0], 
						 1, d_pilot_subcarriers[1], 
						 1, d_pilot_subcarriers[2], 
						 1, d_pilot_subcarriers[3];
		
		// collect rx packet. four symbols of FD pilots, six symbols of packet
		gr_complex* rx_pkt = (gr_complex*)std::malloc(d_num_symbols*d_symbol_len*sizeof(gr_complex));
		int rx_pkt_start = d_pad_front + 4*d_symbol_len + d_delay_tx2rx;
		std::memcpy(rx_pkt, in + rx_pkt_start, d_num_symbols*d_symbol_len*sizeof(gr_complex));

		// for each of the three data symbols, correct the phase and get the symbos
		for(int sym = 0; sym < (d_num_symbols - 3); sym++) 
		{
			// copy symbol without CP
			// fftw_complex test = {0.0, 0.0};
			std::vector<gr_complex> symbol(rx_pkt + (sym+3)*d_symbol_len + d_CP_len, rx_pkt+(sym+4)*d_symbol_len);
			// convert from gr_complex (std::complex) to fftw_complex
			for (int idx = 0; idx < d_FFT_len; idx++)
			{
				d_fft_in[idx][0] = symbol[idx].real();
				d_fft_in[idx][1] = symbol[idx].imag();
			}
			
			// perform the FFT using FFTW3 library. The result will NOT be shifted, meaning this is 
			// equivalent to MATLAB fft(x). We could either shift, or index differently
			std::vector<gr_complex> symbol_fft_shifted(64);
			fftw_execute(d_fft_plan);

			// copy the FFT into a gr_complex vector so that it is shifted and the result
			// is equivalent to MATLAB fftshift(fft(x))
			int N_fft_halved = (div(d_FFT_len,2)).quot; // precompute, not sure if optimizer will get this
			for (int idx = 0; idx < d_FFT_len; idx++) 
			{
				int idx2 = (idx+N_fft_halved) % d_FFT_len;
				fftw_complex cplx = {d_fft_out[idx2][0], d_fft_out[idx2][1]};
				symbol_fft_shifted[idx] = gr_complex(cplx[0],cplx[1]);
			}

			// std::copy(symbol.begin(), symbol.end(), symbol_fft_shifted.begin());

		    // extract pilot symbols
		    std::vector<gr_complex> sym_pilots(4);
		    for (int idx = 0; idx < 4; idx++) 
		    {
		    	sym_pilots[idx] = symbol_fft_shifted[d_pilot_subcarriers[idx]];
		    }

		    // compute phase offsets using pilot symbols and unwrap phase 
		    float prev_phase = 0;
		    for (int idx = 0; idx < 4; idx++)
		    {
		    	pilot_phase_offsets(idx) = fast_atan2f(sym_pilots[idx]) - fast_atan2f(d_pilot_symbols[idx]);
		    	float delta_phi = pilot_phase_offsets(idx) - prev_phase; 
		    	if (delta_phi > M_PI && idx > 0)
		    	{
		    		delta_phi -= 2 * M_PI;
		    	}
		    	if (delta_phi < -M_PI && idx > 0)
		    	{
		    		delta_phi += 2 * M_PI;
		    	}
		    	pilot_phase_offsets(idx) = prev_phase + delta_phi;
		    	prev_phase = pilot_phase_offsets(idx);
		    }

		    // compute best fit for phase offset
		    // this best fit takes as input the data subcarrier index, which has base index 0.
		    Eigen::BDCSVD <Eigen::MatrixXf> svd(pilot_idx_mat, Eigen::ComputeThinU | Eigen::ComputeThinV);
		    Eigen::VectorXf phase_fit = svd.solve(pilot_phase_offsets);

		    if (d_debug)
		    {
		    	std::cout << phase_fit << std::endl;
		    }

		    // phase correct data symbols and clock into output vector
		    for (int idx = 0; idx < d_n_data_sbcrs; idx++) 
		    {
		    	gr_complex data_symbol = symbol_fft_shifted[d_data_subcarriers[idx]];
		    	float phase_correction = phase_fit(0) + (float)d_data_subcarriers[idx]*phase_fit(1);
		    	out_symbols[idx + sym*d_n_data_sbcrs] = data_symbol*gr_expj(-phase_correction);
		    }
		}

		free(rx_pkt);

		normalize_samples(out_symbols);

		float evm = calculate_evm(out_symbols);

    for (int idx = 0; idx < 8; idx++)
    {
      out_evm[idx] = evm;
    }

		// Tell runtime system how many output items we produced.
		return noutput_items;
    }

  } /* namespace fullduplex */
} /* namespace gr */

