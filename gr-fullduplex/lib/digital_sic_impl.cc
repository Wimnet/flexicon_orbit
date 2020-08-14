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
#include "digital_sic_impl.h"

/* main
*
*/

namespace gr {
  namespace fullduplex {

    digital_sic::sptr
    digital_sic::make(bool debug,
                            unsigned int delay_tx2rx,
                            unsigned int pad_front,
                            unsigned int sig_pilot_pos,
                            unsigned int frame_len,
                            unsigned int si_chan_k,
                            unsigned int si_chan_dim,
                            double premultiplier)
    {
      return gnuradio::get_initial_sptr
        (new digital_sic_impl(debug, delay_tx2rx, pad_front, sig_pilot_pos, frame_len, si_chan_k, si_chan_dim, premultiplier));
    }

    /*
     * The private constructor
     */
    digital_sic_impl::digital_sic_impl(bool debug,
                                       unsigned int delay_tx2rx,
                                       unsigned int pad_front,
                                       unsigned int sig_pilot_pos,
                                       unsigned int frame_len,
                                       unsigned int si_chan_k,
                                       unsigned int si_chan_dim,
                                       double premultiplier)
      : gr::tagged_stream_block("digital_sic",
              gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(gr_complex)),
              gr::io_signature::make3(3, 3, sizeof(gr_complex), sizeof(gr_complex), sizeof(gr_complex)),
              "packet_len"),
              d_debug(debug),
              d_delay_tx2rx(delay_tx2rx),
              d_pad_front(pad_front),
              d_sig_pilot_pos(sig_pilot_pos),
              d_frame_len(frame_len),
              d_si_chan_k(si_chan_k),
              d_si_chan_dim(si_chan_dim),
              d_sig_pilot_scale(0.75),
              d_premultiplier(premultiplier),
              d_initial(true) // MK
    {
      // set_tag_propagation_policy(block::TPP_DONT);

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

      set_delay_tx2rx(delay_tx2rx);
    }

    /*
     * Our virtual destructor.
     */
    digital_sic_impl::~digital_sic_impl()
    {
    }

    void
    digital_sic_impl::set_delay_tx2rx(int delay_tx2rx)
    {
      gr::thread::scoped_lock lock(d_mutex);
      d_delay_tx2rx = delay_tx2rx;
    }

    /*
    void
    digital_sic_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      // <+forecast+> e.g. ninput_items_required[0] = noutput_items
      ninput_items_required[0] = 2720; // noutput_items;
      ninput_items_required[1] = 2720; // noutput_items;
    }
    */

