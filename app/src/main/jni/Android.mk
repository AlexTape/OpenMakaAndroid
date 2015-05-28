LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#LOCAL_MODULE := gnustl_shared
#LOCAL_SRC_FILES := libgnustl_shared.so
#LOCAL_LDLIBS        += -lgnustl_shared

LOCAL_MODULE    := nonfree
LOCAL_SRC_FILES := lib/libnonfree.so

include $(PREBUILT_SHARED_LIBRARY)

LOCAL_CFLAGS := -DOPEL_ES_1 -DANDROID_NDK
LOCAL_CXXFLAGS   += -fno-exceptions

LOCAL_ARM_MODE := arm

# opencv
OPENCV_ROOT:=/home/thinker/Android/OpenCV
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
#OPENCV_LIB_TYPE:=SHARED
OPENCV_LIB_TPYE:=STATIC
include ${OPENCV_ROOT}/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := native_openmaka
LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL

FILE_LIST := $(wildcard $(LOCAL_PATH)/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS +=  -L$(LOCAL_PATH)/lib -lGLESv1_CM -llog -ldl -lnonfree
LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

include $(BUILD_SHARED_LIBRARY)