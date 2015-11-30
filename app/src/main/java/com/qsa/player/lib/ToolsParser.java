package com.qsa.player.lib;

import java.util.Arrays;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;

public class ToolsParser
{
    private static JSONObject jsonObject;
    private static SharedPreferences mySharedPreferences;

    /**
     * 获取数组类的data
     * 
     * @param json
     *            需要解析的字符串
     * @return data 的字符串
     */
    public static String getJsonDataArray(String json)
    {
            jsonObject = (JSONObject) JSON.parse(json);
            JSONArray array = jsonObject.getJSONArray("data");
            return array.toJSONString();
    }
    
    
    public static SharedPreferences.Editor getSharedPreferences(Context ac) {

        mySharedPreferences = ac.getSharedPreferences("myjson",
                Activity.MODE_PRIVATE);
        // 实例化SharedPreferences.Editor对象（第二步）
        SharedPreferences.Editor editor = mySharedPreferences.edit(); // 用putString的方法保存数据

        return editor;

    }

    public static String getShareString(Context ac,String str) {

        mySharedPreferences = ac.getSharedPreferences("myjson",
                Activity.MODE_PRIVATE);
        // 实例化SharedPreferences.Editor对象（第二步）

        return mySharedPreferences.getString(str, "none");

    }
    
    public static  int getShareInt(Activity ac, String str) {

         mySharedPreferences = ac.getSharedPreferences("myjson",
                Activity.MODE_PRIVATE);
        // 实例化SharedPreferences.Editor对象（第二步）

        return mySharedPreferences.getInt(str, 2);

    }

    public static  int getShareMyInt(Activity ac, String str, int defualt) {

        mySharedPreferences = ac.getSharedPreferences("myjson",
               Activity.MODE_PRIVATE);
       // 实例化SharedPreferences.Editor对象（第二步）

       return mySharedPreferences.getInt(str, defualt);

   }
    public static boolean getShareBoolean(Context ac, String str) {

         mySharedPreferences = ac.getSharedPreferences("myjson",
                Activity.MODE_PRIVATE);
        // 实例化SharedPreferences.Editor对象（第二步）

        return mySharedPreferences.getBoolean(str, false);

    }

    public static boolean getMyShareBoolean(Activity ac, String str) {

        mySharedPreferences = ac.getSharedPreferences("myjson",
               Activity.MODE_PRIVATE);
       // 实例化SharedPreferences.Editor对象（第二步）

       return mySharedPreferences.getBoolean(str, true);

   }
    
    
    public static void commit(SharedPreferences.Editor editor) {

        editor.commit();

    }
    /**
     * 合并多个数组
     */
    @SuppressLint("NewApi")
    public static <T> T[] concatAll(T[] first, T[]... rest) {  
          int totalLength = first.length;  
          for (T[] array : rest) {  
            totalLength += array.length;  
          }  
          T[] result = Arrays.copyOf(first, totalLength);  
          int offset = first.length;  
          for (T[] array : rest) {  
            System.arraycopy(array, 0, result, offset, array.length);  
            offset += array.length;  
          }  
          return result;  
        } 
}
