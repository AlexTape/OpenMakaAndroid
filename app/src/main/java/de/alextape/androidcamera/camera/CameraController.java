package de.alextape.androidcamera.camera;

import android.content.Context;
import android.graphics.Point;
import android.hardware.Camera;
import android.util.Log;
import android.view.Display;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;

import java.io.IOException;
import java.util.List;

import de.alextape.openmaka.R;
import de.alextape.androidcamera.camera.interfaces.CameraCallbackInterface;
import de.alextape.androidcamera.camera.interfaces.CameraInitializedCallback;

/**
 * This class represents the camera implementation as a singleton to be accessible from
 * any point of your application.
 */
public class CameraController {

    private static final String TAG = CameraController.class.getSimpleName();
    private static CameraController _instance = null;
    private static Orientation initialOrientation = null;

    public int parameterHeight;
    public int parameterWidth;

    private ImageView imageView;
    private Camera mCamera;
    private Camera.Parameters mCameraParameter;
    private CameraCallbackInterface mCameraCallback;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private boolean mPreviewRunning;
    private Integer mWindowWidth;
    private Integer mWindowHeight;
    private Orientation mSurfaceOrientation;
    private List<Camera.Size> mSupportedPreviewSizes;
    private Camera.Size mForcedPreviewSize;
    private Camera.Size mPreviewSize;
    private Integer mPreviewFormat;
    private CameraInitializedCallback mCameraInitializedCallback;
    private String mFlashMode;
    private String mFocusMode;

    private CameraController(Context context, View layoutView, CameraCallbackInterface mCameraCallback) {

        Log.d(TAG, "CameraController");

        this.imageView = (ImageView) layoutView.findViewById(R.id.imageView);
        this.mSurfaceView = (SurfaceView) layoutView.findViewById(R.id.cameraSurface);

        this.mSurfaceHolder = mSurfaceView.getHolder();
        this.mSurfaceHolder.addCallback(mCameraCallback);
        this.mSurfaceHolder.setKeepScreenOn(true);

        // TODO TYPE GPU???
        this.mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        // TODO get initial orientation
        if (initialOrientation != null) {
            Log.d(TAG, "Trigger initial orientation");

            this.mSurfaceOrientation = initialOrientation;
        } else {
            // set orientation to portrait
            this.mSurfaceOrientation = Orientation.PORTRAIT;
        }


        WindowManager windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        Display display = windowManager.getDefaultDisplay();
        Point displaySize = new Point();
        display.getSize(displaySize);

        this.mWindowWidth = displaySize.x;
        this.mWindowHeight = displaySize.y;

        Log.d(TAG,
                String.format("width=%d; height=%d;", mWindowWidth,
                        mWindowHeight));

        this.mCamera = null;
        this.mCameraParameter = null;
        this.mCameraCallback = mCameraCallback;

        this.mSupportedPreviewSizes = null;
        this.mForcedPreviewSize = null;
        this.mPreviewSize = null;

        this.mPreviewFormat = null;
        this.mPreviewRunning = false;

    }

    public static void setInitialOrientation(Orientation initialOrientation) {
        CameraController.initialOrientation = initialOrientation;
    }

    public static CameraController getInstance() {
        return _instance;
    }

    public static CameraController create(Context context, View layoutView, CameraCallbackInterface cameraCallback) {
        Log.d(TAG, "create");
        _instance = new CameraController(context, layoutView, cameraCallback);
        return _instance;
    }

    public void setOnCameraInitializedListener(CameraInitializedCallback releaseListener) {
        mCameraInitializedCallback = releaseListener;
    }

    public ImageView getImageView() {
        return imageView;
    }

