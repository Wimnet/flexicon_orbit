# Install script for directory: /home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks" TYPE FILE FILES
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_packet_decap.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_packet_encap.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_packet_encap2.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_parse_stream_sync.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_digital_sic.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_residual_si_vis.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_ofdm_constellation.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_snr_calc.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_count_packets.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_async_divide.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_fde_config.xml"
    "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/grc/fullduplex_sub20_init.xml"
    )
endif()

