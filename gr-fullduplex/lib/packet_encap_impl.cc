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
#include "packet_encap_impl.h"

namespace gr {
  namespace fullduplex {

    packet_encap::sptr
    packet_encap::make(bool debug,
                       unsigned int sig_pilot_pos,
                       unsigned int pad_front,
                       const std::string& len_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new packet_encap_impl(debug, sig_pilot_pos, pad_front, len_tag_key));
    }

    /*
     * The private constructor
     */
    packet_encap_impl::packet_encap_impl(bool debug,
                                         unsigned int sig_pilot_pos,
                                         unsigned int pad_front,
                                         const std::string& len_tag_key)
      : gr::tagged_stream_block("packet_encap",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              len_tag_key),
      d_debug(debug),
      d_sig_pilot_pos(sig_pilot_pos),
      d_pad_front(pad_front),
      // d_len_tag_key(pmt::string_to_symbol(len_tag_key)),
      // d_length_tag_key_str(length_tag_key),
      d_pad(0),
      d_sig_pilot_len(80)
    {
      set_tag_propagation_policy(TPP_DONT);

      // d_sig_pilot.resize(d_sig_pilot_len);
      /*
      for (unsigned int i = 0; i < d_sig_pilot_len; i++) {
        d_sig_pilot[i] = gr_complex(0.01*i, 0.01*(d_sig_pilot_len-i));
      }
      */
      // d_sig_pilot
      d_sig_pilot[ 0] = gr_complex( 0.0000,  0.0000);
      d_sig_pilot[ 1] = gr_complex( 0.5000, -0.1435);
      d_sig_pilot[ 2] = gr_complex( 0.0517, -0.1429);
      d_sig_pilot[ 3] = gr_complex( 0.0615, -0.1139);
      d_sig_pilot[ 4] = gr_complex( 0.0147, -0.2222);
      d_sig_pilot[ 5] = gr_complex(-0.1490,  0.0156);
      d_sig_pilot[ 6] = gr_complex( 0.0912,  0.0139);
      d_sig_pilot[ 7] = gr_complex( 0.2461, -0.1844);
      d_sig_pilot[ 8] = gr_complex( 0.1973,  0.1315);
      d_sig_pilot[ 9] = gr_complex(-0.0908,  0.2135);
      d_sig_pilot[10] = gr_complex( 0.0181, -0.0698);
      d_sig_pilot[11] = gr_complex( 0.1200, -0.1509);
      d_sig_pilot[12] = gr_complex(-0.2075, -0.1927);
      d_sig_pilot[13] = gr_complex(-0.0559, -0.0674);
      d_sig_pilot[14] = gr_complex(-0.0773, -0.0955);
      d_sig_pilot[15] = gr_complex(-0.1356, -0.3690);
      d_sig_pilot[16] = gr_complex( 0.0930,  0.0000);
      d_sig_pilot[17] = gr_complex(-0.1356,  0.3690);
      d_sig_pilot[18] = gr_complex(-0.0773,  0.0955);
      d_sig_pilot[19] = gr_complex(-0.0559,  0.0674);
      d_sig_pilot[20] = gr_complex(-0.2075,  0.1927);
      d_sig_pilot[21] = gr_complex( 0.1200,  0.1509);
      d_sig_pilot[22] = gr_complex( 0.0181,  0.0698);
      d_sig_pilot[23] = gr_complex(-0.0908, -0.2135);
      d_sig_pilot[24] = gr_complex( 0.1973, -0.1315);
      d_sig_pilot[25] = gr_complex( 0.2461,  0.1844);
      d_sig_pilot[26] = gr_complex( 0.0912, -0.0139);
      d_sig_pilot[27] = gr_complex(-0.1490, -0.0156);
      d_sig_pilot[28] = gr_complex( 0.0147,  0.2222);
      d_sig_pilot[29] = gr_complex( 0.0615,  0.1139);
      d_sig_pilot[30] = gr_complex( 0.0517,  0.1429);
      d_sig_pilot[31] = gr_complex( 0.5000,  0.1435);
      d_sig_pilot[32] = gr_complex( 0.0000,  0.0000);
      d_sig_pilot[33] = gr_complex(-0.5000,  0.1435);
      d_sig_pilot[34] = gr_complex(-0.0517,  0.1429);
      d_sig_pilot[35] = gr_complex(-0.0615,  0.1139);
      d_sig_pilot[36] = gr_complex(-0.0147,  0.2222);
      d_sig_pilot[37] = gr_complex( 0.1490, -0.0156);
      d_sig_pilot[38] = gr_complex(-0.0912, -0.0139);
      d_sig_pilot[39] = gr_complex(-0.2461,  0.1844);
      d_sig_pilot[40] = gr_complex(-0.1973, -0.1315);
      d_sig_pilot[41] = gr_complex( 0.0908, -0.2135);
      d_sig_pilot[42] = gr_complex(-0.0181,  0.0698);
      d_sig_pilot[43] = gr_complex(-0.1200,  0.1509);
      d_sig_pilot[44] = gr_complex( 0.2075,  0.1927);
      d_sig_pilot[45] = gr_complex( 0.0559,  0.0674);
      d_sig_pilot[46] = gr_complex( 0.0773,  0.0955);
      d_sig_pilot[47] = gr_complex( 0.1356,  0.3690);
      d_sig_pilot[48] = gr_complex(-0.0930,  0.0000);
      d_sig_pilot[49] = gr_complex( 0.1356, -0.3690);
      d_sig_pilot[50] = gr_complex( 0.0773, -0.0955);
      d_sig_pilot[51] = gr_complex( 0.0559, -0.0674);
      d_sig_pilot[52] = gr_complex( 0.2075, -0.1927);
      d_sig_pilot[53] = gr_complex(-0.1200, -0.1509);
      d_sig_pilot[54] = gr_complex(-0.0181, -0.0698);
      d_sig_pilot[55] = gr_complex( 0.0908,  0.2135);
      d_sig_pilot[56] = gr_complex(-0.1973,  0.1315);
      d_sig_pilot[57] = gr_complex(-0.2461, -0.1844);
      d_sig_pilot[58] = gr_complex(-0.0912,  0.0139);
      d_sig_pilot[59] = gr_complex( 0.1490,  0.0156);
      d_sig_pilot[60] = gr_complex(-0.0147, -0.2222);
      d_sig_pilot[61] = gr_complex(-0.0615, -0.1139);
      d_sig_pilot[62] = gr_complex(-0.0517, -0.1429);
      d_sig_pilot[63] = gr_complex(-0.5000, -0.1435);
      d_sig_pilot[64] = gr_complex( 0.0000,  0.0000);
      d_sig_pilot[65] = gr_complex( 0.5000, -0.1435);
      d_sig_pilot[66] = gr_complex( 0.0517, -0.1429);
      d_sig_pilot[67] = gr_complex( 0.0615, -0.1139);
      d_sig_pilot[68] = gr_complex( 0.0147, -0.2222);
      d_sig_pilot[69] = gr_complex(-0.1490,  0.0156);
      d_sig_pilot[70] = gr_complex( 0.0912,  0.0139);
      d_sig_pilot[71] = gr_complex( 0.2461, -0.1844);
      d_sig_pilot[72] = gr_complex( 0.1973,  0.1315);
      d_sig_pilot[73] = gr_complex(-0.0908,  0.2135);
      d_sig_pilot[74] = gr_complex( 0.0181, -0.0698);
      d_sig_pilot[75] = gr_complex( 0.1200, -0.1509);
      d_sig_pilot[76] = gr_complex(-0.2075, -0.1927);
      d_sig_pilot[77] = gr_complex(-0.0559, -0.0674);
      d_sig_pilot[78] = gr_complex(-0.0773, -0.0955);
      d_sig_pilot[79] = gr_complex(-0.1356, -0.3690);
      // end of d_sig_pilot

    }

