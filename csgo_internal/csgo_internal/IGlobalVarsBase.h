#pragma once

#include "sdk.h"

class IGlobalVarsBase
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float absoluteframestarttimestddev;
	float curtime;
	float frametime;
	int maxClients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
	int simTicksThisFrame;
	int network_protocol;
	void* pSaveData;
private:
	bool m_bClient;
public:
	bool m_bRemoteClient;
private:
	int nTimestampNetworkingBase;
	int nTimestampRandomizeWindow;
};
