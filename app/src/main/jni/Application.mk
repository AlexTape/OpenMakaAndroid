APP_STL := gnustl_static
APP_CPPFLAGS := -frtti -fexceptions -std=c++11
LOCAL_C_INCLUDES += ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/include
APP_ABI := armeabi-v7a
APP_OPTIM := debug
APP_PLATFORM := android-16
STLPORT_FORCE_REBUILD := true
APP_MODULES := native_openmaka
