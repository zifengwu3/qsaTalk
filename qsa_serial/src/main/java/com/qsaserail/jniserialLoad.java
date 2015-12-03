package com.qsaserail;

/**
 * Created by l on 11/29/15.
 */
public class jniserialLoad {

    private final String TAG = "serial_qsa";

    static {
        System.loadLibrary("serial_qsa");
    }

    //native method
    public native String init_serial_qsa();
}
