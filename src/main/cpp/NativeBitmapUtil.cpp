//
// Created by vance on 2016/8/22.
//

#include "NativeBitmapUtil.h"

#define CONFIG_CLASS mpEnv->FindClass("android/graphics/Bitmap$Config")
#define CONFIG_FIELD_ARGB_8888(clazz) mpEnv->GetStaticFieldID(clazz, "ARGB_8888", "Landroid/graphics/Bitmap$Config;")

#define BITMAP_CLASS mpEnv->FindClass("android/graphics/Bitmap")
#define BITMAP_METHOD_CREATEBITMAP(clazz) mpEnv->GetStaticMethodID(clazz, "createBitmap", "([IIIIILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;")
#define BITMAP_METHOD_GET_WIDTH(clazz) mpEnv->GetMethodID(clazz, "getWidth", "()I")
#define BITMAP_METHOD_GET_HEIGHT(clazz) mpEnv->GetMethodID(clazz, "getHeight", "()I")
#define BITMAP_METHOD_GET_PIXELS(clazz) mpEnv->GetMethodID(clazz, "getPixels", "([IIIIIII)V")
#define BITMAP_METHOD_COMPRESS(clazz) mpEnv->GetMethodID(clazz, "compress", "(Landroid/graphics/Bitmap$CompressFormat;ILjava/io/OutputStream;)Z")
#define BITMAP_METHOD_RECYCLE(clazz) mpEnv->GetMethodID(clazz, "recycle", "()V")
//#define BITMAP_METHOD_SET_HAS_ALPHA(clazz) mpEnv->GetMethodID(clazz, "setHasAlpha", "(Z)V")

#define COMPRESS_FORMAT_CLASS mpEnv->FindClass("android/graphics/Bitmap$CompressFormat")
#define COMPRESS_FORMAT_METHOD_PNG(clazz) mpEnv->GetStaticFieldID(clazz, "PNG", "Landroid/graphics/Bitmap$CompressFormat;")

#define FILE_OUTPUT_STREAM_CLASS mpEnv->FindClass("java/io/FileOutputStream")
#define FILE_OUTPUT_STREAM_CONSTRUCTOR(clazz) mpEnv->GetMethodID(clazz, "<init>", "(Ljava/lang/String;)V")

#define BITMAP_FACTORY_CLASS mpEnv->FindClass("android/graphics/BitmapFactory")
#define BITMAP_FACTORY_METHOD_DECODE_FILE(clazz) mpEnv->GetMethodID(clazz, "decodeFile", "(Ljava/lang/String;)Landroid/graphics/Bitmap;")

NativeBitmapUtil::NativeBitmapUtil(JNIEnv* env) : mpEnv(env) {
}

void NativeBitmapUtil::save(IN const char* path, IN const NativeBitmap& bmp, bool hasAlpha) {
    // translate NativeBitmap to Bitmap
    jobject bitmap;
    if (hasAlpha) {
        bitmap = toBitmap(bmp);
    } else {
        NativeBitmap tmpBmp;
        NativeBitmap::create(bmp, tmpBmp);
        jint* p = tmpBmp.getPixels();

        for (int i=0; i<tmpBmp.getPixelsCount(); i++) {
            p[i] |= 0xFF000000;
        }

        bitmap = toBitmap(tmpBmp);
        tmpBmp.release();
    }

    // get compress format PNG
    jclass clsCompressFormat = COMPRESS_FORMAT_CLASS;
    jfieldID fieldPNG = COMPRESS_FORMAT_METHOD_PNG(clsCompressFormat);
    jobject objPNG = mpEnv->GetStaticObjectField(clsCompressFormat, fieldPNG);
    mpEnv->DeleteLocalRef(clsCompressFormat);

    // path to String
    jstring strPath = mpEnv->NewStringUTF(path);

    // output stream
    jclass clsFileOutputStream = FILE_OUTPUT_STREAM_CLASS;
    jmethodID constructorOutputStream = FILE_OUTPUT_STREAM_CONSTRUCTOR(clsFileOutputStream);
    jobject objFileOutputStream = mpEnv->NewObject(clsFileOutputStream, constructorOutputStream, strPath);
    mpEnv->DeleteLocalRef(clsFileOutputStream);

    // compress bitmap
    jclass clsBitmap = BITMAP_CLASS;
    jmethodID methodCompress = BITMAP_METHOD_COMPRESS(clsBitmap);
    mpEnv->CallBooleanMethod(bitmap, methodCompress, objPNG, 100, objFileOutputStream);

    // recycle bitmap
    jmethodID methodRecycle = BITMAP_METHOD_RECYCLE(clsBitmap);
    mpEnv->CallVoidMethod(bitmap, methodRecycle);
    mpEnv->DeleteLocalRef(clsBitmap);

    // clean up
    mpEnv->DeleteLocalRef(bitmap);
    mpEnv->DeleteLocalRef(objPNG);
    mpEnv->DeleteLocalRef(strPath);
    mpEnv->DeleteLocalRef(objFileOutputStream);
}

