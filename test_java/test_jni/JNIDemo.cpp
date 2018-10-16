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
// 指定要注册的类
#define JNIREG_CLASS "JNIDemo"
// 定义一个JNINativeMethod数组，其中的成员就是Java代码中对应的native方法
// javap -s -p JNIDemo
static JNINativeMethod gMethods[] = {
        { "hello", "()Ljava/lang/String;", (void*)native_hello},
        { "EncodeArray", "([I)V", (void*)test_array},
        { "SendCmd", "([B)V", (void*)SendCmd},
        { "RecvCmd", "()[B", (void*)RecvCmd},
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

