package de.alextape.androidcamera.camera.callbacks;

import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;

import de.alextape.androidcamera.camera.CameraController;
import de.alextape.androidcamera.camera.interfaces.CameraCallbackInterface;

/**
 * This callback is responsible for the camera surface view.
 */
public class CameraCallback implements SurfaceHolder.Callback, CameraCallbackInterface {

    private static final String TAG = CameraCallback.class.getSimpleName();

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        // unused for this class
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        CameraController.getInstance().setPreviewDisplay(holder);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged");
        Log.d(TAG, String.format("Format=%d; width=%d; height=%d", format, width, height));
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
        CameraController.getInstance().stopAndReleaseCamera();
    }

}
