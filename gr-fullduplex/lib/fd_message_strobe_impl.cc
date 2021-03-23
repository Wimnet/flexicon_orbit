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
#include "fd_message_strobe_impl.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <stdexcept>


namespace gr {
  namespace fullduplex {

    fd_message_strobe::sptr
    fd_message_strobe::make(pmt::pmt_t msg, long period_ms, bool debug)
    {
      return gnuradio::get_initial_sptr
        (new fd_message_strobe_impl(msg, period_ms, debug));
    }


    /*
     * The private constructor
     */
    fd_message_strobe_impl::fd_message_strobe_impl(pmt::pmt_t msg, long period_ms, bool debug)
      : gr::block("fd_message_strobe",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              d_period_ms(period_ms),
              d_msg(msg),
              d_finished(false),
              d_port(pmt::mp("strobe"))
    {
      message_port_register_out(d_port);
    }

    /*
     * Our virtual destructor.
     */
    fd_message_strobe_impl::~fd_message_strobe_impl()
    {
    }

    bool
    fd_message_strobe_impl::start ()
    {
      // NOTE: d_finished should be something explicitly thread safe. But since
      // nothing breaks on concurrent access, I'll just leave it as bool.
      d_finished = false;
      d_thread = boost::shared_ptr<gr::thread::thread>(
          new gr::thread::thread(boost::bind(&fd_message_strobe_impl::run, this)));

      return block::start();
    }

    bool 
    fd_message_strobe_impl::stop()
    {
        // Shut down the thread
        d_finished = true;
        d_thread->interrupt();
        d_thread->join();

        return block::stop();
    }

    void 
    fd_message_strobe_impl::run()
    {
        while (!d_finished) {
            boost::this_thread::sleep(
                boost::posix_time::milliseconds(static_cast<long>(d_period_ms)));
            if (d_finished) {
                return;
            }

            unsigned long long now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::cout << "TIME SENT:" << now << std::endl;
            message_port_pub(d_port, d_msg);
        }
    }

  } /* namespace fullduplex */
} /* namespace gr */

