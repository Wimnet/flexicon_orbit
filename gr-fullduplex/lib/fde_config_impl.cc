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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "fde_config_impl.h"
#include <bitset>

namespace gr {
  namespace fullduplex {

    fde_config::sptr
    fde_config::make(
        int init_cap_addr, int init_cap_val, char* init_sn
    )
    {
      return gnuradio::get_initial_sptr
        (new fde_config_impl(init_cap_addr, init_cap_val, init_sn));
    }

    /*
     * The private constructor
     */
    fde_config_impl::fde_config_impl(
        int init_cap_addr, int init_cap_val, char* init_sn
    )
      : gr::block("fde_config",
              gr::io_signature::make(0, 0, sizeof(int)),
              gr::io_signature::make(0, 0, sizeof(int)))
    {
  my_sub20 = NULL;
        fde_config_find_dev(init_sn);
        // set_canc_path(0);   // default is the 1st FDE tap
        // set_dac(0,0);
        // set_dac(1,0);
        // set_dac(2,0);
        // set_cf_cap(0,0);
        // set_cf_cap(1,0);
        // set_qf_cap(0,0);
        // set_qf_cap(1,0);
        // set_att(0,0);
        // set_att(1,127);
        // set_att(2,127);
        // set_tuner_cap(0,0);
        // set_tuner_cap(1,0);
        // set_tuner_cap(2,0);
        // set_all(0, 127, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    /*
     * Our virtual destructor.
     */
    fde_config_impl::~fde_config_impl()
    {
    }

    void
    fde_config_impl::fde_config_find_dev(char* serial) {
  std::cout << "Serial # input: " << serial << std::endl;
  //if (my_sub20) {
  //  sub_close( my_sub20 );
  //  std::cout << "Closing Sub20..." << std::endl;
  //}
        my_sub20 = NULL;
        int rc = 0;
        int config;
        sub_device dev = 0;

        char sn_buf[20];

        //dev = sub_find_devices( dev );

        while ( dev = sub_find_devices(dev) ) {
      std::cout << dev << std::endl;
      // open the next device
            my_sub20 = sub_open(dev);

      // errno 4 means the sub20 was already opened. If this is the case, skip over it. 
      if (!my_sub20 && sub_errno == 4) {
        std::cout << "Device already open, skipping..." << std::endl;
        continue;
      }
            else if (!my_sub20) {
                std::cout << "ERROR: Cannot open handle to device " << sub_strerror(sub_errno) << std::endl;
            }
      // get the serial number of the device
            rc = sub_get_serial_number( my_sub20, sn_buf, sizeof(sn_buf) );
            if (rc < 0) {
                std::cout << "ERROR: Cannot obtain seral number: " << rc <<std::endl;
            }
            std::cout << "1 SUB20 serial no: " << sn_buf << std::endl;
    
      // if the serial number matches what we want, then we break and open it
      if (!strcmp(serial, sn_buf)) {
        std::cout << sn_buf << " " << serial << " Same" << std::endl;
        break;
      }
      else {
        std::cout << sn_buf << " " << serial << " Not the same" << std::endl;
      }
      // otherwise, close the device and continue while loop
      sub_close( my_sub20 );
        }

    /*
        // open SUB20 device and ger serial number
        my_sub20 = sub_open(dev);
        if (!my_sub20) {
            dev = NULL;
        }
        else {
            std::cout<< "SUB20 device found..." << std::endl;
        }
        rc = sub_get_serial_number( my_sub20, sn_buf, sizeof(sn_buf) );
    */
        std::cout << "2 SUB20 serial no: " << sn_buf << std::endl;
        std::cout << "SUB20 device opened!" << std::endl;

        // disable RS232
        rc = sub_rs_set_config( my_sub20, RS_CHAR_8|RS_PARITY_NONE|RS_STOP_1, 9600 );
        if ( 0 != rc ) { std::cout << "disable RS232 error...!" << std::endl; }

        rc = sub_i2c_config( my_sub20, 0, I2C_DISABLE );
        if ( 0 != rc ) { std::cout << "disable I2C error...!" << std::endl; }

        rc = sub_gpio_config( my_sub20, 0x0000FFFF, &config, 0x0000FFFF );
        if ( 0 != rc ) { std::cout << "GPIO config error...!" << std::endl; }
        // rc = sub_gpio_write( my_sub20, 0x00000000, &config, 0x0000FFFF );
        // if ( 0 != rc ) { std::cout << "GPIO config error...!" << std::endl; }
    }


    // function to configure the RF switch
    void
    fde_config_impl::set_canc_path(int val) {
        d_canc_path_val = val;

        int rc = 0;
        int config;
        if ( 0 == d_canc_path_val ) {
            // Gen-2 path on 0x00008000 (GPIO15 - header pin #8)
            rc = sub_gpio_write( my_sub20, 0x00008000, &config, 0x0000C000 );
            // std::cout << "Gen-2 path selected...!" << std::endl;
        }
        else {
            // Gen-1 path on 0x00004000 (GPIO14 - header pin #7)
            rc = sub_gpio_write( my_sub20, 0x00004000, &config, 0x0000C000 );
            // std::cout << "Gen-1 path selected...!" << std::endl;
        }
        if ( 0 != rc ) { std::cout << "RF SW config error...!" << std::endl; }
        usleep(1000);
    }

    // function to configure attenuator
    void
    fde_config_impl::set_att(int addr, int val) {
        d_att_addr  = addr;
        d_att_val[d_att_addr] = val;

        int config;
        int rc = 0;

        sub_spi_config( my_sub20, 0, 0 );
        rc = sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_LSB_FIRST|SPI_CLK_8MHZ, 0 );
        if ( 0 != rc ) {
            std::cout << "Attenuator SPI config error...!" << std::endl;
        }

        // allocate buffer for the SPI write
        char out_buff[2];
        char* spi_write_buff = ( char* ) malloc(2 * sizeof(char));

        out_buff[0] = (char) (d_att_val[d_att_addr]);
        out_buff[1] = (char) (d_att_addr);
        spi_write_buff = &out_buff[0];

        // SS_CONF(0,SS_LLH) should also work
        rc = sub_spi_transfer( my_sub20, spi_write_buff, 0, 2, SS_CONF(0,SS_LO) );
        if ( 0 != rc ) {
          std::cout << "Attenuator " << d_att_addr << " SPI transfer error...!" << std::endl;
        }
    }

