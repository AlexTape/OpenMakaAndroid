package de.alextape.androidcamera.camera.tasks;

import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.ImageView;

import de.alextape.androidcamera.camera.CameraConfig;
import de.alextape.androidcamera.camera.CameraController;

/**
 * This async task is resonsible for async frame processing (against e.g. ndk).
 */
public class AsyncCameraTask extends AsyncTask<byte[], Void, Boolean> {

    private static final String TAG = AsyncCameraTask.class.getSimpleName();

    private long mTiming[] = new long[50];
    private int mTimingSlot = 0;

    private boolean mProcessInProgress;

    private int[] pixels;

    private int imageWidth;
    private int imageHeight;

    @Override
    protected Boolean doInBackground(byte[]... datas) {
        // TODO Auto-generated method stub
        Log.i(TAG, "background process started");

        byte[] data = datas[0];

        imageWidth = CameraController.getInstance().parameterWidth;
        imageHeight = CameraController.getInstance().parameterHeight;
        pixels = new int[imageWidth * imageHeight];

        long t1 = System.currentTimeMillis();

        // process data function
        //convertGray(previewSizeWidth, previewSizeHeight, data, pixels);


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

        CameraController.getInstance().setCameraCallbackBuffer(data);
        mProcessInProgress = false;

        return true;
    }

    @Override
    protected void onPostExecute(Boolean result) {
        //Log.i(TAG, "running onPostExecute");

        ImageView imageView = CameraController.getInstance().getImageView();
        //int width = CameraController.getInstance().parameterWidth;
        //int height = CameraController.getInstance().parameterHeight;

        int height = imageView.getHeight();
        int width = imageView.getWidth();

        imageView.invalidate();
        imageView.destroyDrawingCache();

        Bitmap mBitmap = Bitmap.createBitmap(width, height,
                Bitmap.Config.ARGB_8888);
        imageView.setImageBitmap(mBitmap);

        mBitmap.setPixels(pixels, 0, width,
                0, 0, width, height);
        imageView.setImageBitmap(mBitmap);
        //Log.i(TAG, "bitmap set in imageview");

    }
}
