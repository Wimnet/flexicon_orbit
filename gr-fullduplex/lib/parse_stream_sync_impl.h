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

#ifndef INCLUDED_FULLDUPLEX_PARSE_STREAM_SYNC_IMPL_H
#define INCLUDED_FULLDUPLEX_PARSE_STREAM_SYNC_IMPL_H

#include <fullduplex/parse_stream_sync.h>


// #include <Eigen/Dense>
// #include <list>

// Eigen::MatrixXcf sig_toeplitz(Eigen::VectorXcf &sig, int l, int k, int dim);
// Eigen::VectorXcf si_chnl_est(Eigen::VectorXcf &tx_sig, Eigen::VectorXcf &rx_sig, int l, int k, int dim);

// Eigen::VectorXcf si_chnl_est(Eigen::VectorXcf &tpilot, Eigen::VectorXcf &rpilot, int len);

// Eigen::VectorXf dig_sic(Eigen::VectorXf &tx_data, Eigen::VectorXf &rx_data, Eigen::VectorXf &SI_chnl, int l, int k);

/**********
/ Eigen
***********/
// using Eigen::MatrixXd;

/* Returns the Toeplitz matrix of signal
* @param l: considered signal length
* @param k: est. channel length is 2*k
* @param dim: non-linear dimension is dim
* @return A: the constructed Toeplitz matrix is with size l * (2*k*dim)
*/
/*
Eigen::MatrixXcf sig_toeplitz(Eigen::VectorXcf &sig, int l, int k, int dim)
{
	// cout << "Size: " << sig.size() << endl;
	if (2*k+l+1 > sig.size()) {
		std::cout << "length not correct!" << std::endl;
	}

    Eigen::MatrixXcf A(l, (2*k+1)*dim);
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < 2*k+1; j++) {
            for (int p = 0; p < dim; p++) {
                A(i, j*dim + p) = pow(sig(j+i+k), 2*p+1);
            }
        }
    }

    // cout << "constructd Toeplitz matrix: " << endl;
    // cout << A << endl;
    // cout << "Size of the constructed Toeplitz matrix: " << A.rows() << "x" << A.cols() << endl;
    return A;
}
*/

/* Returns the estimate self-interfence (SI) channel
* @param tx_sig: known TX signal
* @param rx_sig: received RX signal
*/
/*
Eigen::VectorXcf si_chnl_est(Eigen::VectorXcf &tx_sig, Eigen::VectorXcf &rx_sig, int l, int k, int dim)
{
    if (tx_sig.size() != rx_sig.size()) {
        std::cout << "Unmatched TX and RX signal lengths!" << std::endl;
        exit(0);
    }

    // matrix w/ dimenstion l * 2k
    Eigen::MatrixXcf A = sig_toeplitz(tx_sig, l, k, dim);
    Eigen::BDCSVD <Eigen::MatrixXcf> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
    // Eigen::VectorXcf h = svd.solve(rx_sig.segment(k,l));
		Eigen::VectorXcf h = svd.solve(rx_sig.segment(k-1,l));

    return h;
}
*/



namespace gr {
  namespace fullduplex {

    class parse_stream_sync_impl : public parse_stream_sync
    {
     private:
      // Nothing to declare in this block.
      bool d_debug;
      int d_delay_tx2rx;
      int d_pad_front;
      int d_frame_len;

      gr::thread::mutex d_mutex;

     public:
      parse_stream_sync_impl(bool debug,
                             unsigned int delay_tx2rx,
                             unsigned int pad_front,
                             unsigned int frame_len);
      ~parse_stream_sync_impl();

      void set_delay(int delay_tx2rx);

      // Where all the action really happens
      // void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_PARSE_STREAM_SYNC_IMPL_H */
