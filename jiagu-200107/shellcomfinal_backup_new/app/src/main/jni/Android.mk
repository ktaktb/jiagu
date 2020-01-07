# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

#include $(CLEAR_VARS)

#LOCAL_MODULE := libGodinElfHook
#LOCAL_SRC_FILES := ./lib/libGodinElfHook.so
#LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
#include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := sec

LOCAL_SRC_FILES := sec.cpp \
				   godin-hook/utils.cpp \
                   godin-hook/godin_elf_hook.cpp \
                   godin-hook/hook_module.cpp    \
			SymbolFinder.cpp \
				   godin_io_hook.cpp \
				   seclib/restoreDex.cpp \
				   seclib/unzip.c \
				   seclib/ioapi.c \
				   seclib/aes.c \
				   seclib/sha1.c \
				   seclib/amac.c \
				   seclib/aes_mod.c \
				   seclib/AES_wb_decrypt.c \
				   seclib/AES_wb_mod.c \
				   javaadb/jdebug.c \
				   nativeadb/ndebug.c \
				   dexdec/dexdec.c \
				   dexld/dexcache.cpp \
				   fileint/fileint.c \
			asubstrate/SubstrateDebug.cpp \
                   asubstrate/SubstrateHook.cpp \
                   asubstrate/SubstratePosixMemory.cpp \
			asubstrate/Substrateutil.cpp
				
				
				
ifeq (1,$(strip $(shell expr $(PLATFORM_SDK_VERSION) \>= 21)))
       LOCAL_CFLAGS += -DANDROID_L_AFTER
else
      LOCAL_CFLAGS += -UANDROID_L_AFTER
      ifeq ($(TARGET_ARCH_ABI),armeabi)
      LOCAL_C_INCLUDES := $(LOCAL_PATH)/godin-hook/elf_machdep/arch-arm
      endif

      ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
      LOCAL_C_INCLUDES := $(LOCAL_PATH)/godin-hook/elf_machdep/arch-arm
      endif

      ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
      LOCAL_C_INCLUDES := $(LOCAL_PATH)/godin-hook/elf_machdep/arch-arm64
      endif

      ifeq ($(TARGET_ARCH_ABI),x86)
      LOCAL_C_INCLUDES := $(LOCAL_PATH)/godin-hook/elf_machdep/arch-x86
      endif

      ifeq ($(TARGET_ARCH_ABI),x86-64)
      LOCAL_C_INCLUDES := $(LOCAL_PATH)/godin-hook/elf_machdep/arch-x86_64
      endif

endif
#CFLAGS += -FPIC
LOCAL_ARM_NEON := true
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog -lz -lc
LOCAL_CFLAGS	:= -std=gnu++11 -fpermissive -D _TEST_DEBUG_ANDROID -fvisibility=hidden
#LOCAL_STATIC_LIBRARIES := hook
LOCAL_CXXFLAGS:=$(LOCAL_CFLAGS)



include $(BUILD_SHARED_LIBRARY)

