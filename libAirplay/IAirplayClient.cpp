
#define LOG_TAG "IAirplayClient"
//#define LOG_NDEBUG 0

#include <stdint.h>
#include <sys/types.h>

#include <utils/Log.h>

#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

#include "IAirplayClient.h"

#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

namespace android {

class BpAirplayClient: public BpInterface<IAirplayClient>
{
public:
    BpAirplayClient(const sp<IBinder>& impl)
        : BpInterface<IAirplayClient>(impl)
    {
    	TRACE();
    }
		

	void notify(int msg, int ext1, int ext2)
	{
		TRACE();
		Parcel data, reply;
		data.writeInterfaceToken(IAirplayClient::getInterfaceDescriptor()); 
        data.writeInt32(msg);
        data.writeInt32(ext1);
        data.writeInt32(ext2);
		remote()->transact(IAirplayClient::NOTIFY_CALLBACK, data, &reply);
	}

    

};
IMPLEMENT_META_INTERFACE(AirplayClient, "android.IAirplayClient");

status_t BnAirplayClient::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	TRACE();
	switch(code) {
	case NOTIFY_CALLBACK: {
		CHECK_INTERFACE(IAirplayClient, data, reply);
		int msg = data.readInt32();
        int ext1 = data.readInt32();
        int ext2 = data.readInt32();
        notify(msg, ext1, ext2);
        return NO_ERROR;
		} break;
    
	default:
		return BBinder::onTransact(code, data, reply, flags);
	}
}


// ----------------------------------------------------------------------------
}; // namespace android

