/*
 * Libsub implementation
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
* $Id: libsub.c,v 1.137 2013-09-12 04:38:26 avr32 Exp $
*-------------------------------------------------------------------
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef _MSC_VER
#define __LIBSUB_C__
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
#endif	/*MSC_VER*/

#ifdef LIBUSB_1_0
 #include <libusb.h>
#else
 #ifdef __linux__
  #include <stdint.h>
  #include <usb.h>
 #else
  #include "../libusb/libusb-win32-src/src/usb.h"
 #endif
#endif 
#include "libsub.h"
#include <subcfg.h>
#include <sub_if.h>

/* shared memory definitions*/
#if defined(_MSC_VER) && _MSC_VER > 800
#define SHARED_MEM_NAME	"SUB_SHARED_MEM"
HANDLE hFileMapping=NULL;
typedef struct
{
	int	sub_debug_level;
}shared_mem_t;
shared_mem_t *shared_mem=NULL;
#endif	/*MSVC_VER*/

/*
*-----------------------------------------------------------------------------
* Types Definition
*-----------------------------------------------------------------------------
*/
typedef struct
{
	unsigned char	pack_sz;
	union
	{
		sub_tag_t	tag;
		char 		pack_data[SUB_MAX_PACK_LONG-1];
	};
}sub_pack;

/* SUB Device Cash */
typedef struct
{
	sub_handle	handle;
	int			version;

	/* Features */
#define SUB_FEATURE_SPI_SZ	0x00
#define SUB_FEATURE_VERSION	0x01
	int			spi_sz;
}sub_dev_cache_entry;

/*
*-----------------------------------------------------------------------------
* Constants
*-----------------------------------------------------------------------------
*/
#define __VER__		29

#define SUB_VID		0x04D8
#define SUB_PID		0xFFC3

#define SUB_OUT_EP		0x01
#define SUB_IN_EP		0x82
#define SUB_FIFO_OUT_EP	0x03
#define SUB_FIFO_IN_EP	0x84

#define SUB_F_CPU	16000000
				   

/*
*-----------------------------------------------------------------------------
* Global Variables
*-----------------------------------------------------------------------------
*/
#if !defined(_MSC_VER) || _MSC_VER == 800
int	sub_debug_level;
#endif

int	sub_errno;
int sub_i2c_status;

#ifdef LIBUSB_1_0
libusb_context* usb_context;
static libusb_device** list;
static int  init_required=1;
#endif

sub_dev_cache_entry sub_dev_cache[255];


/*
*-----------------------------------------------------------------------------
* Global Function Definition
*-----------------------------------------------------------------------------
*/


/*
*-----------------------------------------------------------------------------
* Local Function Definition
*-----------------------------------------------------------------------------
*/
int sub_transaction( sub_handle hndl, sub_pack* outpk, sub_pack* inpk, int timeout );

void sub_trace( int level, char* format, ... );
void hexdump_buf( int level, char* buf, int sz );


/*
*-----------------------------------------------------------------------------
* MACRO
*-----------------------------------------------------------------------------
*/

/* USB vendor requests definitions */
#define CTRL_REQ_TRANSFER_DIR_IN		(1<<7)
#define CTRL_REQ_TRANSFER_DIR_OUT		0

#define CTRL_REQ_TYPE_STANDARD			(0<<5)
#define CTRL_REQ_TYPE_CLASS				(1<<5)
#define CTRL_REQ_TYPE_VENDOR			(2<<5)

#define CTRL_REQ_RCPT_DEVICE			0
#define CTRL_REQ_RCPT_INTERFACE			1
#define CTRL_REQ_RCPT_ENDPOINT			2
#define CTRL_REQ_RCPT_OTHER				3



/* LSB first */
#define BUF_2_INT(I,BUF) I=(BUF[3]<<24)+(BUF[2]<<16)+(BUF[1]<<8)+BUF[0]
#define INT_2_BUF(I,BUF) BUF[0]=((char)I);BUF[1]=(char)((I)>>8);BUF[2]=(char)((I)>>16);BUF[3]=(char)((I)>>24)
#define INT_2_LE16(I,LE16)				\
	do {								\
		((char*)&LE16)[0]=(I); 			\
		((char*)&LE16)[1]=(I)>>8;  	    \
	} while(0)

/*
*-----------------------------------------------------------------------------
* Device Cache Functions
*-----------------------------------------------------------------------------
*/

/*
* Look for cache entry with given device handle 
*/
static int sub_dev_cache_lookup( sub_handle hndl )
{
	int		i;
	for( i=0; i<255; i++ )
	{
		if( sub_dev_cache[i].handle == hndl )
			return i;
	}
	return -1;
}

static int sub_dev_cache_get_feature( sub_handle hndl, int feature, void* val )
{
	int		i;
	const struct sub_version* version;

	i = sub_dev_cache_lookup( hndl );
	if( i == -1 )
	{
		sub_errno = SE_BAD_HNDL;
		return SE_BAD_HNDL;
	}

	if( !sub_dev_cache[i].version )
	{
		version = sub_get_version( sub_dev_cache[i].handle );
		if( version->sub_device.major == 0xFF )
			return -1;
		sub_dev_cache[i].version = 
					version->sub_device.major*100 +
					version->sub_device.minor*10  +
					version->sub_device.micro;

		if( sub_dev_cache[i].version >= 24 )
			sub_dev_cache[i].spi_sz = SUB_MAX_SPI_RDWR_LONG;
		else
			sub_dev_cache[i].spi_sz = SUB_MAX_SPI_RDWR;
	}

	switch( feature )
	{
	case SUB_FEATURE_SPI_SZ:
		*((int*)val) = sub_dev_cache[i].spi_sz;
		break;
	case SUB_FEATURE_VERSION:
		*((int*)val) = sub_dev_cache[i].version;
		break;
	default:
		return -1;
	}

	return 0;
}


/*
*-----------------------------------------------------------------------------
* Open/Close Functions
*-----------------------------------------------------------------------------
*/
sub_device sub_find_devices( sub_device first )
{
#ifdef LIBUSB_1_0
	static size_t 	dev_cnt;
	int				i;

	if( init_required )
	{
		if( libusb_init(&usb_context) )
		{
			sub_errno = SE_INIT;
			return 0;
		}
		init_required = 0;
		list = 0;
	}
    
	/* first == 0 forces re-enumeration */
	if( !first && list )
	{
		libusb_free_device_list( list, 1 );
		list = 0;
	}

	if( !list )
	{
		first = 0;
		dev_cnt = libusb_get_device_list( usb_context, &list );
		if( dev_cnt < 0 )
		{
			list = 0;
			sub_errno = SE_OUT_OVF;
			return 0;
		}
	}

	for( i=0; i<dev_cnt; i++ )
	{
		if( first )
		{
			/* Skip already found devices */
			if( list[i] == first )
				first = 0;	
		}
		else
		{
			/* Check if interesting device */
			libusb_device* dev = list[i];
			struct libusb_device_descriptor descriptor;
			if( libusb_get_device_descriptor(dev,&descriptor) )
			{
				sub_errno =-1;
				return 0;
			}
			if((descriptor.idVendor==SUB_VID)&&(descriptor.idProduct==SUB_PID))
				return dev;
		}
	}
	
/* 
* Got to the end of the list. 
* Any found device must be open before end of the list reached otherwise
* it will be freed here. Open increases refernce count and preserves device.
*/
	libusb_free_device_list( list, 1 );	/* Free list and all not open devices */
	list = 0;
	sub_errno = SE_NODEV;
    return 0;

#else
/* Libusb 0.1 Compatible Implementation */
	static int	init_required=1;

    struct usb_bus* 	bus=0;
    struct usb_bus*     first_bus=0;
    struct usb_device*	dev=0,*rc_dev=0;

	if( init_required )
	{
		usb_init();
		init_required = 0;
	}
    
    if( !first )
    {
		usb_find_busses();
		usb_find_devices();
        bus = usb_get_busses();
        if( bus )
            dev = bus->devices;
    }
	else
	{
		bus=((struct usb_device*)first)->bus;
		dev=((struct usb_device*)first)->next;
		if(!dev)
		{
			bus=bus->next;
		}
	}

	first_bus = bus;
	for( ; bus; bus=bus->next )
	{
		if(first_bus!=bus)
		{
			/* start from the first device on a new bus */
			dev = bus->devices;
		}

		for( ; dev; dev=dev->next )
		{
			sub_trace( 4, "Device VID/PID: %04X/%04X\n", 
					dev->descriptor.idVendor, dev->descriptor.idProduct );
			if( (dev->descriptor.idVendor == SUB_VID) && 
				(dev->descriptor.idProduct== SUB_PID) )
			{
                rc_dev = dev;
                dev=dev->next; 
                return rc_dev;
			}
		}
	}

	sub_errno = SE_NODEV;

    return 0;
#endif /*LIBUSB_1_0*/
}

/*
* Open SUB device
*/
sub_handle sub_open( sub_device dev )
{
	int		i;

#ifndef _MSC_VER
	/* Determine Debug Level */
	{
		char* level_env = getenv("SUB_DEBUG");
		if( level_env )
			sub_debug_level = atoi(level_env);
	}
#endif /*_MSC_VER*/

#ifdef LIBUSB_1_0
    libusb_device_handle*	handle;
	int		config;

	/* Open Device */
	if( !dev )
    {
		if( init_required )
		{
		   if( libusb_init(&usb_context) )
		   {
			   sub_errno = SE_INIT;
			   return 0;
		   }
		   init_required = 0;
		   list = 0;
		}

		handle = libusb_open_device_with_vid_pid(usb_context,SUB_VID,SUB_PID);
		if( !handle )
		{
			if(errno==EACCES)
			{
				sub_errno = SE_EACCESS;
			}
			else
			{
				sub_errno = SE_NODEV;
			}

			return 0;
		}
    }
	else
	{
		if( libusb_open( dev, &handle ) )
		{
			sub_errno = SE_OPEN;
			return 0;
		}
	}

	/* Set Configuration */
	if( libusb_get_configuration(handle, &config) )
	{
		sub_errno = SE_SETCONF;
		goto fin;
	}
	if( config!=1 )
	{
		if( libusb_set_configuration(handle,1) )
		{
			sub_errno = SE_SETCONF;
			goto fin;
		}
	}

	/* Claim Interface */
    if( libusb_claim_interface(handle,0) )
	{
		sub_errno = SE_CLAIM;
		goto fin;
	}

	/* Update cache */
	i = sub_dev_cache_lookup( 0 );
	sub_dev_cache[i].handle = handle;

	return handle;

fin:
	libusb_close( handle );
	return 0;

#else
/* Libusb 0.1 Compatible Implementation */
	usb_dev_handle* handle;

	if( !dev )
    {
        dev = sub_find_devices( 0 );
        if( !dev )
            return 0;
    }
    handle = usb_open( dev );
	if( !handle )
	{
		sub_errno = SE_OPEN;
		return 0;
	}

	if( usb_set_configuration(handle,1) < 0 )
	{
		sub_errno = SE_SETCONF;
		goto fin;
	}
	if( usb_claim_interface(handle,0) < 0 )
	{
		sub_errno = SE_CLAIM;
		goto fin;
	}
	
	/* Update cache */
	i = sub_dev_cache_lookup( 0 );
	sub_dev_cache[i].handle = handle;
	
	return handle;

fin:
	usb_close( handle );
	return 0;
#endif /*LIBUSB_1_0*/
}

