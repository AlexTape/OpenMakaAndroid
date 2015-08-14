package de.alextape.openmaka;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

abstract class GuiAcitivity extends Activity {

    private static final String TAG = "OpenMaka::GuiActivity";

    private TextView infoLine;

    private String infoLineText;

    private String detector;
    private String extractor;
    private String matcher;

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

        infoLine = (TextView) findViewById(R.id.infoLine);

        infoLineText = getResources().getString(R.string.info_line_text);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu, menu);

        // predeclare checked options
        detector = "SIFT";
        extractor = "SIFT";
        matcher = "FLANN_X";

        // check checkboxes
        menu.findItem(R.id.feature_detector_sift).setChecked(true);
        menu.findItem(R.id.feature_extractor_sift).setChecked(true);
        menu.findItem(R.id.matcher_flannbased).setChecked(true);

        // set info line
        infoLine.setText(String.format(infoLineText, detector, extractor, matcher));

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        item.setChecked(item.isChecked());
        Log.d(TAG, "ItemClick: " + item.getTitle());

        switch (item.getItemId()) {
            case R.id.enable_mode_object_detection:
                boolean isDetection = NativeController.isModeObjectDetection();
                NativeController.setModeObjectDetection(!isDetection);
                break;
            case R.id.enable_mode_tracking:
                boolean isTracking = NativeController.isModeTracking();
                NativeController.setModeTracking(!isTracking);
                break;
            case R.id.enable_mode_opengl:
                boolean isOpenGL = NativeController.isModeOpenGl();
                NativeController.setModeOpenGl(!isOpenGL);
                break;
            case R.id.general_options:
                startActivity(new Intent(this, SettingsActivity.class));
                break;
            case R.id.general_tests:
                startActivity(new Intent(this, TestActivity.class));
                break;
            case R.id.general_options_imprint:
                startActivity(new Intent(this, ImprintActivity.class));
                break;
            // detector switching
            case R.id.feature_detector_surf:
                detector ="SURF";
                update();
                break;
            case R.id.feature_detector_sift:
                detector ="SIFT";
                update();
                break;
            case R.id.feature_detector_fast:
                detector ="FAST";
                update();
                break;
            case R.id.feature_detector_orb:
                detector ="ORB";
                update();
                break;
            case R.id.feature_detector_brisk:
                detector ="BRISK";
                update();
                break;
            case R.id.feature_detector_dense:
                detector ="DENSE";
                update();
                break;
            case R.id.feature_detector_gftt:
                detector ="GFTT";
                update();
                break;
            case R.id.feature_detector_mser:
                detector ="MSER";
                update();
                break;
            case R.id.feature_detector_star:
                detector ="STAR";
                update();
                break;
            // extractor switching
            case R.id.feature_extractor_sift:
                extractor ="SIFT";
                update();
                break;
            case R.id.featureExtractor_brief:
                extractor ="BRIEF";
                update();
                break;
            case R.id.feature_extractor_orb:
                extractor ="ORB";
                update();
                break;
            case R.id.feature_extractor_surf:
                extractor ="SURF";
                update();
                break;
            case R.id.feature_extractor_brisk:
                extractor ="BRISK";
                update();
                break;
            case R.id.feature_extractor_freak:
                extractor ="FREAK";
                update();
                break;
            // matcher switching
            case R.id.matcher_bruteforce:
                matcher = "BF_NORM_L2";
                update();
                break;
            case R.id.matcher_flannbased:
                matcher = "FLANN_X";
                update();
                break;
        }

        infoLine.setText(String.format(infoLineText, detector, extractor, matcher));

        return true;
    }

    private void update() {
        NativeController.setModeObjectDetection(false);
        NativeController.setModeTracking(false);
        Toast.makeText(getApplicationContext(), "Processing disabled.. configuring..", Toast.LENGTH_SHORT).show();
        NativeController.configure(detector, extractor, matcher);
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



