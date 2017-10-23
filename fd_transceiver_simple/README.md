# A Simple Full-Duplex Transceiver Example

## Description
The source code for the simple FD transceiver example. It has two sub-directories:
1. `./uhd/`: The UHD source code for the example
2. `./sub20/`: The source code for controling and configuration the hardware via the SUB-20 device
3. `./scripts`: The `MATLAB` scripts for analying the experiment data in an offline manner

## Build the Example
### Build the UHD Code
The main file is `fd_transceiver_simple.cpp` modified based on Ettus Research's UHD-based `txrx_loopback_to_file.cpp` example (see [here](https://github.com/EttusResearch/uhd/tree/maint/host/examples)). Use file `CMakeList.txt` to build the example:
```
cd uhd/
mkdir build
cd build/
cmake ../
make
```

### Build the Canceller Configuration Code
Use file `Makefile` to build the SUB20 configuration code. Make sure that the `libusb-1.0` library is linked properly.
```
cd sub20/
make
```

## Run the Example
### Run the UHD Code
Run the example with desired parameters (e.g., carrier frequency, sampling rate, TX/RX gain, etc.):
```
./fd_transceiver_simple --args="serial=USRP-SERIAL-NUMBER" --freq FREQ --rate RATE --tx-gain TX-GAIN --rx-gain RX-GAIN --wave-type WAVE-TYPE --wave-freq WAVE-FREQ --ampl AMPL
```
The default settings are:
1. The wave type is Sine: `WAVE-TYPE = SINE`
2. The Sine wave is with frequency `WAVE-FREQ = 100e3 Hz`
3. The baseband amplitude of the Sine wave is `AMPL = 0.3`
4. The TX/RX gain is `TX-GAIN = RX-GAIN = 0`

Once the example is running, you can observe the real-time signal power (in `dBm`) both before and after digital self-interference from the terminal window. This illustrates the amount of self-interference cancellation achieved across both the RF and digital domains.

From our measurements using the [B205mini-i](https://www.ettus.com/product/details/USRP-B205mini-i) with the default setting: 1. The TX output power is around `-78 dBm`
2. The RX input saturation point is around `0 dBm`
In addition, the TX/RX gain increases almost linearly with respect to the input value. Therefore, when running the example, please make sure to set `TX-GAIN` and `RX-GAIN` properly so that the USRP receiver is not saturated.

### Run the SUB20 Configuration Code
Configure the FlexICoN Gen1 RF Canceller by running
```
./rf_canc_gen1_config DAC-CODE ATT-CODE
```
In particular:
1. The DAC takes input of `DAC-CODE = 0, 1, ..., 255`
1. The attenuator (ATT) takes input of `ATT-CODE = 0, 1, ..., 255`, `ATT-code = 0` is the MIN attenuation and `ATT-CODE = 255` is the MAX attenuation


## Post-Analyze the Results
For more detailed analysis using expreiment data, we also provide `MATLAB` scripts under `./scripts/` for post-processing.
