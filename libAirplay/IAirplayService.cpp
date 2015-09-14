
#define LOG_TAG "IAirplayService"
#define LOG_NDEBUG 0

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
		
	virtual int StartAirplayService() 
	{
		TRACE();
		Parcel data, reply;
		data.writeInterfaceToken(IAirplayService::getInterfaceDescriptor()); 
		remote()->transact(IAirplayService::START_AIRPLAY, data, &reply);
		return (reply.readInt32());
	}

	virtual int StopAirplayService() 
	{
		TRACE();
		Parcel data, reply;
		data.writeInterfaceToken(IAirplayService::getInterfaceDescriptor()); 
		remote()->transact(IAirplayService::STOP_AIRPLAY, data, &reply);
		return (reply.readInt32());
	}

	virtual int SetAirplayHostName(const char *apname) 
	{
		TRACE();
		Parcel data, reply;
		data.writeInterfaceToken(IAirplayService::getInterfaceDescriptor()); 
        data.writeCString(apname);
		remote()->transact(IAirplayService::SET_APNAME, data, &reply);
		return (reply.readInt32());
	}

	virtual int GetAirplayHostName(char *apname) 
	{
		TRACE();
		Parcel data, reply;
		data.writeInterfaceToken(IAirplayService::getInterfaceDescriptor()); 
		remote()->transact(IAirplayService::GET_APNAME, data, &reply);
        reply.read(apname, 128);
		return (reply.readInt32());
	}	    

};
IMPLEMENT_META_INTERFACE(AirplayService, "android.IAirplayService");

status_t BnAirplayService::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	TRACE();
	switch(code) {
	case START_AIRPLAY: {
		CHECK_INTERFACE(IAirplayService, data, reply);
		reply->writeInt32(StartAirplayService());
		return NO_ERROR;
		} break;
	case STOP_AIRPLAY: {
		CHECK_INTERFACE(IAirplayService, data, reply);
		reply->writeInt32(StopAirplayService());
		return NO_ERROR;
		} break;

	case SET_APNAME: {
		CHECK_INTERFACE(IAirplayService, data, reply);
		reply->writeInt32(SetAirplayHostName(data.readCString()));
		return NO_ERROR;
		} break;

	case GET_APNAME: {
		CHECK_INTERFACE(IAirplayService, data, reply);
        char apname[128]={0};
        int ret = GetAirplayHostName(apname);
        reply->write(apname, 128);
		reply->writeInt32(ret);
		return NO_ERROR;
		} break;
    
	default:
		return BBinder::onTransact(code, data, reply, flags);
	}
}


// ----------------------------------------------------------------------------
}; // namespace android

