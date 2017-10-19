/*
 * Demo application for libsub header file
 * Copyright (C) 2008-2009 Alex Kholodenko <sub20@xdimax.net>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
*-------------------------------------------------------------------
* Version: $Id: sub_app.h,v 1.54 2012-01-21 16:04:32 avr32 Exp $
*-------------------------------------------------------------------
*/
#ifndef SUB_APP_H_INCLUDED
#define SUB_APP_H_INCLUDED


/*
*------------------------------------------------------------------------------
*                               types
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
*                               defines
*------------------------------------------------------------------------------
*/

/* Version */
#define VERSION_STR     "XDimax LTD, V%s, %s %s\n"
#define VERSION_INFO    "0.2.3"
#define APP_NAME        "sub_app"
#define XPRINT_VERSION do{printf( VERSION_STR, VERSION_INFO, __DATE__, __TIME__ );}while(0);

  
/*
* Usage Descriptor
*/
option_descriptor_t usage_descriptor[]={

    /* Next free 33 */

    {   
        'h', "help", "", 
        "Print this message and exit",
#define HELP_CMD_CASE           0
        HELP_CMD_CASE
    },

    {
        ' ', "version", "",
        "Print version and exit",
#define VER_CMD_CASE            1
        VER_CMD_CASE
    },

    {
        'c', "cmd", "HEX,RESP_SZ",
        "Send command HEX_STR and get response",
#define CMD_CMD_CASE            2
        CMD_CMD_CASE
    },

    {
        ' ', "i2c_stop", "",
        "I2C Stop",
#define STOP_CMD_CASE           3
        STOP_CMD_CASE
    },
    {
        ' ', "i2c_start", "",
        "I2C Start",
#define START_CMD_CASE          4
        START_CMD_CASE
    },

    {
        'i', "i2c", "[H]xSA,[xMA_SZ,xMA,]dSZ,[HEX]",
        "[H-HighSpeed] I2C Read/Write",
#define I2C_CMD_CASE            5
        I2C_CMD_CASE
    },
    {
        ' ', "i2c_cfg", "CONFIG",
        "I2C config \n\tf<FREQ>|s<SA>|m<MODE>|c<STRTCH_MS>|\n\td-Dis|0-get",
#define I2C_CFG_CMD_CASE        7
        I2C_CFG_CMD_CASE
    },

    {
        ' ', "i2c_scan", "",
        "I2C Scan",
#define SCAN_CMD_CASE           8
        SCAN_CMD_CASE
    },

    {
        ' ', "i2c_ch", "I2C_CHANNEL",
        "Bit-bang I2C channel (0-3)",
#define I2C_CH_CMD_CASE         30
        I2C_CH_CMD_CASE
    },

    {
        ' ', "spi_cfg", "SPI_CFG",
        "e|s|p|t|r|f|l|m|1|2|4|8|5|6|7|H(F_CLK)|g-get",
#define SPI_CFG_CMD_CASE        11
        SPI_CFG_CMD_CASE
    },

    {
        ' ', "ss", "SSn,SSmode|E<ess>",
        "SPI SS-[0..4],[h,h..l,hi,l,l..h,lo,hiz,0]",
#define SPI_SS_CMD_CASE        13
        SPI_SS_CMD_CASE
    },

    {
        ' ', "spi", "[I]dSZ,[HEX]",
        "SPI Transfer. I-SDIO. Receive dSZ bytes, send HEX",
#define SPI_CMD_CASE            12
        SPI_CMD_CASE
    },

    {
        ' ', "gpio", "FUNK,VAL,MASK",
        "GPIO[B]/[E]dge operation [B|E]<r|w|c>[,value,mask]",
#define GPIO_CMD_CASE           16
        GPIO_CMD_CASE
    },

    {
        'r', "repeat", "N_REPEAT",
        "Repeat last operation",
#define RPT_CMD_CASE            6
        RPT_CMD_CASE
    },

    {
        ' ', "getsn", "",
        "Get setial number",
#define GETSN_CMD_CASE          9
        GETSN_CMD_CASE
    },
    {
        ' ', "getid", "",
        "Get product ID",
#define GETID_CMD_CASE          10
        GETID_CMD_CASE
    },
    {
        ' ', "getvpd", "",
        "Get Configuration VPD",
#define GETVPD_CMD_CASE         17
        GETVPD_CMD_CASE
    },
    {
        ' ', "boot", "",
        "Enter Bootloader Mode",
#define BOOT_CMD_CASE           14
        BOOT_CMD_CASE
    },
    {
        ' ', "rst", "",
        "Reset",
#define RST_CMD_CASE            32
        RST_CMD_CASE
    },

    /* FIFO */    
    {
        ' ', "fifo", "dSZ[,HEX]",
        "FIFO Read/Write",
#define FIFO_CMD_CASE           18
        FIFO_CMD_CASE
    },

    {
        ' ', "fifo_cfg", "FIFO_CONF",
        "Combination of s-SPI|i-I2C|u-UATR|c-clear",
#define FIFO_CFG_CMD_CASE       23
        FIFO_CFG_CMD_CASE
    },

    /* LCD */
    {
        ' ', "LCD", "LCD_STR",
        "Write on LCD",
#define LCDWR_CMD_CASE          19
        LCDWR_CMD_CASE
    },

    /* RS232/RS485 */
    {
        ' ', "rs_cfg", "RS_CONFIG",
        "RS Config <BAUD>_<SZ><PAR><STOP> like 9600_8N1 or 0",
#define RS_CFG_CMD_CASE         20
        RS_CFG_CMD_CASE
    },

    {
        ' ', "rs_tmg", "RS_TIMING",
        "RS Timing [b|a,]<TX_SPACE_US>,<RX_MSG_US>,<RX_BYTE_US>",
#define RS_TMG_CMD_CASE         21
        RS_TMG_CMD_CASE
    },

    {
        ' ', "rsx", "dSZ,[HEX]",
        "RS Xfer. Receive dSZ bytes, transmit [HEX]",
#define RS_XFER_CMD_CASE        22
        RS_XFER_CMD_CASE
    },

    {   
        ' ', "fpwm", "F_FREQ|N_SET",
        "F<freq_hz|0> | <N>,<duty>",
#define FPWM_CMD_CASE           24
        FPWM_CMD_CASE
    },

    {   
        ' ', "pwm", "CONFIG|N_SET",
        "C<res>,<limit> | <N>,<duty>",
#define PWM_CMD_CASE           27
        PWM_CMD_CASE
    },

    {
        ' ', "adc_cfg", "ADC_CONFIG",
        "ADC Config e-enable|v-VCC|2-2.56V",
#define ADC_CFG_CMD_CASE        25
        ADC_CFG_CMD_CASE
    },        
    {
        ' ', "adc", "ADC_MUX",
        "ADC Read",
#define ADC_CMD_CASE            26
        ADC_CMD_CASE
    },        

    {
        ' ', "mdio", "[1]<R|W|a|w|r|p>,<PRT_PHY>,<DEV_REG>,[DATA]",
        "1-Ch1 R,W-Clause22",
#define MDIO_CMD_CASE           28
        MDIO_CMD_CASE
    },        

	{
        ' ', "serno", "<SN>",
        "Select device by serial number",
#define SERNO_CMD_CASE           29
        SERNO_CMD_CASE
    },        

	{
        ' ', "eep", "dSZ,xADDR,[HEX]",
        "EEP Read/Write if HEX",
#define EEP_CMD_CASE             31
        EEP_CMD_CASE
    },        
};

