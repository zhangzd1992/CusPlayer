package com.example.zhangzd.cusplayer;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.File;

/**
 * @Description:
 * @Author: zhangzd
 * @CreateDate: 2019-06-11 14:29
 */
public class CusPlayer implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("cusPlayer");
    }

    private SurfaceHolder holder;
    public CusPlayer(SurfaceView surfaceView) {
        SurfaceHolder holder = surfaceView.getHolder();
        if (null != this.holder) {
            this.holder.removeCallback(this);
        }
        this.holder = holder;
        this.holder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        this.holder = holder;
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    public void start(File file) {
        start(file.getPath(),holder.getSurface());
    }

    public native void start(String path,Surface surface);

}
