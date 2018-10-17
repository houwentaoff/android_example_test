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
    int[] array = {1,2,3,4,5};
    //call back
    private int onProgressCallBack(long total, long over)
    {
        System.out.println("total:" + total + " over:" + over);
        double per = (over*1.0)/(total);
        System.out.println("%" +  per * 100);
        return 0;
    }
    //JNI API
    public native String TestString(String str);
    public native String hello();
    public native void EncodeArray(int[] arr);
    public native void SendCmd(byte[] cmd);
    public native byte[] RecvCmd();
    public native void MulThreadDL();
    private native void downLoad();
    
    public void DL()
    {
        downLoad();
    }
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

        j.EncodeArray(j.array);
        for (int i : j.array) {
            System.out.printf("%d ", i);
        }
        System.out.println("");
        byte[] cmd = {1,2,3,4,5, 6};
        j.SendCmd(cmd);
        byte []recvBuf =  j.RecvCmd();
        if (recvBuf.length == 0)
        {
            System.out.println("未收到响应!");
        }
        else
        {
            System.out.println("收到响应!");
            for (byte i : recvBuf){
                System.out.printf("0x%x ", i);
            }
            System.out.println("");
        }
        j.DL();
        j.MulThreadDL();
        try
        {
            Thread.sleep(10000); 
        }
        catch (InterruptedException e) {
            e.printStackTrace(); 
        }
    }
}
