package com.qsa.player.lib;

//import android.os.Message;

/***
 * 
 * @author 蔡维泼
 * 
 *         收发信息的内容封装类
 * 
 * **/

public class SerialportMessage {

	public SerialportMessage(int orientation, byte[] serialportContent) {

		this.orientation = orientation;
		this.serialportContent = serialportContent;

	}
	
//	private static String MESSAGE_FLAG = "MESSAGE_FLAG";

	/** 发送状态 **/
	public static int ORIENTATION_SEND = 2;

	/** 接收状态 **/
	public static int ORIENTATION_RECEIVE = 1;

	/** 信息传递方向 1为接收的数据 2为发送的数据 **/
	public int orientation;

	/** 信息内容 **/
	public byte[] serialportContent;

	/** 重发次数 **/
	public int reSendCount = 0;

	/** 发送等待时间 **/
	public int ReceiveTime = 0;

	/**
	 * 
	 * 设置信息 返回封装好的状态信息
	 * 
	 * @param orientation
	 *            数据方向
	 * 
	 * @param serialportContent
	 *            数据内容
	 * 
	 * **/
	public static SerialportMessage SetMessage(int orientation,
			byte[] serialportContent) {

		return new SerialportMessage(orientation, serialportContent);

	}

	/**
	 * 
	 * 设置发送
	 * 
	 * @param orientation
	 *            数据方向
	 * 
	 * @param serialportContent
	 *            数据内容
	 * 
	 * **/
	public static void SendMessage(SerialportMessage serialportContent) {

	
//		Message msg = WriteThread.writeHandler.obtainMessage();
//		msg.what = 1;
//		msg.obj = serialportContent;
//		WriteThread.writeHandler.sendMessage(msg);

	}

}
