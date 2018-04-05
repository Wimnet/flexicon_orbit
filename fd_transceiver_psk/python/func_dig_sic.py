"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
from gnuradio import gr
import time


class sic(gr.sync_block):  # other base classes are basic_block, decim_block, interp_block

    def __init__(self, frame_size=5000, preamble_size=600, k=30, order=3, frames=500, error_report=1, warning_report=0, information_report=0):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""
        gr.sync_block.__init__(
            self,
            name='Digital SIC',   # will show up in GRC
            in_sig=[np.complex64, np.complex64],
            out_sig=[np.complex64]
        )
        # if an attribute with the same name as a parameter is found,
        # a callback is registered (properties work, too).

        if frame_size <= preamble_size:
            print('Error: Frame must be > Preamble !')
            exit(1)

        if preamble_size <= 2 * k:
            print('Error: Preamble must be > 2k !')
            exit(1)
        if order < 1:
            order = 1

        self.fs = int(frame_size)
        self.p = int(preamble_size)
        self.k = int(k)
        self.order = int(order)
        self.er = bool(error_report)
        self.wr = bool(warning_report)
        self.ir = bool(information_report)

        self.in0_buff = []
        self.in1_buff = []
        self.out_buff = []
        self.out_num = 0
        self.first = 1
        self.h = np.zeros(2 * self.k)
        self.f = int(frames)
        self.fc = 0
        self.h_time = 0
        self.data_time = 0

    def work(self, input_items, output_items):

        t = time.time()

        self.in0_buff = np.append(self.in0_buff, input_items[0])
        self.in1_buff = np.append(self.in1_buff, input_items[1])
        in_size = min(len(self.in0_buff), len(self.in1_buff))

        if in_size >= self.fs + 2 * self.k and not(self.first):  # not the first frame
            if self.fc % self.f == 0:
                # find h
                h_time = time.time()
                self.h = find_h_np(self.k, self.order, self.p, self.in0_buff[: self.k + self.fs], self.in1_buff[: self.k + self.fs], self.wr)
                h_time = time.time() - h_time
                if h_time > self.h_time and self.ir:
                    self.h_time = h_time
                    print('h time', self.h_time)
            # calc the output
            data_time = time.time()
            A = build_A(self.in1_buff[: self.fs + 2 * self.k], self.k, self.order)
            self.out_buff = np.append(self.out_buff, self.in0_buff[self.k: self.k + self.fs] - np.dot(A, self.h))
            data_time = time.time() - data_time
            if data_time > self.data_time and self.ir:
                self.data_time = data_time
                print('data time', self.data_time)
            self.out_num += self.fs
            self.in0_buff = self.in0_buff[self.fs:]  # keep last 2k samples for next time
            self.in1_buff = self.in1_buff[self.fs:]  # keep last 2k samples for next time
            self.fc += 1

        elif in_size >= self.fs + self.k and self.first:  # the first frame
            if self.fc % self.f == 0:
                # find h
                h_time = time.time()
                self.h = find_h_np(self.k, self.order, self.p, self.in0_buff[: self.fs], self.in1_buff[: self.fs], self.wr)
                h_time = time.time() - h_time
                if h_time > self.h_time and self.ir:
                    self.h_time = h_time
                    print('h time', self.h_time)
            # calc the output
            data_time = time.time()
            A = build_A(self.in1_buff[: self.fs + self.k], self.k, self.order)
            data_time = time.time() - data_time
            if data_time > self.data_time and self.ir:
                self.data_time = data_time
                print('data time', self.data_time)
            # self.out_buff = np.append(self.out_buff, np.zeros(self.k))
            self.out_buff = np.append(self.out_buff, self.in0_buff[:self.k])
            self.out_buff = np.append(self.out_buff, self.in0_buff[self.k: self.fs] - np.dot(A, self.h))
            self.out_num += self.fs
            self.in0_buff = self.in0_buff[self.fs - self.k:]  # keep last 2k samples for next time
            self.in1_buff = self.in1_buff[self.fs - self.k:]  # keep last 2k samples for next time
            self.fc += 1
            self.first = 0

        # generate output...
        out_len = len(output_items[0])
        if self.out_num >= out_len:
            # print('full'
            output_items[0][:] = self.out_buff[:out_len]
            self.out_buff = self.out_buff[out_len:]
            self.out_num -= out_len
        # elif self.out_num > 0:
        #     # print('some empty'
        #     output_items[0][:self.out_num] = self.out_buff[:self.out_num]
        #     self.out_buff = self.out_buff[self.out_num:]
        #     out_len = self.out_num
        #     self.out_num = 0
        else:  # self.out_num == 0
            # print('empty'
            output_items[0] = np.zeros(out_len)
            # out_len = 0

        if not(self.fc % 19) and self.ir:
            print('SIC time = ', (time.time() - t))

        return out_len


def find_h_np(k, order, p, y, x, wr=0):
    # build matrix A from x (the transmitted signal)
    A = build_A(x[: p], k, order)
    # build weights vector h from matrix A and y (the received signal)
    # the beginning of the first input stream is (k + 1) the end is (k + 1 + 2k)
    h = np.linalg.lstsq(A, y[k: p - k])[0]
    if np.linalg.LinAlgError and wr:
        print('Warn: Least Squares Alg did not converge!')

    return h


def build_A(x, k, order):
    n = len(x) - 2 * k
    A = np.zeros((n, 2 * k * order), dtype=np.complex64)

    # A[0,0]...A[0,2k-1] = x[0].....x[2k-1]
    # A[1,0]...A[1,2k-1] = x[1].....x[1 + 2k-1]
    # ......................................
    # A[n-1,0]...A[1,2k] = x[n-1]...x[n-1 + 2k-1]
    # repeat for 2nd order
    # repeat for 3rd order
    # ...
    for i in range(0, n):
        for j in range(0, order):
            A[i, 2 * k * j: 2 * k * (j + 1)] = np.power(x[i: i + 2 * k], j + 1)
    return A


# def build_A(x, k, order):
#     n = len(x) - 2 * k
#     A = np.zeros((n, 2 * k * order), dtype=np.complex64)

#     # A[0,0]...A[0,2k-1] = x[0].....x[2k-1]
#     # A[1,0]...A[1,2k-1] = x[1].....x[1 + 2k-1]
#     # ......................................
#     # A[n-1,0]...A[1,2k] = x[n-1]...x[n-1 + 2k-1]
#     # repeat for 2nd order
#     # repeat for 3rd order
#     # ...
#     num = order - order % 2 + 1
#     for i in range(0, n):
#         for j in range(0, num):
#             A[i, 2 * k * j: 2 * k * (j + 1)] = np.power(x[i: i + 2 * k], 2 * j + 1)
#     return A
