/* -*- c++ -*- */
/*
 * Copyright 2021 gr-fullduplex author.
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

#ifndef INCLUDED_FULLDUPLEX_ALOHA_CONTROL_IMPL_H
#define INCLUDED_FULLDUPLEX_ALOHA_CONTROL_IMPL_H

#include <fullduplex/aloha_control.h>

namespace gr {
  namespace fullduplex {

    class aloha_control_impl : public aloha_control
    {
     private:
      // Nothing to declare in this block.
      float d_trans_prob;
      bool d_debug;
      unsigned int d_pad_front;
      unsigned int d_pad_tail;
      int d_offset;
      unsigned int d_sig_pilot_pos;
      int d_sig_pilot_len;
      double d_sig_pilot_scale;
      double d_premultiplier;
      int d_sync_word;
      int d_packets_sent;
      int d_radio;
      bool d_record;
      bool d_record_latch;

      std::ofstream d_fstream;

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      aloha_control_impl(float trans_prob, unsigned int radio, unsigned int pad_front, unsigned int pad_tail, int offset, unsigned int sig_pilot_pos, double premultiplier, int sync_word, bool debug, bool record);
      ~aloha_control_impl();

      void set_record(bool record) { d_record = record; }
      void set_offset(int offset) { d_offset = offset; }

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_int &ninput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_ALOHA_CONTROL_IMPL_H */

