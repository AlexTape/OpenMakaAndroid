LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#opencv
OPENCVROOT := /home/thinker/Android/OpenCV-3-0
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

# libgnustl (if needed)
#ifeq ($(gnustl_static),true)
#include /home/thinker/Android/Ndk/sources/cxx-stl/gnu-libstdc++/Android.mk
#endif

LOCAL_MODULE := native_openmaka

# add source files
FILE_LIST := $(wildcard $(LOCAL_PATH)/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS    := -Werror -O3 -ffast-math -DNDEBUG -DANDROID_NDK -DDISABLE_IMPORTGL -DOPEL_ES_1 -ffast-math
LOCAL_CXXFLAGS  += -fno-exceptions
LOCAL_LDLIBS    += -L$(LOCAL_PATH)/lib -llog -landroid  -ldl -lEGL -lGLESv1_CM -lOpenSLES -lGLESv2
LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

include $(BUILD_SHARED_LIBRARY)