package de.alextape.openmaka;


import android.app.ActionBar;
import android.app.Activity;
import android.app.Dialog;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.widget.LinearLayout;

import java.util.ArrayList;

abstract class GuiAcitivity extends Activity {

    private static final String TAG = "OpenMaka::GuiActivity";

    public GuiAcitivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().requestFeature(Window.FEATURE_ACTION_BAR_OVERLAY);

        setContentView(R.layout.open_maka_layout);

        ActionBar actionBar = getActionBar();

        if (actionBar != null) {
            actionBar.setDisplayShowHomeEnabled(false);
            actionBar.setDisplayHomeAsUpEnabled(false);
            actionBar.setTitle("");
            actionBar.setBackgroundDrawable(new ColorDrawable(Color.parseColor("#0000ffff")));
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        item.setChecked(item.isChecked());

        Log.d(TAG, "ItemClick: " + item.getTitle());

        switch (item.getItemId()) {
            case R.id.featureDetector_FAST:
                boolean isDetection = NativeFunctions.isModeObjectDetection();
                NativeFunctions.setModeObjectDetection(!isDetection);
                break;
            case 1:
                boolean isTracking = NativeFunctions.isModeTracking();
                NativeFunctions.setModeTracking(!isTracking);
                break;
            case 2:
                boolean isOpenGL = NativeFunctions.isModeOpenGl();
                NativeFunctions.setModeOpenGl(!isOpenGL);
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

}