/*
* Close SUB device
*/
int sub_close( sub_handle hndl )
{
	int		i;

	/* Update cache */
	i = sub_dev_cache_lookup( hndl );
	if( i != -1 )
		memset(	&sub_dev_cache[i], 0x00, sizeof(sub_dev_cache_entry) );

#ifdef LIBUSB_1_0
    libusb_close( hndl );
    return 0;
#else
	return usb_close( hndl );
#endif
}

/*
* Is boot mode?
*/
int sub_get_mode( sub_handle hndl, int *mode )
{
	int bc;
	uint8_t m;
	bc=sub_control_request(
		hndl, 
		CTRL_REQ_TRANSFER_DIR_IN|CTRL_REQ_TYPE_VENDOR|CTRL_REQ_RCPT_DEVICE,
		SUB_VENDOR_REQUEST,
		0,
		SUB_WINDEX_MODE,
		(char*)&m, 
		sizeof(m), 
		10000 );

	if( bc==1 )
	{
		*mode=m;
	}
	return bc>0?0:bc;
}

/*
* SUB-20 Reset
*/
int sub_reset( sub_handle hndl )
{
	int bc;
	
    bc=sub_control_request(
		hndl, 
		CTRL_REQ_TRANSFER_DIR_OUT|CTRL_REQ_TYPE_VENDOR|CTRL_REQ_RCPT_DEVICE,
		SUB_VENDOR_REQUEST,
		0,
		SUB_WINDEX_RESET,
		NULL, 
		0, 
		10000 );

	return bc>0?0:bc;
}

/*
* Get Version
*/
const struct sub_version* sub_get_version( sub_handle hndl )
{
	uint8_t		rc;
	static struct sub_version sub_ver;

#ifdef LIBUSB_1_0
	sub_ver.dll.major = 1;
	sub_ver.dll.minor = 0;
	sub_ver.dll.micro = 0;
#else
 #ifdef __linux__
	sub_ver.dll.major = 0;
	sub_ver.dll.minor = 1;
	sub_ver.dll.micro = 0;
 #else
	const struct usb_version* usb_version;
	usb_version = usb_get_version();
	sub_ver.dll.major = usb_version->dll.major;
	sub_ver.dll.minor = usb_version->dll.minor;
	sub_ver.dll.micro = usb_version->dll.micro;
	sub_ver.driver.major = usb_version->driver.major;
	sub_ver.driver.minor = usb_version->driver.minor;
	sub_ver.driver.micro = usb_version->driver.micro;
	sub_ver.driver.nano  = usb_version->driver.nano;
 #endif
#endif
	sub_ver.dll.nano  = __VER__;

	if(hndl)
	{
        struct get_version_resp_st ver;

		rc=sub_control_request(
			hndl, 
			CTRL_REQ_TRANSFER_DIR_IN|CTRL_REQ_TYPE_VENDOR|CTRL_REQ_RCPT_DEVICE,
			SUB_VENDOR_REQUEST,
			0,
			SUB_WINDEX_VER,
			(char*)&ver, 
			sizeof(ver), 
			10000 );
		if( rc == sizeof(ver) )
		{
			sub_ver.sub_device.micro = ver.micro;
			sub_ver.sub_device.minor = ver.minor;
			sub_ver.sub_device.major = ver.major;
			sub_ver.sub_device.boot  = ver.boot;
			goto fin;
		}
	}

	/* Version unknown */
	memset( &sub_ver.sub_device, 0xFF, sizeof(sub_ver.sub_device) ) ;

fin:
	return &sub_ver;
}

const struct sub_cfg_vpd* sub_get_cfg_vpd( sub_handle hndl )
{
	uint8_t		rc;
	static struct sub_cfg_vpd cfg_vpd;
	sub_vpd_t	vpd;

	rc=sub_control_request(
		hndl, 
		CTRL_REQ_TRANSFER_DIR_IN|CTRL_REQ_TYPE_VENDOR|CTRL_REQ_RCPT_DEVICE,
		SUB_VENDOR_REQUEST,
		0,
		SUB_WINDEX_CFG_VPD,
		(char*)&vpd, 
		sizeof(vpd), 
		10000 );

	if( rc == sizeof(vpd) )
	{
		cfg_vpd.lcd_w 	= vpd.lcd_w;
		cfg_vpd.lcd_h 	= vpd.lcd_h;
		cfg_vpd.serial	= vpd.serial;
		cfg_vpd.buttons	= vpd.buttons;
		cfg_vpd.ir_car	= vpd.ir_car;
	}
	else
		return 0;

	return &cfg_vpd;
}

/*
*-----------------------------------------------------------------------------
* EEP Read/Write
*-----------------------------------------------------------------------------
*/
int sub_eep_read( sub_handle hndl, int addr, char* buf, int sz )
{
	int 		rc;
	sub_pack	outpk,inpk;

	if( sz > SUB_MAX_EEPRW_SZ )
    {
        sub_errno = SE_PARAM;
		return SE_PARAM;
    }

	outpk.tag.size = sizeof(struct eep_rw_req_st);
	outpk.tag.code = SUB_EEP_RW;
	outpk.tag.eep_rw_req.r_w =1;
	outpk.tag.eep_rw_req.addr_lo = addr;
	outpk.tag.eep_rw_req.addr_hi = addr>>8;
	outpk.tag.eep_rw_req.sz = sz;

	inpk.tag.size = sz;
    rc = sub_transaction( hndl, &outpk, &inpk, 1000 );
	if( !rc )
        memcpy( buf, inpk.tag.eep_rw_resp.buf, sz );
	return rc;
}

int sub_eep_write( sub_handle hndl, int addr, char* buf, int sz )
{
	int 		rc;
	sub_pack	outpk,inpk;

	if( sz > SUB_MAX_EEPRW_SZ )
		return SE_PARAM;

	outpk.tag.size = sizeof(struct eep_rw_req_st)+sz;
	outpk.tag.code = SUB_EEP_RW;
	outpk.tag.eep_rw_req.r_w =0;
	outpk.tag.eep_rw_req.addr_lo = addr;
	outpk.tag.eep_rw_req.addr_hi = addr>>8;
	outpk.tag.eep_rw_req.sz = sz;
	memcpy( outpk.tag.eep_rw_req.buf, buf, sz );

	inpk.tag.size = 0;
    rc = sub_transaction( hndl, &outpk, &inpk, 1000 );
	return rc;
}


/*
*-----------------------------------------------------------------------------
* I2C
*-----------------------------------------------------------------------------
*/
int sub_i2c_freq( sub_handle hndl, sub_int32_t* freq )
{
	int 		rc;
	sub_pack	outpk,inpk;
	unsigned int TWBR,TWPS;

/*
* Fscl = F_CPU/(16+2TWBR*4^TWPS) 
* TWBR should be 10 or higher if the TWI operates in Master mode.
*/
	if( *freq )
	{
		if( *freq > SUB_F_CPU/(16+2*10) )
			return SE_PARAM;	/* Requested bit rate too big */
	
		TWPS = 0;
		TWBR = (SUB_F_CPU/(*freq)-16)/2;
		if( TWBR > 0xFF )
		{
			TWPS = 1;
			TWBR = (SUB_F_CPU/(*freq)-16)/(2*4);
			if( TWBR > 0xFF )
			{
				TWPS = 2;
				TWBR = (SUB_F_CPU/(*freq)-16)/(2*16);
				if( TWBR > 0xFF )
				{
					TWPS = 3;
					TWBR = (SUB_F_CPU/(*freq)-16)/(2*64);
					if( TWBR > 0xFF )
						return SE_PARAM;	/* Requested bit rate too small */
				}
			}
		}
		outpk.tag.size = sizeof(struct i2c_baud_req_st);
		outpk.tag.i2c_baud_req.twi_TWBR = TWBR;
		outpk.tag.i2c_baud_req.twi_TWPS = TWPS;
	}
	else
	{
		outpk.tag.size = 0;
	}

	outpk.tag.code = SUB_I2C_FREQ;
	inpk.tag.size = 2;
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
	{
		TWBR = inpk.tag.i2c_baud_resp.twi_TWBR;
		TWPS = inpk.tag.i2c_baud_resp.twi_TWPS & 0x03;
		*freq = (sub_int32_t) (SUB_F_CPU/( 16 + 2*TWBR * pow(4,TWPS) ));
	}
	return rc;
}

int sub_i2c_config( sub_handle hndl, int sa, int flags )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_I2C_CONFIG;
	outpk.tag.size = sizeof(struct i2c_config_req_st);
	outpk.tag.i2c_config_req.sa = sa;
	outpk.tag.i2c_config_req.flags = flags;

	inpk.tag.size = sizeof(struct i2c_config_req_st);
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	return rc;
}

int sub_i2c_start( sub_handle hndl )
{
	int 		rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_I2C_START;
	outpk.tag.size = sizeof(struct i2c_start_req_st);
	outpk.tag.i2c_start_req.restart = 0;
    
	inpk.tag.size = sizeof(struct i2c_start_resp_st);
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
	{
		sub_i2c_status = inpk.tag.i2c_start_resp.status;
		if( sub_i2c_status )
			rc = SE_I2C;
	}
	return rc;
}


