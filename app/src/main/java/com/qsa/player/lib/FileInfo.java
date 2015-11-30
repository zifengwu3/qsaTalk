package com.qsa.player.lib;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;



public class FileInfo {

	// 数据包总数
	public static int boxsize;

	// 数据包识别码
	public byte[] boxheading;

	// 数据文件集
	public List<File> file = new ArrayList<File>();

	// 数据包号
	public int no;

	// 当前发送的数据包号
	public static int nowbackCount;

	// 总数据包号
	public int allbackCount;

	// 当前是否处于MCU升级状态
	public static boolean isMCUupdate = false;

	/**
	 * 
	 * 根据升级包的分解文件得出指定格式的byte数组
	 * 
	 * 
	 * 
	 * **/
	public static byte[] getMCUpack(int i) {

		if(i==-1){
			
			//发送升级
			byte[] serialportContent = new byte[18];
			serialportContent[0] = (byte) 0xAD;
			serialportContent[2] = (byte) 0x12;
			serialportContent[3] = (byte) SerialPortSupport.getCyclic();
			serialportContent[4] = (byte) 0x20;
			serialportContent[5] = (byte) 0x01;
			serialportContent[6] = (byte) 0x01;
			serialportContent[7] = (byte) (SerialPortService.fileinfo.boxsize >> 8);
			serialportContent[8] = (byte) (SerialPortService.fileinfo.boxsize & 0xFF);
//			serialportContent[7] = (byte) (0x00);
//			serialportContent[8] = (byte) (0x80);
			serialportContent[9] = SerialPortService.fileinfo.boxheading[0];
			serialportContent[10] = SerialPortService.fileinfo.boxheading[1];
			serialportContent[11] = SerialPortService.fileinfo.boxheading[2];
			serialportContent[12] = SerialPortService.fileinfo.boxheading[3];
			serialportContent[13] = SerialPortService.fileinfo.boxheading[4];
			serialportContent[14] = SerialPortService.fileinfo.boxheading[5];
			serialportContent[15] = SerialPortService.fileinfo.boxheading[6];
			serialportContent[16] = SerialPortService.fileinfo.boxheading[7];
			serialportContent[17] = SerialPortSupport.getCheckValue(serialportContent);
			return serialportContent;
		}
		else{
			
			int count = (int)( 7 + 3 + SerialPortService.fileinfo.file.get(i).length());
			byte[] serialportContent = new byte[count];
			serialportContent[0] = (byte) 0xAD;
			serialportContent[1] = (byte) (count >> 8);
			serialportContent[2] = (byte) (count & 0xFF);
			serialportContent[3] = (byte) SerialPortSupport.getCyclic();
			serialportContent[4] = (byte) 0x20;
			serialportContent[5] = (byte) 0x01;
			serialportContent[6] = (byte) 0x02;
			serialportContent[7] = (byte) (i >> 8);
			serialportContent[8] = (byte) (i & 0xFF);
			int next = 0;
			try {
				for (byte now : SerialPortSupport.getBytesFromFile(SerialPortService.fileinfo.file
						.get(i))) {

					serialportContent[9 + next] = now;
					next = next + 1;

				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			serialportContent[count - 1] = SerialPortSupport.getCheckValue(serialportContent);
			return serialportContent;
		}
		
		

	}
}
