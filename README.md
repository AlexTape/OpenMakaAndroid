# OpenMaka

** THIS APPLICATION IS NOT RELEASED YET **

This project contains two different repositories.

## Custom-Branch

This branch implements a custom Camera API to deliver single frames via AsyncTasks to the JNI interface.

## OpenCV-Branch

This branch implements the Android4OpenCV Library to deliver frames to the JNI interface.

## JNI

Both branches will be contain the same C++/JNI/NDK Code to process image frames.

## Setup

- Built with Android Studio
- Using OpenCV 2.4.11 (through JNI)
- Make sure you update Android.mk with your paths to OpenCV
- Also update your ndk path in local.properties
- Make sure you have libgnustl_shared.so in OpenCV native libs dir
