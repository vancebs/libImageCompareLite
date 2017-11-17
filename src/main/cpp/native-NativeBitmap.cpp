#include "native-NativeBitmap.h"
#include "NativeBitmap.h"
#include "NativeBitmapUtil.h"

/*
 * Class:     com_tct_screenshotedit_featurecompare_nativeway_NativeBitmap
 * Method:    nativeCreate
 * Signature: (II)J
 */
JNIEXPORT jlong JNICALL Java_com_hf_imagecomparelite_NativeBitmap_nativeCreate__II(JNIEnv* env, jclass cls, jint width, jint height) {
    return (jlong)(& NativeBitmap::create(width, height, *(new NativeBitmap())));
}

/*
 * Class:     com_hf_imagecomparelite_NativeBitmap
 * Method:    nativeCreate
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_hf_imagecomparelite_NativeBitmap_nativeCreate__J(JNIEnv * env, jclass cls, jlong ptrSrc) {
    return (jlong)(& NativeBitmap::create(NATIVE_BITMAP(ptrSrc), *(new NativeBitmap())));
}

/*
 * Class:     com_hf_imagecomparelite_NativeBitmap
 * Method:    nativeCreate
 * Signature: (JII)J
 */
JNIEXPORT jlong JNICALL Java_com_hf_imagecomparelite_NativeBitmap_nativeCreate__JII(JNIEnv * env, jclass cls, jlong ptrSrc, jint startRow, jint endRow) {
    return (jlong)(& NativeBitmap::create(NATIVE_BITMAP(ptrSrc), startRow, endRow, *(new NativeBitmap())));
}

/*
 * Class:     com_hf_imagecomparelite_NativeBitmap
 * Method:    nativeCreate
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_hf_imagecomparelite_NativeBitmap_nativeCreateUninitialized(JNIEnv * env, jclass cls) {
    return (jlong)(new NativeBitmap());
}

/*
 * Class:     com_tct_screenshotedit_featurecompare_nativeway_NativeBitmap
 * Method:    recycle
 * Signature: (II)J
 */
JNIEXPORT void JNICALL Java_com_hf_imagecomparelite_NativeBitmap_nativeRecycle(JNIEnv* env, jclass cls, jlong ptr) {
    if (ptr != 0) {
        NativeBitmap* pBmp = (NativeBitmap*)ptr;
        delete pBmp;
    }
}

/*
 * Class:     com_hf_imagecomparelite_NativeBitmap
 * Method:    toBitmap
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_hf_imagecomparelite_NativeBitmap_nativeToBitmap(JNIEnv* env, jclass cls, jlong ptr) {
    NativeBitmapUtil util(env);

    const NativeBitmap& bmp = NATIVE_BITMAP(ptr);
    return util.toBitmap(bmp);
}

/*
 * Class:     com_hf_imagecomparelite_NativeBitmap
 * Method:    fromBitmap
 * Signature: (Ljava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL Java_com_hf_imagecomparelite_NativeBitmap_nativeFromBitmap(JNIEnv * env, jclass cls, jobject obj) {
    NativeBitmapUtil util(env);

    NativeBitmap* pBmp = new NativeBitmap();
    util.fromBitmap(obj, *pBmp);

    return (jlong) pBmp;
}

/*
 * Class:     com_hf_imagecomparelite_NativeBitmap
 * Method:    nativeGetWidth
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_hf_imagecomparelite_NativeBitmap_nativeGetWidth(JNIEnv * env, jclass cls, jlong ptr) {
    return NATIVE_BITMAP(ptr).getWidth();
}

/*
 * Class:     com_hf_imagecomparelite_NativeBitmap
 * Method:    nativeGetHeight
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_hf_imagecomparelite_NativeBitmap_nativeGetHeight(JNIEnv * env, jclass cls, jlong ptr) {
    return NATIVE_BITMAP(ptr).getHeight();
}
