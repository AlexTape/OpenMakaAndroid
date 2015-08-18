package de.alextape.openmaka;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

import javax.xml.transform.TransformerException;

public class TestActivity extends AppCompatActivity implements NativeController.OnResultListener, View.OnClickListener {

    private LinearLayout resultContainer;
    private Spinner testSpinner;
    private EditText quantifierText;
    private Button resultButton;
    private Button runButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.test_layout);

        resultContainer = (LinearLayout) findViewById(R.id.result_container);
        quantifierText = (EditText) findViewById(R.id.test_quantifier);
        testSpinner = (Spinner) findViewById(R.id.test_spinner);

        runButton = (Button) findViewById(R.id.run_test_button);
        runButton.setOnClickListener(this);
        resultButton = (Button) findViewById(R.id.save_result_button);
        resultButton.setOnClickListener(this);

        ActionBar actionBar = getSupportActionBar();

        if (actionBar != null) {
            actionBar.setTitle(R.string.general_tests);
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        NativeController.setOnResultListener(this);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        switch(item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                break;
            default:
                break;
        }

        return true;
    }

    private void runAsyncTests() {

        new AsyncTask<Void, Void, Void>() {

            private ProgressDialog dialog = new ProgressDialog(TestActivity.this);

            boolean process = false;

            int test;
            int quantifier;

            int result;

            @Override
            protected void onPreExecute() {
                super.onPreExecute();
                test = testSpinner.getSelectedItemPosition();
                try {
                    this.quantifier = Integer.parseInt(quantifierText.getText().toString());
                    this.dialog.setCanceledOnTouchOutside(false);
                    this.dialog.setCancelable(false);
                    this.dialog.setMessage("Please wait");
                    this.dialog.show();
                    process = true;
                } catch (NumberFormatException e) {
                    Toast.makeText(getApplicationContext(), "Please type iteration number!", Toast.LENGTH_LONG).show();
                    process = false;
                }
            }

            @Override
            protected Void doInBackground(Void... params) {
                if (process) {
                    result = NativeController.test(test, quantifier);
                }
                return null;
            }

            @Override
            protected void onPostExecute(Void aVoid) {
                super.onPostExecute(aVoid);

                // dismiss spinner
                if (dialog.isShowing()) {
                    dialog.dismiss();
                }

                // if successful
                if (process && result == 1) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            resultButton.setVisibility(View.VISIBLE);
                            Toast.makeText(getApplicationContext(), "Results saved..", Toast.LENGTH_LONG).show();
                        }
                    });
                }
            }

        }.execute();

    }

    private void shareResults() {

        Intent intentShareFile = new Intent(Intent.ACTION_SEND);

        // check if existing
        File file = new File(FileManager.STATISTICS_FILE_PATH);

        if (file.exists()) {
            intentShareFile.setType("application/pdf");
            intentShareFile.putExtra(Intent.EXTRA_STREAM, Uri.parse("file://" + FileManager.STATISTICS_FILE_PATH));

            intentShareFile.putExtra(Intent.EXTRA_SUBJECT,
                    "Sharing File...");
            intentShareFile.putExtra(Intent.EXTRA_TEXT, "Sharing File...");

            startActivity(Intent.createChooser(intentShareFile, "Share File"));
        }
    }

    @Override
    public void onResult(final int result) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                TextView resultTextView = (TextView) getLayoutInflater().inflate(R.layout.text_view, resultContainer, false);
                String text;
                switch (result) {
                    case 1:
                        text = testSpinner.getSelectedItem() + " successful";
                        break;
                    default:
                        text = testSpinner.getSelectedItem() + " failed";
                        break;
                }
                resultTextView.setText(text);
                resultContainer.addView(resultTextView);
            }
        });
    }

    @Override
    public void onClick(View v) {

        int id = v.getId();
        switch (id) {
            case R.id.run_test_button:
                resultButton.setVisibility(View.GONE);
                resultContainer.removeAllViews();
                runAsyncTests();
                break;
            case R.id.save_result_button:
                shareResults();
                break;
        }
    }

}
