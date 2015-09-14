LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := IAirplayService.cpp 

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES := libutils liblog libbinder
LOCAL_MODULE := libairplay

include $(BUILD_SHARED_LIBRARY)
