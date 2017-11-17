//
// Created by vance on 2016/8/23.
//

#include "Debugger.h"
#include "NativeBitmapUtil.h"

#define COLOR_FEATURE 0xFFFF0000
#define COLOR_MASKED 0xFF000000

#define PERFORMANCE_TAG "==Performance=="

Debugger::Debugger(JNIEnv* env) :
        mBitmapDebugEnabled(false),
        mPerformanceDebugEnabled(false)
{
#ifdef _DEBUG_PERFORMANCE
    enablePerformanceDebug(true);
#endif

#ifdef _DEBUG_IMAGE
    enableBitmapDebug(true, env, IMAGE_DEBUG_DIR);
#endif
}

void Debugger::enableBitmapDebug(bool enable, JNIEnv* env, const char* dirPath) {
    mBitmapDebugEnabled = enable;
    mpEnv = env;
    mDir = dirPath;
    if (mBitmapDebugEnabled) {
        mkdirs(mDir);
    }
}

void Debugger::enablePerformanceDebug(bool enable) {
    mPerformanceDebugEnabled = enable;
}

bool Debugger::isBitmapDebugEnabled() const {
    return mBitmapDebugEnabled;
}

bool Debugger::isPerformanceDebugEnabled() const {
    return mPerformanceDebugEnabled;
}

void Debugger::printBitmap(IN const char* fileName, IN const NativeBitmap& bmp, bool hasAlpha) const {
    NativeBitmapUtil util(mpEnv);

    char path[256];
    path[0] = 0;

    strcat(path, mDir);
    strcat(path, "/");
    strcat(path, fileName);

    util.save(path, bmp, hasAlpha);
}

void Debugger::printBitmap(IN const char* fileName, IN const NativeBitmap& bmp, FeatureList& featureList, bool hasAlpha) const {
    NativeBitmap featureBmp;
    NativeBitmap::create(bmp, featureBmp);
    for (FeatureList::iterator i=featureList.begin(); i!=featureList.end(); i++) {
        drawFeature(featureBmp, *i);
    }
    printBitmap(fileName, featureBmp, hasAlpha);
}

void Debugger::printHashBitmap(IN const char* fileName, IN const NativeBitmap& bmp, const jint* hash) const {
    NativeBitmap hashBmp;
    NativeBitmap::create(bmp, hashBmp);
    jint* pPixels = hashBmp.getPixels();
    jint height = hashBmp.getHeight();
    jint width = hashBmp.getWidth();
    for (jint i=0,index=20; i<height; i++, index+=width) {
        pPixels[index] = hash[i] | 0xFF000000;
        pPixels[index + 1] = pPixels[index];
        pPixels[index + 2] = pPixels[index];
        pPixels[index + 3] = pPixels[index];
    }
    printBitmap(fileName, hashBmp);
}

void Debugger::printHashMaskBitmap(IN const char* fileName, IN const NativeBitmap& bmp, const jint* hashMask) const {
    NativeBitmap hashBmp;
    NativeBitmap::create(bmp, hashBmp);
    jint* pPixels = hashBmp.getPixels();
    jint height = hashBmp.getHeight();
    jint width = hashBmp.getWidth();
    for (jint i=0,index=20; i<height; i++, index+=width) {
        if (hashMask[i] != 0) {
            pPixels[index] = COLOR_FEATURE;
            pPixels[index + 1] = COLOR_FEATURE;
            pPixels[index + 2] = COLOR_FEATURE;
            pPixels[index + 3] = COLOR_FEATURE;
        } else {
            pPixels[index] = COLOR_MASKED;
            pPixels[index + 1] = COLOR_MASKED;
            pPixels[index + 2] = COLOR_MASKED;
            pPixels[index + 3] = COLOR_MASKED;
        }
        //pPixels[index] = hashMask[i] | 0xFF000000;
    }
    printBitmap(fileName, hashBmp);
}

void Debugger::traceInit() {
    mTraceTotalTime = 0;
}

void Debugger::traceBegin() {
    mTraceBegin = clock();
}

void Debugger::traceEnd() {
    mTraceEnd = clock();
    mTraceTotalTime += (mTraceEnd - mTraceBegin);
}

void Debugger::printLastTraceTime(IN const char* tag) {
    LOGD(PERFORMANCE_TAG, "[%s]# %ldms", tag, ((mTraceEnd - mTraceBegin) / 1000));
}

void Debugger::printFinalTraceRecord(IN const char* tag) {
    LOGD(PERFORMANCE_TAG, "=> final [%s]# %ldms", tag, (mTraceTotalTime / 1000));
}

bool Debugger::mkdirs(const char* dir) {
    jstring strDir = mpEnv->NewStringUTF(dir);
    jclass clsFile = mpEnv->FindClass("java/io/File");
    jmethodID constructor = mpEnv->GetMethodID(clsFile, "<init>", "(Ljava/lang/String;)V");
    jobject objFile = mpEnv->NewObject(clsFile, constructor, strDir);

    jmethodID methodMkdirs = mpEnv->GetMethodID(clsFile, "mkdirs", "()Z");
    jboolean result = mpEnv->CallBooleanMethod(objFile, methodMkdirs);

    mpEnv->DeleteLocalRef(strDir);
    mpEnv->DeleteLocalRef(clsFile);
    mpEnv->DeleteLocalRef(objFile);

    return result;
}

void Debugger::drawFeature(NativeBitmap& bmp, Feature& feature) const {
    jint* pPixels = bmp.getPixels();
    jint width = bmp.getWidth();
    jint leftTop = XY_TO_INDEX(feature.left, feature.top, width);
    jint leftBottom = XY_TO_INDEX(feature.left, feature.bottom, width);
    jint rightTop = XY_TO_INDEX(feature.right, feature.top, width);
    jint rightBottom = XY_TO_INDEX(feature.right, feature.bottom, width);

    // draw left line
    for (jint i=leftTop; i<=leftBottom; i+=width) {
        pPixels[i] = COLOR_FEATURE;
    }

    // draw right line
    for (jint i=rightTop; i<=rightBottom; i+=width) {
        pPixels[i] = COLOR_FEATURE;
    }

    // draw top line
    for (jint i=leftTop; i<=rightTop; i++) {
        pPixels[i] = COLOR_FEATURE;
    }

    // draw bottom line
    for (jint i=leftBottom; i<=rightBottom; i++) {
        pPixels[i] = COLOR_FEATURE;
    }
}