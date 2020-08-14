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


#ifndef INCLUDED_FULLDUPLEX_PARSE_STREAM_SYNC_H
#define INCLUDED_FULLDUPLEX_PARSE_STREAM_SYNC_H

#include <fullduplex/api.h>
// #include <gnuradio/block.h>
// #include <gnuradio/sync_block.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace fullduplex {

    /*!
     * \brief <+description of block+>
     * \ingroup fullduplex
     *
     */
    class FULLDUPLEX_API parse_stream_sync : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<parse_stream_sync> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of fullduplex::parse_stream_sync.
       *
       * To avoid accidental use of raw pointers, fullduplex::parse_stream_sync's
       * constructor is in a private implementation
       * class. fullduplex::parse_stream_sync::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool debug=false,
                       unsigned int delay_tx2rx=0,
                       unsigned int pad_front=0,
                       unsigned int frame_len=0);
      virtual void set_delay(int delay_tx2rx) = 0;

     protected:
       gr_complex d_sig_pilot[80];
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_PARSE_STREAM_SYNC_H */
