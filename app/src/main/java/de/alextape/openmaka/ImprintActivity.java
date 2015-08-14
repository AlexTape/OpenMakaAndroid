package de.alextape.openmaka;

import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;

public class ImprintActivity extends Activity {

    private WebView webView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.imprint);
        webView = (WebView) findViewById(R.id.imprint);

        webView.loadUrl("file:///android_asset/imprint.html");

    }

}
