package com.hf.imagecomparelite;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Decoder
 */
public class BitmapDecoder {
    public static NativeBitmap readImage(String path) {
        Bitmap bitmap = null;
        try {
            bitmap = BitmapFactory.decodeFile(path);
            return NativeBitmap.create(bitmap);
        } finally {
            if (bitmap != null) {
                bitmap.recycle();
            }
        }
    }

    public static NativeBitmap readImage(Bitmap bitmap) {
        return NativeBitmap.create(bitmap);
    }

    public static void writeImage(String path, NativeBitmap bmp) {
        FileOutputStream fos = null;
        Bitmap bitmap = null;
        try {
            File file = new File(path);
            fos = new FileOutputStream(file);

            bitmap = bmp.toBitmap();
            bitmap.compress(Bitmap.CompressFormat.PNG, 100, fos);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } finally {
            if (bitmap != null) {
                bitmap.recycle();
            }
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    @SuppressWarnings("unused")
    public static void writeImage(String path, Bitmap bmp) {
        FileOutputStream fos = null;
        try {
            File file = new File(path);
            fos = new FileOutputStream(file);
            bmp.compress(Bitmap.CompressFormat.PNG, 100, fos);
            android.util.Log.i("==MyTest==", "writeImage : path = " + path);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
