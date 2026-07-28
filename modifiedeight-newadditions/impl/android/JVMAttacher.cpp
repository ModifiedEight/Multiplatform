#ifdef ANDROID
#include <android/JVMAttacher.hpp>

JVMAttacher::JVMAttacher(JavaVM* vm){
    this->vm = vm;
    this->env = 0;
    this->attached = 0;
    if(vm->GetEnv((void**) &this->env, 65540)){
        vm->AttachCurrentThread(&this->env, 0);
        this->attached = this->env != 0;
    }
}

void JVMAttacher::forceDetach() {
    if(this->attached){
        this->vm->DetachCurrentThread();
        this->attached = 0;
    }
}
#endif