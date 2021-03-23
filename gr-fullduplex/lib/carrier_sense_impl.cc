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
#include "carrier_sense_impl.h"
#include <cmath>
#include <chrono>
#include <iostream>

namespace gr {
  namespace fullduplex {

    carrier_sense::sptr
    carrier_sense::make(bool debug, float threshold)
    {
      return gnuradio::get_initial_sptr
        (new carrier_sense_impl(debug, threshold));
    }


    /*
     * The private constructor
     */
    carrier_sense_impl::carrier_sense_impl(bool debug, float threshold)
      : gr::sync_block("carrier_sense",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
              d_noisefloor(9999999.9),
              d_threshold(threshold),
              d_debug(debug),
              d_port(pmt::mp("cs_out"))
    {
      message_port_register_out(d_port);
    }

    /*
     * Our virtual destructor.
     */
    carrier_sense_impl::~carrier_sense_impl()
    {
    }

    void 
    carrier_sense_impl::check_power(float& total_power, int& size)
    {
      float epsilon = 0.00000001;
      total_power += epsilon;

      float power = 10*log10(total_power/size);

      if (power < d_noisefloor)
      {
        d_noisefloor = power;
      }

      // std::cout << "NOISE FLOOR" << d_noisefloor << ", POWER: " << power << ", SIZE: " << size << std::endl;

      if ((power >= d_noisefloor + d_threshold) && size > 0)
      {
        unsigned long long now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (d_debug) 
        {
          std::cout << "TIME DETECTED: " << now << ", NOISE FLOOR" << d_noisefloor << ", POWER: " << power << ", SIZE: " << size << std::endl;
        }
        //std::cout << "NOISE FLOOR" << d_noisefloor << ", POWER: " << power << ", SIZE: " << size << std::endl;
        message_port_pub(d_port, pmt::PMT_T);
      }
      total_power = 0.0;
      size = 0;
    }

    int
    carrier_sense_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {

      const gr_complex* in = (const gr_complex*) input_items[0];

      int n = noutput_items;
      int size = n;
      float total_power = 0.0;
      int samples = 0;

      while (n >= 8) 
      {
        total_power = total_power + pow(std::abs(*in++), 2);
        total_power = total_power + pow(std::abs(*in++), 2);
        total_power = total_power + pow(std::abs(*in++), 2);
        total_power = total_power + pow(std::abs(*in++), 2);
        total_power = total_power + pow(std::abs(*in++), 2);
        total_power = total_power + pow(std::abs(*in++), 2);
        total_power = total_power + pow(std::abs(*in++), 2);
        total_power = total_power + pow(std::abs(*in++), 2);
        n =  n - 8;
        samples = samples + 8;
        if (samples == 512)
        {
          check_power(total_power, samples);
        }
      }

      while (n > 0)
      {
        total_power = total_power + pow(std::abs(*in++), 2);
        n = n - 1;
        samples = samples + 1;
      }

      check_power(total_power, samples);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace fullduplex */
} /* namespace gr */