int sub_i2c_stop( sub_handle hndl )
{
	int 		rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_I2C_STOP;
	outpk.tag.size = 0;
    
	inpk.tag.size = sizeof(struct i2c_stop_resp_st);
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
	{
		sub_i2c_status = inpk.tag.i2c_stop_resp.status;
		if( sub_i2c_status )
			rc = SE_I2C;
	}
	return rc;
}

int sub_i2c_scan( sub_handle hndl, int* slave_cnt, char* slave_buf )
{
	int 		rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_I2C_SCAN;
	outpk.tag.size = 0;
    
	inpk.tag.size = 0;	/* Input size is unknown at this stage */
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
	{
		*slave_cnt = inpk.tag.i2c_scan_resp.slave_n;
		memcpy( slave_buf, inpk.tag.i2c_scan_resp.slave_buf, *slave_cnt );
	}
	return rc;
}

/*
* Copy int to buffer MSB first 
*/
static void ma_copy( uint8_t* ma_buf, sub_int32_t ma, int ma_sz )
{
	int		i,j;

	for( i=(ma_sz&0xFF)-1,j=0; i>=0; i--,j++ )
	{   
        if( j==4 )
            ma_buf[i] = (ma_sz>>16)&0xFF;
        else if( j==5 )
            ma_buf[i] = (ma_sz>>24)&0xFF;
        else
        {
		    ma_buf[i] = ma & 0xFF;
    		ma >>= 8;
        }
	}
}

int sub_i2c_read( sub_handle hndl, int sa, sub_int32_t ma, int ma_sz, 
													char* buf, int sz )
{
	int			rc=0;
	sub_pack	outpk,inpk;
	int			read=0, read_sz;

	if( sz <= 0 )
		return SE_PARAM;

	while( (read < sz) && !rc )
	{
		read_sz = sz - read;
		if( read_sz > SUB_MAX_I2C_READ )
		{
			read_sz = SUB_MAX_I2C_READ;			/* Not last */
			outpk.tag.i2c_rdwr_req.flags = __GENF_NOSTOP;
		}
		else
			outpk.tag.i2c_rdwr_req.flags = 0;	/* Last */

		if( read == 0 )
		{										/* First */
			ma_copy( outpk.tag.i2c_rdwr_req.ma, ma, ma_sz );
            ma_sz &= 0xFF;
			outpk.tag.size = (int)sizeof(struct i2c_rdwr_req_st)+ma_sz;
			outpk.tag.i2c_rdwr_req.ma_sz = ma_sz;
		}
		else
		{									  	/* Not first */
			outpk.tag.size = (int)sizeof(struct i2c_rdwr_req_st);
			outpk.tag.i2c_rdwr_req.ma_sz = 0;
			outpk.tag.i2c_rdwr_req.flags |= __GENF_NOSLA;
		}
		outpk.tag.code = SUB_I2C_RDWR;
		outpk.tag.i2c_rdwr_req.sa = (sa<<1)|0x01;
		outpk.tag.i2c_rdwr_req.sz = read_sz;

		inpk.tag.size = (uint8_t)sizeof(struct i2c_rdwr_resp_st)+read_sz;
		inpk.tag.i2c_rdwr_resp.status = 0;
		
		rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
		sub_i2c_status = inpk.tag.i2c_rdwr_resp.status;

		if( !rc )
		{
			if( sub_i2c_status )
			{
				rc = SE_I2C;
				sub_errno = rc;
			}
			else
			{
				memcpy( buf+read, inpk.tag.i2c_rdwr_resp.data, read_sz );
				read += read_sz;
			}
		}
		else
		{
			if( (rc==SE_BULKIN_SZ) && sub_i2c_status )
			{
				rc = SE_I2C;
				sub_errno = rc;
			}
		}
	}
	return rc;
}

int sub_i2c_write( sub_handle hndl, int sa, sub_int32_t ma, int ma_sz, 
													char* buf, int sz )
{
	int			rc=0;
	sub_pack	outpk,inpk;
	int			written=0, write_sz;

	if( sz <= 0 )
		return SE_PARAM;

	while( (written < sz) && !rc )
	{
		write_sz = sz - written;
		if( write_sz > (int)(SUB_MAX_I2C_WRITE-(ma_sz&0xFF)) )
		{
			write_sz = SUB_MAX_I2C_WRITE-(ma_sz&0xFF);  /* Not last */
			outpk.tag.i2c_rdwr_req.flags = __GENF_NOSTOP;
		}
		else
			outpk.tag.i2c_rdwr_req.flags = 0;	/* Last */

		if( written == 0 )
		{										/* First */
			ma_copy( outpk.tag.i2c_rdwr_req.ma, ma, ma_sz );
            ma_sz &= 0xFF;
			outpk.tag.size = (int)sizeof(struct i2c_rdwr_req_st)+ma_sz+write_sz;
			outpk.tag.i2c_rdwr_req.ma_sz = ma_sz;
		}
		else
		{									  	/* Not first */
			outpk.tag.size = (int)sizeof(struct i2c_rdwr_req_st)+write_sz;
			outpk.tag.i2c_rdwr_req.ma_sz = 0;
			outpk.tag.i2c_rdwr_req.flags |= __GENF_NOSLA;
		}
		memcpy( outpk.tag.i2c_rdwr_req.ma+ma_sz, buf+written, write_sz );
		outpk.tag.code = SUB_I2C_RDWR;
		outpk.tag.i2c_rdwr_req.sa = sa<<1;
		outpk.tag.i2c_rdwr_req.sz = write_sz;
		ma_sz=0; 	/* ma_sz used only once */ 

		inpk.tag.size = sizeof(struct i2c_rdwr_resp_st);
		inpk.tag.i2c_rdwr_resp.status = 0;
		
		rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
		sub_i2c_status = inpk.tag.i2c_rdwr_resp.status;

		if( !rc )
		{
			if( sub_i2c_status )
			{
				rc = SE_I2C;
				sub_errno = rc;
			}
			else
				written += write_sz;
		}
	}
	return rc;
}

/*
* High Speed
*/
int sub_i2c_hs_rw( sub_handle hndl, int mcode, int count, struct sub_i2c_hs_xfer* px )
{
	int			rc,i;
	sub_pack	outpk,inpk;
	struct i2c_hs_xfer *pxfer;
	struct i2c_hs_stat *pstat;
	char		*p;
	int			in_sz=0, out_sz=0;

	outpk.tag.code = SUB_I2C_HS_RW;
	outpk.tag.i2c_hs_rw_req.master_code = mcode & 0x07;
	outpk.tag.i2c_hs_rw_req.n = count;

	/* Assemble request */
	p = (char*)&outpk.tag.i2c_hs_rw_req.xfer[0];

	for( i=0; i<count; i++ )
	{
		pxfer = (struct i2c_hs_xfer*)(p+out_sz);

		pxfer->sa = px[i].sa << 1;
		pxfer->sz = px[i].sz;
		if( px[i].r_w )
		{
			/* Read */
			pxfer->sa |= 0x01;
			out_sz += sizeof(struct i2c_hs_xfer)-1;
			in_sz  += sizeof(struct i2c_hs_stat)-1 + px[i].sz;
		}
		else
		{
			/* Write */
			memcpy( pxfer->data, px[i].data, px[i].sz );
			out_sz += sizeof(struct i2c_hs_xfer)-1 + px[i].sz;
			in_sz  += sizeof(struct i2c_hs_stat)-1;
		}
	}
	
	/* Transaction */
	if( (out_sz > (int) SUB_MAX_I2C_HS_RW_REQ_LONG) || 
		(in_sz  > (int)SUB_MAX_I2C_HS_RW_RESP_LONG) )
	{
		sub_errno = SE_PARAM;
		return SE_PARAM;
	}

	in_sz += sizeof(struct i2c_hs_rw_resp_st);
/*
* The LONG (>1 USB frame) transaction requires exact response size.
* in_sz will be used by SUB as IN.sz
*/
	outpk.tag.i2c_hs_rw_req.in_sz = in_sz;

	outpk.tag.size = sizeof(struct i2c_hs_rw_req_st) + out_sz;
	inpk.tag.size  = in_sz;
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( rc )
		return rc;
	sub_i2c_status = inpk.tag.i2c_hs_rw_resp.hs_stat;
	if( sub_i2c_status )
	{
		rc = SE_I2C;
		sub_errno = rc;
		return rc;
	}

	/* Parse response */
	p = (char*)&inpk.tag.i2c_hs_rw_resp.stat[0];

	for( i=0; i<count; i++ )
	{
		if( i >= inpk.tag.i2c_hs_rw_resp.n )
		{
			px[i].status = 0xF8;	/* Unknown error */
			px[i].act_sz = 0;
			continue;
		}

		pstat = (struct i2c_hs_stat*)p;

		px[i].status = pstat->status;
		px[i].act_sz = pstat->sz;
		if( pstat->sz && px[i].r_w )
		{
			memcpy( px[i].data, pstat->data, pstat->sz );
			p += pstat->sz;
		}

		p += sizeof(struct i2c_hs_stat)-1;
	}
	return rc;
}


/*
*-----------------------------------------------------------------------------
* SPI
*-----------------------------------------------------------------------------
*/
int sub_spi_config( sub_handle hndl, int cfg_set, int* cfg_get )
{
	int			rc;
	sub_pack	outpk,inpk;
	int			version;

    rc = sub_dev_cache_get_feature( hndl, SUB_FEATURE_VERSION, &version );
    if( rc )
        return rc;

	if( version >= 33 )
	{
		outpk.tag.code = SUB_SPI_CONFIG2;
		if( cfg_get )
			outpk.tag.size = 0;
		else
		{
			outpk.tag.size = sizeof(struct spi_config2_req_st);
			outpk.tag.spi_config2_req.spi_SPCR = cfg_set & 0xFF;
			outpk.tag.spi_config2_req.bbh_spi_SPCR = (cfg_set>>8)&0xFF;
		}
		inpk.tag.size = sizeof(struct spi_config2_req_st);
	}
	else
	{
		outpk.tag.code = SUB_SPI_CONFIG;
		if( cfg_get )
			outpk.tag.size = 0;
		else
		{
            if( cfg_set > 0xFF )
            {
                sub_errno = SE_NSUPPORTED;
                return sub_errno;
            }
			outpk.tag.size = sizeof(struct spi_config_req_st);
			outpk.tag.spi_config_req.spi_SPCR = cfg_set;
		}
		inpk.tag.size = sizeof(struct spi_config_resp_st);
	}

    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
	{
		if( cfg_get )
		{
			*cfg_get = inpk.tag.spi_config_req.spi_SPCR;
			if( version >= 33 )
				*cfg_get |= (inpk.tag.spi_config2_req.bbh_spi_SPCR<<8);
		}
	}
	return rc;
}

