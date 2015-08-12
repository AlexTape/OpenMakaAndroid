package de.alextape.openmaka;

import android.app.Activity;
import android.os.Bundle;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Spinner;

public class SettingsActivity extends Activity implements CompoundButton.OnCheckedChangeListener {


    private Spinner mResolutionSpinner;
    private CheckBox mStatisticBox;
    private CheckBox mDebugBox;
    private EditText mDistanceRationEditText;
    private EditText mGroupEditText;
    private EditText mRansacEditText;
    private EditText mMaxConersEditText;
    private EditText mQualityLevelEditText;
    private EditText mMinimumDistanceEditText;


    private String mResolution;

    private boolean mStatisticMode = false;
    private boolean mDebugMode = false;

    private float mDistanceRation;
    private int mKgroup;
    private float mRansacReprojection;

    private int mMaxCorners;
    private float mQualityLevel;
    private int mMinimumDistance;

    // handle the position for selection
    private int mSpinnerPosition;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.settings_layout);
        findViews();
        //fillViews();
    }

    private void findViews() {
        mResolutionSpinner = (Spinner) findViewById(R.id.spinner_processing_resolution);
        mStatisticBox = (CheckBox) findViewById(R.id.check_statistic);
        mStatisticBox.setOnCheckedChangeListener(this);
        mDebugBox = (CheckBox) findViewById(R.id.check_debug);
        mDistanceRationEditText = (EditText) findViewById(R.id.edit_distance_ratio);
        mGroupEditText = (EditText) findViewById(R.id.edit_k_group);
        mRansacEditText = (EditText) findViewById(R.id.edit_ransac_reprojection);
        mMaxConersEditText = (EditText) findViewById(R.id.edit_max_corners);
        mQualityLevelEditText = (EditText) findViewById(R.id.edit_quality_level);
        mMinimumDistanceEditText = (EditText) findViewById(R.id.edit_minimum_distance);
    }

    private void fillViews() {

        // get the values from pref and add to fields
        // like....
        // mResolutionSpinner = getPrefs().getInt("",0);

        mResolutionSpinner.setSelection(mSpinnerPosition);

        mStatisticBox.setChecked(mStatisticMode);
        mDebugBox.setChecked(mDebugMode);


        mDistanceRationEditText.setText(String.valueOf(mDistanceRation));
        mGroupEditText.setText(String.valueOf(mKgroup));
        mRansacEditText.setText(String.valueOf(mRansacReprojection));

        mMaxConersEditText.setText(String.valueOf(mMaxCorners));
        mQualityLevelEditText.setText(String.valueOf(mQualityLevel));
        mMinimumDistanceEditText.setText(String.valueOf(mMinimumDistance));

    }

    @Override
    public void onBackPressed() {
        //saveToPrefs();
        super.onBackPressed();
    }

    private void saveToPrefs() {
        mResolution = mResolutionSpinner.getSelectedItem().toString();
        mSpinnerPosition = mResolutionSpinner.getSelectedItemPosition();

        mDistanceRation = Float.parseFloat(mDistanceRationEditText.getText().toString());
        mKgroup = Integer.parseInt(mGroupEditText.getText().toString());
        mRansacReprojection = Float.parseFloat(mRansacEditText.getText().toString());
        mMaxCorners = Integer.parseInt(mMaxConersEditText.getText().toString());
        mQualityLevel = Float.parseFloat(mQualityLevelEditText.getText().toString());
        mMinimumDistance = Integer.parseInt(mMinimumDistanceEditText.getText().toString());

        // write values to prefs...
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        switch (buttonView.getId()) {
            case R.id.check_statistic:
                mStatisticMode = isChecked;
                break;
            case R.id.check_debug:
                mDebugMode = isChecked;
                break;
        }
    }

}
