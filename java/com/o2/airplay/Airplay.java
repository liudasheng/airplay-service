package com.o2.airplay;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;
import java.lang.ref.WeakReference;


public class Airplay {
    private final static String TAG = "Airplay-JAVA";
	private EventHandler mEventHandler;

    public Airplay() {
        Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else {
            mEventHandler = null;
        }

        /* Native setup requires a weak reference to our object.
         * It's easier to create it here than in C++.
         */
        native_setup(new WeakReference<Airplay>(this));
    }	

    private static final int MEDIA_NOP = 0; // interface test message
    private static final int MEDIA_PREPARED = 1; //start airplay service
    private static final int MEDIA_PLAYBACK_COMPLETE = 2;
    private static final int MEDIA_BUFFERING_UPDATE = 3;
    private static final int MEDIA_STARTED = 4; //start playing
    private static final int MEDIA_PAUSED = 5;
    private static final int MEDIA_STOPPED = 6; //stop airplay service
    private static final int MEDIA_ERROR = 100;	// used
    private static final int MEDIA_INFO = 200;	// used

    private class EventHandler extends Handler
    {
        private Airplay mAirplay;

        public EventHandler(Airplay ap, Looper looper) {
            super(looper);
            mAirplay = ap;
        }

        @Override
        public void handleMessage(Message msg) {
        //Log.v(TAG, "handleMessage: " + msg.what +" " + msg.arg1 + " " + msg.arg2);
            switch(msg.what) {
            case MEDIA_PREPARED:
                if (mOnPreparedListener != null) {
                    mOnPreparedListener.onPrepared(mAirplay, msg.arg1, msg.arg2);
                }				
                return;
            case MEDIA_STOPPED:
				if (mOnStoppedListener != null){
					mOnStoppedListener.onStopped(mAirplay, msg.arg1, msg.arg2);
				}
                return;
            case MEDIA_STARTED:
				if (mOnStartedListener != null){
					mOnStartedListener.onStarted(mAirplay, msg.arg1, msg.arg2);
				}
				return;
            case MEDIA_PAUSED:
				if (mOnPausedListener != null){
					mOnPausedListener.onPaused(mAirplay, msg.arg1, msg.arg2);
				}
				return;
            case MEDIA_BUFFERING_UPDATE:
				if (mOnBufferUpdateListener!= null){
					mOnBufferUpdateListener.onBufferUpdate(mAirplay, msg.arg1, msg.arg2);
				}
				return;
            case MEDIA_ERROR:
                Log.e(TAG, "Error (" + msg.arg1 + "," + msg.arg2 + ")");
                boolean error_was_handled = false;
                if (mOnErrorListener != null) {
                    error_was_handled = mOnErrorListener.onError(mAirplay, msg.arg1, msg.arg2);
                }
                return;
            case MEDIA_INFO:
                if (mOnInfoListener != null) {
                    mOnInfoListener.onInfo(mAirplay, msg.arg1, msg.arg2);
                }
                // No real default action so far.
                return;
            case MEDIA_NOP: // interface test message - ignore
                break;
            default:
                Log.e(TAG, "Unknown message type " + msg.what);
                return;
            }
        }
    }

    private static void postEventFromNative(Object airplay_ref,
                                            int what, int arg1, int arg2, Object obj)
    {
        Airplay ap = (Airplay)((WeakReference)airplay_ref).get();
        if (ap == null) {
            return;
        }

        if (ap.mEventHandler != null) {
            Message m = ap.mEventHandler.obtainMessage(what, arg1, arg2, obj);
			//Log.v(TAG, "postEventFromNative: " + m.what +" " + m.arg1 + " " + m.arg2);
            ap.mEventHandler.sendMessage(m);
        }
    }

    public interface OnPreparedListener
    {
        boolean onPrepared(Airplay ap, int what, int extra);
    }
	
    public void setOnPreparedListener(OnPreparedListener listener)
    {
        mOnPreparedListener = listener;
    }
	private OnPreparedListener mOnPreparedListener;

    public interface OnBufferUpdateListener
    {
        boolean onBufferUpdate(Airplay ap, int what, int extra);
    }
	
    public void setOnBufferUpdateListener(OnBufferUpdateListener listener)
    {
        mOnBufferUpdateListener = listener;
    }
	private OnBufferUpdateListener mOnBufferUpdateListener;

    public interface OnStartedListener
    {
        boolean onStarted(Airplay ap, int what, int extra);
    }
	
    public void setOnStartedListener(OnStartedListener listener)
    {
        mOnStartedListener = listener;
    }
	private OnStartedListener mOnStartedListener;

    public interface OnPausedListener
    {
        boolean onPaused(Airplay ap, int what, int extra);
    }
	
    public void setOnPausededListener(OnPausedListener listener)
    {
        mOnPausedListener = listener;
    }
	private OnPausedListener mOnPausedListener;		

    public interface OnStoppedListener
    {
        boolean onStopped(Airplay ap, int what, int extra);
    }
	
    public void setOnStoppededListener(OnStoppedListener listener)
    {
        mOnStoppedListener = listener;
    }
	private OnStoppedListener mOnStoppedListener;		

    public static final int MEDIA_ERROR_UNKNOWN = 1;
    public static final int MEDIA_ERROR_SERVER_DIED = 100;
    public static final int MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200;

    public interface OnErrorListener
    {
        boolean onError(Airplay ap, int what, int extra);
    }
	
    public void setOnErrorListener(OnErrorListener listener)
    {
        mOnErrorListener = listener;
    }

    private OnErrorListener mOnErrorListener;


    public static final int MEDIA_INFO_UNKNOWN = 1;
	public static final int MEDIA_INFO_CLIENT_CONNECTED = 600; //client connected
	public static final int MEDIA_INFO_CLIENT_DISCONNECTED = 601; // client disconnected	
    public static final int MEDIA_INFO_BUFFERING_START = 700;
    public static final int MEDIA_INFO_BUFFERING_END = 701;
	public static final int MEDIA_INFO_NETWORK_BANDWIDTH = 702;
    public static final int MEDIA_INFO_METADATA_UPDATE = 800;
	public static final int	MEDIA_INFO_VOLUME_CONFIG = 900;

    public interface OnInfoListener
    {
        boolean onInfo(Airplay ap, int what, int extra);
    }

    public void setOnInfoListener(OnInfoListener listener)
    {
        mOnInfoListener = listener;
    }

    private OnInfoListener mOnInfoListener;	
	
	public int StartAirplay(){
		return Start();
	}

	public int StopAirplay(){
		return Stop();
	}

	public int SetAirplayName(AirplayData data){
		return SetHostName(data);
	}

	public int GetAirplayName(AirplayData data){
		return GetHostName(data);
	}

	public int GetMetaData(AirplayData data){
		return GetMetadata(data);
	}	

    private static native final void native_init();
    private native final void native_setup(Object airplay_this);
	private native int Start();
	private native int Stop();
	private native int SetHostName(AirplayData data);
	private native int GetHostName(AirplayData data);
	private native int GetMetadata(AirplayData data);
	
    static {
        try{
            System.loadLibrary("airplayjni");
        }catch(UnsatisfiedLinkError ule) {
            System.err.println("WARNING: Could not load library!");
        }
		native_init();
    }	
}

