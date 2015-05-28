package de.alextape.openmaka;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.MotionEvent;

/**
 * Created by thinker on 11.05.15.
 */
class GlBufferView extends GLSurfaceView {
    private static String TAG = "GLAndroid";

    public GlBufferView(Context context, AttributeSet attrs) {
        super(context, attrs);
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        /* We need to choose an EGLConfig that matches the format of
         * our surface exactly. This is going to be done in our
         * custom config chooser. See ConfigChooser class definition
         * below.
         */


        //setZOrderOnTop(true);
        setRenderer(new MyRenderer());
		/*
		requestFocus();
		setFocusableInTouchMode(true);
		*/
    }
    @Override
    public boolean onTouchEvent(final MotionEvent event)
    {
        queueEvent(new Runnable() {
            public void run() {
                MainActivity.native_touch_event(event.getX(), event.getY(), event.getAction());
            }
        });

        return true;
    }

    @Override
    public boolean onKeyDown(final int keyCode, final KeyEvent event)
    {
        queueEvent(new Runnable() {
            public void run() {
                MainActivity.native_key_event(keyCode, event.getAction());
            }
        });
        return false;
    }

    @Override
    public boolean onKeyUp(final int keyCode, final KeyEvent event)
    {
        queueEvent(new Runnable() {
            public void run() {
                MainActivity.native_key_event(keyCode, event.getAction());
            }
        });

        return false;
    }


}
