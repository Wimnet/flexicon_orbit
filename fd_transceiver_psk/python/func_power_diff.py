"""
Embedded Python Blocks:

Each this file is saved, GRC will instantiate the first class it finds to get
ports and parameters of your block. The arguments to __init__  will be the
parameters. All of them are required to have default values!
"""
import numpy as np
from gnuradio import gr


class power_ratio(gr.sync_block):
    def __init__(self, sample_rate=1e6, samples_per_symbole=8, fft_size=0.5e6, display=1, display_rate=1):  # only default arguments here
        gr.sync_block.__init__(
            self,
            name='Power Ratio',
            in_sig=[np.complex64, np.complex64],
            out_sig=[]

        )
        # self.fft_size = int(sample_rate)
        self.fft_size = int(fft_size)
        self.fft0_buffer = []
        self.fft1_buffer = []
        self.bw = int(sample_rate / samples_per_symbole)
        self.half = int(self.bw / 2)
        self.display = display
        self.disp_r = display_rate

        self.sum_fft0 = 0
        self.sum_fft1 = 0
        self.times = 0

    def work(self, input_items, output_items):

        self.fft0_buffer = np.append(self.fft0_buffer, input_items[0])
        self.fft1_buffer = np.append(self.fft1_buffer, input_items[1])
        # print len(self.fft_buffer), len(input_items[0]), self.fft_size

        if len(self.fft0_buffer) >= self.fft_size and len(self.fft1_buffer) >= self.fft_size:
            fft0 = np.fft.fft(self.fft0_buffer[:self.fft_size]) / self.fft_size
            fft1 = np.fft.fft(self.fft1_buffer[:self.fft_size]) / self.fft_size
            self.fft0_buffer = self.fft0_buffer[self.fft_size:]
            self.fft1_buffer = self.fft1_buffer[self.fft_size:]

            fft0_avg = (np.sum(np.absolute(fft0[:self.half])) + np.sum(np.absolute(fft0[-self.half:]))) / self.bw
            fft1_avg = (np.sum(np.absolute(fft1[:self.half])) + np.sum(np.absolute(fft1[-self.half:]))) / self.bw

            if fft0_avg == 0 or fft1_avg == 0:
                return len(input_items[0])

            self.times += 1

            fft0_avg_db = 20 * np.log10(fft0_avg)
            fft1_avg_db = 20 * np.log10(fft1_avg)

            self.sum_fft0 += fft0_avg_db
            self.sum_fft1 += fft1_avg_db

            if self.times % self.disp_r == 0 and self.display:
                avg0 = self.sum_fft0 / self.times
                avg1 = self.sum_fft1 / self.times
                ratio = avg0 - avg1
                print 'Power Ratio: %0.2f [dB]' % ratio

        return len(input_items[0])
