LOCAL_PATH := $(call my-dir)

# JNI Wrapper
include $(CLEAR_VARS)

LOCAL_CFLAGS := -g
LOCAL_MODULE := libarcontroller_android
LOCAL_SRC_FILES := gen/JNI/c/ARCONTROLLER_JNI_Features.c JNI/c/ARCONTROLLER_JNI_Dictionary.c gen/JNI/c/ARCONTROLLER_JNI_Device.c
LOCAL_LDLIBS := -llog -lz
LOCAL_SHARED_LIBRARIES := libARController-prebuilt libARSAL-prebuilt libARStream-prebuilt libARStream2-prebuilt
include $(BUILD_SHARED_LIBRARY)
