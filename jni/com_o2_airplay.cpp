//#define LOG_NDEBUG 0
#define LOG_TAG "Airplay-JNI"

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
#include "android_runtime/Log.h"
#include "utils/Errors.h"  // for status_t
#include "utils/KeyedVector.h"
#include "utils/String8.h"
#include <binder/Parcel.h>

#include <utils/Log.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <locale.h>

#include "Airplay.h"

#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

using namespace android;


struct fields_t {
    jmethodID   post_event;
};
static fields_t fields;

static sp<Airplay> airplay = NULL;


// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class JNIAirplayListener: public AirplayListener
{
public:
    JNIAirplayListener(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~JNIAirplayListener();
    void notify(int msg, int ext1, int ext2);
private:
    JNIAirplayListener();
    jclass      mClass;     // Reference to airplay class
    jobject     mObject;    // Weak ref to airplay Java object to call on
};

JNIAirplayListener::JNIAirplayListener(JNIEnv* env, jobject thiz, jobject weak_thiz)
{

    // Hold onto the MediaPlayer class for use in calling the static method
    // that posts events to the application thread.
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        ALOGE("Can't find com/o2/Airplay");
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }
    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the MediaPlayer object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);
}

JNIAirplayListener::~JNIAirplayListener()
{
    // remove global references
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);
}


void JNIAirplayListener::notify(int msg, int ext1, int ext2)
{
    ALOGV("%s, msg=%d, ext1=%d, ext2=%d\n",__func__, msg, ext1, ext2);


    JNIEnv *env = AndroidRuntime::getJNIEnv();

    env->CallStaticVoidMethod(mClass, fields.post_event, mObject,
                              msg, ext1, ext2, NULL);

    if (env->ExceptionCheck()) {
        ALOGW("An exception occurred while notifying an event.");
        LOGW_EX(env);
        env->ExceptionClear();
    }
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

/*End by eric_wang. Notify hdmi status.*/

// This function gets some field IDs, which in turn causes class initialization.
// It is called from a static block in MediaPlayer, which won't run until the
// first time an instance of this class is used.
static void com_o2_airplay_native_init(JNIEnv *env)
{
    jclass clazz;

    clazz = env->FindClass("com/o2/airplay/Airplay");
    if (clazz == NULL) {
        return;
    }

    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                               "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (fields.post_event == NULL) {
        return;
    }
}

static void com_o2_airplay_native_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
    ALOGV("native_setup");
    airplay = Airplay::connect();
    if (airplay == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }

    // create new listener and give it to airplay
    sp<JNIAirplayListener> listener = new JNIAirplayListener(env, thiz, weak_this);
    airplay->setListener(listener);
}


JNIEXPORT jint JNICALL com_o2_airplay_Start
(JNIEnv *env, jobject thiz)
{
    TRACE();
    int ret = airplay->Start();
    return ret;
}

JNIEXPORT jint JNICALL com_o2_airplay_Stop
(JNIEnv *env, jobject thiz)
{
    TRACE();
    
    int ret = airplay->Stop();
    return ret;
}

JNIEXPORT jint JNICALL com_o2_airplay_SetHostName
(JNIEnv *env, jobject thiz, jobject data)
{
    TRACE();

	jclass datacls = env->GetObjectClass(data);
	jfieldID idHostName = env->GetFieldID(datacls, "HostName", "Ljava/lang/String;");
	jstring HostName = (jstring)env->GetObjectField(data, idHostName);
    
    char *name = jstringTostring(env, HostName);
    
    int ret = airplay->SetHostName(name);
    
    return ret;
}

JNIEXPORT jint JNICALL com_o2_airplay_GetHostName
(JNIEnv *env, jobject thiz, jobject data)
{
    TRACE();
    char name[128] = {0};
    int ret = airplay->GetHostName(name);

	jclass datacls = env->GetObjectClass(data);
	jfieldID idHostName = env->GetFieldID(datacls, "HostName", "Ljava/lang/String;");
	env->SetObjectField(data, idHostName, stringToJstring(env,name));
    
    return ret;
}

JNIEXPORT jint JNICALL com_o2_airplay_GetMetaData
(JNIEnv *env, jobject thiz, jobject data)
{
    TRACE();
    MetaData_t MetaData;
    memset(&MetaData, 0, sizeof(MetaData_t));
    int ret = airplay->GetMetaData(&MetaData);

    ALOGV("%s album: %s\n", __func__,MetaData.album);
    ALOGV("%s artist: %s\n", __func__,MetaData.artist);
    ALOGV("%s genre: %s\n", __func__,MetaData.genre);
    ALOGV("%s title: %s\n", __func__,MetaData.title);       

	jclass datacls = env->GetObjectClass(data);
	env->SetObjectField(data, env->GetFieldID(datacls, "MetaArtist", "Ljava/lang/String;"), stringToJstring(env,MetaData.artist));
    env->SetObjectField(data, env->GetFieldID(datacls, "MetaAlbum", "Ljava/lang/String;"), stringToJstring(env,MetaData.album));
    env->SetObjectField(data, env->GetFieldID(datacls, "MetaArtwork", "Ljava/lang/String;"), stringToJstring(env,MetaData.artwork));
    env->SetObjectField(data, env->GetFieldID(datacls, "MetaComment", "Ljava/lang/String;"), stringToJstring(env,MetaData.comment));
    env->SetObjectField(data, env->GetFieldID(datacls, "MetaGenre", "Ljava/lang/String;"), stringToJstring(env,MetaData.genre));
    env->SetObjectField(data, env->GetFieldID(datacls, "MetaTitle", "Ljava/lang/String;"), stringToJstring(env,MetaData.title));
    
    return ret;
}


// ----------------------------------------------------------------------------
//the native method need to be registered
static JNINativeMethod gMethods[] = {
{"native_init",      "()V",                             (void *)com_o2_airplay_native_init},
{"native_setup",     "(Ljava/lang/Object;)V",           (void *)com_o2_airplay_native_setup},
{"Start",            "()I",                             (void *)com_o2_airplay_Start},
{"Stop",             "()I",	                            (void *)com_o2_airplay_Stop},
{"SetHostName",      "(Lcom/o2/airplay/AirplayData;)I", (void *)com_o2_airplay_SetHostName},
{"GetHostName",      "(Lcom/o2/airplay/AirplayData;)I", (void *)com_o2_airplay_GetHostName},
{"GetMetadata",      "(Lcom/o2/airplay/AirplayData;)I", (void *)com_o2_airplay_GetMetaData},

};


// This function only registers the native methods
int register_com_o2_airplay(JNIEnv *env)
{
    
    ALOGV("register_com_o2_airplay was called");
    return AndroidRuntime::registerNativeMethods(env,
        "com/o2/airplay/Airplay", gMethods, NELEM(gMethods));
}
