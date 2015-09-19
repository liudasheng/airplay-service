
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := main_AirplayClient.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include \
                    external/airplay/shairport

LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libbinder libairplayService libairplay
LOCAL_MODULE := airplayClient
#LOCAL_MODULE_PATH := $(LOCAL_PATH)/..

include $(BUILD_EXECUTABLE)

