
#define LOG_TAG "airplayClient"
#define LOG_NDEBUG 0

#include <sys/types.h>
#include <unistd.h>

#include <utils/Log.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include "AirplayService.h"

#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

using namespace android;

Mutex mLock;
sp<IAirplayService> getTestService() {
	TRACE();
	Mutex::Autolock _l(mLock);
	sp<IAirplayService> pTestService;
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

	pTestService = interface_cast<IAirplayService>(binder);
	TRACE();
	ALOGE_IF(pTestService==0, "no AirplayService!?");
	return pTestService;
}

int main(int argc, char** argv)
{
	sp<IAirplayService> pTestService = getTestService();
	printf("[client] running pid=%d,tid=%d\n",getpid(),gettid());
	
    char apname[128]={0};
    memset(apname, 0, 128);
    pTestService->GetAirplayHostName(apname);
    printf("get airplay host name:%s\n", apname);

    printf("set airplay host name:%s\n", "airplay_o2");
    pTestService->SetAirplayHostName("airplay_o2");

    memset(apname, 0, 128);
    pTestService->GetAirplayHostName(apname);
    printf("get airplay host name:%s\n", apname);   

    printf("start airplay service..\n");
    pTestService->StartAirplayService();

    sleep(30);

    printf("stop airplay service\n");
    pTestService->StopAirplayService();
    
    
}


