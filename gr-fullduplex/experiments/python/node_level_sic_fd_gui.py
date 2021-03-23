#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Node Level Sic Fd Gui
# GNU Radio version: 3.8.2.0

from distutils.version import StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio.filter import firdes
import sip
from gnuradio import blocks
import numpy
from gnuradio import digital
from gnuradio import gr
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import uhd
import time
from gnuradio.qtgui import Range, RangeWidget
import fullduplex
import threading

from gnuradio import qtgui

class node_level_sic_fd_gui(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Node Level Sic Fd Gui")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Node Level Sic Fd Gui")
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

        self.settings = Qt.QSettings("GNU Radio", "node_level_sic_fd_gui")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.occupied_carriers = occupied_carriers = ((-26, -25, -24, -23, -22, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 23, 24, 25, 26),)
        self.sync_word_zeros = sync_word_zeros = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.sync_word = sync_word = [0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 1, 1, -1, -1, -1, 1, -1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 1, 1, -1, 1, 1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1, -1, 1, -1, 1, -1, -1, -1, -1, 0, 0, 0, 0, 0]
        self.pilot_symbols = pilot_symbols = ((1, 1, 1, -1,),)
        self.pilot_carriers = pilot_carriers = ((-21, -7, 7, 21,),)
        self.packet_len = packet_len = 80-48
        self.occupied_carriers_len = occupied_carriers_len = len(occupied_carriers[0])
        self.variable_function_probe_0_2 = variable_function_probe_0_2 = 0
        self.variable_function_probe_0_1_0 = variable_function_probe_0_1_0 = 0
        self.variable_function_probe_0_0_1 = variable_function_probe_0_0_1 = 0
        self.variable_function_probe_0_0_0_0 = variable_function_probe_0_0_0_0 = 0
        self.usrp_source_addr = usrp_source_addr = "addr0=192.168.50.2"
        self.usrp_sink_addr = usrp_sink_addr = "addr0=192.168.50.2"
        self.tx_gain = tx_gain = 0
        self.sync_word_zero_len = sync_word_zero_len = len(sync_word_zeros)
        self.sync_word_len = sync_word_len = len(sync_word)
        self.sync_word2 = sync_word2 = [0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 1, 1, -1, -1, -1, 1, -1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 1, 1, -1, 1, 1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1, -1, 1, -1, 1, -1, -1, -1, -1, 0, 0, 0, 0, 0]
        self.sync_word1 = sync_word1 = [0., 0., 0., 0., 0., 0., 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., -1.41421356, 0., -1.41421356, 0., -1.41421356, 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., -1.41421356, 0., -1.41421356, 0., -1.41421356, 0., -1.41421356, 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., 1.41421356, 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., 1.41421356, 0., 1.41421356, 0., -1.41421356, 0., 1.41421356, 0., 1.41421356, 0., 1.41421356, 0., 0., 0., 0., 0., 0.]
        self.symbols_num = symbols_num = (packet_len + 4)*8/2 /occupied_carriers_len + 3
        self.subdev_spec = subdev_spec = "A:0"
        self.sub20_sn = sub20_sn = "552C"
        self.samp_rate = samp_rate = 10e6
        self.qf_1 = qf_1 = 2
        self.qf_0 = qf_0 = 2
        self.premultiply = premultiply = 1
        self.pilot_symbols_len = pilot_symbols_len = len(pilot_symbols[0])
        self.pilot_carriers_len = pilot_carriers_len = len(pilot_carriers[0])
        self.pad_tail_len = pad_tail_len = 1000
        self.pad_front_len = pad_front_len = 20000*2
        self.out_buf_size = out_buf_size = 192000
        self.len_tag_key = len_tag_key = "packet_len"
        self.freq = freq = 915e6
        self.frame_len = frame_len = int(((packet_len+4)/12+3 +2+2+ 2*2)*80)
        self.fft_len = fft_len = 64
        self.delay_tx2rx = delay_tx2rx = 51
        self.dac_2 = dac_2 = 0
        self.dac_1 = dac_1 = 195
        self.dac_0 = dac_0 = 165
        self.cf_1 = cf_1 = 0
        self.cf_0 = cf_0 = 14
        self.cap_2 = cap_2 = 0
        self.cap_1 = cap_1 = 9
        self.cap_0 = cap_0 = 6
        self.canc_path = canc_path = 0
        self.att_2 = att_2 = 0
        self.att_1 = att_1 = 127
        self.att_0 = att_0 = 127

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_source = uhd.usrp_source(
            ",".join((usrp_source_addr, "")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,1)),
            ),
        )
        self.uhd_usrp_source.set_subdev_spec(subdev_spec, 0)
        self.uhd_usrp_source.set_center_freq(freq, 0)
        self.uhd_usrp_source.set_gain(10, 0)
        self.uhd_usrp_source.set_antenna('RX2', 0)
        self.uhd_usrp_source.set_samp_rate(samp_rate)
        self.uhd_usrp_source.set_time_now(uhd.time_spec(time.time()), uhd.ALL_MBOARDS)
        self.uhd_usrp_sink = uhd.usrp_sink(
            ",".join((usrp_sink_addr, "")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,1)),
            ),
            '',
        )
        self.uhd_usrp_sink.set_subdev_spec(subdev_spec, 0)
        self.uhd_usrp_sink.set_center_freq(freq, 0)
        self.uhd_usrp_sink.set_gain(tx_gain, 0)
        self.uhd_usrp_sink.set_antenna('TX/RX', 0)
        self.uhd_usrp_sink.set_samp_rate(samp_rate)
        self.uhd_usrp_sink.set_time_now(uhd.time_spec(time.time()), uhd.ALL_MBOARDS)
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
        self.tab_widget_3 = Qt.QWidget()
        self.tab_layout_3 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab_widget_3)
        self.tab_grid_layout_3 = Qt.QGridLayout()
        self.tab_layout_3.addLayout(self.tab_grid_layout_3)
        self.tab.addTab(self.tab_widget_3, 'SI Channel')
        self.top_grid_layout.addWidget(self.tab, 0, 0, 1, 3)
        for r in range(0, 1):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._delay_tx2rx_range = Range(0, 100, 1, 51, 200)
        self._delay_tx2rx_win = RangeWidget(self._delay_tx2rx_range, self.set_delay_tx2rx, 'delay_tx2rx', "counter_slider", int)
        self.top_grid_layout.addWidget(self._delay_tx2rx_win, 7, 0, 1, 3)
        for r in range(7, 8):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
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

                val = self.uhd_usrp_sink.set_start_time(uhd.time_spec_t(5.0))
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

                val = self.uhd_usrp_source.set_start_time(uhd.time_spec_t(5.0))
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
            1 #number of inputs
        )
        self.qtgui_time_sink_x_1.set_update_time(0.10)
        self.qtgui_time_sink_x_1.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_1.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_1.enable_tags(True)
        self.qtgui_time_sink_x_1.set_trigger_mode(qtgui.TRIG_MODE_TAG, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "packet_len")
        self.qtgui_time_sink_x_1.enable_autoscale(False)
        self.qtgui_time_sink_x_1.enable_grid(False)
        self.qtgui_time_sink_x_1.enable_axis_labels(True)
        self.qtgui_time_sink_x_1.enable_control_panel(True)
        self.qtgui_time_sink_x_1.enable_stem_plot(False)


        labels = ["Digital Canc. Tap - I", "Digital Canc. Tap - Q", '', '', '',
            '', '', '', '', '']
        widths = [2, 2, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ['blue', 'red', 'green', 'black', 'cyan',
            'magenta', 'yellow', 'dark red', 'dark green', 'dark blue']
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]
        styles = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        markers = [0, 0, -1, -1, -1,
            -1, -1, -1, -1, -1]


        for i in range(2):
            if len(labels[i]) == 0:
                if (i % 2 == 0):
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
        self.tab_layout_2.addWidget(self._qtgui_time_sink_x_1_win)
        self.qtgui_time_sink_x_0_0_0 = qtgui.time_sink_c(
            frame_len, #size
            samp_rate, #samp_rate
            'Scope Plot', #name
            2 #number of inputs
        )
        self.qtgui_time_sink_x_0_0_0.set_update_time(0.50)
        self.qtgui_time_sink_x_0_0_0.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_0_0_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0_0_0.enable_tags(True)
        self.qtgui_time_sink_x_0_0_0.set_trigger_mode(qtgui.TRIG_MODE_TAG, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "packet_len")
        self.qtgui_time_sink_x_0_0_0.enable_autoscale(True)
        self.qtgui_time_sink_x_0_0_0.enable_grid(False)
        self.qtgui_time_sink_x_0_0_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0_0_0.enable_control_panel(True)
        self.qtgui_time_sink_x_0_0_0.enable_stem_plot(False)


        labels = ["SI after Circ. + RF Canc. - I", "SI after Circ. + RF Canc. - Q", "SI after Circ. + RF Canc. + Digital Canc. - I", "SI after Circ. + RF Canc. + Digital Canc. - Q", '',
            '', '', '', '', '']
        widths = [2, 2, 2, 2, 1,
            1, 1, 1, 1, 1]
        colors = ['blue', 'red', 'green', 'black', 'cyan',
            'magenta', 'yellow', 'dark red', 'dark green', 'dark blue']
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]
        styles = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1]


        for i in range(4):
            if len(labels[i]) == 0:
                if (i % 2 == 0):
                    self.qtgui_time_sink_x_0_0_0.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_0_0_0.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_0_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_0_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0_0_0.pyqwidget(), Qt.QWidget)
        self.tab_layout_0.addWidget(self._qtgui_time_sink_x_0_0_0_win)
        self.qtgui_freq_sink_x_1 = qtgui.freq_sink_c(
            1024, #size
            firdes.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            samp_rate, #bw
            "", #name
            2
        )
        self.qtgui_freq_sink_x_1.set_update_time(0.50)
        self.qtgui_freq_sink_x_1.set_y_axis(-120, 0)
        self.qtgui_freq_sink_x_1.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_1.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_1.enable_autoscale(False)
        self.qtgui_freq_sink_x_1.enable_grid(False)
        self.qtgui_freq_sink_x_1.set_fft_average(1.0)
        self.qtgui_freq_sink_x_1.enable_axis_labels(True)
        self.qtgui_freq_sink_x_1.enable_control_panel(True)



        labels = ["SI after Circ. + RF Canc.", "SI after Circ. + RF + Digital Canc.", '', '', '',
            '', '', '', '', '']
        widths = [2, 2, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(2):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_1.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_1.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_1.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_1.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_1.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_1_win = sip.wrapinstance(self.qtgui_freq_sink_x_1.pyqwidget(), Qt.QWidget)
        self.tab_layout_1.addWidget(self._qtgui_freq_sink_x_1_win)
        self._qf_1_range = Range(0, 31, 1, 2, 200)
        self._qf_1_win = RangeWidget(self._qf_1_range, self.set_qf_1, 'qf_1', "counter_slider", int)
        self.top_grid_layout.addWidget(self._qf_1_win, 3, 1, 1, 1)
        for r in range(3, 4):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._qf_0_range = Range(0, 31, 1, 2, 200)
        self._qf_0_win = RangeWidget(self._qf_0_range, self.set_qf_0, 'qf_0', "counter_slider", int)
        self.top_grid_layout.addWidget(self._qf_0_win, 3, 0, 1, 1)
        for r in range(3, 4):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.fullduplex_packet_encap2_0 = fullduplex.packet_encap2(False, False, 0.001, pad_front_len, pad_tail_len, 0, premultiply, 2)
        self.fullduplex_packet_encap2_0.set_min_output_buffer(192000)
        self.fullduplex_fde_config_0 = fullduplex.fde_config(0, 0, sub20_sn)
        self.fullduplex_digital_sic_0 = fullduplex.digital_sic(False, delay_tx2rx, pad_front_len, 0, frame_len, 10, 1, premultiply)
        self.fullduplex_digital_sic_0.set_min_output_buffer(192000)
        self.digital_ofdm_tx_0_0 = digital.ofdm_tx(
            fft_len=fft_len,
            cp_len=fft_len//4,
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
            scramble_bits=False)
        self._dac_2_range = Range(0, 255, 1, 0, 200)
        self._dac_2_win = RangeWidget(self._dac_2_range, self.set_dac_2, 'dac_2', "counter_slider", int)
        self.top_grid_layout.addWidget(self._dac_2_win, 5, 2, 1, 1)
        for r in range(5, 6):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(2, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._dac_1_range = Range(0, 255, 1, 195, 200)
        self._dac_1_win = RangeWidget(self._dac_1_range, self.set_dac_1, 'dac_1', "counter_slider", int)
        self.top_grid_layout.addWidget(self._dac_1_win, 5, 1, 1, 1)
        for r in range(5, 6):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._dac_0_range = Range(0, 255, 1, 165, 200)
        self._dac_0_win = RangeWidget(self._dac_0_range, self.set_dac_0, 'dac_0', "counter_slider", int)
        self.top_grid_layout.addWidget(self._dac_0_win, 5, 0, 1, 1)
        for r in range(5, 6):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._cf_1_range = Range(0, 15, 1, 0, 200)
        self._cf_1_win = RangeWidget(self._cf_1_range, self.set_cf_1, 'cf_1', "counter_slider", int)
        self.top_grid_layout.addWidget(self._cf_1_win, 2, 1, 1, 1)
        for r in range(2, 3):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._cf_0_range = Range(0, 15, 1, 14, 200)
        self._cf_0_win = RangeWidget(self._cf_0_range, self.set_cf_0, 'cf_0', "counter_slider", int)
        self.top_grid_layout.addWidget(self._cf_0_win, 2, 0, 1, 1)
        for r in range(2, 3):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._cap_2_range = Range(0, 15, 1, 0, 200)
        self._cap_2_win = RangeWidget(self._cap_2_range, self.set_cap_2, 'cap_2', "counter_slider", int)
        self.top_grid_layout.addWidget(self._cap_2_win, 1, 2, 1, 1)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(2, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._cap_1_range = Range(0, 15, 1, 9, 200)
        self._cap_1_win = RangeWidget(self._cap_1_range, self.set_cap_1, 'cap_1', "counter_slider", int)
        self.top_grid_layout.addWidget(self._cap_1_win, 1, 1, 1, 1)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._cap_0_range = Range(0, 15, 1, 6, 200)
        self._cap_0_win = RangeWidget(self._cap_0_range, self.set_cap_0, 'cap_0', "counter_slider", int)
        self.top_grid_layout.addWidget(self._cap_0_win, 1, 0, 1, 1)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._canc_path_range = Range(0, 1, 1, 0, 200)
        self._canc_path_win = RangeWidget(self._canc_path_range, self.set_canc_path, 'canc_path', "counter_slider", int)
        self.top_grid_layout.addWidget(self._canc_path_win, 6, 0, 1, 3)
        for r in range(6, 7):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.blocks_stream_to_tagged_stream_1 = blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, frame_len + pad_front_len + pad_tail_len, "packet_len")
        self.blocks_stream_to_tagged_stream_1.set_min_output_buffer(192000)
        self.blocks_stream_to_tagged_stream_0 = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, packet_len, "packet_len")
        self.blocks_multiply_const_vxx_2_0 = blocks.multiply_const_cc(19.95262314968879601352)
        self.blocks_multiply_const_vxx_2 = blocks.multiply_const_cc(19.95262314968879601352)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_cc(0.01*2.23 * 0.56 * 1.4 * 1.4 * premultiply)
        self._att_2_range = Range(0, 127, 1, 0, 200)
        self._att_2_win = RangeWidget(self._att_2_range, self.set_att_2, 'att_2', "counter_slider", int)
        self.top_grid_layout.addWidget(self._att_2_win, 4, 2, 1, 1)
        for r in range(4, 5):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(2, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._att_1_range = Range(0, 127, 1, 127, 200)
        self._att_1_win = RangeWidget(self._att_1_range, self.set_att_1, 'att_1', "counter_slider", int)
        self.top_grid_layout.addWidget(self._att_1_win, 4, 1, 1, 1)
        for r in range(4, 5):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._att_0_range = Range(0, 127, 1, 127, 200)
        self._att_0_win = RangeWidget(self._att_0_range, self.set_att_0, 'att_0', "counter_slider", int)
        self.top_grid_layout.addWidget(self._att_0_win, 4, 0, 1, 1)
        for r in range(4, 5):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.analog_random_source_x_0 = blocks.vector_source_b(list(map(int, numpy.random.randint(0, 50, 50))), True)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_random_source_x_0, 0), (self.blocks_stream_to_tagged_stream_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.fullduplex_packet_encap2_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2, 0), (self.qtgui_freq_sink_x_1, 0))
        self.connect((self.blocks_multiply_const_vxx_2_0, 0), (self.qtgui_freq_sink_x_1, 1))
        self.connect((self.blocks_stream_to_tagged_stream_0, 0), (self.digital_ofdm_tx_0_0, 0))
        self.connect((self.blocks_stream_to_tagged_stream_1, 0), (self.fullduplex_digital_sic_0, 1))
        self.connect((self.digital_ofdm_tx_0_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.fullduplex_digital_sic_0, 0), (self.blocks_multiply_const_vxx_2, 0))
        self.connect((self.fullduplex_digital_sic_0, 1), (self.blocks_multiply_const_vxx_2_0, 0))
        self.connect((self.fullduplex_digital_sic_0, 0), (self.qtgui_time_sink_x_0_0_0, 0))
        self.connect((self.fullduplex_digital_sic_0, 1), (self.qtgui_time_sink_x_0_0_0, 1))
        self.connect((self.fullduplex_digital_sic_0, 2), (self.qtgui_time_sink_x_1, 0))
        self.connect((self.fullduplex_packet_encap2_0, 0), (self.fullduplex_digital_sic_0, 0))
        self.connect((self.fullduplex_packet_encap2_0, 0), (self.uhd_usrp_sink, 0))
        self.connect((self.uhd_usrp_source, 0), (self.blocks_stream_to_tagged_stream_1, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "node_level_sic_fd_gui")
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
        self.set_frame_len(int(((self.packet_len+4)/12+3 +2+2+ 2*2)*80))
        self.set_symbols_num((self.packet_len + 4)*8/2 /self.occupied_carriers_len + 3)
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

    def get_tx_gain(self):
        return self.tx_gain

    def set_tx_gain(self, tx_gain):
        self.tx_gain = tx_gain
        self.uhd_usrp_sink.set_gain(self.tx_gain, 0)

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

    def get_subdev_spec(self):
        return self.subdev_spec

    def set_subdev_spec(self, subdev_spec):
        self.subdev_spec = subdev_spec

    def get_sub20_sn(self):
        return self.sub20_sn

    def set_sub20_sn(self, sub20_sn):
        self.sub20_sn = sub20_sn
        self.fullduplex_fde_config_0.fde_config_find_dev(self.sub20_sn)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_freq_sink_x_1.set_frequency_range(0, self.samp_rate)
        self.qtgui_time_sink_x_0_0_0.set_samp_rate(self.samp_rate)
        self.qtgui_time_sink_x_1.set_samp_rate(self.samp_rate)
        self.uhd_usrp_sink.set_samp_rate(self.samp_rate)
        self.uhd_usrp_source.set_samp_rate(self.samp_rate)

    def get_qf_1(self):
        return self.qf_1

    def set_qf_1(self, qf_1):
        self.qf_1 = qf_1
        self.fullduplex_fde_config_0.set_qf_cap(1, self.qf_1)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_qf_0(self):
        return self.qf_0

    def set_qf_0(self, qf_0):
        self.qf_0 = qf_0
        self.fullduplex_fde_config_0.set_qf_cap(0, self.qf_0)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_premultiply(self):
        return self.premultiply

    def set_premultiply(self, premultiply):
        self.premultiply = premultiply
        self.blocks_multiply_const_vxx_0.set_k(0.01*2.23 * 0.56 * 1.4 * 1.4 * self.premultiply)

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
        self.blocks_stream_to_tagged_stream_1.set_packet_len(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1.set_packet_len_pmt(self.frame_len + self.pad_front_len + self.pad_tail_len)

    def get_pad_front_len(self):
        return self.pad_front_len

    def set_pad_front_len(self, pad_front_len):
        self.pad_front_len = pad_front_len
        self.blocks_stream_to_tagged_stream_1.set_packet_len(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.blocks_stream_to_tagged_stream_1.set_packet_len_pmt(self.frame_len + self.pad_front_len + self.pad_tail_len)
        self.fullduplex_digital_sic_0.set_pad_front(self.pad_front_len)

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
        self.uhd_usrp_sink.set_center_freq(self.freq, 0)
        self.uhd_usrp_sink.set_center_freq(self.freq, 1)
        self.uhd_usrp_source.set_center_freq(self.freq, 0)
        self.uhd_usrp_source.set_center_freq(self.freq, 1)

    def get_frame_len(self):
        return self.frame_len

    def set_frame_len(self, frame_len):
        self.frame_len = frame_len
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
        self.fullduplex_digital_sic_0.set_delay_tx2rx(self.delay_tx2rx)

    def get_dac_2(self):
        return self.dac_2

    def set_dac_2(self, dac_2):
        self.dac_2 = dac_2
        self.fullduplex_fde_config_0.set_dac(2, self.dac_2)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_dac_1(self):
        return self.dac_1

    def set_dac_1(self, dac_1):
        self.dac_1 = dac_1
        self.fullduplex_fde_config_0.set_dac(1, self.dac_1)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_dac_0(self):
        return self.dac_0

    def set_dac_0(self, dac_0):
        self.dac_0 = dac_0
        self.fullduplex_fde_config_0.set_dac(0, self.dac_0)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_cf_1(self):
        return self.cf_1

    def set_cf_1(self, cf_1):
        self.cf_1 = cf_1
        self.fullduplex_fde_config_0.set_cf_cap(1, self.cf_1)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_cf_0(self):
        return self.cf_0

    def set_cf_0(self, cf_0):
        self.cf_0 = cf_0
        self.fullduplex_fde_config_0.set_cf_cap(0, self.cf_0)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_cap_2(self):
        return self.cap_2

    def set_cap_2(self, cap_2):
        self.cap_2 = cap_2

    def get_cap_1(self):
        return self.cap_1

    def set_cap_1(self, cap_1):
        self.cap_1 = cap_1

    def get_cap_0(self):
        return self.cap_0

    def set_cap_0(self, cap_0):
        self.cap_0 = cap_0

    def get_canc_path(self):
        return self.canc_path

    def set_canc_path(self, canc_path):
        self.canc_path = canc_path
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_att_2(self):
        return self.att_2

    def set_att_2(self, att_2):
        self.att_2 = att_2
        self.fullduplex_fde_config_0.set_att(2, self.att_2)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_att_1(self):
        return self.att_1

    def set_att_1(self, att_1):
        self.att_1 = att_1
        self.fullduplex_fde_config_0.set_att(1, self.att_1)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)

    def get_att_0(self):
        return self.att_0

    def set_att_0(self, att_0):
        self.att_0 = att_0
        self.fullduplex_fde_config_0.set_att(0, self.att_0)
        self.fullduplex_fde_config_0.set_all(self.att_0, self.att_1, self.att_2, self.cf_0, self.cf_1, self.qf_0, self.qf_1, self.dac_0, self.dac_1, self.dac_2, self.canc_path, 0, 0, 0)





def main(top_block_cls=node_level_sic_fd_gui, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    def quitting():
        tb.stop()
        tb.wait()

    qapp.aboutToQuit.connect(quitting)
    qapp.exec_()

if __name__ == '__main__':
    main()
