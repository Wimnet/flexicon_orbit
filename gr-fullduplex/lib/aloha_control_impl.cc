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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "aloha_control_impl.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <streambuf>


namespace gr {
  namespace fullduplex {

    aloha_control::sptr
    aloha_control::make(float trans_prob, unsigned int radio, unsigned int pad_front, unsigned int pad_tail, int offset, unsigned int sig_pilot_pos, double premultiplier, int sync_word, bool debug, bool record)
    {
      return gnuradio::get_initial_sptr
        (new aloha_control_impl(trans_prob, radio, pad_front, pad_tail, offset, sig_pilot_pos, premultiplier, sync_word, debug, record));
    }


    /*
     * The private constructor
     */
    aloha_control_impl::aloha_control_impl(float trans_prob, unsigned int radio, unsigned int pad_front, unsigned int pad_tail, int offset, unsigned int sig_pilot_pos, double premultiplier, int sync_word, bool debug, bool record)
      : gr::tagged_stream_block("aloha_control",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)), "packet_len"),
              d_trans_prob(trans_prob),
              d_pad_front(pad_front),
              d_pad_tail(pad_tail),
              d_offset(offset),
              d_sig_pilot_pos(sig_pilot_pos),
              d_premultiplier(premultiplier),
              d_sync_word(sync_word),
              d_debug(debug),
              d_sig_pilot_len(160),
              d_packets_sent(0),
              d_radio(radio),
              d_record(record),
              d_record_latch(false)
              
    {
        set_tag_propagation_policy(block::TPP_DONT);
        switch (d_sync_word) {
          case 0: {
            pilot_lts(d_sig_pilot);
            std::cout << "WiFi LTS pilot" << std::endl;
            break;
          }
          case 4: {
            pilot_qpsk(d_sig_pilot);
            std::cout << "QPSK pilot" << std::endl;
            break;
          }
          case 3: {
            pilot_symmetric(d_sig_pilot);
            std::cout << "Symmetric OFDM pilot" << std::endl;
            break;
          }
          case 2: {
            pilot_sync2(d_sig_pilot);
            std::cout << "Sync word 2 OFDM pilot" << std::endl;
            break;
          }
          case 1 : {
            pilot_sync1(d_sig_pilot);
            std::cout << "Sync word 1 OFDM pilot" << std::endl;
            break;
          }
          case 5: {
            pilot_zero(d_sig_pilot);
            std::cout << "No pilot";
            break;
          }
        }
      // end of d_sig_pilot

      for (int idx_sig_pilot = 0; idx_sig_pilot<80*2; idx_sig_pilot++) {
        d_sig_pilot[idx_sig_pilot] *= d_premultiplier;
      }

      std::string fn = "radio" + std::to_string(d_radio) + "_tx.out";
      d_fstream.open(fn);
      //std::cout.rdbuf(d_fstream.rdbuf());
    }

    /*
     * Our virtual destructor.
     */
    aloha_control_impl::~aloha_control_impl()
    {
      d_fstream.close();
    }

    int
    aloha_control_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      return ninput_items[0] + d_pad_front + d_pad_tail;
    }

    int
    aloha_control_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex* in = (const gr_complex*) input_items[0];
      gr_complex* out = (gr_complex*) output_items[0];

      int ninput = ninput_items[0];
      int noutput = noutput_items;

      int memset_size = ninput + 4*d_sig_pilot_len + d_pad_front + d_pad_tail;

      // if requested memset size is incorrect, return 0
      if (memset_size < 0 || memset_size > noutput_items)
      {
        if (d_debug)
        {
            std::cout << "REQUESTED MEMSET SIZE INCORRECT: " << memset_size << ", NOUTPUT ITEMS: " << noutput_items << std::endl;
        }
        out[0] = gr_complex(0.0, 0.0);
        return 1;
      }

      if (d_debug)
      {
          std::cout << "MEMSET: " << memset_size << ", NOUTPUT ITEMS: " << noutput_items << std::endl;
      }

      std::memset(out, 0x00, sizeof(gr_complex)*memset_size);

      // transmit with probability p
      float r = ((float) rand() / (RAND_MAX));
      if (!d_record_latch)
      {
        r = 0.0;
      }
      if (r < d_trans_prob)
      {
        if (d_debug)
        {
          unsigned long long now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
          std::cout << "PACKET SENT AT: " << now << std::endl;
        }
        std::memcpy(out + d_pad_front + d_offset + d_sig_pilot_pos*d_sig_pilot_len, d_sig_pilot, sizeof(gr_complex)*d_sig_pilot_len);
        std::memcpy(out + d_pad_front + d_offset +  4*d_sig_pilot_len, in, sizeof(gr_complex)*ninput_items[0]);
      }
      else
      {
        if (d_debug)
        {
          // unsigned long long now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
          std::cout << "PACKET NOT TRANSMITTED, PROBABILITY: " << r << std::endl;
        }
      }

      int produced = ninput_items[0] + 4*d_sig_pilot_len + d_pad_front + d_pad_tail;
      const pmt::pmt_t src = pmt::string_to_symbol(alias());


      std::vector<gr::tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0]);
      for (size_t i = 0; i < tags.size(); i++) {
        add_item_tag(0, nitems_written(0),
        tags[i].key,
        tags[i].value);
      }

      if (d_record)
      {
        d_record_latch = true;
      }

      if (d_record_latch)
      {
        d_fstream << "Sending packet " << d_packets_sent << std::endl;
      }

      d_packets_sent++;

      return produced;

    }

  } /* namespace fullduplex */
} /* namespace gr */

