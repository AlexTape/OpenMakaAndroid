package de.alextape.androidcamera.camera.activities;

import android.app.Application;
import android.content.Context;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.WindowManager;

import de.alextape.androidcamera.camera.CameraController;

/**
 * This class is used to get the initial orientation before Activity.onCreate().
 */
public class CameraApplication extends Application {

    private static final String TAG = CameraApplication.class.getSimpleName();

    @Override
    public void onCreate() {
        super.onCreate();

        // promote initial orientation immediately
        CameraOrientationActivity.Orientation initialOrientation = null;
        final Display display = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        switch (display.getRotation()) {
            case Surface.ROTATION_0:
                initialOrientation = CameraOrientationActivity.Orientation.PORTRAIT;
                break;
            case Surface.ROTATION_90:
                initialOrientation = CameraOrientationActivity.Orientation.LANDSCAPE;
                break;
            case Surface.ROTATION_180:
                initialOrientation = CameraOrientationActivity.Orientation.REVERSE_PORTRAIT;
                break;
            case Surface.ROTATION_270:
                initialOrientation = CameraOrientationActivity.Orientation.REVERSE_LANDSCAPE;
                break;
        }
        Log.d(TAG, "Initial orientation: " + initialOrientation);
        CameraController.setInitialOrientation(initialOrientation);
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        throw new OutOfMemoryError("Low Memory");
    }

}
