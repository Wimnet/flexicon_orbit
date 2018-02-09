#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Dig Sic On Psk
# Generated: Thu Feb  8 18:04:02 2018
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
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from grc_gnuradio import blks2 as grc_blks2
from optparse import OptionParser
import func_desired_msg
import func_dig_sic
import func_power_diff
import func_print_msg
import func_relative_path  # embedded python module
import sip
import sys
from gnuradio import qtgui


class dig_sic_on_psk(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Dig Sic On Psk")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Dig Sic On Psk")
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

        self.settings = Qt.QSettings("GNU Radio", "dig_sic_on_psk")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.rx_message = rx_message = "This is a message from the remote radio"
        self.usrp_delay = usrp_delay = 29
        self.sps = sps = 4
        self.samp_rate = samp_rate = 10e6
        self.rx_pay_len = rx_pay_len = len(rx_message) + len(' (000000000)\n')
        self.rem_signal = rem_signal = -55
        self.pream_size = pream_size = 1e3
        self.power_ratio_display = power_ratio_display = 0
        self.k = k = 50
        self.frames = frames = 1
        self.frame_size = frame_size = 20e3
        self.file_usrp_out = file_usrp_out = "usrp_out"
        self.file_usrp_in = file_usrp_in = "usrp_in"
        self.fd_message_display = fd_message_display = 1
        self.constellation = constellation = 4
        self.access_code = access_code = "0011110011100011000001111111"

        ##################################################
        # Blocks
        ##################################################
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	900e6, #fc
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

        labels = ['SIC In TX', 'SIC Out', '', '', '',
                  '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
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
        self.top_layout.addWidget(self._qtgui_freq_sink_x_0_win)
        self.func_print_msg = func_print_msg.print_fd_message(display=fd_message_display)
        self.func_power_diff = func_power_diff.power_ratio(sample_rate=samp_rate, samples_per_symbole=sps, fft_size=0.5e6, display=power_ratio_display, display_rate=1)
        self.func_dig_sic = func_dig_sic.sic(frame_size=frame_size, preamble_size=pream_size, k=k, frames=frames, error_report=1, warning_report=0, information_report=0)
        self.func_desired_msg = func_desired_msg.rx_fd_message(message=rx_message)
        self.digital_psk_mod_0_0 = digital.psk.psk_mod(
          constellation_points=4,
          mod_code="gray",
          differential=True,
          samples_per_symbol=sps,
          excess_bw=0.35,
          verbose=False,
          log=False,
          )
        self.digital_psk_demod_0 = digital.psk.psk_demod(
          constellation_points=constellation,
          differential=True,
          samples_per_symbol=sps,
          excess_bw=0.35,
          phase_bw=6.28/100.0,
          timing_bw=6.28/100.0,
          mod_code="gray",
          verbose=False,
          log=False,
          )
        self.blocks_multiply_const_vxx_1 = blocks.multiply_const_vcc((10 ** ((rem_signal + 35 )* 50e-3), ))
        self.blocks_file_source_0_0 = blocks.file_source(gr.sizeof_gr_complex*1, file_usrp_out, False)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, file_usrp_in, True)
        self.blocks_delay_0_0_0_0_0_0 = blocks.delay(gr.sizeof_gr_complex*1, usrp_delay)
        self.blocks_delay_0 = blocks.delay(gr.sizeof_gr_complex*1, 0)
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.blks2_packet_encoder_0_0_0 = grc_blks2.packet_mod_b(grc_blks2.packet_encoder(
        		samples_per_symbol=1,
        		bits_per_symbol=1,
        		preamble='',
        		access_code=access_code,
        		pad_for_usrp=False,
        	),
        	payload_length=rx_pay_len,
        )
        self.blks2_packet_decoder_0 = grc_blks2.packet_demod_b(grc_blks2.packet_decoder(
        		access_code=access_code,
        		threshold=0,
        		callback=lambda ok, payload: self.blks2_packet_decoder_0.recv_pkt(ok, payload),
        	),
        )

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blks2_packet_decoder_0, 0), (self.func_print_msg, 0))
        self.connect((self.blks2_packet_encoder_0_0_0, 0), (self.digital_psk_mod_0_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.func_dig_sic, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.func_power_diff, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.qtgui_freq_sink_x_0, 0))
        self.connect((self.blocks_delay_0, 0), (self.blocks_multiply_const_vxx_1, 0))
        self.connect((self.blocks_delay_0_0_0_0_0_0, 0), (self.func_dig_sic, 1))
        self.connect((self.blocks_file_source_0, 0), (self.blocks_delay_0_0_0_0_0_0, 0))
        self.connect((self.blocks_file_source_0_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_multiply_const_vxx_1, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.digital_psk_demod_0, 0), (self.blks2_packet_decoder_0, 0))
        self.connect((self.digital_psk_mod_0_0, 0), (self.blocks_delay_0, 0))
        self.connect((self.func_desired_msg, 0), (self.blks2_packet_encoder_0_0_0, 0))
        self.connect((self.func_dig_sic, 0), (self.digital_psk_demod_0, 0))
        self.connect((self.func_dig_sic, 0), (self.func_power_diff, 1))
        self.connect((self.func_dig_sic, 0), (self.qtgui_freq_sink_x_0, 1))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "dig_sic_on_psk")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_rx_message(self):
        return self.rx_message

    def set_rx_message(self, rx_message):
        self.rx_message = rx_message
        self.set_rx_pay_len(len(self.rx_message) + len(' (000000000)\n') )
        self.func_desired_msg.message = self.rx_message

    def get_usrp_delay(self):
        return self.usrp_delay

    def set_usrp_delay(self, usrp_delay):
        self.usrp_delay = usrp_delay
        self.blocks_delay_0_0_0_0_0_0.set_dly(self.usrp_delay)

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_freq_sink_x_0.set_frequency_range(900e6, self.samp_rate)

    def get_rx_pay_len(self):
        return self.rx_pay_len

    def set_rx_pay_len(self, rx_pay_len):
        self.rx_pay_len = rx_pay_len

    def get_rem_signal(self):
        return self.rem_signal

    def set_rem_signal(self, rem_signal):
        self.rem_signal = rem_signal
        self.blocks_multiply_const_vxx_1.set_k((10 ** ((self.rem_signal + 35 )* 50e-3), ))

    def get_pream_size(self):
        return self.pream_size

    def set_pream_size(self, pream_size):
        self.pream_size = pream_size

    def get_power_ratio_display(self):
        return self.power_ratio_display

    def set_power_ratio_display(self, power_ratio_display):
        self.power_ratio_display = power_ratio_display
        self.func_power_diff.display = self.power_ratio_display

    def get_k(self):
        return self.k

    def set_k(self, k):
        self.k = k
        self.func_dig_sic.k = self.k

    def get_frames(self):
        return self.frames

    def set_frames(self, frames):
        self.frames = frames

    def get_frame_size(self):
        return self.frame_size

    def set_frame_size(self, frame_size):
        self.frame_size = frame_size

    def get_file_usrp_out(self):
        return self.file_usrp_out

    def set_file_usrp_out(self, file_usrp_out):
        self.file_usrp_out = file_usrp_out
        self.blocks_file_source_0_0.open(self.file_usrp_out, False)

    def get_file_usrp_in(self):
        return self.file_usrp_in

    def set_file_usrp_in(self, file_usrp_in):
        self.file_usrp_in = file_usrp_in
        self.blocks_file_source_0.open(self.file_usrp_in, True)

    def get_fd_message_display(self):
        return self.fd_message_display

    def set_fd_message_display(self, fd_message_display):
        self.fd_message_display = fd_message_display

    def get_constellation(self):
        return self.constellation

    def set_constellation(self, constellation):
        self.constellation = constellation

    def get_access_code(self):
        return self.access_code

    def set_access_code(self, access_code):
        self.access_code = access_code


def main(top_block_cls=dig_sic_on_psk, options=None):

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
