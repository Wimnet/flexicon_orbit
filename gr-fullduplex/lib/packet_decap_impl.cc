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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "packet_decap_impl.h"

#include <volk/volk.h>

namespace gr {
  namespace fullduplex {

    packet_decap::sptr
    packet_decap::make(bool debug,
                       unsigned int sig_pilot_pos,
                       unsigned int pad_front,
                       unsigned int delay_tx2rx,
                       unsigned int frame_len,
                       const std::string& len_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new packet_decap_impl(debug, sig_pilot_pos, pad_front, delay_tx2rx, frame_len, len_tag_key));
    }

    /*
     * The private constructor
     */
    packet_decap_impl::packet_decap_impl(bool debug,
                                         unsigned int sig_pilot_pos,
                                         unsigned int pad_front,
                                         unsigned int delay_tx2rx,
                                         unsigned int frame_len,
                                         const std::string& len_tag_key)
      : gr::tagged_stream_block("packet_decap",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              // gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              len_tag_key),
      d_debug(debug),
      d_sig_pilot_pos(sig_pilot_pos),
      d_pad_front(pad_front),
      d_delay_tx2rx(delay_tx2rx),
      d_frame_len(frame_len),
      // d_len_tag_key(pmt::string_to_symbol(len_tag_key)),
      // d_length_tag_key_str(length_tag_key),
      d_pad(0),
      d_sig_pilot_len(80)
    {
      // fft
      // d_shift = true;
      // d_fft = new fft::fft_complex(64, true);
    }

    /*
     * Our virtual destructor.
     */
    packet_decap_impl::~packet_decap_impl()
    {
    }

    /*
    void
    packet_decap_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      // <+forecast+> e.g. ninput_items_required[0] = noutput_items
    }
    */

    /*
    int
    packet_decap_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    */
    int
    packet_decap_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      // const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
      // <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      // gr_complex *out_sync = (gr_complex *) output_items[1]; // short sync

      int ninput = ninput_items[0];
      int noutput = noutput_items;

      /*
      const char *iptr;
      char *optr;
      int cons, ret;

      // Do <+signal processing+>

      // No change in delay; just memcpy ins to outs
      if(d_pad_front == 0) {
        for(size_t i = 0; i < input_items.size(); i++) {
          iptr = (const char *)input_items[i];
          optr = (char *)output_items[i];
          std::memcpy(optr, iptr, sizeof(gr_complex) * noutput_items);
        }
        cons = noutput_items;
        ret = noutput_items;
      }

      else {}
      */

      std::memset(out, 0x00, sizeof(gr_complex) * (d_pad_front + 4*d_sig_pilot_len + ninput_items[0]));
      std::memcpy(out, in+d_pad_front+d_delay_tx2rx, sizeof(gr_complex) * d_frame_len);
      // std::memcpy(out + d_pad_front + 2*d_sig_pilot_len, in, sizeof(gr_complex) * ninput_items[0]);
      // std::cout << "Starting point: " << (d_pad_front+d_delay_tx2rx) << std::endl;

      // std::memset(out_sync, 0x00, sizeof(gr_complex) * 64);
      // std::memcpy(out_sync, in+d_pad_front+d_delay_tx2rx, sizeof(gr_complex) * 64);
      // std::cout << out_sync[0]/gr_complex( 0.5000, -0.1435) << out_sync[63]/gr_complex( 0.5000, -0.1435) << std::endl;

      /*
      std::memcpy(d_fft->get_inbuf(), out_sync, sizeof(gr_complex)*64);
      d_fft->execute(); // compute the fft
      volk_32fc_s32f_x2_power_spectral_density_32f(out_sync, d_fft->get_outbuf(), 64, 1.0, 64);
      */
      // compute the self-interference channel?

      int produced = d_frame_len;

      // Tell runtime system how many input items we consumed on
      // each input stream.
      // consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      // return noutput_items;
      return produced;
    }

  } /* namespace fullduplex */
} /* namespace gr */