void NativeBitmapUtil::load(IN const char* path, OUT NativeBitmap& bmp) {
    // path to String
    jstring strPath = mpEnv->NewStringUTF(path);

    // decode file
    jclass clsBitmapFactory = BITMAP_FACTORY_CLASS;
    jmethodID methodDecodeFile = BITMAP_FACTORY_METHOD_DECODE_FILE(clsBitmapFactory);
    jobject objBitmap = mpEnv->CallStaticObjectMethod(clsBitmapFactory, methodDecodeFile, strPath);
    mpEnv->DeleteLocalRef(clsBitmapFactory);

    // Bitmap to NativeBitmap
    fromBitmap(objBitmap, bmp);

    // clean up
    mpEnv->DeleteLocalRef(strPath);
    mpEnv->DeleteLocalRef(objBitmap);
}

jobject NativeBitmapUtil::toBitmap(IN const NativeBitmap& bmp) {
    // get ARGB_8888
    jclass clsConfig = CONFIG_CLASS;
    jfieldID fieldARGB_8888 = CONFIG_FIELD_ARGB_8888(clsConfig);
    jobject objARGB_8888 = mpEnv->GetStaticObjectField(clsConfig, fieldARGB_8888);

    // prepare pixel array
    jint pixelsCount = bmp.getPixelsCount();
    jintArray pixels = mpEnv->NewIntArray(pixelsCount);
    mpEnv->SetIntArrayRegion(pixels, 0, pixelsCount, bmp.getPixels());

    // create bitmap
    jclass clsBitmap = BITMAP_CLASS;
    jmethodID methodCreateBitmap = BITMAP_METHOD_CREATEBITMAP(clsBitmap);
    jobject objBitmap = mpEnv->CallStaticObjectMethod(clsBitmap, methodCreateBitmap, pixels, 0, bmp.getWidth(), bmp.getWidth(), bmp.getHeight(), objARGB_8888);

    // clean up
    mpEnv->DeleteLocalRef(clsConfig);
    mpEnv->DeleteLocalRef(objARGB_8888);
    mpEnv->DeleteLocalRef(pixels);
    mpEnv->DeleteLocalRef(clsBitmap);

    return objBitmap;
}

void NativeBitmapUtil::fromBitmap(jobject bitmap, OUT NativeBitmap& bmp) {
    jint width;
    jint height;
    jsize pixelsCount;

    jclass clsBitmap = BITMAP_CLASS;

    // get width
    jmethodID methodGetWidth = BITMAP_METHOD_GET_WIDTH(clsBitmap);
    width = mpEnv->CallIntMethod(bitmap, methodGetWidth);

    // get height
    jmethodID methodGetHeight =BITMAP_METHOD_GET_HEIGHT(clsBitmap);
    height = mpEnv->CallIntMethod(bitmap, methodGetHeight);

    // pixelsCount
    pixelsCount = width * height;

    // make int array
    jintArray pixels = mpEnv->NewIntArray(pixelsCount);

    // get pixels
    jmethodID methodGetPixels = BITMAP_METHOD_GET_PIXELS(clsBitmap);
    mpEnv->CallVoidMethod(bitmap, methodGetPixels, pixels, 0, width, 0, 0, width, height);

    // copy pixels to NativeBitmap
    NativeBitmap::create(width, height, bmp);
    mpEnv->GetIntArrayRegion(pixels, 0, pixelsCount, bmp.getPixels());

    // delete reference
    mpEnv->DeleteLocalRef(clsBitmap);
    mpEnv->DeleteLocalRef(pixels);
}