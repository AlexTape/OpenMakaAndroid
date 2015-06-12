/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class de_alextape_openmaka_NativeFunctions */

#ifndef _Included_de_alextape_openmaka_NativeFunctions
#define _Included_de_alextape_openmaka_NativeFunctions
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_start
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1start
  (JNIEnv *, jclass);

/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_initialize
 * Signature: (JLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1initialize
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_displayFunction
 * Signature: (JJ)I
 */
JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1displayFunction
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_glRender
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1glRender
  (JNIEnv *, jclass);

/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_glResize
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1glResize
  (JNIEnv *, jclass, jint, jint);

/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_setObjectDetection
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1setObjectDetection
  (JNIEnv *, jclass, jboolean);

/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_setTracking
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1setTracking
  (JNIEnv *, jclass, jboolean);

/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_setOpenGL
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1setOpenGL
  (JNIEnv *, jclass, jboolean);

#ifdef __cplusplus
}
#endif
#endif