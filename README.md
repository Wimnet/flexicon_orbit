# Open-Access Full-Duplex Wireless in the ORBIT testbed
This GitHub webpage contains the build instructions and source code of the [ORBIT](http://www.orbit-lab.org) wireless testbed tutorial: [Full-duplex Wireless using USRP N210](http://www.orbit-lab.org/wiki/Tutorials/k0SDR/Tutorial25).

This tutorial constains a simple full-duplex (FD) transceiver using the USRP N210 SDR and a customized RF canceller developed within the [Columbia FlexICoN project](http://flexicon.ee.columbia.edu/). The detailed instructions can be found in the tutorial at [here](http://www.orbit-lab.org/wiki/Tutorials/k0SDR/Tutorial25).

More full-duplex examples are currently under development and will be released once ready.

### Paper
Our Gen-1 RF Canceller is install with `node11-10` in the ORBIT main grid. This node is equipped with a USRP N210 SDR. For more information, please read

*Tingjun Chen, Mahmood Baraani Dastjerdi, Jin Zhou, Harish Krishnaswamy, and Gil Zussman, “Open-access full-duplex wireless in the ORBIT testbed,” arXiv preprint: 1801.03069 [cs.NI], Jan. 2018.* [[arXiv]](https://arxiv.org/pdf/1801.03069.pdf)

Please cite the above paper if you are using the hardware. Please email Tingjun Chen (tingjun [at] ee.columbia.edu) if you are using (or plan to use) the full-duplex node, or if you have any questions regarding the FD transceiver and the baseline program.

### Acknowledgements
This work was supported in part by NSF grant ECCS1547406, DARPA RF-FPGA program, DARPA SPAR program, a Qualcomm Innovation Fellowship, Texas Instruments, and Intel. We thank Steven Alfano, Jelena Diakonikolas, Aishwarya Rajen, Jinhui Song, Mingyan Yu for their contributions to various aspects of the project. We thank Ivan Seskar, Jakub Kolodziejski, and Prasanthi Maddala from WINLAB, Rutgers University, for their help on the integration with the ORBIT testbed. We also thank Kira Theuer and Kendall Ruiz from NI and the NI technical support team for their help.

### Useful links
* Columbia FlexICoN project website: [http://flexicon.ee.columbia.edu/](http://flexicon.ee.columbia.edu/)
* ORBIT Testbed: [http://www.orbit-lab.org/](http://www.orbit-lab.org/)
* Full-duplex tutorial at ORBIT: [http://www.orbit-lab.org/wiki/Tutorials/k0SDR/Tutorial25#Full-DuplexWirelessusingUSRPN210](http://www.orbit-lab.org/wiki/Tutorials/k0SDR/Tutorial25#Full-DuplexWirelessusingUSRPN210)

## Hardware (HW)
### Columbia FlexICoN's Gen-1 Frequency-Flat Amplitude- and Phase-based RF Canceller
Please refer to our technical report at [here](https://arxiv.org/pdf/1801.03069.pdf).

### Universal Software Radio Peripheral (USRP)
Our prototypes and systems are based on the Ettus Research USRP. Our example has been tested succfully on USRP models N210/USRP2/B210/B205-mini.

### SUB-20 
The [SUB-20](http://www.xdimax.com/sub20/sub20.html) is a multi interface USB adapter for providing popular interaces between PC (USB host) and different hardware devices.  Specifically, we use the `SPI` module on SUB-20 to control and configure our hardware. The user manual can be found [here](http://www.xdimax.com/sub20/doc/sub20-man.pdf). 

## Software (SW)
### UHD
The instructions of UHD can be found on the Ettus Resesarch UHD repository at [here](https://github.com/EttusResearch/uhd).

### The `Eigen C++` Library
The `Eigen C++` library is used for basic albegra in channel estimation and digital self-interference cancellation. The `Eigen` releases can be found on the [Eigen website](http://eigen.tuxfamily.org/index.php?title=Main_Page). We used the latest stable release `Eigen 3.3.4` through our testings and experiments.

On Linux, download and unzip `Eigen 3.3.4` and copy the main librararies to the desired path by running
```
scp -r /eigen/Eigen/ /usr/include/
```
