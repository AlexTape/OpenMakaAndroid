# OpenMaka

**OpenMaka is NOT released yet!**

This project contains two different repositories.

## Custom-Branch

This branch implements a custom Camera API to deliver single frames via AsyncTasks to the JNI interface.

## OpenCV-Branch

This branch implements the Android4OpenCV Library to deliver frames to the JNI interface.

## JNI

Both branches will be contain the same C++/JNI/NDK Code to process image frames.

## Setup

1. Built with Android Studio.

2. Make sure you update Android.mk with your paths to OpenCV.

3. Also update your ndk path in local.properties

4. Make sure you have libgnustl_shared.so in OpenCV native libs dir.


