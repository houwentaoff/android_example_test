//package testjni;
//no package.

import java.net.*;//URI所在包
import java.io.*;//File所在包

public class JNIDemo {
    static{
        //URI必须加try catch,否则编译报错
        try{

            //URI uri = getClass().getResource("/libtest.so").toURI() ;//该句只能使用绝对路径
            //使用相对路径
            URI uri = JNIDemo.class.getResource("./libtest.so").toURI() ;
            String realPath = new File(uri).getAbsolutePath() ; 
            //String realPath = "/libtest.so";
            System.load(realPath);
            
            /*
            System.loadLibrary("test");
            */
        }catch(Exception e)
        {
            System.out.println("Got a Exception：" + e.getMessage());
        }
    }
    //JNI API
    public native String TestString(String str);
    public native String hello();
    public static void main(String[] args) {
        // TODO Auto-generated method stub
        JNIDemo j = new JNIDemo();
        String str="javastr";
        
        System.out.println("before call JNI");
        String s = j.TestString(str);
        System.out.println("end call JNI");
        System.out.println("str[" + str + "]");
        System.out.println("ret["+ s + "]");
        //String enctypt=j.desEncode(user);
        //System.out.println(enctypt);
        
        str = j.hello();
        System.out.println(str);
    }
}
