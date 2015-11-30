package com.qsa.player.lib;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;

import android.os.Environment;



public class FileCut {

	private String fileName;
	private int size;

	public FileCut(String fileName, String size) {

		this.fileName = fileName;
		this.size = Integer.parseInt(size);

	}

	public FileInfo cut() throws Exception {

		//清除上一次的缓存记录
		File file = new File(getSDPath());
		if(file.listFiles().length>0&&file.listFiles()!=null){
			
			for(int i = 0; i < file.listFiles().length ; i++){
				
				file.listFiles()[i].delete();
				
			}
			
		}
		
		FileInfo fileNow = new FileInfo();
		
		int maxx = 0;
		File inFile = new File(fileName);
		int fileLength = (int) inFile.length(); // 取得文件的大小
		System.out.println(fileLength + "");
		RandomAccessFile count = new RandomAccessFile(inFile, "r");

		byte[] buff = new byte[8];
		count.skipBytes(fileLength - 8);
		count.read(buff);
		count.close();
		
		ByteArrayInputStream bintput = new ByteArrayInputStream(buff);

		BufferedReader reader = new BufferedReader(new InputStreamReader(
				bintput));
		StringBuilder sb = new StringBuilder();
		String line = null;
		while ((line = reader.readLine()) != null) {
			sb.append(line + "\n");
		}
		System.out.println(sb);

		// }

		int value; // 取得要分割的个数
		RandomAccessFile inn = new RandomAccessFile(inFile, "r");
		// 打开要分割的文件
		value = fileLength / size;
		int i = 0;
		int j = 0;
		// 根据要分割的数目输出文件
		for (; j < value; j++) {
			File outFile = new File(getSDPath()+"/" + j + "cut.txt");
			fileNow.file.add(outFile);
			RandomAccessFile outt = new RandomAccessFile(outFile, "rw");
			maxx += size;
			for (; i < maxx; i++) {
				outt.write(inn.read());
			}
			outt.close();
		}
		fileNow.boxheading = buff;
		fileNow.boxsize = fileNow.file.size();
		fileNow.allbackCount = fileNow.file.size();
//		File outFile = new File(inFile.getName() + j + "cut.txt");
//		RandomAccessFile outt = new RandomAccessFile(outFile, "rw");
//		for (; i <= fileLength; i++) {
//			outt.write(inn.read());
//		}

//		outt.close();
		inn.close();

		return fileNow;
		
	}

	public static String getSDPath() {
		File sdDir = null;
		File file =null;
		boolean sdCardExist = Environment.getExternalStorageState().equals(
				android.os.Environment.MEDIA_MOUNTED); // 判断sd卡是否存在
		if (sdCardExist) {
//			sdDir = Environment.getExternalStorageDirectory();// 获取根目录
//			file = new File(QsaApplication.SAVE_DATA_PATH+"/"+"TempNow");  
	        if (!file.exists()) {  
	            try {  
	                //按照指定的路径创建文件夹  
	                file.mkdirs();  
	            } catch (Exception e) {  
	                // TODO: handle exception 
	            	System.out.println("创建失败");
	            }  
	        }  
			
		} else {

//			file = new File(QsaApplication.SAVE_DATA_PATH+"/"+"TempNow");  
	        if (!file.exists()) {  
	            try {  
	                //按照指定的路径创建文件夹  
	                file.mkdirs();  
	            } catch (Exception e) {  
	                // TODO: handle exception 
	            	System.out.println("创建失败");
	            }  
	        }  
			System.out.println("SD卡不存在");

		}
		
//		return file.toString();
//		return QsaApplication.SAVE_DATA_PATH+"/"+"TempNow";
		return "";
	}
	
}