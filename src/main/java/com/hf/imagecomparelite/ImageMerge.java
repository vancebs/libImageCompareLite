package com.hf.imagecomparelite;

/**
 * Image compare
 * Created by vance on 2016/8/5.
 */
public class ImageMerge {
    public static final int INDEX_TRIM_TOP = 0;
    public static final int INDEX_TRIM_BOTTOM = 1;

    /**
     * Compare two bitmap by feature and merge
     * @param bmp1 bitmap 1
     * @param bmp2 bitmap 2
     * @param trimmed trimTop & trimBottom. (Distance from top & bottom) see: {@link #INDEX_TRIM_TOP}, {@link #INDEX_TRIM_BOTTOM}
     * @return distance between two bitmap
     */
    public int compareByFeature(NativeBitmap bmp1, NativeBitmap bmp2, int[] trimmed) {
        if(bmp1 == null || bmp2 == null){
            return 0;
        }
        return nativeCompareByFeature(bmp1.getNativePtr(), bmp2.getNativePtr(), trimmed);
    }

    /**
     * Compare two bitmap by feature and merge
     * @param bmp1 bitmap 1
     * @param bmp2 bitmap 2
     * @param trimmed trimTop & trimBottom. (Distance from top & bottom) see: {@link #INDEX_TRIM_TOP}, {@link #INDEX_TRIM_BOTTOM}
     * @return distance between two bitmap
     */
    public int compareByHash(NativeBitmap bmp1, NativeBitmap bmp2, int[] trimmed) {
        return nativeCompareByHash(bmp1.getNativePtr(), bmp2.getNativePtr(), trimmed);
    }

    /**
     * Compare two bitmap by feature and merge
     * @param bmp1 bitmap 1
     * @param bmp2 bitmap 2
     * @param trimTop trimTop. Distance from top
     * @param trimBottom trimBottom. Distance from bottom
     * @param distance distance
     * @return distance between two bitmap
     */
    public NativeBitmap merge(NativeBitmap bmp1, NativeBitmap bmp2, int trimTop, int trimBottom, int distance) {
        long ptr = nativeMerge(bmp1.getNativePtr(), bmp2.getNativePtr(), trimTop, trimBottom, distance);
        return (ptr == 0) ? null : new NativeBitmap(ptr);
    }

    /**
     * Compare two bitmap by feature and merge
     * @param bmp1 bitmap 1
     * @param bmp2 bitmap 2
     * @param trimmed trimTop & trimBottom. (Distance from top & bottom) see: {@link #INDEX_TRIM_TOP}, {@link #INDEX_TRIM_BOTTOM}
     * @param distance distance
     * @return distance between two bitmap
     */
    public NativeBitmap merge(NativeBitmap bmp1, NativeBitmap bmp2, int[] trimmed, int distance) {
        long ptr = nativeMerge(bmp1.getNativePtr(), bmp2.getNativePtr(), trimmed[INDEX_TRIM_TOP], trimmed[INDEX_TRIM_BOTTOM], distance);
        return (ptr == 0) ? null : new NativeBitmap(ptr);
    }

    /**
     * Merge two NativeBitmap
     * @param bmp1 bitmap1
     * @param startY1 start Y 1
     * @param endY1 end Y 1
     * @param bmp2 bitmap2
     * @param startY2 start Y 2
     * @param endY2 end Y 2
     * @return merged bitmap
     */
    public NativeBitmap merge2(NativeBitmap bmp1, int startY1, int endY1, NativeBitmap bmp2, int startY2, int endY2) {
        long ptr = nativeMerge2(bmp1.getNativePtr(), startY1, endY1, bmp2.getNativePtr(), startY2, endY2);
        return (ptr == 0) ? null : new NativeBitmap(ptr);
    }

    /**
     * Clip Bitmap from top
     * @param bmp bitmap
     * @param startY clip start Y
     * @param endY clip end Y
     * @return clipped bitmap
     */
    public NativeBitmap clip(NativeBitmap bmp, int startY, int endY) {
        return new NativeBitmap(nativeClip(bmp.getNativePtr(), startY, endY));
    }

    /**
     * Merge two bitmaps by feature compare
     * @param bmpPtr1 bitmap 1
     * @param bmpPtr2 bitmap 2
     * @param trimmed trimTop & trimBottom. see: {@link #INDEX_TRIM_TOP}, {@link #INDEX_TRIM_BOTTOM}
     * @return distance
     */
    private static native int nativeCompareByFeature(long bmpPtr1, long bmpPtr2, /* out */ int[] trimmed);

    /**
     * Merge two bitmaps by hash compare
     * @param bmpPtr1 bitmap 1
     * @param bmpPtr2 bitmap 2
     * @param trimmed trimTop & trimBottom. see: {@link #INDEX_TRIM_TOP}, {@link #INDEX_TRIM_BOTTOM}
     * @return distance
     */
    private static native int nativeCompareByHash(long bmpPtr1, long bmpPtr2, /* out */ int[] trimmed);

    /**
     * Merge two NativeBitmap
     * @param bmpPtr1 bitmap 1
     * @param bmpPtr2 bitmap 2
     * @param trimTop trimTop
     * @param trimBottom trimBottom
     * @param distance distance
     * @return 0: merge failed. Otherwise: merged bitmap
     */
    private static native long nativeMerge(long bmpPtr1, long bmpPtr2, int trimTop, int trimBottom, int distance);

    /**
     * Merge two NativeBitmap
     * @param bmp1 bitmap1
     * @param startY1 start Y 1
     * @param endY1 end Y 1
     * @param bmp2 bitmap2
     * @param startY2 start Y 2
     * @param endY2 end Y 2
     * @return merged bitmap
     */
    private static native long nativeMerge2(long bmp1, int startY1, int endY1, long bmp2, int startY2, int endY2);

    /**
     * Clip bitmap from top
     * @param ptr bitmap
     * @param startY clip start Y
     * @param endY clip end Y
     * @return clipped bitmap
     */
    private static native long nativeClip(long ptr, int startY, int endY);
}
