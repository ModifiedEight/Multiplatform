#ifdef ANDROID
#include <android/AppPlatform_android.hpp>
#include <android/JVMAttacher.hpp>
#include <network/mco/LoginInformation.hpp>
#include <_AssetFile.hpp>
#include <string>
#include <stb_image.h>
#include <string.h>

AppPlatform_android::AppPlatform_android(){
	this->mainActivityRef = 0;
	this->_jniSaveScreenshot = 0;
	this->_jniPostScreenshotToFacebook = 0;
	this->_jniGetImageData = 0;
	this->_jniGetFileDataBytes = 0;
	this->_jniPlaySound_NOTFOUND = 0;
	this->_jniDisplayDialog = 0;
	this->_jniTick = 0;
	this->_jniQuit = 0;
	this->_jniInitiateUserInput = 0;
	this->_jniGetUserInputStatus = 0;
	this->_jniGetUserInputString = 0;
	this->field_50 = 0;
	this->_jniCheckLicense = 0;
	this->_jniHasBuyButtonWhenInvalidLicense = 0;
	this->_jniBuyGame = 0;
	this->field_60 = 0;
	this->field_64 = 0;
	this->_jniGetPixelsPerMillimeter = 0;
	this->_jniVibrate = 0;
	this->_jniSupportsNonTouchscreen = 0;
	this->_jniSetIsPowerVR = 0;
	this->_jniIsNetworkEnabled = 0;
	this->_jniOpenLoginWindow = 0;
	this->field_80 = 0;
	this->field_84 = 0;
	this->field_88 = 0;
	this->field_8C = 0;
	this->_jniWebRequest = 0;
	this->_jniGetWebRequestStatus = 0;
	this->_jniGetWebRequestContent = 0;
	this->_jniAbortWebRequest = 0;
	this->screenWidth = 854;
	this->_jniGetPlatformStringVar = 0;
	this->windowReference = 0;
	this->contextReference = 0;
	this->_jniS_INPUT_METHOD_SERVICE = 0;
	this->inputMethodHandlerReference = 0;
	this->_jniGetSystemService = 0;
	this->_jniGetWindow = 0;
	this->_jniGetDecorView = 0;
	this->_jniShowSoftInput = 0;
	this->viewReference = 0;
	this->_jniGetWindowToken = 0;
	this->_jniHideSoftInputFromWindow = 0;
	this->_jniGetKeyFromCode = 0;
	this->_jniShowKeyboard = 0;
	this->_jniHideKeyboard = 0;
	this->_jniUpdateTextboxText = 0;
	this->_jniGetAccessToken = 0;
	this->_jniGetClientID = 0;
	this->_jniGetProfileID = 0;
	this->_jniGetProfileName = 0;
	this->_jniSetLoginInformation = 0;
	this->_jniClearLoginInformation = 0;
	this->_jniStatsTrackEvent = 0;
	this->_jniStatsUpdateUserData = 0;
	this->screenHeight = 480;
	this->field_108 = 0;
	this->field_10C = 0;
	this->initialized = 0;
	this->jvm = 0;
	this->mainActivityReference = 0;
}
void AppPlatform_android::init(JavaVM* jvm){
	if(this->initialized) return;
	JVMAttacher v98(jvm);
	if(v98.env){
		jclass clz = v98.env->FindClass("com/mojang/minecraftpe/MainActivity");
		if(clz){
			this->mainActivityReference = (jclass)v98.env->NewGlobalRef(clz);

			this->_jniSaveScreenshot = v98.env->GetStaticMethodID(this->mainActivityReference, "saveScreenshot",
																  "(Ljava/lang/String;II[I)V");
			this->_jniPostScreenshotToFacebook = v98.env->GetMethodID(this->mainActivityReference, "postScreenshotToFacebook", "(Ljava/lang/String;II[I)V");
			this->_jniGetImageData = v98.env->GetMethodID(this->mainActivityReference, "getImageData", "(Ljava/lang/String;)[I");
			this->_jniGetFileDataBytes = v98.env->GetMethodID(this->mainActivityReference, "getFileDataBytes", "(Ljava/lang/String;)[B");
			this->_jniDisplayDialog = v98.env->GetMethodID(this->mainActivityReference, "displayDialog", "(I)V");
			this->_jniTick = v98.env->GetMethodID(this->mainActivityReference, "tick", "()V");
			this->_jniQuit = v98.env->GetMethodID(this->mainActivityReference, "quit", "()V");
			this->_jniInitiateUserInput = v98.env->GetMethodID(this->mainActivityReference, "initiateUserInput", "(I)V");
			this->_jniGetUserInputStatus = v98.env->GetMethodID(this->mainActivityReference, "getUserInputStatus", "()I");
			this->_jniGetUserInputString = v98.env->GetMethodID(this->mainActivityReference, "getUserInputString", "()[Ljava/lang/String;");
			this->field_50 = v98.env->GetMethodID(this->mainActivityReference, "getDateString", "(I)Ljava/lang/String;");
			this->_jniCheckLicense = v98.env->GetMethodID(this->mainActivityReference, "checkLicense", "()I");
			this->_jniHasBuyButtonWhenInvalidLicense = v98.env->GetMethodID(this->mainActivityReference, "hasBuyButtonWhenInvalidLicense", "()Z");
			this->_jniBuyGame = v98.env->GetMethodID(this->mainActivityReference, "buyGame", "()V");
			this->_jniVibrate = v98.env->GetMethodID(this->mainActivityReference, "vibrate", "(I)V");
			this->_jniSupportsNonTouchscreen = v98.env->GetMethodID(this->mainActivityReference, "supportsNonTouchscreen", "()Z");
			this->_jniSetIsPowerVR = v98.env->GetMethodID(this->mainActivityReference, "setIsPowerVR", "(Z)V");
			this->_jniIsNetworkEnabled = v98.env->GetMethodID(this->mainActivityReference, "isNetworkEnabled", "(Z)Z");
			this->_jniOpenLoginWindow = v98.env->GetMethodID(this->mainActivityReference, "openLoginWindow", "()V");
			this->_jniGetPixelsPerMillimeter = v98.env->GetMethodID(this->mainActivityReference, "getPixelsPerMillimeter", "()F");
			this->_jniGetPlatformStringVar = v98.env->GetMethodID(this->mainActivityReference, "getPlatformStringVar", "(I)Ljava/lang/String;");

			this->windowReference = (jclass) v98.env->NewGlobalRef(v98.env->FindClass("android/view/Window"));
			this->contextReference = (jclass) v98.env->NewGlobalRef(v98.env->FindClass("android/content/Context"));
			this->viewReference = (jclass) v98.env->NewGlobalRef(v98.env->FindClass("android/view/View"));
			this->inputMethodHandlerReference = (jclass) v98.env->NewGlobalRef(v98.env->FindClass("android/view/inputmethod/InputMethodManager"));
			this->_jniS_INPUT_METHOD_SERVICE = v98.env->GetStaticFieldID(this->contextReference, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
			this->_jniGetSystemService = v98.env->GetMethodID(this->mainActivityReference, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
			this->_jniGetWindow = v98.env->GetMethodID(this->mainActivityReference, "getWindow", "()Landroid/view/Window;");
			this->_jniGetDecorView = v98.env->GetMethodID(this->windowReference, "getDecorView", "()Landroid/view/View;");
			this->_jniShowSoftInput = v98.env->GetMethodID(
					this->inputMethodHandlerReference,
					"showSoftInput",
					"(Landroid/view/View;I)Z");
			this->_jniGetWindowToken = v98.env->GetMethodID(this->viewReference, "getWindowToken", "()Landroid/os/IBinder;");
			this->_jniHideSoftInputFromWindow = v98.env->GetMethodID(this->inputMethodHandlerReference, "hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z");
			this->_jniWebRequest = v98.env->GetMethodID(this->mainActivityReference, "webRequest", "(IJLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
			this->_jniGetWebRequestStatus = v98.env->GetMethodID(this->mainActivityReference, "getWebRequestStatus", "(I)I");
			this->_jniGetWebRequestContent = v98.env->GetMethodID(this->mainActivityReference, "getWebRequestContent", "(I)Ljava/lang/String;");
			this->_jniAbortWebRequest = v98.env->GetMethodID(this->mainActivityReference, "abortWebRequest", "(I)I");
			this->_jniGetKeyFromCode = v98.env->GetMethodID(this->mainActivityReference, "getKeyFromKeyCode", "(III)I");
			this->_jniShowKeyboard = v98.env->GetMethodID(this->mainActivityReference, "showKeyboard", "(Ljava/lang/String;IZ)V");
			this->_jniHideKeyboard = v98.env->GetMethodID(this->mainActivityReference, "hideKeyboard", "()V");
			this->_jniUpdateTextboxText = v98.env->GetMethodID(this->mainActivityReference, "updateTextboxText", "(Ljava/lang/String;)V");
			this->_jniGetAccessToken = v98.env->GetMethodID(this->mainActivityReference, "getAccessToken", "()Ljava/lang/String;");
			this->_jniGetClientID = v98.env->GetMethodID(this->mainActivityReference, "getClientId", "()Ljava/lang/String;");
			this->_jniGetProfileID = v98.env->GetMethodID(this->mainActivityReference, "getProfileId", "()Ljava/lang/String;");
			this->_jniGetProfileName = v98.env->GetMethodID(this->mainActivityReference, "getProfileName", "()Ljava/lang/String;");

			this->_jniSetLoginInformation = v98.env->GetMethodID(this->mainActivityReference, "setLoginInformation", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
			this->_jniClearLoginInformation = v98.env->GetMethodID(this->mainActivityReference, "clearLoginInformation", "()V");
			this->_jniStatsTrackEvent = v98.env->GetMethodID(this->mainActivityReference, "statsTrackEvent", "(Ljava/lang/String;Ljava/lang/String;)V");
			this->_jniStatsUpdateUserData = v98.env->GetMethodID(this->mainActivityReference, "statsUpdateUserData", "(Ljava/lang/String;Ljava/lang/String;)V");
			if ( v98.env->ExceptionOccurred() )
			{
				v98.env->ExceptionDescribe();
			}


			this->jvm = jvm;
			this->initialized = 1;
		}
	}
	v98.forceDetach();
}
void AppPlatform_android::convertOpenGlToAndroidPixels(int, int, uint*);

AppPlatform_android::~AppPlatform_android(){}
void AppPlatform_android::saveScreenshot(std::string const&, int, int){
	printf("AppPlatform_android::saveScreenshot - not implemented\n"); //TODO
	//used by the very illegal item called camera
}
std::string AppPlatform_android::getImagePath(const std::string& a3, bool_t a4){
	return "images/" + a3; //TODO check
}

static unsigned int _d65c6fb8(unsigned int a1){
	return (a1 & 0xFF00FF00) | (a1 << 16) & 0xFF0000 | ((a1 >> 16) & 0xff);
}

void AppPlatform_android::loadPNG(ImageData& a2, const std::string& a3, bool_t a4){
	if ( this->initialized )
	{
		if ( this->_jniGetImageData )
		{
			JVMAttacher v21(this->jvm);
			jintArray v11 = (jintArray)v21.env->CallObjectMethod(this->mainActivityRef, this->_jniGetImageData, v21.env->NewStringUTF(a3.c_str()));
			if ( v11 )
			{
				int v13 = v21.env->GetArrayLength(v11) - 2;
				int* v14 = (int*) malloc(sizeof(int) * v13);
				int v15 = 0;
				jint* v16 = v21.env->GetIntArrayElements(v11, 0);
				int v17 = v16[0];
				int v20 = v16[1];
				memcpy(v14, v16 + 2, 4 * v13);
				while ( v15 < v13 )
				{
					v14[v15] = _d65c6fb8(v14[v15]);
					++v15;
				}
				v21.env->ReleaseIntArrayElements(v11, v16, 0);
				a2.width = v17;
				a2.pixels = (unsigned char*) v14;
				a2.height = v20;
			}
			v21.forceDetach();
		}
	}
}
void AppPlatform_android::loadTGA(ImageData& a2, const std::string& a3, bool_t a4){
	if ( this->initialized )
	{
		if ( this->_jniGetFileDataBytes )
		{
			JVMAttacher v16(this->jvm);
			jbyteArray v10 = (jbyteArray)v16.env->CallObjectMethod(this->mainActivityRef, this->_jniGetFileDataBytes, v16.env->NewStringUTF(a3.c_str()));
			if ( v10 )
			{
				int len = v16.env->GetArrayLength(v10);
				unsigned char* v13 = (unsigned char*)v16.env->GetByteArrayElements(v10, 0);
				int v15;
				unsigned char* v14 = stbi_load_from_memory(v13, len, &a2.width, &a2.height, &v15, 0);
				a2.field_C = v15 == 3;
				a2.pixels = v14;
				v16.env->ReleaseByteArrayElements(v10, (jbyte *)v13, 0);
			}
			v16.forceDetach();
		}
	}
}
void AppPlatform_android::playSound(const std::string& a2, float a3, float a4){
	if ( this->initialized )
	{
		if ( this->_jniPlaySound_NOTFOUND )
		{
			JVMAttacher v12(this->jvm);
			v12.env->CallVoidMethod(this->mainActivityRef, this->_jniPlaySound_NOTFOUND, v12.env->NewStringUTF(a2.c_str()), a3, a4);
			v12.forceDetach();
		}
	}
}
void AppPlatform_android::showDialog(int32_t a2){
	if ( this->initialized )
	{
		if ( this->_jniDisplayDialog )
		{
			JVMAttacher v4(this->jvm);
			v4.env->CallVoidMethod(this->mainActivityRef, this->_jniDisplayDialog, a2);
			v4.forceDetach();
		}
	}
}
void AppPlatform_android::createUserInput(void){
	if ( this->initialized )
	{
		if ( this->_jniInitiateUserInput )
		{
			JVMAttacher v2(this->jvm);
			v2.env->CallVoidMethod(this->mainActivityRef, this->_jniInitiateUserInput, 1);
			v2.forceDetach();
		}
	}
}
int32_t AppPlatform_android::getUserInputStatus(void){
	if ( !this->initialized || !this->_jniGetUserInputStatus )
	{
		return -2;
	}
	JVMAttacher v4(this->jvm);
	int v2 = v4.env->CallIntMethod(this->mainActivityRef, this->_jniGetUserInputStatus);
	v4.forceDetach();
	return v2;
}
std::vector<std::string> AppPlatform_android::getUserInput(void){
	printf("AppPlatform_android::getUserInput - not implemented\n"); //TODO
	return {};
}
std::string AppPlatform_android::getDateString(int32_t a2){
	if ( this->initialized && this->field_50 )
	{
		JVMAttacher v10(this->jvm);
		jstring o = (jstring) v10.env->CallObjectMethod(this->mainActivityRef, this->field_50);
		std::string v9;
		const char* utfChars = v10.env->GetStringUTFChars(o, 0);
		if(utfChars) v9 = utfChars;
		v10.env->ReleaseStringUTFChars(o, utfChars);
		v10.forceDetach();
		return v9;
	}else{
		return "";
	}
}
int32_t AppPlatform_android::checkLicense(void){
	if ( !this->initialized || !this->_jniCheckLicense )
	{
		return -2;
	}
	JVMAttacher v4(this->jvm);
	int v2 = v4.env->CallIntMethod(this->mainActivityRef, this->_jniCheckLicense);
	v4.forceDetach();
	return v2;
}
bool_t AppPlatform_android::hasBuyButtonWhenInvalidLicense(void){
	if(!this->initialized){
		return 0;
	}
	if(!this->_jniHasBuyButtonWhenInvalidLicense){
		return 0;
	}
	JVMAttacher v7(this->jvm);
	bool v5 = v7.env->CallBooleanMethod(this->mainActivityRef, this->_jniHasBuyButtonWhenInvalidLicense) == 1;
	v7.forceDetach();
	return v5;
}
void AppPlatform_android::uploadPlatformDependentData(int32_t a2, void* a3){
	if(this->initialized){
		if(this->_jniWebRequest){
			JVMAttacher v17(this->jvm);
			v17.forceDetach();
		}
	}
}
AssetFile AppPlatform_android::readAssetFile(const std::string& a3){
	if(this->initialized && this->_jniGetFileDataBytes != 0){
		JVMAttacher v14(this->jvm);
		jbyteArray v10 = (jbyteArray) v14.env->CallObjectMethod(this->mainActivityRef, this->_jniGetFileDataBytes, v14.env->NewStringUTF(a3.c_str()));
		if(v10){
			int len = v14.env->GetArrayLength(v10);
			unsigned char* v13 = new unsigned char[len];
			jbyte* ba = v14.env->GetByteArrayElements(v10, 0);
			memcpy(v13, ba, len);
			v14.env->ReleaseByteArrayElements(v10, ba, 0);
			v14.forceDetach();
			return AssetFile{v13, len};
		}
	}
	return AssetFile(0, -1);
}
int32_t AppPlatform_android::getScreenWidth(void) {
	return this->screenWidth;
}
int32_t AppPlatform_android::getScreenHeight(void) {
	return this->screenHeight;
}
float AppPlatform_android::getPixelsPerMillimeter(void){
	if ( this->initialized && this->_jniGetPixelsPerMillimeter )
	{
		JVMAttacher v6(this->jvm);
		float v2 = v6.env->CallFloatMethod(this->mainActivityRef, this->_jniGetPixelsPerMillimeter);
		v6.forceDetach();
		return v2;
	}
	return 10;
}
bool_t AppPlatform_android::isNetworkEnabled(bool_t a2){
	if(!this->initialized) return 0;
	if(!this->_jniIsNetworkEnabled) return 0;

	JVMAttacher v6(this->jvm);
	bool v4 = v6.env->CallBooleanMethod(this->mainActivityRef, this->_jniIsNetworkEnabled, a2) != 0;
	v6.forceDetach();
	return v4;
}
void AppPlatform_android::openLoginWindow(void){
	if ( this->initialized )
	{
		if ( this->_jniIsNetworkEnabled )
		{
			JVMAttacher v2(this->jvm);
			v2.env->CallVoidMethod(this->mainActivityRef, this->_jniOpenLoginWindow);
			v2.forceDetach();
		}
	}
}
bool_t AppPlatform_android::isPowerVR(void){
	if(this->_jniSetIsPowerVR){
		if(this->initialized){
			if(!this->field_108){
				JVMAttacher v5(this->jvm);
				this->field_108 = 1;
				v5.env->CallVoidMethod(this->mainActivityRef, this->_jniSetIsPowerVR, 0);
				v5.forceDetach();
			}
		}
	}
	return 0;
}
int32_t AppPlatform_android::getKeyFromKeyCode(int32_t a2, int32_t a3, int32_t a4){
	JVMAttacher v10(this->jvm);
	int v8 = v10.env->CallIntMethod(this->mainActivityRef, this->_jniGetKeyFromCode, a2, a3, a4);
	v10.forceDetach();
	return v8;
}
void AppPlatform_android::buyGame(void){
	if ( this->initialized )
	{
		if ( this->_jniBuyGame )
		{
			JVMAttacher v2(this->jvm);
			v2.env->CallVoidMethod(this->mainActivityRef, this->_jniBuyGame);
			v2.forceDetach();
		}
	}
}
void AppPlatform_android::finish(void){
	if ( this->initialized )
	{
		if ( this->_jniQuit )
		{
			JVMAttacher v3(this->jvm);
			v3.env->CallVoidMethod(this->mainActivityRef, this->_jniQuit);
			v3.env->DeleteGlobalRef(this->mainActivityReference);
			v3.env->DeleteGlobalRef(this->windowReference);
			v3.env->DeleteGlobalRef(this->contextReference);
			v3.env->DeleteGlobalRef(this->viewReference);
			v3.env->DeleteGlobalRef(this->inputMethodHandlerReference);
			v3.forceDetach();
		}
	}
}
bool AppPlatform_android::supportsTouchscreen(void){
	if ( !this->initialized || !this->_jniSupportsNonTouchscreen )
	{
		return 1;
	}
	JVMAttacher v4(this->jvm);
	bool v2 = v4.env->CallBooleanMethod(this->mainActivityRef, this->_jniSupportsNonTouchscreen) == 0;
	v4.forceDetach();
	return v2;
}
bool_t AppPlatform_android::supportsVibration(void){
	return 1;
}
void AppPlatform_android::vibrate(int32_t a2){
	if(this->initialized){
		if(this->_jniWebRequest){
			JVMAttacher v17(this->jvm);
			v17.env->CallVoidMethod(this->mainActivityRef, this->_jniVibrate, a2);
			v17.forceDetach();
		}
	}
}
std::string AppPlatform_android::getPlatformStringVar(int32_t a2){
	if ( this->initialized && this->_jniGetPlatformStringVar )
	{
		JVMAttacher v11(this->jvm);
		std::string v10;
		jstring v8 = (jstring) v11.env->CallObjectMethod(this->mainActivityRef, this->_jniGetPlatformStringVar, a2);
		const char* utfChars =  v11.env->GetStringUTFChars(v8, 0);
		if ( utfChars )
		{
			v10 = utfChars;
		}
		v11.env->ReleaseStringUTFChars(v8, utfChars);
		v11.forceDetach();
		return v10;
	}else{
		return "";
	}
}
void AppPlatform_android::showKeyboard(std::string* a2, int32_t a3, bool_t a4){
	JVMAttacher v13(this->jvm);
	if ( this->_jniShowKeyboard )
	{
		if ( this->initialized )
		{
			v13.env->CallVoidMethod(this->mainActivityRef, this->_jniShowKeyboard, v13.env->NewStringUTF(a2->c_str()), a3, a4);
			{
				std::string v12; //XXX why is this here~?
				this->keyboardShown = 1;
				//~v12
			}
		}
	}
	v13.forceDetach();
}
void AppPlatform_android::hideKeyboard(void){
	JVMAttacher v2(this->jvm);
	if ( this->_jniHideKeyboard )
	{
		if ( this->initialized )
		{
			v2.env->CallVoidMethod(this->mainActivityRef, this->_jniHideKeyboard);
			this->keyboardShown = 0;
		}
	}
	v2.forceDetach();
}
void AppPlatform_android::updateTextBoxText(const std::string& a2){
	JVMAttacher v8(this->jvm);
	if ( this->_jniUpdateTextboxText )
	{
		if ( this->initialized )
		{
			v8.env->CallVoidMethod(this->mainActivityRef, this->_jniUpdateTextboxText, v8.env->NewStringUTF(a2.c_str()));
			this->keyboardShown = 0;
		}
	}
	v8.forceDetach();
}
LoginInformation AppPlatform_android::getLoginInformation(void){
	printf("AppPlatform_android::getLoginInformation - not implemented\n"); //TODO
	return LoginInformation();
}
void AppPlatform_android::setLoginInformation(const LoginInformation& a2){
	if(this->initialized){
		if(this->_jniSetLoginInformation){
			JVMAttacher v11(this->jvm);
			v11.env->CallVoidMethod(this->mainActivityRef, this->_jniSetLoginInformation, v11.env->NewStringUTF(a2.accessToken.c_str()), v11.env->NewStringUTF(a2.clientId.c_str()), v11.env->NewStringUTF(a2.profileId.c_str()), v11.env->NewStringUTF(a2.profileName.c_str()));
			v11.forceDetach();
		}
	}
}
void AppPlatform_android::clearSessionIDAndRefreshToken(void){
	if ( this->initialized )
	{
		if ( this->_jniClearLoginInformation )
		{
			JVMAttacher v2(this->jvm);
			v2.env->CallVoidMethod(this->mainActivityRef, this->_jniClearLoginInformation);
			v2.forceDetach();
		}
	}
}
void AppPlatform_android::statsTrackData(const std::string& a2, const std::string& a3){
	if ( this->initialized )
	{
		if ( this->_jniStatsTrackEvent )
		{
			JVMAttacher v11(this->jvm);
			v11.env->CallVoidMethod(this->mainActivityRef, this->_jniStatsTrackEvent, v11.env->NewStringUTF(a2.c_str()), v11.env->NewStringUTF(a3.c_str()));
			v11.forceDetach();
		}
	}
}
void AppPlatform_android::updateStatsUserData(const std::string& a2, const std::string& a3){
	if ( this->initialized )
	{
		if ( this->_jniStatsUpdateUserData )
		{
			JVMAttacher v11(this->jvm);
			v11.env->CallVoidMethod(this->mainActivityRef, this->_jniStatsUpdateUserData, v11.env->NewStringUTF(a2.c_str()), v11.env->NewStringUTF(a3.c_str()));
			v11.forceDetach();
		}
	}
}
void AppPlatform_android::initWithActivity(ANativeActivity* a2){
	this->field_10C = a2;
}
void AppPlatform_android::webRequest(int a2, long long a3, const std::string& a4, const std::string& a5, const std::string& a6, const std::string& a7){
	if(this->initialized){
		if(this->_jniWebRequest){
			JVMAttacher v17(this->jvm);
			v17.env->CallVoidMethod(this->mainActivityRef, this->_jniWebRequest, a2, a3, v17.env->NewStringUTF(a4.c_str()), v17.env->NewStringUTF(a5.c_str()), v17.env->NewStringUTF(a6.c_str()), v17.env->NewStringUTF(a7.c_str()));
			v17.forceDetach();
		}
	}
}
int AppPlatform_android::getWebRequestStatus(int a2){
	if(!this->initialized) return -99;
	if(!this->_jniGetWebRequestStatus) return -98;
	JVMAttacher v6(this->jvm);
	int v4 = v6.env->CallIntMethod(this->mainActivityRef, this->_jniGetWebRequestStatus, a2);
	v6.forceDetach();
	return v4;
}
std::string AppPlatform_android::getWebRequestContent(int a2){
	if ( this->initialized && this->_jniGetWebRequestContent )
	{
		JVMAttacher v9(this->jvm);
		jstring v6 = (jstring) v9.env->CallObjectMethod(this->mainActivityRef, this->_jniGetWebRequestContent);
		const char* utfChars = v9.env->GetStringUTFChars(v6, 0);
		std::string res = utfChars;
		v9.forceDetach(); //XXX is this thing in destructor?
		return res;
	}else{
		return "";
	}
}
int AppPlatform_android::abortWebRequest(int a2){
	if (!this->initialized ){
		return -99;
	}
	if (!this->_jniAbortWebRequest ){
		return -98;
	}

	JVMAttacher v6(this->jvm);
	int v4 = v6.env->CallIntMethod(this->mainActivityRef, this->_jniAbortWebRequest, a2);
	v6.forceDetach();
	return v4;
}
#endif
