//
// Created by vance on 2016/8/4.
//

#include "NativeBitmap.h"
#include <string.h>

NativeBitmap::NativeBitmap() :
    mWidth(0),
    mHeight(0),
    mPixelsCount(0),
    mpPixels(NULL)
{

}

NativeBitmap::~NativeBitmap() {
    if (mpPixels != NULL) {
        delete mpPixels;
    }
}

inline NativeBitmap& NativeBitmap::init(IN jint width, IN jint height) {
    mWidth = width;
    mHeight = height;
    mPixelsCount = mWidth * mHeight;
    mpPixels = new jint[mPixelsCount];
    return *this;
}

inline NativeBitmap& NativeBitmap::init(IN const NativeBitmap& src) {
    init(src.mWidth, src.mHeight);
    memcpy(mpPixels, src.mpPixels, mPixelsCount * sizeof(jint));
    return *this;
}

inline NativeBitmap& NativeBitmap::init(IN const NativeBitmap& src, IN jint startRow, IN jint endRow) {
    init(src.mWidth, endRow - startRow + 1);
    memcpy(mpPixels, src.mpPixels + (startRow * mWidth), mPixelsCount * sizeof(jint));
    return *this;
}

bool NativeBitmap::isInitialized() const {
    return mpPixels != NULL;
}

void NativeBitmap::release() {
    mWidth = 0;
    mHeight = 0;
    mPixelsCount = 0;
    if (mpPixels != NULL) {
        delete mpPixels;
        mpPixels = NULL;
    }
}

NativeBitmap& NativeBitmap::create(IN jint width, IN jint height, OUT NativeBitmap& bmp) {
    return bmp.isInitialized() ? bmp : bmp.init(width, height);
}

NativeBitmap& NativeBitmap::create(IN const NativeBitmap& src, OUT NativeBitmap& bmp) {
    return bmp.isInitialized() ? bmp : bmp.init(src);
}

NativeBitmap& NativeBitmap::create(IN const NativeBitmap& src, IN jint startRow, IN jint endRow, OUT NativeBitmap& bmp) {
    return bmp.isInitialized() ? bmp : bmp.init(src, startRow, endRow);
}

NativeBitmap* NativeBitmap::fromJlong(jlong ptr) {
    return (NativeBitmap*) ptr;
}

inline jlong NativeBitmap::toJlong(NativeBitmap* pBmp) {
    return (jlong) pBmp;
}

inline jlong NativeBitmap::toJlong() {
    return (jlong) this;
}

jint* NativeBitmap::getPixels() const {
    return mpPixels;
}

jint NativeBitmap::getPixelsCount() const {
    return mPixelsCount;
}

jint NativeBitmap::getWidth() const {
    return mWidth;
}

jint NativeBitmap::getHeight() const {
    return mHeight;
}

jint NativeBitmap::getPixel(jint x, jint y) const {
    return mpPixels[XY_TO_INDEX(x, y, mWidth)];
}

inline void NativeBitmap::setPixel(jint x, jint y, jint color) {
    mpPixels[XY_TO_INDEX(x, y, mWidth)] = color;
}

inline jint NativeBitmap::xyToIndex(jint x, jint y) const {
    return XY_TO_INDEX(x, y, mWidth);
}

jint NativeBitmap::xFromIndex(jint index) const {
    return X_FROM_INDEX(index, mWidth);
}

jint NativeBitmap::yFromIndex(jint index) const {
    return Y_FROM_INDEX(index, mWidth);
}

jint NativeBitmap::trimX(jint x) const {
    jint end = mWidth - 1;
    return TRIM(x, 0, end);
}
jint NativeBitmap::trimY(jint y) const {
    jint end = mHeight - 1;
    return TRIM(y, 0, end);
}

bool NativeBitmap::checkPoint(jint x, jint y) const {
    return (x >= 0) && (x < mWidth) && (y >= 0) && (y < mHeight);
}

bool NativeBitmap::checkColor(jint x, jint y, jint color) const {
    return mpPixels[XY_TO_INDEX(x, y, mWidth)] == color;
}