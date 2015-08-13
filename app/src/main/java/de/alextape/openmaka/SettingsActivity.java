package de.alextape.openmaka;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;

import org.w3c.dom.Document;
import org.xml.sax.SAXException;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringWriter;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

public class SettingsActivity extends Activity implements CompoundButton.OnCheckedChangeListener {

    private CheckBox mStatisticBox;
    private CheckBox mDebugBox;

    private EditText mMaxResolutionEditText;

    private EditText mMinimumInliers;
    private EditText mMinimumMatches;
    private EditText mDistanceRatioEditText;
    private EditText mGroupEditText;
    private EditText mRansacEditText;

    private EditText mMaxConersEditText;
    private EditText mQualityLevelEditText;
    private EditText mMinimumDistanceEditText;

    private boolean statisticsMode;
    private boolean debugMode;

    private int maxImageSize;

    private int minimumInliers;
    private int minimumMatches;
    private float nnDistanceRatio;
    private int kGroups;
    private float ransacReprojectionThreshold;

    private int maxCorners;
    private float qualityLevel;
    private int minimumDistance;

    private Document document;



    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);

        setContentView(R.layout.settings_layout);

        try {

            findViews();

            readXml("/storage/emulated/0/Android/data/de.alextape.openmaka/files/config/config.xml");

            fillViews();

        } catch (IOException e) {
            e.printStackTrace();
        } catch (SAXException e) {
            e.printStackTrace();
        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        }

    }

    private void findViews() {

        mStatisticBox = (CheckBox) findViewById(R.id.check_statistic);
        mStatisticBox.setOnCheckedChangeListener(this);

        mDebugBox = (CheckBox) findViewById(R.id.check_debug);
        mDebugBox.setOnCheckedChangeListener(this);

        mMaxResolutionEditText = (EditText) findViewById(R.id.edit_maximal_image_size);

        mMinimumInliers = (EditText) findViewById(R.id.edit_minimum_inliers);
        mMinimumMatches = (EditText) findViewById(R.id.edit_minimum_matches);
        mDistanceRatioEditText = (EditText) findViewById(R.id.edit_nn_distance_ratio);
        mGroupEditText = (EditText) findViewById(R.id.edit_k_groups);
        mRansacEditText = (EditText) findViewById(R.id.edit_ransac_reprojection_threshold);

        mMaxConersEditText = (EditText) findViewById(R.id.edit_max_corners);
        mQualityLevelEditText = (EditText) findViewById(R.id.edit_quality_level);
        mMinimumDistanceEditText = (EditText) findViewById(R.id.edit_minimum_distance);

    }

    private void readXml(String filePath) throws IOException, SAXException, ParserConfigurationException {

        File inputFile = new File(filePath);
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();

        DocumentBuilder db = dbf.newDocumentBuilder();

        document = db.parse(inputFile);

        try {

            statisticsMode = Boolean.parseBoolean(document.getElementsByTagName("statisticsMode").item(0).getFirstChild().getTextContent());
            debugMode = Boolean.parseBoolean(document.getElementsByTagName("debugMode").item(0).getFirstChild().getTextContent());

            maxImageSize = Integer.parseInt(document.getElementsByTagName("maxImageSize").item(0).getFirstChild().getTextContent());

            minimumInliers = Integer.parseInt(document.getElementsByTagName("minimumInliers").item(0).getFirstChild().getTextContent());
            minimumMatches = Integer.parseInt(document.getElementsByTagName("minimumMatches").item(0).getFirstChild().getTextContent());
            nnDistanceRatio = Float.parseFloat(document.getElementsByTagName("nnDistanceRatio").item(0).getFirstChild().getTextContent());
            kGroups = Integer.parseInt(document.getElementsByTagName("kGroups").item(0).getFirstChild().getTextContent());
            ransacReprojectionThreshold = Float.parseFloat(document.getElementsByTagName("ransacReprojectionThreshold").item(0).getFirstChild().getTextContent());

            maxCorners = Integer.parseInt(document.getElementsByTagName("maxCorners").item(0).getFirstChild().getTextContent());
            qualityLevel = Float.parseFloat(document.getElementsByTagName("qualityLevel").item(0).getFirstChild().getTextContent());
            minimumDistance = Integer.parseInt(document.getElementsByTagName("minimumDistance").item(0).getFirstChild().getTextContent());

        } catch (NumberFormatException e) {
            Log.d("OpenMaka","XML contains invalid values..");
            e.printStackTrace();
        }
    }

    private void writeXml(String filePath) throws TransformerException {

        TransformerFactory transformerFactory = TransformerFactory.newInstance();
        Transformer transformer = transformerFactory.newTransformer();
        DOMSource source = new DOMSource(document);

        StreamResult result = new StreamResult(new StringWriter());

        //t.setParameter(OutputKeys.INDENT, "yes");
        transformer.setOutputProperty(OutputKeys.INDENT, "yes");
        transformer.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "5");
        transformer.transform(source, result);

        //writing to file
        FileOutputStream fop = null;
        File outputFile;
        try {

            outputFile = new File(filePath);
            fop = new FileOutputStream(outputFile);

            // if file doesnt exists, then create it
            if (!outputFile.exists()) {
                outputFile.createNewFile();
            }

            // get the content in bytes
            String xmlString = result.getWriter().toString();
            System.out.println(xmlString);
            byte[] contentInBytes = xmlString.getBytes();

            fop.write(contentInBytes);
            fop.flush();
            fop.close();

        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (fop != null) {
                    fop.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void fillViews() {

        mStatisticBox.setChecked(statisticsMode);
        mDebugBox.setChecked(debugMode);

        mMaxResolutionEditText.setText(String.valueOf(maxImageSize));

        mMinimumInliers.setText(String.valueOf(minimumInliers));
        mMinimumMatches.setText(String.valueOf(minimumMatches));
        mDistanceRatioEditText.setText(String.valueOf(nnDistanceRatio));
        mGroupEditText.setText(String.valueOf(kGroups));
        mRansacEditText.setText(String.valueOf(ransacReprojectionThreshold));

        mMaxConersEditText.setText(String.valueOf(maxCorners));
        mQualityLevelEditText.setText(String.valueOf(qualityLevel));
        mMinimumDistanceEditText.setText(String.valueOf(minimumMatches));

    }

    @Override
    public void onBackPressed() {
        try {

            readViews();

        } catch (TransformerException e) {
            e.printStackTrace();
        }
        super.onBackPressed();
    }

    private void readViews() throws TransformerException {

        try {

            statisticsMode = mStatisticBox.isChecked();
            debugMode = mDebugBox.isChecked();

            maxImageSize = Integer.parseInt(mMaxResolutionEditText.getText().toString());

            minimumInliers = Integer.parseInt(mMinimumInliers.getText().toString());
            minimumMatches = Integer.parseInt(mMinimumMatches.getText().toString());
            nnDistanceRatio = Float.parseFloat(mDistanceRatioEditText.getText().toString());
            kGroups = Integer.parseInt(mGroupEditText.getText().toString());
            ransacReprojectionThreshold = Float.parseFloat(mRansacEditText.getText().toString());

            maxCorners = Integer.parseInt(mMaxConersEditText.getText().toString());
            qualityLevel = Float.parseFloat(mQualityLevelEditText.getText().toString());
            minimumMatches = Integer.parseInt(mMinimumDistanceEditText.getText().toString());

            document.getElementsByTagName("statisticsMode").item(0).getFirstChild().setTextContent(String.valueOf(statisticsMode));
            document.getElementsByTagName("debugMode").item(0).getFirstChild().setTextContent(String.valueOf(debugMode));

            document.getElementsByTagName("maxImageSize").item(0).getFirstChild().setTextContent(String.valueOf(maxImageSize));

            document.getElementsByTagName("minimumInliers").item(0).getFirstChild().setTextContent(String.valueOf(minimumInliers));
            document.getElementsByTagName("minimumMatches").item(0).getFirstChild().setTextContent(String.valueOf(minimumMatches));
            document.getElementsByTagName("nnDistanceRatio").item(0).getFirstChild().setTextContent(String.valueOf(nnDistanceRatio));
            document.getElementsByTagName("kGroups").item(0).getFirstChild().setTextContent(String.valueOf(kGroups));
            document.getElementsByTagName("ransacReprojectionThreshold").item(0).getFirstChild().setTextContent(String.valueOf(ransacReprojectionThreshold));

            document.getElementsByTagName("maxCorners").item(0).getFirstChild().setTextContent(String.valueOf(maxCorners));
            document.getElementsByTagName("qualityLevel").item(0).getFirstChild().setTextContent(String.valueOf(qualityLevel));
            document.getElementsByTagName("minimumMatches").item(0).getFirstChild().setTextContent(String.valueOf(minimumMatches));

            writeXml("/storage/emulated/0/Android/data/de.alextape.openmaka/files/config/config.xml");

        } catch (NumberFormatException e) {
            Log.d("OpenMaka","View contains invalid values..");
            e.printStackTrace();
        }

    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        switch (buttonView.getId()) {
            case R.id.check_statistic:
                statisticsMode = isChecked;
                break;
            case R.id.check_debug:
                debugMode = isChecked;
                break;
        }
    }

}
