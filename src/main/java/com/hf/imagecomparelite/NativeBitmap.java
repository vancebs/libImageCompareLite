package com.hf.imagecomparelite;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;

/**
 * NativeBitmap
 * Created by vance on 2016/8/4.
 */
public class NativeBitmap {
    private static final String TAG = NativeBitmap.class.getName();

    private long mNativePtr = 0;

    /*package*/ NativeBitmap(long ptr) {
        mNativePtr = ptr;
    }

    /**
     * Create NativeBitmap
     * @param width width
     * @param height height
     * @return NativeBitmap
     */
    public static NativeBitmap create(int width, int height) {
        return new NativeBitmap(nativeCreate(width, height));
    }

    /**
     * Create NativeBitmap
     * @param src src NativeBitmap
     * @return NativeBitmap
     */
    public static NativeBitmap create(NativeBitmap src) {
        return new NativeBitmap(nativeCreate(src.mNativePtr));
    }

    /**
     * Create NativeBitmap
     * @param src src NativeBitmap
     * @param startRow start row index
     * @param endRow end row index
     * @return NativeBitmap
     */
    public static NativeBitmap create(NativeBitmap src, int startRow, int endRow) {
        return new NativeBitmap(nativeCreate(src.mNativePtr, startRow, endRow));
    }

    /**
     * Create a uninitialized NativeBitmap
     * @return NativeBitmap
     */
    public static NativeBitmap createUninitialized() {
        return new NativeBitmap(nativeCreateUninitialized());
    }

    /**
     * Create NativeBitmap from Bitmap
     * @param bitmap bitmap
     * @return NativeBitmap
     */
    public static NativeBitmap create(Bitmap bitmap) {
        if(bitmap == null || bitmap.isRecycled()){
            return null;
        }
        return new NativeBitmap(nativeFromBitmap(bitmap));
    }

    /**
     * Create from file path
     * @param path path of the file
     * @return NativeBitmap
     */
    public static NativeBitmap create(String path) {
        if (path == null || path.trim().isEmpty()) {
            return null;
        }

        Bitmap bmp = BitmapFactory.decodeFile(path);
        NativeBitmap nb = new NativeBitmap(nativeFromBitmap(bmp));
        bmp.recycle();
        return nb;
    }

    /**
     * Get the native pointer
     * @return native pointer
     */
    public long getNativePtr() {
        return mNativePtr;
    }

    /**
     * Recycle native bitmap
     */
    public void recycle() {
        if (mNativePtr != 0) {
            nativeRecycle(mNativePtr);
            mNativePtr = 0;
        }
    }

    public void save(String path) {
        if (path == null || path.trim().isEmpty()) {
            return;
        }

        save(new File(path));
    }

    public void save(File file) {
        if (file == null) {
            return;
        }

        Bitmap bmp = toBitmap();
        try {
            bmp.compress(Bitmap.CompressFormat.PNG, 100, new FileOutputStream(file));
        } catch (FileNotFoundException e) {
            Log.e(TAG, "save bitmap failed.", e);
        } finally {
            if (bmp != null) {
                bmp.recycle();
            }
        }
    }

    /**
     * Convert NativeBitmap to Bitmap
     * @return Bitmap
     */
    public Bitmap toBitmap() {
        return (Bitmap) nativeToBitmap(mNativePtr);
    }

    /**
     * Get NativeBitmap width
     * @return width
     */
    @SuppressWarnings("unused")
    public int getWidth() {
        return nativeGetWidth(mNativePtr);
    }

    /**
     * Get NativeBitmap height
     * @return height
     */
    @SuppressWarnings("unused")
    public int getHeight() {
        return nativeGetHeight(mNativePtr);
    }

    private static native long nativeCreate(int width, int height);
    private static native long nativeCreate(long srcPtr);
    private static native long nativeCreate(long srcPtr, int startRow, int endRow);
    private static native long nativeCreateUninitialized();
    private static native void nativeRecycle(long ptr);
    private static native Object nativeToBitmap(long ptr);
    private static native long nativeFromBitmap(Object bitmap);

    private static native int nativeGetWidth(long ptr);
    private static native int nativeGetHeight(long ptr);

    static {
        System.loadLibrary("libImageCompareLite");
    }
}
