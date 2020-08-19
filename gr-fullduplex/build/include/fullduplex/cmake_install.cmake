# Install script for directory: /home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/fullduplex" TYPE FILE FILES
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/api.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/packet_decap.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/packet_encap.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/packet_encap2.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/parse_stream_sync.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/digital_sic.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/residual_si_vis.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/ofdm_constellation.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/snr_calc.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/count_packets.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/async_divide.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/fde_config.h"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/include/fullduplex/sub20_init.h"
    )
endif()

