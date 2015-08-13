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

        // predeclare checked options
        menu.findItem(R.id.featureDetector_SIFT).setChecked(true);
        menu.findItem(R.id.featureExtractor_SIFT).setChecked(true);
        menu.findItem(R.id.matcherFlannBased).setChecked(true);

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        item.setChecked(item.isChecked());
        Log.d(TAG, "ItemClick: " + item.getTitle());

        switch (item.getItemId()) {
            case R.id.enableModeObjectDetection:
                boolean isDetection = NativeController.isModeObjectDetection();
                NativeController.setModeObjectDetection(!isDetection);
                break;
            case R.id.enableModeTracking:
                boolean isTracking = NativeController.isModeTracking();
                NativeController.setModeTracking(!isTracking);
                break;
            case R.id.enableModeOpenGL:
                boolean isOpenGL = NativeController.isModeOpenGl();
                NativeController.setModeOpenGl(!isOpenGL);
                break;
            case R.id.generalOptions:
                startActivity(new Intent(this, SettingsActivity.class));
                break;
            case R.id.generalOptionsImprint:
                startActivity(new Intent(this, ImprintActivity.class));
                break;
            // detector switching
            case R.id.featureDetector_SURF:
                NativeController.setDetector("SURF");
                break;
            case R.id.featureDetector_SIFT:
                NativeController.setDetector("SIFT");
                break;
            case R.id.featureDetector_FAST:
                NativeController.setDetector("FAST");
                break;
            case R.id.featureDetector_ORB:
                NativeController.setDetector("ORB");
                break;
            case R.id.featureDetector_BRISK:
                NativeController.setDetector("BRISK");
                break;
            case R.id.featureDetector_DENSE:
                NativeController.setDetector("DENSE");
                break;
            case R.id.featureDetector_GFTT:
                NativeController.setDetector("GFTT");
                break;
            case R.id.featureDetector_MSER:
                NativeController.setDetector("MSER");
                break;
            case R.id.featureDetector_STAR:
                NativeController.setDetector("STAR");
                break;
            // extractor switching
            case R.id.featureExtractor_SIFT:
                NativeController.setExtractor("SIFT");
                break;
            case R.id.featureExtractor_BRIEF:
                NativeController.setExtractor("BRIEF");
                break;
            case R.id.featureExtractor_ORB:
                NativeController.setExtractor("ORB");
                break;
            case R.id.featureExtractor_SURF:
                NativeController.setExtractor("SURF");
                break;
            case R.id.featureExtractor_BRISK:
                NativeController.setExtractor("BRISK");
                break;
            case R.id.featureExtractor_FREAK:
                NativeController.setExtractor("FREAK");
                break;
            // matcher switching
            case R.id.matcherBruteForce:
                NativeController.setMatcher("BF_NORM_L2");
                break;
            case R.id.matcherFlannBased:
                NativeController.setMatcher("FLANN_X");
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



