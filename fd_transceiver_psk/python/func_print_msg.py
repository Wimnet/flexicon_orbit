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

        if not(self.disp):
            return len(input_items[0])

        in_size = len(input_items[0])
        found = 0
        i = 0
        while i < in_size and input_items[0][i] != 10:
            i += 1
        if i < in_size:
            found = 1
            i += 1

        j = i + 1
        while j < in_size and input_items[0][j] != 10:
            j += 1
        if j < in_size:
            found = 2
            j += 1

        if found == 2:
            self.count += 1
            print self.count, ': ', input_items[0][i:j].tobytes()

        return len(input_items[0])
