# A Simple Full-Duplex Transceiver Example

## Description
The source code for the simple FD transceiver example. It has two sub-directories:
1. `./uhd/`: The UHD source code modified based on Ettus Research's UHD-based `txrx_loopback_to_file.cpp` example (see [here](https://github.com/EttusResearch/uhd/tree/maint/host/examples)).
2. `./sub20/`: The source code for controling and configuration the hardware via the SUB-20 device.
3. `./scripts`: The `MATLAB` scripts for analying the experiment data in an offline manner.

## Build the Example
Use file `CMakeList.txt` to build the example:
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
./fd_transceiver_simple --args="serial=USRP-SERIAL-NUMBER" --tx-freq TX-FREQ --rx-freq RX-FREQ --tx-rate TX-RATE --rx-rate RX-RATE --wave-type SINE --wave-freq WAVE-FREQ --tx-gain TX-GAIN --rx-gain RX-GAIN
```

Once the example is running, you can observe the real-time signal power (in dBm) both before and after digital self-interference from the terminal window. This illustrates the amount of self-interference cancellation achieved across both the RF and digital domains.

## Post-Analyze the Results
For more detailed analysis using expreiment data, we also provide `MATLAB` scripts under `./scripts/` for post-processing.
