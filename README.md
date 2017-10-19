# flexicon_orbit
FlexICoN project on ORBIT testbed

## A Simple Full-Duplex Transceiver Example
### Description
The example in located in `./fd_transceiver_simple` with two sub-directories:
* `./fd_transceiver_simple/uhd/` contains the UHD source code, which is modified based on Ettus Research's UHD-based `txrx_loopback_to_file.cpp` example (see [here](https://github.com/EttusResearch/uhd/tree/maint/host/examples))
* `./fd_transceiver_simple/sub20/` contains the hardware control and configuration code

#### UHD
To build the example, run
```
cd ./fd_transceiver_simple/uhd/
mkdir build
cd build
cmake ../
make
'''
