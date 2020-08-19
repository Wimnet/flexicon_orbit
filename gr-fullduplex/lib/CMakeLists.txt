# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR}
		    ${FFTW3F_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${FFTW3F_LIBRARY_DIRS})

list(APPEND fullduplex_sources
    packet_decap_impl.cc
    packet_encap_impl.cc
    packet_encap2.cc
    parse_stream_sync_impl.cc
    digital_sic_impl.cc
    residual_si_vis_impl.cc
    ofdm_constellation_impl.cc
    snr_calc_impl.cc
    #compute_ofdm_prr_impl.cc
    count_packets_impl.cc
    async_divide_impl.cc
    fde_config_impl.cc
    sub20_init_impl.cc )

set(fullduplex_sources "${fullduplex_sources}" PARENT_SCOPE)
if(NOT fullduplex_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT fullduplex_sources)

# find_path(SUB_INCLUDE_DIR libsub.h)
# message("Found libsub.h at " ${SUB_INCLUDE_DIR})
find_library(SUB_LIBRARIES sub)
message("Found lib sub at " ${SUB_LIBRARIES})
include_directories(${SUB_INCLUDE_DIR})

# set(LIBUSB_LIBRARIES /usr/local/lib/libusb-1.0.so)
find_library(LIBUSB_LIBRARIES usb-1.0)
message("Found lib usb-1.0 at " ${LIBUSB_LIBRARIES})

add_library(gnuradio-fullduplex SHARED ${fullduplex_sources})
target_link_libraries(gnuradio-fullduplex ${Boost_LIBRARIES} fftw3 ${GNURADIO_ALL_LIBRARIES}  ${SUB_LIBRARIES} ${LIBUSB_LIBRARIES})
set_target_properties(gnuradio-fullduplex PROPERTIES DEFINE_SYMBOL "gnuradio_fullduplex_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-fullduplex PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-fullduplex RUNTIME_COMPONENT "fullduplex_runtime" DEVEL_COMPONENT "fullduplex_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_fullduplex_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_fullduplex.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_fullduplex.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_residual_si_vis.cc
)

add_executable(test-fullduplex ${test_fullduplex_sources})

target_link_libraries(
  test-fullduplex
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  ${FFTW3F_LIBRARIES}
  gnuradio-fullduplex
)

GR_ADD_TEST(test_fullduplex test-fullduplex)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")
