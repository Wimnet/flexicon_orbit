/*
 * Copyright (C) 2014 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef INCLUDED_FULLDUPLEX_PACKET_ENCAP2_H
#define INCLUDED_FULLDUPLEX_PACKET_ENCAP2_H

#include <fullduplex/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace fullduplex {

class FULLDUPLEX_API packet_encap2 : virtual public gr::tagged_stream_block
{
public:

  typedef boost::shared_ptr<packet_encap2> sptr;
  static sptr make(bool debug = false,
                   bool delay=false,
                   double delay_sec=0.01,
                   unsigned int pad_front = 0,
                   unsigned int pad_tail = 0,
                   unsigned int sig_pilot_pos = 0,
				   double premultiplier = 1,
				   unsigned int sync_word = 1);

protected:
  int d_sig_pilot_duplicates;
  gr_complex d_sig_pilot[2*80];
  gr::thread::mutex d_mutex;

};

}  // namespace fullduplex
}  // namespace gr

#endif /* INCLUDED_FULLDUPLEX_PACKET_ENCAP2_H */
