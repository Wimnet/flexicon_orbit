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
    if (argc != 6) {
        printf("Input arguments invalid...\n");
        printf("Please use DAC ([0, 255]), ATT ([0, 127]), CAP1 ([0 31]), CAP2 ([0 31]), CAP3 ([0 31]): \n");
        exit(1);
    }

    int spi_write_val_DAC = atoi(argv[1]);
    int spi_write_val_ATT = atoi(argv[2]);
    int spi_write_val_CAP1 = atoi(argv[3]);
    int spi_write_val_CAP2 = atoi(argv[4]);
    int spi_write_val_CAP3 = atoi(argv[5]);

    // check input
    if (spi_write_val_DAC < 0 || spi_write_val_DAC > 255
        || spi_write_val_ATT < 0 || spi_write_val_ATT > 127) {
            printf("Input arguments out of range: DAC [0, 255], ATT [0, 127].\n");
            exit(1);
        }
    if (spi_write_val_CAP1 < 0 || spi_write_val_CAP1 > 31
        || spi_write_val_CAP2 < 0 || spi_write_val_CAP2 > 31
        || spi_write_val_CAP3 < 0 || spi_write_val_CAP3 > 31) {
            printf("Input arguments out of range: CAP1 [0, 31], CAP2 [0, 31], CAP3 [0, 31].\n");
            exit(1);
        }

    // allocate buffer for the SPI write
    char out_buff_DAC[2];
    char out_buff_ATT[2];
    char out_buff_CAP1, out_buff_CAP2, out_buff_CAP3;
    // 2-byte buffer for writing DAC and ATT
    char* spi_write_buff_char = malloc(2 * sizeof(char));
    // 1-byte buffer for writing CAP1, CAP2, and CAP3
    char* spi_write_buff_char_one_byte = malloc(sizeof(char));

    // DAC @slave 0 (SS0), write MSB first at the falling edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SETUP_SMPL|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_DAC[0] = (char) (spi_write_val_DAC >> 4);
    out_buff_DAC[1] = (char) (spi_write_val_DAC << 4);
    spi_write_buff_char = &out_buff_DAC[0];
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char, 0, 2, SS_CONF(0,SS_LO) );
    if (!rc) {
        printf("...Finished programming DAC with value %d!\n", spi_write_val_DAC);
    }

    // ATT @slave 1 (SS1), write LSB first at the rising edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_LSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_ATT[0] = (char) (spi_write_val_ATT);
    out_buff_ATT[1] = (char) (0);
    spi_write_buff_char = &out_buff_ATT[0];
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char, 0, 2, SS_CONF(1,SS_LO) );
    if (!rc) {
        printf("...Finished programming ATT with value %d!\n", spi_write_val_ATT);
    }

    // CAP1 @slave 2 (SS2), write MSB first at the rising edge of CLK
    sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_1MHZ, 0 );
    out_buff_CAP1 = (char) (spi_write_val_CAP1);
    spi_write_buff_char_one_byte = &out_buff_CAP1;
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(2,SS_HI) );
    if (!rc) {
        printf("...Finished programming CAP1 with value %d!\n", spi_write_val_CAP1);
    }

    // CAP2 @slave 3 (SS3), write MSB first at the rising edge of CLK
    // sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_CAP2 = (char) (spi_write_val_CAP2);
    spi_write_buff_char_one_byte = &out_buff_CAP2;
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(3,SS_HI) );
    if (!rc) {
        printf("...Finished programming CAP2 with value %d!\n", spi_write_val_CAP2);
    }

    // CAP3 @slave 4 (SS4), write MSB first at the rising edge of CLK
    // sub_spi_config( my_sub20, SPI_ENABLE|SPI_CPOL_RISE|SPI_SMPL_SETUP|SPI_MSB_FIRST|SPI_CLK_8MHZ, 0 );
    out_buff_CAP3 = (char) (spi_write_val_CAP3);
    spi_write_buff_char_one_byte = &out_buff_CAP3;
    rc = sub_spi_transfer( my_sub20, spi_write_buff_char_one_byte, 0, 1, SS_CONF(4,SS_HI) );
    if (!rc) {
        printf("...Finished programming CAP3 with value %d!\n", spi_write_val_CAP3);
    }

}
