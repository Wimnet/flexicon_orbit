clear all; close all; clc;

filename = 'test_packet_encap_file';
count = 10000;

f = fopen(filename, 'rb'); 
v = fread(f, [2,count], 'float'); 
fclose(f);

figure(); hold off; hold on; grid on;
plot(v(1,:), 'r');
plot(v(2,:), 'b');
legend('I', 'Q');
