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

#ifndef INCLUDED_FULLDUPLEX_ASYNC_DIVIDE_IMPL_H
#define INCLUDED_FULLDUPLEX_ASYNC_DIVIDE_IMPL_H

#include <fullduplex/async_divide.h>

namespace gr {
  namespace fullduplex {

    class async_divide_impl : public async_divide
    {
     private:
      // Nothing to declare in this block.
      float d_result;
      float d_in1;
      float d_in2;
      void handle1(pmt::pmt_t msg);
      void handle2(pmt::pmt_t msg);

     public:
      async_divide_impl();
      ~async_divide_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_ASYNC_DIVIDE_IMPL_H */

