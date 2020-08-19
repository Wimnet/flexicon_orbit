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


#ifndef INCLUDED_FULLDUPLEX_SUB20_INIT_H
#define INCLUDED_FULLDUPLEX_SUB20_INIT_H

#include <fullduplex/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace fullduplex {

    /*!
     * \brief <+description of block+>
     * \ingroup fullduplex
     *
     */
    class FULLDUPLEX_API sub20_init : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<sub20_init> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of subdev::sub20_init.
       *
       * To avoid accidental use of raw pointers, subdev::sub20_init's
       * constructor is in a private implementation
       * class. subdev::sub20_init::make is the public interface for
       * creating new instances.
       */
      static sptr make(int phase_code, int atten_code);
      virtual void set_atten_code(int code) = 0;
      virtual void set_phase_code(int code) = 0;
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_SUB20_INIT_H */

