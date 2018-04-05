"""
Embedded Python Blocks:

Each this file is saved, GRC will instantiate the first class it finds to get
ports and parameters of your block. The arguments to __init__  will be the
parameters. All of them are required to have default values!
"""
import numpy as np
from gnuradio import gr

class print_fd_message(gr.sync_block):
    def __init__(self, display=0):  # only default arguments here
        gr.sync_block.__init__(
            self,
            name='Print FD Message',
            in_sig=[np.uint8],
            out_sig=[]
        )
        self.disp = display
        self.count = 0

    def work(self, input_items, output_items):

        in_size = len(input_items[0])

        if self.disp:
            start = 0
            for i in range(0, in_size):
                if input_items[0][i] == 10:
                    stop = i + 1
                    self.count += 1
                    print self.count, ': ', input_items[0][start:stop].tobytes()
                    start = i + 1

        return len(input_items[0])
