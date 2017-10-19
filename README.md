# flexicon_orbit
FlexICoN project on ORBIT testbed

## Prerequisites
### UHD

### Sub20

### Eigen
We use the Eigen `C++` library for basic albegra in channel estimation and digital self-interference cancellation. The Eigen releases can be found on the official Eigen [website](http://eigen.tuxfamily.org/index.php?title=Main_Page). We used the latest stable release `Eigen 3.3.4` through our testings and experiments.

On Linux, download and unzip `Eigen 3.3.4` and copy the main librararies to the desired path by running `scp -r /eigen/Eigen/ /usr/include/`

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
```
Then, you can run the simple transeiver example by
```
./fd_transceiver_simple --args="serial=USRP-SERIAL-NUMBER" --tx-freq TX-FREQ --rx-freq RX-FREQ --tx-rate TX-RATE --rx-rate RX-RATE --wave-type SINE --wave-freq WAVE-FREQ --tx-gain TX-GAIN --rx-gain RX-GAIN
```
