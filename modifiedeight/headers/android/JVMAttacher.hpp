#ifdef ANDROID
#pragma once
#include <_types.h>
#include <jni.h>
struct JVMAttacher{
    JavaVM* vm;
    JNIEnv* env;
    bool attached;
    //align 9, a, b

    JVMAttacher(JavaVM*);
    void forceDetach();
};
#endif