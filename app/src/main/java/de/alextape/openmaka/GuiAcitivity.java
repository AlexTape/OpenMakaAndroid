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

        infoLineText = getResources().getString(R.string.infoLineText);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu, menu);

        // predeclare checked options
        detector = "SIFT";
        extractor = "SIFT";
        matcher = "FLANN_X";

        // configure native controller
        NativeController.setDetector(detector);
        NativeController.setExtractor(extractor);
        NativeController.setMatcher(matcher);

        // check checkboxes
        menu.findItem(R.id.featureDetector_SIFT).setChecked(true);
        menu.findItem(R.id.featureExtractor_SIFT).setChecked(true);
        menu.findItem(R.id.matcherFlannBased).setChecked(true);

        // set info line
        infoLine.setText(String.format(infoLineText, detector, extractor, matcher));

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
                detector ="SURF";
                break;
            case R.id.featureDetector_SIFT:
                detector ="SIFT";
                break;
            case R.id.featureDetector_FAST:
                detector ="FAST";
                break;
            case R.id.featureDetector_ORB:
                detector ="ORB";
                break;
            case R.id.featureDetector_BRISK:
                detector ="BRISK";
                break;
            case R.id.featureDetector_DENSE:
                detector ="DENSE";
                break;
            case R.id.featureDetector_GFTT:
                detector ="GFTT";
                break;
            case R.id.featureDetector_MSER:
                detector ="MSER";
                break;
            case R.id.featureDetector_STAR:
                detector ="STAR";
                break;
            // extractor switching
            case R.id.featureExtractor_SIFT:
                extractor ="SIFT";
                break;
            case R.id.featureExtractor_BRIEF:
                extractor ="BRIEF";
                break;
            case R.id.featureExtractor_ORB:
                extractor ="ORB";
                break;
            case R.id.featureExtractor_SURF:
                extractor ="SURF";
                break;
            case R.id.featureExtractor_BRISK:
                extractor ="BRISK";
                break;
            case R.id.featureExtractor_FREAK:
                extractor ="FREAK";
                break;
            // matcher switching
            case R.id.matcherBruteForce:
                matcher = "BF_NORM_L2";
                break;
            case R.id.matcherFlannBased:
                matcher = "FLANN_X";
                break;
        }

        NativeController.setDetector(detector);
        NativeController.setExtractor(extractor);
        NativeController.setMatcher(matcher);

        infoLine.setText(String.format(infoLineText, detector, extractor, matcher));

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



