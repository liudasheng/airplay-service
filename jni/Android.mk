LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES += \
        com_o2_airplay.cpp \
        onload.cpp
        

LOCAL_C_INCLUDES += $(JNI_H_INCLUDE) \
                    $(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libnativehelper \
    libutils \
    libcutils \
    liblog \
    libbinder \
    libairplay \
    libairplayService 

LOCAL_MODULE := libairplayjni
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
