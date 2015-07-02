package de.alextape.androidcamera.camera.interfaces;

import android.hardware.Camera;
import android.view.SurfaceHolder;

/**
 * This interface is needed to switch between linear and async frame processing.
 */
public interface CameraCallbackInterface {

    public void onPreviewFrame(byte[] data, Camera camera);

    public void surfaceCreated(SurfaceHolder holder);

    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height);

    public void surfaceDestroyed(SurfaceHolder holder);

}
