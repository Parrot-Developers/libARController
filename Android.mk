LOCAL_PATH := $(call my-dir)

# JNI Wrapper
include $(CLEAR_VARS)

# directory containing generated files
CTR_GEN_OUT = $(NDK_OUT)/libarcontroller-jni-gen/

LOCAL_CFLAGS := -g
LOCAL_MODULE := libarcontroller_android

LOCAL_GEN_FILES := \
	$(CTR_GEN_OUT)/JNI/c/ARCONTROLLER_JNI_Features.c \
	$(CTR_GEN_OUT)/JNI/c/ARCONTROLLER_JNI_Device.c

LOCAL_SRC_FILES := \
	JNI/c/ARCONTROLLER_JNI_Dictionary.c \
	$(LOCAL_GEN_FILES)

LOCAL_LDLIBS := -llog -lz
LOCAL_SHARED_LIBRARIES := \
	libARController \
	libARSAL \
	libARStream \
	libARStream2 \
	libmux

include $(BUILD_SHARED_LIBRARY)

# Add rule to run jni controller generator.
# ----------------------------------------

# avoid defining the rule multiple time as this Android.mk is included for each eabi
ifndef CTR_ARSDK_XML_ROOT

# arsdkgen in current arch product sdk
CTR_ARSDK_XML_ROOT := $(PRODUCT_OUT_DIR)/$(TARGET_ARCH_ABI)/sdk/host/usr/lib/arsdkgen/
LIBARCTR_DIR := $(PACKAGES_DIR)/libARController/

# LOCAL_GEN_FILES depend on parser, generator, and all xml files
$(LOCAL_GEN_FILES): $(CTR_ARSDK_XML_ROOT)/arsdkparser.py $(wildcard $(LIBARCTR_DIR)/Python/*.py) $(wildcard $(CTR_ARSDK_XML_ROOT)/xml/*.xml)
	$(CTR_ARSDK_XML_ROOT)/arsdkgen.py $(LIBARCTR_DIR)/Python/generateControllersSources.py -o $(CTR_GEN_OUT) jni

clean-arsdk-jni-$(TARGET_ARCH_ABI):: clean-arsdk-generated

clean-ctr-generated:
	rm $(CTR_GEN_OUT)/*

endif
