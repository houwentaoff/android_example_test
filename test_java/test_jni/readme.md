# JNI (Java Native Interface)

## 需要材料
工具: 
javac.exe:编译器 
java.exe:虚拟机 
javah.exe:为在 Java 源代码文件中所找到的每个 native 方法定义 C++ 风格的函数 

## 步凑
1. 写一个native方法,testString,作为Java程序中调用的函数
```
public native String TestString(String str);
```
2. 生成C++头文件
`javah -jni JNIDemo`

3. 实现cpp文件
将2中生成的h文件中的声明copy到cpp中并实现

4. 生成so文件
`g++ -fpic -shared -o libtest.so JNIDemo.cpp -I/usr/lib/jvm/java-7-openjdk-i386/include`
`g++ -fpic -shared -o libtest.so JNIDemo.cpp -I /usr/lib/jvm/java-1.8.0-openjdk/include/   -I /usr/lib/jvm/java-1.8.0-openjdk/include/linux/`
` g++ -fpic -shared -o libtest.so JNIDemo.cpp -I /usr/lib/jvm/java-8-openjdk-amd64/include/   -I /usr/lib/jvm/java-1.8.0-openjdk-amd64/include/linux`

5. 编译JAVA
`javac JNIDemo.java`

6. 查看成员变量和方法签名
`javap -s -p JNIDemo`

7. 运行JAVA
`java JNIDemo`

8. 运行结果
```
before call JNI
[JNI] ==> Java_JNIDemo_TestString calling JNI str:[javastr]
end call JNI
str[javastr]
ret[ c++]
```
