LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := AirplayService.cpp 

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include \
                    external/airplay/shairport

LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libbinder libairplay libshairport
LOCAL_MODULE := libairplayService

include $(BUILD_SHARED_LIBRARY)
