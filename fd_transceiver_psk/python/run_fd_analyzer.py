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
# Date: Apr. 15, 2018
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
	usrp_in, usrp_out, remote_signal = open_files('usrp_in', 'usrp_out', 'remote_signal', usr_sett)

	# Create SIC object
	sic = grc_sic.sic(usr_sett.frame_size, usr_sett.pream_size, usr_sett.k, usr_sett.order, usr_sett.h_frames, usr_sett.er, usr_sett.wr, usr_sett.ir)

	# Create empty buffers and set SIC buffers size
	sic_buffs = sic_buffers()
	res_buffs = result_buffers()
	sic_input_buffers_size = usr_sett.frame_size + usr_sett.k
	sic_output_buffers_size = usr_sett.frame_size

	# Get the first frame size samples from data files
	get_first_frame(sic_input_buffers_size, sic_buffs, usr_sett, usrp_in, usrp_out, remote_signal)
	sic_buffs.output_items[0] = np.zeros(int(sic_output_buffers_size), np.complex64)

	# Do SIC work and append all buffers to the results buffers
	out_len = sic.work(sic_buffs.input_items, sic_buffs.output_items)
	save_results(sic_buffs, out_len, sic.h, res_buffs)

	# For every data frame, get frame size samples, do SIC work and append all buffers to the results buffers
	for i in range(0, int(usr_sett.plot_frame_stop)):
		get_frame(sic_input_buffers_size, sic_buffs, usr_sett, usrp_in, usrp_out, remote_signal)
		sic_buffs.output_items[0] = np.zeros(int(sic_output_buffers_size), np.complex64)
		out_len = sic.work(sic_buffs.input_items, sic_buffs.output_items)
		save_results(sic_buffs, out_len, sic.h, res_buffs)

	# Plot signals in time domain, show the constellation, FFT and h weights vector
	plot(res_buffs, usr_sett)
	save_frame2files(res_buffs, usr_sett, 'usrp_in_frame', 'usrp_out_frame', 'sic_out_frame')

	# padd = 0.3
	# plt.subplots_adjust(hspace=padd, wspace=padd)
	plt.show()


