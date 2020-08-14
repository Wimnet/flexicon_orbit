#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Ofdm Link Fd Gui
# GNU Radio version: 3.7.13.5
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import fullduplex
import sip
import sys
import threading
import time
from gnuradio import qtgui


class ofdm_link_fd_gui(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Ofdm Link Fd Gui")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Ofdm Link Fd Gui")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "ofdm_link_fd_gui")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.occupied_carriers = occupied_carriers = (range(-26, -21) + range(-20, -7) + range(-6, 0) + range(1, 7) + range(8, 21) + range(22, 27),)
        self.sync_word_zeros = sync_word_zeros = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.sync_word = sync_word = [0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 1, 1, -1, -1, -1, 1, -1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 1, 1, -1, 1, 1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1, -1, 1, -1, 1, -1, -1, -1, -1, 0, 0, 0, 0, 0]
        self.pilot_symbols = pilot_symbols = ((1, 1, 1, -1,),)
        self.pilot_carriers = pilot_carriers = ((-21, -7, 7, 21,),)
        self.packet_len = packet_len = 32
        self.occupied_carriers_len = occupied_carriers_len = len(occupied_carriers[0])
        self.variable_function_probe_0_2 = variable_function_probe_0_2 = 0
        self.variable_function_probe_0_1_0 = variable_function_probe_0_1_0 = 0
        self.variable_function_probe_0_0_1 = variable_function_probe_0_0_1 = 0
        self.variable_function_probe_0_0_0_0 = variable_function_probe_0_0_0_0 = 0
        self.usrp_source_addr = usrp_source_addr = "addr0=192.168.30.3, addr1=192.168.30.4"
        self.usrp_sink_addr = usrp_sink_addr = "addr0=192.168.30.3, addr1=192.168.30.4"
        self.sync_word_zero_len = sync_word_zero_len = len(sync_word_zeros)
        self.sync_word_len = sync_word_len = len(sync_word)
        self.sync_word2 = sync_word2 = [0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 1, 1, -1, -1, -1, 1, -1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 1, 1, -1, 1, 1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1, -1, 1, -1, 1, -1, -1, -1, -1, 0, 0, 0, 0, 0]
        self.sync_word1 = sync_word1 = [0., 0., 0., 0., 0., 0., 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., -1.41421356, 0., -1.41421356, 0., -1.41421356, 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., -1.41421356, 0., -1.41421356, 0., -1.41421356, 0., -1.41421356, 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., 1.41421356, 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., 1.41421356, 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., 1.41421356, 0., 1.41421356, 0., 0., 0., 0., 0., 0.]
        self.symbols_num = symbols_num = (packet_len + 4)*8/2 /occupied_carriers_len + 3
        self.samp_rate = samp_rate = 5e6
        self.premultiply = premultiply = 1
        self.pilot_symbols_len = pilot_symbols_len = len(pilot_symbols[0])
        self.pilot_carriers_len = pilot_carriers_len = len(pilot_carriers[0])
        self.pad_tail_len = pad_tail_len = 1000
        self.pad_front_len = pad_front_len = 40000
        self.out_buf_size = out_buf_size = 192000
        self.len_tag_key = len_tag_key = "packet_len"
        self.freq = freq = 915e6
        self.frame_len = frame_len = ((packet_len+4)/12+3+2 + 2 + 2*2)*80
        self.fft_len = fft_len = 64
        self.delay_tx2rx = delay_tx2rx = 40

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_source = uhd.usrp_source(
        	",".join((usrp_source_addr, "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(2),
        	),
        )
        self.uhd_usrp_source.set_clock_source('mimo', 1)
        self.uhd_usrp_source.set_time_source('mimo', 1)
        self.uhd_usrp_source.set_samp_rate(samp_rate)
        self.uhd_usrp_source.set_center_freq(freq, 0)
        self.uhd_usrp_source.set_gain(10, 0)
        self.uhd_usrp_source.set_antenna('RX2', 0)
        self.uhd_usrp_source.set_auto_dc_offset(True, 0)
        self.uhd_usrp_source.set_auto_iq_balance(True, 0)
        self.uhd_usrp_source.set_center_freq(freq, 1)
        self.uhd_usrp_source.set_gain(10, 1)
        self.uhd_usrp_source.set_antenna('RX2', 1)
        self.uhd_usrp_source.set_auto_dc_offset(True, 1)
        self.uhd_usrp_source.set_auto_iq_balance(True, 1)
        self.uhd_usrp_sink = uhd.usrp_sink(
        	",".join((usrp_sink_addr, "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(2),
        	),
        )
        self.uhd_usrp_sink.set_clock_source('mimo', 1)
        self.uhd_usrp_sink.set_time_source('mimo', 1)
        self.uhd_usrp_sink.set_samp_rate(samp_rate)
        self.uhd_usrp_sink.set_center_freq(freq, 0)
        self.uhd_usrp_sink.set_gain(10, 0)
        self.uhd_usrp_sink.set_antenna('TX/RX', 0)
        self.uhd_usrp_sink.set_center_freq(freq, 1)
        self.uhd_usrp_sink.set_gain(10, 1)
        self.uhd_usrp_sink.set_antenna('TX/RX', 1)
        self.tab = Qt.QTabWidget()
        self.tab_widget_0 = Qt.QWidget()
        self.tab_layout_0 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab_widget_0)
        self.tab_grid_layout_0 = Qt.QGridLayout()
        self.tab_layout_0.addLayout(self.tab_grid_layout_0)
        self.tab.addTab(self.tab_widget_0, 'Rx Packets')
        self.tab_widget_1 = Qt.QWidget()
        self.tab_layout_1 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab_widget_1)
        self.tab_grid_layout_1 = Qt.QGridLayout()
        self.tab_layout_1.addLayout(self.tab_grid_layout_1)
        self.tab.addTab(self.tab_widget_1, 'Rx Signal Spectr')
        self.tab_widget_2 = Qt.QWidget()
        self.tab_layout_2 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab_widget_2)
        self.tab_grid_layout_2 = Qt.QGridLayout()
        self.tab_layout_2.addLayout(self.tab_grid_layout_2)
        self.tab.addTab(self.tab_widget_2, 'Estimated Digital Taps')
        self.top_grid_layout.addWidget(self.tab)

        def _variable_function_probe_0_2_probe():
            while True:
                val = self.uhd_usrp_sink.set_time_now(uhd.time_spec_t(0.0))
                try:
                    self.set_variable_function_probe_0_2(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (1e-9))
        _variable_function_probe_0_2_thread = threading.Thread(target=_variable_function_probe_0_2_probe)
        _variable_function_probe_0_2_thread.daemon = True
        _variable_function_probe_0_2_thread.start()


        def _variable_function_probe_0_1_0_probe():
            while True:
                val = self.uhd_usrp_source.set_time_now(uhd.time_spec_t(0.0))
                try:
                    self.set_variable_function_probe_0_1_0(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (1e-9))
        _variable_function_probe_0_1_0_thread = threading.Thread(target=_variable_function_probe_0_1_0_probe)
        _variable_function_probe_0_1_0_thread.daemon = True
        _variable_function_probe_0_1_0_thread.start()


        def _variable_function_probe_0_0_1_probe():
            while True:
                val = self.uhd_usrp_sink.set_start_time(uhd.time_spec_t(0.5))
                try:
                    self.set_variable_function_probe_0_0_1(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (1e-9))
        _variable_function_probe_0_0_1_thread = threading.Thread(target=_variable_function_probe_0_0_1_probe)
        _variable_function_probe_0_0_1_thread.daemon = True
        _variable_function_probe_0_0_1_thread.start()


        def _variable_function_probe_0_0_0_0_probe():
            while True:
                val = self.uhd_usrp_source.set_start_time(uhd.time_spec_t(0.5))
                try:
                    self.set_variable_function_probe_0_0_0_0(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (1e-9))
        _variable_function_probe_0_0_0_0_thread = threading.Thread(target=_variable_function_probe_0_0_0_0_probe)
        _variable_function_probe_0_0_0_0_thread.daemon = True
        _variable_function_probe_0_0_0_0_thread.start()

        self.qtgui_time_sink_x_1 = qtgui.time_sink_c(
        	21 * 1, #size
        	samp_rate, #samp_rate
        	"", #name
        	2 #number of inputs
        )
        self.qtgui_time_sink_x_1.set_update_time(0.20)
        self.qtgui_time_sink_x_1.set_y_axis(-0.5, 0.5)

        self.qtgui_time_sink_x_1.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_1.enable_tags(-1, True)
        self.qtgui_time_sink_x_1.set_trigger_mode(qtgui.TRIG_MODE_TAG, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "packet_len")
        self.qtgui_time_sink_x_1.enable_autoscale(False)
        self.qtgui_time_sink_x_1.enable_grid(True)
        self.qtgui_time_sink_x_1.enable_axis_labels(True)
        self.qtgui_time_sink_x_1.enable_control_panel(True)
        self.qtgui_time_sink_x_1.enable_stem_plot(False)

        if not True:
          self.qtgui_time_sink_x_1.disable_legend()

        labels = ['Est. h for FD node 0 - I', 'Est. h for FD node 0 - Q', 'Est. h for FD node 1 - I', 'Est. h for FD node 1 - Q', '',
                  '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [0, 0, 0, 0, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]

        for i in xrange(4):
            if len(labels[i]) == 0:
                if(i % 2 == 0):
                    self.qtgui_time_sink_x_1.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_1.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_1.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_1.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_1.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_1.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_1.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_1.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_1_win = sip.wrapinstance(self.qtgui_time_sink_x_1.pyqwidget(), Qt.QWidget)
        self.tab_grid_layout_2.addWidget(self._qtgui_time_sink_x_1_win)
        self.qtgui_time_sink_x_0_0 = qtgui.time_sink_f(
        	packet_len, #size
        	samp_rate, #samp_rate
        	'Scope Plot', #name
        	2 #number of inputs
        )
        self.qtgui_time_sink_x_0_0.set_update_time(0.1)
        self.qtgui_time_sink_x_0_0.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_0_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0_0.enable_autoscale(True)
        self.qtgui_time_sink_x_0_0.enable_grid(False)
        self.qtgui_time_sink_x_0_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0_0.enable_control_panel(True)
        self.qtgui_time_sink_x_0_0.enable_stem_plot(False)

        if not True:
          self.qtgui_time_sink_x_0_0.disable_legend()

        labels = ["Radio 1", "Radio 2", '', '', '',
                  '', '', '', '', '']
        widths = [2, 2, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]

        for i in xrange(2):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0_0.pyqwidget(), Qt.QWidget)
        self.tab_grid_layout_0.addWidget(self._qtgui_time_sink_x_0_0_win)
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	0, #fc
        	samp_rate, #bw
        	"", #name
        	2 #number of inputs
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(False)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0.enable_control_panel(True)

        if not True:
          self.qtgui_freq_sink_x_0.disable_legend()

        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_freq_sink_x_0.set_plot_pos_half(not True)

        labels = ["Radio 1", "Radio 2", '', '', '',
                  '', '', '', '', '']
        widths = [2, 2, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(2):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.tab_grid_layout_1.addWidget(self._qtgui_freq_sink_x_0_win)
        self.fullduplex_packet_encap2_0_0 = fullduplex.packet_encap2(False, False, 0.001, pad_front_len, pad_tail_len, 1, premultiply, 1)
        (self.fullduplex_packet_encap2_0_0).set_min_output_buffer(192000)
        self.fullduplex_packet_encap2_0 = fullduplex.packet_encap2(False, False, 0.001, pad_front_len, pad_tail_len, 0, premultiply, 1)
        (self.fullduplex_packet_encap2_0).set_min_output_buffer(192000)
        self.fullduplex_digital_sic_0_0 = fullduplex.digital_sic(False, delay_tx2rx, pad_front_len, 1, frame_len, 10, 1, premultiply)
        (self.fullduplex_digital_sic_0_0).set_min_output_buffer(192000)
        self.fullduplex_digital_sic_0 = fullduplex.digital_sic(False, delay_tx2rx, pad_front_len, 0, frame_len, 10, 1, premultiply)
        (self.fullduplex_digital_sic_0).set_min_output_buffer(192000)
        self.digital_ofdm_tx_0_0_0 = digital.ofdm_tx(
        	  fft_len=fft_len, cp_len=fft_len/4,
        	  packet_length_tag_key=len_tag_key,
        	  occupied_carriers=occupied_carriers,
        	  pilot_carriers=pilot_carriers,
        	  pilot_symbols=pilot_symbols,
        	  sync_word1=sync_word1,
        	  sync_word2=sync_word2,
        	  bps_header=1,
        	  bps_payload=2,
        	  rolloff=0,
        	  debug_log=False,
        	  scramble_bits=False
        	 )
        self.digital_ofdm_tx_0_0 = digital.ofdm_tx(
        	  fft_len=fft_len, cp_len=fft_len/4,
        	  packet_length_tag_key=len_tag_key,
        	  occupied_carriers=occupied_carriers,
        	  pilot_carriers=pilot_carriers,
        	  pilot_symbols=pilot_symbols,
        	  sync_word1=sync_word1,
        	  sync_word2=sync_word2,
        	  bps_header=1,
        	  bps_payload=2,
        	  rolloff=0,
        	  debug_log=False,
        	  scramble_bits=False
        	 )
        self.digital_ofdm_rx_0_0_0 = digital.ofdm_rx(
        	  fft_len=fft_len, cp_len=fft_len/4,
        	  frame_length_tag_key='frame_'+"rx_len_tag_key",
        	  packet_length_tag_key="rx_len_tag_key",
        	  occupied_carriers=occupied_carriers,
        	  pilot_carriers=pilot_carriers,
        	  pilot_symbols=pilot_symbols,
        	  sync_word1=sync_word1,
        	  sync_word2=sync_word2,
        	  bps_header=1,
        	  bps_payload=2,
        	  debug_log=False,
        	  scramble_bits=False
        	 )
        (self.digital_ofdm_rx_0_0_0).set_min_output_buffer(192000)
        self.digital_ofdm_rx_0_0 = digital.ofdm_rx(
        	  fft_len=fft_len, cp_len=fft_len/4,
        	  frame_length_tag_key='frame_'+"rx_len_tag_key",
        	  packet_length_tag_key="rx_len_tag_key",
        	  occupied_carriers=occupied_carriers,
        	  pilot_carriers=pilot_carriers,
        	  pilot_symbols=pilot_symbols,
        	  sync_word1=sync_word1,
        	  sync_word2=sync_word2,
        	  bps_header=1,
        	  bps_payload=2,
        	  debug_log=False,
        	  scramble_bits=False
        	 )
        (self.digital_ofdm_rx_0_0).set_min_output_buffer(192000)
        self.blocks_vector_source_x_0_0 = blocks.vector_source_b([0, 1, 2, 3, 4, 3, 2, 1], True, 1, ())
        self.blocks_vector_source_x_0 = blocks.vector_source_b([4, 4, 4 , 4, 0, 0, 0, 0], True, 1, ())
        self.blocks_uchar_to_float_0_0 = blocks.uchar_to_float()
        self.blocks_uchar_to_float_0 = blocks.uchar_to_float()
        self.blocks_tag_debug_0 = blocks.tag_debug(gr.sizeof_char*1, 'Rx Packets', ""); self.blocks_tag_debug_0.set_display(True)
        self.blocks_stream_to_tagged_stream_1_0 = blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, frame_len + pad_front_len + pad_tail_len, "packet_len")
        (self.blocks_stream_to_tagged_stream_1_0).set_min_output_buffer(192000)
        self.blocks_stream_to_tagged_stream_1 = blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, frame_len + pad_front_len + pad_tail_len, "packet_len")
        (self.blocks_stream_to_tagged_stream_1).set_min_output_buffer(192000)
        self.blocks_stream_to_tagged_stream_0_0 = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, packet_len, len_tag_key)
        self.blocks_stream_to_tagged_stream_0 = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, packet_len, len_tag_key)
        self.blocks_null_sink_0_0_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vcc((0.01*2.23 * 0.56 * 1.4 * 1.4 * premultiply, ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc((0.01*2.23 * 0.56 * 1.4 * 1.4 * premultiply, ))



        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.fullduplex_packet_encap2_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.fullduplex_packet_encap2_0_0, 0))
        self.connect((self.blocks_stream_to_tagged_stream_0, 0), (self.digital_ofdm_tx_0_0, 0))
        self.connect((self.blocks_stream_to_tagged_stream_0_0, 0), (self.digital_ofdm_tx_0_0_0, 0))
        self.connect((self.blocks_stream_to_tagged_stream_1, 0), (self.fullduplex_digital_sic_0, 1))
        self.connect((self.blocks_stream_to_tagged_stream_1_0, 0), (self.fullduplex_digital_sic_0_0, 1))
        self.connect((self.blocks_uchar_to_float_0, 0), (self.qtgui_time_sink_x_0_0, 0))
        self.connect((self.blocks_uchar_to_float_0_0, 0), (self.qtgui_time_sink_x_0_0, 1))
        self.connect((self.blocks_vector_source_x_0, 0), (self.blocks_stream_to_tagged_stream_0, 0))
        self.connect((self.blocks_vector_source_x_0_0, 0), (self.blocks_stream_to_tagged_stream_0_0, 0))
        self.connect((self.digital_ofdm_rx_0_0, 0), (self.blocks_uchar_to_float_0, 0))
        self.connect((self.digital_ofdm_rx_0_0_0, 0), (self.blocks_tag_debug_0, 0))
        self.connect((self.digital_ofdm_rx_0_0_0, 0), (self.blocks_uchar_to_float_0_0, 0))
        self.connect((self.digital_ofdm_tx_0_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.digital_ofdm_tx_0_0_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.fullduplex_digital_sic_0, 0), (self.blocks_null_sink_0_0_0, 0))
        self.connect((self.fullduplex_digital_sic_0, 1), (self.digital_ofdm_rx_0_0, 0))
        self.connect((self.fullduplex_digital_sic_0, 1), (self.qtgui_freq_sink_x_0, 0))
        self.connect((self.fullduplex_digital_sic_0, 2), (self.qtgui_time_sink_x_1, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 0), (self.blocks_null_sink_0_0_0, 1))
        self.connect((self.fullduplex_digital_sic_0_0, 1), (self.digital_ofdm_rx_0_0_0, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 1), (self.qtgui_freq_sink_x_0, 1))
        self.connect((self.fullduplex_digital_sic_0_0, 2), (self.qtgui_time_sink_x_1, 1))
        self.connect((self.fullduplex_packet_encap2_0, 0), (self.fullduplex_digital_sic_0, 0))
        self.connect((self.fullduplex_packet_encap2_0, 0), (self.uhd_usrp_sink, 0))
        self.connect((self.fullduplex_packet_encap2_0_0, 0), (self.fullduplex_digital_sic_0_0, 0))
        self.connect((self.fullduplex_packet_encap2_0_0, 0), (self.uhd_usrp_sink, 1))
        self.connect((self.uhd_usrp_source, 0), (self.blocks_stream_to_tagged_stream_1, 0))
        self.connect((self.uhd_usrp_source, 1), (self.blocks_stream_to_tagged_stream_1_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "ofdm_link_fd_gui")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_occupied_carriers(self):
        return self.occupied_carriers

    def set_occupied_carriers(self, occupied_carriers):
        self.occupied_carriers = occupied_carriers
        self.set_occupied_carriers_len(len(self.occupied_carriers[0]))

    def get_sync_word_zeros(self):
        return self.sync_word_zeros

    def set_sync_word_zeros(self, sync_word_zeros):
        self.sync_word_zeros = sync_word_zeros
        self.set_sync_word_zero_len(len(self.sync_word_zeros))

    def get_sync_word(self):
        return self.sync_word

    def set_sync_word(self, sync_word):
        self.sync_word = sync_word
        self.set_sync_word_len(len(self.sync_word))

    def get_pilot_symbols(self):
        return self.pilot_symbols

    def set_pilot_symbols(self, pilot_symbols):
        self.pilot_symbols = pilot_symbols
        self.set_pilot_symbols_len(len(self.pilot_symbols[0]))

    def get_pilot_carriers(self):
        return self.pilot_carriers

    def set_pilot_carriers(self, pilot_carriers):
        self.pilot_carriers = pilot_carriers
        self.set_pilot_carriers_len(len(self.pilot_carriers[0]))

    def get_packet_len(self):
        return self.packet_len

    def set_packet_len(self, packet_len):
        self.packet_len = packet_len
        self.set_frame_len(((self.packet_len+4)/12+3+2 + 2 + 2*2)*80 )
        self.set_symbols_num((self.packet_len + 4)*8/2 /self.occupied_carriers_len + 3)
        self.blocks_stream_to_tagged_stream_0_0.set_packet_len(self.packet_len)
        self.blocks_stream_to_tagged_stream_0_0.set_packet_len_pmt(self.packet_len)
        self.blocks_stream_to_tagged_stream_0.set_packet_len(self.packet_len)
        self.blocks_stream_to_tagged_stream_0.set_packet_len_pmt(self.packet_len)

    def get_occupied_carriers_len(self):
        return self.occupied_carriers_len

    def set_occupied_carriers_len(self, occupied_carriers_len):
        self.occupied_carriers_len = occupied_carriers_len
        self.set_symbols_num((self.packet_len + 4)*8/2 /self.occupied_carriers_len + 3)

    def get_variable_function_probe_0_2(self):
        return self.variable_function_probe_0_2

    def set_variable_function_probe_0_2(self, variable_function_probe_0_2):
        self.variable_function_probe_0_2 = variable_function_probe_0_2

    def get_variable_function_probe_0_1_0(self):
        return self.variable_function_probe_0_1_0

    def set_variable_function_probe_0_1_0(self, variable_function_probe_0_1_0):
        self.variable_function_probe_0_1_0 = variable_function_probe_0_1_0

    def get_variable_function_probe_0_0_1(self):
        return self.variable_function_probe_0_0_1

    def set_variable_function_probe_0_0_1(self, variable_function_probe_0_0_1):
        self.variable_function_probe_0_0_1 = variable_function_probe_0_0_1

    def get_variable_function_probe_0_0_0_0(self):
        return self.variable_function_probe_0_0_0_0

    def set_variable_function_probe_0_0_0_0(self, variable_function_probe_0_0_0_0):
        self.variable_function_probe_0_0_0_0 = variable_function_probe_0_0_0_0

    def get_usrp_source_addr(self):
        return self.usrp_source_addr

    def set_usrp_source_addr(self, usrp_source_addr):
        self.usrp_source_addr = usrp_source_addr

    def get_usrp_sink_addr(self):
        return self.usrp_sink_addr

    def set_usrp_sink_addr(self, usrp_sink_addr):
        self.usrp_sink_addr = usrp_sink_addr

    def get_sync_word_zero_len(self):
        return self.sync_word_zero_len

    def set_sync_word_zero_len(self, sync_word_zero_len):
        self.sync_word_zero_len = sync_word_zero_len

    def get_sync_word_len(self):
        return self.sync_word_len

    def set_sync_word_len(self, sync_word_len):
        self.sync_word_len = sync_word_len

    def get_sync_word2(self):
        return self.sync_word2

    def set_sync_word2(self, sync_word2):
        self.sync_word2 = sync_word2

    def get_sync_word1(self):
        return self.sync_word1

    def set_sync_word1(self, sync_word1):
        self.sync_word1 = sync_word1

    def get_symbols_num(self):
        return self.symbols_num

    def set_symbols_num(self, symbols_num):
        self.symbols_num = symbols_num

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_source.set_samp_rate(self.samp_rate)
        self.uhd_usrp_sink.set_samp_rate(self.samp_rate)
        self.qtgui_time_sink_x_1.set_samp_rate(self.samp_rate)
        self.qtgui_time_sink_x_0_0.set_samp_rate(self.samp_rate)
        self.qtgui_freq_sink_x_0.set_frequency_range(0, self.samp_rate)

    def get_premultiply(self):
        return self.premultiply

    def set_premultiply(self, premultiply):
        self.premultiply = premultiply
        self.fullduplex_packet_encap2_0_0.set_premultiplier(self.premultiply)
        self.fullduplex_packet_encap2_0.set_premultiplier(self.premultiply)
        self.blocks_multiply_const_vxx_0_0.set_k((0.01*2.23 * 0.56 * 1.4 * 1.4 * self.premultiply, ))
        self.blocks_multiply_const_vxx_0.set_k((0.01*2.23 * 0.56 * 1.4 * 1.4 * self.premultiply, ))

    def get_pilot_symbols_len(self):
        return self.pilot_symbols_len

    def set_pilot_symbols_len(self, pilot_symbols_len):
        self.pilot_symbols_len = pilot_symbols_len

    def get_pilot_carriers_len(self):
        return self.pilot_carriers_len

    def set_pilot_carriers_len(self, pilot_carriers_len):
        self.pilot_carriers_len = pilot_carriers_len

    def get_pad_tail_len(self):
        return self.pad_tail_len

    def set_pad_tail_len(self, pad_tail_len):
        self.pad_tail_len = pad_tail_len
        self.blocks_stream_to_tagged_stream_1_0.set_packet_len(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1_0.set_packet_len_pmt(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1.set_packet_len(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1.set_packet_len_pmt(self.frame_len + self.pad_front_len + self.pad_tail_len)

    def get_pad_front_len(self):
        return self.pad_front_len

    def set_pad_front_len(self, pad_front_len):
        self.pad_front_len = pad_front_len
        self.blocks_stream_to_tagged_stream_1_0.set_packet_len(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1_0.set_packet_len_pmt(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1.set_packet_len(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1.set_packet_len_pmt(self.frame_len + self.pad_front_len + self.pad_tail_len)

    def get_out_buf_size(self):
        return self.out_buf_size

    def set_out_buf_size(self, out_buf_size):
        self.out_buf_size = out_buf_size

    def get_len_tag_key(self):
        return self.len_tag_key

    def set_len_tag_key(self, len_tag_key):
        self.len_tag_key = len_tag_key

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.uhd_usrp_source.set_center_freq(self.freq, 0)
        self.uhd_usrp_source.set_center_freq(self.freq, 1)
        self.uhd_usrp_sink.set_center_freq(self.freq, 0)
        self.uhd_usrp_sink.set_center_freq(self.freq, 1)

    def get_frame_len(self):
        return self.frame_len

    def set_frame_len(self, frame_len):
        self.frame_len = frame_len
        self.blocks_stream_to_tagged_stream_1_0.set_packet_len(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1_0.set_packet_len_pmt(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1.set_packet_len(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1.set_packet_len_pmt(self.frame_len + self.pad_front_len + self.pad_tail_len)

    def get_fft_len(self):
        return self.fft_len

    def set_fft_len(self, fft_len):
        self.fft_len = fft_len

    def get_delay_tx2rx(self):
        return self.delay_tx2rx

    def set_delay_tx2rx(self, delay_tx2rx):
        self.delay_tx2rx = delay_tx2rx
        self.fullduplex_digital_sic_0_0.set_delay_tx2rx(self.delay_tx2rx)
        self.fullduplex_digital_sic_0.set_delay_tx2rx(self.delay_tx2rx)


def main(top_block_cls=ofdm_link_fd_gui, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
