package com.qsa.player.lib;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.concurrent.Executors;




import android.app.Activity;
import android.app.Instrumentation;
import android.content.Context;
import android.hardware.Camera;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.KeyEvent;



/**
 * 
 * 
 * 上传信息类解析模块
 * 
 * 
 * 
 * ***/

public class SerialPortInfoUpload
{
    
    private static SerialPortInfoUpload instance;
    private static Instrumentation key_send = new Instrumentation();
    public boolean islock = true;
    private Execute mExecute;
    private Execute mExecute2;
    
    /******** 异常刷卡字段 *********/
    
    private SerialPortInfoUpload()
    {
        
    }
    
    public static SerialPortInfoUpload getInstance()
    {
        
        if (instance == null)
        {
            
            return instance = new SerialPortInfoUpload();
            
        }
        else
        {
            
            return instance;
            
        }
        
    }
    
    public void doparse(byte value[])
    {
        
    }
    
    /**
     * 
     * 按键信息判断
     * 
     * 
     * **/
    public void btn_parse(byte value)
    {
        
        switch (value)
        {
        
        // 按键信息不变
        case 0x00:
            
            break;
        
        // 按键信息1
        case 0x01:
            
            keyEvent(KeyEvent.KEYCODE_1);
            System.out.println("按键1被触发");
            break;
        
        // 按键信息2
        case 0x02:
            keyEvent(KeyEvent.KEYCODE_2);
            System.out.println("按键2被触发");
            break;
        
        // 按键信息3
        case 0x03:
            keyEvent(KeyEvent.KEYCODE_3);
            System.out.println("按键3被触发");
            break;
        
        // 按键信息4
        case 0x04:
            keyEvent(KeyEvent.KEYCODE_4);
            System.out.println("按键4被触发");
            break;
        
        // 按键信息5
        case 0x05:
            keyEvent(KeyEvent.KEYCODE_5);
            System.out.println("按键5被触发");
            break;
        
        // 按键信息6
        case 0x06:
            keyEvent(KeyEvent.KEYCODE_6);
            System.out.println("按键6被触发");
            break;
        
        // 按键信息7
        case 0x07:
            keyEvent(KeyEvent.KEYCODE_7);
            System.out.println("按键7被触发");
            break;
        
        // 按键信息8
        case 0x08:
            keyEvent(KeyEvent.KEYCODE_8);
            System.out.println("按键8被触发");
            break;
        
        // 按键信息9
        case 0x09:
            keyEvent(KeyEvent.KEYCODE_9);
            System.out.println("按键9被触发");
            break;
        
        // 按键信息0
        case 0x0A:
            keyEvent(KeyEvent.KEYCODE_0);
            System.out.println("按键0被触发");
            break;
        
        // 按键信息*
        case 0x10:
            keyEvent(KeyEvent.KEYCODE_DEL);
            System.out.println("按键*被触发");
            break;
        
        // 按键信息*+1
        case 0x11:
            keyEvent(KeyEvent.KEYCODE_F10);
            System.out.println("按键*+1被触发");
            break;
        
        // 按键信息*+2
        case 0x12:
            keyEvent(KeyEvent.KEYCODE_F11);
            System.out.println("按键*+2被触发");
            break;
        
        // 按键信息*+3
        case 0x13:
            keyEvent(KeyEvent.KEYCODE_F12);
            System.out.println("按键*+3被触发");
            break;
        
        // 按键信息*+4
        case 0x14:
            System.out.println("按键*+4被触发");
            break;
        
        // 按键信息*+5
        case 0x15:
            System.out.println("按键*+5被触发");
            break;
        
        // 按键信息*+6
        case 0x16:
            System.out.println("按键*+6被触发");
            break;
        
        // 按键信息*+7
        case 0x17:
            System.out.println("按键*+7被触发");
            break;
        
        // 按键信息*+8
        case 0x18:
            System.out.println("按键*+8被触发");
            break;
        
        // 按键信息*+9
        case 0x19:
            System.out.println("按键*+9被触发");
            break;
        
        // 按键信息*+0
        case 0x1A:
            System.out.println("按键*+0被触发");
            break;
        
        // 按键信息#
        case 0x20:
            keyEvent(KeyEvent.KEYCODE_POUND);
            System.out.println("按键#被触发");
            break;
        
        // 按键信息#+1
        case 0x21:
            keyEvent(KeyEvent.KEYCODE_F1);
            System.out.println("按键#+1被触发");
            break;
        
        // 按键信息#+2
        case 0x22:
            keyEvent(KeyEvent.KEYCODE_F2);
            System.out.println("按键#+2被触发");
            break;
        
        // 按键信息#+3
        case 0x23:
            keyEvent(KeyEvent.KEYCODE_F3);
            System.out.println("按键#+3被触发");
            break;
        
        // 按键信息#+4
        case 0x24:
            keyEvent(KeyEvent.KEYCODE_F4);
            System.out.println("按键#+4被触发");
            break;
        
        // 按键信息#+5
        case 0x25:
            keyEvent(KeyEvent.KEYCODE_F5);
            System.out.println("按键#+5被触发");
            break;
        
        // 按键信息#+6
        case 0x26:
            keyEvent(KeyEvent.KEYCODE_F6);
            System.out.println("按键#+6被触发");
            break;
        
        // 按键信息#+7
        case 0x27:
            keyEvent(KeyEvent.KEYCODE_F7);
            System.out.println("按键#+7被触发");
            break;
        
        // 按键信息#+8
        case 0x28:
            keyEvent(KeyEvent.KEYCODE_F8);
            System.out.println("按键#+8被触发");
            break;
        
        // 按键信息#+9
        case 0x29:
            keyEvent(KeyEvent.KEYCODE_F9);
            System.out.println("按键#+9被触发");
            break;
        
        // 按键信息#+A
        case 0x2A:
            keyEvent(KeyEvent.KEYCODE_F10);
            System.out.println("按键#+0被触发");
            break;
        
        // 按键信息#+*
        case 0x30:
            keyEvent(KeyEvent.KEYCODE_A);
            System.out.println("按键#+*被触发");
            
            break;
        
        }
        
        answer((byte) 0x04);
        
    }
    
