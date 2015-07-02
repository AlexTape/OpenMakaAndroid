package de.alextape.androidcamera.camera.activities;

import android.content.Context;
import android.content.res.Configuration;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.OrientationEventListener;
import android.view.Surface;
import android.view.ViewTreeObserver;
import android.view.WindowManager;

import de.alextape.androidcamera.camera.CameraController;

/**
 * This class implements robust orientation changes for the camera activity.
 */
public class CameraOrientationActivity extends CameraActivity {

    private static final String TAG = CameraOrientationActivity.class.getSimpleName();

    private OrientationEventListener mOrientationEventListener;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // The activity is being created.
        Log.d(TAG, "onCreate");

        mOrientationEventListener = new OrientationHelper(this);
        if (mOrientationEventListener.canDetectOrientation()) {
            mOrientationEventListener.enable();
        } else {
            finish();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        // The activity has become visible (it is now "resumed").
        Log.d(TAG, "onResume");
        if (mOrientationEventListener.canDetectOrientation()) {
            mOrientationEventListener.enable();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        // Another activity is taking focus (this activity is about to be "paused").
        Log.d(TAG, "onPause");
        mOrientationEventListener.disable();
    }

    @Override
    protected void onStop() {
        super.onStop();
        // The activity is no longer visible (it is now "stopped")
        Log.d(TAG, "onStop");
        mOrientationEventListener.disable();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // The activity is about to be destroyed.
        Log.d(TAG, "onDestroy");
        mOrientationEventListener.disable();
    }

    public void onConfigurationChanged(Configuration newConfiguration) {

        super.onConfigurationChanged(newConfiguration);

        ViewTreeObserver observer = layoutView.getViewTreeObserver();
        observer.addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {

            @Override
            public void onGlobalLayout() {

                Orientation orientation = getScreenOrientation();
                Log.d(TAG,
                        String.format("new width=%d; new height=%d; new orientation=" + orientation, layoutView.getWidth(),
                                layoutView.getHeight()));

                CameraController.getInstance().configureCamera(orientation, layoutView.getWidth(), layoutView.getHeight());

                layoutView.getViewTreeObserver().removeOnGlobalLayoutListener(this);
            }
        });
    }

    private Orientation getScreenOrientation() {
        Orientation returnThis = null;
        final Display display = ((WindowManager) getSystemService(WINDOW_SERVICE)).getDefaultDisplay();
        switch (display.getRotation()) {
            case Surface.ROTATION_0:
                returnThis = Orientation.PORTRAIT;
                break;
            case Surface.ROTATION_90:
                returnThis = Orientation.LANDSCAPE;
                break;
            case Surface.ROTATION_180:
                returnThis = Orientation.REVERSE_PORTRAIT;
                break;
            case Surface.ROTATION_270:
                returnThis = Orientation.REVERSE_LANDSCAPE;
                break;
        }
        return returnThis;
    }

    public enum Orientation {
        PORTRAIT, LANDSCAPE, REVERSE_PORTRAIT, REVERSE_LANDSCAPE
    }

    private class OrientationHelper extends OrientationEventListener {

        public OrientationHelper(Context context) {
            super(context, SensorManager.SENSOR_DELAY_NORMAL);
        }

        @Override
        public void onOrientationChanged(int degree) {
            // Fix undetected REVERSE_LANDSCAPE orientation when switching from LANDSCAPE to REVERSE_LANDSCAPE.
            CameraOrientationActivity.Orientation orientation = CameraController.getInstance().getOrientation();
            if (orientation == CameraOrientationActivity.Orientation.LANDSCAPE) {
                if (degree < 170) {
                    orientation = CameraOrientationActivity.Orientation.REVERSE_LANDSCAPE;
                    CameraController.getInstance().rotateOrientation(orientation);
                }
            }
            if (orientation == CameraOrientationActivity.Orientation.REVERSE_LANDSCAPE) {
                if (degree > 190) {
                    orientation = CameraOrientationActivity.Orientation.LANDSCAPE;
                    CameraController.getInstance().rotateOrientation(orientation);
                }
            }
        }

    }

}
