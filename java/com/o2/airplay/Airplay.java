package com.o2.airplay;

public class Airplay {
	
	public int StartAirplay(){
		return StartAirplayService();
	}

	public int StopAirplay(){
		return StopAirplayService();
	}

	public int SetAirplayName(AirplayData data){
		return SetAirplayHostName(data);
	}

	public int GetAirplayName(AirplayData data){
		return GetAirplayHostName(data);
	}

	private native int StartAirplayService();
	private native int StopAirplayService();
	private native int SetAirplayHostName(AirplayData data);
	private native int GetAirplayHostName(AirplayData data);
	
    static {
        try{
            System.loadLibrary("airplayjni");
        }catch(UnsatisfiedLinkError ule) {
            System.err.println("WARNING: Could not load library!");
        }
    }	
}

