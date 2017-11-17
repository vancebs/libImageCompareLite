//
// Created by vance on 2016/8/4.
//

#ifndef NATIVEIMAGEMERGE_NATIVEBITMAP_H
#define NATIVEIMAGEMERGE_NATIVEBITMAP_H

#include <jni.h>
#include "inc.h"

#define XY_TO_INDEX(x, y, width) ((y) * (width) + (x))
#define X_FROM_INDEX(index, width) ((index) % (width))
#define Y_FROM_INDEX(index, width) ((index) / (width))
#define TRIM(v, min, max) (((v) < (min)) ? (min) : (((v) > (max)) ? (max) : (v)))

class NativeBitmap {
private:
    jint mWidth;
    jint mHeight;
    jint mPixelsCount;
    jint *mpPixels;

public:
    NativeBitmap();
    virtual ~NativeBitmap();

    bool isInitialized() const;
    void release();

    static NativeBitmap& create(IN jint width, IN jint height, OUT NativeBitmap& bmp);
    static NativeBitmap& create(IN const NativeBitmap& src, OUT NativeBitmap& bmp);
    static NativeBitmap& create(IN const NativeBitmap& src, IN jint startRow, IN jint endRow, OUT NativeBitmap& bmp);

    static NativeBitmap* fromJlong(jlong ptr);
    static jlong toJlong(NativeBitmap* pBmp);
    jlong toJlong();

    jint *getPixels() const;
    jint getPixelsCount() const;
    jint getWidth() const;
    jint getHeight() const;
    jint getPixel(jint x, jint y) const;
    void setPixel(jint x, jint y, jint color);
    jint xyToIndex(jint x, jint y) const;
    jint xFromIndex(jint index) const;
    jint yFromIndex(jint index) const;
    jint trimX(jint x) const;
    jint trimY(jint y) const;
    bool checkPoint(jint x, jint y) const;
    bool checkColor(jint x, jint y, jint color) const;

private:
    NativeBitmap& init(IN jint width, IN jint height);
    NativeBitmap& init(IN const NativeBitmap& src);
    NativeBitmap& init(IN const NativeBitmap& src, IN jint startRow, IN jint endRow);
};


#endif //NATIVEIMAGEMERGE_NATIVEBITMAP_H
