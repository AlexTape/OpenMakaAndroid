package org.opencv.core;

public class Core {

/**
 * <p>Returns the number of ticks.</p>
 *
 * <p>The function returns the number of ticks after the certain event (for
 * example, when the machine was turned on).
 * It can be used to initialize "RNG" or to measure a function execution time by
 * reading the tick count before and after the function call. See also the tick
 * frequency.</p>
 *
 * @see <a href="http://docs.opencv.org/modules/core/doc/utility_and_system_functions_and_macros.html#gettickcount">org.opencv.core.Core.getTickCount</a>
 */
    public static long getTickCount()
    {

        long retVal = getTickCount_0();

        return retVal;
    }


    //
    // C++:  double getTickFrequency()
    //

/**
 * <p>Returns the number of ticks per second.</p>
 *
 * <p>The function returns the number of ticks per second.That is, the following
 * code computes the execution time in seconds: <code></p>
 *
 * <p>// C++ code:</p>
 *
 * <p>double t = (double)getTickCount();</p>
 *
 * <p>// do something...</p>
 *
 * <p>t = ((double)getTickCount() - t)/getTickFrequency();</p>
 *
 * @see <a href="http://docs.opencv.org/modules/core/doc/utility_and_system_functions_and_macros.html#gettickfrequency">org.opencv.core.Core.getTickFrequency</a>
 */
    public static double getTickFrequency()
    {

        double retVal = getTickFrequency_0();

        return retVal;
    }


/**
 * <p>Calculates the natural logarithm of every array element.</p>
 *
 * <p>The function <code>log</code> calculates the natural logarithm of the
 * absolute value of every element of the input array:</p>
 *
 * <p><em>dst(I) = log|src(I)| if src(I) != 0 ; C otherwise</em></p>
 *
 * <p>where <code>C</code> is a large negative number (about -700 in the current
 * implementation).
 * The maximum relative error is about <code>7e-6</code> for single-precision
 * input and less than <code>1e-10</code> for double-precision input. Special
 * values (NaN, Inf) are not handled.</p>
 *
 * @param src input array.
 * @param dst output array of the same size and type as <code>src</code>.
 *
 * @see <a href="http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#log">org.opencv.core.Core.log</a>
 * @see org.opencv.core.Core#cartToPolar
 * @see org.opencv.core.Core#pow
 * @see org.opencv.core.Core#sqrt
 * @see org.opencv.core.Core#magnitude
 * @see org.opencv.core.Core#polarToCart
 * @see org.opencv.core.Core#exp
 * @see org.opencv.core.Core#phase
 */
    public static void log(Mat src, Mat dst)
    {

        log_0(src.nativeObj, dst.nativeObj);

        return;
    }

    // C++:  int64 getTickCount()
    private static native long getTickCount_0();

    // C++:  double getTickFrequency()
    private static native double getTickFrequency_0();

    // C++:  void log(Mat src, Mat& dst)
    private static native void log_0(long src_nativeObj, long dst_nativeObj);


}
