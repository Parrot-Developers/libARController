###############################################################################
# THE LIBRARIES TO BUILD
###############################################################################
ACLOCAL_AMFLAGS                                             =   -I m4

# The list of header files that belong to the library (to be installed later)
HEADER_FILES                                                =   ../Includes/libARController/ARCONTROLLER_Dictionary.h \
                                                                ../Includes/libARController/ARCONTROLLER_Error.h  \
                                                                ../Includes/libARController/ARCONTROLLER_Frame.h \
                                                                ../Includes/libARController/ARController.h \
                                                                ../Includes/libARController/ARCONTROLLER_Network.h \
                                                                ../Includes/libARController/ARCONTROLLER_Stream.h \
                                                                ../Includes/libARController/ARCONTROLLER_Stream1.h \
                                                                ../Includes/libARController/ARCONTROLLER_Stream2.h \
                                                                ../Includes/libARController/ARCONTROLLER_StreamPool.h \
                                                                ../Includes/libARController/ARCONTROLLER_StreamQueue.h \
                                                                ../gen/Includes/libARController/ARCONTROLLER_Device.h \
                                                                ../gen/Includes/libARController/ARCONTROLLER_DICTIONARY_Key.h \
                                                                ../gen/Includes/libARController/ARCONTROLLER_Feature.h
# The sources to add to the library and to add to the source distribution
SOURCE_FILES                                                =   $(HEADER_FILES) \
                                                                ../Sources/ARCONTROLLER_Dictionary.c \
                                                                ../Sources/ARCONTROLLER_Dictionary.h \
                                                                ../Sources/ARCONTROLLER_Frame.c \
                                                                ../Sources/ARCONTROLLER_Frame.h \
                                                                ../Sources/ARCONTROLLER_Network.c \
                                                                ../Sources/ARCONTROLLER_Network.h \
                                                                ../Sources/ARCONTROLLER_Stream.c \
                                                                ../Sources/ARCONTROLLER_Stream.h \
                                                                ../Sources/ARCONTROLLER_StreamPool.c \
                                                                ../Sources/ARCONTROLLER_StreamPool.h \
                                                                ../Sources/ARCONTROLLER_StreamQueue.c \
                                                                ../Sources/ARCONTROLLER_StreamQueue.h \
                                                                ../Sources/ARCONTROLLER_Stream1.c \
                                                                ../Sources/ARCONTROLLER_Stream1.h \
                                                                ../Sources/ARCONTROLLER_Stream2.c \
                                                                ../Sources/ARCONTROLLER_Stream2.h \
                                                                ../gen/Includes/libARController/ARCONTROLLER_Device.h \
                                                                ../gen/Includes/libARController/ARCONTROLLER_DICTIONARY_Key.h \
                                                                ../gen/Includes/libARController/ARCONTROLLER_Feature.h \
                                                                ../gen/Sources/ARCONTROLLER_Device.c \
                                                                ../gen/Sources/ARCONTROLLER_Device.h \
                                                                ../gen/Sources/ARCONTROLLER_DICTIONARY_Key.c \
                                                                ../gen/Sources/ARCONTROLLER_Feature.c \
                                                                ../gen/Sources/ARCONTROLLER_Feature.h \
                                                                ../gen/Sources/ARCONTROLLER_Error.c

# The library names to build (note we are building static and shared libs)
if DEBUG_MODE
lib_LTLIBRARIES                                             =   libarcontroller_dbg.la
libarcontroller_dbg_ladir                                      =   $(includedir)/libARController
libarcontroller_dbg_la_HEADERS                                 =   $(HEADER_FILES)
libarcontroller_dbg_la_SOURCES                                 =   $(SOURCE_FILES)
else
lib_LTLIBRARIES                                             =   libarcontroller.la
libarcontroller_ladir                                       =   $(includedir)/libARController
libarcontroller_la_HEADERS                                  =   $(HEADER_FILES)
libarcontroller_la_SOURCES                                  =   $(SOURCE_FILES)
endif

check_PROGRAMS                                              =   ../TestBench/Unix/networkControllerAutoTest \
                                                                ../TestBench/Unix/deviceControllerAutoTest
                                                                #../TestBench/Unix/featureControllerAutoTest


___TestBench_Unix_networkControllerAutoTest_SOURCES =   ../TestBench/Common/ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest.c \
                                                        ../TestBench/Unix/ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_Unix.c
                                                        
#___TestBench_Unix_featureControllerAutoTest_SOURCES =   ../TestBench/Common/ARCONTROLLER_TESTBENCH_FeatureControllerAutoTest.c \
#                                                        ../TestBench/Unix/ARCONTROLLER_TESTBENCH_FeatureControllerAutoTest_Unix.c

___TestBench_Unix_deviceControllerAutoTest_SOURCES =   ../TestBench/Common/ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest.c \
                                                       ../TestBench/Unix/ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_Unix.c

if DEBUG_MODE
___TestBench_Unix_networkControllerAutoTest_LDADD =   -larsal_dbg \
                                                      -lardiscovery_dbg \
                                                      -larnetworkal_dbg \
                                                      -larcommands_dbg \
                                                      -larstream_dbg \
                                                      -ljson \
                                                      libarcontroller_dbg.la
                                                      
#___TestBench_Unix_featureControllerAutoTest_LDADD =   -larsal_dbg \
                                                      #-lardiscovery_dbg \
                                                      #-larnetworkal_dbg \
                                                      #-larcommands_dbg \
                                                      #-ljson \
                                                      #libarcontroller_dbg.la
                                                      
___TestBench_Unix_deviceControllerAutoTest_LDADD =   -larsal_dbg \
                                                      -lardiscovery_dbg \
                                                      -larnetworkal_dbg \
                                                      -larcommands_dbg \
                                                      -larstream_dbg \
                                                      -ljson \
                                                      libarcontroller_dbg.la
else
___TestBench_Unix_networkControllerAutoTest_LDADD =   -larsal \
                                                      -lardiscovery \
                                                      -larnetworkal  \
                                                      -larcommands  \
                                                      -larstream \
                                                      -ljson \
                                                      libarcontroller.la
                                                      
#___TestBench_Unix_featureControllerAutoTest_LDADD =   -larsal \
                                                      #-lardiscovery \
                                                      #-larnetworkal  \
                                                      #-larcommands  \
                                                      #-ljson \
                                                      #libarcontroller.la
                                                      
___TestBench_Unix_deviceControllerAutoTest_LDADD =   -larsal \
                                                      -lardiscovery \
                                                      -larnetworkal  \
                                                      -larcommands  \
                                                      -larstream \
                                                      -ljson \
                                                      libarcontroller.la

endif

CLEAN_FILES                                                 =   libarcontroller.la       \
                                                                libarcontroller_dbg.la

EXTRA_DIST                                                  =   bootstrap

# Ensure the distribution of the doxygen configuration file
# Doxygen support
if HAVE_DOXYGEN
include doxygen.am
EXTRA_DIST                                                  +=  doxygen.ac  \
                                                                doxygen.am  \
                                                                $(DX_CONFIG)
endif
