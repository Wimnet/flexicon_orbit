/* -*- c++ -*- */

#define FULLDUPLEX_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "fullduplex_swig_doc.i"

%{
#include "fullduplex/packet_decap.h"
#include "fullduplex/packet_encap.h"
#include "fullduplex/packet_encap2.h"
#include "fullduplex/parse_stream_sync.h"
#include "fullduplex/digital_sic.h"
#include "fullduplex/residual_si_vis.h"
#include "fullduplex/ofdm_constellation.h"
#include "fullduplex/snr_calc.h"
#include "fullduplex/count_packets.h"
#include "fullduplex/async_divide.h"
#include "fullduplex/fde_config.h"
//#include "fullduplex/compute_ofdm_prr.h"
%}


%include "fullduplex/packet_decap.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, packet_decap);
%include "fullduplex/packet_encap.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, packet_encap);
%include "fullduplex/packet_encap2.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, packet_encap2);
%include "fullduplex/parse_stream_sync.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, parse_stream_sync);
%include "fullduplex/digital_sic.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, digital_sic);
%include "fullduplex/residual_si_vis.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, residual_si_vis);
%include "fullduplex/ofdm_constellation.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, ofdm_constellation);
%include "fullduplex/snr_calc.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, snr_calc);
//%include "fullduplex/compute_ofdm_prr.h"
//GR_SWIG_BLOCK_MAGIC2(fullduplex, compute_ofdm_prr);
%include "fullduplex/count_packets.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, count_packets);
%include "fullduplex/async_divide.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, async_divide);
%include "fullduplex/fde_config.h"
GR_SWIG_BLOCK_MAGIC2(fullduplex, fde_config);
