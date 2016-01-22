LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libARController
LOCAL_DESCRIPTION := ARSDK Controller
LOCAL_CATEGORY_PATH := dragon/libs

LOCAL_MODULE_FILENAME := libarcontroller.so

LOCAL_LIBRARIES := \
	libARSAL \
	libARDiscovery \
	libARCommands \
	libARUtils \
	libARStream \
	libARStream2 \
	libARNetworkAL \
	libARNetwork \
	uthash \
	json

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/Includes \
	$(LOCAL_PATH)/gen/Includes \
	$(LOCAL_PATH)/Sources

LOCAL_SRC_FILES := \
	Sources/ARCONTROLLER_Dictionary.c \
	Sources/ARCONTROLLER_Frame.c \
	Sources/ARCONTROLLER_Network.c \
	Sources/ARCONTROLLER_Stream.c \
	Sources/ARCONTROLLER_StreamPool.c \
	Sources/ARCONTROLLER_StreamQueue.c \
	Sources/ARCONTROLLER_Stream1.c \
	Sources/ARCONTROLLER_Stream2.c \
	gen/Sources/ARCONTROLLER_Device.c \
	gen/Sources/ARCONTROLLER_DICTIONARY_Key.c \
	gen/Sources/ARCONTROLLER_Error.c \
	gen/Sources/ARCONTROLLER_Feature.c

LOCAL_INSTALL_HEADERS := \
	Includes/libARController/ARController.h:usr/include/libARController/ \
	Includes/libARController/ARCONTROLLER_Dictionary.h:usr/include/libARController/ \
	Includes/libARController/ARCONTROLLER_Error.h:usr/include/libARController/ \
	Includes/libARController/ARCONTROLLER_Frame.h:usr/include/libARController/ \
	Includes/libARController/ARCONTROLLER_Network.h:usr/include/libARController/ \
	Includes/libARController/ARCONTROLLER_Stream.h:usr/include/libARController/ \
	Includes/libARController/ARCONTROLLER_Stream1.h:usr/include/libARController/ \
	Includes/libARController/ARCONTROLLER_Stream2.h:usr/include/libARController/ \
	Includes/libARController/ARCONTROLLER_StreamPool.h:usr/include/libARController/ \
	Includes/libARController/ARCONTROLLER_StreamQueue.h:usr/include/libARController/ \
	gen/Includes/libARController/ARCONTROLLER_Device.h:usr/include/libARController/ \
	gen/Includes/libARController/ARCONTROLLER_DICTIONARY_Key.h:usr/include/libARController/ \
	gen/Includes/libARController/ARCONTROLLER_Feature.h:usr/include/libARController/

include $(BUILD_LIBRARY)
