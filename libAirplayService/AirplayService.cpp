
#define LOG_TAG "AirplayService"
#define LOG_NDEBUG 0

#include <utils/Log.h>
#include <utils/String16.h>
#include <utils/Errors.h>

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>

#include "AirplayService.h"

#include "airplay.h"

#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

#define __GETCLIENTINIT(clients, x)        \
sp<Client> x;                   \
for(int kk=0; kk<(int)(clients.size()); kk++)    \
{\
    x = clients[kk].promote();

#define __GETCLIENTEND()        \
}


namespace android {

Mutex mLock;

static sp<AirplayService> pAirplayService = NULL;

void sendEvent(int msg, int ext1, int ext2)
{
    if(pAirplayService != NULL)
    {
         ALOGD("%s: msg=%d, ext1=%d, ext2=%d\n", __func__, msg, ext1, ext2);
         pAirplayService->notify(msg, ext1, ext2);
    }
    return;
}

sp<AirplayService> AirplayService::instantiate() 
{
	TRACE();
    pAirplayService = new AirplayService();
    ALOGV("AirplayService instantiate\n");
	defaultServiceManager()->addService(
			String16("AirplayService"), pAirplayService);
    return pAirplayService;
}

AirplayService::AirplayService()
{
	TRACE();
    ALOGI("AirplayService started: pid=%d", getpid());
    RegisterNotifyFn();
}

AirplayService::~AirplayService()
{
	TRACE();
}

int AirplayService::notifyhandle(int msg, int ext1, int ext2)
{
    ALOGD("%s: msg=%d, ext1=%d, ext2=%d\n", __func__, msg, ext1, ext2);

    sendEvent(msg, ext1, ext2);

    return 0;
}

void AirplayService::RegisterNotifyFn()
{
    TRACE();
    register_airplay_notify(notifyhandle);
}

void AirplayService::notify(int msg, int ext1, int ext2)
{
    ALOGD("notify: msg=%d, ext1=%d, ext2=%d\n", msg, ext1, ext2);
        
    __GETCLIENTINIT(m_Clients, mCurrentClient)
    mCurrentClient->m_AirplayClient->notify(msg, ext1, ext2);
    __GETCLIENTEND()

}


sp<IAirplay> AirplayService::connect(const sp<IAirplayClient>& airplayClient)
{
    pid_t callingPid = IPCThreadState::self()->getCallingPid();
    ALOGV("AirplayService::connect(callingPid %d, client %p)\n", callingPid, airplayClient->asBinder().get());

    sp<Client> sp_client = new Client(this, airplayClient, callingPid);
    wp<Client> wp_client = sp_client;
    Mutex::Autolock lock(m_Lock);
    m_Clients.add(wp_client);
    return sp_client;
}

void AirplayService::removeClient(wp<Client> client)
{
    ALOGV("removeClient(callingPid %d)\n", IPCThreadState::self()->getCallingPid());

    Mutex::Autolock lock(m_Lock);
    m_Clients.remove(client);
}

int AirplayService::incUsers()
{
    return android_atomic_inc(&m_Users);
}

void AirplayService::decUsers()
{
    android_atomic_dec(&m_Users);
}

AirplayService::Client::Client(const sp<AirplayService>& airplayService,
                                const sp<IAirplayClient>& airplayClient,
                                pid_t clientPid)
: m_AirplayService(airplayService), m_AirplayClient(airplayClient), m_ClientPid(clientPid)
{
    ALOGV("AirplayService::Client constructor(callingPid %d)\n", clientPid);
    m_ConnId = m_AirplayService->incUsers();
}

AirplayService::Client::~Client()
{
    ALOGV("AirplayService::Client destructor(callingPid %d)\n", m_ClientPid);
    disconnect();
}

void AirplayService::Client::disconnect()
{
    ALOGV("disconnect(callingPid %d, clientPid %d)\n", IPCThreadState::self()->getCallingPid(), m_ClientPid);
    wp<Client> client(this);
    m_AirplayService->removeClient(client);
    m_AirplayService->decUsers();
    m_AirplayClient.clear();
}

int AirplayService::Client::Start()
{	
	TRACE();
	return start_airplay();
}

int AirplayService::Client::Stop()
{	
	TRACE();
	return stop_airplay();
}

int AirplayService::Client::SetHostName(const char *apname)
{	
	TRACE();
	return set_airplay_hostname(apname);
}

int AirplayService::Client::GetHostName(char *apname)
{	
	TRACE();
	return get_airplay_hostname(apname);
}


};//namespace android
//---------------------------------------------------------------------

