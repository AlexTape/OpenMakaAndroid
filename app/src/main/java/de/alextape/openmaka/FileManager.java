package de.alextape.openmaka;

import android.content.Context;
import android.util.Log;

import org.apache.commons.io.IOUtils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by thinker on 11.06.15.
 */
public class FileManager {

    private static final String TAG = "OpenMaka::FileManager";

    private Context context;

    public FileManager(Context context) {
        this.context = context;
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    public void copyAssets() {

        // TODO save destination
        Log.i(TAG, "Copy assets.. /storage/emulated/0/Android/data/de.alextape.openmaka/files/");
        String configFile = null;

        try {
            InputStream inputStream = context.getAssets().open("config/config.xml");
            configFile = IOUtils.toString(inputStream, "UTF-8");

            String[] dataFiles = new String[]{"marker/book.jpg","config/config.xml"};

            File storage = context.getExternalFilesDir(null);
            storage.mkdirs();

            String[] folders = new String[]{"config", "marker"};

            for (String folder : folders) {
                File f = new File(storage.getPath() + "/" + folder);
                f.mkdirs();
            }

            for (int i = 0; i < dataFiles.length; i++) {
                String filename = dataFiles[i];
                String filepath = storage.getPath() + "/" + filename;
                File file = new File(filepath);
                if (!file.exists()) {

                    InputStream in = null;
                    OutputStream out = null;
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
