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
#include "async_divide_impl.h"

namespace gr {
  namespace fullduplex {

    async_divide::sptr
    async_divide::make()
    {
      return gnuradio::get_initial_sptr
        (new async_divide_impl());
    }

    void
    async_divide_impl::handle1(pmt::pmt_t msg)
    {
      d_in1 = float(pmt::to_double(pmt::cdr(msg)));
    }

    void
    async_divide_impl::handle2(pmt::pmt_t msg)
    {
      d_in2 = float(pmt::to_double(pmt::cdr(msg)));
    }

    /*
     * The private constructor
     */
    async_divide_impl::async_divide_impl()
      : gr::sync_block("async_divide",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(float))),
              d_result(0.0),
              d_in1(0.0),
              d_in2(1.0)
    {
      message_port_register_in(pmt::mp("in1"));
      set_msg_handler(pmt::mp("in1"), boost::bind(&async_divide_impl::handle1, this, _1));

      message_port_register_in(pmt::mp("in2"));
      set_msg_handler(pmt::mp("in2"), boost::bind(&async_divide_impl::handle2, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    async_divide_impl::~async_divide_impl()
    {
    }

    int
    async_divide_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      float* out = (float*) output_items[0];

      noutput_items = 8;

      d_result = d_in1/d_in2;

      for (int idx = 0; idx < noutput_items; idx++)
      {
        out[idx] = d_result;
      }

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace fullduplex */
} /* namespace gr */

