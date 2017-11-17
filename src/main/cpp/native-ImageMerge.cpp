#include "native-ImageMerge.h"
#include "ImageMerge.h"

#define INDEX_TRIM_TOP 0
#define INDEX_TRIM_BOTTOM 1

ImageMerge sImageCompare;

/*
 * Class:     com_hf_imagecomparelite_ImageMerge
 * Method:    nativeCompareByFeature
 * Signature: (JJ[I)I
 */
JNIEXPORT jint JNICALL Java_com_hf_imagecomparelite_ImageMerge_nativeCompareByFeature(JNIEnv * env, jclass cls, jlong bmp1, jlong bmp2, jintArray trimmed) {
    if (bmp1 == 0 || bmp2 == 0) {
        return 0;
    }

    jint trimmedArray[2];
    jint distance;

    // compare
#if defined _DEBUG_IMAGE || defined _DEBUG_PERFORMANCE
    Debugger debugger(env);
    distance = sImageCompare.compareByFeature(NATIVE_BITMAP(bmp1), NATIVE_BITMAP(bmp2), trimmedArray[INDEX_TRIM_TOP], trimmedArray[INDEX_TRIM_BOTTOM], &debugger);
#else
    distance = sImageCompare.compareByFeature(NATIVE_BITMAP(bmp1), NATIVE_BITMAP(bmp2), trimmedArray[INDEX_TRIM_TOP], trimmedArray[INDEX_TRIM_BOTTOM]);
#endif

    // return trimmed top & bottom
    env->SetIntArrayRegion(trimmed, 0, 2, trimmedArray);

    return distance;
}

/*
 * Class:     com_hf_imagecomparelite_ImageMerge
 * Method:    nativeCompareByHash
 * Signature: (JJ[I)I
 */
JNIEXPORT jint JNICALL Java_com_hf_imagecomparelite_ImageMerge_nativeCompareByHash(JNIEnv * env, jclass cls, jlong bmp1, jlong bmp2, jintArray trimmed) {
    if (bmp1 == 0 || bmp2 == 0) {
        return 0;
    }

    jint trimmedArray[2];
    jint distance;

    // compare
#if defined _DEBUG_IMAGE || defined _DEBUG_PERFORMANCE
    Debugger debugger(env);
    distance = sImageCompare.compareByHash(NATIVE_BITMAP(bmp1), NATIVE_BITMAP(bmp2), trimmedArray[INDEX_TRIM_TOP], trimmedArray[INDEX_TRIM_BOTTOM], &debugger);
#else
    distance = sImageCompare.compareByHash(NATIVE_BITMAP(bmp1), NATIVE_BITMAP(bmp2), trimmedArray[INDEX_TRIM_TOP], trimmedArray[INDEX_TRIM_BOTTOM]);
#endif

    // return trimmed top & bottom
    env->SetIntArrayRegion(trimmed, 0, 2, trimmedArray);

    return distance;
}

/*
 * Class:     com_hf_imagecomparelite_ImageMerge
 * Method:    nativeMerge
 * Signature: (JJIII)J
 */
JNIEXPORT jlong JNICALL Java_com_hf_imagecomparelite_ImageMerge_nativeMerge(JNIEnv * env, jclass cls, jlong bmp1, jlong bmp2, jint trimTop, jint trimBottom, jint distance) {
    if (bmp1 == 0 || bmp2 == 0) {
        return 0;
    }

    NativeBitmap* pMergedBmp = new NativeBitmap();

    // merge
    sImageCompare.mergeBitmap(NATIVE_BITMAP(bmp1), NATIVE_BITMAP(bmp2), trimTop, trimBottom, distance, *pMergedBmp);

    return (jlong) pMergedBmp;
}

JNIEXPORT jlong JNICALL Java_com_hf_imagecomparelite_ImageMerge_nativeMerge2(JNIEnv * env, jclass cls, jlong bmp1, jint startY1, jint endY1, jlong bmp2, jint startY2, jint endY2) {
    if (bmp1 == 0 || bmp2 == 0) {
        return 0;
    }

    NativeBitmap* pMergedBmp = new NativeBitmap();

    // merge
    sImageCompare.mergeBitmap2(NATIVE_BITMAP(bmp1), startY1, endY1, NATIVE_BITMAP(bmp2), startY2, endY2, *pMergedBmp);

    return (jlong) pMergedBmp;
}

/*
 * Class:     com_hf_imagecomparelite_ImageMerge
 * Method:    nativeClip
 * Signature: (JII)J
 */
JNIEXPORT jlong JNICALL Java_com_hf_imagecomparelite_ImageMerge_nativeClip(JNIEnv * env, jclass cls, jlong ptr, jint startY, jint endY) {
    NativeBitmap* pOutBmp = new NativeBitmap;
    sImageCompare.clipBitmap(NATIVE_BITMAP(ptr), startY, endY, *pOutBmp);
    return (long) pOutBmp;
}
