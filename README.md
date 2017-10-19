# flexicon_orbit
FlexICoN project on ORBIT testbed

## Prerequisites
### UHD

### SUB20
The SUB-20 is a multi interface USB adapter for providing popular interaces between PC (USB host) and different hardware devices. Specifically, we use the `SPI` module on SUB-20 to control and configure our hardware.

### `Eigen`
The Eigen `C++` library is used for basic albegra in channel estimation and digital self-interference cancellation. The Eigen releases can be found on the official Eigen [website](http://eigen.tuxfamily.org/index.php?title=Main_Page). We used the latest stable release `Eigen 3.3.4` through our testings and experiments.

On Linux, download and unzip `Eigen 3.3.4` and copy the main librararies to the desired path by running `scp -r /eigen/Eigen/ /usr/include/`

## A Simple Full-Duplex (FD) Transceiver Example
### Directories
#### `./fd_transceiver_simple`
The source code for the simple FD transceiver example, It has two sub-directories:
##### `./fd_transceiver_simple/uhd/`
The UHD source code modified based on Ettus Research's UHD-based `txrx_loopback_to_file.cpp` example (see [here](https://github.com/EttusResearch/uhd/tree/maint/host/examples)).
##### `./fd_transceiver_simple/sub20/` 
The source code for controling and configuration the hardware via the SUB-20 device.

### Run the Example
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
