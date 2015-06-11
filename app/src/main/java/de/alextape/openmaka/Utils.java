//package de.alextape.openmaka;
//
///**
// * Created by thinker on 28.05.15.
// */
//
//import android.content.res.AssetManager;
//
//import java.io.File;
//
//
///**
// * Utils functions.
// */
//public class Utils {
//
//    private static final String TAG = Utils.class.getSimpleName();
//
//    public static void CopyAssets(AssetManager assetManager, File filesDir, int modePrivate) {
//
////
////        String[] files = null;
////        try {
////            files = assetManager.list("");
////        } catch (IOException e) {
////            Log.e("tag", e.getMessage());
////        }
////
////            String[] configFiles = new String[] {"config/camera_matrix_qcam.txt", "config/db.txt", "config/visualWord.bin", "config/vw_index.txt", "slides/slide.xml", "miku/miku.xml"};
////
////            String[] folders = new String[] {"config", "slides", "miku"};
////            for (String folder: folders) {
////                File f = new File(filesDir.getPath() + "/" + folder);
////                f.mkdirs();
////            }
//
//    }
//
//    public static void doJob() {
//
////
////        String configFile = null;
////        try {
////            String[] configFiles = new String[] {"config/camera_matrix_qcam.txt", "config/db.txt", "config/visualWord.bin", "config/vw_index.txt", "slides/slide.xml", "miku/miku.xml"};
////            File externalStorage = this.getExternalFilesDir(null);
////            String[] folders = new String[] {"config", "slides", "miku"};
////            for (String folder: folders) {
////                File f = new File(externalStorage.getPath() + "/" + folder);
////                f.mkdirs();
////            }
////            for (int i= 0; i < configFiles.length; i++) {
////                String filename = configFiles[i];
////                String filepath = externalStorage.getPath() + "/" + filename;
////                File file = new File(filepath);
////                if (!file.exists()) {
////                    InputStream is = getAssets().open(filename);
////                    writeBytesToFile(is, file);
////                }
////                Log.i(TAG, "filepath: " + filepath);
////                configFile = configFile.replace(filename, filepath);
////                configFile = configFile.replace("Visual","ARGH");
////            }
////        } catch (Exception e) {
////            Log.i(TAG, "BUG:" + e.getMessage());
////            e.printStackTrace();
////        }
////
////
////        String[] files = null;
////        try {
////            files = assetManager.list("");
////        } catch (IOException e) {
////            Log.e("tag", e.getMessage());
////        }
////        File f = new File(sdcardPath + "/" + folderName);
////        if (f.isFile()) {
////            throw new RuntimeException("Folder " + sdcardPath + "/" + folderName + " is a file");
////        } else if (!f.isDirectory()) {
////            f.mkdir();
////        }
////
////        for (String filename : files) {
////            InputStream in = null;
////            OutputStream out = null;
////            try {
////                in = assetManager.open(filename);
////
////                f = new File(sdcardPath + "/" + folderName + "/" + filename);
////                if (f.exists()) {
////                    Log.i("gamekit", "fileexists!");
////                    continue;
////                }
////
////                out = new FileOutputStream(sdcardPath + "/" + folderName + "/" + filename);
////                copyFile(in, out);
////                in.close();
////                in = null;
////                out.flush();
////                out.close();
////                out = null;
////            } catch (Exception e) {
////                Log.e("tag", e.getMessage());
////            }
////        }
//    }
//
//
//}