class user_settings:
	def __init__(self):
		# default settings

		# sic settings
		self.frame_size = 20e3
		self.pream_size = 1e3
		self.k = 50
		self.order = 1
		self.h_frames = 1
		self.er, self.wr, self.ir = 1, 0, 0

		# signals settings
		self.remote = 0
		self.usrp_delay = 29
		self.remote_delay = 0
		self.sample_rate = 10e6
		self.center_freq = 900e6
		self.samples_per_symbol = 2
		self.tx_gain = 10
		self.rx_gain = 25
		self.rx_gain_cal = 3  # -30 to simulate a circulator

		# plot settings
		self.plot_im = 0
		self.plot_usrp_in = 0
		self.plot_usrp_out = 1
		self.plot_frame_start = 50
		self.plot_frame_stop = 50
		self.ffts_per_frame = 1
		self.const_points_scale = 1
		self.fft_smooth = 10
		self.remove_center_band = 200e3
		self.plot_only_fft = 0

	def get_user_settings(self):

		def get(old_val, string):
			from_user = raw_input(string + ': ')
			if from_user != '':
				return float(from_user)
			else:
				return old_val

		# 1st menu
		print('Enter      \'s\' - Edit Settings      \'def\' - Load Default Settings')
		print('Press ENTER to skip')
		user_input = raw_input()

		if user_input == 's' or user_input == 'S':
			self.read_settings_file()

			# Signals settings menu
			print('\nSignals settings:')
			print('- USRP TX/RX delay = {} samps\n'
				'- TX gain = {} dB\n'
				'- RX gain = {} dB\n'
				'- RX calibration = {} dB\n'
				'- Sample rate = {} samp/sec\n'
				'- Samples per symbol = {}\n'
				'- Carrier frequency = {} Hz\n'
				'- Simulated remote signal on = {}\n'
				'- Simulated remote signal delay = {} samps\n'.format(self.usrp_delay, self.tx_gain, self.rx_gain, self.rx_gain_cal, self.sample_rate, self.samples_per_symbol, self.center_freq, self.remote, self.remote_delay))
			print('Enter \'s\' to edit       Press ENTER to skip')
			user_input = raw_input()
			if user_input == 's' or user_input == 'S':
				self.usrp_delay = get(self.usrp_delay, 'USRP TX/RX delay (samps)')
				self.tx_gain = get(self.tx_gain, 'TX gain (db)')
				self.rx_gain = get(self.rx_gain, 'RX gain (db)')
				self.rx_gain_cal = get(self.rx_gain_cal, 'RX gain calibration (db)')
				self.sample_rate = get(self.sample_rate, 'Sample rate (samp/sec)')
				self.samples_per_symbol = get(self.samples_per_symbol, 'Samples per symbol')
				self.center_freq = get(self.center_freq, 'Carrier frequency (Hz)')
				self.remote = get(self.remote, 'Simulated remote signal on, 1/0')
				self.remote_delay = get(self.remote_delay, 'Simulated remote signal delay (samps)')

			# SIC settings menu
			print('\nSIC settings:')
			print('- Non-linearity order = {} \n'
				'- # of digital SI canceller taps = {} samps\n'
				'- Frame Size = {} samps\n'
				'- Preamble Size = {} samps\n'
				'- Update frequency of h = {} frames\n'.format(self.order, self.k, self.frame_size, self.pream_size, self.h_frames))
			print('Enter \'s\' to edit       Press ENTER to skip')
			user_input = raw_input()
			if user_input == 's' or user_input == 'S':
				self.order = get(self.order, 'Non-linearity order')
				self.k = get(self.k, '# of digital SI canceller taps (samps)')
				self.frame_size = get(self.frame_size, 'Frame Size (samps)')
				self.pream_size = get(self.pream_size, 'Preamble Size (samps)')
				self.h_frames = get(self.h_frames, 'Update frequency of h (frames)')

			# Plot settings menu
			print('\nPlot settings:')
			print('- Start plot from frame = {} \n'
				'- End plot in frame = {} \n'
				'- Disregard center band = {} \n'
				'- # of FFTs per frame = {} \n'
				'- Constellation interleaving = {} samps\n'
				'- Power spectrum smoothing = {} samps\n'
				'- Plot RX signals = {} \n'
				'- Plot TX signals = {} \n'
				'- Plot Im parts = {} \n'
				'- Plot only FFT = {} \n'.format(self.plot_frame_start, self.plot_frame_stop, self.remove_center_band, self.ffts_per_frame, self.const_points_scale, self.fft_smooth, self.plot_usrp_out, self.plot_usrp_in, self.plot_im, self.plot_only_fft))
			print('Enter \'s\' to edit       Press ENTER to skip')
			user_input = raw_input()
			if user_input == 's' or user_input == 'S':
				self.plot_frame_start = get(self.plot_frame_start, 'Start plot from frame')
				self.plot_frame_stop = get(self.plot_frame_stop, 'End plot in frame')
				self.remove_center_band = get(self.remove_center_band, 'Disregard center band (Hz)')
				self.ffts_per_frame = get(self.ffts_per_frame, '# of FFTs per frame')
				self.const_points_scale = get(self.const_points_scale, 'Constellation interleaving (samps)')
				self.fft_smooth = get(self.fft_smooth, 'Power spectrum smoothing (samps)')
				self.plot_usrp_out = get(self.plot_usrp_out, 'Plot RX signals, 1/0')
				self.plot_usrp_in = get(self.plot_usrp_in, 'Plot TX signals, 1/0')
				self.plot_im = get(self.plot_im, 'Plot RX signals (imag), 1/0')
				self.plot_only_fft = get(self.plot_only_fft, 'Plot only FFT, 1/0')


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


def get_first_frame(buffer_size, sic_buffs, usr_sett, usrp_in, usrp_out, remote_signal=''):

	sic_buffs.input_items[0], sic_buffs.input_items[1] = np.zeros(int(buffer_size), np.complex64), np.zeros(int(buffer_size), np.complex64)
	sic_buffs.remote_items[0] = np.zeros(int(buffer_size), np.complex64)

	for i in range(0, int(buffer_size)):

		if i < usr_sett.usrp_delay:  # add (usrp_delay) zeros to the start of the samples going into the usrp
			sic_buffs.input_items[1][i] = np.complex64(0)
		else:
			sic_buffs.input_items[1][i] = get_sample(usrp_in)

		if usr_sett.remote and remote_signal != '':
			if i < usr_sett.remote_delay:  # add (remote_delay) zeros to the start of the remote samples
				sic_buffs.remote_items[0][i] = np.complex64(0)
				sic_buffs.input_items[0][i] = get_sample(usrp_out)

			else:
				sic_buffs.remote_items[0][i] = get_sample(remote_signal)
				sic_buffs.input_items[0][i] = get_sample(usrp_out) + sic_buffs.remote_items[0][i]

		else:
			sic_buffs.input_items[0][i] = get_sample(usrp_out)


