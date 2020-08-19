# CMake generated Testfile for 
# Source directory: /home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/python
# Build directory: /home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/build/python
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(qa_parse_stream_sync "/bin/sh" "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/build/python/qa_parse_stream_sync_test.sh")
set_tests_properties(qa_parse_stream_sync PROPERTIES  _BACKTRACE_TRIPLES "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/cmake/Modules/GrTest.cmake;114;add_test;/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/python/CMakeLists.txt;47;GR_ADD_TEST;/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/python/CMakeLists.txt;0;")
add_test(qa_digital_sic "/bin/sh" "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/build/python/qa_digital_sic_test.sh")
set_tests_properties(qa_digital_sic PROPERTIES  _BACKTRACE_TRIPLES "/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/cmake/Modules/GrTest.cmake;114;add_test;/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/python/CMakeLists.txt;48;GR_ADD_TEST;/home/wimnet-flexicon/Git/flexicon_dev/gr-fullduplex/python/CMakeLists.txt;0;")
