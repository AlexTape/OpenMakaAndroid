package de.alextape.androidcamera.camera.activities;

import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import java.util.ArrayList;
import java.util.List;

import de.alextape.androidcamera.camera.CameraController;
import de.alextape.androidcamera.camera.callbacks.AutoFocusCallback;

/**
 * This class extends the orientation activity to get a centralized location
 * for the implementation of supported camera features.
 */
public class CameraFocusActivity extends CameraOrientationActivity implements View.OnTouchListener {

    private static final String TAG = CameraFocusActivity.class.getSimpleName();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // The activity is being created.
        Log.d(TAG, "onCreate");

        // get view for onTouch()
        layoutView.setOnTouchListener(this);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        Log.d(TAG, "onTouch");
        Camera.Parameters parameters = CameraController.getInstance().getCameraParameter();
        if (Camera.Parameters.FOCUS_MODE_AUTO == parameters.getFocusMode()) {
            Log.d(TAG, "Focus Mode == AUTO");
            focusOnTouch(event);
        }
        return true;
    }

    public String[] getFocusOptions() {
        Log.d(TAG, "getFocusOptions");
        String[] options = new String[7];
        options[0] = "Auto";
        options[1] = "Continuous Video";
        options[2] = "EDOF";
        options[3] = "Fixed";
        options[4] = "Infinity";
        options[5] = "Makro";
        options[6] = "Continuous Picture";
        return options;
    }

    public String[] getFlashOptions() {
        Log.d(TAG, "getFlashOptions");
        String[] options = new String[5];
        options[0] = "Auto";
        options[0] = "Off";
        options[2] = "On";
        options[3] = "Red-Eye";
        options[4] = "Torch";
        return options;
    }

    public void focusOnTouch(MotionEvent event) {
        Log.d(TAG, "focusOnTouch");
        Rect focusRect = calculateTapArea(event.getRawX(), event.getRawY(), 1f);
        Rect meteringRect = calculateTapArea(event.getRawX(), event.getRawY(), 1.5f);

        Log.d(TAG, focusRect.toString());

        Camera.Parameters parameters = CameraController.getInstance().getCameraParameter();
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);

        if (parameters.getMaxNumFocusAreas() > 0) {
            List<Camera.Area> focusAreas = new ArrayList<Camera.Area>();
            focusAreas.add(new Camera.Area(focusRect, 1000));

            parameters.setFocusAreas(focusAreas);
        }

        if (parameters.getMaxNumMeteringAreas() > 0) {
            List<Camera.Area> meteringAreas = new ArrayList<Camera.Area>();
            meteringAreas.add(new Camera.Area(meteringRect, 1000));

            parameters.setMeteringAreas(meteringAreas);
        }

        CameraController.getInstance().setCameraParameter(parameters);
        CameraController.getInstance().setAutoFocusCallback(new AutoFocusCallback());
    }

    public Camera.Size getResolution() {
        Log.d(TAG, "getResolution");
        Camera.Parameters params = CameraController.getInstance().getCameraParameter();
        Camera.Size s = params.getPreviewSize();
        return s;
    }

    private Rect calculateTapArea(float x, float y, float coefficient) {
        Log.d(TAG, "calculateTapArea");
        float focusAreaSize = 300;
        int areaSize = Float.valueOf(focusAreaSize * coefficient).intValue();

        int centerX = (int) (x / getResolution().width - 1000);
        int centerY = (int) (y / getResolution().height - 1000);

        int left = clamp(centerX - areaSize / 2, -1000, 1000);
        int top = clamp(centerY - areaSize / 2, -1000, 1000);

        RectF rectF = new RectF(left, top, left + areaSize, top + areaSize);

        return new Rect(Math.round(rectF.left), Math.round(rectF.top), Math.round(rectF.right), Math.round(rectF.bottom));
    }

    private int clamp(int x, int min, int max) {
        Log.d(TAG, "clamp");
        if (x > max) {
            return max;
        }
        if (x < min) {
            return min;
        }
        return x;
    }

    public String setFocusMode(int type) {
        Log.d(TAG, "setFocusMode");

        String returnThis = null;
        List<String> focusModes = CameraController.getInstance().getSupportedFocusModes();

        switch (type) {
            case 0:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
                    CameraController.getInstance().setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
                } else {
                    returnThis = "Auto Mode not supported";
                }
                break;
            case 1:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
                    CameraController.getInstance().setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
                } else {
                    returnThis = "Continuous Video Mode not supported";
                }
                break;
            case 2:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_EDOF)) {
                    CameraController.getInstance().setFocusMode(Camera.Parameters.FOCUS_MODE_EDOF);
                } else {
                    returnThis = "EDOF Mode not supported";
                }
                break;
            case 3:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_FIXED)) {
                    CameraController.getInstance().setFocusMode(Camera.Parameters.FOCUS_MODE_FIXED);
                } else {
                    returnThis = "Fixed Mode not supported";
                }
                break;
            case 4:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_INFINITY)) {
                    CameraController.getInstance().setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY);
                } else {
                    returnThis = "Infinity Mode not supported";
                }
                break;
            case 5:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_MACRO)) {
                    CameraController.getInstance().setFocusMode(Camera.Parameters.FOCUS_MODE_MACRO);
                } else {
                    returnThis = "Macro Mode not supported";
                }
                break;
            case 6:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                    CameraController.getInstance().setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
                } else {
                    returnThis = "Continuous Picture Mode not supported";
                }
                break;
        }

        Log.d(TAG, "setFocusMode done..");
        return returnThis;
    }

    public String setFlashMode(int type) {
        Log.d(TAG, "setFlashMode");

        String returnThis = null;
        List<String> FlashModes = CameraController.getInstance().getSupportedFlashModes();

        switch (type) {
            case 0:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_AUTO)) {
                    CameraController.getInstance().setFlashMode(Camera.Parameters.FLASH_MODE_AUTO);
                } else {
                    returnThis = "Auto Mode not supported";
                }
                break;
            case 1:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_OFF)) {
                    CameraController.getInstance().setFlashMode(Camera.Parameters.FLASH_MODE_OFF);
                } else {
                    returnThis = "Off Mode not supported";
                }
                break;
            case 2:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_ON)) {
                    CameraController.getInstance().setFlashMode(Camera.Parameters.FLASH_MODE_ON);
                } else {
                    returnThis = "On Mode not supported";
                }
                break;
            case 3:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_RED_EYE)) {
                    CameraController.getInstance().setFlashMode(Camera.Parameters.FLASH_MODE_RED_EYE);
                } else {
                    returnThis = "Red Eye Mode not supported";
                }
                break;
            case 4:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_TORCH)) {
                    CameraController.getInstance().setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
                } else {
                    returnThis = "Torch Mode not supported";
                }
                break;
        }

        return returnThis;
    }

}
