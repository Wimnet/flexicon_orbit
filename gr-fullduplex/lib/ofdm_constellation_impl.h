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

#ifndef INCLUDED_FULLDUPLEX_OFDM_CONSTELLATION_IMPL_H
#define INCLUDED_FULLDUPLEX_OFDM_CONSTELLATION_IMPL_H

#include <fullduplex/ofdm_constellation.h>
#include <Eigen/Dense>
#include <fftw3.h>
#include <cmath>
#include <gnuradio/expj.h>
#include <gnuradio/math.h>

namespace gr {
  namespace fullduplex {

    class ofdm_constellation_impl : public ofdm_constellation
    {
     private:
      // variables
      int d_delay_tx2rx;
      int d_pad_front;
      int d_packet_len;
      int d_symbol_len;
      int d_CP_len;
      int d_FFT_len;
      int d_n_data_sbcrs;
      float d_qpsk_mag;
      int d_num_symbols;
      bool d_debug;
      std::vector<gr_complex> d_pilot_symbols;
      int d_pilot_subcarriers[4] = {11, 25, 39, 53};
      int d_data_subcarriers[48] = {6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26, 27, 28, 29, 30, 31, 33, 34, 35, 36, 37, 38, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 54, 55, 56, 57, 58};

      // mutex
      gr::thread::mutex d_mutex;

      // FFT things
      fftw_complex* d_fft_in;
      fftw_complex* d_fft_out;
      fftw_plan d_fft_plan;

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);
      void set_delay_tx2rx(int delay_tx2rx);
      float average_mag(gr_complex* symbols, int n);
      void normalize_samples(gr_complex* rx_symbols);
      float calculate_evm(const gr_complex* rx_symbols);

     public:
      ofdm_constellation_impl(bool debug, int delay_tx2rx, int pad_front, int packet_len);
      ~ofdm_constellation_impl();

      // Where all the action really happens
      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_OFDM_CONSTELLATION_IMPL_H */

