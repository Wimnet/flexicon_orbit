/*
 * Copyright (C) 2013 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <fullduplex/packet_encap2.h>

#include <gnuradio/io_signature.h>
/*
#include <sys/time.h>
#ifdef FOO_UHD
#include <uhd/types/time_spec.hpp>
#endif
*/

using namespace gr::fullduplex;


class packet_encap2_impl : public packet_encap2 {

public:
packet_encap2_impl(bool debug,
                   bool delay,
                   double delay_sec,
                   unsigned int pad_front,
                   unsigned int pad_tail,
                   unsigned int sig_pilot_pos,
									 double premultiplier) : tagged_stream_block("fullduplex_packet_encap2",
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

      for (int idx_sig_pilot = 0; idx_sig_pilot<80; idx_sig_pilot++) {
        d_sig_pilot[idx_sig_pilot+80] = d_sig_pilot[idx_sig_pilot];
      }
      for (int idx_sig_pilot = 0; idx_sig_pilot<80*2; idx_sig_pilot++) {
				d_sig_pilot[idx_sig_pilot] *= d_sig_pilot_scale;
        d_sig_pilot[idx_sig_pilot] *= d_premultiplier;
      }
}

~packet_encap2_impl(){
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
										double premultiplier) {
  return gnuradio::get_initial_sptr(new packet_encap2_impl(debug, delay, delay_sec, pad_front, pad_tail, sig_pilot_pos, premultiplier));
}
