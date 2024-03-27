#一般情况无需修改该文件

LOCAL_PATH=$(dir  $(abspath $(lastword $(MAKEFILE_LIST))))

EASYUI_COMMON_INCLUDE := $(LOCAL_PATH)/include

#配置源文件目录
PROJ_ROOT_PATH  :=./
#源文件后缀名
PROJ_SRC_SUFFIX := %.cpp %.c
PROJ_HEAD_SUFFIX := %.h %.hpp %.cc
# 递归遍历目录下的所有的文件 
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))  
# 获取相应的源文件
PROJ_ALL_FILES := $(foreach src_path,$(PROJ_ROOT_PATH), $(call rwildcard,$(src_path),*.*) )   
PROJ_SRC_FILES := $(filter $(PROJ_SRC_SUFFIX),$(PROJ_ALL_FILES))   
PROJ_HEAD_FILES := $(filter $(PROJ_HEAD_SUFFIX),$(PROJ_ALL_FILES))   

# 获取相应的源文件
LOCAL_SRC_FILES  := $(PROJ_SRC_FILES) 
LOCAL_HEAD_FILES := $(PROJ_HEAD_FILES) 

#源文件目录结构
PROJ_DIR_TREE := $(dir $(foreach src_path, $(LOCAL_SRC_FILES), $(call rwildcard,$(src_path),*/) ) )  
PROJ_DIR_TREE := $(sort $(PROJ_DIR_TREE))  


OBJS_ROOT_DIR=../obj/
OBJS_1 = $(patsubst %.cpp,%.o,$(LOCAL_SRC_FILES))
OBJS_2 = $(patsubst %.c,%.o,$(OBJS_1))
OBJS   = $(addprefix $(OBJS_ROOT_DIR),$(OBJS_2))
OBJS_DIR_TREE=$(addprefix $(OBJS_ROOT_DIR),$(PROJ_DIR_TREE))

CFLAGS=-Os -fPIC -pipe -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro -Wl,-z,defs -DUSE_ANDROID_LOG
CXXFLAGS=-Os -fPIC -pipe -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro -Wl,-z,defs -DUSE_ANDROID_LOG 

CXXFLAGS+= -fexceptions -Wall -nostdinc++ -std=c++11

INCLUDE_DIR := -I$(EASYUI_COMMON_INCLUDE) \
               -I$(EASYUI_COMMON_INCLUDE)/system \
               -I$(EASYUI_COMMON_INCLUDE)/uClibc++ \
			   -I$(LOCAL_PATH)/logic \
               -I.

LDFLAGS += -lcurl -lssl -lcrypto  -lz -L../libs/network
LDFLAGS += -lzkhardware -lzknet -leasyui -llog -luClibc++
LDFLAGS += -L../libs -L. 

TARGET=../libs/armeabi/libzkgui.so

ifeq ($(TARGET), $(wildcard $(TARGET)))
	DELETE_TARGET=$(TARGET)
endif

# alias commads if compile on windows
RM =rm
MKDIR=mkdir
ECHO=echo
ifeq ($(OS),Windows_NT)
	RM=win32-rm
	MKDIR=win32-mkdir
	ECHO=win32-echo
else
	#keep default
endif
#alias end

all: prepare $(TARGET)
	@$(ECHO) 
	@$(ECHO) "[armeabi] Install        : libzkgui.so => libs/armeabi/libzkgui.so"
	

$(OBJS_ROOT_DIR)%.o: %.cpp $(LOCAL_HEAD_FILES)
	@$(ECHO) "[armeabi] Compile++      : "$< 
	@$(CC) -c $< -o $@ $(CXXFLAGS) $(INCLUDE_DIR) $(LDFLAGS) 
	
$(OBJS_ROOT_DIR)%.o: %.c $(LOCAL_HEAD_FILES)
	@$(ECHO) "[armeabi] Compile++      : "$< 
	@$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_DIR) $(LDFLAGS) 

$(TARGET):$(OBJS) 
	@$(ECHO) "[armeabi] SharedLibrary  : "$@ 
	@$(CC) -fPIC -shared $^ -o $@ $(LDFLAGS) $(CXXFLAGS)
	@$(CROSS_COMPILE)strip $@
	
prepare: 
	@$(ECHO) "[armeabi] GCC            : "$(CC) 
	@-$(MKDIR) ../libs/armeabi -p 
	@-$(MKDIR) $(OBJS_DIR_TREE) -p
	@-$(RM) $(TARGET) -rf
	
clean:
	@$(ECHO) "[armeabi] Clean          : "$(OBJS_ROOT_DIR) 
	@-$(RM)  $(OBJS_ROOT_DIR) -rf 
	@$(ECHO) "[armeabi] Clean          : "$(TARGET)
	@-$(RM)  $(TARGET) -rf 

.PHONY:all clean prepare $(TARGET)
