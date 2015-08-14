package de.alextape.openmaka;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class TestActivity extends Activity implements NativeController.OnResultListener, View.OnClickListener {

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

        NativeController.setOnResultListener(this);

    }

    private void runAsyncTests() {

        new AsyncTask<Void, Void, Void>() {

            boolean process = false;
            String test;
            int quantifier;

            @Override
            protected void onPreExecute() {
                super.onPreExecute();
                test = (String) testSpinner.getSelectedItem();
                try {
                    quantifier = Integer.parseInt(quantifierText.getText().toString());
                    process = true;
                } catch (NumberFormatException e) {
                    Toast.makeText(getApplicationContext(),"Please type iteration number!",Toast.LENGTH_LONG).show();
                    process = false;
                }
            }

            @Override
            protected Void doInBackground(Void... params) {
                if (process) {
                    NativeController.test(test, quantifier);
                }
                return null;
            }

            @Override
            protected void onPostExecute(Void aVoid) {
                super.onPostExecute(aVoid);
                if (process) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {

//stuff that updates ui
                            resultButton.setVisibility(View.VISIBLE);

                        }
                    });
                }
            }

        }.execute();

    }

    @Override
    public void onResult(String result) {
        TextView resultTextView = (TextView) getLayoutInflater().inflate(R.layout.text_view, resultContainer, false);
        resultTextView.setText(result);
        resultContainer.addView(resultTextView);

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

                break;
        }

    }

}
