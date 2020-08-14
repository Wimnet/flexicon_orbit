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

#ifndef INCLUDED_FULLDUPLEX_RESIDUAL_SI_VIS_IMPL_H
#define INCLUDED_FULLDUPLEX_RESIDUAL_SI_VIS_IMPL_H

#include <fullduplex/residual_si_vis.h>

namespace gr {
  namespace fullduplex {

    class residual_si_vis_impl : public residual_si_vis
    {
     private:
      // Nothing to declare in this block.
     	float m_real;
     	float m_imag;

     public:
      residual_si_vis_impl();
      ~residual_si_vis_impl();

      void setVals(int cf_0, int cf_1);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_RESIDUAL_SI_VIS_IMPL_H */

