package de.alextape.openmaka;


import android.app.ActionBar;
import android.app.Dialog;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

import java.util.ArrayList;

import de.alextape.androidcamera.camera.HardwareCamera;

public class GuiAcitivity extends HardwareCamera implements View.OnTouchListener {

    private static final String TAG = "OpenMaka::GuiActivity";

    private ArrayList<String> menuItems;
    private Dialog menuDialog;

    public GuiAcitivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    private LinearLayout mContainer;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);

        FileManager fileManager = new FileManager(getApplicationContext());
        fileManager.copyAssets();

        RelativeLayout relativeLayout = (RelativeLayout) findViewById(R.id.layoutContainer);
        relativeLayout.setOnTouchListener(this);

        ActionBar actionBar = getActionBar();

        if (actionBar != null) {
            actionBar.setDisplayShowHomeEnabled(false);
            actionBar.setDisplayHomeAsUpEnabled(false);
            actionBar.setTitle("");
            actionBar.setBackgroundDrawable(new ColorDrawable(Color.parseColor("#0000ffff")));
        }

        System.loadLibrary("native_openmaka");
    }


    @Override
    public void onCameraInitialized() {

//        if (!initialized) {
//            String configFile = "/storage/emulated/0/Android/data/de.alextape.openmaka/files/config/config.xml";
//            initialized = NativeFunctions.initialize(mRgbaAddr, configFile);
//        }

        Log.d(TAG, "onReleaseCamera");

        Integer mode = null;
        String result = null;

        // Return a list of Available Flash modes
        String[] flashOptions = getFlashOptions();
        // pass index of option to setFlashMode(int)
        result = setFlashMode(1);
        if (result == null) {
            Log.d(TAG, "Success");
        } else {
            Log.d(TAG, "Fail: " + result);
        }

        // Returns a list of available Focus modes
        String[] focusOptions = getFlashOptions();
        // pass index of option to setFocusMode(int)
        result = setFocusMode(1);
        if (result == null) {
            Log.d(TAG, "Success");
        } else {
            Log.d(TAG, "Fail: " + result);
        }

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu, menu);
        // View view = LayoutInflater.from(this).inflate(R.layout.menu_headline, null);
        // menu.findItem(R.id.featureDetectorsHeadline).setOnMenuItemClickListener(null).setActionView(view);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        item.setChecked(item.isChecked());

        Log.d(TAG, "ItemClick: " + item.getTitle());

        switch (item.getItemId()) {
            case R.id.featureDetector_FAST:
                boolean isDetection = NativeFunctions.isViewModeObjectDetection();
                NativeFunctions.setViewModeObjectDetection(!isDetection);
                break;
            case 1:
                boolean isTracking = NativeFunctions.isViewModeTracking();
                NativeFunctions.setViewModeTracking(!isTracking);
                break;
            case 2:
                boolean isOpenGL = NativeFunctions.isViewModeOpenGl();
                NativeFunctions.setViewModeOpenGl(!isOpenGL);
                break;
        }

        return true;
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        switch (view.getId()) {
            case R.id.layoutContainer:
                //motionEvent.getAction() == MotionEvent.ACTION_UP
                Log.e("touch", "x: " + motionEvent.getX() + "  y: " + motionEvent.getY());
                break;
            default:
                break;
        }
        return false;
    }

}



