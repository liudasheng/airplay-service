#ifndef AIRPLAY_H
#define AIRPLAY_H

#include "IAirplay.h"
#include "IAirplayClient.h"
#include "IAirplayService.h"


namespace android {

enum media_event_type {
    MEDIA_NOP               = 0, // interface test message
    MEDIA_PREPARED          = 1,
    MEDIA_PLAYBACK_COMPLETE = 2,
    MEDIA_BUFFERING_UPDATE  = 3,
    MEDIA_STARTED           = 4,
    MEDIA_PAUSED            = 5,
    MEDIA_STOPPED           = 6,
    MEDIA_ERROR             = 100,
    MEDIA_INFO              = 200,
};

// Generic error codes for the media player framework.  Errors are fatal, the
// playback must abort.
//
// Errors are communicated back to the client using the
// MediaPlayerListener::notify method defined below.
// In this situation, 'notify' is invoked with the following:
//   'msg' is set to MEDIA_ERROR.
//   'ext1' should be a value from the enum media_error_type.
//   'ext2' contains an implementation dependant error code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   1xx: Android Player errors. Something went wrong inside the MediaPlayer.
//   2xx: Media errors (e.g Codec not supported). There is a problem with the
//        media itself.
//   3xx: Runtime errors. Some extraordinary condition arose making the playback
//        impossible.
//
enum media_error_type {
    // 0xx
    MEDIA_ERROR_UNKNOWN = 1,
    // 1xx
    MEDIA_ERROR_SERVER_DIED = 100,
    // 2xx
    MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200,
    // 3xx
};


// Info and warning codes for the media player framework.  These are non fatal,
// the playback is going on but there might be some user visible issues.
//
// Info and warning messages are communicated back to the client using the
// MediaPlayerListener::notify method defined below.  In this situation,
// 'notify' is invoked with the following:
//   'msg' is set to MEDIA_INFO.
//   'ext1' should be a value from the enum media_info_type.
//   'ext2' contains an implementation dependant info code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   7xx: Android Player info/warning (e.g player lagging behind.)
//   8xx: Media info/warning (e.g media badly interleaved.)
//
enum media_info_type {
    // 0xx
    MEDIA_INFO_UNKNOWN = 1,
    // 6xx
    // client devices statues 
    MEDIA_INFO_CLIENT_CONNECTED = 600,
    MEDIA_INFO_CLIENT_DISCONNECTED = 601,
    // 7xx
    // MediaPlayer is temporarily pausing playback internally in order to
    // buffer more data.
    MEDIA_INFO_BUFFERING_START = 700,
    // MediaPlayer is resuming playback after filling buffers.
    MEDIA_INFO_BUFFERING_END = 701,
    // Bandwidth in recent past
    MEDIA_INFO_NETWORK_BANDWIDTH = 702,
    // 8xx
    // metadata info 
    // New media metadata is available.
    MEDIA_INFO_METADATA_UPDATE = 800,
    //9xx
    // volume
    MEDIA_INFO_VOLUME_CONFIG = 900,
};


class AirplayListener: virtual public RefBase
{
public:
    virtual void notify(int msg, int ext1, int ext2) = 0;
};


class Airplay : public BnAirplayClient, public IBinder::DeathRecipient
{
public:
    Airplay();
    ~Airplay();
    static sp<Airplay> connect();
    void disconnect();    
    virtual int Start(); 
    virtual int Stop(); 
    virtual int SetHostName(const char *apname);
    virtual int GetHostName(char *apname); 
    virtual int GetMetaData(MetaData_t *MetaData); 
    
    status_t setListener(const sp<AirplayListener>& listener);
    void  notify(int msg, int ext1, int ext2);
    status_t thread_start();
    status_t thread_stop();    

private:
    class EventSenderThread : public Thread
	{
		public:
			EventSenderThread(Airplay& airplay, bool bCanCallJava = false);
		private:
			virtual bool        threadLoop();
			virtual status_t    readyToRun() { return NO_ERROR; }
			virtual void        onFirstRef() {}
			Airplay&            mAirplay;
			Mutex               mLock;
	};

	sp<EventSenderThread>  mEventSenderThread;

	static const sp<IAirplayService>& getAirplayService();

	static Mutex mLock;
	static sp<IAirplayService> mAirplayService;
	sp<IAirplay> mAirplay;
    sp<AirplayListener> mListener;	
	Mutex                  mNotifyLock;
	status_t               mStatus;

	// ----------------------------------------------------------------------------

	virtual void binderDied(const wp<IBinder>& who);
	class DeathNotifier : public IBinder::DeathRecipient
	{
		public:
			~DeathNotifier();
			virtual void binderDied(const wp<IBinder>& who);
	};
	static sp<DeathNotifier> mDeathNotifier;    
};

}; // namespace android

#endif //AIRPLAY_H
//---------------------------------------------------------------------

