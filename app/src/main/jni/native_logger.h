#ifndef __dbg_h__
#define __dbg_h__
 
#include <stdio.h>
#include <errno.h>
#include <string.h>
 
// Debug tag
#define DTAG "OpenMaka::Native"
 
#ifdef __ANDROID__
#include <android/log.h>
#ifdef NDEBUG
#define debug(D, M, ...)
#else
#define debug(D, M, ...) __android_log_print(ANDROID_LOG_ERROR, D, "DEBUG %s:%d: " M "", __FILE__, __LINE__, ##__VA_ARGS__)
#endif
 
#define clean_errno() (errno == 0 ? "None" : strerror(errno))
 
#define log_err(D, M, ...) __android_log_print(ANDROID_LOG_ERROR, D, "[ERROR] (%s:%d: errno: %s) " M "", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
 
#define log_warn(D, M, ...) __android_log_print(ANDROID_LOG_WARN, D, "[WARN] (%s:%d: errno: %s) " M "", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
 
#define log_info(D, M, ...) __android_log_print(ANDROID_LOG_INFO, D, "[INFO] " M "", ##__VA_ARGS__)

#define log(...)  __android_log_print(ANDROID_LOG_INFO, DTAG, __VA_ARGS__)
 
#define log_debug(D, M, ...) __android_log_print(ANDROID_LOG_DEBUG, D, "[DEBUG] (%s:%d) " M "", __FILE__, __LINE__, ##__VA_ARGS__)
 
#define check(A, D, M, ...) errno=0; if(!(A)) { log_err(D, M, ##__VA_ARGS__); errno=0; goto error; }
 
#define check_zero(A, D, M, ...) check(A==0, D, M, ##__VA_ARGS__)
 
#define sentinel(D, M, ...)  { log_err(D, M, ##__VA_ARGS__); errno=0; goto error; }
 
#define check_mem(A, D) check((A), D, "Out of memory.")
 
#define check_debug(A, D, M, ...) if(!(A)) { debug(D, M, ##__VA_ARGS__); errno=0; goto error; }
 
#endif // __ANDROID__

#endif // _dbg_h_