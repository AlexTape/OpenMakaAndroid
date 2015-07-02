package de.alextape.androidcamera.camera.activities;

import android.os.Bundle;
import android.util.Log;

import de.alextape.androidcamera.camera.CameraConfig;
import de.alextape.androidcamera.camera.CameraController;
import de.alextape.androidcamera.camera.callbacks.AsyncCameraCallback;
import de.alextape.androidcamera.camera.callbacks.CameraCallback;

/**
 * This class implements the camera lifecycle.
 */
public abstract class CameraActivity extends CameraLayoutActivity {

    private static final String TAG = CameraActivity.class.getSimpleName();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // The activity is being created.
        Log.d(TAG, "onCreate");

        // init CameraController
        if (CameraConfig.ASYNC_CAMERA) {
            CameraController.create(this, layoutView, new AsyncCameraCallback());
        } else {
            CameraController.create(this, layoutView, new CameraCallback());
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        // The activity is about to become visible.
        Log.d(TAG, "onStart");
        CameraController.getInstance().startCamera();
    }

    @Override
    protected void onResume() {
        super.onResume();
        // The activity has become visible (it is now "resumed").
        Log.d(TAG, "onResume");
        //cameraController.startCamera();
    }

    @Override
    protected void onPause() {
        super.onPause();
        // Another activity is taking focus (this activity is about to be "paused").
        Log.d(TAG, "onPause");
        //cameraController.stopCamera();
    }

    @Override
    protected void onStop() {
        super.onStop();
        // The activity is no longer visible (it is now "stopped")
        Log.d(TAG, "onStop");
        //cameraController.stopCamera();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // The activity is about to be destroyed.
        Log.d(TAG, "onDestroy");
        CameraController cameraController = CameraController.getInstance();
        cameraController.stopAndReleaseCamera();
        cameraController.releaseView();
    }

}