    /**
     * 
     * 门状态提醒
     * 
     * 
     * **/
    public void door_parse(byte value)
    {
        
        switch (value)
        {
        
        // 无操作
        case 0x00:
            
            break;
        
        // 提示开门
        case 0x01:
            System.out.println("提示开门");
            break;
        
        }
        
        answer((byte) 0x05);
        
    }
    
    /**
     * 
     * 门磁信息
     * 
     * 
     * **/
    public void door_contact_parse(byte value)
    {
        
        switch (value)
        {
        
        // 无操作
        case 0x00:
            
            break;
        
        // 门已开
        case 0x01:
            System.out.println("门已开");
            break;
        
        // 门已关
        case 0x02:
            System.out.println("门已关");
            break;
        
        }
        
        answer((byte) 0x06);
        
    }
    
    /**
     * 
     * 防拆信息
     * 
     * 
     ***/
    public void dismantle_parse(byte value, Context ac)
    {
        switch (value)
        {
        
        // 不变
        case 0x00:
            
            break;
        
        // 有触发
        case 0x01:
            // if(DemoApplication.isControlFlag)
            // {
            // System.out.println("触发了防拆信息");
            // activity.showToast("触发了防拆信息");
            // Application.Credit_card_type = Application.tamper_alarm_type;
            // activity.fv.takepic_lock_control(Constant.IMEI,
            // SerialPortSupport.getSystemTime(), "9999999999");
            // }
            
//            if (QsaApplication.isControlFlag)
//            {
//                MobiCameraService.getInstance().getFvPic()
//                        .takepic(new CameraCallback()
//                        {
//                            
//                            @Override
//                            public void takeSuccess()
//                            {
//                                // TODO Auto-generated method stub
//                                
//                            }
//                            
//                            @Override
//                            public void takeFail(String reason)
//                            {
//                                // TODO Auto-generated method stub
//                                
//                            }
//                            
//                            @Override
//                            public void onPictureTaken(final byte[] data,
//                                    Camera camera)
//                            {
//                                // TODO Auto-generated method stub
//                                
//                                Runnable task = new Runnable()
//                                {
//                                    
//                                    @Override
//                                    public void run()
//                                    {
//                                        // TODO Auto-generated method stub
//                                        try
//                                        {
//                                            MultipartEntity postEntity = new MultipartEntity();
//                                            Date now = new Date();
//                                            SimpleDateFormat dateFormat = new SimpleDateFormat(
//                                                    "yyyy-MM-dd HH:mm:ss",
//                                                    Locale.getDefault());// 可以方便地修改日期格式
//                                            postEntity.addPart("number",
//                                                    new StringBody(
//                                                            Constant.IMEI));
//                                            postEntity.addPart(
//                                                    "open_time",
//                                                    new StringBody(dateFormat
//                                                            .format(now)));
//                                            postEntity.addPart("card_number",
//                                                    new StringBody(""));
//                                            postEntity.addPart("type",
//                                                    new StringBody(
//                                                            "security_log"));
//                                            final String result = MobiCameraService
//                                                    .getInstance()
//                                                    .getFvPic()
//                                                    .save(data,
//                                                            postEntity,
//                                                            Environment
//                                                                    .getExternalStorageDirectory()
//                                                                    .toString()
//                                                                    + "/backstageCamera/image/",
//                                                            Constant.FILE_UPLOAD_LOG);
//                                            MobiCameraService.getInstance()
//                                                    .getFvPic().clean();
//                                        }
//                                        catch (final Exception e)
//                                        {
//                                            
//                                        }
//                                    }
//                                    
//                                };
//                                new Thread(task).start();
//                                
//                            }
//                        });
//            }
            
            break;
        
        }
        
        answer((byte) 0x07);
        
    }
    