int sub_spi_transfer( sub_handle hndl, char* out_buf, char* in_buf, int sz, int ss_config )
{
	int			rc=0;
	sub_pack 	outpk,inpk;
	int			tr_done=0, tr_sz, tr_ss=ss_config, max_tr_sz;

	rc = sub_dev_cache_get_feature( hndl, SUB_FEATURE_SPI_SZ, &max_tr_sz );
	if( rc )
		return rc;
	
	while( (tr_done < sz) && !rc )
	{
		if( tr_done == 0 )
		{
			/* First */
			switch( ss_config & 0x0F )
			{
			case SS_HI:
				tr_ss = (tr_ss & 0xF0)|SS_H;
				break;
			case SS_LO:
				tr_ss = (tr_ss & 0xF0)|SS_L;
				break;
			}
		}

		tr_sz = sz - tr_done;
		if( tr_sz > max_tr_sz )
		{
			/* Not Last */
			tr_sz = max_tr_sz;
		}
		else
		{
			/* Last */
			switch( ss_config & 0x0F )
			{
			case SS_HI:
				tr_ss = (tr_ss & 0xF0)|SS_HI;
				break;
			case SS_LO:
				tr_ss = (tr_ss & 0xF0)|SS_LO;
			}
		}

		if( in_buf && !out_buf )
		{
			/* Read */
			outpk.tag.code = SUB_SPI_READ;
			outpk.tag.size = sizeof(struct spi_rdwr_req_st);
			outpk.tag.spi_rdwr_req.SS_config = tr_ss;
			outpk.tag.spi_rdwr_req.sz = tr_sz;
			inpk.tag.size = (uint8_t)sizeof(struct spi_rdwr_resp_st)+tr_sz;
		}
		else if( out_buf && !in_buf )
		{
			/* Write */
			outpk.tag.code = SUB_SPI_WRITE;
			outpk.tag.size = (uint8_t)sizeof(struct spi_rdwr_req_st)+tr_sz;
			outpk.tag.spi_rdwr_req.SS_config = tr_ss;
			outpk.tag.spi_rdwr_req.sz = tr_sz;
			memcpy( outpk.tag.spi_rdwr_req.data, out_buf+tr_done, tr_sz );
			inpk.tag.size = sizeof(struct spi_rdwr_resp_st);
		}
		else if( out_buf && in_buf )
		{
			/* Read/Write */
			outpk.tag.code = SUB_SPI_RDWR;
			outpk.tag.size = (uint8_t)sizeof(struct spi_rdwr_req_st)+tr_sz;
			outpk.tag.spi_rdwr_req.SS_config = tr_ss;
			outpk.tag.spi_rdwr_req.sz = tr_sz;
			memcpy( outpk.tag.spi_rdwr_req.data, out_buf+tr_done, tr_sz );
			inpk.tag.size = (uint8_t)sizeof(struct spi_rdwr_resp_st)+tr_sz;
		}
		else
			return SE_PARAM;

		rc = sub_transaction(hndl, &outpk, &inpk, 10000);
		if( !rc )
		{
			if( in_buf )
				memcpy( in_buf+tr_done, inpk.tag.spi_rdwr_resp.data, tr_sz );
			tr_done += tr_sz;
			tr_ss &= 0xF0;
		}
		if( (rc==SE_BULKIN_SZ) && (inpk.tag.spi_rdwr_resp.sz==0xFF) )
		{
			rc = SE_SPI_DIS;
			sub_errno = SE_SPI_DIS;
		}
	}
	return rc;
}

int sub_sdio_transfer( sub_handle hndl, char* out_buf, char* in_buf, 
										int out_sz, int in_sz, int ss_config )
{
	int			rc=0;
	sub_pack	outpk,inpk;

	if( (in_sz>SUB_MAX_SPI_SDIO_RDWR) || (out_sz>SUB_MAX_SPI_SDIO_RDWR) )
	{
		sub_errno = SE_PARAM;
		return SE_PARAM;
	}

	outpk.tag.code = SUB_SPI_SDIO_RDWR;
	outpk.tag.size = (uint8_t)sizeof(struct spi_sdio_rdwr_req_st)+out_sz;
	outpk.tag.spi_sdio_rdwr_req.SS_config = ss_config;
	outpk.tag.spi_sdio_rdwr_req.out_sz = out_sz;
	outpk.tag.spi_sdio_rdwr_req.in_sz = in_sz;
	memcpy( outpk.tag.spi_sdio_rdwr_req.data, out_buf, out_sz );

	inpk.tag.size = (uint8_t)sizeof(struct spi_sdio_rdwr_resp_st)+in_sz;
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
		memcpy( in_buf, inpk.tag.spi_sdio_rdwr_resp.data, in_sz );

	return rc;
}

int sub_spi_transfer_ess( sub_handle hndl, 
						  char* out_buf, char* in_buf, int sz, char* ess_str )
{
	int			rc=0;
	int			ess_sz = (int)strlen(ess_str);
	sub_pack 	outpk,inpk;

	if( (sz+ess_sz) > SUB_MAX_SPI_ESS_RDWR_LONG )
	{
		sub_errno = SE_PARAM;
		return SE_PARAM;
	}

	outpk.tag.code = SUB_SPI_RDWR_ESS;
	outpk.tag.size = sizeof(struct spi_rdwr_ess_req_st)+sz+ess_sz;
	outpk.tag.spi_rdwr_ess_req.sz = sz;
	outpk.tag.spi_rdwr_ess_req.SS_config_sz = ess_sz;
	strcpy( (char*)outpk.tag.spi_rdwr_ess_req.buf, ess_str );
	if( out_buf )
		memcpy( outpk.tag.spi_rdwr_ess_req.buf+ess_sz, out_buf, sz );

	inpk.tag.size = (uint8_t)sizeof(struct spi_rdwr_resp_st)+sz;
	rc = sub_transaction(hndl, &outpk, &inpk, 10000);
	if( !rc )
	{
		if( in_buf )
			memcpy( in_buf, inpk.tag.spi_rdwr_resp.data, sz );
	}
	if( (rc==SE_BULKIN_SZ) && (inpk.tag.spi_rdwr_resp.sz==0xFF) )
	{
		rc = SE_SPI_DIS;
		sub_errno = SE_SPI_DIS;
	}
	return rc;
}


/*
*-----------------------------------------------------------------------------
* MDIO
*-----------------------------------------------------------------------------
*/
int sub_mdio22( sub_handle hndl, int op, int phyad, int regad, int data, int* content )
{
	int		rc;
	union sub_mdio_frame frame;

	frame.clause22.op = op|SUB_MDIO22;
	frame.clause22.phyad = phyad;
	frame.clause22.regad = regad;
	frame.clause22.data = data;

	rc = sub_mdio_xfer( hndl, 1, &frame );
	if( !rc && content )
		*content = frame.clause22.data;
	return rc;
}

int sub_mdio45( sub_handle hndl, int op, int prtad, int devad, int data, int* content )
{
	int		rc;
	union sub_mdio_frame frame;

	frame.clause45.op = op;
	frame.clause45.prtad = prtad;
	frame.clause45.devad = devad;
	frame.clause45.data = data;

	rc = sub_mdio_xfer( hndl, 1, &frame );
	if( !rc && content )
		*content = frame.clause45.data;
	return rc;
}

int	sub_mdio_xfer_ex( sub_handle hndl, int channel, int count, 
												union sub_mdio_frame* mdios )
{
	int			i;
	int			rc;

	/* Embed channel into devad(6..5) */
	for( i=0; i<count; i++ )
	{
		mdios[i].clause45.devad &= 0x1F;
		mdios[i].clause45.devad |= (channel & 0x03)<<5;
		if( channel & SUB_CFP_MSA )
			mdios[i].clause45.devad |= 0x80;
	}

	rc = sub_mdio_xfer(hndl, count, mdios);
	
	/* Clear devad(7..5) */
	for( i=0; i<count; i++ )
	{
		mdios[i].clause45.devad &= 0x1F;
	}
	return rc;
}

int	sub_mdio_xfer( sub_handle hndl, int count, union sub_mdio_frame* mdios )
{
	int			rc;
	sub_pack	outpk,inpk;
	int			i;

	if( count > (int) SUB_MAX_MDIO_XFER )
	{
		sub_errno = SE_PARAM;
		return SE_PARAM;
	}

	outpk.tag.code = SUB_MDIO_XFER;
	outpk.tag.size = (uint8_t) sizeof(struct mdio_xfer_st)*count +1;
	outpk.tag.mdio_xfer_req.n = count;

	for( i=0; i<count; i++ )
	{
		outpk.tag.mdio_xfer_req.xfer[i].op_prtad = 		/* PRTAD */
									mdios[i].clause45.prtad & 0x1F;
		outpk.tag.mdio_xfer_req.xfer[i].op_prtad|=		/* OP */
									(mdios[i].clause45.op & 0x03) << 5; 
		outpk.tag.mdio_xfer_req.xfer[i].op_prtad|= 		/* MDIO22 */
									(mdios[i].clause45.op & 0x80); 
		outpk.tag.mdio_xfer_req.xfer[i].devad = mdios[i].clause45.devad;
		outpk.tag.mdio_xfer_req.xfer[i].data_lo = 
									mdios[i].clause45.data & 0xFF;
		outpk.tag.mdio_xfer_req.xfer[i].data_hi = 
									(mdios[i].clause45.data>>8) & 0xFF;
	}
	
	inpk.tag.size = outpk.tag.size;
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
	{
		for( i=0; i<count; i++ )
		{
			if( mdios[i].clause45.op & 0x02 )
			{
				/* Read or Post-read-increment-address frame */
				mdios[i].clause45.data = 
					inpk.tag.mdio_xfer_req.xfer[i].data_lo +
					(inpk.tag.mdio_xfer_req.xfer[i].data_hi<<8);
			}
		}
	}
	return rc;
}


