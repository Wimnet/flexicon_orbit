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

#ifndef INCLUDED_FULLDUPLEX_CSMA_CONTROL_IMPL_H
#define INCLUDED_FULLDUPLEX_CSMA_CONTROL_IMPL_H

#include <fullduplex/csma_control.h>

namespace gr {
  namespace fullduplex {

    class csma_control_impl : public csma_control
    {
     private:
      // Nothing to declare in this block.

      void run();

     public:
      csma_control_impl(pmt::pmt_t msg, float trans_prob, int max_backoff, int sense_time, bool debug);
      ~csma_control_impl();

      boost::shared_ptr<gr::thread::thread> d_thread;
      int d_max_backoff;
      int d_sense_time;
      float d_trans_prob;
      bool d_debug;
      bool d_finished;
      bool d_detected;
      const pmt::pmt_t d_port_out;
      const pmt::pmt_t d_port_in;
      pmt::pmt_t d_msg;

      void cs_handle(pmt::pmt_t msg);

      void set_msg(pmt::pmt_t msg) { d_msg = msg; }
      pmt::pmt_t msg() const { return d_msg; }

      // Overloading these to start and stop the internal thread that
      // periodically produces the message.
      bool start();
      bool stop();


    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_CSMA_CONTROL_IMPL_H */

