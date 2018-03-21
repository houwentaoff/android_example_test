#include "JNIDemo.h"
#include "stdlib.h"
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
