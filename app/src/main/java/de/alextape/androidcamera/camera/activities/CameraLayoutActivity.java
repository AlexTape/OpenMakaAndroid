package de.alextape.androidcamera.camera.activities;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import de.alextape.openmaka.R;

/**
 * This class is the root activity object to define the environment of all subclasses.
 */
public class CameraLayoutActivity extends Activity {

    protected View layoutView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().requestFeature(Window.FEATURE_ACTION_BAR_OVERLAY);

        // TODO remove this later
        setContentView(R.layout.open_maka_layout);
        this.layoutView = this.findViewById(R.id.layoutContainer);
    }

}
