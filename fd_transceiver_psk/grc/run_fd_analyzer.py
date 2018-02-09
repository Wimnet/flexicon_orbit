#######################################################################################################
# This script shows the working of the Digital SIC by generating relevant graphs.
# It uses 'func_dig_sic.py' module for performing digital SIC.
# The input files are 'usrp_in' and 'usrp_out'. 
# Optionally, a simulated 'remote_signal' data file can be used,
# in case where 'usrp_out' data, doesn't contain a remote signal.
#
# Output graphs include:
# - Time domain signals
# - Constellation 
# - Power specturm
# - Digital SI canceler taps
#
# * The amount of digital SIC is shown on the power spetrum plot when simulated signal is diabled.
# * The script needs to run from the console so the user can change the settings.
# * The 'settings' file is generated to save and load the user settings for the next run.
#
# Authors:
# - Guy Farkash,	email: gf2373@columbia.edu
# - Tingjun Chen, 	email: tc2668@columbia.edu
#
# Date: Feb. 8, 2018
#
# For more details, please refer to the Columbia FlexICoN project
########################################################################################################

import func_dig_sic as grc_sic
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptch
import os
import pickle


def main():

	# Init settings, get settings from user and loads data files
	usr_sett = user_settings()
	usr_sett.get_user_settings()
	usrp_in, usrp_out, remote_signal = open_files('usrp_in', 'usrp_out', 'remote_signal', usr_sett.remote)

	# Create SIC object
	sic = grc_sic.sic(usr_sett.frame_size, usr_sett.pream_size, usr_sett.k, usr_sett.h_frames, usr_sett.er, usr_sett.wr, usr_sett.ir)

	# Create empty buffers and set SIC buffers size
	sic_buffs = sic_buffers()
	res_buffs = result_buffers()
	sic_input_buffers_size = usr_sett.frame_size + usr_sett.k
	sic_output_buffers_size = usr_sett.frame_size

	# Get the first frame size sample from data files
	get_first_frame(usrp_in, usrp_out, remote_signal, sic_input_buffers_size, sic_buffs, usr_sett)
	sic_buffs.output_items[0] = np.zeros(int(sic_output_buffers_size), np.complex64)

	# Do SIC work and append all buffers to the results buffers
	out_len = sic.work(sic_buffs.input_items, sic_buffs.output_items)
	save_results(sic_buffs, out_len, sic.h, res_buffs)

	# For every data frame, get frame size samples, do SIC work and append all buffers to the results buffers
	for i in range(0, int(usr_sett.plot_frame_stop)):
		get_frame(usrp_in, usrp_out, remote_signal, sic_input_buffers_size, sic_buffs, usr_sett)
		sic_buffs.output_items[0] = np.zeros(int(sic_output_buffers_size), np.complex64)
		out_len = sic.work(sic_buffs.input_items, sic_buffs.output_items)
		save_results(sic_buffs, out_len, sic.h, res_buffs)

	# Plot signals in time domain, show the constellation, FFT and h weights vector
	plot_signal(res_buffs, usr_sett)
	plot_constellation(res_buffs, usr_sett)
	plot_fft(res_buffs, usr_sett)
	plot_h(res_buffs, usr_sett)

	plt.show()


