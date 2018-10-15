#include "JNIDemo.h"
#include "stdlib.h"
#include <jni.h>
#include <assert.h>

JNIEXPORT jstring JNICALL Java_JNIDemo_TestString
  (JNIEnv *env, jobject obj, jstring jstr){
        char* cha = (char*)env->GetStringUTFChars(jstr, NULL);
        printf("[JNI] ==> %s calling JNI str:[%s]\n", __func__, cha);
        //下面语句有效?
        sprintf(cha, " c++");

        jstring ret = env->NewStringUTF(cha);
        env->ReleaseStringUTFChars(jstr, cha);
        return ret;//返回" c++"
}
/*
 * 定义native方法
 */
JNIEXPORT jstring JNICALL native_hello(JNIEnv *env, jclass clazz) 
{ 
    printf("hello in c++ native code.\n"); 
    return env->NewStringUTF("hello world returned."); 
} 
// 指定要注册的类
#define JNIREG_CLASS "JNIDemo"
// 定义一个JNINativeMethod数组，其中的成员就是Java代码中对应的native方法
static JNINativeMethod gMethods[] = {
        { "hello", "()Ljava/lang/String;", (void*)native_hello},
};
static int registerNativeMethods(JNIEnv* env, const char* className, JNINativeMethod* gMethods, int numMethods) 
{ 
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) 
    {
        return JNI_FALSE; 
    } 
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) 
    { 
        return JNI_FALSE; 
    } 
    return JNI_TRUE; 
}

static int registerNatives(JNIEnv* env) 
{ 
    if (!registerNativeMethods(env, JNIREG_CLASS, gMethods, sizeof(gMethods) / sizeof(gMethods[0]))) 
    { 
        return JNI_FALSE; 
    } 
    return JNI_TRUE; 
}
/*
 * 动态注册工作在这里进行
 * 这个是JNI的一个入口函数，我们在Java层通过System.loadLibrary加载完动态库后，
 * 紧接着就会去查找一个叫JNI_OnLoad的方法。如果有，就会调用它
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) 
{ 
    JNIEnv* env = NULL;
    jint result = -1;
    if (vm-> GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) 
    { 
        return -1; 
    } 
    assert(env != NULL);
    if (!registerNatives(env)) 
    { 
        //注册 
        return -1;
    } 
    result = JNI_VERSION_1_4;
    return result; 
}

