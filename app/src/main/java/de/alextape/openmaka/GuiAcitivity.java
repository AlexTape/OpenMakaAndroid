package de.alextape.openmaka;


import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.text.Layout;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

abstract class GuiAcitivity extends Activity {

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
        // View view = LayoutInflater.from(this).inflate(R.layout.menu_headline, null);
        // menu.findItem(R.id.featureDetectorsHeadline).setOnMenuItemClickListener(null).setActionView(view);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        item.setChecked(item.isChecked());

//        switch (position) {
//            case 0:
//                boolean isDetection = NativeFunctions.isViewModeObjectDetection();
//                NativeFunctions.setViewModeObjectDetection(!isDetection);
//                break;
//            case 1:
//                boolean isTracking = NativeFunctions.isViewModeTracking();
//                NativeFunctions.setViewModeTracking(!isTracking);
//                break;
//            case 2:
//                boolean isOpenGL = NativeFunctions.isViewModeOpenGl();
//                NativeFunctions.setViewModeOpenGl(!isOpenGL);
//                break;
//        }

        Log.d(TAG, "ItemClick: " + item.getItemId());
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



