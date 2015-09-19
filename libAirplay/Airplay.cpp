
#define LOG_TAG "Airplay"
#define LOG_NDEBUG 0

#include <stdint.h>
#include <sys/types.h>

#include <utils/Log.h>

#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

#include "Airplay.h"

#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

namespace android {

Mutex Airplay::mLock;
sp<IAirplayService> Airplay::mAirplayService;
sp<Airplay::DeathNotifier> Airplay::mDeathNotifier;

const sp<IAirplayService>& Airplay::getAirplayService()
{
    TRACE();
    Mutex::Autolock _l(Airplay::mLock);
    if(Airplay::mAirplayService == NULL)
    {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
        int retry = 0;

        do
        {
            binder = sm->getService(String16("AirplayService"));
            if((binder != NULL) || (retry >= 10))
            {
                break;
            }
            ALOGW("AirplayService not published, waiting...\n");
            usleep(500000); // 0.5 s
            retry++;
        } while(true);

        if(binder != NULL)
        {
            if(Airplay::mDeathNotifier == NULL)
            {
                Airplay::mDeathNotifier = new DeathNotifier();
            }
            binder->linkToDeath(Airplay::mDeathNotifier);
            Airplay::mAirplayService = interface_cast<IAirplayService>(binder);
        }
        if(Airplay::mAirplayService == NULL)
            ALOGE("Can't get AirplayService service!\n");
    }

    return Airplay::mAirplayService;
}

Airplay::Airplay(){
    TRACE();
    ALOGV("constructor\n\n");
    mListener = NULL;

    mEventSenderThread = new EventSenderThread(*this, true);
    if (mEventSenderThread == 0) {
        mStatus = NO_INIT;
    }
    else {
        mStatus = NO_ERROR;
    }
}

Airplay::~Airplay()
{
    TRACE();
    ALOGV("destructor\n\n");
    if (mStatus == NO_ERROR) {
        // Make sure that callback function exits in the case where
        // it is looping on buffer empty condition in obtainBuffer().
        // Otherwise the callback thread will never exit.
        thread_stop();
        if (mEventSenderThread != 0) {
            mEventSenderThread->requestExitAndWait();
            mEventSenderThread.clear();
        }
    }
    disconnect();
    IPCThreadState::self()->flushCommands();
}

sp<Airplay> Airplay::connect()
{
    TRACE();

    sp<Airplay> airplay = new Airplay();
    const sp<IAirplayService>& service = getAirplayService();
    if(service != NULL)
    {
        airplay->mAirplay = service->connect(airplay);
    }

    if(airplay->mAirplay != NULL)
    {
        airplay->mAirplay->asBinder()->linkToDeath(airplay);
    }
    else
    {
        airplay.clear();
    }

    return airplay;

}

void Airplay::disconnect()
{
    TRACE();
    if(mAirplay == NULL)
    {
        return;
    }

    mAirplay->disconnect();
    mAirplay->asBinder()->unlinkToDeath(this);
    mAirplay.clear();
}

int Airplay::Start()
{
    TRACE();
    if(mAirplay == NULL)
    {
        return -1;
    }

    return mAirplay->Start();
}

int Airplay::Stop()
{
    TRACE();
    if(mAirplay == NULL)
    {
        return -1;
    }

    return mAirplay->Stop();
}

int Airplay::SetHostName(const char *apname)
{
    TRACE();
    if(mAirplay == NULL)
    {
        return -1;
    }

    if(apname == NULL)
    {
        ALOGE("apname is null!");
        return -1;
    }

    return mAirplay->SetHostName(apname);
}

int Airplay::GetHostName(char *apname)
{
    TRACE();
    if(mAirplay == NULL)
    {
        return -1;
    }

    if(apname == NULL)
    {
        ALOGE("apname is null!");
        return -1;
    }

    return mAirplay->GetHostName(apname);
}

int Airplay::GetMetaData(MetaData_t *MetaData)
{
    TRACE();
    if(mAirplay == NULL)
    {
        return -1;
    }

    return mAirplay->GetMetaData(MetaData);
}


status_t Airplay::setListener(const sp<AirplayListener>& listener)
{
    TRACE();
    Mutex::Autolock _l(Airplay::mLock);
    mListener = listener;
    return NO_ERROR;
}

status_t Airplay::thread_start()
{
    TRACE();
    sp<EventSenderThread> t = mEventSenderThread;
    if (t != 0) {
        t->run("EventSenderThread", ANDROID_PRIORITY_NORMAL);
    }
    return NO_ERROR;
}

status_t Airplay::thread_stop()
{
    TRACE();
    sp<EventSenderThread> t = mEventSenderThread;
    if (t != 0) {
        t->requestExit();
    }
    return NO_ERROR;
}

Airplay::EventSenderThread::EventSenderThread(Airplay& airplay, bool bCanCallJava)
    : Thread(bCanCallJava), mAirplay(airplay)
{
}

bool Airplay::EventSenderThread::threadLoop()
{
    static int i = 0;
    mAirplay.notify(MEDIA_INFO, i++, 0);
    sleep(1);
    return true;
}

void Airplay::notify(int msg, int ext1, int ext2)
{
    ALOGV("message received msg=%d, ext1=%d, ext2=%d", msg, ext1, ext2);

    switch (msg) {
        case MEDIA_PREPARED:
            ALOGW("prepare (%d, %d)", ext1, ext2);
            break;
        case MEDIA_STARTED:
            ALOGW("start (%d, %d)", ext1, ext2);
            break;
        case MEDIA_PAUSED:
            ALOGW("pause (%d, %d)", ext1, ext2);
            break;
        case MEDIA_STOPPED:
            ALOGW("stop (%d, %d)", ext1, ext2);
            break;
        case MEDIA_ERROR:
            ALOGE("error (%d, %d)", ext1, ext2);
            break;
        case MEDIA_INFO:
            ALOGW("info/warning (%d, %d)", ext1, ext2);
            break;
        default:
            ALOGV("unrecognized message: (%d, %d, %d)", msg, ext1, ext2);
            break;
    }

    sp<AirplayListener> listener = mListener;
    if(listener != NULL)
    {
        Mutex::Autolock _l(Airplay::mLock);
        ALOGV("callback application");
        listener->notify(msg, ext1, ext2);
        ALOGV("back from callback");
    }
}

   

void Airplay::binderDied(const wp<IBinder>& who)
{
    ALOGV("IAirplay died!\n");
    mAirplay.clear();
}

Airplay::DeathNotifier::~DeathNotifier()
{
    ALOGV("DeathNotifier::~DeathNotifier");
    Mutex::Autolock _l(Airplay::mLock);
    if(Airplay::mAirplayService != NULL)
    {
        Airplay::mAirplayService->asBinder()->unlinkToDeath(this);
    }
}

void Airplay::DeathNotifier::binderDied(const wp<IBinder>& who)
{
    ALOGV("IAirplayService died!\n");
    Mutex::Autolock _l(Airplay::mLock);
    Airplay::mAirplayService.clear();
}


// ----------------------------------------------------------------------------
}; // namespace android

