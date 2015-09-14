
#define LOG_TAG "AirplayServer"
//#define LOG_NDEBUG 0

#include <sys/prctl.h>
#include <sys/wait.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <cutils/properties.h>
#include <utils/Log.h>

#include "AirplayService.h"

using namespace android;

int main(int argc, char** argv)
{
	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm = defaultServiceManager();
	ALOGI("ServiceManager: %p", sm.get());

    AirplayService::instantiate();
	
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
}
//------------------------------------------------------------

