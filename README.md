# flexicon_orbit
FlexICoN project on ORBIT testbed

## Prerequisites
### Hardware (HW)
#### Universal Software Radio Peripheral (USRP)
Our prototypes and systems are based on the Ettus Research USRP. Our example has been tested succfully on USRP models N201/B210/B205-mini.

#### Columbia FlexICoN's Gen-1 Frequency-Flat Amplitude- and Phase-based RF Canceller
For more information, please read

*T. Chen, J. Zhou, N. Grimwood, R. Fogel, J. Marasevic, H. Krishnaswamy, and G. Zussman, “Demo: Full-duplex Wireless based on a Small-Form-Factor Analog Self-Interference Canceller,” in Proc. ACM MobiHoc’16, 2016.* [[PDF]](http://wimnet.ee.columbia.edu/wp-content/uploads/2016/06/full_duplex_demo_mobihoc16.pdf)

#### SUB-20
The [SUB-20](http://www.xdimax.com/sub20/sub20.html) is a multi interface USB adapter for providing popular interaces between PC (USB host) and different hardware devices.  Specifically, we use the `SPI` module on SUB-20 to control and configure our hardware. The user manual can be found [here](http://www.xdimax.com/sub20/doc/sub20-man.pdf). 

### Software (SW)
#### UHD
The instructions of UHD can be found on the Ettus Resesarch UHD repository at [here](https://github.com/EttusResearch/uhd).

#### The `Eigen C++` Library
The `Eigen C++` library is used for basic albegra in channel estimation and digital self-interference cancellation. The `Eigen` releases can be found on the [Eigen website](http://eigen.tuxfamily.org/index.php?title=Main_Page). We used the latest stable release `Eigen 3.3.4` through our testings and experiments.

On Linux, download and unzip `Eigen 3.3.4` and copy the main librararies to the desired path by running
```
scp -r /eigen/Eigen/ /usr/include/
```

## A Simple Full-Duplex (FD) Transceiver Example
Please see detailed instructions under directory `./fd_transceiver_simple`.