def get_frame(buffer_size, sic_buffs, usr_sett, usrp_in, usrp_out, remote_signal=''):

	sic_buffs.input_items[0], sic_buffs.input_items[1] = np.zeros(int(buffer_size), np.complex64), np.zeros(int(buffer_size), np.complex64)
	sic_buffs.remote_items[0] = np.zeros(int(buffer_size), np.complex64)

	if usr_sett.remote and remote_signal != '':
		for i in range(0, int(buffer_size)):
			u_in_samp = get_sample(usrp_in)
			r_s_samp = get_sample(remote_signal)
			u_out_samp = get_sample(usrp_out)
			if u_in_samp == '' or r_s_samp == '' or u_out_samp == '':
				break
			sic_buffs.input_items[1][i] = u_in_samp
			sic_buffs.remote_items[0][i] = r_s_samp
			sic_buffs.input_items[0][i] = u_out_samp + r_s_samp

	else:
		for i in range(0, int(buffer_size)):
			u_in_samp = get_sample(usrp_in)
			u_out_samp = get_sample(usrp_out)
			if u_in_samp == '' or u_out_samp == '':
				break
			sic_buffs.input_items[1][i] = u_in_samp
			sic_buffs.input_items[0][i] = u_out_samp


def get_sample(file):

	samp = file.read(8)

	if samp == '':
		return ''

	return np.frombuffer(samp, dtype=np.dtype('complex64'))[0]


def save_results(sic_buffs, out_len, h, res_buffs):

	res_buffs.out_len_res[0] += out_len
	res_buffs.h_res[0] = np.append(res_buffs.h_res[0], h)
	res_buffs.in_res[0] = np.append(res_buffs.in_res[0], sic_buffs.input_items[0])
	res_buffs.in_res[1] = np.append(res_buffs.in_res[1], sic_buffs.input_items[1])
	res_buffs.out_res[0] = np.append(res_buffs.out_res[0], sic_buffs.output_items[0][:out_len])
	res_buffs.rem_res[0] = np.append(res_buffs.rem_res[0], sic_buffs.remote_items[0])


def open_files(usrp_in, usrp_out, remote_signal, usr_sett):

	u_in = open(usrp_in, 'r')
	u_out = open(usrp_out, 'r')

	if not os.path.isfile(remote_signal):
		usr_sett.remote = 0
		return u_in, u_out, ''

	if usr_sett.remote:
		re_sig = open(remote_signal, 'r')
		return u_in, u_out, re_sig
	else:
		return u_in, u_out, ''


def plot(res_buffs, usr_sett):

	if int(usr_sett.plot_only_fft):
		plot_fft(res_buffs, usr_sett, 0)
		plt.show(block=False)
		return

	plt.subplots(2, 2)
	plot_signal(res_buffs, usr_sett, 1)
	plot_constellation(res_buffs, usr_sett, 2)
	plot_fft(res_buffs, usr_sett, 3)
	plot_h(res_buffs, usr_sett, 4)

	padd = 0.3
	plt.subplots_adjust(hspace=padd, wspace=padd)
	plt.show(block=False)


