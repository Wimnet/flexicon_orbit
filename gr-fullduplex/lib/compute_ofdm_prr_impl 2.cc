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
#include "compute_ofdm_prr_impl.h"

namespace gr {
  namespace fullduplex {

    compute_ofdm_prr::sptr
    compute_ofdm_prr::make(bool debug, int packet_len)
    {
      return gnuradio::get_initial_sptr
        (new compute_ofdm_prr_impl(debug, packet_len));
    }

    /*
     * The private constructor
     */
    compute_ofdm_prr_impl::compute_ofdm_prr_impl(bool debug, int packet_len)
      : gr::tagged_stream_block("compute_ofdm_prr",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make2(2, 2, sizeof(char), sizeof(char)), "packet_len"),
              d_debug(debug),
              d_packet_len(packet_len)
    {}

    /*
     * Our virtual destructor.
     */
    compute_ofdm_prr_impl::~compute_ofdm_prr_impl()
    {
    }

    int
    compute_ofdm_prr_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int noutput_items = 1;
      return noutput_items ;
    }

    int
    compute_ofdm_prr_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const byte *in_tx = (const char*) input_items[0];
      const byte *in_rx = (const char*) input_items[1];
      byte *out = (char *) output_items[0];

      int match = 1;

      for (idx = 0; idx < d_packet_len; idx++)
      {
        if (in_tx[idx] != in_rx[idx])
        {
          match = 0;
        }
      }

      if (match)
      {

      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace fullduplex */
} /* namespace gr */