    private void openCamera() {
        if (mCamera == null) {
            if (CameraConfig.CAMERA == CameraType.FRONT_CAMERA) {
                mCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_FRONT);
            } else {
                mCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_BACK);
            }
            if (mCameraParameter == null) {
                mCameraParameter = mCamera.getParameters();
            }
        }
    }

    public void initialize() {
        Log.d(TAG, "start");
        destroy();
        openCamera();
        if (mCameraInitializedCallback != null) {
            mCameraInitializedCallback.onCameraIsInitialized();
        }
    }

    public void start() {
        if (mCameraCallback != null) {
            mCamera.setPreviewCallback(mCameraCallback);
        }
        mCamera.setParameters(mCameraParameter);
        mCamera.startPreview();

        mSurfaceView.requestLayout();
        mPreviewRunning = true;
    }

    public Integer getPreviewFormat() {
        return mPreviewFormat;
    }

    public void reconfigure(Orientation orientation, int width, int height) {
        Log.d(TAG, "configureCameraWithValues");
        mSurfaceOrientation = orientation;
        mWindowWidth = width;
        mWindowHeight = height;
        publish();
    }

    private void configure() {
        Log.d(TAG, "configure");
        if (mCamera != null) {

            // TODO get optimal size
            mSupportedPreviewSizes = mCameraParameter.getSupportedPreviewSizes();

//            if (mSupportedPreviewSizes != null) {
//                mPreviewSize = getOptimalPreviewSize(mSupportedPreviewSizes, mWindowWidth, mWindowHeight);
//                mCameraParameter.setPreviewSize(mWindowWidth, mWindowHeight);
//            }

            mPreviewFormat = mCameraParameter.getPreviewFormat();

            // TODO FIX PREVIEW SIZES and make the available via options menu
            // configure preview size
            mPreviewSize = null;
            if (mForcedPreviewSize == null) {
                mPreviewSize = getOptimalPreviewSize(mSupportedPreviewSizes, mWindowWidth, mWindowHeight);
            } else {
                mPreviewSize = mForcedPreviewSize;
            }

//            for (Camera.Size size : mSupportedPreviewSizes) {
//                double ratio = (double) size.height / size.width;
//                Log.d(TAG, "mSupportedPreviewSize: x=" + size.width + "; y=" + size.height + "; ratio=" + ratio);
//                if (opt != null) {
//                    Log.d(TAG, "mOptimalPreviewSize: x=" + opt.width + "; y=" + opt.height + ";");
//                } else {
//                    Log.d(TAG, "mOptimalPreviewSize: null;");
//                }
//            }

            // TODO trigger resolution
            Camera.Size cs = mSupportedPreviewSizes.get(2);
            mCameraParameter.setPreviewSize(cs.width, cs.height);
            Log.d(TAG, "setPreviewSize mWindow: x=" + mWindowWidth + "; y=" + mWindowHeight);

            parameterWidth = cs.width;
            parameterHeight = cs.height;
            Log.d(TAG, "setPreviewSize:cs x=" + parameterWidth + "; y=" + parameterHeight);

            rotate();

            try {
                mCamera.setPreviewDisplay(mSurfaceHolder);
                if (CameraConfig.ASYNC_CAMERA) {
                    mCamera.setPreviewCallback(mCameraCallback);
                }
                mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
            } catch (IOException e) {
                e.printStackTrace();
            }

            if (mFlashMode != null) {
                mCameraParameter.setFlashMode(mFlashMode);
            }

            if (mFocusMode != null) {
                mCameraParameter.setFocusMode(mFocusMode);
            }

        } else {
            Log.d(TAG, "Camera is NULL");
        }
    }

    private void rotate() {
        Log.d(TAG, "DO ROTATION");
        switch (mSurfaceOrientation) {
            case PORTRAIT:
                Log.d(TAG, "rotate to PORTRAIT");
                mCamera.setDisplayOrientation(90);
                break;
            case LANDSCAPE:
                Log.d(TAG, "rotate to LANDSCAPE");
                mCamera.setDisplayOrientation(0);
                break;
            case REVERSE_PORTRAIT:
                Log.d(TAG, "rotate to REVERSE_PORTRAIT");
                mCamera.setDisplayOrientation(270);
                break;
            case REVERSE_LANDSCAPE:
                Log.d(TAG, "rotate to REVERSE_LANDSCAPE");
                mCamera.setDisplayOrientation(180);
                break;
            default:
                Log.d(TAG, "rotate to default");
                mCamera.setDisplayOrientation(0);
                break;
        }
    }

    private Camera.Size getOptimalPreviewSize(List<Camera.Size> sizes, int width, int height) {

        Log.i(TAG, "getOptimalPreviewSize()");
        Camera.Size optimalSize = null;

        final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) height / width;

        for (Camera.Size size : sizes) {

            double ratio = (double) size.height / size.width;
            Log.d(TAG, "getOptimalPreviewSize " + String.format("x=%s; y=%s; ratio=%s; targetRatio=%s", size.width, size.height, ratio, targetRatio));

            if (ratio <= targetRatio + ASPECT_TOLERANCE && ratio >= targetRatio - ASPECT_TOLERANCE) {
                optimalSize = size;
            }
        }
        return optimalSize;
    }

    private void publish() {
        Log.d(TAG, "publish");
        stop();
        configure();
        start();
    }

    public void stop() {
        if (mCamera != null) {
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mPreviewRunning = false;
            mCameraParameter = mCamera.getParameters();
        }
    }

    public void destroy() {
        Log.d(TAG, "destroy");
        if (mCamera != null) {
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
            mPreviewRunning = false;
        }
        if (mSurfaceView != null) {
            mSurfaceView.destroyDrawingCache();
            mSurfaceView.invalidate();
        }
    }

    public List<String> getSupportedFocusModes() {
        return mCameraParameter.getSupportedFocusModes();
    }

    public void setFocusMode(String supportedFocusMode) {
        Log.d(TAG, "setFocusMode=" + supportedFocusMode);
        mFocusMode = supportedFocusMode;
        if (mPreviewRunning) {
            publish();
        }
    }

    public List<String> getSupportedFlashModes() {
        return mCameraParameter.getSupportedFlashModes();
    }

    public void setFlashMode(String supportedFlashMode) {
        Log.d(TAG, "setFlashMode=" + supportedFlashMode);
        mFlashMode = supportedFlashMode;
        if (mPreviewRunning) {
            publish();
        }
    }

    public void setPreviewDisplay(SurfaceHolder holder) {
        mSurfaceHolder = holder;
        publish();
    }

    public void addCameraCallbackBuffer(byte[] data) {
        if (mCamera != null) {
            mCamera.addCallbackBuffer(data);
        } else {
            Log.d(TAG, "skipped addCallbackBuffer (mCamera==null)");
        }
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

    public String setFocusMode(int type) {
        Log.d(TAG, "setFocusMode");

        String returnThis = null;
        List<String> focusModes = mCameraParameter.getSupportedFocusModes();

        switch (type) {
            case 0:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
                    setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
                } else {
                    returnThis = "Auto Mode not supported";
                }
                break;
            case 1:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
                    setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
                } else {
                    returnThis = "Continuous Video Mode not supported";
                }
                break;
            case 2:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_EDOF)) {
                    setFocusMode(Camera.Parameters.FOCUS_MODE_EDOF);
                } else {
                    returnThis = "EDOF Mode not supported";
                }
                break;
            case 3:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_FIXED)) {
                    setFocusMode(Camera.Parameters.FOCUS_MODE_FIXED);
                } else {
                    returnThis = "Fixed Mode not supported";
                }
                break;
            case 4:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_INFINITY)) {
                    setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY);
                } else {
                    returnThis = "Infinity Mode not supported";
                }
                break;
            case 5:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_MACRO)) {
                    setFocusMode(Camera.Parameters.FOCUS_MODE_MACRO);
                } else {
                    returnThis = "Macro Mode not supported";
                }
                break;
            case 6:
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                    setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
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
        List<String> FlashModes = mCameraParameter.getSupportedFlashModes();

        switch (type) {
            case 0:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_AUTO)) {
                    setFlashMode(Camera.Parameters.FLASH_MODE_AUTO);
                } else {
                    returnThis = "Auto Mode not supported";
                }
                break;
            case 1:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_OFF)) {
                    setFlashMode(Camera.Parameters.FLASH_MODE_OFF);
                } else {
                    returnThis = "Off Mode not supported";
                }
                break;
            case 2:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_ON)) {
                    setFlashMode(Camera.Parameters.FLASH_MODE_ON);
                } else {
                    returnThis = "On Mode not supported";
                }
                break;
            case 3:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_RED_EYE)) {
                    setFlashMode(Camera.Parameters.FLASH_MODE_RED_EYE);
                } else {
                    returnThis = "Red Eye Mode not supported";
                }
                break;
            case 4:
                if (FlashModes.contains(Camera.Parameters.FLASH_MODE_TORCH)) {
                    setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
                } else {
                    returnThis = "Torch Mode not supported";
                }
                break;
        }

        return returnThis;
    }

    public void setPreviewCallback(CameraCallbackInterface callback) {
        this.mCameraCallback = callback;
    }

    public enum CameraType {
        FRONT_CAMERA, BACK_CAMERA
    }

    public enum Orientation {
        PORTRAIT, LANDSCAPE, REVERSE_PORTRAIT, REVERSE_LANDSCAPE
    }
}
