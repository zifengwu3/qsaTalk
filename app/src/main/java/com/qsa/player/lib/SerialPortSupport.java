package com.qsa.player.lib;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Locale;

import android.util.Log;

public class SerialPortSupport {

	// 循环码初值
	private static byte cyclic = 0;
	// 巡检异常计数
	public static int inspectionCount = 0;
	private final static String TAG = SerialPortSupport.class.getSimpleName();

	// 将指定byte转换成16进制字符串
	public static String HexString(byte b) {

		String hex = Integer.toHexString(b & 0xFF);
		return hex.toUpperCase(Locale.getDefault());

	}

	// 将指定byte转换成16进制字符串
	public static String HexStringAdd(byte b) {

		String hex = Integer.toHexString(b & 0xFF);
		String str;
		if (hex.toUpperCase(Locale.getDefault()).length() == 1) {

			str = "0" + hex.toUpperCase(Locale.getDefault());

		} else {

			str = hex.toUpperCase(Locale.getDefault());

		}

		return str;

	}

	/**
	 * 
	 * 将多个字节内容合并为一个数
	 * 
	 * @param b
	 *            分别表示长度的2个相邻字节
	 * 
	 * */
	public static int getCount(byte[] b) {

		String str = "";
		String value = "";
		for (int i = 0; i < b.length; i++) {

			if (HexString(b[i]).length() < 2) {

				value = "0" + HexString(b[i]);

			} else {

				value = HexString(b[i]);

			}
			str += value;
		}

		return Integer.parseInt(str, 16);
	}

	/***
	 * 
	 * @param data
	 *            需要处理的16进制byte数据
	 * @return 无符号数据
	 * 
	 * **/
	public static int getUnsignedByte(byte data) { // 将data字节型数据转换为0~255 (0xFF
													// 即BYTE)。
		return data & 0x0FF;
	}

	/**
	 * 
	 * @param data
	 *            需要处理的字节数组
	 * 
	 * @return 如果校验结果小于255 取最低位字节
	 * 
	 * */
	public static byte getCheckValue(byte[] data) {

		// byte result;
		int checkValue = 0;
		for (int i = 0; i < data.length - 1; i++) {

			checkValue += getUnsignedByte(data[i]);

		}

		return (byte) (checkValue % 256);
	}

	/**
	 * Convert hex string to byte[]
	 * 
	 * @param hexString
	 *            the hex string
	 * @return byte[]
	 */
	public static byte hexStringToBytes(String hexString) {

		if (hexString == null || hexString.equals("")) {
			return 0;
		}
		hexString = hexString.toUpperCase(Locale.getDefault());
		int length = hexString.length() / 2;
		char[] hexChars = hexString.toCharArray();
		byte d = 0;
		for (int i = 0; i < length; i++) {
			int pos = i * 2;
			d = (byte) (charToByte(hexChars[pos]) << 4 | charToByte(hexChars[pos + 1]));
		}
		return d;

	}

	/**
	 * Convert char to byte
	 * 
	 * @param c
	 *            char
	 * @return byte
	 */
	private static byte charToByte(char c) {
		return (byte) "0123456789ABCDEF".indexOf(c);
	}

	public synchronized static byte getCyclic() {

		if (cyclic <= 255) {

			cyclic++;

		} else {

			cyclic = 0;

		}
		return cyclic;

	}

	// 将指定byte数组以16进制的形式打印到控制台
	public static void printHexString(byte[] b) {
		for (int i = 0; i < b.length; i++) {
			String hex = Integer.toHexString(b[i] & 0xFF);
			if (hex.length() == 1) {
				hex = '0' + hex;
			}
			System.out.print(hex.toUpperCase(Locale.getDefault()) + " ");
		}
		System.out.println("");
	}

	/**
	 * @param int value 设置的参数值
	 * 
	 * @return 返回2个16进制数字的byte[]
	 * 
	 * **/
	public static byte[] setCount(int value) {

		String str = Integer.toHexString(value);
		System.out.println(str.length());
		if (str.length() < 2) {

			str = "000" + str;

		} else if (str.length() < 3) {

			str = "00" + str;

		} else if (str.length() < 4) {

			str = "0" + str;

		}
		byte[] count = new byte[2];
		count[0] = hexStringToBytes(str.substring(0, 2));
		count[1] = hexStringToBytes(str.substring(2, 4));

		return count;

	}

	// 返回一个byte数组