def plot_signal(res_buffs, usr_sett, plot_num):

	if plot_num != 0:
		plt.subplot(2, 2, plot_num)

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

	# RX calibration scaling, TX and RX gain scaling
	tx_scale = 10 ** (float(usr_sett.tx_gain) / 10)
	rx_cal = usr_sett.rx_gain_cal - usr_sett.rx_gain
	rx_scale = 10 ** (float(rx_cal) / 20)

	# Plot signals based on user settings
	if usr_sett.plot_im:
		if usr_sett.plot_usrp_in:
			plt.plot(x, tx_scale * res_buffs.in_res[1][start:stop].imag, 'black', linestyle='--')

		if usr_sett.plot_usrp_out:
			plt.plot(x, rx_scale * res_buffs.in_res[0][start:stop].imag, 'red', linestyle='--')

		plt.plot(x, rx_scale * res_buffs.out_res[0][start:stop].imag, 'blue', linestyle='--')

		if usr_sett.remote:
			plt.plot(x, res_buffs.rem_res[0][start:stop].imag, 'green', linestyle='--')

	if usr_sett.plot_usrp_in:
		plt.plot(x, tx_scale * res_buffs.in_res[1][start:stop].real, 'black')

	if usr_sett.plot_usrp_out:
		plt.plot(x, rx_scale * res_buffs.in_res[0][start:stop].real, 'red')

	plt.plot(x, rx_scale * res_buffs.out_res[0][start:stop].real, 'blue')

	if usr_sett.remote:
		plt.plot(x, res_buffs.rem_res[0][start:stop].real, 'green')

	black_patch_r = ptch.Patch(color='black', label='TX Re')
	black_patch_i = ptch.Patch(color='black', linestyle='--', fill=0, label='TX Im')
	red_patch_r = ptch.Patch(color='red', label='RX Re')
	red_patch_i = ptch.Patch(color='red', linestyle='--', fill=0, label='RX Im')
	green_patch_r = ptch.Patch(color='green', label='Remote Signal Re')
	green_patch_i = ptch.Patch(color='green', linestyle='--', fill=0, label='Remote Signal Im')
	blue_patch_r = ptch.Patch(color='blue', label='RX after Dig. SIC Re')
	blue_patch_i = ptch.Patch(color='blue', linestyle='--', fill=0, label='RX after Dig. SIC Im')

	patches = []
	if usr_sett.plot_usrp_in:
		patches.append(black_patch_r)
		if usr_sett.plot_im:
			patches.append(black_patch_i)

	if usr_sett.plot_usrp_out:
		patches.append(red_patch_r)
		if usr_sett.plot_im:
			patches.append(red_patch_i)

	if usr_sett.remote:
		patches.append(green_patch_r)
		if usr_sett.plot_im:
			patches.append(green_patch_i)

	patches.append(blue_patch_r)
	if usr_sett.plot_im:
		patches.append(blue_patch_i)

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


def plot_constellation(res_buffs, usr_sett, plot_num):

	if plot_num != 0:
		plt.subplot(2, 2, plot_num)

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

	# RX calibration scaling, TX and RX gain scaling
	tx_scale = 10 ** (float(usr_sett.tx_gain) / 10)
	rx_cal = usr_sett.rx_gain_cal - usr_sett.rx_gain
	rx_scale = 10 ** (float(rx_cal) / 20)

	# Plot constellation points every 'num' samples
	num = int(usr_sett.const_points_scale)

	if usr_sett.plot_usrp_in:
		plt.scatter(tx_scale * res_buffs.in_res[1][start:stop].real[0::num], tx_scale * res_buffs.in_res[1][start:stop].imag[0::num], c='black', marker='.')

	if usr_sett.plot_usrp_out:
		plt.scatter(rx_scale * res_buffs.in_res[0][start:stop].real[0::num], rx_scale * res_buffs.in_res[0][start:stop].imag[0::num], c='red', marker='.')

	plt.scatter(rx_scale * res_buffs.out_res[0][start:stop].real[0::num], rx_scale * res_buffs.out_res[0][start:stop].imag[0::num], c='blue', marker='x')

	if usr_sett.remote:
		plt.scatter(res_buffs.rem_res[0][start:stop].real[0::num], res_buffs.rem_res[0][start:stop].imag[0::num], c='green', marker='o')

	black_patch = ptch.Patch(color='black', label='TX')
	red_patch = ptch.Patch(color='red', label='RX')
	green_patch = ptch.Patch(color='green', label='Remote Signal')
	blue_patch = ptch.Patch(color='blue', label='RX after Dig. SIC')

	patches = []
	if usr_sett.plot_usrp_in:
		patches.append(black_patch)

	if usr_sett.plot_usrp_out:
		patches.append(red_patch)

	if usr_sett.remote:
		patches.append(green_patch)

	patches.append(blue_patch)

	plt.legend(handles=patches)

	plt.xlabel('I')
	plt.ylabel('Q')
	plt.axhline(y=0, color='k')
	plt.axvline(x=0, color='k')

	plt.title('Constellation', fontsize=14, fontweight='bold')


