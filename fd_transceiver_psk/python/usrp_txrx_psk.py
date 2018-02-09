#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Usrp Txrx Psk
# Generated: Thu Feb  8 17:59:35 2018
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import func_relative_path  # embedded python module
import threading
import time


class usrp_txrx_psk(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Usrp Txrx Psk")

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
        self.freq = freq = 900e6
        self.file_usrp_out = file_usrp_out = "usrp_out"
        self.file_usrp_in = file_usrp_in = "usrp_in"

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

        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, file_usrp_in, True)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_gr_complex*1, file_usrp_out, False)
        self.blocks_file_sink_0.set_unbuffered(False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_0, 0), (self.uhd_usrp_sink_0, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.blocks_file_sink_0, 0))

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

    def get_rx_gain(self):
        return self.rx_gain

    def set_rx_gain(self, rx_gain):
        self.rx_gain = rx_gain
        self.uhd_usrp_source_0.set_gain(self.rx_gain, 0)


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
        self.blocks_file_sink_0.open(self.file_usrp_out)

    def get_file_usrp_in(self):
        return self.file_usrp_in

    def set_file_usrp_in(self, file_usrp_in):
        self.file_usrp_in = file_usrp_in
        self.blocks_file_source_0.open(self.file_usrp_in, True)


def main(top_block_cls=usrp_txrx_psk, options=None):

    tb = top_block_cls()
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
