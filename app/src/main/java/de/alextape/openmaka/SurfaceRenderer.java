package de.alextape.openmaka;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.opengles.GL10;

/**
 * Created by thinker on 11.05.15.
 */
class SurfaceRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = "OpenMaka::SurfaceRenderer";

    @Override
    public void onSurfaceCreated(GL10 gl, javax.microedition.khronos.egl.EGLConfig arg1) {
        NativeController.start();
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
        NativeController.glResize(w, h);
    }

    public void onDrawFrame(GL10 gl) {

        if (NativeController.isViewModeOpenGl()) {
            NativeController.glRender();
        } else {
            // clear screen
            gl.glClearColor(0, 0, 0, 0);
            gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
        }

    }

}
