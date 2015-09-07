# OpenMakaAndroid App

## Master-Branch

This branch implements the OpenMakaEngine as an Android Application. It is using the Android4OpenCV Library to deliver frames via the JNI Interface for processing with OpenMakaEngine.

## Custom-Branch

This branch implements a custom Camera API to deliver single frames via AsyncTasks to the JNI interface. OpenMakaEngine not ported yet.

## Setup

- Built with Android Studio
- Using OpenCV 2.4.11 (through JNI)
- Make sure you update Android.mk with your paths to OpenCV
- Also update your ndk path in local.properties
- Make sure you have libgnustl_shared.so in OpenCV native libs dir
