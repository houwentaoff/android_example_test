#include "JNIDemo.h"
#include "stdlib.h"
#include <jni.h>
#include <assert.h>
#include <thread>
#include <unistd.h>
#include <stdio.h>

JavaVM *g_VM;
jobject g_obj;

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
JNIEXPORT void JNICALL test_array(JNIEnv *env, jobject jobj, jintArray arr) 
{ 
    int length = env->GetArrayLength(arr);
    int* arrp = env->GetIntArrayElements(arr, 0);
    int i = 0;
    printf("arr len[%d]\n", length);
    for(i = 0; i<length; i++)
    {
        *(arrp + i) += 10; //将数组中的每个元素加10
        printf("%d ", *(arrp + i));
    }
    printf("\n");
    //JNI_COMMIT : forces the native array to be copied back to the original array in the Java virtual machine
    //0 : copy back and free the elems buffer
    env->ReleaseIntArrayElements(arr, arrp, 0);
} 
JNIEXPORT void JNICALL SendCmd(JNIEnv *env, jobject jobj, jbyteArray cmd) 
{
    int length = env->GetArrayLength(cmd);
    jbyte* arrp = env->GetByteArrayElements(cmd, 0);
    int i = 0;
    printf("send cmd .");
    for (i=0; i<length; i++)
    {
        printf("0x%2.2x ", arrp[i]);
    }
    printf("\n");
    env->ReleaseByteArrayElements(cmd, arrp, JNI_ABORT);
}
JNIEXPORT jbyteArray JNICALL RecvCmd(JNIEnv *env, jobject jobj) 
{
    int i = 0;
    jbyteArray jbytes = env->NewByteArray(7);
    jbyte *elems = env->GetByteArrayElements(jbytes, NULL); 
    for (i=0; i<6; i++)
    {
        elems[i] = i;
    }
    env->ReleaseByteArrayElements(jbytes, elems, 0);
    return jbytes;
}
JNIEXPORT void JNICALL Download(JNIEnv *env, jobject jobj) 
{
    jclass jSdkClass; 
    jSdkClass =env->GetObjectClass(jobj);
    if (jSdkClass == NULL)
    {
        printf("unable to find class\n");
        return;
    }
    jmethodID javaCallback = env->GetMethodID(jSdkClass,
            "onProgressCallBack", "(JJ)I");
    jint ret = env->CallIntMethod(jobj, javaCallback, 5, 1);
    return;
}
void *func(void *data)
{
    JNIEnv *env = NULL;
    ::sleep(3);
    int envStat = g_VM->GetEnv((void **) &env,  JNI_VERSION_1_4);
    if (envStat == JNI_EDETACHED)
    {
        if (g_VM->AttachCurrentThread((void **)&env, NULL))
        {
            return NULL;
        }
    }
    jclass javaClass = env->GetObjectClass(g_obj);
    if (javaClass == 0)
    {
        g_VM->DetachCurrentThread();
        return NULL;
    }
    //该方法可以为java中的接口，这样只要实现了该接口就能进行回调
    //不用知道每个类要回调的方法的名字
    jmethodID javaCallback = env->GetMethodID(javaClass,
            "onProgressCallBack", "(JJ)I");
    long total = 7;
    long over = 0;
    do 
    {
        jint ret = env->CallIntMethod(g_obj, javaCallback, total, over);
        sleep(1);
    }
    while (total > over++);
    g_VM->DetachCurrentThread();
    return NULL;
}
JNIEXPORT void JNICALL MulThreadDownload(JNIEnv *env, jobject jobj) 
{
    std::thread tid;
    env->GetJavaVM(&g_VM);
    // 生成一个全局引用保留下来，以便回调
    g_obj = env->NewGlobalRef(jobj);
    tid = std::thread(func, nullptr);
    tid.detach();
}
// 指定要注册的类
#define JNIREG_CLASS "JNIDemo"
// 定义一个JNINativeMethod数组，其中的成员就是Java代码中对应的native方法
// javap -s -p JNIDemo
static JNINativeMethod gMethods[] = {
        { "hello", "()Ljava/lang/String;", (void*)native_hello},
        { "EncodeArray", "([I)V", (void*)test_array},
        { "SendCmd", "([B)V", (void*)SendCmd},
        { "RecvCmd", "()[B", (void*)RecvCmd},
        { "downLoad", "()V", (void*)Download},
        { "MulThreadDL", "()V", (void*)MulThreadDownload},
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