    /*
    int
    digital_sic_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    */
    int
    digital_sic_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)

    {
      const gr_complex *in_tx = (const gr_complex *) input_items[0];
      const gr_complex *in_rx = (const gr_complex *) input_items[1];

      gr_complex *out_rx = (gr_complex *) output_items[0];
      gr_complex *out_rx_res = (gr_complex *) output_items[1];

      /*
      gr_complex *out_rx = (gr_complex *) output_items[0];
      gr_complex *out_rx_res = (gr_complex *) output_items[1];
      */
      gr_complex *h = (gr_complex *) output_items[2];

      noutput_items = d_frame_len; // ninput_items[0];

      std::memset(out_rx, 0x00, sizeof(gr_complex) * noutput_items);
      std::memset(out_rx_res, 0x00, sizeof(gr_complex) * noutput_items);
      std::memset(h, 0x00, sizeof(gr_complex) * (2*d_si_chan_k+1));

      // init Eigen vectors
      Eigen::VectorXcf pilot_tx(80*2);
      Eigen::VectorXcf pilot_rx(80*2);
      Eigen::VectorXcf tx_sig_buff(d_frame_len + 2*d_si_chan_k + 1);
      Eigen::VectorXcf rx_sig_buff(d_frame_len + 2*d_si_chan_k + 1);

      for (int idx = 0; idx < 80*2; idx++) {
        pilot_tx(idx) = in_tx[d_pad_front+d_sig_pilot_pos*80*2 + idx];
        pilot_rx(idx) = in_rx[d_pad_front+d_sig_pilot_pos*80*2+d_delay_tx2rx + idx];
      }

      for (int idx = 0; idx < d_frame_len; idx++) {
        // out_tx[idx] = in_tx[d_pad_front+idx];
        out_rx[idx] = in_rx[d_pad_front+d_delay_tx2rx+idx];
      }

      for (int idx = 0; idx < d_frame_len + 2*d_si_chan_dim + 1; idx++) {
        tx_sig_buff(idx) = in_tx[d_pad_front-d_si_chan_k+idx];
        rx_sig_buff(idx) = in_rx[d_pad_front-d_si_chan_k+d_delay_tx2rx+idx];
      }
      // memcpy(pilot, in, sizeof(gr_complex) * 80)
      // cout << "Vector element type is " << typeid(pilot(1)).name() << endl;

      // Eigen::MatrixXcf pilot_toeplitz;
      // pilot_toeplitz = sig_toeplitz(pilot, 40, 10, 1);
      Eigen::VectorXcf hh( (2*d_si_chan_k+1) * d_si_chan_dim );
      Eigen::VectorXf hh_mag( (2*d_si_chan_k+1) * d_si_chan_dim );
      float hh_mag_max;

      // Eigen::VectorXcf si_chnl_est(Eigen::VectorXcf &tx_sig, Eigen::VectorXcf &rx_sig, int l, int k)
      // if (d_initial) { // MK
      //   d_h = Eigen::VectorXcf( (2*d_si_chan_k+1) * d_si_chan_dim ); // MK
      //   d_h = si_chnl_est(pilot_tx, pilot_rx, 80*2-2*d_si_chan_k-1, d_si_chan_k, d_si_chan_dim); // MK
      //   d_initial = false; // MK
      // }
      // hh = d_h; // MK
    
      hh = si_chnl_est(pilot_tx, pilot_rx, 80*2-2*d_si_chan_k-1, d_si_chan_k, d_si_chan_dim);
      for (int idx = 0; idx < (2*d_si_chan_k+1) * d_si_chan_dim; idx++) {
        h[idx] = hh(idx);
      }
      hh_mag = hh.cwiseAbs2();
      // std::cout << "Max |h| = " << hh_mag << std::endl;
      std::ptrdiff_t hh_mag_max_idx;
      hh_mag_max = hh_mag.maxCoeff(&hh_mag_max_idx);
      // std::cout << "Max |h| at " << hh_mag_max_idx << std::endl;

      // perform digital SIC
      Eigen::MatrixXcf tx_sig_toeplitz;
      tx_sig_toeplitz = sig_toeplitz(tx_sig_buff, d_frame_len, d_si_chan_k, d_si_chan_dim);
      Eigen::VectorXcf rx_sig_res;
      rx_sig_res = rx_sig_buff.segment(d_si_chan_k-1, d_frame_len) - tx_sig_toeplitz * hh;
      for (int idx = 0; idx < d_frame_len - 80*2; idx++) {
        out_rx_res[idx] = rx_sig_res(idx+d_si_chan_k-1);
      }



      /* estimate tx pilot
      Eigen::MatrixXcf pilot_tx_toeplitz;
      Eigen::VectorXcf pilot_rx_res;
      pilot_tx_toeplitz = sig_toeplitz(pilot_tx, 80-2*d_si_chan_k-1, d_si_chan_k, d_si_chan_dim);
      pilot_rx_res = pilot_rx.segment(d_si_chan_k-1, 80-2*d_si_chan_k-1) - pilot_tx_toeplitz * hh;
      for (int idx = 0; idx < 80-2*d_si_chan_k-1; idx++) {
        out_rx_pilot_est[idx+d_si_chan_k-1] = pilot_rx_res(idx);
      }
      */





      int produced = noutput_items; // d_frame_len;

      // Tell runtime system how many input items we consumed on
      // each input stream.
      // consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      // return noutput_items;
      return produced;

      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      // consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      // return noutput_items;
    }

  } /* namespace fullduplex */
} /* namespace gr */
