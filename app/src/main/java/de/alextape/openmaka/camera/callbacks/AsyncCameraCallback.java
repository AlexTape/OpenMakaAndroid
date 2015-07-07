package de.alextape.openmaka.camera.callbacks;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.os.AsyncTask;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.ImageView;

import de.alextape.openmaka.NativeController;
import de.alextape.openmaka.camera.CameraConfig;
import de.alextape.openmaka.camera.CameraController;
import de.alextape.openmaka.camera.interfaces.CameraCallbackInterface;

/**
 * This callback delegates incoming frames to AsyncCameraTask. Furthermore this
 * class is responsible for data integrity.
 */
public class AsyncCameraCallback implements CameraCallbackInterface {

    private static final String TAG = AsyncCameraCallback.class.getSimpleName();

    private static final int STATE_OFF = 0;
    private static final int STATE_PREVIEW = 1;
    private static final int STATE_NO_CALLBACKS = 2;
    private int mState = STATE_OFF;
    private boolean mProcessInProgress = false;
    private boolean mProcessingFirstFrame = false;

    private int[] pixels;

    private int imageWidth;
    private int imageHeight;
    public Bitmap mBitmap;
    public ImageView imageView;

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        Camera mCamera = CameraController.getInstance().getCamera();
        if (mProcessInProgress) {
            mCamera.addCallbackBuffer(data);
            return;
        }
        if (data == null) {
            return;
        }

        //Log.d(TAG, "onPreviewFrame");
        if (CameraController.getInstance().getPreviewFormat() != ImageFormat.NV21) {

            Log.d(TAG, "wrong format");
        } else {

            if (data != null) {

                imageView = CameraController.getInstance().getImageView();
                imageHeight = imageView.getHeight();
                imageWidth = imageView.getWidth();

                Camera.Size test = CameraController.getInstance().getPreviewSize();

                Log.d(TAG, "ARGH imageView x=" + imageWidth + "; y=" + imageHeight);
                Log.d(TAG, "ARGH CameraController x=" + test.width + "; y=" + test.height);

//                int expectedBytes = mPreviewSize.width * mPreviewSize.height *
//                        ImageFormat.getBitsPerPixel(ImageFormat.NV21) / 8;
//
//                if (expectedBytes != data.length) {
//                    Log.e(TAG, "Mismatched size of buffer! Expected ");
//
//                    mState = STATE_NO_CALLBACKS;
//                    mCamera.setPreviewCallbackWithBuffer(null);
//                    return;
//                }
//
                mCamera.setPreviewCallbackWithBuffer(this);
                mCamera.addCallbackBuffer(data);


                new AsyncCameraTask().execute(data);
            }

        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        CameraController.getInstance().setPreviewDisplay(holder);
        CameraController.getInstance().setPreviewCallback(this);
        CameraController.getInstance().publish();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged");

        // width and height unused!

        imageView = CameraController.getInstance().getImageView();
        imageWidth = imageView.getWidth();
        imageHeight = imageView.getHeight();


        Log.d(TAG, String.format("Format=%d; width=%d; height=%d", format, imageWidth, imageHeight));
        CameraController.getInstance().reconfigure(format, width, height);

        Log.d(TAG, "ARGH surfaceChanged x=" + imageWidth + "; y=" + imageHeight);
        pixels = new int[imageWidth * imageHeight];

        mBitmap = Bitmap.createBitmap(imageWidth, imageHeight,
                Bitmap.Config.ARGB_8888);

        mBitmap.eraseColor(android.graphics.Color.GREEN);

        imageView.setImageBitmap(mBitmap);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
        CameraController.getInstance().destroy();
    }

    /**
     * This async task is resonsible for async frame processing (against e.g. ndk).
     */
    public class AsyncCameraTask extends AsyncTask<byte[], Void, Boolean> {

        private final String TAG = AsyncCameraTask.class.getSimpleName();

        private long mTiming[] = new long[50];
        private int mTimingSlot = 0;

        private byte[] data;

        @Override
        protected Boolean doInBackground(byte[]... datas) {
            // TODO Auto-generated method stub
            Log.i(TAG, "background process started");

            data = datas[0];

            long t1 = System.currentTimeMillis();

            // process data function
            NativeController.displayFunction(imageWidth, imageHeight, data, pixels);

            long t2 = System.currentTimeMillis();
            mTiming[mTimingSlot++] = t2 - t1;
            if (mTimingSlot >= mTiming.length) {
                float total = 0;
                for (int i = 0; i < mTiming.length; i++) {
                    total += (float) mTiming[i];
                }
                total /= mTiming.length;
                if (CameraConfig.BENCHMARK) {
                    Log.d(TAG, "time + " + String.valueOf(total));
                }
                mTimingSlot = 0;
            }

            if (CameraConfig.BENCHMARK) {
                Log.i(TAG, "processing time = " + String.valueOf(t2 - t1));
            }

            Camera camer = CameraController.getInstance().getCamera();
            camer.addCallbackBuffer(data);
            mProcessInProgress = false;

            return true;
        }

        @Override
        protected void onPostExecute(Boolean result) {
            //Log.i(TAG, "running onPostExecute");

            if (imageView == null) {
                Log.d(TAG, "ARGH1");
                return;
            }

            if (mBitmap == null) {
                Log.d(TAG, "ARGH2");
                return;
            }

            if (pixels == null) {
                Log.d(TAG, "ARGH3");
                return;
            }


            Log.d(TAG, "ARGH onPostExecute x=" + imageWidth + "; y=" + imageHeight);

//            imageView.invalidate();
//            imageView.destroyDrawingCache();

            mBitmap.setPixels(pixels, 0, imageWidth,
                    0, 0, imageWidth, imageHeight);
            imageView.setImageBitmap(mBitmap);

//            imageView.invalidate();

            Log.i(TAG, "bitmap set in imageview result=" + result);
        }
    }

}