    // /**
    // *
    // * 防劫信息
    // *
    // *
    // * **/
    // public static void plunder_parse(byte value) {
    //
    // switch (value) {
    //
    // // 不变
    // case 0x00:
    //
    // break;
    //
    // // 有触发
    // case 0x01:
    // System.out.println("触发了防劫信息");
    // break;
    //
    // }
    //
    // }
    
    /**
     * 
     * 电源警报
     * 
     * 
     * **/
    public void power_parse(byte value[])
    {
        
        // 模块编码
        SerialPortSupport.HexString(value[0]);
        
        switch (value[1])
        {
        
        // 不变
        case 0x00:
            
            break;
        
        // 有触发
        case 0x01:
            System.out.println("触发了电源警报,模块编码是"
                    + SerialPortSupport.HexString(value[0]));
            break;
        
        }
        
        answer((byte) 0x08);
        
    }
    
    /**
     * 
     * NFC转换按键信息
     * 
     * **/
    public void nfc_changer_parse(byte value)
    {
        
        switch (value)
        {
        
        // 不变
        case 0x00:
            
            break;
        
        // 有触发
        case 0x01:
            System.out.println("NFC转换信息触发");
            break;
        
        }
        
        answer((byte) 0x09);
        
    }
    
    /**
     * 
     * NFC状态 模式
     * 
     * **/
    public void nfc_state_parse(byte value)
    {
        
        switch (value)
        {
        
        // 不变
        case 0x00:
            
            break;
        
        // 读卡模式
        case 0x01:
            System.out.println("现在NFC模式为：读卡模式");
            break;
        
        // 点对点模式
        case 0x02:
            System.out.println("现在NFC模式为：点对点模式");
            break;
        
        // 卡模式
        case 0x03:
            System.out.println("现在NFC模式为：卡模式");
            break;
        
        }
        
        answer((byte) 0x0A);
        
    }
    
    /**
     * 
     * 人体感应距离
     * 
     * **/
    public static void somatosensory_parse(byte value[])
    {
        
        SerialPortSupport.getCount(value);
        
    }
    
    private int illeGality = 0;
    private boolean startCheck = true;
    // private Handler illeGalityHandler = new Handler();
    private Runnable illeGalityRunnable = new Runnable()
    {
        
        @Override
        public void run()
        {
            // TODO Auto-generated method stub
//            
//            SipControlService.enterAddCard = false;
//            illeGality = 0;
//            startCheck = true;
//            Log.e("state", "非法刷卡计数清零");
            
        }
    };
    
    /**
     * 
     * 门禁信息
     * 
     * **/
    
    private String Cardresult;
    
