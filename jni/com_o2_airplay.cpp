#define LOG_NDEBUG 0
#define LOG_TAG "O2Airplay"

#include "utils/Log.h"

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <utils/threads.h>
#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "utils/Errors.h"  // for status_t
#include "utils/KeyedVector.h"
#include "utils/String8.h"
#include <binder/Parcel.h>

#include <utils/Log.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <locale.h>

#include "AirplayService.h"

#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

using namespace android;

sp<IAirplayService> pAirplayService;
Mutex mLock;

sp<IAirplayService> getAirplayService() {
	TRACE();
	Mutex::Autolock _l(mLock);
	sp<IAirplayService> pAirplay;
	TRACE();
	sp<IServiceManager> sm = defaultServiceManager();
	sp<IBinder> binder;
	do {
		binder = sm->getService(String16("AirplayService"));
		TRACE();
		if (binder != 0)
			break;
		ALOGW("AirplayService not published, waiting...");
		usleep(500000); // 0.5 s
	} while(true);

	pAirplay = interface_cast<IAirplayService>(binder);
	TRACE();
	ALOGE_IF(pAirplay==0, "no AirplayService!?");
	return pAirplay;
}


static jstring stringToJstring(JNIEnv* env, const char* pat){
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
}

static char* jstringTostring(JNIEnv* env, jstring jstr)
{
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0)
    {
        rtn = (char*)malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}


JNIEXPORT jint JNICALL com_o2_airplay_StartAirplayService
(JNIEnv *env, jobject thiz)
{
    TRACE();
    int ret = pAirplayService->StartAirplayService();
    return ret;
}

JNIEXPORT jint JNICALL com_o2_airplay_StopAirplayService
(JNIEnv *env, jobject thiz)
{
    TRACE();
    int ret = pAirplayService->StopAirplayService();
    return ret;
}

JNIEXPORT jint JNICALL com_o2_airplay_SetAirplayHostName
(JNIEnv *env, jobject thiz, jobject data)
{
    TRACE();

	jclass datacls = env->GetObjectClass(data);
	jfieldID idHostName = env->GetFieldID(datacls, "HostName", "Ljava/lang/String;");
	jstring HostName = (jstring)env->GetObjectField(data, idHostName);
    
    char *name = jstringTostring(env, HostName);
    int ret = pAirplayService->SetAirplayHostName(name);
    
    return ret;
}

JNIEXPORT jint JNICALL com_o2_airplay_GetAirplayHostName
(JNIEnv *env, jobject thiz, jobject data)
{
    TRACE();
    char name[128] = {0};
    int ret = pAirplayService->GetAirplayHostName(name);

	jclass datacls = env->GetObjectClass(data);
	jfieldID idHostName = env->GetFieldID(datacls, "HostName", "Ljava/lang/String;");
	env->SetObjectField(data, idHostName, stringToJstring(env,name));
    
    return ret;
}

// ----------------------------------------------------------------------------
//the native method need to be registered
static JNINativeMethod gMethods[] = {
{"StartAirplayService",   "()I",                             (void *)com_o2_airplay_StartAirplayService},
{"StopAirplayService",    "()I",	                         (void *)com_o2_airplay_StopAirplayService},
{"SetAirplayHostName",    "(Lcom/o2/airplay/AirplayData;)I", (void *)com_o2_airplay_SetAirplayHostName},
{"GetAirplayHostName",    "(Lcom/o2/airplay/AirplayData;)I", (void *)com_o2_airplay_GetAirplayHostName},

};


// This function only registers the native methods
int register_com_o2_airplay(JNIEnv *env)
{
    pAirplayService = getAirplayService();
    
    ALOGV("register_com_o2_airplay was called");
    return AndroidRuntime::registerNativeMethods(env,
        "com/o2/airplay/Airplay", gMethods, NELEM(gMethods));
}