    // function to configure DAC
    void
    fde_config_impl::set_dac(int addr, int val) {
        d_dac_addr  = addr;
        d_dac_val[d_dac_addr] = val;

        int config;
        int rc = 0;
        rc = sub_gpio_config( my_sub20, 0x00000100<<(2*d_dac_addr), &config, 0x00000100<<(2*d_dac_addr) );
        if ( 0 != rc ) {
            std::cout << "DAC " << d_dac_addr << " GPIO config error...!" << std::endl;
        }

        // SPI config: write MSB first at the falling edge of CLK
        // sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_FALL|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_4MHZ, 0 );
        sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SETUP_SMPL|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );

        // write 2-byte work to DAC
        char out_buff[2];
        char* spi_write_buff = ( char* ) malloc(2 * sizeof(char));
        out_buff[0] = (char) (d_dac_val[d_dac_addr] >> 4);
        out_buff[1] = (char) (d_dac_val[d_dac_addr] << 4);
        spi_write_buff = &out_buff[0];
        // pull ~SYNC to low
        rc = sub_gpio_write( my_sub20, 0x00000000, &config, 0x00000100<<(2*d_dac_addr) );
        if ( 0 != rc ) {
            std::cout << "DAC " << d_dac_addr << " GPIO write error...!" << std::endl;
        }
        // working version
        // rc = sub_spi_transfer( my_sub20, spi_write_buff, 0, 2, SS_CONF(1,SS_LLH));
        rc = sub_spi_transfer( my_sub20, spi_write_buff, 0, 2, 0);
        if ( 0 != rc ) {
            std::cout << "DAC " << d_dac_addr << " SPI transfer error...!" << std::endl;
        }
        if ( 0 != sub_gpio_write( my_sub20, 0x00000100<<(2*d_dac_addr), &config, 0x00000100<<(2*d_dac_addr) ) ) {
            std::cout << "DAC " << d_dac_addr << " GPIO write error...!" << std::endl;
        }
        // std::cout << "DAC " << d_dac_addr << " programmed with value " << val << "...!" << std::endl;
    }