    public void door_info_parse(byte[] value, final Context ac)
    {
        boolean iscan;
        if (ToolsParser.getShareString(ac, "mcu_version").equals("none"))
        {
            /** 旧版本MCU **************************************************************/
            System.out.println("旧MCU");
            if (value[6] == 0x00)
            {
                
                iscan = false;
                
            }
            else
            {
                
                iscan = true;
                
            }
            ;
            
        }
        else
        {
            /** 新版本MCU **************************************************************/
            System.out.println("新版本MCU");
            if (value[7] == 0x00)
            {
                
                iscan = false;
                
            }
            else
            {
                
                iscan = true;
                
            }
            ;
            
        }
        
        if (!iscan)
        {
            
            System.out.println("无门禁信息");
            
        }
        else
        {
            
            if (ToolsParser.getShareString(ac, "mcu_version").equals("none"))
            {
                
                Cardresult = SerialPortSupport.ParseCard(value);
                
            }
            else
            {
                
                Cardresult = SerialPortSupport.ParseCardNew(value);
                
            }
            
            if (!Cardresult.equals("error")){}
//            {
//                
//                boolean state = DoorControlService.getInstance().swipingCard(
//                        Cardresult, 0);
//                
//                if (state)
//                {
//                    
//                    DoorControlService.getInstance().picBy = false;
//                    
//                    Log.e("state", "刷卡成功");
//                    ac.playSound(10, 0);
//                    SerialPortService.getInstance().set_lock_control(3);
//                    
//                    MobiCameraService.getInstance().getFvPic()
//                            .takepic(new CameraCallback()
//                            {
//                                
//                                @Override
//                                public void takeSuccess()
//                                {
//                                    
//                                }
//                                
//                                @Override
//                                public void takeFail(String reason)
//                                {
//                                    // TODO Auto-generated method stub
//                                    
//                                }
//                                
//                                @Override
//                                public void onPictureTaken(final byte[] data,
//                                        Camera camera)
//                                {
//                                    // TODO Auto-generated method stub
//                                    
//                                    Runnable task = new Runnable()
//                                    {
//                                        
//                                        @Override
//                                        public void run()
//                                        {
//                                            // TODO Auto-generated method stub
//                                            try
//                                            {
//                                                MultipartEntity postEntity = new MultipartEntity();
//                                                Date now = new Date();
//                                                SimpleDateFormat dateFormat = new SimpleDateFormat(
//                                                        "yyyy-MM-dd HH:mm:ss",
//                                                        Locale.getDefault());// 可以方便地修改日期格式
//                                                postEntity.addPart("number",
//                                                        new StringBody(
//                                                                Constant.IMEI));
//                                                postEntity
//                                                        .addPart(
//                                                                "open_time",
//                                                                new StringBody(
//                                                                        dateFormat
//                                                                                .format(now)));
//                                                postEntity.addPart(
//                                                        "card_number",
//                                                        new StringBody(
//                                                                Cardresult));
//                                                postEntity.addPart("type",
//                                                        new StringBody(
//                                                                "open_log"));
//                                                final String result = MobiCameraService
//                                                        .getInstance()
//                                                        .getFvPic()
//                                                        .save(data,
//                                                                postEntity,
//                                                                Environment
//                                                                        .getExternalStorageDirectory()
//                                                                        .toString()
//                                                                        + "/backstageCamera/image/",
//                                                                Constant.FILE_UPLOAD_LOG);
//                                                MobiCameraService.getInstance()
//                                                        .getFvPic().clean();
//                                                Log.e("state", "刷卡开门上传成功");
//                                            }
//                                            catch (final Exception e)
//                                            {
//                                                
//                                            }
//                                        }
//                                        
//                                    };
//                                    new Thread(task).start();
//                                    
//                                }
//                            });
//                    
//                }
//                else
//                {
//                    
//                    Log.e("state", "刷卡失败" + illeGality);
//                    illeGality++;
//                    if (illeGality == 1)
//                    {
//                        
//                        SipControlService.enterAddCard = true;
//                        
//                        ac.runOnUiThread(new Runnable()
//                        {
//                            
//                            @Override
//                            public void run()
//                            {
//                                // TODO Auto-generated method stub
//                                FileUtils.showToastUI(ac, "友情提示:#键加卡");
//                            }
//                        });
//                        SipControlService.LocalCard = Cardresult;
//                        Log.e("state", "加卡：" + Cardresult);
//                        
//                    }
//                    
//                    if (illeGality > 3)
//                    {
//                        
//                        Log.e("state", "非法刷卡");
//                        MobiCameraService.getInstance().getFvPic()
//                                .takepic(new CameraCallback()
//                                {
//                                    
//                                    @Override
//                                    public void takeSuccess()
//                                    {
//                                        // TODO Auto-generated method stub
//                                        
//                                    }
//                                    
//                                    @Override
//                                    public void takeFail(String reason)
//                                    {
//                                        // TODO Auto-generated method stub
//                                        
//                                    }
//                                    
//                                    @Override
//                                    public void onPictureTaken(
//                                            final byte[] data, Camera camera)
//                                    {
//                                        // TODO Auto-generated method stub
//                                        
//                                        Runnable task = new Runnable()
//                                        {
//                                            
//                                            @Override
//                                            public void run()
//                                            {
//                                                // TODO Auto-generated method
//                                                // stub
//                                                try
//                                                {
//                                                    MultipartEntity postEntity = new MultipartEntity();
//                                                    Date now = new Date();
//                                                    SimpleDateFormat dateFormat = new SimpleDateFormat(
//                                                            "yyyy-MM-dd HH:mm:ss",
//                                                            Locale.getDefault());// 可以方便地修改日期格式
//                                                    postEntity
//                                                            .addPart(
//                                                                    "number",
//                                                                    new StringBody(
//                                                                            Constant.IMEI));
//                                                    postEntity
//                                                            .addPart(
//                                                                    "open_time",
//                                                                    new StringBody(
//                                                                            dateFormat
//                                                                                    .format(now)));
//                                                    postEntity
//                                                            .addPart(
//                                                                    "card_number",
//                                                                    new StringBody(
//                                                                            Cardresult));
//                                                    postEntity.addPart("type",
//                                                            new StringBody(
//                                                                    "illegal"));
//                                                    final String result = MobiCameraService
//                                                            .getInstance()
//                                                            .getFvPic()
//                                                            .save(data,
//                                                                    postEntity,
//                                                                    Environment
//                                                                            .getExternalStorageDirectory()
//                                                                            .toString()
//                                                                            + "/backstageCamera/image/",
//                                                                    Constant.FILE_UPLOAD_LOG);
//                                                    MobiCameraService
//                                                            .getInstance()
//                                                            .getFvPic().clean();
//                                                    Log.e("state", "非法刷卡上传成功");
//                                                    illeGality = 0;
//                                                    SipControlService.enterAddCard = false;
//                                                    ac.welHandler
//                                                            .removeCallbacks(illeGalityRunnable);
//                                                }
//                                                catch (final Exception e)
//                                                {
//                                                    
//                                                }
//                                            }
//                                            
//                                        };
//                                        new Thread(task).start();
//                                        
//                                    }
//                                });
//                        
//                    }
//                    
//                    if (startCheck)
//                    {
//                        
//                        ac.runOnUiThread(new Runnable()
//                        {
//                            
//                            @Override
//                            public void run()
//                            {
//                                // TODO Auto-generated method stub
//                                ac.welHandler.postDelayed(illeGalityRunnable,
//                                        10000);
//                                
//                            }
//                        });
//                        
//                        startCheck = false;
//                    }
//                    
//                }
//                
//            }
            else
            {
                
                Log.e("state", "卡号解析错误");
                
            }
            
        }
        
        answer((byte) 0x0A);
        
    }
    
