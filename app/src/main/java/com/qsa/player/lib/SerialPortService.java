package com.qsa.player.lib;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.security.InvalidParameterException;
import java.util.Vector;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import android.content.Context;
import android.content.SharedPreferences.Editor;
import android.os.Handler;
import android.util.Log;

public class SerialPortService implements ISerialPortService {

	private SerialPort mSerialPort;
	private OutputStream mOutputStream;
	private InputStream mInputStream;
	// MCU版本号
	public static String MCU_version;
	/** 待接收回复列表 **/
	public static Vector<SerialportMessage> waitList = new Vector<SerialportMessage>();
	/** ------------------串口成员控制变量----------------------- **/
	/** NFC转换 **/
	public static final int NFC_CHANGE = 2;
	/** NFC读卡模式 **/
	public static final int NFC_READ_CARD = 21;
	/** NFC点对点模式 **/
	public static final int NFC_POINT_TO_POINT = 22;
	/** NFC卡模式 **/
	public static final int NFC_CARD = 23;

	/** -------------------------------------------------------- **/

	private ReadThread mReadThread;
	// 串口开关
	private boolean serialportSwith = true;
	private static SerialPortService mSerialPortService = null;

	// 锁控类型
	public static int lock_control = 0;
	public final static int lock_control_by_sip = 1;
	public final static int lock_control_by_card = 2;
	private static final String TAG = SerialPortService.class.getName();
	private static final String SECURIT_INFO = "安全模块测试信息------->";
	private static final int NAME_LENGTH = 30;
	private static final int SEX_LENGTH = 2;
	private static final int FAIMLY_LENGTH = 4;
	private static final int BORNDATE_LENGTH = 16;
	private static final int ADDRESS_LENGHT = 70;
	private static final int ID_LENGTH = 36;
	private static final int IS_ATRY_LENGTH = 30;
	private static final int START_DATE_LENGTH = 16;
	private static final int END_DATE_LENGTH = 16;
	// 刷卡类型
	public static int Credit_card_type = 0;
	public static int illegal_card_type = 1;
	public static int egal_card_type = 2;
	public static int tamper_alarm_type = 3;
	// 巡检异常计数
	public static int inspectionCount = 0;
	// MCU当前的循环码值
	public static byte MCUcyclic = 0X00;

	// ARM当前的循环码值
	public static byte ARMcyclic = 0X00;

	// 发包超时次数
	private int timeOutCount = 0;

	public SerialPortFinder mSerialPortFinder = new SerialPortFinder();

	// 创建一个可重用固定线程数的线程池
	public static ExecutorService pool = Executors.newFixedThreadPool(4);

	// MCU升级文件内存信息
	public static FileInfo fileinfo;

	public Context ac;

	private SerialPortService() {

	}

	public static SerialPortService getInstance() {
		if (mSerialPortService == null) {
			mSerialPortService = new SerialPortService();
		}
		return mSerialPortService;
	}

	@Override
	public void open(final Context ac) {
		// TODO Auto-generated method stub
		serialportSwith = true;
		inspectionCount = 0;
		MCUcyclic = 0X00;
		ARMcyclic = 0X00;
		try {

			this.ac = ac;
			Log.e("now_state", "________________开启串口");
			// new File("/dev/ttyS3");
			mSerialPort = new SerialPort(new File("/dev/ttyS3"), 9600, 0);// COM0，波特率9600
			mOutputStream = mSerialPort.getOutputStream();
			mInputStream = mSerialPort.getInputStream();

			/* Create a receiving thread */
			mReadThread = new ReadThread();
			mReadThread.setPriority(Thread.MAX_PRIORITY);
			mReadThread.start();

			if (ToolsParser.getShareBoolean(ac, "mcu_flag")) {

				this.set_routing_inspection_with_A20_updata(2);
				Editor mEditor = ToolsParser.getSharedPreferences(ac);
				mEditor.putBoolean("mcu_flag", false);
				mEditor.commit();
				// 如果在1秒内 开启巡检失败后 再次启动巡检
				new Handler().postDelayed(new Runnable() {

					@Override
					public void run() {
						// TODO Auto-generated method stub
						if (!ToolsParser.getShareBoolean(ac, "mcu_flag")) {
							set_routing_inspection_with_A20_updata(2);

						}

					}
				}, 1000);

			}

			// 获取MCU版本号
			this.get_mcu_version();

		} catch (SecurityException e) {
			// DisplayError(R.string.error_security);
		} catch (IOException e) {
			// DisplayError(R.string.error_unknown);
		} catch (InvalidParameterException e) {
			// DisplayError(R.string.error_configuration);
		}

	}

