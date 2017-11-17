//
// Created by vance on 2016/8/4.
//

#ifndef NATIVEIMAGEMERGE_LOG_H
#define NATIVEIMAGEMERGE_LOG_H
#include <android/log.h>

#ifdef _DEBUG_LOG



#define  LOGI(tag, ...)  __android_log_print(ANDROID_LOG_INFO,  tag, __VA_ARGS__)
#define  LOGE(tag, ...)  __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)
#define  LOGD(tag, ...)  __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
#define  LOGW(tag, ...)  __android_log_print(ANDROID_LOG_WARN,  tag, __VA_ARGS__)

#else

#define  LOGI(tag, ...)
#define  LOGE(tag, ...)
#define  LOGD(tag, ...)
#define  LOGW(tag, ...)

#endif // _DEBUG_LOG

#endif //NATIVEIMAGEMERGE_LOG_H