    /**
     * 
     * 门禁信息
     * 
     * **/
    public String door_info_parse(byte value[], int info_length)
    {
        
        byte[] new_value = new byte[info_length];
        for (int i = 0; i < info_length; i++)
        {
            
            new_value[i] = value[i + 7];
            
        }
        StringBuffer str = new StringBuffer();
        for (int i = 0; i < new_value.length; i++)
        {
            
            str.append(SerialPortSupport.HexString(new_value[i]) + "");
            
        }
        // SerialPortSupport.getCount(new_value);
        return str.toString();
        
    }
    
    /**
     * 
     * 卡键报警信息(预留)
     * 
     * **/
    public void stuck_key_parse(byte[] value)
    {
        
        String str = "";
        boolean warn = false;
        for (int i = 0; i < value.length; i++)
        {
            
            if (value[i] == 0x01)
            {
                warn = true;
                switch (i)
                {
                
                case 0:
                    str += "  0";
                    break;
                case 1:
                    str += "  1";
                    break;
                case 2:
                    str += "  2";
                    break;
                case 3:
                    str += "  3";
                    break;
                case 4:
                    str += "  4";
                    break;
                case 5:
                    str += "  5";
                    break;
                case 6:
                    str += "  6";
                    break;
                case 7:
                    str += "  7";
                    break;
                case 8:
                    str += "  8";
                    break;
                case 9:
                    str += "  9";
                    break;
                case 10:
                    str += "  *";
                    break;
                case 11:
                    str += "  #";
                    break;
                
                }
                
            }
            
            if (warn)
            {
                
                str = "收到卡键警报：" + str;
                System.out.println(str);
                
            }
            
        }
        
        answer((byte) 0x0C);
        
    }
    
