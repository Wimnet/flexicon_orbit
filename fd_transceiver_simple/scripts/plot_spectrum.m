% test transceiver_canceler(/_multi_tone) in real time
close all; clc; clear all;

fs = 1e6;

% filename = '/home/wimnet/Documents/uhd/host/examples/txrx_loopback_to_file/build/usrp_samples.dat';

filename = './data/rx_spec_single.dat';
fid = fopen(filename,'r');
rx_spec = fread(fid,'double');
fclose(fid);

filename = './data/res_spec_single.dat';
fid = fopen(filename,'r');
res_spec = fread(fid,'double');
fclose(fid);

filename = './data/rx_data.dat';
fid = fopen(filename,'r');
rx_data = fread(fid,'float');
fclose(fid);

%%
figure(1); hold off; hold on; grid on;
plot(rx_spec, 'r');
plot(res_spec, 'b');
legend('RX Signal Spectrum', 'Res Signal Spectrum');

figure(2); hold off; hold on; grid on;
[fx, rx_spec] = func_comp_spectrum(rx_data, fs);
plot(fx, rx_spec, 'r-');

PSA = dsp.SpectrumAnalyzer;
PSA.PlotAsTwoSidedSpectrum = true;
PSA.SampleRate = 1e6;
PSA(rx_data);
