package com.qsa.player.lib;

import java.io.File;

import android.app.Activity;
import android.content.Context;





public interface ISerialPortService {
	
	void open(Context ac);
	
	void close();
	
	void write(byte[] value);
	
	/** 锁控信息 1:开启 2：关门 3：MCU自动开关 **/
	void set_lock_control(int vlaue);

	
	/** 背光信息  1:开启 2:关闭**/
	void set_backlight_control(int vlaue);
	
	/** NFC转换  **/
	void set_nfc_changer(int Mode);
	
	/** 获取MCU版本号  **/
	void get_mcu_version();

	/** 
	 * 设置读取卡内数据  
	 * 
	 * 数据A：启用状态(1 byte) +扇区编码(1 byte) +所在块别(1 byte) +起始位置(1 byte) + 数据长度(1 byte)+ 扇区密码(6 byte)
	 * @param state
	 * 			启用状态：启用－0x01；禁用－0x02；
	 * @param sector_coding
	 * 			扇区编码：范围：0x01-0x16；格式：BCD码；
	 * @param chunk
	 * 			所在块别：范围：0x00-0x02；格式：BCD码；
	 * @param start_place 
	 * 			起始位置：范围：0x00-0x15；格式：BCD码；
	 * @param pwd
	 *			扇区密码：格式：BCD码；全0xFF为无密码；
	 * 
	 * **/
	void set_read_card_data(int state,int sector_coding,int chunk,int start_place,String pwd);
	
	/** 设置巡检时间  value:0-65535分钟    (默认值为0x0000，表示默认关闭巡检) **/
	void set_routing_inspection_time(int value);
	
	/**	巡检开关-关闭/打开巡检	1:停止   2:开始**/
	void set_routing_inspection_with_A20_updata(int value);
	
	/** 设置MCU强制重启A20密码  0-8位**/
	void set_MCU_forcePwd(String pwd);
	
	/** 读取MCU强制重启A20密码  **/
	void get_MCU_forcePwd();

	/**
	 * 
	 * 门锁时间设置
	 * 
	 * @param Value
	 *            门锁的延迟时间    0秒－0x00，254秒－0xFE   开锁时间0xFF为默认值，默认值为10秒。
	 * **/
	void set_door_control_delay(int Value);

	/**
	 * 
	 * MCU升级
	 * 
	 * @param File
	 *            file 所需要操作的MCU升级文件
	 * 
	 * **/
	int MCUupdate(String path);

}
