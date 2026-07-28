#ifdef ANDROID
#include <jni.h>
#include <main.hpp>
#include <android/log.h>
#include <NinecraftApp.hpp>
#include <input/Keyboard.hpp>
#include <utils.h>

jobject mainActivity_ref;
extern "C" {

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	struct timeval v1;
	gettimeofday(&v1, 0);
	startedAtSec = v1.tv_sec; //TODO mojang does it differently?

	pthread_mutex_init(&_D6E04480, 0);
	pthread_mutex_lock(&_D6E04480);
	pthread_self();
	appPlatform.init(vm);
	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeBackPressed(JNIEnv* env, jobject dis){
	ninecraftApp->handleBack();
}
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeBackSpacePressed(JNIEnv* env, jobject dis){
	Keyboard::feed(8, 1);
	Keyboard::feed(8, 0);
}
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeLoginData(JNIEnv* env, jobject dis, jstring a, jstring b, jstring c, jstring d){
    __android_log_write(ANDROID_LOG_ERROR, "MCPE081DECOMP", "Java_com_mojang_minecraftpe_MainActivity_nativeLoginData - not implemented");
}
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeRegisterThis(JNIEnv* env, jobject dis){
	pthread_self();
	mainActivity_ref = env->NewGlobalRef(dis);
	pthread_mutex_unlock(&_D6E04480);
}
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeReturnKeyPressed(JNIEnv* env, jobject dis){
	Keyboard::feed(13, 1);
	Keyboard::feed(13, 0);
}
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeSetTextboxText(JNIEnv* env, jobject dis, jstring s){
	const char* utfChars = env->GetStringUTFChars(s, 0);
	DEBUGMSG("NATIVESETTBTEXT %s", utfChars);
	std::string v7 = !utfChars ? "" : utfChars;
	env->ReleaseStringUTFChars(s, utfChars);
	ninecraftApp->setTextboxText(v7);
}
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeStopThis(JNIEnv* env, jobject dis){

}
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeSuspend(JNIEnv* env, jobject dis){

}
std::string nativeUtf8Input;
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeTypeCharacter(JNIEnv* env, jobject dis, jstring s){
    const char* utfChars = env-> GetStringUTFChars(s, 0);
	if(utfChars){
		nativeUtf8Input = utfChars;
		std::string v8 = nativeUtf8Input;
		Keyboard::feedText(v8, 0);
	}
	printf("@nativeTypeCharacter: %s\n", nativeUtf8Input.c_str());
	env->ReleaseStringUTFChars(s, utfChars);

}
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeUnregisterThis(JNIEnv* env, jobject dis){
	pthread_self();
	env->DeleteGlobalRef(mainActivity_ref);
	mainActivity_ref = 0;
	pthread_mutex_destroy(&_D6E04480);
}
JNIEXPORT void JNICALL Java_com_mojang_minecraftpe_MainActivity_nativeWebRequestCompleted(JNIEnv* env, jobject dis, jint a2, jlong a3, jint a4, jstring a5){
    __android_log_write(ANDROID_LOG_ERROR, "MCPE081DECOMP", "Java_com_mojang_minecraftpe_MainActivity_nativeWebRequestCompleted - not implemented");
}

}
#endif
