#######################################################################################################
# Authors:
# - Guy Farkash,    email: gf2373@columbia.edu
# - Tingjun Chen,   email: tc2668@columbia.edu
#
# Date: Feb. 8, 2018
#
# For more details, please refer to the Columbia FlexICoN project
########################################################################################################

import gen_data_psk as gen
import usrp_txrx_psk as txrx
import dig_sic_on_psk as sic
from PyQt4 import Qt
from gnuradio import gr
import sys
import time


from distutils.version import StrictVersion
if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
    style = gr.prefs().get_string('qtgui', 'style', 'raster')
    Qt.QApplication.setGraphicsSystem(style)
qapp = Qt.QApplication(sys.argv)


def gen_main(top_block_cls=gen.gen_data_psk, options=None):

    # from distutils.version import StrictVersion
    # if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
    #     style = gr.prefs().get_string('qtgui', 'style', 'raster')
    #     Qt.QApplication.setGraphicsSystem(style)
    # qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
        tb.top_widget.close()

    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    Qt.QTimer.singleShot(5000, qapp.quit)
    qapp.exec_()


def txrx_main(top_block_cls=txrx.usrp_txrx_psk, options=None):

    tb = top_block_cls()
    tb.start()

    def quitting():
        tb.stop()
        tb.wait()

    time.sleep(3)
    quitting()


def sync_main(top_block_cls=sic.dig_sic_on_psk, options=None):

    # from distutils.version import StrictVersion
    # if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
    #     style = gr.prefs().get_string('qtgui', 'style', 'raster')
    #     Qt.QApplication.setGraphicsSystem(style)
    # qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
        tb.top_widget.close()

    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


gen_main()
txrx_main()
sync_main()
