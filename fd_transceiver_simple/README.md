# A Simple Full-Duplex Transceiver Example

## Description
The source code for the simple FD transceiver example. It has two sub-directories:
1. `./uhd/`: The UHD source code modified based on Ettus Research's UHD-based `txrx_loopback_to_file.cpp` example (see [here](https://github.com/EttusResearch/uhd/tree/maint/host/examples)).
2. `./sub20/`: The source code for controling and configuration the hardware via the SUB-20 device.
3. `./scripts`: The `MATLAB` scripts for analying the experiment data in an offline manner.

## Build the Example
The main file is `fd_transceiver_simple.cpp`. Use file `CMakeList.txt` to build the example:
```
cd uhd/
mkdir build
cd build
cmake ../
make
```

## Run the Example
Run the example with customized parameters (e.g., carrier frequency, sampling rate, TX/RX gain, etc.):
```
./fd_transceiver_simple --args="serial=USRP-SERIAL-NUMBER" --freq FREQ --rate RATE --tx-gain TX-GAIN --rx-gain RX-GAIN
```
The default settings are:
1. The wave type is Sine: `wave-type = SINE`
2. The Sine wave is with frequency `wave-freq = 100e3 Hz`
3. The baseband amplitude of the Sine wave is `ampl = 0.3`
4. The TX/RX gain is `TX-GAIN = RX-GAIN = 0`

Once the example is running, you can observe the real-time signal power (in `dBm`) both before and after digital self-interference from the terminal window. This illustrates the amount of self-interference cancellation achieved across both the RF and digital domains.

From our measurements using the [B205mini-i](https://www.ettus.com/product/details/USRP-B205mini-i) with the default setting, the TX output power is around `-78dBm`. When running the example, please make sure to set `--tx-gain` and `--rx-gain` properly so that the USRP receiver is not saturated.

## Post-Analyze the Results
For more detailed analysis using expreiment data, we also provide `MATLAB` scripts under `./scripts/` for post-processing.