	@Override
	public void close() {
		// TODO Auto-generated method stub
		serialportSwith = false;
		if (mReadThread != null)
			mReadThread.interrupt();
		mSerialPort = null;

	}

	byte[] temp;
	private boolean iscomplete = true;
	private int packLength;
	private byte[] packageBuf = new byte[500];
	private int hadGetLength;

	@Override
	public synchronized void write(byte[] value) {
		// TODO Auto-generated method stub

		try {
			temp = value;
			Log.e("开始写入", "----");
			SerialPortSupport.printHexString(value);
			Log.e("结束写入", "----");
			mOutputStream.write(value);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	public void resend() {

		System.out.println("开始重发");
		write(temp);

	}

	@Override
	public void set_lock_control(int vlaue) {
		// TODO Auto-generated method stub

		byte[] serialportContent = new byte[8];

		serialportContent[0] = (byte) 0xAD;
		serialportContent[2] = 0x08;
		serialportContent[3] = SerialPortSupport.getCyclic();
		serialportContent[4] = 0x02;
		serialportContent[5] = 0x01;
		// 命令位
		serialportContent[6] = (byte) vlaue;
		serialportContent[7] = SerialPortSupport
				.getCheckValue(serialportContent);
		this.write(serialportContent);

	}

	@Override
	public void set_backlight_control(int vlaue) {
		// TODO Auto-generated method stub

		byte[] serialportContent = new byte[8];

		serialportContent[0] = (byte) 0xAD;
		serialportContent[2] = 0x08;
		serialportContent[3] = SerialPortSupport.getCyclic();
		serialportContent[4] = 0x13;
		serialportContent[5] = 0x01;
		// 命令位
		serialportContent[6] = (byte) vlaue;
		serialportContent[7] = SerialPortSupport
				.getCheckValue(serialportContent);
		this.write(serialportContent);

	}


	@Override
	public void set_nfc_changer(int Mode) {
		// TODO Auto-generated method stub

		byte[] serialportContent = new byte[8];
		serialportContent[0] = (byte) 0xAD;
		serialportContent[2] = 0x08;
		serialportContent[3] = SerialPortSupport.getCyclic();
		serialportContent[4] = 0x03;
		serialportContent[5] = 0x01;
		// 命令位
		switch (Mode) {

		// 读卡模式
		case NFC_READ_CARD:
			serialportContent[6] = 0x01;
			break;
		// 点对点模式
		case NFC_POINT_TO_POINT:
			serialportContent[6] = 0x02;
			break;
		// 卡模式
		case NFC_CARD:
			serialportContent[6] = 0x03;
			break;

		}

		serialportContent[7] = SerialPortSupport
				.getCheckValue(serialportContent);

	}

	@Override
	public void set_door_control_delay(int Value) {
		// TODO Auto-generated method stub

		byte[] serialportContent = new byte[8];
		serialportContent[0] = (byte) 0xAD;
		serialportContent[2] = 0x08;
		serialportContent[3] = SerialPortSupport.getCyclic();
		serialportContent[4] = 0x14;
		serialportContent[5] = 0x01;
		serialportContent[6] = (byte) Value;

		serialportContent[7] = SerialPortSupport
				.getCheckValue(serialportContent);

		write(serialportContent);

	}

	@Override
	public void get_mcu_version() {
		// TODO Auto-generated method stub
		byte[] serialportContent = new byte[8];
		serialportContent[0] = (byte) 0xAD;
		serialportContent[2] = 0x08;
		serialportContent[3] = SerialPortSupport.getCyclic();
		serialportContent[4] = 0x21;
		serialportContent[5] = 0x01;
		serialportContent[6] = (byte) 0x01;

		serialportContent[7] = SerialPortSupport
				.getCheckValue(serialportContent);

		write(serialportContent);

	}

	@Override
	public void set_routing_inspection_time(int value) {
		// TODO Auto-generated method stub
		byte[] serialportContent = new byte[9];
		serialportContent[0] = (byte) 0xAD;
		serialportContent[2] = 0x09;
		serialportContent[3] = SerialPortSupport.getCyclic();
		serialportContent[4] = 0x16;
		serialportContent[5] = 0x01;
		byte[] temp = SerialPortSupport.setCount(value);
		serialportContent[6] = temp[0];
		serialportContent[7] = temp[1];
		serialportContent[8] = SerialPortSupport
				.getCheckValue(serialportContent);

		write(serialportContent);

		SerialPortSupport.printHexString(serialportContent);

	}

	@Override
	public void set_routing_inspection_with_A20_updata(int value) {
		// TODO Auto-generated method stub
		byte[] serialportContent = new byte[8];
		serialportContent[0] = (byte) 0xAD;
		serialportContent[2] = 0x08;
		serialportContent[3] = SerialPortSupport.getCyclic();
		serialportContent[4] = 0x17;
		serialportContent[5] = 0x01;
		serialportContent[6] = (byte) value;

		serialportContent[7] = SerialPortSupport
				.getCheckValue(serialportContent);

		write(serialportContent);
	}

	@Override
	public void set_read_card_data(int state, int sector_coding, int chunk,
			int start_place, String pwd) {
		// TODO Auto-generated method stub

	}

	@Override
	public void get_MCU_forcePwd() {
		// TODO Auto-generated method stub
		byte[] serialportContent = new byte[7];
		serialportContent[0] = (byte) 0xAD;
		serialportContent[2] = 0x07;
		serialportContent[3] = SerialPortSupport.getCyclic();
		serialportContent[4] = 0x19;
		serialportContent[5] = 0x01;

		serialportContent[6] = SerialPortSupport
				.getCheckValue(serialportContent);

		write(serialportContent);
	}

	@Override
	public void set_MCU_forcePwd(String pwd) {
		// TODO Auto-generated method stub
		byte[] serialportContent = new byte[16];
		serialportContent[0] = (byte) 0xAD;
		serialportContent[2] = 0x10;
		serialportContent[3] = SerialPortSupport.getCyclic();
		serialportContent[4] = 0x18;
		serialportContent[5] = 0x01;
		serialportContent[6] = (byte) pwd.length();
		for (int i = 0; i < pwd.length(); i++) {

			serialportContent[7 + i] = Byte.valueOf(pwd.charAt(i) + "");

		}
		SerialPortSupport.printHexString(serialportContent);
		serialportContent[15] = SerialPortSupport
				.getCheckValue(serialportContent);

		write(serialportContent);
	}

	@Override
	public int MCUupdate(final String path) {
		// TODO Auto-generated method stub

		int count = 0;

		if (!FileInfo.isMCUupdate) {

			File filetemp = new File(path);

			if (filetemp.exists()) {

				count = (int) (filetemp.length() / 128);
				if (filetemp.length() % 128 != 0) {

					count += 1;

				}

			} else {

				Log.i("now", "文件不存在，请重新下载更新");
				FileInfo.isMCUupdate = false;
				return (Integer) 0;

			}

			Log.i("now", "---------------------------------");
			FileInfo.isMCUupdate = true;
			Runnable excu = new Runnable() {

				public void run() {

					FileCut cun = new FileCut(path, "128");

					try {

						fileinfo = cun.cut();
						FileInfo.nowbackCount = -1;
						System.out.println("检测升级");
						// ac.welHandler.postDelayed(new Runnable() {
						//
						// @Override
						// public void run() {
						// // TODO Auto-generated method stub
						//
						// if(FileInfo.nowbackCount==-1&&FileInfo.isMCUupdate){
						//
						// FileInfo.isMCUupdate = false;
						// System.out.println("升级失败");
						//
						// }
						//
						// }
						// }, 1500);

						SerialPortService.getInstance().write(
								FileInfo.getMCUpack(FileInfo.nowbackCount));

					} catch (Exception e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
						FileInfo.isMCUupdate = false;
					}

				};

			};
			pool.execute(excu);

		} else {

			Log.i("now", "已经处于升级状态");

		}

		return count;

	}

	/**
	 * timeID_a timeID_b 用于计算 身份证信息获取的时间
	 * 
	 * 
	 */
	private long timeID_a;
	private long timeID_b;

	private class ReadThread extends Thread {

		@Override
		public void run() {
			super.run();
			while (serialportSwith) {
				int size;
				try {
					byte[] buffer = new byte[500];
					if (mInputStream == null)
						return;
					
					size = mInputStream.read(buffer);
					
					if (size > 0) {
						onDataReceived(buffer, size);
					}
				} catch (IOException e) {
					e.printStackTrace();
					return;
				}

			}
		}
	}

	// 接收数据处理

	protected void onDataReceived(byte[] buffer, int size) {
		byte[] receiveData = null;
		String str = "";
		if (!iscomplete) {
			System.arraycopy(buffer, 0, packageBuf, hadGetLength, size);
			hadGetLength += size;

			if (hadGetLength >= packLength) // 收到包的长度大于包内数据长度
			{
				Log.i(TAG, "======包接收完整===");
				iscomplete = true;
				receiveData = new byte[packLength];
				System.arraycopy(packageBuf, 0, receiveData, 0, packLength);
				sendToDoparse(receiveData);

			}
			return;
		}
		for (int i = 0; i < size; i++) {
			if (buffer[i] == (byte) 0xAD) {
				Log.v(TAG, "找到匹配的起始位");
				packLength = SerialPortSupport.getCount(new byte[] {
						buffer[i + 1], buffer[i + 2] });
				// 判断指定长度是否大于实际有效包的的长度
				int relength = size - i; // 从 0xAD 以后的数据长度
				if (packLength > relength) {
					// 包还未接收完整
					timeID_a = System.currentTimeMillis();
					Log.i(TAG, "======包未接收完整 包长度" + packLength + "＝＝＝＝");

					iscomplete = false;
					System.arraycopy(buffer, i, packageBuf, 0, relength);
					hadGetLength = relength;
					return;
				}
				if (packLength <= 500 && packLength >= 7) {

				} else {
					Log.e(TAG, "读取数据长度有问题");
					return;
				}
				receiveData = new byte[packLength];
				for (int j = 0; j < packLength; j++) {
					receiveData[j] = buffer[j + i];
					str = str + " "
							+ SerialPortSupport.HexString(receiveData[j]);
				}
				sendToDoparse(receiveData);
				break;
			}

		}

	};

	public void sendToDoparse(byte[] receiveData) {
		// 将读取的内容传送到解析线程中
		if (receiveData != null) {

			System.out.println("符合安全的长度");

			// 开始计时，性能测试用nanoTime会更精确，因为它是纳秒级的
			long startTime = System.nanoTime();
			doParse(receiveData, ac);
			// 停止计时
			long endTime = System.nanoTime();
			// 耗时
			long spendTime = (endTime - startTime);
			Log.d("GoogleIO", "解析花费了" + spendTime + "纳秒");
			// msg.obj = SerialportMessage.SetMessage(
			// SerialportMessage.ORIENTATION_RECEIVE, receiveData);
			// ParseThread.parseHanler.sendMessage(msg);
			// System.out.println(str);

		}
	}

	// SPI解析器

	public void doParse(byte[] value, Context ac) {
		Log.i(TAG, "----doParse----" + value.length);
		// 校验是否通过
		boolean isNext = checkValue(value);

		// 确认协议方向 true为发送 false为回复
		boolean orient = checkOrient(value[5]);

		if (isNext) {

			// 校验成功
			System.out.println("校验成功");
			inspectionCount = 0;

			// MCU向ARM发送执行事件
			if (orient) {

				System.out.println("MCU向ARM发送执行事件________________");
				// 如果循环码相同则为已执行过事件 抛弃重复数据
				if (MCUcyclic != value[3]) {

					MCUcyclic = value[3];
					switch (value[4]) {

					// 按键信息上传
					case 0x04:

						System.out.println("按键信息上传");
						SerialPortInfoUpload.getInstance().btn_parse(value[6]);

						break;

					// 开门按键信息上传
					case 0x05:

						System.out.println("开门按键信息上传");
						SerialPortInfoUpload.getInstance().door_parse(value[6]);

						break;

					// 门磁信息上传
					case 0x06:

						System.out.println("门磁信息上传");
						SerialPortInfoUpload.getInstance().door_contact_parse(
								value[6]);
						break;

					// 防拆信息上传
					case 0x07:

						System.out.println("防拆信息上传");
						SerialPortInfoUpload.getInstance().dismantle_parse(
								value[6], ac);

						break;

					// 电源报警上传
					case 0x08:

						System.out.println("电源报警上传");
						SerialPortInfoUpload.getInstance().power_parse(
								new byte[] { value[6], value[7] });

						break;

					// NFC转换按键信息上传
					case 0x09:

						System.out.println("NFC转换按键信息上传");
						SerialPortInfoUpload.getInstance().nfc_changer_parse(
								value[06]);

						break;

					// NFC模式信息上传
					case 0x0A:

						System.out.println("NFC模式信息上传");
						SerialPortInfoUpload.getInstance().nfc_state_parse(
								value[6]);

						break;

					// 门禁信息上传
					case 0x0B:

						System.out.println("门禁信息上传");
						SerialPortInfoUpload.getInstance().door_info_parse(
								value, ac);

						break;

					// 卡键信息上传
					case 0x0C:

						System.out.println("卡键信息上传");
						SerialPortInfoUpload.getInstance().stuck_key_parse(
								value);

						break;

					// 加热异常报警上传
					case 0x0D:

						System.out.println("加热异常报警上传");
						SerialPortInfoUpload.getInstance().heat_warning_parse(
								value[6]);

						break;

					// 独立门禁向ARM发送协议
					case 0x0F:

						System.out.println("独立门禁向ARM发送协议");
						// 内容待定

						break;

					// 巡检
					case 0x10:

						System.out.println("巡检---");
						// SerialPortInfoPatrol.getInstance().calback();

						break;

					// NFC向ARM发送信息
					case 0x12:

						System.out.println("NFC向ARM发送信息");
						// 暂无内容

						break;
					case 0x1C:
						Log.i(TAG, "get security info===============");
						//Deal_security(value);
						break;
					}

				} else {

					System.out.println("该事件已经被执行");
					byte[] now = new byte[] { (byte) 0xAD, 0x00, 0x08,
							MCUcyclic, 0x04, 0x02, 0x01, 0x00 };
					now[now.length - 1] = SerialPortSupport.getCheckValue(now);
					write(now);
					// SerialportMessage msg = new SerialportMessage(
					// SerialportMessage.ORIENTATION_RECEIVE, now);
					// Message msg1 = WriteThread.writeHandler.obtainMessage();
					// msg1.what = 1;
					// msg1.obj = msg;
					// WriteThread.writeHandler.sendMessage(msg1);

				}

			}
			// MCU向ARM发送回复事件
			else {

				System.out.println("MCU向ARM发送回复事件________________");
				// 如果循环码相同则为已回复过事件 抛弃重复数据
				if (ARMcyclic != value[3]) {

					ARMcyclic = value[3];
					// 检测是否为等待列表需要的回复

					// if (Application.waitList.size() > 0) {
					//
					// // for (int i = 0; i < Application.waitList.size(); i++)
					// // {
					//
					// System.out
					// .println(Application.waitList.get(0).serialportContent[3]
					// + "");
					// if ((Application.waitList.get(0).serialportContent[3]) ==
					// value[3]) {
					//
					// // 该命令已经回复 从计时数据中删除
					// System.out.println("已经将循环码为" + value[3]
					// + "的待回复命令删除");
					// Application.waitList.remove(0);
					//
					// }
					//
					// // }
					//
					// }

					switch (value[4]) {

					// 锁控信息
					case 0x02:

						// SerialPortInfoControl.getInstance().lock_control_parse(
						// value[6], ac);

						break;

					// NFC转换模式
					case 0x03:

						// SerialPortInfoControl.getInstance().nfc_change_parse(
						// value[6]);

						break;

					// ARM向独立门禁发送协议
					case 0x04:
						System.out.println("ARM向独立门禁发送协议");
						break;

					// 独立门禁向ARM发送协议
					case 0x05:
						System.out.println("独立门禁向ARM发送协议");
						break;

					// MCU升级
					case 0x20:

						System.out.println("MCU");

						timeOutCount = 0;
						// SerialPortInfoUpdate.doparse(value, ac);
						byte ARMcyclicTemp = ARMcyclic;
						// MCU_RUN run = new
						// MCU_RUN(value,ac,ARMcyclicTemp,value[4]);
						// ac.welHandler.postDelayed(run , 500);

						break;

					// ARM向NFC发送信息
					case 0x08:
						System.out.println("ARM向NFC发送信息");
						break;

					// 背光指令回复
					case 0x13:
						// SerialPortInfoControl.getInstance()
						// .backlight_control_parse(value[6]);
						System.out.println("背光指令回复");

						break;

					// 锁控延迟时间回复
					case 0x14:
						// SerialPortInfoControl.getInstance().door_control_delay(
						// value[6]);

						break;

					// 巡检时间设置回复
					case 0x16:

						// SerialPortInfoControl.getInstance()
						// .routing_inspection_time_result(value[6]);
						System.out.println("巡检时间设置回复");

						break;

					// 巡检开关回复
					case 0x17:

						// SerialPortInfoControl.getInstance().A20_updata_result(
						// value[6],ac);
						System.out.println("巡检开关操作回复");

						break;

					// 设置MCU强制重启A20密码回复
					case 0x18:

						// SerialPortInfoControl.getInstance().set_MCU_forcePwd_result(value[6]);

						break;

					// 读取MCU强制重启A20密码回复
					case 0x19:

						int lengh = value[6];

						if (lengh == 0) {

							System.out.println("密码为null");
							Editor mEditor = ToolsParser
									.getSharedPreferences(ac);
							mEditor.putString("mcu_password", "");
							mEditor.commit();
							break;

						}
						byte[] pwd = new byte[lengh];
						for (int i = 0; i <= lengh - 1; i++) {

							pwd[i] = value[7 + i];

						}
						// SerialPortInfoControl.getInstance().get_MCU_forcePwd_result(pwd);
						System.out.println("读取MCU强制重启A20密码回复");
						Editor mEditor = ToolsParser.getSharedPreferences(ac);
						mEditor.putString("mcu_password",
								SerialPortSupport.HexStringUnZeroBack(pwd));
						mEditor.commit();

						break;

					// 获取MCU版本回复
					case 0x21:
						// SerialPortInfoControl.getInstance().getMCU_version(
						// new byte[] { value[6], value[7] }, ac);
						System.out.println("获取MCU版本回复");

					}

				} else {

					System.out.println("该回复已经执行");

				}

			}

		} else {

			// 校验失败 抛弃数据
			System.out.println("校验失败");

		}

	}

	ByteArrayOutputStream baos;

    /*
	private void Deal_security(byte[] value) {

		// TODO Auto-generated method stub
		byte decPart = value[6];
		short allPackages = (short) (0xffff & value[7]);
		short crtPackages = (short) (0xffff & value[8]);
		byte[] content = new byte[256];
		answerIdty(decPart, value[7], value[8]);

		Log.i(TAG, decPart + "===========" + allPackages + "=========="
				+ crtPackages + "====" + value.length);
		if (baos == null)
			baos = new ByteArrayOutputStream();
		System.arraycopy(value, 9, content, 0, value.length - 10);
		try {
			baos.write(content);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if (crtPackages == 0) {// 收到第一个包数，发出广播信息
			// parseSecurityData(decPart, baos.toByteArray());

			Intent intent = new Intent(MainActivity.ID_CHAR_INFO);
			String chInfo = Base64.encodeToString(baos.toByteArray(),
					Base64.DEFAULT);
			intent.putExtra("IDCHAR", chInfo);
			intent.putExtra("INFODRCT", decPart); // 门内外标志位
			ac.sendBroadcast(intent);
			timeID_b = System.currentTimeMillis();
			Log.v(TAG, "获取到身份证信息  用时： " + (timeID_b - timeID_a) + " ms");

		}
		if (crtPackages == allPackages - 1) {
			// 所有包接收完整
			byte[] pictures = baos.toByteArray();
			Log.i(TAG, "=========>照片包接收完整========");
			int savePic = SaveBMP(pictures);
			if (savePic == 1) {
				Intent intent = new Intent(MainActivity.ID_PICTURE_INFO);
				String picture64tostring = Base64.encodeToString(pictures,
						Base64.DEFAULT);
				intent.putExtra("PICTUREBYTE", picture64tostring);
				ac.sendBroadcast(intent);
				timeID_b = System.currentTimeMillis();
				Log.v(TAG, "身份证信息获取完毕  用时： " + (timeID_b - timeID_a)
						+ " ms");
			}
			try {
				if (baos != null)
					baos.close();
				baos = null;
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		}
		// 回复mcu

	}

	private int SaveBMP(byte[] pictures) {
		// TODO Auto-generated method stub
		int Readflage = -1;
		int ret = IDCReaderSDK.Init();
		if (ret == 0) {
			Log.i(TAG, "picture.lenght" + pictures.length);
			byte[] datawlt = new byte[1384];
			byte[] head = { (byte) 0xAA, (byte) 0xAA, (byte) 0xAA, (byte) 0x96,
					(byte) 0x69, (byte) 0x05, (byte) 0x08, (byte) 0x00,
					(byte) 0x00, (byte) 0x90, (byte) 0x01, (byte) 0x00,
					(byte) 0x04, (byte) 0x00 };
			byte[] byLicData = { (byte) 0x05, (byte) 0x00, (byte) 0x01,
					(byte) 0x00, (byte) 0x5B, (byte) 0x03, (byte) 0x33,
					(byte) 0x01, (byte) 0x5A, (byte) 0xB3, (byte) 0x1E,
					(byte) 0x00 };
			System.arraycopy(head, 0, datawlt, 0, head.length);
			System.arraycopy(pictures, 0, datawlt, head.length, 1281);
			int t = IDCReaderSDK.unpack(datawlt, byLicData);
			if (t == 1) {
				Readflage = 1;// 解析成功
				Log.i(TAG, "------图片解析成功－－－－－");
			} else {

				Log.i(TAG, "------图片解析错误－－－－－");
			}
		} else {
			Log.i(TAG, "------图片解析--错误－－－－－");
		}
		return Readflage;
	}
	*/

	// 字节转int
	public int byteToint(byte b[]) {
		int t1 = (b[3] & 0xff) << 24;
		int t2 = (b[2] & 0xff) << 16;
		int t3 = (b[1] & 0xff) << 8;
		int t4 = b[0] & 0xff;
		// System.out.println(b[1]&0xff);//输出的是一个整形数据
		// 在java中，设计int和比int位数来的小的类型b，如byte,char等，都是先把小类型扩展成int再来运算，
		// return( t1<<24)+(t2<<16)+(t3<<8)+t4;//必须加括号
		return t1 + t2 + t3 + t4;
	}

	/**
	 * 身份证信息回复mcu
	 * 
	 * @param decPart
	 * @param allpackage
	 * @param crtpackage
	 */

	public void answerIdty(byte decPart, byte allpackage, byte crtpackage) {
		Log.i(TAG, "answer to mcu");
		byte[] now = new byte[] { (byte) 0xAD, 0x00, 0x0B,
				SerialPortService.MCUcyclic, 0x1C, 0x02, decPart, allpackage,
				crtpackage, 0x01, 0x00 };// 通讯标示暂时都是成功
		now[now.length - 1] = SerialPortSupport.getCheckValue(now);
		SerialPortService.getInstance().write(now);
	}

    /*
	private void parseSecurityData(byte decPart, byte[] byteArray) {
		// TODO Auto-generated method stub
		Log.i(TAG, "------" + byteArray.length + "   ");
		if (byteArray.length != 256)
			return;
		switch (decPart) {
		case 0x01:
			Log.i(TAG, SECURIT_INFO + "机内");
			break;
		case 0x02:
			Log.i(TAG, SECURIT_INFO + "机外");
			break;
		}
		int parseIndex = 0;
		byte[] namebyte = null;
		namebyte = getIbyte(NAME_LENGTH + 2);
		System.arraycopy(byteArray, parseIndex, namebyte, 2, NAME_LENGTH);
		String Iname = null;
		parseIndex += NAME_LENGTH;

		byte[] sexbyte = null;
		sexbyte = getIbyte(SEX_LENGTH + 2);
		System.arraycopy(byteArray, parseIndex, sexbyte, 2, SEX_LENGTH);
		String Isex = null;
		parseIndex += SEX_LENGTH;

		byte[] familybyte = null;
		familybyte = getIbyte(FAIMLY_LENGTH + 2);
		System.arraycopy(byteArray, parseIndex, familybyte, 2, FAIMLY_LENGTH);
		String Ifamily = null;
		parseIndex += FAIMLY_LENGTH;

		byte[] bornDatebyte = null;
		bornDatebyte = getIbyte(BORNDATE_LENGTH + 2);
		System.arraycopy(byteArray, parseIndex, bornDatebyte, 2,
				BORNDATE_LENGTH);
		String IbornDate = null;
		parseIndex += BORNDATE_LENGTH;

		byte[] addressbyte = null;
		addressbyte = getIbyte(ADDRESS_LENGHT + 2);
		System.arraycopy(byteArray, parseIndex, addressbyte, 2, ADDRESS_LENGHT);
		String Iaddress = null;
		parseIndex += ADDRESS_LENGHT;

		byte[] idbyte = null;
		idbyte = getIbyte(ID_LENGTH + 2);
		System.arraycopy(byteArray, parseIndex, idbyte, 2, ID_LENGTH);
		String Iid = null;
		parseIndex += ID_LENGTH;

		byte[] isartybyte = null;
		isartybyte = getIbyte(IS_ATRY_LENGTH + 2);
		System.arraycopy(byteArray, parseIndex, isartybyte, 2, IS_ATRY_LENGTH);
		String Iisarty = null;
		parseIndex += IS_ATRY_LENGTH;

		byte[] startDatebyte = null;
		startDatebyte = getIbyte(START_DATE_LENGTH + 2);
		System.arraycopy(byteArray, parseIndex, startDatebyte, 2,
				START_DATE_LENGTH);
		String IstartDate = null;
		parseIndex += START_DATE_LENGTH;

		byte[] endDatebyte = null;
		endDatebyte = getIbyte(END_DATE_LENGTH + 2);
		System.arraycopy(byteArray, parseIndex, endDatebyte, 2, END_DATE_LENGTH);
		String IendDate = null;
		parseIndex += END_DATE_LENGTH;
		try {

			Iname = new String(namebyte, "UCS-2");
			Isex = new String(sexbyte, "UCS-2");
			Ifamily = new String(familybyte, "UCS-2");
			IbornDate = new String(bornDatebyte, "UCS-2");
			Iaddress = new String(addressbyte, "UCS-2");
			Iid = new String(idbyte, "UCS-2");
			Iisarty = new String(isartybyte, "UCS-2");
			IstartDate = new String(startDatebyte, "UCS-2");
			IendDate = new String(endDatebyte, "UCS-2");

			Log.i(TAG, SECURIT_INFO + "name :" + Iname + "\n Sex:" + Isex
					+ "\n family :" + Ifamily + "\n bornDate :" + IbornDate
					+ "\n address :" + Iaddress + "\n id :" + Iid
					+ "\n issusAuthority :" + Iisarty + "\n start date:"
					+ IstartDate + "\n end date:" + IendDate);
		} catch (UnsupportedEncodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		String idInfo = null;
		byte[] infobyte = new byte[258];
		infobyte[0] = (byte) 0xFF;
		infobyte[1] = (byte) 0xFE;
		System.arraycopy(byteArray, 0, infobyte, 2, 256);

		try {
			idInfo = new String(infobyte, "UCS-2");
			Log.i(TAG, SECURIT_INFO + "name :" + idInfo);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		// new String(data, charsetName)
		// new String(data, )

	}
	*/

	public byte[] getIbyte(int length) {

		byte[] buf = new byte[length];
		buf[0] = (byte) 0xFF;
		buf[1] = (byte) 0xFE;
		return buf;

	}

	private void printByte(byte[] buf) {
		// TODO Auto-generated method stub
		StringBuffer Value = new StringBuffer();
		for (int i = 0; i < buf.length; i++) {
			Value.append(SerialPortSupport.HexString(buf[i]) + " ");
		}
		Log.i("原始数据", "********" + Value.toString() + "SIZE" + buf.length
				+ "*****");

	}

	/**
	 * 
	 * @param value
	 *            SPI通讯传入的内容
	 * 
	 * @return 校验结果是否成功
	 * 
	 * */
	public static boolean checkValue(byte[] value) {

		// SerialPortSupport.printHexString(value);

		// 校验头
		if (!SerialPortSupport.HexString(value[0]).equals("AD")) {

			return false;

		}

		// 校验校验码
		if (SerialPortSupport.getUnsignedByte(SerialPortSupport
				.getCheckValue(value)) != SerialPortSupport
				.getUnsignedByte(value[value.length - 1])) {

			System.out.println("\n"
					+ SerialPortSupport.getUnsignedByte(SerialPortSupport
							.getCheckValue(value)) + "校验结果");
			System.out.println(SerialPortSupport
					.getUnsignedByte(value[value.length - 1]) + "校验数");

			return false;

		}

		return true;

	}

	/**
	 * 
	 * @param value
	 *            SPI通讯传入的内容
	 * 
	 * @return 确认协议方向 true为发送 false为回复
	 * 
	 * */
	public static boolean checkOrient(byte value) {

		if (value == 0x01) {

			return true;

		}

		else {

			return false;

		}

	}

	class MCU_RUN implements Runnable {

		private byte[] value;
		// private MainActivity ac;
		private byte ARMcyclicTemp;
		private byte now;

		@Override
		public void run() {
			// TODO Auto-generated method stub
			if (ARMcyclicTemp == ARMcyclic && now == 0x20
					&& FileInfo.isMCUupdate) {

				if (timeOutCount == 3) {

					timeOutCount = 0;
					System.out.println("升级失败");
					FileInfo.isMCUupdate = false;

				} else {

					timeOutCount++;
					System.out.println("发包超时");
					SerialPortService.getInstance().write(
							FileInfo.getMCUpack(FileInfo.nowbackCount));
					// ac.welHandler.postDelayed(new
					// MCU_RUN(value,ac,ARMcyclicTemp,(byte)0x20), 500);

				}

			}

		}

	}

}
