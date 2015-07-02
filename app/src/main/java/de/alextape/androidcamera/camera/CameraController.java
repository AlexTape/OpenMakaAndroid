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
import de.alextape.androidcamera.camera.activities.CameraOrientationActivity;
import de.alextape.androidcamera.camera.callbacks.AsyncCameraCallback;
import de.alextape.androidcamera.camera.callbacks.AutoFocusCallback;
import de.alextape.androidcamera.camera.callbacks.CameraCallback;
import de.alextape.androidcamera.camera.interfaces.CameraCallbackInterface;
import de.alextape.androidcamera.camera.interfaces.CameraInitializedCallback;

/**
 * This class represents the camera implementation as a singleton to be accessible from
 * any point of your application.
 */
public class CameraController {

    private static final String TAG = CameraController.class.getSimpleName();
    private static CameraController _instance = null;
    private static CameraOrientationActivity.Orientation initialOrientation = null;

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
    private CameraOrientationActivity.Orientation mSurfaceOrientation;
    private List<Camera.Size> mSupportedPreviewSizes;
    private List<Camera.Size> mPreviewSizes;
    private Camera.Size mForcedPreviewSize;
    private Camera.Size mPreviewSize;
    private Integer mPreviewFormat;
    private CameraInitializedCallback mCameraInitializedCallback;

    private CameraController(Context context, View layoutView, CameraCallback mCameraCallback) {

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
            this.mSurfaceOrientation = CameraOrientationActivity.Orientation.PORTRAIT;
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

    public static void setInitialOrientation(CameraOrientationActivity.Orientation initialOrientation) {
        CameraController.initialOrientation = initialOrientation;
    }

    public static CameraController getInstance() {
        return _instance;
    }

    public static CameraController create(Context context, View layoutView, CameraCallback cameraCallback) {
        Log.d(TAG, "create");
        _instance = new CameraController(context, layoutView, cameraCallback);
        return _instance;
    }

    public Camera.Parameters getCameraParameter() {
        return mCameraParameter;
    }

    public void setCameraParameter(Camera.Parameters mCameraParameter) {
        this.mCameraParameter = mCameraParameter;
        if (mPreviewRunning) {
            mCamera.stopPreview();
            mCamera.setParameters(mCameraParameter);
            mCamera.startPreview();
        } else {
            mCamera.setParameters(mCameraParameter);
        }
    }

    public void setOnCameraInitializedListener(CameraInitializedCallback releaseListener) {
        mCameraInitializedCallback = releaseListener;
    }

    public ImageView getImageView() {
        return imageView;
    }

    public void startCamera() {

        Log.d(TAG, "startCamera");
        stopAndReleaseCamera();

        try {

            if (CameraConfig.CAMERA == CameraType.FRONT_CAMERA) {
                mCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_FRONT);
            } else {
                mCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_BACK);
            }

            configureCamera();

            mCamera.setPreviewDisplay(mSurfaceHolder);

            if (CameraConfig.ASYNC_CAMERA) {
                mCamera.setPreviewCallback((AsyncCameraCallback) mCameraCallback);
            }

            mCamera.startPreview();
            mPreviewRunning = true;

            mSurfaceView.requestLayout();

            if (mCameraInitializedCallback != null) {
                mCameraInitializedCallback.onCameraIsInitialized();
            }

        } catch (IOException e) {
            mCamera.release();
            mCamera = null;
            e.printStackTrace();
        }
    }

    public void configureCamera(CameraOrientationActivity.Orientation orientation, int width, int height) {
        Log.d(TAG, "configureCameraWithValues");
        stopAndReleaseCamera();
        mSurfaceOrientation = orientation;
        mWindowWidth = width;
        mWindowHeight = height;
        startCamera();
    }

    public Integer getPreviewFormat() {
        return mPreviewFormat;
    }

    public void configureCamera() {
        Log.d(TAG, "configureCamera");
        if (mCamera != null) {

            mCameraParameter = mCamera.getParameters();

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

            rotateOrientation();

            mCamera.setParameters(mCameraParameter);
        } else {
            Log.d(TAG, "Camera is NULL");
        }
    }

    public void rotateOrientation(CameraOrientationActivity.Orientation orientation) {
        Log.d(TAG, "configureCameraWithValues");
        mSurfaceOrientation = orientation;
        rotateOrientation();
    }

    private void rotateOrientation() {
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

    public void stopAndReleaseCamera() {
        Log.d(TAG, "stopAndReleaseCamera");
        if (mCamera != null) {
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
            mPreviewRunning = false;
        }
    }

    public void releaseView() {
        Log.d(TAG, "releaseView");
        if (mSurfaceView != null) {
            mSurfaceView.destroyDrawingCache();
        }
    }

    public CameraOrientationActivity.Orientation getOrientation() {
        return mSurfaceOrientation;
    }

    public List<String> getSupportedFocusModes() {
        return mCameraParameter.getSupportedFocusModes();
    }

    public void setFocusMode(String supportedFocusMode) {
        Log.d(TAG, "setFocusMode=" + supportedFocusMode);
        mCamera.stopPreview();
        mCameraParameter.setFocusMode(supportedFocusMode);
        mCamera.setParameters(mCameraParameter);
        mCamera.startPreview();
    }

    public List<String> getSupportedFlashModes() {
        return mCameraParameter.getSupportedFlashModes();
    }

    public void setFlashMode(String supportedFlashMode) {
        Log.d(TAG, "setFlashMode=" + supportedFlashMode);
        mCameraParameter.setFlashMode(supportedFlashMode);
        if (mPreviewRunning) {
            mCamera.stopPreview();
            mCamera.setParameters(mCameraParameter);
            mCamera.startPreview();
        } else {
            mCamera.setParameters(mCameraParameter);
        }
    }

    public void setAutoFocusCallback(AutoFocusCallback autoFocusCallback) {
        mCamera.autoFocus(autoFocusCallback);
    }

    public void setPreviewDisplay(SurfaceHolder holder) {
        try {
            mCamera.setPreviewDisplay(holder);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void setCameraCallbackBuffer(byte[] data) {
        if (mCamera != null) {
            mCamera.addCallbackBuffer(data);
        } else {
            Log.d(TAG, "skipped addCallbackBuffer (mCamera==null)");
        }
    }

    public enum CameraType {
        FRONT_CAMERA, BACK_CAMERA
    }

}
