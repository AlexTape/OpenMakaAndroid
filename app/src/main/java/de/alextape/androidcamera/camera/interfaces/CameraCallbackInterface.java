package de.alextape.androidcamera.camera.interfaces;

import android.hardware.Camera;
import android.view.SurfaceHolder;

/**
 * This interface is needed to switch between linear and async frame processing.
 */
public interface CameraCallbackInterface extends SurfaceHolder.Callback, Camera.PreviewCallback {

    void onPreviewFrame(byte[] data, Camera camera);

    void surfaceCreated(SurfaceHolder holder);

    void surfaceChanged(SurfaceHolder holder, int format, int width, int height);

    void surfaceDestroyed(SurfaceHolder holder);

}
