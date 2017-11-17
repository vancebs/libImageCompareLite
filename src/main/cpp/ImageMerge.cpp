//
// Created by vance on 2016/8/5.
//

#include "ImageMerge.h"
#include "FeatureCompare.h"
#include "HashCompare.h"

#define DEBUG_IMG_TRIMMED1     "trimmed1.png"
#define DEBUG_IMG_TRIMMED2     "trimmed2.png"

jint ImageMerge::compareByFeature(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN const jint scrollDirection, OUT jint& trimTop, OUT jint& trimBottom, IN Debugger* pDebugger) {
    TRACE_INIT(pDebugger);

    // trim
    TRACE_BEGIN(pDebugger);
    NativeBitmap trimmed1, trimmed2;
    bool result = trim(bmp1, bmp2, 100, trimmed1, trimmed2, trimTop, trimBottom);
    TRACE_END(pDebugger, "trim");
    if (result) {
        PRINT_IMAGE(pDebugger, DEBUG_IMG_TRIMMED1, trimmed1);
        PRINT_IMAGE(pDebugger, DEBUG_IMG_TRIMMED2, trimmed2);
    } else {
    	return 0;
    }

    // generate distance
    FeatureCompare compare;
    jint distance = compare.compare(trimmed1, trimmed2, scrollDirection, pDebugger);
    //jint distance = compare.compareWithMultiThread(trimmed1, trimmed2, scrollDirection, pDebugger);
    trimmed1.release();
    trimmed2.release();
    LOGI("==MyTest==", "distance: %d", distance);

    TRACE_FINISH(pDebugger, "finished");

    return distance;
}

jint ImageMerge::compareByHash(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, OUT jint& trimTop, OUT jint& trimBottom, IN Debugger* pDebugger) {
    TRACE_INIT(pDebugger);

    // trim
    TRACE_BEGIN(pDebugger);
    NativeBitmap trimmed1, trimmed2;
    bool result = trim(bmp1, bmp2, 100, trimmed1, trimmed2, trimTop, trimBottom);
    TRACE_END(pDebugger, "trim");
    if (result) {
        PRINT_IMAGE(pDebugger, DEBUG_IMG_TRIMMED1, trimmed1);
        PRINT_IMAGE(pDebugger, DEBUG_IMG_TRIMMED2, trimmed2);
    } else {
    	return 0;
    }

    // compare
    HashCompare compare;
    jint distance = compare.compare(bmp1, bmp2, pDebugger);
    trimmed1.release();
    trimmed2.release();
    LOGI("==MyTest==", "distance: %d", distance);

    TRACE_FINISH(pDebugger, "finished");

    return distance;
}

void ImageMerge::mergeBitmap(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN const jint trimTop, IN const jint trimBottom, IN const jint distance, OUT NativeBitmap& merged) {
    if (merged.isInitialized()) {
        LOGW(DEBUG_TAG, "ImageMerge::mergeBitmap()# Could not output bitmap to initialized NativeBitmap");
        return;
    }

    if (!bmp1.isInitialized() || !bmp2.isInitialized()) {
        LOGW(DEBUG_TAG, "ImageMerge::mergeBitmap()# Could not read from uninitialized NativeBitmap. bmp1: %d, bmp2: %d.", bmp1.isInitialized(), bmp2.isInitialized());
        return;
    }

    // generate start row & end row
    jint width = bmp1.getWidth();
    jint height1 = bmp1.getHeight();
    jint height2 = bmp2.getHeight();
    jint newHeight = height1 + distance;
    jint sameHeight = height2 - trimBottom - trimTop - distance;

    //
    jint newY1 = 0;
    jint newHeight1 = height1 - trimBottom - (sameHeight / 2);
    jint newY2 = trimTop + sameHeight - (sameHeight / 2);
    jint newHeight2 = height2 - newY2;

    //LOGI("==MyTest==", "trimTop: %d, trimBottom: %d, newY1: %d, newHeight1: %d, newY2: %d, newHeight2: %d", trimTop, trimBottom, newY1, newHeight1, newY2, newHeight2);

    // merge to out
    NativeBitmap::create(width, newHeight, merged);
    jint* pOut = merged.getPixels();
    jint* pBmp1 = bmp1.getPixels();
    jint* pBmp2 = bmp2.getPixels();
    memcpy( pOut,
            pBmp1 + (newY1 * width),
            newHeight1 * width * sizeof(jint));
    memcpy( pOut + (newHeight1 * width),
            pBmp2 + (newY2 * width),
            newHeight2 * width * sizeof(jint));
}

