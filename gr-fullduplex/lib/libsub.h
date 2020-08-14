/*
 * Public libsub header file
 * Copyright (C) 2008-2009 Alex Kholodenko <sub20@xdimax.net>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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
* $Id: libsub.h,v 1.82 2013-09-12 04:38:26 avr32 Exp $
*-------------------------------------------------------------------
*/
#ifndef SUB_H_INCLUDED
#define SUB_H_INCLUDED 

#ifdef __cplusplus
extern "C"{
#endif	/*__cplusplus*/

/*
*------------------------------------------------------------------------------
* Types Definition
*------------------------------------------------------------------------------
*/

#if _MSC_VER == 800
typedef long sub_int32_t;
#else
typedef int sub_int32_t;
#endif

typedef void* sub_device;
typedef void* sub_handle;

struct sub_version
{
	struct 
	{
		int major;
		int minor;
		int micro;
		int nano;
	}dll;
	
	struct 
	{
		int major;
		int minor;
		int micro;
		int nano;
	}driver;

	struct
	{
		int micro;
		int minor;
		int major;
		int boot;
	}sub_device;
};

struct sub_cfg_vpd
{
    int lcd_w;      /* LCD With (0-no LCD) */
    int lcd_h;      /* LCD Lines 1,2,... */
    int serial;     /* 2-RS232, 4-RS485 */
    int buttons;    /* Number of buttons */
    int ir_car;		/* 0-no IR, IR carrier frequency Hz */
};

/* HS I2C Master */
struct sub_i2c_hs_xfer
{
	int		sa;
    int     r_w;
	int		sz;
	int		act_sz;
	char	data[64];
	int		status;
};

/* MDIO */
union sub_mdio_frame
{
	struct 
	{
		int		op;
		int		phyad;
		int		regad;
		int		data;
	}clause22;
	struct
	{
		int		op;
		int		prtad;
		int		devad;
		int		data;
	}clause45;
};

/*
*-----------------------------------------------------------------------------
* Constants
*-----------------------------------------------------------------------------
*/

/*
* Error Codes
*/
#define SUB_ERRORS	\
DEF_SUB_ERROR( SE_OK, 			0, 	"OK" 									)\
DEF_SUB_ERROR( SE_NODEV, 		1, 	"SUB Device not found" 					)\
DEF_SUB_ERROR( SE_OPEN,			2,	"Can't open SUB device"					)\
DEF_SUB_ERROR( SE_SETCONF,	    3,	"Can't set configuration"				)\
DEF_SUB_ERROR( SE_CLAIM,	    4,	"Can't claim interface"					)\
DEF_SUB_ERROR( SE_SETA,		    5,	"Failed to setup async transaction"		)\
DEF_SUB_ERROR( SE_SUBMIT,	    6,	"Failed to submit async transaction"	)\
DEF_SUB_ERROR( SE_BULKOUT,	    7,	"Bulk write failed"						)\
DEF_SUB_ERROR( SE_BULKIN_RC,    8,	"Bulk read failed"						)\
DEF_SUB_ERROR( SE_BULKIN_SZ,    9,	"Bulk read incomplete"					)\
DEF_SUB_ERROR( SE_OUT_OVF,    	10,	"Out buffer overflow"					)\
DEF_SUB_ERROR( SE_I2C,    		11,	"I2C error"								)\
DEF_SUB_ERROR( SE_TAG_CODE,    	12,	"Wrong tag code in response"			)\
DEF_SUB_ERROR( SE_TAG_SIZE,    	13,	"Wrong tag size in response"			)\
DEF_SUB_ERROR( SE_PARAM,    	14,	"Wrong parameters"						)\
DEF_SUB_ERROR( SE_SPI_DIS,    	15,	"SPI Disabled"							)\
DEF_SUB_ERROR( SE_NSUPPORTED,   16,	"Feature not supported"					)\
DEF_SUB_ERROR( SE_TIMEOUT,    	17,	"Timeout"								)\
DEF_SUB_ERROR( SE_INIT,    		18,	"Init Failed"							)\
DEF_SUB_ERROR( SE_BAD_HNDL,		19,	"Bad Handle"							)\
DEF_SUB_ERROR( SE_IR,			20,	"InfraRed Failure"						)\
DEF_SUB_ERROR( SE_EACCESS,		21,	"Access Denied"							)

#define DEF_SUB_ERROR(NAME,VAL,DESCR)	NAME=VAL,
enum { SUB_ERRORS SUB_LASTERROR };

#ifndef ETIMEDOUT
 #define ETIMEDOUT  116
#endif

/*
*-----------------------------------------------------------------------------
* Global Variables
*-----------------------------------------------------------------------------
*/

#ifdef __SUB_STATIC__
    #define __SUB_DECL__    extern
#else
 #if defined(_MSC_VER) && _MSC_VER > 800
	#ifdef __LIBSUB_C__
	    #define __SUB_DECL__ extern /* libsub.c */
	#else
		#define __SUB_DECL__    _declspec( dllimport )
	#endif	/*__LIBSUB_C__ */
 #elif defined(__CYGWIN__)
	/* gcc under cygwin - dynamic linkage */
    #define __SUB_DECL__    __declspec( dllimport )
 #else	/*defined(_MSC_VER) && _MSC_VER > 800 */
	/* linux, dos - static linkage */
    #define __SUB_DECL__    extern
 #endif	/*defined(_MSC_VER) && _MSC_VER > 800 */
#endif

__SUB_DECL__    int	sub_errno, sub_i2c_status;

/*
*-----------------------------------------------------------------------------
* MACRO
*-----------------------------------------------------------------------------
*/

/*
* I2C
*/
#define I2C_GCE			0x01	/* Enable general call (sa=0) in slave mode */
#define I2C_DISABLE     0x02	/* Disable I2C Module */
#define I2C_MA_HI(VAL)    ((VAL<<16))  /* Optional bytes 4,5 of MA */  

/* Bit-bang I2C mode */
#define BB_I2C_FAST_P	0x00
#define BB_I2C_FAST		0x01
#define BB_I2C_STD		0x03

/* 
* SPI 
*/
#define SPI_ENABLE		0x40

/* SPI clock polarity */
#define SPI_CPOL_RISE	0x00	/* ____/--\__ */
#define	SPI_CPOL_FALL	0x08	/* ----\__/-- */

/* SPI phase */
#define SPI_SMPL_SETUP	0x00	/* Leading - Sample, Traling - Setup  */
#define SPI_SETUP_SMPL	0x04    /* Leading - Setup,  Traling - Sample */ 

/* SPI Slave */
#define SPI_SLAVE		0x10

/* SPI Bit order */
#define SPI_LSB_FIRST	0x20	/* Transmit LSB first */
#define SPI_MSB_FIRST	0x00	/* Transmit MSB first */

/* SPI CLK Frequency */
#define SPI_CLK_8MHZ	0x80
#define SPI_CLK_4MHZ	0x00
#define SPI_CLK_2MHZ	0x81
#define SPI_CLK_1MHZ	0x01
#define SPI_CLK_500KHZ	0x82
#define	SPI_CLK_250KHZ	0x02
#define	SPI_CLK_125KHZ	0x03

/* Low Speed SPI */
#define SPI_CLK_LOW		0x0083		/* 250 ... 4 KHz */
#define SPI_CLK_HZ(FHZ)	( ( ((1000000UL/(FHZ)/2)&0x7F) <<8) |SPI_CLK_LOW)

/* SS Configuration for SPI Read/Write */
#define SS_CONF(SS_N,SS_MODE)	((SS_N)<<4)|((SS_MODE)&0x0F)
	#define SS_H		0x0F	/* ____/----- - - - */
	#define SS_HL		0x09	/* ____/-\___ _ _ _ */
	#define SS_HHL		0x0A	/* ____/--\__ _ _ _ */
	#define SS_HHHL		0x0B	/* ____/---\_ _ _ _ */
	#define SS_HHHHL	0x0C	/* ____/----\__ _ _ */
	#define	SS_HI		0x0D	/* ____/-- - -\__ _ */
	#define SS_L		0x07	/* ----\_____ _ _ _ */
	#define SS_LH		0x01	/* ----\_/----- - - */
	#define SS_LLH		0x02	/* ----\__/---- - - */
	#define SS_LLLH		0x03	/* ----\___/--- - - */
	#define SS_LLLLH	0x04	/* ----\____/-- - - */
	#define SS_LO		0x05	/* ----\__ _ _/-- - */
	#define SS_HiZ		0x08	/* xxxxxxxxxxxxxxxx */

/*
* MDIO
*/
#define SUB_MDIO22			0x80
#define SUB_MDIO22_READ		(0x02|SUB_MDIO22)
#define SUB_MDIO22_WRITE	(0x01|SUB_MDIO22)
#define SUB_MDIO45_ADDR		0x00
#define SUB_MDIO45_WRITE	0x01
#define SUB_MDIO45_PRIA		0x02	/* Post-Read-Increment-Address */
#define SUB_MDIO45_READ		0x03

#define SUB_CFP_MSA			0x80

/*
* Fast PWM
*/
#define FPWM_ENABLE		0x0001
#define FPWM_EN0 		0x8000	/* Enable FPWM0 */
#define FPWM_EN1		0x2000	/* Enable FPWM1 */
#define FPWM_EN2		0x0800	/* Enable FPWM2 */

/* 
* ADC
*/
#define ADC_ENABLE	  	0x8000	/* ADC Enable */  
#define ADC_REF_VCC		0x0040	/* ADC Reference - Vcc */
#define ADC_REF_AREF	0x0000	/* ADC Reference - AREF */
#define ADC_REF_2_56	0x00C0	/* ADC Reference - 2.56V */

enum
{
	/* Single ended ADC inputs */
	ADC_S0, ADC_S1, ADC_S2, ADC_S3,
	ADC_S4, ADC_S5, ADC_S6, ADC_S7,

	ADC_D10_10X = 9,	/* Differential ADC1(+) ADC0(-) Gain 10x  */
	ADC_D10_200X= 11,	/* Differential ADC1(+) ADC0(-) Gain 200x */
	ADC_D32_10X = 13,	/* Differential ADC3(+) ADC2(-) Gain 10x  */
	ADC_D32_200X= 15,	/* Differential ADC3(+) ADC2(-) Gain 200x */
	ADC_D01 	= 16,	/* Differential ADC0(+) ADC1(-) Gain 1x   */
	ADC_D21     = 18,	/* Differential ADC2(+) ADC1(-) Gain 1x   */
	ADC_D31     = 19,	/* Differential ADC3(+) ADC1(-) Gain 1x   */
	ADC_D41     = 20,	/* Differential ADC4(+) ADC1(-) Gain 1x   */
	ADC_D51     = 21,	/* Differential ADC5(+) ADC1(-) Gain 1x   */
	ADC_D61     = 22,	/* Differential ADC6(+) ADC1(-) Gain 1x   */
	ADC_D71     = 23,	/* Differential ADC7(+) ADC1(-) Gain 1x   */
	ADC_D02     = 24,	/* Differential ADC0(+) ADC2(-) Gain 1x   */
	ADC_D12     = 25,	/* Differential ADC1(+) ADC2(-) Gain 1x   */
	ADC_D32     = 27,	/* Differential ADC3(+) ADC2(-) Gain 1x   */
	ADC_D42     = 28,	/* Differential ADC4(+) ADC2(-) Gain 1x   */
	ADC_D52     = 29,	/* Differential ADC5(+) ADC2(-) Gain 1x   */
	ADC_1_1V	= 30,	/* Single Ended internally connected to 1.1V */
	ADC_GND		= 31,	/* Single Ended internally connected to GND  */ 
};

/*
* RS232/RS485
*/

#define RS_RX_ENABLE	0x80
#define RS_TX_ENABLE	0x40

/* Character Size */
#define RS_CHAR_5		0x00
#define RS_CHAR_6		0x02
#define RS_CHAR_7		0x04
#define RS_CHAR_8		0x06
#define RS_CHAR_9		0x07

/* Parity */
#define RS_PARITY_NONE	0x00
#define RS_PARITY_EVEN	0x20
#define RS_PARITY_ODD	0x30

/* Stop Bits */
#define RS_STOP_1		0x00
#define RS_STOP_2		0x08

/* RS Timing Flags */
#define RS_RX_BEFORE_TX	0x01
#define RS_RX_AFTER_TX	0x02

/* IR (InfraRed) */
#define IR_UART_MODE	0x00
#define IR_ROW_MODE	    0x01

/*
* FIFO
*/
#define FIFO_SELECT_SPI		0x01
#define FIFO_SELECT_UART	0x02
#define FIFO_SELECT_I2C		0x10
#define FIFO_CLEAR			0x100
#define FIFO_READ_FULL		0x200

/*
* Counters
*/
#define CNTR_EN         0x80
#define CNTR_CLR        0x40
#define CNTR_16MHZ      0x01
#define CNTR_2MHZ       0x02
#define CNTR_250KHZ     0x03
#define CNTR_62500HZ    0x04
#define CNTR_15625HZ    0x05
#define CNTR_FEDGE      0x06
#define CNTR_REDGE      0x07


/*
*-----------------------------------------------------------------------------
* Function Definitions
*-----------------------------------------------------------------------------
*/
/* mode */
#define SUB_BOOT_MODE	0
int sub_get_mode( sub_handle hndl, int *boot );

/* SUB-20 reset */
int sub_reset( sub_handle hndl );

/*debugging */
int sub_set_debug_level( int level );

sub_device sub_find_devices( sub_device first );
sub_handle sub_open( sub_device dev );
int sub_close( sub_handle hndl );
const struct sub_version* sub_get_version( sub_handle hndl );
const struct sub_cfg_vpd* sub_get_cfg_vpd( sub_handle hndl );

/* I2C */
int sub_i2c_freq( sub_handle hndl, sub_int32_t *freq );
int sub_i2c_config( sub_handle hndl, int sa, int flags );
int sub_i2c_start( sub_handle hndl );
int sub_i2c_stop( sub_handle hndl );
int sub_i2c_scan( sub_handle hndl, int* slave_cnt, char* slave_buf );
int sub_i2c_read( sub_handle hndl, int sa, sub_int32_t ma, int ma_sz, char* buf, int sz );
int sub_i2c_write( sub_handle hndl, int sa, sub_int32_t ma, int ma_sz, char* buf, int sz );
int sub_i2c_hs_rw( sub_handle hndl, int mcode, int count, struct sub_i2c_hs_xfer* px ); 

/* SPI Master */
int sub_spi_config( sub_handle hndl, int cfg_set, int* cfg_get );
int sub_spi_transfer( sub_handle hndl, char* out_buf, char* in_buf, int sz, int ss_config );
int sub_sdio_transfer( sub_handle hndl, char* out_buf, char* in_buf, int out_sz, int in_sz, int ss_config );
int sub_spi_transfer_ess( sub_handle hndl, char* out_buf, char* in_buf, int sz, char* ess_str );

/* MDIO */
int sub_mdio22( sub_handle hndl, int op, int phyad, int regad, int data, int* content );
int sub_mdio45( sub_handle hndl, int op, int prtad, int devad, int data, int* content );
int	sub_mdio_xfer( sub_handle hndl, int count, union sub_mdio_frame* mdios );
int	sub_mdio_xfer_ex( sub_handle hndl, int channel, int count, union sub_mdio_frame* mdios );

/* GPIO */
int	sub_gpio_config( sub_handle hndl, sub_int32_t set, sub_int32_t* get, sub_int32_t mask );
int	sub_gpio_read( sub_handle hndl, sub_int32_t* get );
int	sub_gpio_write( sub_handle hndl, sub_int32_t set, sub_int32_t* get, sub_int32_t mask );
/* GPIOB */
int	sub_gpiob_config( sub_handle hndl, sub_int32_t set, sub_int32_t* get, sub_int32_t mask );
int	sub_gpiob_read( sub_handle hndl, sub_int32_t* get );
int	sub_gpiob_write( sub_handle hndl, sub_int32_t set, sub_int32_t* get, sub_int32_t mask );
/* Edge */
int sub_edge_config( sub_handle hndl, sub_int32_t set, sub_int32_t* get );
int sub_edge_read( sub_handle hndl, sub_int32_t *get, sub_int32_t* edge );

/* Fast PWM */
int sub_fpwm_config( sub_handle hndl, double freq_hz, int flags );
int sub_fpwm_set( sub_handle hndl, int pwm_n, double duty );

/* PWM */
int sub_pwm_config( sub_handle hndl, int res_us, int limit );
int sub_pwm_set( sub_handle hndl, int pwm_n, int duty );


/* ADC */
int sub_adc_config( sub_handle hndl, int flags );
int sub_adc_single( sub_handle hndl, int* data, int mux );
int sub_adc_read( sub_handle hndl, int* data, int* mux, int reads );

/* LCD */
int	sub_lcd_write( sub_handle hndl, char* str );

/* RS232/RS485 */
int sub_rs_set_config( sub_handle hndl, int config, sub_int32_t baud );
int sub_rs_get_config( sub_handle hndl, int* config, sub_int32_t* baud );
int sub_rs_timing( sub_handle hndl, 
				int flags, int tx_space_us, int rx_msg_us, int rx_byte_us );
int sub_rs_xfer( sub_handle hndl, 
				char* tx_buf, int tx_sz, char* rx_buf, int rx_sz );

/* Bit-bang I2C */
int sub_bb_i2c_config( sub_handle hndl, int mode, int stretch_ms );
int sub_bb_i2c_scan( sub_handle hndl, int channel, 
					   			int* slave_cnt, char* slave_buf );
int sub_bb_i2c_read( sub_handle hndl, int channel, int sa, 
								sub_int32_t ma, int ma_sz, char* buf, int sz );
int sub_bb_i2c_write( sub_handle hndl, int channel, int sa, 
								sub_int32_t ma, int ma_sz, char* buf, int sz );

/* EEP Read/Write */
int sub_eep_read( sub_handle hndl, int addr, char* buf, int sz );
int sub_eep_write( sub_handle hndl, int addr, char* buf, int sz );

/* IR */
int sub_ir_config( sub_handle hndl, int carrier, int duty, int mode, ... );
int sub_ir_tx( sub_handle hndl, int sz, char* buf );

/* FIFO */
int sub_fifo_write( sub_handle hndl, char* buf, int sz, int to_ms );
int sub_fifo_read( sub_handle hndl, char* buf, int sz, int to_ms );
int sub_fifo_config( sub_handle hndl, int config );

/* USB */
int usb_transaction( sub_handle hndl,
							 char* out_buf, int out_sz,
							 char*  in_buf, int	in_sz,
							 int timeout );

/* product identification */
int sub_control_request( sub_handle hndl, 
							int type, int request, int value, int index,
							char* buf, int sz, int timeout 
						);
int sub_get_serial_number( sub_handle hndl, char *buf, int sz);
int sub_get_product_id( sub_handle hndl, char *buf, int sz);


char* sub_strerror( int errnum );
int	sub_get_errno( void ) ;
int sub_get_i2c_status( void ) ;


#ifdef __cplusplus
}
#endif	/*__cplusplus*/

#endif /*_H_INCLUDED*/

