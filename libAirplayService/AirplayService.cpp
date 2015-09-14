
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

#include "shairport_lib.h"


#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

namespace android {

void AirplayService::instantiate() {
	TRACE();
	defaultServiceManager()->addService(
			String16("AirplayService"), new AirplayService);
}

AirplayService::AirplayService()
{
	TRACE();
    ALOGI("AirplayService started: pid=%d", getpid());
}

AirplayService::~AirplayService()
{
	TRACE();
}

int AirplayService::StartAirplayService()
{	
	TRACE();
	return start_shairport();
}


int AirplayService::StopAirplayService()
{	
	TRACE();
	return stop_shairport();
}

int AirplayService::SetAirplayHostName(const char *apname)
{	
	TRACE();
	return set_shairport_hostname(apname);
}

int AirplayService::GetAirplayHostName(char *apname)
{	
	TRACE();
	return get_shairport_hostname(apname);
}


};//namespace android
//---------------------------------------------------------------------

