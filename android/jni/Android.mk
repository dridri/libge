LOCAL_PATH:= $(call my-dir)

# ------------------------------------------------------------------
# Static library
# ------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libge

ifneq (,$(filter $(TARGET_ARCH), x86_64 arm64 arm64-v8 mips64))
	ARCH = 64
else
	ARCH = 32
endif

SRC := $(wildcard ../src/*.c)
SRC += $(wildcard ../src/video/*.c)
# SRC += $(wildcard ../src/audio/*.c)
SRC += $(wildcard ../src/gui/*.c)
SRC += $(wildcard ../src/lua/*.c)
SRC += $(wildcard ../src/system/android/*.c)
SRC += $(wildcard ../src/video/android/*.c)
# SRC += $(wildcard ../src/audio/android/*.c) TODO: install mad

SRC := $(filter-out ../src/main.c, $(SRC))
SRC := $(filter-out ../src/gecl.c, $(SRC))
SRC := $(filter-out ../src/ge_internal_32.c, $(SRC))
SRC := $(filter-out ../src/ge_internal_64.c, $(SRC))
SRC += ../src/ge_internal_$(ARCH).c

#LOCAL_SRC_FILES = $(addprefix ../, $(SRC:.cpp=.o))
#LOCAL_SRC_FILES += main.c

LOCAL_LDFLAGS := -fPIC
LOCAL_CFLAGS := -fPIC -I../include -DPLATFORM_android

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

LOCAL_LDLIBS    := -llua52 -lfreetype2-static -ljpeg9 -lpng -lz -lm -llog -landroid -lEGL -lGLESv2
LOCAL_WHOLE_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_STATIC_LIBRARY)
# include $(BUILD_SHARED_LIBRARY)

$(call import-module, android/native_app_glue)
