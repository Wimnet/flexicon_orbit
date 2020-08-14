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


#ifndef INCLUDED_FULLDUPLEX_OFDM_CONSTELLATION_H
#define INCLUDED_FULLDUPLEX_OFDM_CONSTELLATION_H

#include <fullduplex/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace fullduplex {

    /*!
     * \brief <+description of block+>
     * \ingroup fullduplex
     *
     */
    class FULLDUPLEX_API ofdm_constellation : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<ofdm_constellation> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of fullduplex::ofdm_constellation.
       *
       * To avoid accidental use of raw pointers, fullduplex::ofdm_constellation's
       * constructor is in a private implementation
       * class. fullduplex::ofdm_constellation::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool debug, int delay_tx2rx, int pad_front, int packet_len);
      virtual void set_delay_tx2rx(int delay_tx2rx) = 0;
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_OFDM_CONSTELLATION_H */

