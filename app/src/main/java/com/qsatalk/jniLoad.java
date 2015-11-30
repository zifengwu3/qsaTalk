package com.qsatalk;

import android.util.Log;

/**
 * Created by l on 11/29/15.
 */
public class jniLoad {

    private final String TAG = "talk_qsa";

    static {
        System.loadLibrary("talk_qsa");
    }

    //native method
    public native String sendHello();
    //native method
    public native String SayHelloInC(String str);
    /* callback Java method from C */
    public native void callCcode();
    public native void callCcode1();
    public native void callCcode2();

    ///C调用java中的空方法
    public void helloFromJava(){
        Log.d(TAG, "hello from java");
    }
    //C调用java中的带两个int参数的方法
    public int Add(int x,int y){
        System.out.println();
        Log.d(TAG, "相加的结果为"+ (x+y));
        return x + y;
    }
    //C调用java中参数为string的方法
    public void printString(String s){
        Log.d(TAG, "in java code "+ s);
    }
}
