/* -*- c++ -*- */
/*
 * Copyright 2019 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_FULLDUPLEX_PACKET_DECAP_IMPL_H
#define INCLUDED_FULLDUPLEX_PACKET_DECAP_IMPL_H

#include <fullduplex/packet_decap.h>

#include <gnuradio/fft/fft.h>

namespace gr {
  namespace fullduplex {

    class packet_decap_impl : public packet_decap
    {
     private:
      // Nothing to declare in this block.
      bool d_debug;
      int d_sig_pilot_pos;
      int d_pad_front;
      int d_delay_tx2rx;
      int d_frame_len;

      // fft
      // bool d_shift;
      // fft::fft_complex *d_fft;

     public:
      packet_decap_impl(bool debug,
                        unsigned int sig_pilot_pos,
                        unsigned int pad_front,
                        unsigned int delay_tx2rx,
                        unsigned int frame_len,
                        const std::string& len_tag_key);
      ~packet_decap_impl();

      // Where all the action really happens
      // void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
      /*
      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
      */

      int d_pad;
      // int d_sig_pilot;
      int d_sig_pilot_len;
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_PACKET_DECAP_IMPL_H */
