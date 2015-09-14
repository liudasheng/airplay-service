LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, .)

LOCAL_MODULE := com.o2.airplay

include $(BUILD_STATIC_JAVA_LIBRARY)
#include $(BUILD_JAVA_LIBRARY)
