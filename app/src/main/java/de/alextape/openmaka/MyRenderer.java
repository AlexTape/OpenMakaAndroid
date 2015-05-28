package de.alextape.openmaka;

import android.opengl.GLSurfaceView;
import android.os.SystemClock;

import javax.microedition.khronos.opengles.GL10;

/**
 * Created by thinker on 11.05.15.
 */
class MyRenderer implements GLSurfaceView.Renderer {

    @Override
    public void onSurfaceCreated(GL10 gl, javax.microedition.khronos.egl.EGLConfig arg1) {
			/* do nothing */
        MainActivity.native_start();
    }


    public void onSurfaceChanged(GL10 gl, int w, int h) {
        MainActivity.native_gl_resize(w, h);
    }

    public void onDrawFrame(GL10 gl) {
        time = SystemClock.uptimeMillis();

        if (time >= (frameTime + 1000.0f)) {
            frameTime = time;
            avgFPS += framerate;
            framerate = 0;
        }

        if (time >= (fpsTime + 3000.0f)) {
            fpsTime = time;
            avgFPS /= 3.0f;
            //Log.d("GLAndroid", "FPS: " + Float.toString(avgFPS));
            avgFPS = 0;
        }
        framerate++;

        if(MainActivity.result > 0){
            MainActivity.native_gl_render();
        }else{
            gl.glClearColor(0,0,0,0);
            gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
        }
    }
    public long time = 0;
    public short framerate = 0;
    public long fpsTime = 0;
    public long frameTime = 0;
    public float avgFPS = 0;
}