def plot_fft(res_buffs, usr_sett, plot_num):

	if plot_num != 0:
		plt.subplot(2, 2, plot_num)

	if res_buffs.out_len_res[0] == 0:
		return

	# Put the last user requested frame in sic_in and sic_out
	fr = int(usr_sett.plot_frame_stop)
	fr_size = int(usr_sett.frame_size)
	usrp_in = res_buffs.in_res[1][fr * fr_size:(fr + 1) * fr_size]
	usrp_out = res_buffs.in_res[0][fr * fr_size:(fr + 1) * fr_size]
	sic_out = res_buffs.out_res[0][fr * fr_size:(fr + 1) * fr_size]

	# Calculate FFT for both signals
	fft_times = int(usr_sett.ffts_per_frame) + (usr_sett.ffts_per_frame == 0) * 1
	fft_size = int(usr_sett.frame_size / fft_times)
	fft_usrp_in = np.zeros(fft_size)
	fft_usrp_out = np.zeros(fft_size)
	fft_sic_out = np.zeros(fft_size)
	for i in range(0, fft_times):
		fft_usrp_in += np.abs(np.fft.fft(usrp_in[i * fft_size:(i + 1) * fft_size])) ** 2 / fft_size
		fft_usrp_out += np.abs(np.fft.fft(usrp_out[i * fft_size:(i + 1) * fft_size])) ** 2 / fft_size
		fft_sic_out += np.abs(np.fft.fft(sic_out[i * fft_size:(i + 1) * fft_size])) ** 2 / fft_size

	fft_usrp_in = fft_usrp_in / fft_times
	fft_usrp_out = fft_usrp_out / fft_times
	fft_sic_out = fft_sic_out / fft_times

	# Move the center freq to the the middle of the array
	center = int(fft_size / 2)
	fft_usrp_in = np.append(fft_usrp_in[center:], fft_usrp_in[:center])
	fft_usrp_out = np.append(fft_usrp_out[center:], fft_usrp_out[:center])
	fft_sic_out = np.append(fft_sic_out[center:], fft_sic_out[:center])

	# Smooth the samples
	smooth = int(usr_sett.fft_smooth)
	fft_usrp_in_smooth = np.convolve(fft_usrp_in, np.ones(smooth) / smooth, mode='same')
	fft_usrp_out_smooth = np.convolve(fft_usrp_out, np.ones(smooth) / smooth, mode='same')
	fft_sic_out_smooth = np.convolve(fft_sic_out, np.ones(smooth) / smooth, mode='same')

	# Calculate FFT spectrum in dBm
	fft_usrp_in_db = 10 * np.log10(fft_usrp_in_smooth)
	fft_usrp_out_db = 10 * np.log10(fft_usrp_out_smooth)
	fft_sic_out_db = 10 * np.log10(fft_sic_out_smooth)

	# TX and RX calibration scaling, RX gain scaling
	fft_usrp_in_db += usr_sett.tx_gain
	fft_usrp_out_db += usr_sett.rx_gain_cal - usr_sett.rx_gain
	fft_sic_out_db += usr_sett.rx_gain_cal - usr_sett.rx_gain

	# Calculate average SIC Attenuation if there is no remote signal
	if not(usr_sett.remote):
		bw = usr_sett.sample_rate / usr_sett.samples_per_symbol
		resolution = usr_sett.sample_rate / fft_size
		half = int(bw / resolution / 2)
		samps_remove = int(usr_sett.remove_center_band / resolution / 2)
		samps_remove = (samps_remove < half) * samps_remove + (half <= samps_remove) * 0
		tx_scale = 10 ** (usr_sett.tx_gain / 10)
		rx_scale = 10 ** ((usr_sett.rx_gain_cal - usr_sett.rx_gain) / 10)

		power_usrp_in = np.sum(fft_usrp_in[center - half:center - samps_remove] * tx_scale + fft_usrp_in[center + samps_remove:center + half] * tx_scale) / (2 * half - samps_remove * 2)
		power_usrp_in_db = 10 * np.log10(power_usrp_in)
		power_sic_in = np.sum(fft_usrp_out[center - half:center - samps_remove] * rx_scale + fft_usrp_out[center + samps_remove:center + half] * rx_scale) / (2 * half - samps_remove * 2)
		power_sic_in_db = 10 * np.log10(power_sic_in)
		power_sic_out = np.sum((fft_sic_out[center - half:center - samps_remove] * rx_scale + fft_sic_out[center + samps_remove:center + half]) * rx_scale) / (2 * half - samps_remove * 2)
		power_sic_out_db = 10 * np.log10(power_sic_out)
		power_hsic_ratio = power_usrp_in_db - power_sic_in_db
		power_dsic_ratio = power_sic_in_db - power_sic_out_db

	# Create frequency axis
	scale = 1 / 1e6
	freq_start = scale * (usr_sett.center_freq - usr_sett.sample_rate / 2)
	freq_stop = scale * (usr_sett.center_freq + usr_sett.sample_rate / 2)
	x = np.linspace(freq_start, freq_stop, fft_size)

	# Plot FFT signals
	plt.plot(x, fft_usrp_in_db, 'black')
	plt.plot(x, fft_usrp_out_db, 'red')
	plt.plot(x, fft_sic_out_db, 'blue')
	black_patch = ptch.Patch(color='black', label='TX')
	red_patch = ptch.Patch(color='red', label='RX')
	blue_patch = ptch.Patch(color='blue', label='RX after Dig. SIC')
	legends = plt.legend(handles=[black_patch, red_patch, blue_patch], loc=1)
	plt.gca().add_artist(legends)

	plt.xlabel('Frequency (MHz)')
	plt.ylabel('Power (dBm)')

	# Add SIC Attenuation result to the plot if there is no remote signal
	if not(usr_sett.remote):
		text = 'Hw. SIC: ' + "{0:.2f}".format(power_hsic_ratio) + ' dB'
		hsic_result = ptch.Patch(color='black', label=text)
		text = 'Dig. SIC: ' + "{0:.2f}".format(power_dsic_ratio) + ' dB'
		dsic_result = ptch.Patch(color='black', label=text)
		plt.legend(handles=[hsic_result, dsic_result], loc=3, fontsize=12)

	plt.title('Power Spectrum', fontsize=14, fontweight='bold')
	plt.grid()