    /*
     * Our virtual destructor.
     */
    packet_encap_impl::~packet_encap_impl()
    {
    }

    /*
    void
    packet_encap_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      // <+forecast+> e.g. ninput_items_required[0] = noutput_items
    }
    */

    /*
    int
    packet_encap_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    */
    int
    packet_encap_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      // pilot signa
      /*
      gr_complex pilot[d_sig_pilot_len];
      for (unsigned int i = 0; i < d_sig_pilot_len; i++) {
        pilot[i] = gr_complex(0.01*i, 0.01*(d_sig_pilot_len-i));
      }
      */

      int ninput = ninput_items[0];
      int noutput = noutput_items;

      std::cout << "call: pad" << d_pad << "   ninput " << ninput << "  noutput " << noutput << std::endl;

      // Do <+signal processing+>
      std::memset(out, 0x00, sizeof(gr_complex) * (d_pad_front + 2*d_sig_pilot_len + ninput_items[0]));
      std::memcpy(out + d_pad_front + d_sig_pilot_pos*d_sig_pilot_len, d_sig_pilot, sizeof(gr_complex) * d_sig_pilot_len);
      std::memcpy(out + d_pad_front + 2*d_sig_pilot_len, in, sizeof(gr_complex) * ninput_items[0]);

      int produced = d_pad_front + 2*d_sig_pilot_len + ninput_items[0];

      // search for tags
      /*
      const uint64_t nread = this->nitems_read(0);
      std::vector<gr::tag_t> tags;
      get_tags_in_range(tags, 0, nread, nread + ninput);
      std::sort(tags.begin(), tags.end(), tag_t::offset_compare);

      uint64_t n = std::min(ninput, noutput);
      */

      // debug
      // std::cout << "tags.size = " << tags.size() << std::endl << std::endl;
      /*
      std::vector<gr::tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0]);
      for (size_t i = 0; i < tags.size(); i++) {
        add_item_tag(0, nitems_written(0),
        tags[i].key,
        tags[i].value);
      }
      */

      // std::cout << "tags.size = " << tags.size() << std::endl << std::endl;

      // Tell runtime system how many input items we consumed on
      // each input stream.
      // consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      // return noutput_items;
      return produced;
    }

  } /* namespace fullduplex */
} /* namespace gr */

