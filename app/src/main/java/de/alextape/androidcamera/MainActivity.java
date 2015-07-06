package de.alextape.androidcamera;

import android.os.Bundle;
import android.util.Log;

import de.alextape.androidcamera.camera.CameraConfig;
import de.alextape.androidcamera.camera.CameraController;
import de.alextape.androidcamera.camera.AndroidCamera;
import de.alextape.androidcamera.camera.callbacks.AsyncCameraCallback;
import de.alextape.androidcamera.camera.callbacks.CameraCallback;

/**
 * This is a demo how to implement the camera easily.
 */
public class MainActivity extends AndroidCamera {

    private static final String TAG = MainActivity.class.getSimpleName();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // The activity is being created.
        Log.d(TAG, "onCreate");
    }

    @Override
    public void onStart() {
        super.onStart();
        // The activity is about to become visible.
        Log.d(TAG, "onStart");
    }

    @Override
    public void onResume() {
        super.onResume();
        // The activity has become visible (it is now "resumed").
        Log.d(TAG, "onResume");
    }

    @Override
    public void onPause() {
        super.onPause();
        // Another activity is taking focus (this activity is about to be "paused").
        Log.d(TAG, "onPause");
    }

    @Override
    public void onStop() {
        super.onStop();
        // The activity is no longer visible (it is now "stopped")
        Log.d(TAG, "onStop");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        // The activity is about to be destroyed.
        Log.d(TAG, "onDestroy");
    }

    /**
     * This method is used to gather options passed by a user interactions.
     * e.g. Focus mode, Flash mode.. directly AFTER camera is initialized.
     */
    @Override
    public void onCameraInitialized() {
        Log.d(TAG, "onReleaseCamera");

        // Returns a list of available Focus modes
        String[] focusOptions = CameraController.getInstance().getFlashOptions();
        // pass index of option to setFocusMode(int)
        String result = CameraController.getInstance().setFocusMode(0);
        if (result == null) {
            Log.d(TAG, "Success");
        } else {
            Log.d(TAG, "Fail: " + result);
        }

    }

}
