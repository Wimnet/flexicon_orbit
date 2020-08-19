#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/lib
export PATH=/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/build/lib:$PATH
export LD_LIBRARY_PATH=/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-fullduplex 
