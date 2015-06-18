LOCAL_PATH  := $(call my-dir)

include $(CLEAR_VARS)

#opencv
OPENCVROOT := /home/thinker/Android/OpenCV-2-4-11
OPENCV_CAMERA_MODULES:=off
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

LOCAL_C_INCLUDES :=				\
	$(LOCAL_PATH)				\
	/home/thinker/Android/OpenCV-2-4-11/sdk/native/jni/include

LOCAL_SRC_FILES :=				\
	nonfree_init.cpp			\
	sift.cpp					\
	surf.cpp

LOCAL_MODULE := nonfree
LOCAL_CFLAGS := -Werror -O3 -ffast-math
LOCAL_LDLIBS := -llog -ldl

include $(BUILD_SHARED_LIBRARY)