class user_settings:
	def __init__(self):
		# default settings

		# sic settings
		self.frame_size = 20e3
		self.pream_size = 1e3
		self.k = 50
		self.h_frames = 1
		self.er, self.wr, self.ir = 1, 0, 0

		# signals settings
		self.remote = 0
		self.usrp_delay = 29
		self.remote_delay = 0
		self.sample_rate = 10e6
		self.center_freq = 900e6
		self.samples_per_symbol = 4

		# plot settings
		self.plot_im = 0
		self.plot_usrp_out = 1
		self.plot_frame_start = 10
		self.plot_frame_stop = 11
		self.ffts_per_frame = 1
		self.const_points_scale = 1

	def get_user_settings(self):
		def get(old_val, string):
			from_user = raw_input(string + ' ({}): '.format(old_val))
			if from_user != '':
				return float(from_user)
			else:
				return old_val

		print('Enter      \'s\' - Edit Settings      \'def\' - Load Default Settings')
		print('Press ENTER to skip')
		user_input = raw_input()

		if user_input == 's' or user_input == 'S':
			self.read_settings_file()

			print('Enter \'s\' - Signals settings')
			print('Press ENTER to skip')
			user_input = raw_input()
			if user_input == 's' or user_input == 'S':
				self.usrp_delay = get(self.usrp_delay, 'USRP TX/RX delay (samps)')
				self.remote = get(self.remote, 'Simulated remote signal on, 1/0')
				self.remote_delay = get(self.remote_delay, 'Remote signal delay (samps)')
				self.sample_rate = get(self.sample_rate, 'Sample rate (samp/sec)')
				self.samples_per_symbol = get(self.samples_per_symbol, 'Samples per symbol')
				self.center_freq = get(self.center_freq, 'Carrier frequency (Hz)')

			print('Enter \'s\' - SIC settings')
			print('Press ENTER to skip')
			user_input = raw_input()
			if user_input == 's' or user_input == 'S':
				self.h_frames = get(self.h_frames, 'Update frequency of h (frames)')
				self.k = get(self.k, '# of digital SI canceller taps (samps)')
				self.frame_size = get(self.frame_size, 'Frame Size (samps)')
				self.pream_size = get(self.pream_size, 'Preamble Size (samps)')
				print('\n')

			print('Enter \'s\' - Plot settings')
			print('Press ENTER to skip')
			user_input = raw_input()
			if user_input == 's' or user_input == 'S':
				self.plot_usrp_out = get(self.plot_usrp_out, 'Plot RX signals, 1/0')
				self.plot_im = get(self.plot_im, 'Plot RX signals (imag), 1/0')
				self.plot_frame_start = get(self.plot_frame_start, 'Start plot from frame')
				self.plot_frame_stop = get(self.plot_frame_stop, 'End plot in frame')
				self.ffts_per_frame = get(self.ffts_per_frame, '# of FFTs per frame')
				self.const_points_scale = get(self.const_points_scale, 'Constellation interleaving (samps)')
				print('\n')

		elif user_input == 'def' or user_input == 'DEF':
			self.__init__()

		else:
			self.read_settings_file()

		self.write_settings_file()

	def read_settings_file(self):
		if os.path.isfile('settings'):
			settings_file = open('settings', 'r')
			settings = pickle.load(settings_file)
			settings_file.close()
			self.__dict__.update(settings.__dict__)

	def write_settings_file(self):
		settings_file = open('settings', 'w')
		pickle.dump(self, settings_file)
		settings_file.close()


class sic_buffers():
	def __init__(self):
		self.input_items = [[], []]
		self.output_items = [[]]
		self.remote_items = [[]]


class result_buffers():
	def __init__(self):
		self.in_res = [[], []]
		self.out_res = [[]]
		self.rem_res = [[]]
		self.h_res = [[]]
		self.out_len_res = [0]


def get_first_frame(usrp_in, usrp_out, remote_signal, buffer_size, sic_buffs, usr_sett):

	sic_buffs.input_items[0], sic_buffs.input_items[1] = np.zeros(int(buffer_size), np.complex64), np.zeros(int(buffer_size), np.complex64)
	sic_buffs.remote_items[0] = np.zeros(int(buffer_size), np.complex64)

	for i in range(0, int(buffer_size)):

		if i < usr_sett.usrp_delay:  # add (usrp_delay) zeros to the start of the samples going into the usrp
			sic_buffs.input_items[1][i] = np.complex64(0)
		else:
			sic_buffs.input_items[1][i] = get_sample(usrp_in)

		if usr_sett.remote and i < usr_sett.remote_delay:  # add (remote_delay) zeros to the start of the remote samples
			sic_buffs.remote_items[0][i] = np.complex64(0)
			sic_buffs.input_items[0][i] = get_sample(usrp_out)

		elif usr_sett.remote:
			sic_buffs.remote_items[0][i] = get_sample(remote_signal)
			sic_buffs.input_items[0][i] = get_sample(usrp_out) + sic_buffs.remote_items[0][i]

		else:
			sic_buffs.input_items[0][i] = get_sample(usrp_out)


def get_frame(usrp_in, usrp_out, remote_signal, buffer_size, sic_buffs, usr_sett):

	sic_buffs.input_items[0], sic_buffs.input_items[1] = np.zeros(int(buffer_size), np.complex64), np.zeros(int(buffer_size), np.complex64)
	sic_buffs.remote_items[0] = np.zeros(int(buffer_size), np.complex64)

	if usr_sett.remote:
		for i in range(0, int(buffer_size)):
			sic_buffs.input_items[1][i] = get_sample(usrp_in)
			sic_buffs.remote_items[0][i] = get_sample(remote_signal)
			sic_buffs.input_items[0][i] = get_sample(usrp_out) + sic_buffs.remote_items[0][i]

	else:
		for i in range(0, int(buffer_size)):
			sic_buffs.input_items[1][i] = get_sample(usrp_in)
			sic_buffs.input_items[0][i] = get_sample(usrp_out)


def get_sample(file):

	samp = file.read(8)

	return np.frombuffer(samp, dtype=np.dtype('complex64'))[0]


