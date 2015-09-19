
#ifndef IAIRPLAY_H
#define IAIRPLAY_H

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>  //for data transact
#include <binder/IPCThreadState.h>

#include <binder/IMemory.h>

#include "airplay.h"

namespace android {

class IAirplay : public IInterface
{
public:
    enum {
        DISCONNECT = IBinder::FIRST_CALL_TRANSACTION,
        START, 
        STOP,
        SET_HOSTNAME,
        GET_HOSTNAME,
        GET_METADATA,
    };
    DECLARE_META_INTERFACE(Airplay);//must need
    virtual void disconnect() = 0;
    virtual int Start() = 0; 
    virtual int Stop() = 0;
    virtual int SetHostName(const char *apname) = 0;
    virtual int GetHostName(char *apname) = 0;
    virtual int GetMetaData(MetaData_t *MetaData) = 0; 
};

class BnAirplay: public BnInterface<IAirplay>
{
public:
    virtual status_t onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};



}; //namespace
#endif //IAIRPLAY_H

