#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/python
export PATH=/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/build/python:$PATH
export LD_LIBRARY_PATH=/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/build/swig:$PYTHONPATH
/usr/bin/python2 /home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/python/qa_parse_stream_sync.py 
