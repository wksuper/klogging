LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libklogging
LOCAL_SRC_FILES := src/klogging.cpp
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := example/myprog.c
LOCAL_MODULE := myprog
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := libklogging
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
