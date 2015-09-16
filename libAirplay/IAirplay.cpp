
#define LOG_TAG "IAirplay"
#define LOG_NDEBUG 0

#include <stdint.h>
#include <sys/types.h>

#include <utils/Log.h>

#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

#include "IAirplay.h"

#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

namespace android {

class BpAirplay: public BpInterface<IAirplay>
{
public:
    BpAirplay(const sp<IBinder>& impl)
        : BpInterface<IAirplay>(impl)
    {
    	TRACE();
    }

    void disconnect()
    {
        printf("Send DISCONNECT\n");
        Parcel data, reply;
        data.writeInterfaceToken(IAirplay::getInterfaceDescriptor());
        remote()->transact(IAirplay::DISCONNECT, data, &reply);
    }		
	virtual int Start() 
	{
		TRACE();
		Parcel data, reply;
		data.writeInterfaceToken(IAirplay::getInterfaceDescriptor()); 
		remote()->transact(IAirplay::START, data, &reply);
		return (reply.readInt32());
	}

	virtual int Stop() 
	{
		TRACE();
		Parcel data, reply;
		data.writeInterfaceToken(IAirplay::getInterfaceDescriptor()); 
		remote()->transact(IAirplay::STOP, data, &reply);
		return (reply.readInt32());
	}

	virtual int SetHostName(const char *apname) 
	{
		TRACE();
		Parcel data, reply;
		data.writeInterfaceToken(IAirplay::getInterfaceDescriptor()); 
        data.writeCString(apname);
		remote()->transact(IAirplay::SET_HOSTNAME, data, &reply);
		return (reply.readInt32());
	}

	virtual int GetHostName(char *apname) 
	{
		TRACE();
		Parcel data, reply;
		data.writeInterfaceToken(IAirplay::getInterfaceDescriptor()); 
		remote()->transact(IAirplay::GET_HOSTNAME, data, &reply);
        reply.read(apname, 128);
		return (reply.readInt32());
	}	    

};
IMPLEMENT_META_INTERFACE(Airplay, "android.IAirplay");

status_t BnAirplay::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	TRACE();
	switch(code) {
    case DISCONNECT:{
            ALOGV("Receive DISCONNECT\n");
            CHECK_INTERFACE(IAirplay, data, reply);
            disconnect();
            return NO_ERROR;
        } break;        
	case START: {
		CHECK_INTERFACE(IAirplay, data, reply);
		reply->writeInt32(Start());
		return NO_ERROR;
		} break;
	case STOP: {
		CHECK_INTERFACE(IAirplay, data, reply);
		reply->writeInt32(Stop());
		return NO_ERROR;
		} break;

	case SET_HOSTNAME: {
		CHECK_INTERFACE(IAirplay, data, reply);
		reply->writeInt32(SetHostName(data.readCString()));
		return NO_ERROR;
		} break;

	case GET_HOSTNAME: {
		CHECK_INTERFACE(IAirplay, data, reply);
        char apname[128]={0};
        int ret = GetHostName(apname);
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