/*
*-----------------------------------------------------------------------------
* GPIO
*-----------------------------------------------------------------------------
*/
int	sub_gpio_config( sub_handle hndl, sub_int32_t set, sub_int32_t* get, sub_int32_t mask )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_GPIO_CONFIG;
	outpk.tag.size = sizeof(struct gpio_req_st);
	INT_2_BUF( set, outpk.tag.gpio_req.val );
	INT_2_BUF( mask,outpk.tag.gpio_req.mask);

	inpk.tag.size = sizeof(struct gpio_resp_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
		BUF_2_INT( *get, inpk.tag.gpio_resp.val );
	return rc;
}

int	sub_gpio_read( sub_handle hndl, sub_int32_t* get )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_GPIO_READ;
	outpk.tag.size = 0;

	inpk.tag.size = sizeof(struct gpio_resp_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
		BUF_2_INT( *get, inpk.tag.gpio_resp.val );
	return rc;
}

int	sub_gpio_write( sub_handle hndl, sub_int32_t set, sub_int32_t* get, sub_int32_t mask )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_GPIO_WRITE;
	outpk.tag.size = sizeof(struct gpio_req_st);
	INT_2_BUF( set, outpk.tag.gpio_req.val );
	INT_2_BUF( mask,outpk.tag.gpio_req.mask);

	inpk.tag.size = sizeof(struct gpio_resp_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
		BUF_2_INT( *get, inpk.tag.gpio_resp.val );
	return rc;
}

/*
*-----------------------------------------------------------------------------
* GPIOB
*-----------------------------------------------------------------------------
*/
int	sub_gpiob_config( sub_handle hndl, sub_int32_t set, sub_int32_t* get, sub_int32_t mask )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_GPIOB_CONFIG;
	outpk.tag.size = sizeof(struct gpiob_req_st);
	outpk.tag.gpiob_req.val  = set;
	outpk.tag.gpiob_req.mask = mask;

	inpk.tag.size = sizeof(struct gpiob_resp_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
		*get = inpk.tag.gpiob_resp.val;
	return rc;
}

int	sub_gpiob_read( sub_handle hndl, sub_int32_t* get )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_GPIOB_READ;
	outpk.tag.size = 0;

	inpk.tag.size = sizeof(struct gpiob_resp_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
		*get = inpk.tag.gpiob_resp.val;
	return rc;
}

int	sub_gpiob_write( sub_handle hndl, sub_int32_t set, sub_int32_t* get, sub_int32_t mask )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_GPIOB_WRITE;
	outpk.tag.size = sizeof(struct gpiob_req_st);
	outpk.tag.gpiob_req.val  = set;
	outpk.tag.gpiob_req.mask = mask;

	inpk.tag.size = sizeof(struct gpiob_resp_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
		*get = inpk.tag.gpiob_resp.val;
	return rc;
}

/*
*-----------------------------------------------------------------------------
* Edge
*-----------------------------------------------------------------------------
*/
int sub_edge_config( sub_handle hndl, sub_int32_t set, sub_int32_t* get )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_EDGE_CONFIG;
	if( !get )
	{
		outpk.tag.edge_config._EICRA = set;		
		outpk.tag.edge_config._EICRB = set>>8;
		outpk.tag.size = sizeof(struct edge_config_st);
	}
	else
		outpk.tag.size = 0;

	inpk.tag.size = sizeof(struct edge_config_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
	{
		if( get )
			*get= inpk.tag.edge_config._EICRA|(inpk.tag.edge_config._EICRB<<8);
	}
	return rc;
}

int sub_edge_read( sub_handle hndl, sub_int32_t *get, sub_int32_t *edge )
{
	int			rc;
    sub_pack    outpk,inpk;
 
    outpk.tag.code = SUB_EDGE_READ;
    outpk.tag.size = 0;
 
    inpk.tag.size = sizeof(struct edge_read_st);
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
    if( !rc )
    { 
        BUF_2_INT( *get, inpk.tag.edge_read.val );
		*edge = inpk.tag.edge_read._EIFR;
    }
    return rc;
}


/*
*-----------------------------------------------------------------------------
* Fast PWM 
*-----------------------------------------------------------------------------
*/
int sub_fpwm_config( sub_handle hndl, double freq_hz, int flags )
{
	int			rc;
	sub_pack	outpk,inpk;
	double		ICR;
    int			presc;

    outpk.tag.code = SUB_FAST_PWM_CONFIG;
	outpk.tag.size = sizeof(struct fast_pwm_config_req_st);

	if( flags & FPWM_ENABLE )
	{
		if( freq_hz > SUB_F_CPU/2 )
			return SE_PARAM;
		if( freq_hz > SUB_F_CPU/0xFFFF )
		{
			ICR = SUB_F_CPU/freq_hz;
			presc = 0x01;
		}
		else if( freq_hz > (double)SUB_F_CPU/8/0xFFFF )
		{
			ICR = SUB_F_CPU/8/freq_hz;
			presc = 0x02;
		}
		else if( freq_hz > (double)SUB_F_CPU/64/0xFFFF )
		{
			ICR = SUB_F_CPU/64/freq_hz;
			presc = 0x03;
		}
		else if( freq_hz > (double)SUB_F_CPU/256/0xFFFF )
		{
			ICR = SUB_F_CPU/256/freq_hz;
			presc = 0x04;
		}
		else if( freq_hz > (double)SUB_F_CPU/1024/0xFFFF )
		{
			ICR = SUB_F_CPU/1024/freq_hz;
			presc = 0x05;
		}
		else
		{
			sub_errno = SE_PARAM;
			return SE_PARAM;
		}

		outpk.tag.fast_pwm_config_req.TCCRA = (flags>>8)|0x02;	/* WGM1=1 */
		outpk.tag.fast_pwm_config_req.TCCRB = presc|0x18;	 	/* WGM2,3=1 */
		outpk.tag.fast_pwm_config_req.TCCRC = 0;
		outpk.tag.fast_pwm_config_req.ICRL = (int)ICR;
		outpk.tag.fast_pwm_config_req.ICRH = ((int)ICR)>>8;
	}
	else
		outpk.tag.fast_pwm_config_req.TCCRB = 0;	/* Stop PWM timer */

	inpk.tag.size = sizeof(struct fast_pwm_config_req_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );

	return rc;
}

int sub_fpwm_set( sub_handle hndl, int pwm_n, double duty )
{
	int			rc;
	sub_pack	outpk,inpk;
	int 		duty16;

	if( (pwm_n < 0) || (pwm_n > 2) )
		return SE_PARAM;
	if( (duty < 0) || (duty > 100) )
		return SE_PARAM;

    outpk.tag.code = SUB_FAST_PWM_SET;
	outpk.tag.size = sizeof(struct fast_pwm_set_req_st);
	outpk.tag.fast_pwm_set_req.pwm_n = pwm_n;
		
	duty16 = (int)((0xFFFF*duty)/100);
	outpk.tag.fast_pwm_set_req.duty_lo = duty16 & 0xFF;
	outpk.tag.fast_pwm_set_req.duty_hi = duty16 >> 8;

	inpk.tag.size = sizeof(struct fast_pwm_set_req_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );

	return rc;
}

