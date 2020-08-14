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

#ifndef INCLUDED_FULLDUPLEX_COMPUTE_OFDM_PRR_IMPL_H
#define INCLUDED_FULLDUPLEX_COMPUTE_OFDM_PRR_IMPL_H

#include <fullduplex/compute_ofdm_prr.h>

namespace gr {
  namespace fullduplex {

    class compute_ofdm_prr_impl : public compute_ofdm_prr
    {
     private:
      // Nothing to declare in this block.
      bool d_debug;
      int d_packet_len;
      int d_decoded_count;
      int d_transmitted_count;

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      compute_ofdm_prr_impl(bool debug);
      ~compute_ofdm_prr_impl();

      // Where all the action really happens
      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_COMPUTE_OFDM_PRR_IMPL_H */

