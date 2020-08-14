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

#ifndef INCLUDED_FULLDUPLEX_SNR_CALC_IMPL_H
#define INCLUDED_FULLDUPLEX_SNR_CALC_IMPL_H

#include <fullduplex/snr_calc.h>
#include <Eigen/Dense>
#include <fftw3.h>
#include <cmath>
#include <sstream>
#include <string>
#include <gnuradio/expj.h>
#include <gnuradio/math.h>

namespace gr {
  namespace fullduplex {

    class snr_calc_impl : public snr_calc
    {
     private:
      bool d_debug;
      int d_delay_tx2rx;
      int d_pad_front;
      int d_data_symbols;
      int d_symbol_len;
      int d_CP_len;
      int d_noise_start_idx;
      int d_noise_length;

      // buffers
      fftw_complex* d_noise_t;
      fftw_complex* d_noise_f;
      fftw_complex* d_pkt_t;
      fftw_complex* d_pkt_f;

      // fft plan
      fftw_plan d_noise_p;
      fftw_plan d_pkt_p;

      // mutex
      gr::thread::mutex d_mutex;

      // debug stream
      std::stringstream d_debug_stream;

     protected:
      void debug_print();
      int calculate_output_stream_length(const gr_vector_int &ninput_items);
      void set_delay_tx2rx(int delay_tx2rx);
      float compute_fft_power(const fftw_complex* fft, int fft_len);

     public:
      snr_calc_impl(bool debug, int delay_txrx, int pad_front, int data_symbols, int noise_start_idx, int noise_length);
      ~snr_calc_impl();

      // Where all the action really happens
      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_SNR_CALC_IMPL_H */

