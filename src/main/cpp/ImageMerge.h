//
// Created by vance on 2016/8/5.
//

#ifndef NATIVEIMAGEMERGE_IMAGECOMPARE_H
#define NATIVEIMAGEMERGE_IMAGECOMPARE_H

#include "inc.h"
#include "NativeBitmap.h"
#include "Debugger.h"


class ImageMerge {
public:
    jint compareByFeature(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, OUT jint& trimTop, OUT jint& trimBottom, IN Debugger* pDebugger = NULL);
    jint compareByHash(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, OUT jint& trimTop, OUT jint& trimBottom, IN Debugger* pDebugger = NULL);
    void mergeBitmap(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN const jint trimTop, IN const jint trimBottom, IN const jint distance, OUT NativeBitmap& merged);
    void mergeBitmap2(IN const NativeBitmap& bmp1, IN jint startY1, IN jint endY1, IN const NativeBitmap& bmp2, IN jint startY2, IN jint endY2, OUT NativeBitmap& merged);

    void clipBitmap(IN const NativeBitmap& src, IN jint startY, IN jint endY, OUT NativeBitmap& bmp);

private:
    bool trim(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN const jint topOffset, OUT NativeBitmap& trimmed1, OUT NativeBitmap& trimmed2, OUT jint& trimTop, OUT jint& trimBottom);
};


#endif //NATIVEIMAGEMERGE_IMAGECOMPARE_H
