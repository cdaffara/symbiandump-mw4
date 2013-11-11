/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  IKEv2 SA data definition               
*
*/

#ifndef _IKEV2SA_H_
#define _IKEV2SA_H_

#include "ikev2SAdata.h"
#include "ikev2keepalive.h"

class CIkev2PluginSession;
class TIkeV2IpsecSAData;
class MIkeDebug;

//
//  Class CIkev2SA
//
NONSHARABLE_CLASS(CIkev2SA) : public CTimer, public MIkeV2DpdHeartBeatEventHandler
    {
public:
	static CIkev2SA* NewL(CIkev2PluginSession& aIkeV2PluginSession, 
	                      TIkev2SAData& aIkev2SAdata, 
	                      MIkeDebug& aDebug);	
    ~CIkev2SA();
    
	void UpdateL(TIkev2SAData* aIkev2SAdata, TIkeV2IpsecSAData* aIpsecSAData);
	TBool RemoteAddrChanged(TInetAddr& aNewIp);	
	TIkeV2IpsecSAData* RemoveIpsecSaData(const TDesC8& aInSpi, const TDesC8& aOutSpi);
	TIkeV2IpsecSAData* FindIpsecSaData(const TDesC8& aInSpi, const TDesC8& aOutSpi, TBool aRemove);
	void DeleteIpsecSaData(const TDesC8& aInSpi, const TDesC8& aOutSpi);
	void EventHandlerL();
	TIkeV2IpsecSAData* GetIpsecSaQue();
	void SetIpsecSaQue(TIkeV2IpsecSAData* aQue);
	
private:
    CIkev2SA(CIkev2PluginSession& aIkeV2PluginSession, MIkeDebug& aDebug);
    void ConstructL(TIkev2SAData& aIkev2SAdata);
	void StartTimer();
	void DeleteIpsecSas(TIkeV2IpsecSAData* aSa);	
	void LinkIpsecSa(TIkeV2IpsecSAData* aSa); 
	void PurgeIpsecDataQue();	
	
protected:
	//
	// CActive methods
	//
	void DoCancel();
	void RunL();

public:	
	//
	// Header Data
	//
	TIkev2SAData iIkeV2SaData;      // Common negotiation info
	CIkev2SA* iNext;        // A link field to maintain negotiations.
	
private:
    CIkev2PluginSession& iIkeV2PluginSession;
    MIkeDebug&           iDebug;
	TIkeV2IpsecSAData*  iIpsecSaQue;     // Ipsec SA information queue
	CIkeV2KeepAlive* iIkeKeepAlive;   // Pointer to common IKE keepalive object	
	TUint32        iRemainingTime;   // Current lifetime left
	TUint32        iRekeyingThreshold;

    };

#endif
