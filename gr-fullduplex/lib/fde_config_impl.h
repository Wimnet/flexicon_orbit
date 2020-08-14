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

#include <libsub.h>

#ifndef INCLUDED_FULLDUPLEX_FDE_CONFIG_IMPL_H
#define INCLUDED_FULLDUPLEX_FDE_CONFIG_IMPL_H

#include <fullduplex/fde_config.h>
// #include <subdev/sub20_init.h>

namespace gr {
  namespace fullduplex {

    class fde_config_impl : public fde_config
    {
     private:
      // Nothing to declare in this block.

     public:
      fde_config_impl(int init_cap_addr, int init_cap_val, char* init_sn);
      ~fde_config_impl();

      // initialize sub20 device using libusb-1.0
      void fde_config_find_dev(char* serial);

      void set_canc_path(int val);

      void set_att(int addr, int val);
      void set_dac(int addr, int val);

      void set_cf_cap(int addr, int val);
      void set_qf_cap(int addr, int val);

      void set_tuner_cap(int addr, int val);

      void set_all(int att_0_val, int att_1_val, int att_2_val,
                   int cf_cap_0_val, int cf_cap_1_val, int qf_cap_0_val, int qf_cap_1_val,
                   int dac_0_val, int dac_1_val, int dac_2_val, int canc_path_val,
                   int tuner_cap_0_val, int tuner_cap_1_val, int tuner_cap_2_val);

      // sub20 handle
      sub_handle my_sub20;

      int d_canc_path_val;

      // callback params
      int d_att_addr;
      int d_att_val[3] = {127, 127, 127};
      int d_dac_addr;
      int d_dac_val[3] = {  0,   0,   0};

      int d_cf_cap_addr;
      char d_cf_cap_val[2] = {0, 0};

      int d_qf_cap_addr;
      char d_qf_cap_val[2] = {0, 0};

      int d_tuner_cap_addr;
      int d_tuner_cap_val[3] = {0, 0, 0};

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace fullduplex
} // namespace gr

#endif /* INCLUDED_FULLDUPLEX_FDE_CONFIG_IMPL_H */


