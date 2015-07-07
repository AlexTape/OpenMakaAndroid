package de.alextape.openmaka;

/**
 * Created by thinker on 11.06.15.
 */
public class NativeController {

    private static final String TAG = "OpenMaka::NativeFunctions";

    private static boolean VIEW_MODE_OBJECT_DETECTION = false;
    private static boolean VIEW_MODE_TRACKING = false;
    private static boolean VIEW_MODE_OPEN_GL = false;

    static {
        System.loadLibrary("native_openmaka");
    }

    /**
     * Application functions.
     */
    public static void start() {
        native_start();
    }

    public static boolean initialize(long matAddrRgba, String configPath) {
        boolean returnThis = false;
        int i = native_initialize(matAddrRgba, configPath);
        if (i == 1) {
            returnThis = true;
        }
        return returnThis;
    }

    private static native void native_start();

    private static native int native_initialize(long matAddrRgba, String configPath);

    /**
     * Display functions.
     */
    public static Integer displayFunction(int width, int height, byte[] data, int[] pixels) {
//        if (VIEW_MODE_OBJECT_DETECTION || VIEW_MODE_TRACKING || VIEW_MODE_OPEN_GL) {
            return native_displayFunction(width, height, data, pixels);
//        }
//        return null;
    }

    public static void glRender() {
        native_glRender();
    }

    public static void glResize(int w, int h) {
        native_glResize(w, h);
    }

    private static native int native_displayFunction(int width, int height, byte[] data, int[] pixels);

    private static native void native_glRender();

    private static native void native_glResize(int w, int h);

    /**
     * Input functions.
     */
    public static void touchEvent(float x, float y, int action) {
        native_touchEvent(x, y, action);
    }

    private static native void native_touchEvent(float x, float y, int action);

    public static void keyEvent(int keyCode, int action) {
        native_keyEvent(keyCode, action);
    }

    private static native void native_keyEvent(int keyCode, int action);

    /**
     * Options getter/setter.
     */
    public static boolean isViewModeObjectDetection() {
        return VIEW_MODE_OBJECT_DETECTION;
    }

    public static void setViewModeObjectDetection(boolean viewModeObjectDetection) {
        VIEW_MODE_OBJECT_DETECTION = viewModeObjectDetection;
        native_setObjectDetection(VIEW_MODE_OBJECT_DETECTION);
    }

    public static boolean isViewModeTracking() {
        return VIEW_MODE_TRACKING;
    }

    public static void setViewModeTracking(boolean viewModeTracking) {
        VIEW_MODE_TRACKING = viewModeTracking;
        native_setTracking(VIEW_MODE_TRACKING);
    }

    public static boolean isViewModeOpenGl() {
        return VIEW_MODE_OPEN_GL;
    }

    public static void setViewModeOpenGl(boolean viewModeOpenGl) {
        VIEW_MODE_OPEN_GL = viewModeOpenGl;
        native_setOpenGL(VIEW_MODE_OPEN_GL);
    }

    private static native void native_setObjectDetection(boolean isActive);

    private static native void native_setTracking(boolean isActive);

    private static native void native_setOpenGL(boolean isActive);
}
