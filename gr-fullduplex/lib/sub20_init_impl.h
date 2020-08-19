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

#include <libsub.h>

#ifndef INCLUDED_FULLDUPLEX_SUB20_INIT_IMPL_H
#define INCLUDED_FULLDUPLEX_SUB20_INIT_IMPL_H

#include <fullduplex/sub20_init.h>

namespace gr {
  namespace fullduplex {

    class sub20_init_impl : public sub20_init
    {
     private:
      // Nothing to declare in this block.

     public:
      sub20_init_impl(int atten_code, int phase_code);
      ~sub20_init_impl();

      // initialize sub20 device using libusb-1.0
      void sub20_init_find_dev();
      void set_atten_code(int code);
      void set_phase_code(int code);
      void set_tuner();

      // sub20 handle
      sub_handle my_sub20;

      // callback params
      int d_atten_code;
      int d_phase_code;

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_SUB20_INIT_IMPL_H */

