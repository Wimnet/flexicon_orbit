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
#include "residual_si_vis_impl.h"

namespace gr {
  namespace fullduplex {

    residual_si_vis::sptr
    residual_si_vis::make()
    {
      return gnuradio::get_initial_sptr
        (new residual_si_vis_impl());
    }

    /*
     * The private constructor
     */
    residual_si_vis_impl::residual_si_vis_impl()
      : gr::sync_block("residual_si_vis",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, 64 * sizeof(gr_complex)))
    {}

    /*
     * Our virtual destructor.
     */
    residual_si_vis_impl::~residual_si_vis_impl()
    {
    }

    void
    residual_si_vis_impl::setVals(int cf_0, int cf_1)
    {
    	m_real = (float)cf_0;
    	m_imag = (float)cf_1;
    }

    int
    residual_si_vis_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr_complex *out = (gr_complex *) output_items[0];

      gr_complex *t = (gr_complex *)malloc(64 * sizeof(gr_complex));
      for (int i = 0; i < 64; i++) {
      	t[i] = gr_complex{m_real, m_imag};
      }
      memcpy(out, t, 64 * sizeof(gr_complex));
      free(t);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace fullduplex */
} /* namespace gr */

