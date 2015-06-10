package de.alextape.openmaka;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.ListView;

import org.apache.commons.io.IOUtils;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

public class MainActivity extends Activity implements CvCameraViewListener2, View.OnClickListener, AdapterView.OnItemClickListener {

    private static final String TAG = MainActivity.class.getSimpleName();

    private static boolean VIEW_MODE_OBJECT_DETECTION = false;
    private static boolean VIEW_MODE_TRACKING = false;
    private static boolean VIEW_MODE_OPEN_GL = false;

    private static boolean initizalized = false;

    private Mat mRgba;
    private Mat mIntermediateMat;
    private Mat mGray;

    private CameraBridgeViewBase mOpenCvCameraView;

    public long time = 0;
    public static int result = 0;

    private ArrayList<String> menuItems;
    private Dialog menuDialog;

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.i(TAG, "OpenCV loaded successfully");

                    // Load native library after(!) OpenCV initialization

                    // TODO load libs
                    //System.loadLibrary("nonfree");
                    System.loadLibrary("native_openmaka");

                    mOpenCvCameraView.enableView();
                }
                break;
                default: {
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    public MainActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.tutorial2_surface_view);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.tutorial2_activity_surface_view);
        mOpenCvCameraView.setCvCameraViewListener(this);

        //Utils.CopyAssets(getResources().getAssets(), this.getFilesDir(), this.MODE_PRIVATE);
        copyAssets();

        if (!OpenCVLoader.initDebug()) {
            Log.e(TAG, "Cannot connect to OpenCV Manager");
        } else {
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }

        ImageButton menuButton = (ImageButton) findViewById(R.id.menu_button);
        menuButton.setOnClickListener(this);

        menuItems = new ArrayList<>();
        menuItems.clear();
        menuItems.add("Trigger ObjectDetection");
        menuItems.add("Trigger Tracking");
        menuItems.add("Trigger OpenGL");
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume() {
        super.onResume();
        if (!OpenCVLoader.initDebug()) {
            Log.e(TAG, "Cannot connect to OpenCV Manager");
        } else {
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
        mRgba = new Mat(height, width, CvType.CV_8UC4);
        mIntermediateMat = new Mat(height, width, CvType.CV_8UC4);
        mGray = new Mat(height, width, CvType.CV_8UC1);
    }

    public void onCameraViewStopped() {
        mRgba.release();
        mGray.release();
        mIntermediateMat.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {

        mRgba = inputFrame.rgba();
        mGray = inputFrame.gray();
        long mRgbaAddr = mRgba.getNativeObjAddr();
        long mGrayAddr = mGray.getNativeObjAddr();

        if (!initizalized) {
            String configFile = "/storage/emulated/0/Android/data/de.alextape.openmaka/files/config/config.xml";
            initizalized = native_initialize(mRgbaAddr, configFile);
        }

        if (VIEW_MODE_OBJECT_DETECTION || VIEW_MODE_TRACKING || VIEW_MODE_OPEN_GL) {
            if (SystemClock.uptimeMillis() - time >= 1000) {
                native_displayFunction(mRgbaAddr, mGrayAddr);
            }
        }

        return mRgba;
    }

    //    static native void native_findFeatures(long matAddrGr, long matAddrRgba);

    static native int native_displayFunction(long matAddrRgba, long matAddrGray);

    static native boolean native_initialize(long matAddrRgba, String configPath);

    static native void native_setObjectDetection(boolean isActive);

    static native void native_setTracking(boolean isActive);

    static native void native_setOpenGL(boolean isActive);

    static native void native_start();

    static native void native_gl_resize(int w, int h);

    static native void native_gl_render();

    static native void native_key_event(int key, int status);

    static native void native_touch_event(float x, float y, int status);

    @Override
    public void onClick(View v) {
        int id = v.getId();
        switch (id) {
            case R.id.menu_button:
                showMenu();
                break;
            default:
                break;
        }
    }

    private void showMenu() {
        //todo integrate menu
        LayoutInflater inflater = getLayoutInflater();
        View view = inflater.inflate(R.layout.menu_layout, null);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setView(view);
        builder.setCancelable(true);
        view.setRotation(270);

        ListView listView = (ListView) view.findViewById(R.id.menu_list);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, android.R.id.text1, menuItems);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(this);

        this.menuDialog = builder.create();
        this.menuDialog.show();
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

        switch (position) {
            case 0:
                VIEW_MODE_OBJECT_DETECTION = !VIEW_MODE_OBJECT_DETECTION;
                native_setObjectDetection(VIEW_MODE_OBJECT_DETECTION);
                break;
            case 1:
                VIEW_MODE_TRACKING = !VIEW_MODE_TRACKING;
                native_setTracking(VIEW_MODE_TRACKING);
                break;
            case 2:
                VIEW_MODE_OPEN_GL = !VIEW_MODE_OPEN_GL;
                native_setOpenGL(VIEW_MODE_OPEN_GL);
                break;
        }
        if (this.menuDialog != null && this.menuDialog.isShowing()) {
            this.menuDialog.dismiss();
        }
    }

    private void copyAssets() {

        // TODO save destination
        Log.i(TAG, "Copy assets.. /storage/emulated/0/Android/data/de.alextape.openmaka/files/");
        String configFile = null;

        try {
            InputStream inputStream = getAssets().open("config/config.xml");
            configFile = IOUtils.toString(inputStream, "UTF-8");

            String[] dataFiles = new String[]{"config/config.xml", "config/camera_matrix_qcam.txt", "config/db.txt", "config/visualWord.bin", "config/vw_index.txt", "slides/slide.xml", "miku/miku.xml"};

            File storage = this.getExternalFilesDir(null);
            storage.mkdirs();

            String[] folders = new String[]{"config", "slides", "miku"};

            for (String folder : folders) {
                File f = new File(storage.getPath() + "/" + folder);
                f.mkdirs();
            }

            for (int i = 0; i < dataFiles.length; i++) {
                String filename = dataFiles[i];
                String filepath = storage.getPath() + "/" + filename;
                File file = new File(filepath);
                if (!file.exists()) {

                    InputStream in = null;
                    OutputStream out = null;
                    try {
                        in = getAssets().open(filename);
                        out = new FileOutputStream(filepath);
                        copyFile(in, out);
                        in.close();
                        out.flush();
                        out.close();
                    } catch (Exception e) {
                        Log.e("tag", e.getMessage());
                    }
                }

                Log.i(TAG, "filepath: " + filepath);
                configFile = configFile.replace(filename, filepath);
                //configFile = configFile.replace("Visual", "ARGH");
            }


        } catch (Exception e) {

            Log.i(TAG, "BUG:" + e.getMessage());
            e.printStackTrace();
        }
    }

    private void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

    @Deprecated
    private void writeBytesToFile(InputStream is, File file) throws Exception {

        Log.i("CHECK", file.getAbsolutePath());

        try {
            final FileOutputStream output = new FileOutputStream(file);
            try {
                try {
                    final byte[] buffer = new byte[1024];
                    int read;

                    while ((read = is.read(buffer)) != -1)
                        output.write(buffer, 0, read);

                    output.flush();
                } finally {
                    output.close();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        } finally {
            is.close();
        }
    }

    public static boolean isViewModeObjectDetection() {
        return VIEW_MODE_OBJECT_DETECTION;
    }

    public static boolean isViewModeTracking() {
        return VIEW_MODE_TRACKING;
    }

    public static boolean isViewModeOpenGl() {
        return VIEW_MODE_OPEN_GL;
    }
}



