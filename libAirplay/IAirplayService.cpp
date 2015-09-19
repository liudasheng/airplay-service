
#define LOG_TAG "IAirplayService"
//#define LOG_NDEBUG 0

#include <stdint.h>
#include <sys/types.h>

#include <utils/Log.h>

#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

#include "IAirplayService.h"

#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

namespace android {

class BpAirplayService: public BpInterface<IAirplayService>
{
public:
    BpAirplayService(const sp<IBinder>& impl)
        : BpInterface<IAirplayService>(impl)
    {
    	TRACE();
    }

    sp<IAirplay> connect(const sp<IAirplayClient>& AirplayClient)
    {
        TRACE();
        ALOGV("Send CONNECT\n");
        Parcel data, reply;

        data.writeInterfaceToken(IAirplayService::getInterfaceDescriptor());
        data.writeStrongBinder(AirplayClient->asBinder());
        remote()->transact(IAirplayService::CONNECT, data, &reply);
        return interface_cast<IAirplay>(reply.readStrongBinder());
    }   
		
};
IMPLEMENT_META_INTERFACE(AirplayService, "android.IAirplayService");

status_t BnAirplayService::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	TRACE();
	switch(code) {
        case CONNECT:{
            ALOGV("Receive CONNECT\n");
            CHECK_INTERFACE(IAirplayService, data, reply);
            sp<IAirplayClient> AirplayClient = interface_cast<IAirplayClient>(data.readStrongBinder());
    
            sp<IAirplay> Airplay = connect(AirplayClient);
            reply->writeStrongBinder(Airplay->asBinder());
            return NO_ERROR;
        } break;

	default:
		return BBinder::onTransact(code, data, reply, flags);
	}
}


// ----------------------------------------------------------------------------
}; // namespace android