    /**
     * 
     * 加热异常报警信息(预留)
     * 
     * **/
    public void heat_warning_parse(byte value)
    {
        
        // 加热异常
        if (value == 0x01)
        {
            System.out.println("加热异常");
        }
        // 正常情况
        else
        {
        }
        
        answer((byte) 0x0D);
        
    }
    
    /**
     * 
     * 模拟按键事件
     * 
     * @param KeyValue
     *            键值
     * 
     */
    KeyTask keyTask;
    
    private synchronized void keyEvent(int KeyValue)
    {
        
        // Log.e("keyEvent", "msg" + KeyValue);
        if (!SerialPortService.pool.isShutdown())
        {
            
            Log.e("keyEvent1", "msg" + KeyValue);
            mExecute = new Execute(KeyValue);
            SerialPortService.pool.execute(mExecute);
            
        }
        else
        {
            SerialPortService.pool = Executors.newFixedThreadPool(4);
            mExecute2 = new Execute(KeyValue);
            SerialPortService.pool.execute(mExecute);
        }
        // keyTask = new KeyTask();
        // keyTask.execute(KeyValue);
        
    }
    
    static class KeyTask extends AsyncTask<Integer, Integer, String>
    {
        
        @Override
        protected String doInBackground(Integer... params)
        {
            // TODO Auto-generated method stub
            
            key_send.sendKeyDownUpSync(params[0]);
            
            return null;
        }
        
    }
    
    class Execute implements Runnable
    {
        
        private int keyMode;
        
        public Execute(int keyMode)
        {
            
            this.keyMode = keyMode;
            
        }
        
        @Override
        public void run()
        {
            // TODO Auto-generated method stub
            
            key_send.sendKeyDownUpSync(keyMode);
            
        }
        
    }
    
    public void doparse(byte value[], boolean value1)
    {
        
        byte[] now = new byte[]
        { (byte) 0xAD, 0x00, 0x13, SerialPortService.MCUcyclic, 0x03, 0x02,
                0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                0x01, 0x01, 0x00 };
        now[now.length - 1] = SerialPortSupport.getCheckValue(now);
        // SerialportMessage msg = new SerialportMessage(
        // SerialportMessage.ORIENTATION_RECEIVE, now);
        // Message msg1 = WriteThread.writeHandler.obtainMessage();
        // msg1.what = 1;
        // msg1.obj = msg;
        // WriteThread.writeHandler.sendMessage(msg1);
        SerialPortService.getInstance().write(now);
        
    }
    
    /**
     * 
     * 封装好的回复信息
     * 
     * @param value
     *            命令字段
     * 
     * 
     * */
    public void answer(byte value)
    {
        
        byte[] now = new byte[]
        { (byte) 0xAD, 0x00, 0x08, SerialPortService.MCUcyclic, value, 0x02,
                0x01, 0x00 };
        now[now.length - 1] = SerialPortSupport.getCheckValue(now);
        SerialPortService.getInstance().write(now);
    }
    
}