/*
*-----------------------------------------------------------------------------
* PWM 
*-----------------------------------------------------------------------------
*/
int sub_pwm_config( sub_handle hndl, int res_us, int limit )
{
	int			rc;
	sub_pack	outpk,inpk;

    outpk.tag.code = SUB_PWM_CONFIG;
	outpk.tag.size = sizeof(struct pwm_config_req_st);

	outpk.tag.pwm_config_req.TCCRA = 0x02;		/* WGM1,WGM0 = 10 */
	if( limit == 0 )
		outpk.tag.pwm_config_req.TCCRB = 0;
	else
	{
		int prescaller;
		if( (res_us < 20) || (limit > 0xFF) )
		{
			sub_errno = SE_PARAM;
			return SE_PARAM;
		}

		/* Calculate prescaller and MAX for given resolution */
		if( res_us <= (256L*1000000L/SUB_F_CPU) )
		{
			outpk.tag.pwm_config_req.TCCRB = 0x01;	/* CLK/1 */
			prescaller = 1;
		}
		else if( res_us <= (256L*1000000L/SUB_F_CPU*8) )
		{
			outpk.tag.pwm_config_req.TCCRB = 0x02;	/* CLK/8 */
			prescaller = 8;
		}
		else if( res_us <= (256L*1000000L/SUB_F_CPU*64) )
		{
			outpk.tag.pwm_config_req.TCCRB = 0x03;	/* CLK/64 */
			prescaller = 64;
		}
		else if( res_us <= (256L*1000000L/SUB_F_CPU*256) )
		{
			outpk.tag.pwm_config_req.TCCRB = 0x04;	/* CLK/256 */
			prescaller = 256;
		}
		else if( res_us <= (256L*1000000L/SUB_F_CPU*1024) )
		{
			outpk.tag.pwm_config_req.TCCRB = 0x05;	/* CLK/1024 */
			prescaller = 1024;
		}
		else
		{
			sub_errno = SE_PARAM;
			return SE_PARAM;
		}

		outpk.tag.pwm_config_req.OCRA = (uint8_t) (res_us*(SUB_F_CPU/1000000L)/prescaller);
		outpk.tag.pwm_config_req.limit = limit;
	}

	inpk.tag.size = sizeof(struct pwm_config_req_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
    return rc;
}

int sub_pwm_set( sub_handle hndl, int pwm_n, int duty )
{
	int			rc;
	sub_pack	outpk,inpk;

	if( (pwm_n < 0) || (pwm_n > 7) )
	{
		sub_errno = SE_PARAM;
		return SE_PARAM;
	}
	if( (duty < 0) || (duty > 0xFF) )
	{
		sub_errno = SE_PARAM;
		return SE_PARAM;
	}

    outpk.tag.code = SUB_PWM_SET;
	outpk.tag.size = sizeof(struct pwm_set_req_st);
	outpk.tag.pwm_set_req.pwm_n = pwm_n;
	outpk.tag.pwm_set_req.duty = duty;
		
	inpk.tag.size = sizeof(struct pwm_set_req_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	return rc;
}


/*
*-----------------------------------------------------------------------------
* ADC
*-----------------------------------------------------------------------------
*/
int sub_adc_config( sub_handle hndl, int flags )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_ADC_CONFIG;
	outpk.tag.size = sizeof(struct adc_config_req_st);
	outpk.tag.adc_config_req.admux  = flags;
	outpk.tag.adc_config_req.adcsra = flags>>8;

	inpk.tag.size = sizeof(struct adc_config_req_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	return rc;
}

int sub_adc_single( sub_handle hndl, int* data, int mux )
{
	return sub_adc_read( hndl, data, &mux, 1 );
}
int sub_adc_read( sub_handle hndl, int* data, int* mux, int reads )
{
	int			rc,i;
	sub_pack	outpk,inpk;
	
	if( reads > MAX_ADC_READS )
		return SE_PARAM;

	outpk.tag.code = SUB_ADC_READ;
	outpk.tag.size = (uint8_t)sizeof(struct adc_read_req_st) + reads;
	outpk.tag.adc_read_req.reads = reads;
	for( i=0; i<reads; i++ )
		outpk.tag.adc_read_req.mux[i] = mux[i];

	inpk.tag.size = (uint8_t)sizeof(struct adc_read_resp_st) + reads*2;
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );

	if( !rc )
	{
		for( i=0; i<reads; i++ )
		{
			data[i] = inpk.tag.adc_read_resp.adc_lh[i*2] +
				  (inpk.tag.adc_read_resp.adc_lh[i*2+1]<<8);
			/* Convert differential result */
			if( (mux[i]>ADC_S7) && (mux[i]<=ADC_D52) && (data[i]&0x200) )
				data[i] = data[i]|((-1)<<10);
		}
	}
	return rc;
}

/*
*-----------------------------------------------------------------------------
* Counters
*-----------------------------------------------------------------------------
*/
int sub_cntr_config( sub_handle hndl, int cntr0_cfg, int cntr1_cfg )
{
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_CNTR_CONFIG;
    outpk.tag.cntr_config_req._TCCR0B = cntr0_cfg;
    outpk.tag.cntr_config_req._TCCR1B = cntr1_cfg;
	outpk.tag.size = sizeof(struct cntr_config_req_st);
	
    inpk.tag.size = sizeof(struct cntr_config_req_st);
	return sub_transaction( hndl, &outpk, &inpk, 10000 );
}

int sub_cntr_read( sub_handle hndl, int* cntr0, int* cntr1 )
{
    int         rc;
	sub_pack	outpk,inpk;
	
    outpk.tag.code = SUB_CNTR_READ;
	outpk.tag.size = 0;

    inpk.tag.size = sizeof(struct cntr_read_resp_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
    
    if( !rc )
    {
        *cntr0 =     inpk.tag.cntr_read_resp.cntr0[0]+        
                    (inpk.tag.cntr_read_resp.cntr0[1]>>8)+
                    (inpk.tag.cntr_read_resp.cntr0[2]>>16)+
                    (inpk.tag.cntr_read_resp.cntr0[3]>>24);
        *cntr1 =     inpk.tag.cntr_read_resp.cntr1[0]+        
                    (inpk.tag.cntr_read_resp.cntr1[1]>>8)+
                    (inpk.tag.cntr_read_resp.cntr1[2]>>16)+
                    (inpk.tag.cntr_read_resp.cntr1[3]>>24);
    }
    return rc;
}


/*
*-----------------------------------------------------------------------------
* LCD
*-----------------------------------------------------------------------------
*/
int	sub_lcd_write( sub_handle hndl, char* str )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_LCD_WRITE;
	strncpy( outpk.tag.LCD_write_req.str, str, SUB_MAX_LCD_STR );
	outpk.tag.size = (uint8_t)strlen(outpk.tag.LCD_write_req.str)+1;

	inpk.tag.size = 1;
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
		
	switch( inpk.tag.LCD_write_resp.status )
	{
	case __LCD_NOT_SUPPORTED:
		rc = SE_NSUPPORTED;
		break;
	}
	return rc;
}

/*
*-----------------------------------------------------------------------------
* RS232/RS485
*-----------------------------------------------------------------------------
*/
int sub_rs_set_config( sub_handle hndl, int config, sub_int32_t baud )
{
	int			rc;
	sub_pack	outpk,inpk;
	int			ubrr;

	outpk.tag.code = SUB_RS_CONFIG;
	outpk.tag.size = sizeof(struct RS_config_req_st);
	outpk.tag.RS_config_req.UCSRC = config;

	/* Translate Baud to UBRRL,UBRRH */
	if( baud <= 0 )
		return SE_PARAM;

	ubrr = SUB_F_CPU/(8*baud)-1;
	ubrr |= 0x8000;

	outpk.tag.RS_config_req.UBRRL = ubrr;
	outpk.tag.RS_config_req.UBRRH = ubrr>>8;

	inpk.tag.size = 3;
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );

	return rc;
}

int sub_rs_get_config( sub_handle hndl, int* config, sub_int32_t* baud )
{
	int			rc;
	sub_pack	outpk,inpk;
	int			ubrr;

	outpk.tag.code = SUB_RS_CONFIG;
	outpk.tag.size = 0;

	inpk.tag.size = 3;
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
	{
		*config = inpk.tag.RS_config_resp.UCSRC;
		ubrr = (inpk.tag.RS_config_resp.UBRRH<<8)+inpk.tag.RS_config_resp.UBRRL; 
		if( ubrr & 0x8000 )
			*baud = SUB_F_CPU/(8*((ubrr&0x7FFF)+1));
		else
			*baud = SUB_F_CPU/(16*(ubrr+1));
	}
	return rc;
}

int sub_rs_timing( sub_handle hndl, 
				   int flags, int tx_space_us, int rx_msg_us, int rx_byte_us )
{
	int			rc;
	sub_pack 	outpk,inpk;

	outpk.tag.code = SUB_RS_TIMING;
	outpk.tag.size = sizeof(struct RS_timing_req_st);

	outpk.tag.RS_timing_req.flags = flags;
	INT_2_LE16( tx_space_us/64	,outpk.tag.RS_timing_req.tx_space   );
	INT_2_LE16( rx_msg_us/64	,outpk.tag.RS_timing_req.rx_msg_to  );
	INT_2_LE16( rx_byte_us/64	,outpk.tag.RS_timing_req.rx_byte_to );

	inpk.tag.size = sizeof(struct RS_timing_req_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );

	return rc;
}

/*
* rc >= 0 - received bytes
* -1 - error
*/
int sub_rs_xfer( sub_handle hndl, 
					char* tx_buf, int tx_sz, char* rx_buf, int rx_sz )
{
	int			rc;
	sub_pack 	outpk,inpk;

	outpk.tag.code = SUB_RS_XFER;
	outpk.tag.RS_xfer_req.rx_sz = rx_sz;
	memcpy( outpk.tag.RS_xfer_req.tx_buf, tx_buf, tx_sz );
	outpk.tag.size = tx_sz+1;

	inpk.tag.size = 0;
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( rc )
		return -1;

	rc = inpk.tag.RS_xfer_resp.rx_sz;
	memcpy( rx_buf, inpk.tag.RS_xfer_resp.rx_buf, rc );
	return rc;
}


/*
*-----------------------------------------------------------------------------
* SUB-20 Bit-bang
*-----------------------------------------------------------------------------
*/
int sub_bb_i2c_config( sub_handle hndl, int mode, int stretch_ms )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_BB_I2C_CONFIG;
	outpk.tag.size = sizeof(struct bb_i2c_config_req_st);
	outpk.tag.bb_i2c_config_req.mode = mode;
	if( (stretch_ms<=16) && (stretch_ms>0) )
		outpk.tag.bb_i2c_config_req.stretch = 1;
	else
		outpk.tag.bb_i2c_config_req.stretch = (stretch_ms*1000)/16384;

	inpk.tag.size = sizeof(struct bb_i2c_config_req_st);
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	return rc;
}

int sub_bb_i2c_scan( sub_handle hndl, int channel, 
					 			int* slave_cnt, char* slave_buf )
{
	int 		rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_BB_I2C_SCAN;
	outpk.tag.bb_i2c_scan_req.channel = channel;
	outpk.tag.size = sizeof(struct bb_i2c_scan_req_st);
    
	inpk.tag.size = 0;	/* Input size is unknown at this stage */
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	if( !rc )
	{
		*slave_cnt = inpk.tag.i2c_scan_resp.slave_n;
		memcpy( slave_buf, inpk.tag.i2c_scan_resp.slave_buf, *slave_cnt );
	}
	return rc;
}



int sub_bb_i2c_read( sub_handle hndl, int channel, int sa, 
								sub_int32_t ma, int ma_sz, char* buf, int sz )
{
	int			rc=0;
	sub_pack	outpk,inpk;

	if( (sz<=0) || (sz>SUB_MAX_BB_I2C_READ) )
		return SE_PARAM;

	outpk.tag.code = SUB_BB_I2C_RDWR;
	outpk.tag.size = sizeof(struct bb_i2c_rdwr_req_st) + ma_sz;
	outpk.tag.bb_i2c_rdwr_req.channel = channel;
	outpk.tag.bb_i2c_rdwr_req.sa = (sa<<1)|0x01;
	outpk.tag.bb_i2c_rdwr_req.flags = 0;
	outpk.tag.bb_i2c_rdwr_req.ma_sz = ma_sz;
	outpk.tag.bb_i2c_rdwr_req.sz = sz;
	ma_copy( outpk.tag.bb_i2c_rdwr_req.ma, ma, ma_sz );

	inpk.tag.size = (uint8_t)sizeof(struct bb_i2c_rdwr_resp_st)+sz;
	inpk.tag.bb_i2c_rdwr_resp.status = 0;

	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	sub_i2c_status = inpk.tag.i2c_rdwr_resp.status;
	if( !rc )
	{
		if( sub_i2c_status )
		{
			rc = SE_I2C;
			sub_errno = rc;
		}
		else
		{
			memcpy( buf, inpk.tag.bb_i2c_rdwr_resp.data, sz );
		}
	}
	else
	{
		if( (rc==SE_BULKIN_SZ) && sub_i2c_status )
		{
			rc = SE_I2C;
			sub_errno = rc;
		}
	}
	return rc;
}