	public static byte[] getBytesFromFile(File file) throws IOException {

		InputStream is = new FileInputStream(file);

		// 获取文件大小

		long length = file.length();

		if (length > Integer.MAX_VALUE) {

			// 文件太大，无法读取
			// 2015 2-4 修改
			is.close();
			throw new IOException("File is to large " + file.getName());

		}

		// 创建一个数据来保存文件数据

		byte[] bytes = new byte[(int) length];

		// 读取数据到byte数组中

		int offset = 0;

		int numRead = 0;

		while (offset < bytes.length

		&& (numRead = is.read(bytes, offset, bytes.length - offset)) >= 0) {

			offset += numRead;

		}

		// 确保所有数据均被读取

		if (offset < bytes.length) {

			// 2015 2-4 修改
			is.close();
			throw new IOException("Could not completely read file "
					+ file.getName());

		}

		// Close the input stream and return bytes

		is.close();

		return bytes;

	}

	// 解析门禁卡 解析类型 IC ID 身份证 NFC卡
	/** 解析失败返回error **/
	public static String ParseCard(byte[] cardValue) {

		int cardLenght = Integer.parseInt(HexString(cardValue[6]), 16);
		System.out.println("门禁信息长度:" + cardLenght);

		if (cardLenght == 4) {

			String ICnumber = HexStringAdd(cardValue[7])
					+ HexStringAdd(cardValue[8]) + HexStringAdd(cardValue[9])
					+ HexStringAdd(cardValue[10]);
			Long ICnumberLong = Long.parseLong(ICnumber, 16);
			ICnumber = zerofill(ICnumberLong.toString(), 10);
			ICnumber = ICnumber.substring(2, ICnumber.toString().length());
			Log.d(TAG, "当前卡为IC:" + ICnumber);

			return ICnumber;

		}
		if (cardLenght == 5) {

			String IDnumber = HexStringAdd(cardValue[8])
					+ HexStringAdd(cardValue[9]) + HexStringAdd(cardValue[10])
					+ HexStringAdd(cardValue[11]);
			Long IDnumberLong = Long.parseLong(IDnumber, 16);
			IDnumber = zerofill(IDnumberLong.toString(), 10);
			IDnumber = IDnumber.substring(2, IDnumber.toString().length());
			Log.d(TAG, "当前卡为ID:" + IDnumber);

			return IDnumber;
		}

		if (cardLenght == 8) {

			String IDnumber = HexStringAdd(cardValue[7])
					+ HexStringAdd(cardValue[8]) + HexStringAdd(cardValue[9])
					+ HexStringAdd(cardValue[10]) + HexStringAdd(cardValue[11])
					+ HexStringAdd(cardValue[12]) + HexStringAdd(cardValue[13])
					+ HexStringAdd(cardValue[14]);
			Long IDnumberLong = Long.parseLong(IDnumber, 16);
			IDnumber = zerofill(IDnumberLong.toString(), 19);
			Log.d(TAG, "当前卡为身份证:" + IDnumber);

			return IDnumber;
		}

		if (cardLenght == 7) {

			String IDnumber = HexStringAdd(cardValue[7])
					+ HexStringAdd(cardValue[8]) + HexStringAdd(cardValue[9])
					+ HexStringAdd(cardValue[10]) + HexStringAdd(cardValue[11])
					+ HexStringAdd(cardValue[12]) + HexStringAdd(cardValue[13]);

			Long IDnumberLong = Long.parseLong(IDnumber, 16);
			IDnumber = zerofill(IDnumberLong.toString(), 17);
			Log.d(TAG, "当前卡为NFC:" + IDnumber);

			return IDnumber;
		}

		return "error";

	}

