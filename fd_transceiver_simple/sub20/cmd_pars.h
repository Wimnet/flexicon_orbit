/*
 * Command line parser header file for libsub application
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
* $Id: cmd_pars.h,v 1.2 2009-06-01 11:54:37 avr32 Exp $
*-------------------------------------------------------------------
*/

#ifndef CMD_PARS_H_INCLUDED
#define CMD_PARS_H_INCLUDED 

#ifdef CMD_PARS_MODULE
#undef EXTERN
 #define EXTERN
#else
 #define EXTERN extern
#endif

/*
*------------------------------------------------------------------------------
* Types Definition
*------------------------------------------------------------------------------
*/

/**
* \brief Descriptor for command line option
*/
typedef struct option_descriptor 
{
	char m_option;			/**< One character option: -x		*/
	char* mm_option;		/**< String option: --xxx			*/
	char* equ_alias;		/**< Alias for rvalue, enables '=' 	*/
	char* help_desc;		/**< Option help string				*/
	unsigned int case_code;	/**< Case constant					*/ 
} option_descriptor_t; 

/*
*-----------------------------------------------------------------------------
* Constants
*-----------------------------------------------------------------------------
*/
#define OPTIONS_N	(sizeof(usage_descriptor)/sizeof(struct option_descriptor))
/**
* Options with case_code grater then MIN_HIDDEN_CODE are hidden in help
*/
#define MIN_HIDDEN_CODE	1000

/*
*-----------------------------------------------------------------------------
* Global Variables
*-----------------------------------------------------------------------------
*/

extern option_descriptor_t usage_descriptor[]; 


/*
*-----------------------------------------------------------------------------
* Global Function Definition
*-----------------------------------------------------------------------------
*/

int parse_argv( int my_argc, char** my_argv, int option_n );
void usage( int full_help, int option_n );

extern int process_arg( int opt_index, char* equ_ptr );

#endif /*CMD_PARS_H_INCLUDED*/

