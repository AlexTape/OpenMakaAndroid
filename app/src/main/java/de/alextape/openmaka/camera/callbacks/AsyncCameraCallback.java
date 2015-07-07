package de.alextape.openmaka.camera.callbacks;

import android.graphics.Bitmap;
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

    private boolean mProcessInProgress = false;

    private int[] pixels;

    private int mOutputWidth;
    private int mOutputHeight;
    public Bitmap mBitmap;
    public ImageView mImageView;
    private Camera mCamera;

    public AsyncCameraCallback(ImageView imageView) {
        mImageView = imageView;
        initialized = false;
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {

        if (mProcessInProgress) {
            mCamera.addCallbackBuffer(data);
            return;
        }

        if (CameraController.getInstance().getPreviewFormat() != ImageFormat.NV21) {

            Log.d(TAG, "wrong format");
        } else {

            if (data != null) {

                mProcessInProgress = true;

                if (mBitmap == null) {
                    mBitmap = Bitmap.createBitmap(mOutputWidth, mOutputHeight,
                            Bitmap.Config.ARGB_8888);
                    mImageView.setImageBitmap(mBitmap);
                }

                Camera.Size previewSize = CameraController.getInstance().getPreviewSize();
                mOutputWidth = previewSize.width;
                mOutputHeight = previewSize.height;

                double expectedBytes = CameraController.getInstance().getExpectedByteBufferSize();

                if (expectedBytes != data.length) {
                    Log.e(TAG, "Mismatched size of buffer! Expected " + expectedBytes + "; Got " + data.length + ";");
                    return;
                } else {

                    new AsyncCameraTask().execute(data);
                }

            }

        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        CameraController.getInstance().setPreviewDisplay(holder);
        CameraController.getInstance().setPreviewCallback(this);
        CameraController.getInstance().publish();
        mCamera = CameraController.getInstance().getCamera();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged");

        mOutputWidth = width;
        mOutputHeight = height;

        //Log.d(TAG, String.format("Format=%d; width=%d; height=%d", format, mOutputWidth, mOutputHeight));
        CameraController.getInstance().reconfigure(format, width, height);

        //Log.d(TAG, "ARGH SIZE x=" + mOutputWidth + "; y=" + mOutputHeight);

        pixels = new int[mOutputHeight * mOutputWidth];

        mBitmap = Bitmap.createBitmap(mOutputHeight, mOutputWidth,
                Bitmap.Config.ARGB_8888);

        mImageView.setImageBitmap(mBitmap);
        mImageView.setRotation(90);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
        CameraController.getInstance().destroy();
    }

    private Long lastMillis;
    private boolean initialized;

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

            data = datas[0];

            long t1 = System.currentTimeMillis();
            if (lastMillis != null) {
                if (lastMillis + CameraConfig.doInBackgroundTimer > t1) {
                    return false;
                }
            }

            lastMillis = t1;

            // process data function
            if (!initialized) {
                String configFile = "/storage/emulated/0/Android/data/de.alextape.openmaka/files/config/config.xml";
                initialized = NativeController.initialize(mOutputWidth, mOutputHeight, data, pixels, configFile);
            } else {
                NativeController.displayFunction(mOutputWidth, mOutputHeight, data, pixels);
            }

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

            if (mCamera != null) {
                mCamera.addCallbackBuffer(data);
            }
            mProcessInProgress = false;

            return true;
        }

        @Override
        protected void onPostExecute(Boolean result) {
            //Log.i(TAG, "running onPostExecute");

            if (mImageView == null) {
                return;
            }

            if (mBitmap == null) {
                return;
            }

            if (pixels == null) {
                return;
            }

            mImageView.invalidate();
            mBitmap.setPixels(pixels, 0, mOutputWidth,
                    0, 0, mOutputWidth, mOutputHeight);
//            mImageView.setImageBitmap(mBitmap);

        }
    }

}