int sub_bb_i2c_write( sub_handle hndl, int channel, int sa, 
								sub_int32_t ma, int ma_sz, char* buf, int sz )
{
	int			rc=0;
	sub_pack	outpk,inpk;

	if( (sz<=0) || (sz>(int)SUB_MAX_BB_I2C_WRITE-ma_sz) )
		return SE_PARAM;

	outpk.tag.code = SUB_BB_I2C_RDWR;
	outpk.tag.size = sizeof(struct bb_i2c_rdwr_req_st)+ma_sz+sz;
	outpk.tag.bb_i2c_rdwr_req.channel = channel;
	outpk.tag.bb_i2c_rdwr_req.sa = sa<<1;
	outpk.tag.bb_i2c_rdwr_req.flags = 0;
	outpk.tag.bb_i2c_rdwr_req.ma_sz = ma_sz;
	outpk.tag.bb_i2c_rdwr_req.sz = sz;
	ma_copy( outpk.tag.bb_i2c_rdwr_req.ma, ma, ma_sz );
	memcpy( outpk.tag.bb_i2c_rdwr_req.ma+ma_sz, buf, sz );
	
	inpk.tag.size = sizeof(struct bb_i2c_rdwr_resp_st);
	inpk.tag.bb_i2c_rdwr_resp.status = 0;

	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	sub_i2c_status = inpk.tag.bb_i2c_rdwr_resp.status;
    if( !rc )
	{
		if( sub_i2c_status )
		{
			rc = SE_I2C;
			sub_errno = rc;
		}
	}
	return rc;
}


