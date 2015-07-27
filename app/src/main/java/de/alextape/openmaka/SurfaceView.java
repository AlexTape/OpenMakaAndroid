package de.alextape.openmaka;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

/**
 * Created by thinker on 11.05.15.
 */
public class SurfaceView extends GLSurfaceView {

    private static String TAG = "OpenMaka::SurfaceView";

    boolean isObjectDetection = false;

    long timestamp;

    public SurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        /* We need to choose an EGLConfig that matches the format of
         * our surface exactly. This is going to be done in our
         * custom config chooser. See ConfigChooser class definition
         * below.
         */


        //setZOrderOnTop(true);
        setRenderer(new SurfaceRenderer());
		/*
		requestFocus();
		setFocusableInTouchMode(true);
		*/
    }

    @Override
    public boolean onTouchEvent(final MotionEvent event)
    {

        queueEvent(new Runnable() {

            private long timestamp = System.currentTimeMillis();

            public void run() {
                //NativeFunctions.touchEvent(event.getX(), event.getY(), event.getAction());
                Log.d(TAG, "onTouchEvent=" + event.getAction());

                long time = System.currentTimeMillis();

                if (timestamp + 300 < time) {
                    if (event.getAction() == MotionEvent.ACTION_UP) {

                        isObjectDetection = !isObjectDetection;
                        NativeFunctions.setModeObjectDetection(isObjectDetection);
                        Log.d(TAG, "isObjectDetection=" + isObjectDetection);
                        timestamp = System.currentTimeMillis();
                    }
                }
            }
        });

        return true;
    }

    @Override
    public boolean onKeyDown(final int keyCode, final KeyEvent event)
    {
        queueEvent(new Runnable() {
            public void run() {
                Log.d(TAG, "onKeyDown=" + event.getAction());
                //NativeFunctions.keyEvent(keyCode, event.getAction());
            }
        });
        return false;
    }

    @Override
    public boolean onKeyUp(final int keyCode, final KeyEvent event)
    {
        queueEvent(new Runnable() {
            public void run() {
                Log.d(TAG, "onKeyUp=" + event.getAction());
                //NativeFunctions.keyEvent(keyCode, event.getAction());
            }
        });

        return false;
    }


}
