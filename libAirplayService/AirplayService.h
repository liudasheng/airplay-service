#ifndef AIRPLAYSERVICE_H
#define AIRPLAYSERVICE_H

#include <utils/threads.h>
#include <utils/SortedVector.h>

#include <signal.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include "IAirplayService.h"
#include "IAirplay.h"


namespace android {

typedef void (*notify_callback_f)(int msg, int ext1, int ext2);

class AirplayService : public BnAirplayService
{
    class Client;
public:
    static AirplayService* instantiate();

    // IAirplayService interface
    virtual sp<IAirplay> connect(const sp<IAirplayClient>& airplayClient);
    void removeClient(wp<Client> client);
private:
    
    // ----------------------------------------------------------------------------
    class Client : public BnAirplay
    {
    public:
        virtual void disconnect();
        virtual int Start(); 
        virtual int Stop();
        virtual int SetHostName(const char *apname);
        virtual int GetHostName(char *apname);
        
        const sp<IAirplayClient>& getAirplayClient() const
        {
            return m_AirplayClient;
        };
        virtual void notify(int msg, int ext1, int ext2);        
        static Mutex m_FuncLock;
    private:
        static int notifyhandle(int msg, int ext1, int ext2);
        virtual void RegisterNotifyFn();

        /*
        notify_callback_f mNotify;
        Mutex  mNotifyLock;
        virtual void setNotifyCallback(notify_callback_f notifyFunc) {
            Mutex::Autolock autoLock(mNotifyLock);
            mNotify = notifyFunc;
        }
        
        void sendEvent(int msg, int ext1=0, int ext2=0) {
            Mutex::Autolock autoLock(mNotifyLock);
            if (mNotify) mNotify(msg, ext1, ext2);
        }*/
        
        friend class AirplayService;
        Client(const sp<AirplayService>& aiplayService,
               const sp<IAirplayClient>& airplayClient,
               pid_t clientPid);
        ~Client();

        mutable Mutex m_Lock;
        sp<AirplayService> m_AirplayService;
        sp<IAirplayClient> m_AirplayClient;
        pid_t m_ClientPid;
        int32_t m_ConnId;      
    };

    AirplayService();
    ~AirplayService();

    mutable Mutex m_Lock;
    //wp<Client> m_Client;
    SortedVector< wp<Client> > m_Clients;

    volatile int32_t m_Users;
    virtual int32_t incUsers();
    virtual void decUsers();

};



}; // namespace android

#endif //AIRPLAYSERVICE_H
//---------------------------------------------------------------------

