LOCAL_PATH := $(call my-dir)

# JNI Wrapper
include $(CLEAR_VARS)

LOCAL_CFLAGS := -g
LOCAL_MODULE := libarcontroller_android
LOCAL_SRC_FILES := gen/JNI/c/ARCONTROLLER_JNI_FeatureSkyController.c gen/JNI/c/ARCONTROLLER_JNI_FeatureJumpingSumo.c gen/JNI/c/ARCONTROLLER_JNI_FeatureCommon.c JNI/c/ARCONTROLLER_JNI_Dictionary.c gen/JNI/c/ARCONTROLLER_JNI_FeatureARDrone3.c gen/JNI/c/ARCONTROLLER_JNI_FeatureMiniDrone.c gen/JNI/c/ARCONTROLLER_JNI_Device.c gen/JNI/c/ARCONTROLLER_JNI_FeaturePro.c
LOCAL_LDLIBS := -llog -lz
LOCAL_SHARED_LIBRARIES := libARController-prebuilt libARSAL-prebuilt libARStream-prebuilt libARStream2-prebuilt
include $(BUILD_SHARED_LIBRARY)