/*
*-----------------------------------------------------------------------------
* InfraRed
*-----------------------------------------------------------------------------
*/
int sub_ir_config( sub_handle hndl, int carrier, int duty, int mode, ... )
{
	int		rc;
	sub_pack	outpk,inpk;
    int		icr;
	int		ocr;
	
	va_list args;
	va_start( args, mode );

    outpk.tag.code = SUB_IR_CONFIG;
	outpk.tag.size = sizeof(struct ir_config_req_st);

	/* Carrier */
	if( (carrier <=0) || (carrier >= SUB_F_CPU/4) )
		return SE_PARAM;
    icr = SUB_F_CPU/carrier-1;
	if( icr > 0xFFFF )
		return SE_PARAM;
	outpk.tag.ir_config_req.ICRL = icr & 0xFF;
	outpk.tag.ir_config_req.ICRH = icr >>8;

	/* Duty */
	if( (duty<0) || (duty>100) )
		return SE_PARAM;
	ocr = (icr*duty)/100;
	outpk.tag.ir_config_req.OCRL = ocr & 0xFF;
	outpk.tag.ir_config_req.OCRH = ocr >>8;

    outpk.tag.ir_config_req.mode = mode;
	va_start( args, mode );
	switch( mode )
	{
	case IR_UART_MODE:
		outpk.tag.ir_config_req.ir_config_mode.uart_mode.pp_bit = 
													va_arg( args, int );
		break;
	case IR_ROW_MODE:
		outpk.tag.ir_config_req.ir_config_mode.row_mode.pp_bit = 
													va_arg( args, int );
		break;
	default:
		va_end( args );
		return SE_PARAM;
	}

	inpk.tag.size = sizeof(struct ir_config_req_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	
	va_end( args );
	return rc;
}

int sub_ir_tx( sub_handle hndl, int sz, char* buf )
{
	int			rc=0;
	sub_pack	outpk,inpk;

	if( (sz<=0) || (sz>SUB_MAX_IR_TX) )
		return SE_PARAM;

	outpk.tag.code = SUB_IR_TX;
	outpk.tag.size = sizeof(struct ir_tx_req_st)+sz;
	memcpy( outpk.tag.ir_tx_req.buf, buf, sz );
    outpk.tag.ir_tx_req.sz = sz;
	
	inpk.tag.size = sizeof(struct ir_tx_resp_st);
	rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
    if( !rc )
	{
		if( inpk.tag.ir_tx_resp.status )
		{
			rc = SE_IR;
			sub_errno = SE_IR;
		}
	}
	return rc;
}


/*
*-----------------------------------------------------------------------------
* SUB-20 FIFO
*-----------------------------------------------------------------------------
*/
int sub_fifo_write( sub_handle hndl, char* buf, int sz, int to_ms )
{
	int		rc;

	sub_trace( 3, "FIFO WRITE %d bytes:\n", sz );
	hexdump_buf( 3, buf,sz );

#ifdef LIBUSB_1_0
	int transferred=0;
	rc = libusb_bulk_transfer( hndl, SUB_FIFO_OUT_EP, 
		(unsigned char*)buf, sz, &transferred, to_ms );
	/* mimic the old interface behavior to be consistent */
	if( 0 == rc )
	{
		rc = transferred;
	}
	else if( LIBUSB_ERROR_TIMEOUT == rc )
	{
		if( transferred )
		{
			rc = transferred;
		}
		else
		{
			rc = -ETIMEDOUT;
		}
	}
#else
	rc = usb_bulk_write( hndl, SUB_FIFO_OUT_EP, buf, sz, to_ms );
#endif


	if( rc<0 )
	{
	switch( rc )
		{
		case -ETIMEDOUT:
			sub_errno = SE_TIMEOUT;
			break;
		default:
			sub_errno = SE_BULKOUT;
		}

		sub_trace( 3, "ERROR %d\n", sub_errno );
	}

	return rc;
}

int sub_fifo_read( sub_handle hndl, char* buf, int sz, int to_ms )
{
	int		rc;
	int		aligned_sz,use_tmp;
	char*	tmp_buf;

	if( sz%64 == 0 )
	{
		/* If user buffer is 64 aligned use it directly */
		tmp_buf = buf;
		aligned_sz = sz;
        use_tmp=0;
	}
	else
	{
		/* Otherwise use temporarry buffer */
		aligned_sz = (sz/64)*64;
		if( sz % 64 )
			aligned_sz+=64;
		tmp_buf = malloc( aligned_sz );
		if( !tmp_buf )
			return -ENOMEM;
        use_tmp=1;
	}

	sub_trace( 3, "FIFO READ %d bytes:\n", aligned_sz );

#ifdef LIBUSB_1_0
	int transferred=0;
	rc = libusb_bulk_transfer( hndl, SUB_FIFO_IN_EP, 
		(unsigned char*)tmp_buf, aligned_sz, &transferred, to_ms );
	/* mimic the old interface behavior to be consistent */

        if( 0 == rc )
        {
                rc = transferred;
        }
        else if( LIBUSB_ERROR_TIMEOUT == rc )
        {
                if( transferred )
                {
                        rc = transferred;
                }
                else
                {
                        rc = -ETIMEDOUT;
                }
        }

#else
	rc = usb_bulk_read( hndl, SUB_FIFO_IN_EP, tmp_buf, aligned_sz, to_ms );
#endif
	if( rc<0 )
	{
		switch( rc )
		{
		case -ETIMEDOUT:
			sub_errno = SE_TIMEOUT;
			break;
		default:
			sub_errno = SE_BULKIN_RC;
		}
		sub_trace( 3, "ERROR %d rc=%d\n", sub_errno, rc );
	}
	else
	{
		if( rc > sz )
		{
			sub_errno = SE_OUT_OVF;
			sub_trace( 3, "ERROR %d rc=%d\n", sub_errno, rc );
			rc = -EFBIG;
		}
		else
		{
			hexdump_buf( 3, tmp_buf,rc);
			if( use_tmp )
				memcpy( buf, tmp_buf, rc );
		}
	}

	if( use_tmp )
		free( tmp_buf );
	return rc;
}

int sub_fifo_config( sub_handle hndl, int config )
{
	int			rc;
	sub_pack	outpk,inpk;

	outpk.tag.code = SUB_FIFO_CONFIG;
	outpk.tag.size = sizeof(struct FIFO_config_req_st);
	outpk.tag.FIFO_config_req.selector = config;
	outpk.tag.FIFO_config_req.flags = config>>8;

	inpk.tag.size = sizeof(struct FIFO_config_req_st);
    rc = sub_transaction( hndl, &outpk, &inpk, 10000 );
	
	return rc;
}

/*
*-----------------------------------------------------------------------------
* Common
*-----------------------------------------------------------------------------
*/

/*
* Request-Responce transaction with SUB-20 device
* Input:
*	outpk - filled with output tag
*   inpk  - filled with tag.size
* NOTES:
*  1. If inpk->tag.size == 0 there will be no input tag size check. 
*     It is used then input tag size is unknown.
*/
int sub_transaction( sub_handle hndl, sub_pack* outpk, sub_pack* inpk, int timeout )
{
    int		sz, in_sz, in_tag_sz, out_sz;

	in_tag_sz = inpk->tag.size;
	in_sz = 64;
	if( in_sz < (in_tag_sz+3) )	
		in_sz = in_tag_sz+3;	/* For IN bigger than frame use exact size */

	out_sz = outpk->tag.size+3;
	outpk->pack_sz = out_sz-1;

	sz = usb_transaction( hndl, 
				(char*)outpk, out_sz, (char*)inpk, in_sz, timeout);
	if( sz < 0 ) 
		return sub_errno;

	/* Check tag code and size */
	if( (sz == 3) && (inpk->tag.code == SUB_UNKNOWN) )
	{
		sub_errno = SE_NSUPPORTED;
		return sub_errno;
	}
	if( sz < (in_tag_sz+3) )
	{
		sub_errno = SE_BULKIN_SZ;
		return sub_errno;
	}
	if( outpk->tag.code != inpk->tag.code )
	{
		sub_errno = SE_TAG_CODE;
		return sub_errno;
	}
	if( in_tag_sz && (inpk->tag.size != in_tag_sz) )
	{
		sub_errno = SE_TAG_SIZE;
		return sub_errno;
	}
	return 0;
}

#ifdef LIBUSB_1_0
/* Asynchronous Transfer Callback */
void transfer_callback( struct libusb_transfer *transfer )
{
	*((int*)(transfer->user_data)) = transfer->status;
    if( transfer->status != LIBUSB_TRANSFER_COMPLETED )
        sub_trace( 1, "Transfer status: %d\n", transfer->status );
    else
        sub_trace( 8, "Transfer completed, transferred=%d\n",
                                        transfer->actual_length );
}
#endif

int usb_transaction( sub_handle hndl,
									char* out_buf, int out_sz,
									char*  in_buf, int	in_sz,
									int timeout )
{

#ifdef LIBUSB_1_0
#else
	int rc,sz=0;
 #ifdef __linux__

 #else
	void *context = NULL;
 #endif
#endif /*LIBUSB_1_0*/

	sub_trace( 3, "BULK_OUT %d bytes:\n", out_sz );
	hexdump_buf( 3, out_buf,out_sz );

#ifdef LIBUSB_1_0
	int		rc;
	int		transferred,status=-1;


#define ASYNC_OUT


#ifdef ASYNC_OUT
	/* Allocate OUT Transfer */
	struct libusb_transfer* transfer;
	transfer = libusb_alloc_transfer( 0 );
	if( !transfer )
	{
		sub_errno = SE_SETA;
		return -1;
	}

	/* Fill OUT Transfer */
	libusb_fill_bulk_transfer( transfer, hndl, 
								SUB_OUT_EP,
								(unsigned char*)out_buf, out_sz, 
 								transfer_callback,
 								&status,	/* User Data */
  								timeout );

	transfer->flags |= LIBUSB_TRANSFER_FREE_TRANSFER; 

	/* Submit OUT Transfer */
	if( libusb_submit_transfer( transfer ) )
	{
		libusb_free_transfer( transfer );
		sub_errno = SE_SUBMIT;
		return -1;
	}
#else	
    /* Synchronous bulk write */
	rc = libusb_bulk_transfer( hndl, 
								SUB_OUT_EP,
								(unsigned char*)out_buf, out_sz, 
								&transferred, 
								timeout );
    if( rc )
    {
	    rc = -1;
		sub_errno = SE_BULKOUT;
        return rc;
    }
    status = LIBUSB_TRANSFER_COMPLETED;
#endif /*ASYNC_OUT*/

	/* Synchronous bulk read */
	rc = libusb_bulk_transfer( hndl, 
								SUB_IN_EP,
								(unsigned char*)in_buf, in_sz, 
								&transferred, 
								timeout );
	if( rc )
	{
		rc = -1;
		sub_errno = SE_BULKIN_RC;
	}
	else
	{
		sub_trace( 3, "BULK_IN %d bytes:\n", transferred );
		hexdump_buf( 3, in_buf, transferred );
	}

	/* Finish asynchronous OUT transfer */
	if( status == -1 )
	{
		/* Transfer not completed */
        sub_trace(1, "Transfer incomplete\n");

	}
    if( !rc )
    {
	    if( status == LIBUSB_TRANSFER_COMPLETED )
		    rc = transferred;
	    else
	    {
		    rc = -1;
		    sub_errno = SE_BULKOUT;
	    }
    }
	return rc;

#else
 #ifdef __linux__

	 /* libusb 0.1.x implementation */
	
	rc = usb_bulk_write( hndl, SUB_OUT_EP, out_buf, out_sz, timeout );
	if( sz < 0 )
	{
		rc = SE_BULKOUT;
		usb_clear_halt( hndl, SUB_OUT_EP );
		return rc;
	}


	sz = usb_bulk_read( hndl, SUB_IN_EP, in_buf, in_sz, timeout );
	if( sz < 0 )
	{
		rc = SE_BULKIN_RC;
		usb_clear_halt( hndl, SUB_IN_EP );
	}
	else
	{
		sub_trace( 3, "BULK_IN %d bytes:\n", sz );
		hexdump_buf( 3, in_buf, sz );
	}

	return rc;

 #else
/* Libusb-Win32 0.1 Implementation */

	rc = usb_bulk_setup_async( hndl, &context, SUB_OUT_EP );
	if( rc )
	{
		rc = SE_SETA;
		goto fin;
	}

	rc = usb_submit_async( context, out_buf, out_sz );
	if( rc )
	{
		rc = SE_SUBMIT;
		goto fin;
	}

	sz = usb_bulk_read( hndl, SUB_IN_EP, in_buf, in_sz, timeout );
    if( sz < 0 )
	{
		rc = SE_BULKIN_RC;
		usb_clear_halt( hndl, SUB_IN_EP );
	}
	else
	{
		sub_trace( 3, "BULK_IN %d bytes:\n", sz );
		hexdump_buf( 3, in_buf, sz );
	}

    if( usb_reap_async(context, timeout) != out_sz )
		rc = SE_BULKOUT;

fin:
	if( context )
		usb_free_async( &context );
    if( rc )
	{
        sub_errno = rc;
		return -1;
	}
	return sz;
 #endif /* __linux__ */
#endif /*LIBUSB_1_0*/
}


/*
* sub_strerror
*/
#undef DEF_SUB_ERROR
#define DEF_SUB_ERROR(NAME,VAL,DESCR)	DESCR,
char* sub_errlist[] = 
{
	SUB_ERRORS
};

char* sub_strerror( int errnum )
{
    static char buf[100];

	if( (errnum < 0 ) || (errnum >= SUB_LASTERROR) )
	{
		sprintf( buf, "Unrecognized error %d", errnum );
		return buf;
	}

	return sub_errlist[errnum];
}

/*
* sub_control_request
*/
int sub_control_request( sub_handle hndl, 
							int type, int request, int value, int index,
							char* buf, int sz, int timeout 
						)
{
#ifdef LIBUSB_1_0
	return libusb_control_transfer( hndl, 
		    type, request, value, index, (unsigned char*)buf, sz, timeout );
#else
	return usb_control_msg( hndl, 
						type, request, value, index, buf, sz, timeout );
#endif
}

/*
* sub_get_sernum
*/
int sub_get_serial_number( sub_handle hndl, char *buf, int sz)
{
#ifdef LIBUSB_1_0
	return libusb_get_string_descriptor_ascii(hndl, 3, (unsigned char*)buf, sz);
#else
	return usb_get_string_simple(hndl, 3, buf, sz);
#endif
}

/*
* sub_get_product_id
*/
int sub_get_product_id( sub_handle hndl, char *buf, int sz)
{
#ifdef LIBUSB_1_0
	return libusb_get_string_descriptor_ascii(hndl, 2, (unsigned char*)buf, sz);
#else
	return usb_get_string_simple(hndl, 2, buf, sz);
#endif
}

/*
* Trace Output
*/
void sub_trace( int level, char* format, ... )
{
	va_list args;
	va_start( args, format );
#if defined(_MSC_VER) &&  _MSC_VER > 800
	if( (shared_mem) && (level <= shared_mem->sub_debug_level) )
	{
		char str[256];
		vsnprintf_s(str, sizeof(str),
			_TRUNCATE, format, args );
		OutputDebugString(str);
	}
#else	/*_MSC_VER*/
	if( level <= sub_debug_level )
		vprintf( format, args );
#endif	/*_MSC_VER*/
	va_end( args );
}

/*
* Hex Dump buffer Trace
*/
void hexdump_buf( int level, char* buf, int sz )
{
    int i;
    for( i=0; i<sz; i++ )
    {
        sub_trace( level, "%02x ", (unsigned int)(buf[i])&0x00FF );
        if( (i & 0x0F) == 0x0F )
            sub_trace( level, "\n" );
    }
    if( (i & 0x0F) != 0x0F )
        sub_trace( level, "\n" );
}

#if defined(_MSC_VER) && _MSC_VER > 800

/*
* Set debug level
if level==-1 sub_debug_level will not be changed
Returns previous debug level
*/
int sub_set_debug_level( int level )
{
	if(shared_mem)
	{
		int old_level=shared_mem->sub_debug_level;
		if( level!=-1)
		{
			shared_mem->sub_debug_level=level;
		}
		return old_level;
	}
	else
	{
		return -1;
	}
}

BOOL sub_shared_mem_init()
{
	hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(shared_mem_t),
		SHARED_MEM_NAME);

	if(!hFileMapping)
	{
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			hFileMapping = OpenFileMapping(
				FILE_MAP_WRITE,
				FALSE,
				SHARED_MEM_NAME);
			if(!hFileMapping)
			{
				return FALSE ;
			}

		}
		else
		{
			return FALSE ;
		}

	}

	shared_mem = (shared_mem_t *)MapViewOfFile(
		hFileMapping,
		FILE_MAP_WRITE, 
		0,
		0,
		sizeof(shared_mem_t));

	if(!shared_mem)
	{
		CloseHandle(hFileMapping);
		hFileMapping=NULL;
	}

	return (shared_mem!=NULL) ;
}

BOOL sub_shared_mem_cleanup()
{
	BOOL bSuccess;
	bSuccess=UnmapViewOfFile(shared_mem);
	if(!bSuccess)
	{
		return FALSE ;
	}
	bSuccess=CloseHandle(hFileMapping);
	return bSuccess;
}

BOOL WINAPI libusbDllMain(HANDLE module, DWORD reason, LPVOID reserved);
/* DLL main entry point */
BOOL WINAPI DllMain(HANDLE module, DWORD reason, LPVOID reserved)
{
	switch(reason)
	{
	case DLL_PROCESS_ATTACH:
		sub_shared_mem_init();
		break;
	case DLL_PROCESS_DETACH:
		sub_shared_mem_cleanup();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	default:
		break;
	}
	return libusbDllMain(module, reason, reserved);
}

#if 0
/* for Win98, rename libusb.lib v0.1.12.1 to libusbst.lib */
/* DLL main entry point */
BOOL WINAPI libusbDllMain(HANDLE module, DWORD reason, LPVOID reserved)
{
  return TRUE;
}
#endif
#endif /* _MSC_VER */

/*
*-----------------------------------------------------------------------------
* access to the errno & i2c_status by calling functions
*-----------------------------------------------------------------------------
*/

int sub_get_errno( void )
{
	return sub_errno ;
}

int sub_get_i2c_status( void )
{
	return sub_i2c_status ;
}
