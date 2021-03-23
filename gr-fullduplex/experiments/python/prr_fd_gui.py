#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Prr Fd Gui
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

class prr_fd_gui(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Prr Fd Gui")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Prr Fd Gui")
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

        self.settings = Qt.QSettings("GNU Radio", "prr_fd_gui")

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
        self.packet_len = packet_len = 32
        self.occupied_carriers_len = occupied_carriers_len = len(occupied_carriers[0])
        self.variable_function_probe_0_2 = variable_function_probe_0_2 = 0
        self.variable_function_probe_0_1_0 = variable_function_probe_0_1_0 = 0
        self.variable_function_probe_0_0_1 = variable_function_probe_0_0_1 = 0
        self.variable_function_probe_0_0_0_0 = variable_function_probe_0_0_0_0 = 0
        self.usrp_source_addr = usrp_source_addr = "addr0=192.168.50.2"
        self.usrp_sink_addr = usrp_sink_addr = "addr0=192.168.50.2"
        self.tx_gain = tx_gain = 20
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
        self.pad_front_len = pad_front_len = 320000
        self.out_buf_size = out_buf_size = 1000000
        self.len_tag_key = len_tag_key = "packet_len"
        self.freq = freq = 915e6
        self.frame_len = frame_len = int(((packet_len+4)/12+3+2 + 2 + 2*2)*80  + 0*80)
        self.fft_len = fft_len = 64
        self.delay_tx2rx = delay_tx2rx = 58
        self.bb_gain = bb_gain = -21.5

        ##################################################
        # Blocks
        ##################################################
        self._tx_gain_range = Range(0, 25, 1, 20, 200)
        self._tx_gain_win = RangeWidget(self._tx_gain_range, self.set_tx_gain, 'Tx Gain', "counter_slider", int)
        self.top_grid_layout.addWidget(self._tx_gain_win, 5, 0, 1, 6)
        for r in range(5, 6):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 6):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.uhd_usrp_source = uhd.usrp_source(
            ",".join((usrp_source_addr, "")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,2)),
            ),
        )
        self.uhd_usrp_source.set_center_freq(freq, 0)
        self.uhd_usrp_source.set_gain(10, 0)
        self.uhd_usrp_source.set_antenna('RX2', 0)
        self.uhd_usrp_source.set_center_freq(freq, 1)
        self.uhd_usrp_source.set_gain(10, 1)
        self.uhd_usrp_source.set_antenna('RX2', 1)
        self.uhd_usrp_source.set_samp_rate(samp_rate)
        self.uhd_usrp_source.set_time_unknown_pps(uhd.time_spec())
        self.uhd_usrp_sink = uhd.usrp_sink(
            ",".join((usrp_sink_addr, "")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,2)),
            ),
            '',
        )
        self.uhd_usrp_sink.set_center_freq(freq, 0)
        self.uhd_usrp_sink.set_gain(tx_gain, 0)
        self.uhd_usrp_sink.set_antenna('TX/RX', 0)
        self.uhd_usrp_sink.set_center_freq(freq, 1)
        self.uhd_usrp_sink.set_gain(tx_gain, 1)
        self.uhd_usrp_sink.set_antenna('TX/RX', 1)
        self.uhd_usrp_sink.set_samp_rate(samp_rate)
        self.uhd_usrp_sink.set_time_unknown_pps(uhd.time_spec())
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

        self.qtgui_time_sink_x_0 = qtgui.time_sink_c(
            frame_len, #size
            samp_rate, #samp_rate
            "", #name
            2 #number of inputs
        )
        self.qtgui_time_sink_x_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0.enable_tags(True)
        self.qtgui_time_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0.enable_autoscale(True)
        self.qtgui_time_sink_x_0.enable_grid(False)
        self.qtgui_time_sink_x_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0.enable_control_panel(False)
        self.qtgui_time_sink_x_0.enable_stem_plot(False)


        labels = ["After RF SIC, I", "After RF SIC, Q", "After Digital SIC, I", "After Digital SIC, Q", '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
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
                    self.qtgui_time_sink_x_0.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_0.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_time_sink_x_0_win, 0, 0, 3, 6)
        for r in range(0, 3):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 6):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_number_sink_0_0_0_0_0 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_HORIZ,
            1
        )
        self.qtgui_number_sink_0_0_0_0_0.set_update_time(0.10)
        self.qtgui_number_sink_0_0_0_0_0.set_title("Measured PRR")

        labels = ["Packet Reception  Ratio", '', '', '', '',
            '', '', '', '', '']
        units = ['', '', '', '', '',
            '', '', '', '', '']
        colors = [("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
            ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]

        for i in range(1):
            self.qtgui_number_sink_0_0_0_0_0.set_min(i, 0)
            self.qtgui_number_sink_0_0_0_0_0.set_max(i, 1)
            self.qtgui_number_sink_0_0_0_0_0.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0_0_0_0_0.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0_0_0_0_0.set_label(i, labels[i])
            self.qtgui_number_sink_0_0_0_0_0.set_unit(i, units[i])
            self.qtgui_number_sink_0_0_0_0_0.set_factor(i, factor[i])

        self.qtgui_number_sink_0_0_0_0_0.enable_autoscale(False)
        self._qtgui_number_sink_0_0_0_0_0_win = sip.wrapinstance(self.qtgui_number_sink_0_0_0_0_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_number_sink_0_0_0_0_0_win, 4, 0, 1, 6)
        for r in range(4, 5):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 6):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_number_sink_0 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_HORIZ,
            1
        )
        self.qtgui_number_sink_0.set_update_time(0.10)
        self.qtgui_number_sink_0.set_title("Measured SNR")

        labels = ["Signal-to-Noise Ratio", '', '', '', '',
            '', '', '', '', '']
        units = ["dB", '', '', '', '',
            '', '', '', '', '']
        colors = [("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
            ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]

        for i in range(1):
            self.qtgui_number_sink_0.set_min(i, 0)
            self.qtgui_number_sink_0.set_max(i, 35)
            self.qtgui_number_sink_0.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0.set_label(i, labels[i])
            self.qtgui_number_sink_0.set_unit(i, units[i])
            self.qtgui_number_sink_0.set_factor(i, factor[i])

        self.qtgui_number_sink_0.enable_autoscale(False)
        self._qtgui_number_sink_0_win = sip.wrapinstance(self.qtgui_number_sink_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_number_sink_0_win, 3, 0, 1, 6)
        for r in range(3, 4):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 6):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_const_sink_x_0_0 = qtgui.const_sink_c(
            3*48, #size
            "Constellation after RF SIC", #name
            1 #number of inputs
        )
        self.qtgui_const_sink_x_0_0.set_update_time(0.10)
        self.qtgui_const_sink_x_0_0.set_y_axis(-2, 2)
        self.qtgui_const_sink_x_0_0.set_x_axis(-2, 2)
        self.qtgui_const_sink_x_0_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, "")
        self.qtgui_const_sink_x_0_0.enable_autoscale(False)
        self.qtgui_const_sink_x_0_0.enable_grid(False)
        self.qtgui_const_sink_x_0_0.enable_axis_labels(True)


        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "red", "red", "red",
            "red", "red", "red", "red", "red"]
        styles = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        markers = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_const_sink_x_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_const_sink_x_0_0.set_line_label(i, labels[i])
            self.qtgui_const_sink_x_0_0.set_line_width(i, widths[i])
            self.qtgui_const_sink_x_0_0.set_line_color(i, colors[i])
            self.qtgui_const_sink_x_0_0.set_line_style(i, styles[i])
            self.qtgui_const_sink_x_0_0.set_line_marker(i, markers[i])
            self.qtgui_const_sink_x_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_const_sink_x_0_0_win = sip.wrapinstance(self.qtgui_const_sink_x_0_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_const_sink_x_0_0_win, 0, 7, 3, 3)
        for r in range(0, 3):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(7, 10):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_const_sink_x_0 = qtgui.const_sink_c(
            3*48, #size
            "Constellation after Digital SIC", #name
            1 #number of inputs
        )
        self.qtgui_const_sink_x_0.set_update_time(0.10)
        self.qtgui_const_sink_x_0.set_y_axis(-2, 2)
        self.qtgui_const_sink_x_0.set_x_axis(-2, 2)
        self.qtgui_const_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, "")
        self.qtgui_const_sink_x_0.enable_autoscale(False)
        self.qtgui_const_sink_x_0.enable_grid(False)
        self.qtgui_const_sink_x_0.enable_axis_labels(True)


        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "red", "red", "red",
            "red", "red", "red", "red", "red"]
        styles = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        markers = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_const_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_const_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_const_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_const_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_const_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_const_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_const_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_const_sink_x_0_win = sip.wrapinstance(self.qtgui_const_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_const_sink_x_0_win, 3, 7, 3, 3)
        for r in range(3, 6):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(7, 10):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.fullduplex_snr_calc_0 = fullduplex.snr_calc(False, 0, -20, 6, 0, 140)
        self.fullduplex_packet_encap2_0_0 = fullduplex.packet_encap2(False, False, 0.001, pad_front_len, pad_tail_len, 1, 0.2, 2)
        self.fullduplex_packet_encap2_0_0.set_min_output_buffer(1000000)
        self.fullduplex_packet_encap2_0 = fullduplex.packet_encap2(False, False, 0.001, pad_front_len, pad_tail_len, 0, premultiply, 2)
        self.fullduplex_packet_encap2_0.set_min_output_buffer(1000000)
        self.fullduplex_ofdm_constellation_0_0 = fullduplex.ofdm_constellation(False, 8, -20, frame_len)
        self.fullduplex_ofdm_constellation_0_0.set_min_output_buffer(1000000)
        self.fullduplex_ofdm_constellation_0 = fullduplex.ofdm_constellation(False, 0, -20, frame_len)
        self.fullduplex_ofdm_constellation_0.set_min_output_buffer(1000000)
        self.fullduplex_digital_sic_0_0 = fullduplex.digital_sic(False, delay_tx2rx, pad_front_len, 0, frame_len, 10, 1, premultiply)
        self.fullduplex_digital_sic_0_0.set_min_output_buffer(1000000)
        self.fullduplex_count_packets_0_0 = fullduplex.count_packets(False)
        self.fullduplex_count_packets_0 = fullduplex.count_packets(False)
        self.fullduplex_async_divide_0 = fullduplex.async_divide()
        self.digital_ofdm_tx_0_0_0 = digital.ofdm_tx(
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
        self.digital_ofdm_rx_0_0_0 = digital.ofdm_rx(
            fft_len=fft_len, cp_len=fft_len//4,
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
            scramble_bits=False)
        self.digital_ofdm_rx_0_0_0.set_min_output_buffer(1000000)
        self.blocks_stream_to_tagged_stream_1 = blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, frame_len + pad_front_len + pad_tail_len, "packet_len")
        self.blocks_stream_to_tagged_stream_1.set_min_output_buffer(1000000)
        self.blocks_stream_to_tagged_stream_0_0_0 = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, packet_len, len_tag_key)
        self.blocks_stream_to_tagged_stream_0_0 = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, packet_len, len_tag_key)
        self.blocks_stream_to_tagged_stream_0 = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, packet_len, len_tag_key)
        self.blocks_null_sink_3_1_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_3_1 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_1 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_cc(0.01*2.23 * 0.56 * 1.4 * 1.4 * premultiply * 10**(bb_gain/20.0))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_cc(0.01*2.23 * 0.56 * 1.4 * 1.4 * premultiply * 10 ** (-5.0/20))
        self.blocks_float_to_uchar_0 = blocks.float_to_uchar()
        self.blocks_complex_to_arg_0 = blocks.complex_to_arg(1)
        self.analog_random_source_x_0_0 = blocks.vector_source_b(list(map(int, numpy.random.randint(51, 100, 50))), True)
        self.analog_random_source_x_0 = blocks.vector_source_b(list(map(int, numpy.random.randint(0, 50, 50))), True)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.fullduplex_count_packets_0, 'out'), (self.fullduplex_async_divide_0, 'in1'))
        self.msg_connect((self.fullduplex_count_packets_0_0, 'out'), (self.fullduplex_async_divide_0, 'in2'))
        self.connect((self.analog_random_source_x_0, 0), (self.blocks_stream_to_tagged_stream_0, 0))
        self.connect((self.analog_random_source_x_0_0, 0), (self.blocks_stream_to_tagged_stream_0_0, 0))
        self.connect((self.blocks_complex_to_arg_0, 0), (self.blocks_float_to_uchar_0, 0))
        self.connect((self.blocks_float_to_uchar_0, 0), (self.fullduplex_count_packets_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.fullduplex_packet_encap2_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.fullduplex_packet_encap2_0_0, 0))
        self.connect((self.blocks_stream_to_tagged_stream_0, 0), (self.digital_ofdm_tx_0_0, 0))
        self.connect((self.blocks_stream_to_tagged_stream_0_0, 0), (self.digital_ofdm_tx_0_0_0, 0))
        self.connect((self.blocks_stream_to_tagged_stream_0_0_0, 0), (self.fullduplex_count_packets_0, 0))
        self.connect((self.blocks_stream_to_tagged_stream_1, 0), (self.fullduplex_digital_sic_0_0, 1))
        self.connect((self.digital_ofdm_rx_0_0_0, 0), (self.blocks_stream_to_tagged_stream_0_0_0, 0))
        self.connect((self.digital_ofdm_tx_0_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.digital_ofdm_tx_0_0_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.fullduplex_async_divide_0, 0), (self.qtgui_number_sink_0_0_0_0_0, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 1), (self.blocks_complex_to_arg_0, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 2), (self.blocks_null_sink_3_1_0, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 1), (self.digital_ofdm_rx_0_0_0, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 1), (self.fullduplex_ofdm_constellation_0, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 0), (self.fullduplex_ofdm_constellation_0_0, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 1), (self.fullduplex_snr_calc_0, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 0), (self.qtgui_time_sink_x_0, 0))
        self.connect((self.fullduplex_digital_sic_0_0, 1), (self.qtgui_time_sink_x_0, 1))
        self.connect((self.fullduplex_ofdm_constellation_0, 1), (self.blocks_null_sink_3_1, 1))
        self.connect((self.fullduplex_ofdm_constellation_0, 0), (self.qtgui_const_sink_x_0, 0))
        self.connect((self.fullduplex_ofdm_constellation_0_0, 1), (self.blocks_null_sink_3_1, 0))
        self.connect((self.fullduplex_ofdm_constellation_0_0, 0), (self.qtgui_const_sink_x_0_0, 0))
        self.connect((self.fullduplex_packet_encap2_0, 0), (self.fullduplex_digital_sic_0_0, 0))
        self.connect((self.fullduplex_packet_encap2_0, 0), (self.uhd_usrp_sink, 0))
        self.connect((self.fullduplex_packet_encap2_0_0, 0), (self.uhd_usrp_sink, 1))
        self.connect((self.fullduplex_snr_calc_0, 0), (self.qtgui_number_sink_0, 0))
        self.connect((self.uhd_usrp_source, 1), (self.blocks_null_sink_1, 0))
        self.connect((self.uhd_usrp_source, 0), (self.blocks_stream_to_tagged_stream_1, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "prr_fd_gui")
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
        self.set_frame_len(int(((self.packet_len+4)/12+3+2 + 2 + 2*2)*80  + 0*80))
        self.set_symbols_num((self.packet_len + 4)*8/2 /self.occupied_carriers_len + 3)
        self.blocks_stream_to_tagged_stream_0.set_packet_len(self.packet_len)
        self.blocks_stream_to_tagged_stream_0.set_packet_len_pmt(self.packet_len)
        self.blocks_stream_to_tagged_stream_0_0.set_packet_len(self.packet_len)
        self.blocks_stream_to_tagged_stream_0_0.set_packet_len_pmt(self.packet_len)
        self.blocks_stream_to_tagged_stream_0_0_0.set_packet_len(self.packet_len)
        self.blocks_stream_to_tagged_stream_0_0_0.set_packet_len_pmt(self.packet_len)

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
        self.uhd_usrp_sink.set_gain(self.tx_gain, 1)

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
        self.qtgui_time_sink_x_0.set_samp_rate(self.samp_rate)
        self.uhd_usrp_sink.set_samp_rate(self.samp_rate)
        self.uhd_usrp_source.set_samp_rate(self.samp_rate)

    def get_premultiply(self):
        return self.premultiply

    def set_premultiply(self, premultiply):
        self.premultiply = premultiply
        self.blocks_multiply_const_vxx_0.set_k(0.01*2.23 * 0.56 * 1.4 * 1.4 * self.premultiply * 10 ** (-5.0/20))
        self.blocks_multiply_const_vxx_0_0.set_k(0.01*2.23 * 0.56 * 1.4 * 1.4 * self.premultiply * 10**(self.bb_gain/20.0))

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
        self.fullduplex_digital_sic_0_0.set_pad_front(self.pad_front_len)

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
        self.fullduplex_digital_sic_0_0.set_delay_tx2rx(self.delay_tx2rx)

    def get_bb_gain(self):
        return self.bb_gain

    def set_bb_gain(self, bb_gain):
        self.bb_gain = bb_gain
        self.blocks_multiply_const_vxx_0_0.set_k(0.01*2.23 * 0.56 * 1.4 * 1.4 * self.premultiply * 10**(self.bb_gain/20.0))





def main(top_block_cls=prr_fd_gui, options=None):

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
