LOCAL_PATH:= $(call my-dir)

ZKSW_PREBUILT_LIB_PATH := $(ZKSW_REAL_PREBUILT)/lib
ZKSW_PREBUILT_INCLUDE_PATH := $(ZKSW_REAL_PREBUILT)/include

EASYUI_COMMON_INCLUDE := $(LOCAL_PATH)/include


include $(CLEAR_VARS)
LOCAL_MODULE	:= easyui
LOCAL_SRC_FILES	:= libeasyui.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)

PROJ_ROOT_PATH  :=  $(LOCAL_PATH)  
PROJ_SRC_SUFFIX := %.cpp %.c  
  
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))  
  
PROJ_ALL_FILES := $(foreach src_path,$(PROJ_ROOT_PATH), $(call rwildcard,$(src_path),*.*) )   
PROJ_ALL_FILES := $(PROJ_ALL_FILES:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)  
PROJ_SRC_FILES  := $(filter $(PROJ_SRC_SUFFIX),$(PROJ_ALL_FILES))   
PROJ_SRC_FILES  := $(PROJ_SRC_FILES:$(LOCAL_PATH)/%=%)  
  
LOCAL_SRC_FILES  := $(PROJ_SRC_FILES)  

LOCAL_CPPFLAGS := -fexceptions -fpermissive
LOCAL_LDFLAGS := -L$(ZKSW_PREBUILT_LIB_PATH)/system -lstlport

LOCAL_C_INCLUDES := $(EASYUI_COMMON_INCLUDE) \
					$(ZKSW_PREBUILT_INCLUDE_PATH)/bionic \
					$(ZKSW_PREBUILT_INCLUDE_PATH)/stlport \
					$(ZKSW_PREBUILT_INCLUDE_PATH)/gui \

LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -DUSE_ANDROID_LOG

LOCAL_SHARED_LIBRARIES := easyui
LOCAL_MODULE := zkgui

include $(BUILD_SHARED_LIBRARY)