	// 解析门禁卡 解析类型 IC ID 身份证 NFC卡
	/** 解析失败返回error **/
	public static String ParseCardNew(byte[] cardValue) {

		int cardLenght = Integer.parseInt(HexString(cardValue[7]), 16);
		String cardType = HexStringAdd(cardValue[6]);
		System.out.println("门禁信息长度:" + cardLenght);

		if (cardType.equals("01")) {

			String ICnumber = HexStringAdd(cardValue[8])
					+ HexStringAdd(cardValue[9]) + HexStringAdd(cardValue[10])
					+ HexStringAdd(cardValue[11]);
			Long ICnumberLong = Long.parseLong(ICnumber, 16);
			ICnumber = zerofill(ICnumberLong.toString(), 10);
			ICnumber = ICnumber.substring(2, ICnumber.toString().length());
			Log.d(TAG, "当前卡为IC:" + ICnumber);

			return ICnumber;

		}
		if (cardType.equals("02")) {

			String IDnumber = HexStringAdd(cardValue[9])
					+ HexStringAdd(cardValue[10]) + HexStringAdd(cardValue[11])
					+ HexStringAdd(cardValue[12]);
			Long IDnumberLong = Long.parseLong(IDnumber, 16);
			IDnumber = zerofill(IDnumberLong.toString(), 10);
			IDnumber = IDnumber.substring(2, IDnumber.toString().length());
			Log.d(TAG, "当前卡为ID:" + IDnumber);

			return IDnumber;
		}

		if (cardType.equals("05")) {

			String IDnumber = HexStringAdd(cardValue[8])
					+ HexStringAdd(cardValue[9]) + HexStringAdd(cardValue[10])
					+ HexStringAdd(cardValue[11]) + HexStringAdd(cardValue[12])
					+ HexStringAdd(cardValue[13]) + HexStringAdd(cardValue[14])
					+ HexStringAdd(cardValue[15]);
			Long IDnumberLong = Long.parseLong(IDnumber, 16);
			IDnumber = zerofill(IDnumberLong.toString(), 19);
			Log.d(TAG, "当前卡为身份证:" + IDnumber);

			return IDnumber;
		}

		if (cardType.equals("04")) {

			String IDnumber = HexStringAdd(cardValue[8])
					+ HexStringAdd(cardValue[9]) + HexStringAdd(cardValue[10])
					+ HexStringAdd(cardValue[11]) + HexStringAdd(cardValue[12])
					+ HexStringAdd(cardValue[13]) + HexStringAdd(cardValue[14]);

			Long IDnumberLong = Long.parseLong(IDnumber, 16);
			IDnumber = zerofill(IDnumberLong.toString(), 17);
			Log.d(TAG, "当前卡为NFC:" + IDnumber);

			return IDnumber;
		}

		if (cardType.equals("03")) {

			String IDnumber = HexStringAdd(cardValue[8])
					+ HexStringAdd(cardValue[9]) + HexStringAdd(cardValue[10])
					+ HexStringAdd(cardValue[11]) + HexStringAdd(cardValue[12])
					+ HexStringAdd(cardValue[13]);

			// Long IDnumberLong = Long.parseLong(IDnumber, 16);
			// IDnumber = zerofill(IDnumberLong.toString(), 12);
			Log.d(TAG, "当前卡为手机串号卡:" + IDnumber);

			return IDnumber;
		}

		if (cardType.equals("00")) {

			Log.d(TAG, "当前位置卡类型");

			return "error";
		}

		return "error";

	}

	/**
	 * 
	 * 计算不足位数自动补零
	 * 
	 * @param content
	 *            计算后卡号
	 * 
	 * @param goalInt
	 *            目标位数
	 * 
	 * @return 补零后的值
	 * 
	 * */
	private static String zerofill(String content, int goalInt) {

		if (content.length() < goalInt) {

			int addValue = goalInt - content.length();
			Log.d(TAG, "当前计算的卡号需要补足的位数为：" + addValue);
			for (int i = 0; i < addValue; i++) {

				content = "0" + content;

			}

		} else {

			Log.d(TAG, "当前卡号无需补位");

		}

		return content;
	}

	/**
	 * 
	 * @param b
	 *            传入的版本字节数组
	 * 
	 * @return BCD版本号
	 * 
	 * */
	public static String HexString_BCD_VERSION(byte[] b) {
		String BCD = "";
		for (int i = 0; i < b.length; i++) {
			String hex = Integer.toHexString(b[i] & 0xFF);

			if (i == b.length - 1) {

				BCD += hex.toUpperCase(Locale.getDefault());

			} else {

				BCD += hex.toUpperCase(Locale.getDefault()) + ".";

			}

		}
		return BCD;
	}

	// 将指定byte数组以16进制的形式打印到控制台
	public static void HexStringUnZero(byte[] b) {
		for (int i = 0; i < b.length; i++) {
			String hex = Integer.toHexString(b[i] & 0xFF);
			System.out.print(hex.toUpperCase(Locale.getDefault()));
		}

	}

	// 将指定byte数组以16进制的形式打印到控制台
	public static String HexStringUnZeroBack(byte[] b) {

		String str = "";
		for (int i = 0; i <= b.length-1; i++) {
			String hex = Integer.toHexString(b[i] & 0xFF);
			System.out.print("--"+hex.toUpperCase(Locale.getDefault()));
			str += hex.toUpperCase(Locale.getDefault());
		}
		return str;
	}

}
