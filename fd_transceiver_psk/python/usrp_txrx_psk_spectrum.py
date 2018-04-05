#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Usrp Txrx Psk Spectrum
# Generated: Thu Apr  5 10:38:03 2018
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
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import func_relative_path  # embedded python module
import sip
import sys
import threading
import time
from gnuradio import qtgui


class usrp_txrx_psk_spectrum(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Usrp Txrx Psk Spectrum")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Usrp Txrx Psk Spectrum")
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

        self.settings = Qt.QSettings("GNU Radio", "usrp_txrx_psk_spectrum")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.variable_function_probe_0_1 = variable_function_probe_0_1 = 0
        self.variable_function_probe_0_0_0 = variable_function_probe_0_0_0 = 0
        self.variable_function_probe_0_0 = variable_function_probe_0_0 = 0
        self.variable_function_probe_0 = variable_function_probe_0 = 0
        self.usrp_addr = usrp_addr = "type=usrp2"
        self.tx_gain = tx_gain = 0
        self.samp_rate = samp_rate = 10e6
        self.rx_gain = rx_gain = 0
        self.phase_code = phase_code = 120
        self.freq = freq = 900e6
        self.file_usrp_out = file_usrp_out = "usrp_out"
        self.file_usrp_in = file_usrp_in = "usrp_in"
        self.att_code = att_code = 32

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join((usrp_addr, "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(samp_rate)
        self.uhd_usrp_source_0.set_center_freq(freq, 0)
        self.uhd_usrp_source_0.set_gain(rx_gain, 0)
        self.uhd_usrp_source_0.set_antenna('RX2', 0)
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
        	",".join((usrp_addr, "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_sink_0.set_samp_rate(samp_rate)
        self.uhd_usrp_sink_0.set_center_freq(freq, 0)
        self.uhd_usrp_sink_0.set_gain(tx_gain, 0)
        self.uhd_usrp_sink_0.set_antenna('TX/RX', 0)

        def _variable_function_probe_0_1_probe():
            while True:
                val = self.uhd_usrp_source_0.set_time_now(uhd.time_spec_t(0.0))
                try:
                    self.set_variable_function_probe_0_1(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (1e-9))
        _variable_function_probe_0_1_thread = threading.Thread(target=_variable_function_probe_0_1_probe)
        _variable_function_probe_0_1_thread.daemon = True
        _variable_function_probe_0_1_thread.start()


        def _variable_function_probe_0_0_0_probe():
            while True:
                val = self.uhd_usrp_source_0.set_start_time(uhd.time_spec_t(0.5))
                try:
                    self.set_variable_function_probe_0_0_0(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (1e-9))
        _variable_function_probe_0_0_0_thread = threading.Thread(target=_variable_function_probe_0_0_0_probe)
        _variable_function_probe_0_0_0_thread.daemon = True
        _variable_function_probe_0_0_0_thread.start()


        def _variable_function_probe_0_0_probe():
            while True:
                val = self.uhd_usrp_sink_0.set_start_time(uhd.time_spec_t(0.5))
                try:
                    self.set_variable_function_probe_0_0(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (1e-9))
        _variable_function_probe_0_0_thread = threading.Thread(target=_variable_function_probe_0_0_probe)
        _variable_function_probe_0_0_thread.daemon = True
        _variable_function_probe_0_0_thread.start()


        def _variable_function_probe_0_probe():
            while True:
                val = self.uhd_usrp_sink_0.set_time_now(uhd.time_spec_t(0.0))
                try:
                    self.set_variable_function_probe_0(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (1e-9))
        _variable_function_probe_0_thread = threading.Thread(target=_variable_function_probe_0_probe)
        _variable_function_probe_0_thread.daemon = True
        _variable_function_probe_0_thread.start()

        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	0, #fc
        	samp_rate, #bw
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(False)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)

        if not True:
          self.qtgui_freq_sink_x_0.disable_legend()

        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_freq_sink_x_0.set_plot_pos_half(not True)

        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_freq_sink_x_0_win)
        self._phase_code_range = Range(0, 255, 1, 120, 200)
        self._phase_code_win = RangeWidget(self._phase_code_range, self.set_phase_code, "phase_code", "counter_slider", float)
        self.top_layout.addWidget(self._phase_code_win)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc((10, ))
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, file_usrp_in, True)
        self._att_code_range = Range(0, 127, 1, 32, 200)
        self._att_code_win = RangeWidget(self._att_code_range, self.set_att_code, "att_code", "counter_slider", float)
        self.top_layout.addWidget(self._att_code_win)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_0, 0), (self.uhd_usrp_sink_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.qtgui_freq_sink_x_0, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.blocks_multiply_const_vxx_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "usrp_txrx_psk_spectrum")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_variable_function_probe_0_1(self):
        return self.variable_function_probe_0_1

    def set_variable_function_probe_0_1(self, variable_function_probe_0_1):
        self.variable_function_probe_0_1 = variable_function_probe_0_1

    def get_variable_function_probe_0_0_0(self):
        return self.variable_function_probe_0_0_0

    def set_variable_function_probe_0_0_0(self, variable_function_probe_0_0_0):
        self.variable_function_probe_0_0_0 = variable_function_probe_0_0_0

    def get_variable_function_probe_0_0(self):
        return self.variable_function_probe_0_0

    def set_variable_function_probe_0_0(self, variable_function_probe_0_0):
        self.variable_function_probe_0_0 = variable_function_probe_0_0

    def get_variable_function_probe_0(self):
        return self.variable_function_probe_0

    def set_variable_function_probe_0(self, variable_function_probe_0):
        self.variable_function_probe_0 = variable_function_probe_0

    def get_usrp_addr(self):
        return self.usrp_addr

    def set_usrp_addr(self, usrp_addr):
        self.usrp_addr = usrp_addr

    def get_tx_gain(self):
        return self.tx_gain

    def set_tx_gain(self, tx_gain):
        self.tx_gain = tx_gain
        self.uhd_usrp_sink_0.set_gain(self.tx_gain, 0)


    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
        self.uhd_usrp_sink_0.set_samp_rate(self.samp_rate)
        self.qtgui_freq_sink_x_0.set_frequency_range(0, self.samp_rate)

    def get_rx_gain(self):
        return self.rx_gain

    def set_rx_gain(self, rx_gain):
        self.rx_gain = rx_gain
        self.uhd_usrp_source_0.set_gain(self.rx_gain, 0)


    def get_phase_code(self):
        return self.phase_code

    def set_phase_code(self, phase_code):
        self.phase_code = phase_code

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.uhd_usrp_source_0.set_center_freq(self.freq, 0)
        self.uhd_usrp_sink_0.set_center_freq(self.freq, 0)

    def get_file_usrp_out(self):
        return self.file_usrp_out

    def set_file_usrp_out(self, file_usrp_out):
        self.file_usrp_out = file_usrp_out

    def get_file_usrp_in(self):
        return self.file_usrp_in

    def set_file_usrp_in(self, file_usrp_in):
        self.file_usrp_in = file_usrp_in
        self.blocks_file_source_0.open(self.file_usrp_in, True)

    def get_att_code(self):
        return self.att_code

    def set_att_code(self, att_code):
        self.att_code = att_code


def main(top_block_cls=usrp_txrx_psk_spectrum, options=None):

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
