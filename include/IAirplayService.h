
#ifndef IAIRPLAYSERVICE_H
#define IAIRPLAYSERVICE_H

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>  //for data transact
#include <binder/IMemory.h>

#include "IAirplayClient.h"
#include "IAirplay.h"

namespace android {


class IAirplayService : public IInterface
{
public:
    enum {
        CONNECT = IBinder::FIRST_CALL_TRANSACTION,
    };
    DECLARE_META_INTERFACE(AirplayService);//must need
    virtual sp<IAirplay> connect(const sp<IAirplayClient>& AirplayClient) = 0;
};

class BnAirplayService: public BnInterface<IAirplayService>
{
public:
    virtual status_t onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};



}; //namespace
#endif //IAIRPLAYSERVICE_H