def save_results(sic_buffs, out_len, h, res_buffs):

	res_buffs.out_len_res[0] += out_len
	res_buffs.h_res[0] = np.append(res_buffs.h_res[0], h)
	res_buffs.in_res[0], res_buffs.in_res[1] = np.append(res_buffs.in_res[0], sic_buffs.input_items[0]), np.append(res_buffs.in_res[1], sic_buffs.input_items[1])
	res_buffs.out_res[0] = np.append(res_buffs.out_res[0], sic_buffs.output_items[0][:out_len])
	res_buffs.rem_res[0] = np.append(res_buffs.rem_res[0], sic_buffs.remote_items[0])


def open_files(usrp_in, usrp_out, remote_signal, remote_on):

	u_in = open('usrp_in', 'r')
	u_out = open('usrp_out', 'r')

	if remote_on:
		re_sig = open('remote_signal', 'r')
		return u_in, u_out, re_sig
	else:
		return u_in, u_out, 0


def plot_signal(res_buffs, usr_sett):

	plt.subplot(2, 2, 1)

	if res_buffs.out_len_res[0] == 0:
		return

	# Set starting sample and end sample based on user settings
	stop = (usr_sett.plot_frame_stop + 1) * usr_sett.frame_size
	if stop > res_buffs.out_len_res[0]:
		stop = res_buffs.out_len_res[0]

	start = usr_sett.plot_frame_start * usr_sett.frame_size
	if start > stop:
		start = 0

	start = int(start)
	stop = int(stop)

	# Create the time axis
	step = 1e6 / usr_sett.sample_rate
	x = np.arange(start * step, stop * step, step)

	# Plot signals based on user settings
	if usr_sett.plot_im:
		if usr_sett.plot_usrp_out:
			plt.plot(x, res_buffs.in_res[0][start:stop].imag, 'b--')

		plt.plot(x, res_buffs.out_res[0][start:stop].imag, 'g--')

		if usr_sett.remote:
			plt.plot(x, res_buffs.rem_res[0][start:stop].imag, 'r--')

	if usr_sett.plot_usrp_out:
		plt.plot(x, res_buffs.in_res[0][start:stop].real, 'b')

	plt.plot(x, res_buffs.out_res[0][start:stop].real, 'g')

	if usr_sett.remote:
		plt.plot(x, res_buffs.rem_res[0][start:stop].real, 'r')

	blue_patch_r = ptch.Patch(color='b', label='RX Re')
	blue_patch_i = ptch.Patch(color='b', linestyle='--', fill=0, label='RX Im')
	red_patch_r = ptch.Patch(color='r', label='Remote Signal Re')
	red_patch_i = ptch.Patch(color='r', linestyle='--', fill=0, label='Remote Signal Im')
	green_patch_r = ptch.Patch(color='g', label='RX after Dig. SIC Re')
	green_patch_i = ptch.Patch(color='g', linestyle='--', fill=0, label='RX after Dig. SIC Im')

	patches = []

	if usr_sett.plot_usrp_out:
		patches.append(blue_patch_r)
		if usr_sett.plot_im:
			patches.append(blue_patch_i)

	if usr_sett.remote:
		patches.append(red_patch_r)
		if usr_sett.plot_im:
			patches.append(red_patch_i)

	patches.append(green_patch_r)
	if usr_sett.plot_im:
		patches.append(green_patch_i)

	legends = plt.legend(handles=patches, loc=1)
	plt.gca().add_artist(legends)

	plt.xlabel('Time (us)')
	plt.ylabel('Amplitude')
	samp_rate = usr_sett.sample_rate / 1e6
	frame_time = 1e6 * usr_sett.frame_size / usr_sett.sample_rate
	text = 'Sample Rate: ' + str(samp_rate) + ' MSa/s, Frame length: ' + str(frame_time) + ' us'
	info = ptch.Patch(color='black', label=text)
	plt.legend(handles=[info], loc=3, fontsize=12)

	plt.title('Time Domain Signal', fontsize=14, fontweight='bold')


def plot_constellation(res_buffs, usr_sett):

	plt.subplot(2, 2, 2)

	if res_buffs.out_len_res[0] == 0:
		return

	# Set starting sample and end sample based on user settings
	stop = (usr_sett.plot_frame_stop + 1) * usr_sett.frame_size
	if stop > res_buffs.out_len_res[0]:
		stop = res_buffs.out_len_res[0]

	start = usr_sett.plot_frame_start * usr_sett.frame_size
	if start > stop:
		start = 0

	start = int(start)
	stop = int(stop)

	# Plot constellation points every 'num' samples
	num = int(usr_sett.const_points_scale)

	if usr_sett.plot_usrp_out:
		plt.scatter(res_buffs.in_res[0][start:stop].real[0::num], res_buffs.in_res[0][start:stop].imag[0::num], c='b', marker='.')

	plt.scatter(res_buffs.out_res[0][start:stop].real[0::num], res_buffs.out_res[0][start:stop].imag[0::num], c='g', marker='x')

	if usr_sett.remote:
		plt.scatter(res_buffs.rem_res[0][start:stop].real[0::num], res_buffs.rem_res[0][start:stop].imag[0::num], c='r', marker='o')

	blue_patch = ptch.Patch(color='b', label='RX')
	red_patch = ptch.Patch(color='r', label='Remote Signal')
	green_patch = ptch.Patch(color='g', label='RX after Dig. SIC')

	patches = []
	if usr_sett.plot_usrp_out:
		patches.append(blue_patch)

	if usr_sett.remote:
		patches.append(red_patch)

	patches.append(green_patch)

	plt.legend(handles=patches)

	plt.xlabel('I')
	plt.ylabel('Q')
	plt.axhline(y=0, color='k')
	plt.axvline(x=0, color='k')

	plt.title('Constellation', fontsize=14, fontweight='bold')


