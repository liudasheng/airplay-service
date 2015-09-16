
#ifndef IAIRPLAYCLIENT_H
#define IAIRPLAYCLIENT_H

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>  //for data transact
#include <binder/IMemory.h>

namespace android {


class IAirplayClient : public IInterface
{
public:
    enum {
        NOTIFY_CALLBACK = IBinder::FIRST_CALL_TRANSACTION,
    };
    DECLARE_META_INTERFACE(AirplayClient);//must need
    virtual void notify(int msg, int ext1, int ext2) = 0;   
};

class BnAirplayClient: public BnInterface<IAirplayClient>
{
public:
    virtual status_t onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};



}; //namespace
#endif //IAIRPLAYCLIENT_H

