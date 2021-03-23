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
#include <fullduplex/packet_encap2.h>
#include <volk/volk.h>
#include <sstream>

using namespace gr::fullduplex;


class packet_encap2_impl : public packet_encap2 {

public:
packet_encap2_impl(bool debug,
                   bool delay,
                   double delay_sec,
                   unsigned int pad_front,
                   unsigned int pad_tail,
                   unsigned int sig_pilot_pos,
                   double premultiplier,
                   int sync_word) : tagged_stream_block("fullduplex_packet_encap2",
      gr::io_signature::make(1, 1, sizeof(gr_complex)),
      gr::io_signature::make(1, 1, sizeof(gr_complex)),
      "packet_len"),
      d_debug(debug),
      d_delay(delay),
      d_delay_sec(delay_sec),
      d_pad_front(pad_front),
      d_pad_tail(pad_tail),
      d_sig_pilot_pos(sig_pilot_pos),
      d_sig_pilot_len(80*2),
      d_sig_pilot_scale(0.75),
      d_premultiplier(premultiplier) {
      set_tag_propagation_policy(block::TPP_DONT);

      // d_sig_pilot
      // 5: WiFi LTS
      // 4: QPSK modulated "sync word"
      // 3: Sync word which is symmetric in the time domain
      // 2: Sync word 2 from GRC OFDM (all subcarriers present)
      // 1: Sync word 1 from GRC OFDM
      // 0: No sync word
      //std::stringstream ss(sync_word);
      //int sync_word_i;
      //ss >> sync_word_i;
      switch (sync_word) {
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

      for (int idx_sig_pilot = 0; idx_sig_pilot<80; idx_sig_pilot++) {
        d_sig_pilot[idx_sig_pilot+80] = d_sig_pilot[idx_sig_pilot];
      }

      for (int idx_sig_pilot = 0; idx_sig_pilot<80*2; idx_sig_pilot++) {
        d_sig_pilot[idx_sig_pilot] *= d_premultiplier;
      }

}

~packet_encap2_impl(){
}

void
set_premultiplier(float premultiplier)
{
  gr::thread::scoped_lock lock(d_mutex);
  // // undo old premultiplier
  // for (int idx_sig_pilot = 0; idx_sig_pilot<80*2; idx_sig_pilot++) {
  //   d_sig_pilot[idx_sig_pilot] /= d_premultiplier;
  // }
  // d_premultiplier = premultiplier;
  // // apply new premultiplier
  // for (int idx_sig_pilot = 0; idx_sig_pilot<80*2; idx_sig_pilot++) {
  //   d_sig_pilot[idx_sig_pilot] *= d_premultiplier;
  // }
}

int calculate_output_stream_length(const gr_vector_int &ninput_items) {
  return ninput_items[0] + d_pad_front + d_pad_tail;
}

int work (int noutput_items, gr_vector_int& ninput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items) {

  const gr_complex *in = (const gr_complex*)input_items[0];
  gr_complex *out = (gr_complex*)output_items[0];

  int ninput = ninput_items[0];
  int noutput = noutput_items;

  /*
  std::memset(out, 0x00, sizeof(gr_complex) * (ninput_items[0] + d_pad_front + d_pad_tail));

  std::memcpy(out + d_pad_front, in, sizeof(gr_complex) * ninput_items[0]);
  */
  std::memset(out, 0x00, sizeof(gr_complex)*(ninput_items[0] + 4*d_sig_pilot_len + d_pad_front + d_pad_tail));
  std::memcpy(out + d_pad_front + d_sig_pilot_pos*d_sig_pilot_len, d_sig_pilot, sizeof(gr_complex)*d_sig_pilot_len);
  std::memcpy(out + d_pad_front + 2*d_sig_pilot_len, in, sizeof(gr_complex)*ninput_items[0]);

  // std::cout << "Pad position: " << d_sig_pilot_pos*d_sig_pilot_len << std::endl;

  int produced = ninput_items[0] + 4*d_sig_pilot_len + d_pad_front + d_pad_tail;
  const pmt::pmt_t src = pmt::string_to_symbol(alias());

  // volk_32fc_s32fc_multiply_32fc(out, out, 0.316227766, produced);

  /*
  #ifdef FOO_UHD
  if(d_delay) {
    static const pmt::pmt_t time_key = pmt::string_to_symbol("tx_time");
    struct timeval t;
    gettimeofday(&t, NULL);
    uhd::time_spec_t now = uhd::time_spec_t(t.tv_sec + t.tv_usec / 1000000.0)
      + uhd::time_spec_t(d_delay_sec);

    const pmt::pmt_t time_value = pmt::make_tuple(
      pmt::from_uint64(now.get_full_secs()),
      pmt::from_double(now.get_frac_secs())
    );
    add_item_tag(0, nitems_written(0), time_key, time_value, src);
  }
  #endif
  */

  std::vector<gr::tag_t> tags;
  get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0]);
  for (size_t i = 0; i < tags.size(); i++) {
    add_item_tag(0, nitems_written(0),
    tags[i].key,
    tags[i].value);
  }

  return produced;
}

private:
  bool   d_debug;
  bool   d_delay;
  double d_delay_sec;
  unsigned int d_pad_front;
  unsigned int d_pad_tail;
  unsigned int d_sig_pilot_pos;
  int d_sig_pilot_len;
  double d_sig_pilot_scale;
  double d_premultiplier;
};

packet_encap2::sptr
packet_encap2::make(bool debug,
                    bool delay,
                    double delay_sec,
                    unsigned int pad_front,
                    unsigned int pad_tail,
                    unsigned int sig_pilot_pos,
                    double premultiplier,
                    int sync_word) {
  return gnuradio::get_initial_sptr(new packet_encap2_impl(debug, delay, delay_sec, pad_front, pad_tail, sig_pilot_pos, premultiplier, sync_word));
}
