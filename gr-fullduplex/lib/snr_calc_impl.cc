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
#include "snr_calc_impl.h"

namespace gr {
  namespace fullduplex {

    snr_calc::sptr
    snr_calc::make(bool debug, int delay_txrx, int pad_front, int data_symbols, int noise_start_idx, int noise_length)
    {
      return gnuradio::get_initial_sptr
        (new snr_calc_impl(debug, delay_txrx, pad_front, data_symbols, noise_start_idx, noise_length));
    }

    /*
     * The private constructor
     */
    snr_calc_impl::snr_calc_impl(bool debug, int delay_txrx, int pad_front, int data_symbols, int noise_start_idx, int noise_length)
      : gr::tagged_stream_block("snr_calc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(float)), "packet_len"),
              d_debug(debug),
              d_delay_tx2rx(delay_txrx),
              d_pad_front(pad_front),
              d_data_symbols(data_symbols),
              d_noise_start_idx(noise_start_idx),
              d_noise_length(noise_length),
              d_symbol_len(80),
              d_CP_len(16)
    {
        // get a noise sample and the data part of the packet (ignore FD symbols)
        // setup buffers
        d_debug_stream << "Build buffers" << std::endl; debug_print();
        int N = d_data_symbols * d_symbol_len;
        std::cout << d_data_symbols << " * " << d_symbol_len << " = " << N << std::endl;
        d_noise_t = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * d_noise_length);
        d_pkt_t = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
        d_noise_f = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * d_noise_length);
        d_pkt_f = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

        d_debug_stream << "Setup Noise FFT" << std::endl; debug_print();
        d_noise_p = fftw_plan_dft_1d(d_noise_length, d_noise_t, d_noise_f, FFTW_FORWARD, FFTW_MEASURE);
        d_debug_stream << "Setup packet FFT" << std::endl; debug_print();
        d_pkt_p = fftw_plan_dft_1d(N, d_pkt_t, d_pkt_f, FFTW_FORWARD, FFTW_MEASURE);
    }

    /*
     * Our virtual destructor.
     */
    snr_calc_impl::~snr_calc_impl()
    {
        d_debug_stream << "Free buffers" << std::endl; debug_print();
        free(d_noise_t);
        free(d_pkt_t);
        free(d_noise_f);
        free(d_pkt_f);
        fftw_destroy_plan(d_noise_p);
        fftw_destroy_plan(d_pkt_p);
    }

    void
    snr_calc_impl::debug_print()
    {
        if (d_debug)
        {
            std::cout << d_debug_stream.str();
            d_debug_stream.str(std::string());
            d_debug_stream.clear();
        }
    }

    void
    snr_calc_impl::set_delay_tx2rx(int delay_tx2rx)
    {
        gr::thread::scoped_lock lock(d_mutex);
        d_delay_tx2rx = delay_tx2rx;
    }

    int
    snr_calc_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
        int noutput_items = 1;
        return noutput_items ;
    }

    float
    snr_calc_impl::compute_fft_power(const fftw_complex* fft, int fft_len)
    {
        float pow_sum = 0.0;
        for (int idx = 0; idx < fft_len; idx++)
        {
            float pow = std::sqrt(std::pow(fft[idx][0],2) + std::pow(fft[idx][1],2));
            pow_sum += pow;
        }
        return 20.0*(std::log10(pow_sum/fft_len));
    }

    int
    snr_calc_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex* in = (const gr_complex*) input_items[0];
        float* out = (float*) output_items[0];

        // our output is a single floating point value which is the SNR
        // of the current received packet.
        noutput_items = calculate_output_stream_length(ninput_items);

        // copy from the input stream into the noise and packet buffers
        // noise (just take 480 samples in the middle of the padding)
        d_debug_stream << "Fill buffers" << std::endl; debug_print();
        int start_idx = d_noise_start_idx;
        int end_idx = start_idx + d_noise_length;
        for (int idx = start_idx; idx < end_idx; idx++)
        {
            int buff_idx = idx - start_idx;
            d_noise_t[buff_idx][0] = in[idx].real();
            d_noise_t[buff_idx][1] = in[idx].imag();
        }
        // packet
        start_idx = d_pad_front + 4*d_symbol_len + d_delay_tx2rx;
        end_idx = start_idx + d_data_symbols*d_symbol_len;
        for (int idx = start_idx; idx < end_idx; idx++)
        {
            int buff_idx = idx - start_idx;
            d_pkt_t[buff_idx][0] = in[idx].real();
            d_pkt_t[buff_idx][1] = in[idx].imag();
        }

        // take fft of both
        d_debug_stream << "Take Noise FFT" << std::endl; debug_print();
        fftw_execute(d_noise_p);
        d_debug_stream << "Take packet FFT" << std::endl; debug_print();
        fftw_execute(d_pkt_p);

        d_debug_stream << "compute power" << std::endl; debug_print();
        // compute the power of both
        float noise_pow = compute_fft_power(d_noise_f, d_noise_length);
        float pkt_pow = compute_fft_power(d_pkt_f, d_data_symbols*d_symbol_len);

        d_debug_stream << "Finish" << std::endl; debug_print();
        
        // place SNR into output buffer
        float SNR = pkt_pow - noise_pow;

        d_debug_stream << "Packet power: " << pkt_pow << ", Noise power" << noise_pow << ", SNR:" << SNR << std::endl; debug_print();

        out[0] = pkt_pow - noise_pow;

        // Done, tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace fullduplex */
} /* namespace gr */

