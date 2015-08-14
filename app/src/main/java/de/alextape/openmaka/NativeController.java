package de.alextape.openmaka;

public class NativeController {

    private static final String TAG = "OpenMaka::NativeFunctions";

    private static boolean MODE_OBJECT_DETECTION = false;
    private static boolean MODE_TRACKING = false;
    private static boolean MODE_OPEN_GL = false;

    private static OnResultListener onResultListener;

    public interface OnResultListener {
        void onResult(int result);
    }

    public static void setOnResultListener(OnResultListener resultListener) {
        onResultListener = resultListener;
    }

    public static void receiveResult(int result) {
        onResultListener.onResult(result);
    }

    /**
     * Basic functions.
     */
    public static boolean initialize(long matAddrRgba, String configPath) {
        boolean returnThis = false;
        int i = native_initialize(matAddrRgba, configPath);
        if (i == 1) {
            returnThis = true;
        }
        return returnThis;
    }

    public static int test(int test, int quantifier) {
        int result = native_test(test, quantifier);
        receiveResult(result);
        return result;
    }

    private static native int native_initialize(long matAddrRgba, String configPath);
    private static native int native_test(int test, int quantifier);

    /**
     * Display functions.
     */
    public static Integer displayFunction(long matAddrRgba, long matAddrGray) {
        System.out.println("display call");
        return native_displayFunction(matAddrRgba, matAddrGray);
    }

    public static void glRender() {
        native_glRender();
    }

    public static void glResize(int w, int h) {
        native_glResize(w, h);
    }

    private static native int native_displayFunction(long matAddrRgba, long matAddrGray);

    private static native void native_glRender();

    private static native void native_glResize(int w, int h);

    /**
     * Options getter/setter.
     */
    public static int setDetector(String type) {
        return native_setDetector(type);
    }

    public static int setExtractor(String type) {
        return native_setExtractor(type);
    }

    public static int setMatcher(String type) {
        return native_setMatcher(type);
    }

    private static native int native_setDetector(String type);
    private static native int native_setExtractor(String type);
    private static native int native_setMatcher(String type);

    public static boolean isModeObjectDetection() {
        return MODE_OBJECT_DETECTION;
    }

    public static void setModeObjectDetection(boolean isObjectDetection) {
        MODE_OBJECT_DETECTION = isObjectDetection;
        native_setModeObjectDetection(MODE_OBJECT_DETECTION);
    }

    public static boolean isModeTracking() {
        return MODE_TRACKING;
    }

    public static void setModeTracking(boolean isModeTracking) {
        MODE_TRACKING = isModeTracking;
        native_setModeTracking(MODE_TRACKING);
    }

    public static boolean isModeOpenGl() {
        return MODE_OPEN_GL;
    }

    public static void setModeOpenGl(boolean isModeOpenGl) {
        MODE_OPEN_GL = isModeOpenGl;
        native_setModeOpenGL(MODE_OPEN_GL);
    }

    private static native void native_setModeObjectDetection(boolean isActive);

    private static native void native_setModeTracking(boolean isActive);

    private static native void native_setModeOpenGL(boolean isActive);


}
