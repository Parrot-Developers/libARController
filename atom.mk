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
	librtsp \
	libsdp \
	libfutils \
	libpomp \
	json

LOCAL_CONDITIONAL_LIBRARIES := \
	OPTIONAL:libmux

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/Includes \
	$(call local-get-build-dir)/gen/Includes \
	$(LOCAL_PATH)/Sources \
	$(call local-get-build-dir)/gen/Sources

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H

LOCAL_SRC_FILES := \
	Sources/ARCONTROLLER_Dictionary.c \
	Sources/ARCONTROLLER_Frame.c \
	Sources/ARCONTROLLER_Network.c \
	Sources/ARCONTROLLER_Stream.c \
	Sources/ARCONTROLLER_StreamPool.c \
	Sources/ARCONTROLLER_StreamQueue.c \
	Sources/ARCONTROLLER_Stream1.c \
	Sources/ARCONTROLLER_Stream2.c \
	Sources/ARCONTROLLER_StreamSender.c \
	Sources/ARCONTROLLER_NAckCbs.c

LOCAL_GENERATED_SRC_FILES := \
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
	$(call local-get-build-dir)/gen/Includes/libARController/ARCONTROLLER_Device.h:usr/include/libARController/ \
	$(call local-get-build-dir)/gen/Includes/libARController/ARCONTROLLER_DICTIONARY_Key.h:usr/include/libARController/ \
	$(call local-get-build-dir)/gen/Includes/libARController/ARCONTROLLER_Feature.h:usr/include/libARController/

LOCAL_CUSTOM_MACROS := \
	arsdkgen-macro:$(LOCAL_PATH)/Python/generateControllersSources.py,$(call local-get-build-dir)/gen,native

include $(BUILD_LIBRARY)

###############################################################################
#  Messages documentation generation
###############################################################################

include $(CLEAR_VARS)

LOCAL_MODULE := libARController_doc
LOCAL_DESCRIPTION := ARSDK Controller
LOCAL_CATEGORY_PATH := dragon/libs

LOCAL_CUSTOM_MACROS := \
	arsdkgen-macro:$(LOCAL_PATH)/Python/generateCommandsDoc.py,$(call local-get-build-dir)/gen,native:-r

include $(BUILD_CUSTOM)