void ImageMerge::mergeBitmap2(IN const NativeBitmap& bmp1, IN jint startY1, IN jint endY1, IN const NativeBitmap& bmp2, IN jint startY2, IN jint endY2, OUT NativeBitmap& merged) {
    if (merged.isInitialized()) {
        LOGW(DEBUG_TAG, "ImageMerge::mergeBitmap2()# Could not output bitmap to initialized NativeBitmap");
        return;
    }

    if (!bmp1.isInitialized() || !bmp2.isInitialized()) {
        LOGW(DEBUG_TAG, "ImageMerge::mergeBitmap2()# Could not read from uninitialized NativeBitmap. bmp1: %d, bmp2: %d.", bmp1.isInitialized(), bmp2.isInitialized());
        return;
    }

    jint width = bmp1.getWidth();
    jint y1 = startY1;
    jint h1 = endY1 - startY1 + 1;
    jint y2 = startY2;
    jint h2 = endY2 - startY2 + 1;
    jint newHeight = h1 + h2;
    jint offset1 = y1 * width;
    jint length1 = h1 * width;
    jint offset2 = y2 * width;
    jint length2 = h2 * width;

    // create merged bitmap
    NativeBitmap::create(width, newHeight, merged);

    // copy pixels
    jint* pMergedPixels = merged.getPixels();
    jint* pBmp1Pixels = bmp1.getPixels();
    jint* pBmp2Pixels = bmp2.getPixels();
    memcpy(pMergedPixels, pBmp1Pixels + offset1, length1 * sizeof(jint));
    memcpy(pMergedPixels + length1, pBmp2Pixels + offset2, length2 * sizeof(jint));
}

void ImageMerge::clipBitmap(IN const NativeBitmap& src, IN jint startY, IN jint endY, OUT NativeBitmap& bmp) {
    if (bmp.isInitialized()) {
        LOGW(DEBUG_TAG, "ImageMerge::clipBitmapTop()# Could not output bitmap to initialized NativeBitmap");
        return;
    }

    if (!src.isInitialized()) {
        LOGW(DEBUG_TAG, "ImageMerge::clipBitmapTop()# Could not read uninitialized NativeBitmap");
        return;
    }

    jint width = src.getWidth();
    jint newHeight = endY - startY + 1;
    jint offset = startY * width;
    jint length = newHeight * width;
    NativeBitmap::create(width, newHeight, bmp);
    memcpy(bmp.getPixels(), src.getPixels() + offset, length * sizeof(jint));
}

bool ImageMerge::trim(IN const NativeBitmap& bmp1, IN const NativeBitmap& bmp2, IN const jint topOffset, OUT NativeBitmap& trimmed1, OUT NativeBitmap& trimmed2, OUT jint& trimTop, OUT jint& trimBottom) {
    if (trimmed1.isInitialized() || trimmed2.isInitialized()) {
        LOGW(DEBUG_TAG, "ImageMerge::trim()# Could not output bitmap to initialized NativeBitmap. trimmed1: %d, trimmed2: %d", trimmed1.isInitialized(), trimmed2.isInitialized());
        return false;
    }

    if (!bmp1.isInitialized() || !bmp2.isInitialized()) {
        LOGW(DEBUG_TAG, "ImageMerge::trim()# Could not read uninitialized NativeBitmap. bmp1: %d, bmp2: %d", bmp1.isInitialized(), bmp2.isInitialized());
        return false;
    }

    jint width = bmp1.getWidth();
    jint height = bmp1.getHeight();
    jint pixelsCount = bmp1.getPixelsCount();

    // get pixels
    jint* pPixels1 = bmp1.getPixels();
    jint* pPixels2 = bmp2.getPixels();

    // find the first different index from top & bottom
    jint topIndex = topOffset * width;
    jint bottomIndex = pixelsCount - 1;
    while (topIndex < pixelsCount && pPixels1[topIndex] == pPixels2[topIndex]) {
        topIndex ++;
    }
    while (bottomIndex >= 0 && pPixels1[bottomIndex] == pPixels2[bottomIndex]) {
        bottomIndex --;
    }

    // get row index that should be
    jint trimmedTopRowIndex = topIndex / width;
    jint trimmedBottomRowIndex = bottomIndex / width;

    if (trimmedTopRowIndex > trimmedBottomRowIndex) {
        return false;
    }

    //LOGI("==MyTest==", "bmp1: %d, bmp2: %d, trimmedTopRowIndex: %d, trimmedBottomRowIndex: %d, trimmed1: %d, trimmed2: %d", &bmp1, &bmp2, trimmedTopRowIndex, trimmedBottomRowIndex, &trimmed1, &trimmed2);
    // create trimmed bitmap
    NativeBitmap::create(bmp1, trimmedTopRowIndex, trimmedBottomRowIndex, trimmed1);
    NativeBitmap::create(bmp2, trimmedTopRowIndex, trimmedBottomRowIndex, trimmed2);

    // generate the number of rows that should be trimmed from top & bottom
    trimTop = trimmedTopRowIndex;
    trimBottom = height - trimmedBottomRowIndex - 1;

    return true;
}
