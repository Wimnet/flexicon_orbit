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

#ifndef INCLUDED_FULLDUPLEX_CARRIER_SENSE_IMPL_H
#define INCLUDED_FULLDUPLEX_CARRIER_SENSE_IMPL_H

#include <fullduplex/carrier_sense.h>

namespace gr {
  namespace fullduplex {

    class carrier_sense_impl : public carrier_sense
    {
     private:
      // Nothing to declare in this block.
      const pmt::pmt_t d_port;
      float d_noisefloor;
      float d_threshold;
      bool d_debug;


     public:
      carrier_sense_impl(bool debug, float threshold);
      ~carrier_sense_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );

      void check_power(float& total_power, int& size);
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_CARRIER_SENSE_IMPL_H */

