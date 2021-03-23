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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "csma_control_impl.h"
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include "csma_control_impl.h"

namespace gr {
  namespace fullduplex {

    csma_control::sptr
    csma_control::make(pmt::pmt_t msg, float trans_prob, int max_backoff, int sense_time, bool debug)
    {
      return gnuradio::get_initial_sptr
        (new csma_control_impl(msg, trans_prob, max_backoff, sense_time, debug));
    }

    void
    csma_control_impl::cs_handle(pmt::pmt_t msg)
    {
      d_detected = pmt::to_bool(msg);
    }


    /*
     * The private constructor
     */
    csma_control_impl::csma_control_impl(pmt::pmt_t msg, float trans_prob, int max_backoff, int sense_time, bool debug)
      : gr::block("csma_control",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              d_max_backoff(max_backoff),
              d_sense_time(sense_time),
              d_debug(debug),
              d_finished(false),
              d_trans_prob(trans_prob),
              d_port_out(pmt::mp("pkt")),
              d_port_in(pmt::mp("cs_in")),
              d_msg(msg)
    {
      message_port_register_out(d_port_out);

      message_port_register_in(d_port_in);
      set_msg_handler(d_port_in, boost::bind(&csma_control_impl::cs_handle, this, _1));

      srand( (unsigned)time(NULL) );
    }

    /*
     * Our virtual destructor.
     */
    csma_control_impl::~csma_control_impl()
    {
    }

    bool
    csma_control_impl::start ()
    {
      // NOTE: d_finished should be something explicitly thread safe. But since
      // nothing breaks on concurrent access, I'll just leave it as bool.
      d_finished = false;
      d_thread = boost::shared_ptr<gr::thread::thread>(
          new gr::thread::thread(boost::bind(&csma_control_impl::run, this)));

      return block::start();
    }

    bool 
    csma_control_impl::stop()
    {
        // Shut down the thread
        d_finished = true;
        d_thread->interrupt();
        d_thread->join();

        return block::stop();
    }

    /*
    Algorithm:
      - Sense the channel for X us
      - If idle, transmit with probability p
      - If not idle, backoff for Y us
    */
    void 
    csma_control_impl::run()
    {
        while (!d_finished) 
        {
            d_detected = false;
            boost::this_thread::sleep(
                boost::posix_time::microseconds(static_cast<long>(d_sense_time)));
            if (d_finished) 
            {
                return;
            }
            if (d_detected) 
            {
              if (d_debug)
              {
                unsigned long long now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                std::cout << "DETECTED TRANSMISSION AT: " << now << std::endl;
              }
              long backoff = rand() % d_max_backoff;
              boost::this_thread::sleep(
                  boost::posix_time::microseconds(backoff));
            }
            else 
            {
              float r = ((float) rand() / (RAND_MAX));
              if (r < d_trans_prob)
              {
                if (d_debug)
                {
                  unsigned long long now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                  std::cout << "PACKET SENT AT: " << now << std::endl;
                }
                message_port_pub(d_port_out, d_msg);
              }
              boost::this_thread::sleep(
                boost::posix_time::milliseconds(static_cast<long>(10)));
            }

            if (d_finished) {
                return;
            }
        }
    }


  } /* namespace fullduplex */
} /* namespace gr */

