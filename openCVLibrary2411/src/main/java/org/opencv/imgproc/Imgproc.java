
//
// This file is auto-generated. Please don't modify it!
//
package org.opencv.imgproc;

import org.opencv.core.Mat;

public class Imgproc {

    public static final int COLOR_YUV2RGBA_NV21 = 96;

    public static void cvtColor(Mat src, Mat dst, int code, int dstCn)
    {
        cvtColor_0(src.nativeObj, dst.nativeObj, code, dstCn);
        return;
    }

    // C++:  void cvtColor(Mat src, Mat& dst, int code, int dstCn = 0)
    private static native void cvtColor_0(long src_nativeObj, long dst_nativeObj, int code, int dstCn);
}
