package de.alextape.openmaka;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.ListView;

import java.util.ArrayList;

abstract class GuiAcitivity extends Activity implements View.OnClickListener, AdapterView.OnItemClickListener {

    private static final String TAG = "OpenMaka::GuiActivity";

    private ArrayList<String> menuItems;
    private Dialog menuDialog;

    public GuiAcitivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.open_maka_layout);

        ImageButton menuButton = (ImageButton) findViewById(R.id.menuButton);
        menuButton.setOnClickListener(this);

        menuItems = new ArrayList<String>();
        menuItems.clear();
        menuItems.add("Trigger ObjectDetection");
        menuItems.add("Trigger Tracking");
        menuItems.add("Trigger OpenGL");
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
    public void onClick(View v) {
        int id = v.getId();
        switch (id) {
            case R.id.menuButton:
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
        if (this.menuDialog != null && this.menuDialog.isShowing()) {
            this.menuDialog.dismiss();
        }
    }




}



