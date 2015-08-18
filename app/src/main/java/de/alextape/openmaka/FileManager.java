package de.alextape.openmaka;

import android.content.Context;
import android.util.Log;

import org.apache.commons.io.IOUtils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class FileManager {

    private static final String TAG = "OpenMaka::FileManager";

    private Context context;

    public static String STORAGE_PATH;
    public static String CONFIG_FILE = "/config/config.xml";
    public static String CONFIG_FILE_PATH;
    public static String STATISTICS_FILE = "/statistics.csv";
    public static String STATISTICS_FILE_PATH;
    public static String[] STORAGE_FOLDERS = new String[]{"config", "images"};
    public static String[] COPY_ASSETS = new String[]{"images/card.jpg", "images/card_frame.jpg", "config/config.xml"};

    public FileManager(Context context) {
        this.context = context;
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    public void copyAssets() {

        File storage = context.getExternalFilesDir(null);
        assert storage != null;
        storage.mkdirs();

        STORAGE_PATH = storage.getPath();
        CONFIG_FILE_PATH = storage.getPath() + CONFIG_FILE;
        STATISTICS_FILE_PATH = storage.getPath() + STATISTICS_FILE;

        // TODO save destination
        Log.i(TAG, "Copy assets.. " + storage.getPath());
        String configFile = null;

        try {
            InputStream inputStream = context.getAssets().open("config/config.xml");
            configFile = IOUtils.toString(inputStream, "UTF-8");

            for (String folder : STORAGE_FOLDERS) {
                File f = new File(storage.getPath() + "/" + folder);
                f.mkdirs();
            }

            for (String filename : COPY_ASSETS) {
                String filepath = storage.getPath() + "/" + filename;
                InputStream in;
                OutputStream out;
                try {
                    in = context.getAssets().open(filename);
                    out = new FileOutputStream(filepath);
                    copyFile(in, out);
                    in.close();
                    out.flush();
                    out.close();
                } catch (Exception e) {
                    Log.e("tag", e.getMessage());
                }

                Log.i(TAG, "filepath: " + filepath);
                configFile = configFile.replace(filename, filepath);
            }


        } catch (Exception e) {

            Log.i(TAG, "BUG:" + e.getMessage());
            e.printStackTrace();
        }
    }

    private void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

    @Deprecated
    private void writeBytesToFile(InputStream is, File file) throws Exception {
        try {
            final FileOutputStream output = new FileOutputStream(file);
            try {
                try {
                    final byte[] buffer = new byte[1024];
                    int read;

                    while ((read = is.read(buffer)) != -1)
                        output.write(buffer, 0, read);

                    output.flush();
                } finally {
                    output.close();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        } finally {
            is.close();
        }
    }
}
