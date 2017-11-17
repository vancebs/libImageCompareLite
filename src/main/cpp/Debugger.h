//
// Created by vance on 2016/8/23.
//

#ifndef NATIVEIMAGEMERGE_DEBUGGER_H
#define NATIVEIMAGEMERGE_DEBUGGER_H

#include <jni.h>
#include "inc.h"
#include "NativeBitmap.h"
#include "Feature.h"
#include <time.h>

#ifdef _DEBUG_PERFORMANCE
#define TRACE_INIT(pDebugger) \
    if (pDebugger != NULL && pDebugger->isPerformanceDebugEnabled()) { \
        pDebugger->traceInit(); \
    }

#define TRACE_BEGIN(pDebugger) \
    if (pDebugger != NULL && pDebugger->isPerformanceDebugEnabled()) { \
        pDebugger->traceBegin(); \
    }

#define TRACE_END(pDebugger, tag) \
    if (pDebugger != NULL && pDebugger->isPerformanceDebugEnabled()) { \
        pDebugger->traceEnd(); \
        pDebugger->printLastTraceTime(tag); \
    }

#define TRACE_FINISH(pDebugger, tag) \
    if (pDebugger != NULL && pDebugger->isPerformanceDebugEnabled()) { \
        pDebugger->printFinalTraceRecord(tag); \
    }
#else
#define TRACE_INIT(pDebugger)
#define TRACE_BEGIN(pDebugger)
#define TRACE_END(pDebugger, tag)
#define TRACE_FINISH(pDebugger, tag)
#endif

#ifdef _DEBUG_IMAGE
#define IMAGE_DEBUG_DIR "/sdcard/SupershotTest"
#define PRINT_IMAGE(pDebugger, file, bmp) \
    if (pDebugger != NULL && pDebugger->isBitmapDebugEnabled()) { \
        pDebugger->printBitmap(file, bmp); \
    }

#define PRINT_XOR_IMAGE(pDebugger, file, bmp) \
    if (pDebugger != NULL && pDebugger->isBitmapDebugEnabled()) { \
        pDebugger->printBitmap(file, bmp, false); \
    }

#define PRINT_FEATURE_IMAGE(pDebugger, file, bmp, feature) \
    if (pDebugger != NULL && pDebugger->isBitmapDebugEnabled()) { \
        pDebugger->printBitmap(file, bmp, feature); \
    }

#define PRINT_HASH_IMAGE(pDebugger, file, bmp, hash) \
    if (pDebugger != NULL && pDebugger->isBitmapDebugEnabled()) { \
        pDebugger->printHashBitmap(file, bmp, hash); \
    }

#define PRINT_HASH_MASK_IMAGE(pDebugger, file, bmp, hash) \
    if (pDebugger != NULL && pDebugger->isBitmapDebugEnabled()) { \
        pDebugger->printHashMaskBitmap(file, bmp, hash); \
    }

#else
#define IMAGE_DEBUG_DIR
#define PRINT_IMAGE(pDebugger, file, bmp)
#define PRINT_XOR_IMAGE(pDebugger, file, bmp)
#define PRINT_FEATURE_IMAGE(pDebugger, file, bmp, feature)
#define PRINT_HASH_IMAGE(pDebugger, file, bmp, hash)
#define PRINT_HASH_MASK_IMAGE(pDebugger, file, bmp, hash)
#endif

class Debugger {
private:
    const char* mDir;
    JNIEnv* mpEnv;

    clock_t mTraceTotalTime;
    clock_t mTraceBegin;
    clock_t mTraceEnd;

    bool mBitmapDebugEnabled;
    bool mPerformanceDebugEnabled;

public:
    Debugger(JNIEnv* env);

    void enableBitmapDebug(bool enable, JNIEnv* env = NULL, const char* dirPath = NULL);
    void enablePerformanceDebug(bool enable);

    bool isBitmapDebugEnabled() const;
    bool isPerformanceDebugEnabled() const;

    void printBitmap(IN const char* fileName, IN const NativeBitmap& bmp, bool hasAlpha = true) const;
    void printBitmap(IN const char* fileName, IN const NativeBitmap& bmp, FeatureList& featureList, bool hasAlpha = true) const;

    void printHashBitmap(IN const char* fileName, IN const NativeBitmap& bmp, const jint* hash) const;
    void printHashMaskBitmap(IN const char* fileName, IN const NativeBitmap& bmp, const jint* hashMask) const;

    void traceInit();
    void traceBegin();
    void traceEnd();
    void printLastTraceTime(IN const char* tag);
    void printFinalTraceRecord(IN const char* tag);

private:
    bool mkdirs(const char* dir);
    void drawFeature(NativeBitmap& bmp, Feature& feature) const;
};


#endif //NATIVEIMAGEMERGE_DEBUGGER_H
