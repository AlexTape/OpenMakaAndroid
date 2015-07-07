package de.alextape.openmaka.camera;

import android.app.Activity;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;

import de.alextape.openmaka.R;
import de.alextape.openmaka.camera.callbacks.AsyncCameraCallback;
import de.alextape.openmaka.camera.callbacks.CameraCallback;
import de.alextape.openmaka.camera.interfaces.CameraInitializedCallback;

/**
 * Extend this class to get a full initialized camera.
 */
public abstract class AndroidCamera extends Activity implements CameraInitializedCallback {

    private static final String TAG = AndroidCamera.class.getSimpleName();

    protected View layoutView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().requestFeature(Window.FEATURE_ACTION_BAR_OVERLAY);

        setContentView(R.layout.open_maka_layout);
        this.layoutView = this.findViewById(R.id.layoutContainer);

        ImageView imageView = (ImageView) layoutView.findViewById(R.id.imageView);

        // init CameraController
        if (CameraConfig.ASYNC_CAMERA) {
            CameraController.create(this, layoutView, new AsyncCameraCallback(imageView));
        } else {
            CameraController.create(this, layoutView, new CameraCallback());
        }

        // this is essential because it will notify you
        // if the camera is ready to take option parameters
        // e.g. camera != null and startPreview() called before
        CameraController.getInstance().setOnCameraInitializedListener(this);
    }

    @Override
    public void onStart() {
        super.onStart();
        // The activity is about to become visible.
        Log.d(TAG, "onStart");
        CameraController.getInstance().initialize();
    }

    @Override
    public void onResume() {
        super.onResume();
        // The activity has become visible (it is now "resumed").
        Log.d(TAG, "onResume");
        CameraController.getInstance().start();
    }

    @Override
    public void onPause() {
        super.onPause();
        // Another activity is taking focus (this activity is about to be "paused").
        Log.d(TAG, "onPause");
        CameraController.getInstance().stop();
    }

    @Override
    public void onStop() {
        super.onStop();
        // The activity is no longer visible (it is now "stopped")
        Log.d(TAG, "onStop");
        CameraController.getInstance().stop();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        // The activity is about to be destroyed.
        Log.d(TAG, "onDestroy");
        CameraController.getInstance().destroy();
    }

    @Override
    public void onCameraIsInitialized() {
        onCameraInitialized();
    }

    public abstract void onCameraInitialized();

    public void onConfigurationChanged(Configuration newConfiguration) {

        super.onConfigurationChanged(newConfiguration);

        final View layoutView = this.findViewById(R.id.layoutContainer);

        ViewTreeObserver observer = layoutView.getViewTreeObserver();
        observer.addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {

            @Override
            public void onGlobalLayout() {

                CameraController.Orientation orientation = getScreenOrientation();
                Log.d(TAG,
                        String.format("new width=%d; new height=%d; new orientation=" + orientation, layoutView.getWidth(),
                                layoutView.getHeight()));

                CameraController.getInstance().reconfigure(orientation, layoutView.getWidth(), layoutView.getHeight());

                layoutView.getViewTreeObserver().removeOnGlobalLayoutListener(this);
            }
        });
    }

    private CameraController.Orientation getScreenOrientation() {
        CameraController.Orientation returnThis = null;
        final Display display = ((WindowManager) getSystemService(WINDOW_SERVICE)).getDefaultDisplay();
        switch (display.getRotation()) {
            case Surface.ROTATION_0:
                returnThis = CameraController.Orientation.PORTRAIT;
                break;
            case Surface.ROTATION_90:
                returnThis = CameraController.Orientation.LANDSCAPE;
                break;
            case Surface.ROTATION_180:
                returnThis = CameraController.Orientation.REVERSE_PORTRAIT;
                break;
            case Surface.ROTATION_270:
                returnThis = CameraController.Orientation.REVERSE_LANDSCAPE;
                break;
        }
        return returnThis;
    }

}