/*
*------------------------------------------------------------------------------
*                               Global Variables
*------------------------------------------------------------------------------
*/
struct 
{
    int     usb_to_ms;      /* Default timeout */

#define MAX_CMD 256
    char    cmd[MAX_CMD];
    int     cmd_sz;
    int     resp_sz;

    char    todo[2000];
    int     todo_sz;

    int     sa;             /* Slave Address */
    sub_int32_t     ma;             /* Memory Address */
    int     ma_sz;
    sub_int32_t     i2c_freq;
    int     i2c_sa;
    int     i2c_dis;        /* Disable */
    int     bb_i2c_ch;      /* Bit-bang I2C channel */
    int     bb_i2c_mode;
    int     bb_i2c_stretch;
    /* HS I2C */
    int     mcode;
    int     hs_n;
    struct sub_i2c_hs_xfer hs_xfer[20];

    int     spi_config;
    int     spi_clk_low;
    int     ss;
    int     spi_out_sz;
    int     spi_in_sz;
    char    spi_ess[300];
#if _MSC_VER == 800
    char    buf[0x1000];
#else
    char    buf[0x100000];
#endif
    int     sz;

    sub_int32_t		gpio_val;
    int     gpio_mask;
    int     gpiob_val;
    int     gpiob_mask;
    int     edge_config;

    double  fpwm_freq;
    double  fpwm_duty[3];
    int     fpwm_flags;

    int     pwm_res;
    int     pwm_limit;
    int     pwm_duty[8];    

    int     adc_cfg;
    int     adc_reads;
    int     adc_mux[30];

    sub_int32_t     rs_baud;
    int     rs_config;
    int     rs_flags;
    int     rs_tx_space;
    int     rs_rx_msg_to;
    int     rs_rx_byte_to;
    int     rs_tx_sz;
    int     rs_rx_sz;

    char*   lcd_str;

    int     fifo_cfg;

    int     mdio_n;
    int     mdio_channel;
    union   sub_mdio_frame mdios[20];

	int		serno;

    /* EEP */
    int     eep_sz;
    int     eep_addr;
    char    eep_buf[64];
}config = 
{
    10000,      /* usb_to_ms */
	"",         /* cmd */
    0,          /* cmd_sz */
    0,          /* resp_sz */
	"",         /* todo */
    0,          /* todo_sz */
};


/*
*------------------------------------------------------------------------------
*                               MACRO
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
*                               Functions
*------------------------------------------------------------------------------
*/

#endif /*H_INCLUDED*/

