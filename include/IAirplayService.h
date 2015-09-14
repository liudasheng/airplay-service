
#ifndef IAIRPLAYSERVICE_H
#define IAIRPLAYSERVICE_H

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>  //for data transact
#include <binder/IMemory.h>

namespace android {


class IAirplayService : public IInterface
{
public:
    enum {
        START_AIRPLAY = IBinder::FIRST_CALL_TRANSACTION,
        STOP_AIRPLAY,
        SET_APNAME,
        GET_APNAME,
    };
    DECLARE_META_INTERFACE(AirplayService);//must need
    virtual int StartAirplayService() = 0; 
    virtual int StopAirplayService() = 0;
    virtual int SetAirplayHostName(const char *apname) = 0;
    virtual int GetAirplayHostName(char *apname) = 0;     
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

