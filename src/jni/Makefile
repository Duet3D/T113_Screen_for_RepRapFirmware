# No need to modify this file in general

CC=arm-unknown-linux-musleabihf-gcc
CROSS_COMPILE=arm-unknown-linux-musleabihf-
ZKSW_REAL_PREBUILT="c:/flythings/sdk/platforms/t113/prebuilt"
ZKSW_REAL_TOOLCHAIN="c:/flythings/sdk/platforms/t113/toolchain"

# Set the local path to the directory of the current Makefile
LOCAL_PATH=$(dir  $(abspath $(lastword $(MAKEFILE_LIST))))

# Define the include directory for EasyUI common files
EASYUI_COMMON_INCLUDE := $(LOCAL_PATH)/include

# Configure source file directory
PROJ_ROOT_PATH  :=./
# Source file extensions
PROJ_SRC_SUFFIX := %.cpp %.c
PROJ_HEAD_SUFFIX := %.h %.hpp %.cc
# Recursively traverse all files in directories
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))  
# Get corresponding source files
PROJ_ALL_FILES := $(foreach src_path,$(PROJ_ROOT_PATH), $(call rwildcard,$(src_path),*.*) )   
PROJ_SRC_FILES := $(filter $(PROJ_SRC_SUFFIX),$(PROJ_ALL_FILES))   
PROJ_HEAD_FILES := $(filter $(PROJ_HEAD_SUFFIX),$(PROJ_ALL_FILES))   

# Get corresponding source files
LOCAL_SRC_FILES  := $(PROJ_SRC_FILES) 
LOCAL_HEAD_FILES := $(PROJ_HEAD_FILES) 

# Source file directory structure
PROJ_DIR_TREE := $(dir $(foreach src_path, $(LOCAL_SRC_FILES), $(call rwildcard,$(src_path),*/) ) )  
PROJ_DIR_TREE := $(sort $(PROJ_DIR_TREE))  

# Set the root directory for object files
OBJS_ROOT_DIR=../obj/
# Generate object file names from source file names
OBJS_1 = $(patsubst %.cpp,%.o,$(LOCAL_SRC_FILES))
OBJS_2 = $(patsubst %.c,%.o,$(OBJS_1))
OBJS   = $(addprefix $(OBJS_ROOT_DIR),$(OBJS_2))
# Generate object file directory structure
OBJS_DIR_TREE=$(addprefix $(OBJS_ROOT_DIR),$(PROJ_DIR_TREE))

# Compiler flags
CFLAGS=-Os -fPIC -pipe -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro -Wl,-z,defs -DUSE_ANDROID_LOG
CXXFLAGS=-Os -fPIC -pipe -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro -Wl,-z,defs -DUSE_ANDROID_LOG 

# Add -MMD -MP to generate .d files
CFLAGS += -MMD -MP
CXXFLAGS += -MMD -MP

# Additional compiler flags
CXXFLAGS+= -fexceptions -Wall -nostdinc++ -std=c++11

# Compiler flags for debugging
# CXXFLAGS+= -ftime-report -v

# Include directories
# INCLUDE_DIR += -I$(ZKSW_REAL_PREBUILT)/include
# INCLUDE_DIR += -I$(ZKSW_REAL_PREBUILT)/include/system
INCLUDE_DIR += -I$(EASYUI_COMMON_INCLUDE)/system
INCLUDE_DIR += -I$(EASYUI_COMMON_INCLUDE)/uClibc++			# some files have been modified
INCLUDE_DIR += -I$(ZKSW_REAL_PREBUILT)/include/uClibc++		# including all the files slowed down the compilation for some reason
INCLUDE_DIR += -I$(EASYUI_COMMON_INCLUDE)
INCLUDE_DIR += -I$(EASYUI_COMMON_INCLUDE)/../logic
INCLUDE_DIR += -I.

# Linker flags
LDFLAGS += -lcurl -lssl -lcrypto  -lz -L../libs/network
LDFLAGS += -lzkhardware -lzknet -leasyui -llog -luClibc++
LDFLAGS += -L../libs -L. 

# Target output file
TARGET=../libs/armeabi/libzkgui.so

# Check if the target file already exists and delete it if necessary
ifeq ($(TARGET), $(wildcard $(TARGET)))
	DELETE_TARGET=$(TARGET)
endif

# Alias commands if compiling on Windows
RM =rm
MKDIR=mkdir
ECHO=echo
ifeq ($(OS),Windows_NT)
	RM=win32-rm
	MKDIR=win32-mkdir
	ECHO=win32-echo
else
	# Keep default
endif
# Alias end

# Collect .d files from OBJS_ROOT_DIR
DEP_FILES := $(OBJS:.o=.d)

-include $(DEP_FILES)

# Default target
all: prepare $(OBJS) $(TARGET)
	@$(ECHO) 
	@$(ECHO) "[armeabi] Install        : libzkgui.so => libs/armeabi/libzkgui.so"
	
# Compile C++ source files
$(OBJS_ROOT_DIR)%.o: %.cpp
	@$(ECHO) "[armeabi] Compile++      : "$< 
	@$(CC) -c $< -o $@ $(CXXFLAGS) $(INCLUDE_DIR) $(LDFLAGS) 
	
# Compile C source files
$(OBJS_ROOT_DIR)%.o: %.c
	@$(ECHO) "[armeabi] Compile++      : "$< 
	@$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_DIR) $(LDFLAGS) 

# Link object files to create the target shared library
$(TARGET):$(OBJS) 
	@$(ECHO) "[armeabi] SharedLibrary  : "$@ 
	@$(CC) -fPIC -shared $^ -o $@ $(LDFLAGS) $(CXXFLAGS)
	@$(CROSS_COMPILE)strip $@
	
# Prepare the build environment
prepare: 
	@$(ECHO) "[armeabi] GCC            : "$(CC) 
	@-$(MKDIR) -p ../libs/armeabi
	@-$(MKDIR) -p $(OBJS_DIR_TREE)
	@-$(RM) $(TARGET) -rf
	
# Clean the build artifacts
clean:
	@$(ECHO) "[armeabi] Clean          : "$(OBJS_ROOT_DIR) 
	@-$(RM)  $(OBJS_ROOT_DIR) -rf 
	@$(ECHO) "[armeabi] Clean          : "$(TARGET)
	@-$(RM)  $(TARGET) -rf 

.PHONY:all clean prepare $(TARGET)
