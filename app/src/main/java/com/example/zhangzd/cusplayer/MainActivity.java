package com.example.zhangzd.cusplayer;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;

import java.io.File;


public class MainActivity extends AppCompatActivity {

    CusPlayer player;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        SurfaceView surfaceView = findViewById(R.id.surfaceView);

        player = new CusPlayer(surfaceView);
    }


    public void start(View view) {
        File file  = new File(Environment.getExternalStorageDirectory(),"vtest.mp4");
        player.start(file);
    }
}
