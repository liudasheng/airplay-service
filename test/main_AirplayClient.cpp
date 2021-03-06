
#define LOG_TAG "airplayClient"
#define LOG_NDEBUG 0

#include <sys/types.h>
#include <unistd.h>

#include <utils/Log.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include "Airplay.h"

#define TRACE() ALOGV("[%d] %s", __LINE__, __func__)

using namespace android;

static sp<Airplay> pAirplay = NULL;

class ClientListener : public AirplayListener
{
public:
    ClientListener(const sp<Airplay>& airplay)
       : mAirplay(airplay)
    {
    }
    virtual void notify(int msg, int ext1, int ext2);
private:
    sp<Airplay> mAirplay;
};

MetaData_t MetaData;

void ClientListener::notify(int msg, int ext1, int ext2)
{
    printf("notify: msg=%d, ext=%d, ext2=%d\n", msg, ext1, ext2);

    switch(msg)
    {
        case 200:
            switch(ext1)
            {
                case 800:
                    memset(&MetaData, 0, sizeof(MetaData_t));
                    pAirplay->GetMetaData(&MetaData);
                    printf("META album: %s\n", MetaData.album);
                    printf("META artist: %s\n", MetaData.artist);
                    printf("META genre: %s\n", MetaData.genre);
                    printf("META title: %s\n", MetaData.title);
                    break;
                default:
                    break;
            }

        default:
            break;
    }
    return ;       
}


int main(int argc, char** argv)
{
	pAirplay = Airplay::connect();
	printf("[client] running pid=%d,tid=%d\n",getpid(),gettid());

    if(pAirplay == NULL )
    {
        printf("can not connect to AirplayService!\n");
        return -1;
    }

    pAirplay->setListener(new ClientListener(pAirplay));
    
    ProcessState::self()->startThreadPool();
    
    char apname[128]={0};
    memset(apname, 0, 128);
    pAirplay->GetHostName(apname);
    printf("get airplay host name:%s\n", apname);

    printf("set airplay host name:%s\n", "airplay_o2");
    pAirplay->SetHostName("airplay_o2");

    memset(apname, 0, 128);
    pAirplay->GetHostName(apname);
    printf("get airplay host name:%s\n", apname);   

    printf("start airplay service..\n");
    pAirplay->Start();

    while(1);

    sleep(30);

    printf("stop airplay service\n");
    pAirplay->Stop();
    
    
}