def plot_h(res_buffs, usr_sett, plot_num):

	if plot_num != 0:
		plt.subplot(2, 2, plot_num)

	k = int(usr_sett.k)
	order = int(usr_sett.order)
	# make sure that order >=1
	order = (order < 1) * 1 + (order >= 1) * order
	start = 2 * k * order * int(usr_sett.plot_frame_stop)
	stop = start + 2 * k * order
	x = np.arange(-k, 2 * k * order - k)
	# edit x ticks to show every order part of h, with it's own tap indexes
	skip_tick = 2 * int(usr_sett.k / 10) * order
	xt = np.arange(-k, k, skip_tick)
	xt = np.tile(xt, order)
	plt.stem(x, abs(res_buffs.h_res[0][start:stop]))
	# ticks_loc = np.arange(0, 2 * k * order, skip_tick)
	# plt.xticks(ticks_loc, xt)
	plt.autoscale(enable=True, axis='x', tight=True)
	plt.xlabel('Tap Index')
	plt.ylabel('Amplitude')
	plt.title('Digital SI Canceller Taps', fontsize=14, fontweight='bold')


def save_frame2files(res_buffs, usr_sett, name_usrp_in, name_usrp_out, name_sic_out):

	if res_buffs.out_len_res[0] == 0:
		return

	# Put the last user requested frame in sic_in and sic_out
	fr = int(usr_sett.plot_frame_stop)
	fr_size = int(usr_sett.frame_size)
	usrp_in = np.complex64(res_buffs.in_res[1][fr * fr_size:(fr + 1) * fr_size])
	usrp_out = np.complex64(res_buffs.in_res[0][fr * fr_size:(fr + 1) * fr_size])
	sic_out = np.complex64(res_buffs.out_res[0][fr * fr_size:(fr + 1) * fr_size])

	u_in = open(name_usrp_in, 'wb')
	u_out = open(name_usrp_out, 'wb')
	s_out = open(name_sic_out, 'wb')

	u_in.write(usrp_in.tobytes())
	u_out.write(usrp_out.tobytes())
	s_out.write(sic_out.tobytes())

	u_in.close()
	u_out.close()
	s_out.close()


main()