def plot_fft(res_buffs, usr_sett):

	plt.subplot(2, 2, 3)

	if res_buffs.out_len_res[0] == 0:
		return

	# Put the last user requested frame in sic_in and sic_out
	fr = int(usr_sett.plot_frame_stop)
	fr_size = int(usr_sett.frame_size)
	sic_in = res_buffs.in_res[0][fr * fr_size:(fr + 1) * fr_size].real
	sic_out = res_buffs.out_res[0][fr * fr_size:(fr + 1) * fr_size].real

	# Calculate FFT for both signals
	fft_times = int(usr_sett.ffts_per_frame) + (usr_sett.ffts_per_frame == 0) * 1
	fft_size = int(usr_sett.frame_size / fft_times)
	plot_in = np.zeros(fft_size, dtype='complex64')
	plot_out = np.zeros(fft_size, dtype='complex64')
	for i in range(0, fft_times):
		plot_in += np.fft.fft(sic_in[i * fft_size:(i + 1) * fft_size])
		plot_out += np.fft.fft(sic_out[i * fft_size:(i + 1) * fft_size])

	# Calculate FFT spectrum in dBs
	center = int(fft_size / 2)
	plot_in = np.append(plot_in[center:], plot_in[:center])
	plot_in = (np.abs(plot_in) / fft_times) / fft_size
	plot_in_smooth = np.convolve(plot_in, np.ones(10) / 10, mode='same')
	plot_in_db = 20 * np.log10(plot_in_smooth)
	plot_out = np.append(plot_out[center:], plot_out[:center])
	plot_out = (np.abs(plot_out) / fft_times) / fft_size
	plot_out_smooth = np.convolve(plot_out, np.ones(10) / 10, mode='same')
	plot_out_db = 20 * np.log10(plot_out_smooth)

	# Calculate SIC Attenuation if there is no remote signal
	if not(usr_sett.remote):
		bw = usr_sett.sample_rate / usr_sett.samples_per_symbol
		resolution = usr_sett.sample_rate / fft_size
		half = int((bw / resolution) / 2)
		power_in = np.sum(plot_in[center - half:center + half]) / (2 * half)
		power_in_db = 20 * np.log10(power_in)
		power_out = np.sum(plot_out[center - half:center + half]) / (2 * half)
		power_out_db = 20 * np.log10(power_out)
		power_ratio = power_in_db - power_out_db

	# Create frequency axis
	scale = 1 / 1e6
	freq_start = scale * (usr_sett.center_freq - usr_sett.sample_rate / 2)
	freq_stop = scale * (usr_sett.center_freq + usr_sett.sample_rate / 2)
	x = np.linspace(freq_start, freq_stop, fft_size)

	# Plot FFT signals
	plt.plot(x, plot_out_db, 'g')
	plt.plot(x, plot_in_db, 'b')
	blue_patch = ptch.Patch(color='b', label='RX')
	green_patch = ptch.Patch(color='g', label='RX after Dig. SIC')
	legends = plt.legend(handles=[blue_patch, green_patch], loc=1)
	plt.gca().add_artist(legends)

	plt.xlabel('Frequency (MHz)')
	plt.ylabel('Power (dBm)')

	# Add SIC Attenuation result to the plot if there is no remote signal
	if not(usr_sett.remote):
		text = 'Dig. SIC: ' + "{0:.2f}".format(power_ratio) + ' dB'
		power_ration_result = ptch.Patch(color='black', label=text)
		plt.legend(handles=[power_ration_result], loc=3, fontsize=12)

	plt.title('Power Spectrum', fontsize=14, fontweight='bold')


def plot_h(res_buffs, usr_sett):

	plt.subplot(2, 2, 4)

	k = usr_sett.k
	start = 2 * k * (int(usr_sett.plot_frame_stop))
	stop = start + 2 * k
	x = np.arange(-k, k)
	plt.stem(x, abs(res_buffs.h_res[0][start:stop]))
	plt.xlabel('Tap Index')
	plt.ylabel('Amplitude')
	plt.title('Digital SI Canceller Taps', fontsize=14, fontweight='bold')


main()
