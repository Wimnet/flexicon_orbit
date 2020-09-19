# Open-Access Full-Duplex Wireless in the ORBIT and COSMOS Testbeds
This GitHub repository contains the hardware designs, source code, and experiment files for the [FlexICoN](https://flexicon.ee.columbia.edu) [Gen-1](https://flexicon.ee.columbia.edu/gen-1-open-access) and [Gen-2](https://flexicon.ee.columbia.edu/gen-2-open-access) full-duplex (FD) radios integrated in the ORBIT and COSMOS testbeds. For a tutorial on using these FD radios, please see the [FD tutorial located on the COSMOS/ORBIT Wiki](https://wiki.cosmos-lab.org/wiki/Tutorials/Wireless/FullDuplex).

The tutorial consists of three demo experiments which run through GNU Radio on the integrated 1st-generation (Gen-1) *narrowband* and 2nd-generation (Gen-2) *wideband* FD radios. Each experiment is accessible at `./experiments`. While these demo experiments are designed to be run on the ORBIT and COSMOS testbeds, we provide them here so that they may be used as a basis for other experimentation with FD wireless. 

We will update this repository with more full-duplex source code and examples as they are developed.

## Publications
For further information on the integrated Gen-1 and Gen-2 radios, please see the following papers:

*Manav Kohli, Tingjun Chen, Mahmood Baraani Dastjerdi, Jackson Welles, Ivan Seskar, Harish Krishnaswamy, and Gil Zussman, "Open-Access Full-Duplex Wireless in the ORBIT and COSMOS Testbeds," in Proc. ACM MobiCom'20 Workshop on Wireless Network Testbeds, Experimental evaluation & CHaracterization (WiNTECH '20) (invited paper), Sept. 2020.* [[download](https://wimnet.ee.columbia.edu/wp-content/uploads/2020/08/wintech2020_orbit_cosmos_fullduplex_integration.pdf)]

*Tingjun Chen, Mahmood Baraani Dastjerdi, Jin Zhou, Harish Krishnaswamy, and Gil Zussman, “Wideband Full-Duplex Wireless via Frequency-Domain Equalization: Design and Experimentation,” in Proc. ACM MobiCom'19, Oct. 2019.* [[download](https://wimnet.ee.columbia.edu/wp-content/uploads/2018/12/FDE_MobiCom19.pdf)]

We greatly appreciate that you cite the above papers if you use the hardware or software. Please email Tingjun Chen (tingjun [at] ee.columbia.edu) or Manav Kohli (mpk2138 [at] columbia.edu) if you use (or plan to use) the full-duplex node or if you have any questions.

## Hardware
### The Gen-1 Narrowband Frequency-Flat Amplitude- and Phase-based RF Canceller Box
Please refer to our [WiNTECH'20 paper](https://wimnet.ee.columbia.edu/wp-content/uploads/2020/08/wintech2020_orbit_cosmos_fullduplex_integration.pdf) for details on the design and integration of the Gen-1 RF Canceller Box in the ORBIT testbed.

### The Gen-2 Wideband Frequency-Domain Equalization (FDE)-based RF Canceller Box
Please refer to our [WiNTECH'20 paper](https://wimnet.ee.columbia.edu/wp-content/uploads/2020/08/wintech2020_orbit_cosmos_fullduplex_integration.pdf) for details on the integration of the Gen-2 RF Canceller Box in the COSMOS testbed. Our [MobiCom'19 paper](https://wimnet.ee.columbia.edu/wp-content/uploads/2018/12/FDE_MobiCom19.pdf) provides an in-depth evaluation of the Gen-2 hardware.

### Universal Software Radio Peripheral (USRP) 
Our full-duplex transceiver is based on the Ettus Research USRP. Our examples have been tested successfully on USRP models N210/USRP2.

### SUB-20 
The [SUB-20](http://www.xdimax.com/sub20/sub20.html) is a multi-interface USB adapter for providing standard interfaces between PC (USB host) and different hardware devices.  Specifically, we use the `SPI` module on the SUB-20 to control and configure our hardware. The user manual can be found [here](http://www.xdimax.com/sub20/doc/sub20-man.pdf). 

## Software
Our software runs on GNU Radio version 3.7, with UHD version 3.14. This setup can be used on both Ubuntu 16.04 and Ubuntu 18.04.

We have built a set of GNU Radio OOT modules in C++ to support USRP operation in FD mode. These modules include packet-level digital SIC, and control of the RF canceller PCB. The source code is under `./gr-fullduplex/lib` and further details on the software are available in the WiNTECH paper.

For the Gen-2 FD radios integrated in COSMOS, the software runs on a dedicated compute node. For the ORBIT testbed, we provide an image that may be loaded onto the FD node, `node11-10`. The most updated ORBIT node image with all the configurations and examples is `flexicon-orbit-v3.ndz` (May 18, 2019).

### Experiments
The following experiments are available at `./experiments`. All experiments contain a graphical user interface (GUI) which visualizes the key results. The second two experiments require two FD radios, and therefore are only available in COSMOS.

* `node_level_sic_fd_gui`: An example FD transceiver demonstrating self-interference cancellation (SIC) in the RF and digital domains. Experimenters may configure the canceller PCB through the GUI. 
* `ofdm_link_fd_sic`: An example FD link, demonstrating FD operation with two integrated Gen-2 radios. 
* `prr_fd_gui`: Another example FD link, which demonstrates the FD link packet reception ratio (PRR). The experimenter may change the link SNR.

We also provide a legacy experiment in `./fd_transceiver_simple`. This experiment runs through the command line using UHD.

### UHD
The instructions for UHD can be found on the Ettus Resesarch UHD repository [here](https://github.com/EttusResearch/uhd).

### The Eigen C++ Library
The Eigen C++ library is used for basic albegra in channel estimation and digital self-interference cancellation. The Eigen releases can be found on the [Eigen website](http://eigen.tuxfamily.org/index.php?title=Main_Page). We used the latest stable release Eigen 3.3.4 for our experiments.

On Linux, download and unzip Eigen 3.3.4 and copy the main librararies to the desired path by running `scp -r /eigen/Eigen/ /usr/include/`. The Eigen libraries are  already included in the provided node image `flexicon-orbit-v3.ndz`.

## Useful links
* Full-duplex tutorial: [https://wiki.cosmos-lab.org/wiki/Tutorials/Wireless/FullDuplex](https://wiki.cosmos-lab.org/wiki/Tutorials/Wireless/FullDuplex). We highly recommend you to go through the tutorials.
* Columbia FlexICoN project website: [https://flexicon.ee.columbia.edu/](https://flexicon.ee.columbia.edu/)
* ORBIT Testbed: [https://www.orbit-lab.org/](https://www.orbit-lab.org/)
* COSMOS Testbed: [https://www.cosmos-lab.org/](https://www.cosmos-lab.org/)

## Acknowledgements
This work was supported in part by NSF grants ECCS-1547406 and CNS-1827923, NSF-BSF grant CNS-1910757, the DARPA RF-FPGA and SPAR programs, a Qualcomm Innovation Fellowship, Texas Instruments, and Intel. We thank Steven Alfano, Jelena Diakonikolas, Aishwarya Rajen, Jinhui Song, and Mingyan Yu for their contributions to various aspects of the project. We thank Ivan Seskar, Jakub Kolodziejski, Michael Sherman, and Prasanthi Maddala from WINLAB, Rutgers University, for their help on the integration with the ORBIT and COSMOS testbeds. We also thank Kira Theuer and Kendall Ruiz from NI and the NI technical support team for their help.
