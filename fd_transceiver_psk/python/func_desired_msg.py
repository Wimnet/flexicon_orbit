"""
Embedded Python Blocks:

Each this file is saved, GRC will instantiate the first class it finds to get
ports and parameters of your block. The arguments to __init__  will be the
parameters. All of them are required to have default values!
"""
import numpy as np
from gnuradio import gr


class rx_fd_message(gr.sync_block):
    def __init__(self, message="rx_message"):  # only default arguments here
        gr.sync_block.__init__(
            self,
            name='FD Message',
            in_sig=[],
            out_sig=[np.uint8]
        )

        self.message = message
        self.message_size = len(self.message + ' (000000000)\n')

        self.count = 1
        self.message_count = 0

    def work(self, input_items, output_items):

        out_size = len(output_items[0])
        zeros_num = out_size % self.message_size
        char_num = out_size - zeros_num
        messages_num = char_num / self.message_size

        for i in range(0, messages_num):
            zeros = '0' * (9 - len(str(self.count)))
            m = self.message + ' (' + zeros + str(self.count) + ')\n'
            output_items[0][self.message_size * i:self.message_size * (i + 1)] = bytearray(m)
            self.count += 1

        output_items[0][self.message_size * (i + 1):] = np.zeros(zeros_num)

        return len(output_items[0])
