package com.qsatalk;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
//import android.app.Activity;
import android.widget.TextView;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.view.View;
import android.view.View.OnClickListener;

public class Call extends AppCompatActivity implements OnClickListener {

    private final String TAG = "talk_qsa";
    private Button call_btn, hangup_btn;
    private Button start_video_btn, start_audio_btn;
    private EditText edit_number, edit_ip;
    private TextView tv_stats;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_call);

        init_param_view();
        init_a20_fm2018_param();

        jniLoad talk_qsa= new jniLoad();
        Log.d(TAG, talk_qsa.sendHello());
        Log.d(TAG, talk_qsa.SayHelloInC(" Add Good Work "));
        talk_qsa.callCcode();
        talk_qsa.callCcode1();
        talk_qsa.callCcode2();

        /* 显示View后才能监听按键动作 */
        call_btn.setOnClickListener(this);
        hangup_btn.setOnClickListener(this);
        start_video_btn.setOnClickListener(this);
        start_audio_btn.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {

        switch (view.getId()) {
            case R.id.btn_call:
                Log.d (TAG, "call");
                break;
            case R.id.btn_hangup:
                Log.d (TAG, "hangup");
                break;
            case R.id.btn_start_audio:
                Log.d (TAG, "start_video");
                break;
            case R.id.btn_start_video:
                Log.d (TAG, "start_audio");
                break;
            default:
                break;
        }
    }

    private void init_param_view() {
        call_btn = (Button) this.findViewById(R.id.btn_call);
        hangup_btn = (Button) this.findViewById(R.id.btn_hangup);
        start_video_btn = (Button) this.findViewById(R.id.btn_start_video);
        start_audio_btn = (Button) this.findViewById(R.id.btn_start_audio);

        edit_number = (EditText) this.findViewById(R.id.edt_input_number1);
        edit_ip = (EditText) this.findViewById(R.id.edt_input_ip);
        tv_stats = (TextView) this.findViewById(R.id.txt_state);
    }

    private void init_a20_fm2018_param() {

        doCommond("mount -o remount rw /system");
        doCommond("chmod 777 /system/fm2018-1204-6-2.ko");
        doCommond("insmod /system/fm2018-1204-6-2.ko");

        doCommond("mount -o remount rw /system/lib");
        doCommond("chmod 777 /system/lib/fm2018-1204-6-2.ko");
        doCommond("insmod /system/lib/fm2018-1204-6-2.ko");
    }

    public Process su;
    private void doCommond(String str) {
        try {
            su = Runtime.getRuntime().exec("/system/xbin/su");
            System.out.println(str + " \n");
            su.getOutputStream().write((str + " \n exit \n").getBytes());
            // su.waitFor();
        } catch (Exception e) {
            System.out.println("doCommond Fail");
        }
    }
}
