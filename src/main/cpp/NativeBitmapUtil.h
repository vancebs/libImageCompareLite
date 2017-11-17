//
// Created by vance on 2016/8/22.
//

#ifndef NATIVEIMAGEMERGE_NATIVEBITMAPUTIL_H
#define NATIVEIMAGEMERGE_NATIVEBITMAPUTIL_H

#include <jni.h>
#include "inc.h"
#include "NativeBitmap.h"

class NativeBitmapUtil {
private:
    JNIEnv* mpEnv;

public:
    NativeBitmapUtil(JNIEnv* env);

    void save(IN const char* path, IN const NativeBitmap& bmp, bool hasAlpha = true);
    void load(IN const char* path, OUT NativeBitmap& bmp);

    jobject toBitmap(IN const NativeBitmap& bmp);
    void fromBitmap(jobject bitmap, OUT NativeBitmap& bmp);
};


#endif //NATIVEIMAGEMERGE_NATIVEBITMAPUTIL_H
