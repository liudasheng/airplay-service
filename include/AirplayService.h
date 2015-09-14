#ifndef AIRPLAYSERVICE_H
#define AIRPLAYSERVICE_H

#include "IAirplayService.h"

namespace android {

class AirplayService : public BnAirplayService
{
public:
    AirplayService();
    virtual ~AirplayService();       
    static void instantiate();
    virtual int StartAirplayService(); 
    virtual int StopAirplayService(); 
    virtual int SetAirplayHostName(const char *apname);
    virtual int GetAirplayHostName(char *apname); 
       
};



}; // namespace android

#endif //AIRPLAYSERVICE_H
//---------------------------------------------------------------------

