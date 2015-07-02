package de.alextape.androidcamera.camera;

import android.os.Bundle;

import de.alextape.androidcamera.camera.activities.CameraFocusActivity;
import de.alextape.androidcamera.camera.interfaces.CameraInitializedCallback;

/**
 * Extend this class to get a full initialized camera.
 */
public abstract class HardwareCamera extends CameraFocusActivity implements CameraInitializedCallback {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // this is essential because it will notify you
        // if the camera is ready to take option parameters
        // e.g. camera != null and startPreview() called before
        CameraController.getInstance().setOnCameraInitializedListener(this);
    }

    @Override
    public void onCameraIsInitialized() {
        onCameraInitialized();
    }

    public abstract void onCameraInitialized();

}
