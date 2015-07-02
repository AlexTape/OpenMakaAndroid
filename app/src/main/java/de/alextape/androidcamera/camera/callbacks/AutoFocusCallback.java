package de.alextape.androidcamera.camera.callbacks;

import android.hardware.Camera;
import android.util.Log;

/**
 * This callback is registered to finished auto focus events.
 */
public class AutoFocusCallback implements Camera.AutoFocusCallback {

    private static final String TAG = AutoFocusCallback.class.getSimpleName();

    @Override
    public void onAutoFocus(boolean success, Camera camera) {
        Log.d(TAG, "onAutoFocus state:" + success);
    }

}
