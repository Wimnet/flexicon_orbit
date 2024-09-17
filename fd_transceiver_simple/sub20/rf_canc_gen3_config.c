//Version2 all the attnuator pins are written parallely
//using a single write operation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libsub.h"
#include "cmd_pars.h"
#include "sub_app.h"
#include <unistd.h>

#if defined(_MSC_VER)
#define strcasecmp _stricmp
#endif

// define global sub20 handle
sub_handle my_sub20 = NULL ;

int main(int argc, char *argv[])
{
    int rc = 0;
    sub_int32_t tmp1;
    sub_device dev = 0;

    // buffer for SUB20 serial number
    char ser_num_buff[20];
    int found = 0 ;

    printf("\nStarted...");
    dev = sub_find_devices( dev );

    // open SUB20 device
    my_sub20 = sub_open(dev);
    if (!my_sub20) {
        dev = NULL; exit(1);
        printf("\nERROR: Cannot open handle to SUB20 device %s \n", sub_strerror(sub_errno));
        exit(1);
    }
    else {
        rc = sub_get_serial_number( my_sub20, ser_num_buff, sizeof(ser_num_buff) );
        printf("\nSub20 device found... Serial Number is %s\n", ser_num_buff);
    }
    printf("Device Opened!\n");

    // prepare data to write
    if (argc != 11) {
        printf("Input arguments invalid...\n");
        printf("Please use ATT0 ([0, 127]), ATT1 ([0, 127]), ATT2 ([0, 127]), DAC0 ([0, 255]), DAC1 ([0, 255]), DAC2 ([0, 255]), CF0 ([0 31]), CF1 ([0 31]), QF0 ([0 31]), QF1 ([0 31]): \n");
        exit(1);
    }

    int spi_write_val_ATT0 = atoi(argv[1]);
    int spi_write_val_ATT1 = atoi(argv[2]);
    int spi_write_val_ATT2 = atoi(argv[3]);
    int spi_write_val_DAC0 = atoi(argv[4]);
    int spi_write_val_DAC1 = atoi(argv[5]);
    int spi_write_val_DAC2 = atoi(argv[6]);
    int spi_write_val_CF0 = atoi(argv[7]);
    int spi_write_val_CF1 = atoi(argv[8]);
    int spi_write_val_QF0 = atoi(argv[9]);
    int spi_write_val_QF1 = atoi(argv[10]);

    // check input
    if (spi_write_val_ATT0 < 0 || spi_write_val_ATT0 > 127
        || spi_write_val_ATT1 < 0 || spi_write_val_ATT1 > 127
        || spi_write_val_ATT2 < 0 || spi_write_val_ATT2 > 127) {
            printf("Input arguments out of range: ATT0 [0, 127], ATT1 [0, 127], ATT2 [0, 127].\n");
            exit(1);
        }
    if (spi_write_val_DAC0 < 0 || spi_write_val_DAC0 > 255
        || spi_write_val_DAC1 < 0 || spi_write_val_DAC1 > 255
        || spi_write_val_DAC2 < 0 || spi_write_val_DAC2 > 255) {
            printf("Input arguments out of range: DAC0 [0, 255], DAC1 [0, 255], DAC2 [0, 255].\n");
            exit(1);
        }
    if (spi_write_val_CF0 < 0 || spi_write_val_CF0 > 31
        || spi_write_val_CF1 < 0 || spi_write_val_CF1 > 31) {
            printf("Input arguments out of range: CF0 [0, 31], CF1 [0, 31].\n");
            exit(1);
        }
    if (spi_write_val_QF0 < 0 || spi_write_val_QF0 > 31
        || spi_write_val_QF1 < 0 || spi_write_val_QF1 > 31) {
            printf("Input arguments out of range: QF0 [0, 31], QF1 [0, 31].\n");
            exit(1);
        }

    // allocate buffer for the SPI write
    char out_buff_ATT0[2];
    char out_buff_ATT1[2];
    char out_buff_ATT2[2];
    char out_buff_DAC0[2];
    char out_buff_DAC1[2];
    char out_buff_DAC2[2];
    char out_buff_CF0, out_buff_CF1, out_buff_QF0, out_buff_QF1;
    // 2-byte buffer for writing DAC and ATT
    char* spi_write_buff_char = malloc(2 * sizeof(char));
    // 1-byte buffer for writing CAP1, CAP2, and CAP3
    char* spi_write_buff_char_one_byte = malloc(sizeof(char));

    // ATT0 @slave 0 (SS0), write LSB first at the rising edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_LSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_ATT0[0] = (char) (spi_write_val_ATT0);
    out_buff_ATT0[1] = (char) (0);
    spi_write_buff_char = &out_buff_ATT0[0];
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char, 0, 2, SS_CONF(1,SS_LO) );
    if (!rc) {
        printf("...Finished programming ATT0 with value %d!\n", spi_write_val_ATT0);
    }

    // ATT1 @slave 1 (SS1), write LSB first at the rising edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_LSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_ATT1[0] = (char) (spi_write_val_ATT1);
    out_buff_ATT1[1] = (char) (0);
    spi_write_buff_char = &out_buff_ATT1[0];
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char, 0, 2, SS_CONF(1,SS_LO) );
    if (!rc) {
        printf("...Finished programming ATT1 with value %d!\n", spi_write_val_ATT1);
    }

    // ATT2 @slave 2 (SS2), write LSB first at the rising edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_LSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_ATT2[0] = (char) (spi_write_val_ATT2);
    out_buff_ATT2[1] = (char) (0);
    spi_write_buff_char = &out_buff_ATT2[0];
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char, 0, 2, SS_CONF(1,SS_LO) );
    if (!rc) {
        printf("...Finished programming ATT2 with value %d!\n", spi_write_val_ATT2);
    }

    // DAC0 @slave 3 (SS3), write MSB first at the falling edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SETUP_SMPL|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_DAC0[0] = (char) (spi_write_val_DAC0 >> 4);
    out_buff_DAC0[1] = (char) (spi_write_val_DAC0 << 4);
    spi_write_buff_char = &out_buff_DAC0[0];
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char, 0, 2, SS_CONF(0,SS_LO) );
    if (!rc) {
        printf("...Finished programming DAC0 with value %d!\n", spi_write_val_DAC0);
    }

    // DAC1 @slave 4 (SS4), write MSB first at the falling edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SETUP_SMPL|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_DAC1[0] = (char) (spi_write_val_DAC1 >> 4);
    out_buff_DAC1[1] = (char) (spi_write_val_DAC1 << 4);
    spi_write_buff_char = &out_buff_DAC1[0];
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char, 0, 2, SS_CONF(0,SS_LO) );
    if (!rc) {
        printf("...Finished programming DAC1 with value %d!\n", spi_write_val_DAC1);
    }

    // DAC2 @slave 5 (SS5), write MSB first at the falling edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SETUP_SMPL|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_DAC2[0] = (char) (spi_write_val_DAC2 >> 4);
    out_buff_DAC2[1] = (char) (spi_write_val_DAC2 << 4);
    spi_write_buff_char = &out_buff_DAC2[0];
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char, 0, 2, SS_CONF(0,SS_LO) );
    if (!rc) {
        printf("...Finished programming DAC2 with value %d!\n", spi_write_val_DAC2);
    }

    // CF0 @slave 6 (SS6), write MSB first at the rising edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_1MHZ, 0 );
    out_buff_CF0 = (char) (spi_write_val_CF0);
    spi_write_buff_char_one_byte = &out_buff_CF0;
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(2,SS_HI) );
    if (!rc) {
        printf("...Finished programming CF0 with value %d!\n", spi_write_val_CF0);
    }

    // CF1 @slave 7 (SS7), write MSB first at the rising edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_1MHZ, 0 );
    out_buff_CF1 = (char) (spi_write_val_CF1);
    spi_write_buff_char_one_byte = &out_buff_CF1;
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(2,SS_HI) );
    if (!rc) {
        printf("...Finished programming CF1 with value %d!\n", spi_write_val_CF1);
    }

    // QF0 @slave 8 (SS6), write MSB first at the rising edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_1MHZ, 0 );
    out_buff_QF0 = (char) (spi_write_val_QF0);
    spi_write_buff_char_one_byte = &out_buff_QF0;
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(2,SS_HI) );
    if (!rc) {
        printf("...Finished programming QF0 with value %d!\n", spi_write_val_QF0);
    }

    // QF1 @slave 9 (SS7), write MSB first at the rising edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_1MHZ, 0 );
    out_buff_QF1 = (char) (spi_write_val_QF1);
    spi_write_buff_char_one_byte = &out_buff_QF1;
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(2,SS_HI) );
    if (!rc) {	
        printf("...Finished programming QF1 with value %d!\n", spi_write_val_QF1);
    }

}
