package de.alextape.androidcamera.camera.callbacks;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.util.Log;

import de.alextape.androidcamera.camera.CameraController;
import de.alextape.androidcamera.camera.tasks.AsyncCameraTask;

/**
 * This callback delegates incoming frames to AsyncCameraTask. Furthermore this
 * class is responsible for data integrity.
 */
public class AsyncCameraCallback extends CameraCallback implements Camera.PreviewCallback {

    private static final String TAG = AsyncCameraCallback.class.getSimpleName();

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {

        //Log.d(TAG, "onPreviewFrame");
        if (CameraController.getInstance().getPreviewFormat() != ImageFormat.NV21) {

            Log.d(TAG, "wrong format");
        } else {

            if (data != null) {
                camera.addCallbackBuffer(data);
                new AsyncCameraTask().execute(data);
            }

        }
    }

}