    // function to configure center frequency cap
    void
    fde_config_impl::set_cf_cap(int addr, int val) {
        d_cf_cap_addr = addr;
        d_cf_cap_val[d_cf_cap_addr] = val;

        int rc = 0;
        // SPI config: write MSB first at the falling edge of CLK
        rc = sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
        if ( 0 != rc ) {
            std::cout << "CF Cap " << d_cf_cap_addr << " SPI config error...!" << std::endl;
        }
        // write 1-byte word on SPI SS1/2
        char out_buff;
        char* spi_write_buff = ( char* ) malloc(sizeof(char));
        out_buff = (char) (d_cf_cap_val[d_cf_cap_addr]);
        spi_write_buff = &out_buff;
        rc = sub_spi_transfer( my_sub20, spi_write_buff, 0, 1, SS_CONF(1+d_cf_cap_addr,SS_HL) );
        if ( 0 != rc ) {
            std::cout << "CF Cap " << d_cf_cap_addr << " SPI transfer error...!" << std::endl;
        }
    }

    // function to configure quality factor cap
    void
    fde_config_impl::set_qf_cap(int addr, int val) {
        d_qf_cap_addr = addr;
        d_qf_cap_val[d_qf_cap_addr] = val;

        int rc = 0;
        // SPI config: write MSB first at the falling edge of CLK
        if ( 0 != sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 ) ) {
            std::cout << "QF Cap " << d_qf_cap_addr << " SPI config error...!" << std::endl;
        }
        // write 1-byte word on SPI SS3/4
        char out_buff;
        char* spi_write_buff = ( char* ) malloc(sizeof(char));
        out_buff = (char) (d_qf_cap_val[d_qf_cap_addr]);
        spi_write_buff = &out_buff;
        rc = sub_spi_transfer( my_sub20, spi_write_buff, 0, 1, SS_CONF(3+d_qf_cap_addr,SS_HL) );
        if ( 0 != rc ) {
            std::cout << "QF Cap " << d_qf_cap_addr << " SPI transfer error...!" << std::endl;
        }
    }

    // function to configure the tuner capacitors
    void
    fde_config_impl::set_tuner_cap(int addr, int val) {
        d_tuner_cap_addr = addr;
        d_tuner_cap_val[d_tuner_cap_addr]  = val;

        if ( 0 != sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 ) ) {
            std::cout << "Tuner cap " << d_tuner_cap_addr << " SPI config error...!" << std::endl;
        }

        int config;
        // if ( 0 != sub_gpio_config( my_sub20, 0x00000200 << 2*d_tuner_cap_addr, &config, 0x00000200 << 2*d_tuner_cap_addr ) ) {
        //     std::cout << "Tuner cap " << d_tuner_cap_addr << " GPIO config error...!" << std::endl;
        // }

        // allocate buffer for the SPI write
        char out_buff;
        // 1-byte buffer for writing CAP1, CAP2, and CAP3
        char* spi_write_buff = ( char* ) malloc(sizeof(char));
        out_buff = (char) (d_tuner_cap_val[d_tuner_cap_addr]);
        spi_write_buff = &out_buff;

        if ( 0 != sub_gpio_write( my_sub20, 0x00000000, &config, 0x00000200 << 2*d_tuner_cap_addr ) ) {
            std::cout << "Tuner cap " << d_tuner_cap_addr << " GPIO write error...!" << std::endl;
        }
        if ( 0 != sub_gpio_write( my_sub20, 0x00000200 << 2*d_tuner_cap_addr, &config, 0x00000200 << 2*d_tuner_cap_addr ) ) {
            std::cout << "Tuner cap " << d_tuner_cap_addr << " GPIO write error...!" << std::endl;
        }
        if ( 0 != sub_spi_transfer( my_sub20, spi_write_buff, 0, 1, 0 ) ) {
            std::cout << "Tuner cap " << d_tuner_cap_addr << " SPI write error...!" << std::endl;
        }
        if ( 0 != sub_gpio_write( my_sub20, 0x00000000, &config, 0x00000200 << 2*d_tuner_cap_addr ) ) {
            std::cout << "Tuner cap " << d_tuner_cap_addr << " GPIO write error...!" << std::endl;
        }
    }


    // set all
    /*
    void
    fde_config_impl::set_all() {
      for ( int idx_att = 0; idx_att < 3; idx_att++ ) {
        set_att(idx_att, d_att_val[idx_att]);
      }

      for ( int idx_cf_cap = 0; idx_cf_cap < 2; idx_cf_cap++ ) {
        set_cf_cap(idx_cf_cap, d_cf_cap_val[idx_cf_cap]);
      }

      for ( int idx_qf_cap = 0; idx_qf_cap < 2; idx_qf_cap++ ) {
        set_qf_cap(idx_qf_cap, d_qf_cap_val[idx_qf_cap]);
      }

      for ( int idx_dac = 0; idx_dac < 3; idx_dac++ ) {
        set_dac(idx_dac, d_dac_val[idx_dac]);
      }

      set_canc_path(d_canc_path_val);

      for ( int idx_tuner_cap = 0; idx_tuner_cap < 3; idx_tuner_cap++ ) {
        set_tuner_cap(idx_tuner_cap, d_tuner_cap_val[idx_tuner_cap]);
      }
    }
    */

    void
    fde_config_impl::set_all(int att_0_val, int att_1_val, int att_2_val,
                             int cf_cap_0_val, int cf_cap_1_val, int qf_cap_0_val, int qf_cap_1_val,
                             int dac_0_val, int dac_1_val, int dac_2_val, int canc_path_val,
                             int tuner_cap_0_val, int tuner_cap_1_val, int tuner_cap_2_val) {

      set_tuner_cap(0, tuner_cap_0_val);
      set_tuner_cap(1, tuner_cap_1_val);
      set_tuner_cap(2, tuner_cap_2_val);

      set_dac(0, dac_0_val);
      set_dac(1, dac_1_val);
      set_dac(2, dac_2_val);

      set_cf_cap(0, cf_cap_0_val);
      set_cf_cap(1, cf_cap_1_val);

      set_qf_cap(0, qf_cap_0_val);
      set_qf_cap(1, qf_cap_1_val);

      set_att(0, att_0_val);
      set_att(1, att_1_val);
      set_att(2, att_2_val);

      set_canc_path(canc_path_val);
      //


      /*
      for ( int idx_cf_cap = 0; idx_cf_cap < 2; idx_cf_cap++ ) {
        set_cf_cap(idx_cf_cap, d_cf_cap_val[idx_cf_cap]);
      }

      for ( int idx_qf_cap = 0; idx_qf_cap < 2; idx_qf_cap++ ) {
        set_qf_cap(idx_qf_cap, d_qf_cap_val[idx_qf_cap]);
      }

      for ( int idx_dac = 0; idx_dac < 3; idx_dac++ ) {
        set_dac(idx_dac, d_dac_val[idx_dac]);
      }

      set_canc_path(d_canc_path_val);

      for ( int idx_tuner_cap = 0; idx_tuner_cap < 3; idx_tuner_cap++ ) {
        set_tuner_cap(idx_tuner_cap, d_tuner_cap_val[idx_tuner_cap]);
      }
      */
    }


    // gnuradio forcast function
    void
    fde_config_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
        ninput_items_required[0] = noutput_items;
    }

    int
    fde_config_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        int *in = (int *) input_items[0];
        int *out = (int *) output_items[0];

        // Do <+signal processing+>
        // Tell runtime system how many input items we consumed on
        // each input stream.
        // consume_each (noutput_items);
        for (int i = 0; i < noutput_items; i++) {
            out[i] = in[i] * 2;
        }

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace fullduplex */
} /* namespace gr */

