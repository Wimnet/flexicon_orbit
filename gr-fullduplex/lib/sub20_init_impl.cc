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
#include "sub20_init_impl.h"

namespace gr {
  namespace fullduplex {

    sub20_init::sptr
        sub20_init::make(
            int atten_code, int phase_code
        )
        {
            return gnuradio::get_initial_sptr
            (new sub20_init_impl(atten_code, phase_code));
        }

        /*
        * The private constructor
        */
        sub20_init_impl::sub20_init_impl(
            int atten_code, int phase_code
        )
        : gr::block("sub20_init",
        gr::io_signature::make(0, 0, sizeof(int)),
        gr::io_signature::make(0, 0, sizeof(int)))
        {
            sub20_init_find_dev();
            set_atten_code(32);
            set_phase_code(120);
            set_tuner();
        }

        /*
        * Our virtual destructor.
        */
        sub20_init_impl::~sub20_init_impl()
        {
        }

        void
        sub20_init_impl::sub20_init_find_dev() {
            my_sub20 = NULL;
            int rc = 0;
            sub_device dev = 0;

            char sn_buf[20];
            int found = 0 ;

            dev = sub_find_devices( dev );

            while ( dev = sub_find_devices(dev) ) {
                my_sub20 = sub_open(dev);

                if (!my_sub20) {
                    std::cout<< "ERROR: Cannot open handle to device " << sub_strerror(sub_errno) << std::endl;
                }

                rc = sub_get_serial_number( my_sub20, sn_buf, sizeof(sn_buf) );
                std::cout << "SUB20 serial no: " << sn_buf << std::endl;
                sub_close( my_sub20 );

                if (rc < 0) {
                    std::cout << "ERROR: Cannot obtain seral number: " << rc <<std::endl;
                }
                found = 1;
            }

            // open SUB20 device and ger serial number
            my_sub20 = sub_open(dev);
            if (!my_sub20) {
                dev = NULL;
            }
            else {
                std::cout<< "SUB20 device found..." << std::endl;
            }
            rc = sub_get_serial_number( my_sub20, sn_buf, sizeof(sn_buf) );
            std::cout << "SUB20 serial no: " << sn_buf << std::endl;
            std::cout << "SUB20 device opened!" << std::endl;
        }

        // function to configure the attenuator
        void
        sub20_init_impl::set_atten_code(int code) {
            d_atten_code = code;

            if (d_atten_code < 0 || d_atten_code > 127) {
                std::cout << "Atten code out of range [0, 127]" << std::endl;
                exit(1);
            }
            else {
                std::cout << "Setting attenuation to code " << d_atten_code << "..." << std::endl;
            }

            int rc = 0;
            // ATT @slave 1 (SS1), write LSB first at the rising edge of CLK
            sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_LSB_FIRST|SPI_CLK_8MHZ, 0 );
            // allocate buffer for the SPI write
            char out_buff[2];
            char* spi_write_buff = ( char* ) malloc(2 * sizeof(char));
            out_buff[0] = (char) (d_atten_code);
            out_buff[1] = (char) (0);
            spi_write_buff = &out_buff[0];
            // write SPI @slave 1 (SS1)
            rc = sub_spi_transfer( my_sub20, spi_write_buff, 0, 2, SS_CONF(1,SS_LO) );
            if (!rc) {
                std::cout << "Atten code set to " << d_atten_code << std::endl << std::endl;
            }
            usleep(1000);
        }

        // function to configure the phase shifter
        void
        sub20_init_impl::set_phase_code(int code) {
            d_phase_code = code;

            if (d_phase_code < 0 || d_phase_code > 255) {
                std::cout << "Phase code out of range [0, 255]" << std::endl;
                exit(1);
            }
            else {
                std::cout << "Setting phase to code " << d_phase_code << "..." << std::endl;
            }

            int rc = 0;
            // DAC @slave 0 (SS0), write MSB first at the falling edge of CLK
            sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SETUP_SMPL|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
            // allocate buffer for the SPI write
            char out_buff[2];
            char* spi_write_buff = ( char* ) malloc(2 * sizeof(char));
            out_buff[0] = (char) (d_phase_code >> 4);
            out_buff[1] = (char) (d_phase_code << 4);
            spi_write_buff = &out_buff[0];
            // write SPI @slave 0 (SS0)
            rc = sub_spi_transfer( my_sub20, spi_write_buff, 0, 2, SS_CONF(0,SS_LO) );
            if (!rc) {
                std::cout << "Phase code set to " << d_phase_code << std::endl << std::endl;
            }
            usleep(1000);
        }

        // function to configure the capacitors
        void
        sub20_init_impl::set_tuner() {

            int rc = 0;

      char out_buff_CAP1, out_buff_CAP2, out_buff_CAP3;
      // 1-byte buffer for writing CAP1, CAP2, and CAP3
      char* spi_write_buff_char_one_byte = ( char* ) malloc(sizeof(char));

            // CAP1 @slave 2 (SS2), write MSB first at the rising edge of CLK
      sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_1MHZ, 0 );
      out_buff_CAP1 = (char) (16);
      spi_write_buff_char_one_byte = &out_buff_CAP1;
      rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(2,SS_HI) );
            usleep(1000);

      // CAP2 @slave 3 (SS3), write MSB first at the rising edge of CLK
      // sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
      out_buff_CAP2 = (char) (6);
      spi_write_buff_char_one_byte = &out_buff_CAP2;
      rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(3,SS_HI) );

      // CAP3 @slave 4 (SS4), write MSB first at the rising edge of CLK
      // sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
      out_buff_CAP3 = (char) (6);
      spi_write_buff_char_one_byte = &out_buff_CAP3;
      rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(4,SS_HI) );
        }

        // gnuradio forcast function
        void
        sub20_init_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
        {
            /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
            ninput_items_required[0] = noutput_items;
        }

        int
        sub20_init_impl::general_work (int noutput_items,
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

