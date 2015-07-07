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
import android.view.Window;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

import java.util.ArrayList;

import de.alextape.openmaka.camera.AndroidCamera;
import de.alextape.openmaka.camera.CameraController;
import de.alextape.openmaka.storage.FileManager;

public class MainActivity extends AndroidCamera implements View.OnTouchListener {

    private static final String TAG = MainActivity.class.getSimpleName();

    private ArrayList<String> menuItems;
    private Dialog menuDialog;
    private LinearLayout mContainer;

    public MainActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // The activity is being created.
        Log.d(TAG, "onCreate");



        FileManager fileManager = new FileManager(getApplicationContext());
        fileManager.copyAssets();

        RelativeLayout relativeLayout = (RelativeLayout) findViewById(R.id.layoutContainer);
        relativeLayout.setOnTouchListener(this);

        ActionBar actionBar = getActionBar();

        if (actionBar != null) {
            Log.d(TAG, "ARGHARGHARGH");
            actionBar.setDisplayShowHomeEnabled(false);
            actionBar.setDisplayHomeAsUpEnabled(false);
            actionBar.setTitle("");
            actionBar.setBackgroundDrawable(new ColorDrawable(Color.parseColor("#0000ffff")));
        }

    }

    /**
     * This method is used to gather options passed by a user interactions.
     * e.g. Focus mode, Flash mode.. directly AFTER camera is initialized.
     */
    @Override
    public void onCameraInitialized() {

        Log.d(TAG, "onReleaseCamera");

//        // Returns a list of available Focus modes
        String[] focusOptions = CameraController.getInstance().getFocusOptions();
//        // pass index of option to setFocusMode(int)
        String result = CameraController.getInstance().setFocusMode(1);
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
                boolean isDetection = NativeController.isViewModeObjectDetection();
                NativeController.setViewModeObjectDetection(!isDetection);
                break;
            case 1:
                boolean isTracking = NativeController.isViewModeTracking();
                NativeController.setViewModeTracking(!isTracking);
                break;
            case 2:
                boolean isOpenGL = NativeController.isViewModeOpenGl();
                NativeController.setViewModeOpenGl(!isOpenGL);
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
        // The activity has become visible (it is now "resumed").
        Log.d(TAG, "onResume");
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

    @Override
    public void onStart() {
        super.onStart();
        // The activity is about to become visible.
        Log.d(TAG, "onStart");
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



