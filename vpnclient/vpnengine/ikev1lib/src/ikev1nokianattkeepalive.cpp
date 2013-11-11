/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Keepalive object for Nokia IPsec over NAT
*
*/



#include "ikedebug.h" 
#include "ikev1pluginsession.h"
#include <commdbconnpref.h> // TCommDbConnPref
#include "ikev1nokianattkeepalive.h" // CIkev1NokiaNattKeepAlive

_LIT8(KMsgContent, "\xff");

CIkev1NokiaNattKeepAlive* CIkev1NokiaNattKeepAlive::NewL( CIkev1PluginSession&  aPluginSession,
                                                          TInetAddr& aDestAddr, 
                                                          TUint16 aPort,
                                                          TUint aInterval,
                                                          MIkeDebug& aDebug )
	{
	CIkev1NokiaNattKeepAlive *self = new (ELeave) CIkev1NokiaNattKeepAlive( aPluginSession,
	                                                                        aDestAddr,
	                                                                        aPort,
	                                                                        aDebug );
	CleanupStack::PushL(self);
	self->ConstructL(aInterval);
	CleanupStack::Pop(self);
	return self;
	}
		
CIkev1NokiaNattKeepAlive::~CIkev1NokiaNattKeepAlive()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		delete iTimer;
		iTimer = NULL;
		}
	
	}
	
CIkev1NokiaNattKeepAlive::CIkev1NokiaNattKeepAlive( CIkev1PluginSession& aPluginSession,
                                                    TInetAddr& aDestAddr, 
                                                    TUint16 aPort,
                                                    MIkeDebug& aDebug )
: iPluginSession(aPluginSession),
  iDestAddr(aDestAddr),
  iPort(aPort),
  iMsg(KMsgContent),
  iDebug(aDebug)
	{
	iDestAddr.SetPort(iPort);
	}


void CIkev1NokiaNattKeepAlive::ConstructL(TUint aInterval)
	{
	/*
     * Set up periodic timer
     */
    
	// Interval and initial delay
	TTimeIntervalMicroSeconds32 interval(aInterval * 1000000);
	
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	iTimer->Start(interval, interval, 
			TCallBack(CIkev1NokiaNattKeepAlive::PeriodicCallback, this));
	DEBUG_LOG(_L("CIkev1NokiaNattKeepAlive::ConstructL(aInterval) Constructed"));
	}

void CIkev1NokiaNattKeepAlive::Send()
	{	
	TRAPD( err, iPluginSession.SendNokiaNatKeepAliveL( iDestAddr, iMsg, 0 ) );
	err = err;
	DEBUG_LOG1(_L("CIkev1NokiaNattKeepAlive::Send() Request sending of keepalive packet, err=%d"), err);
	}

TInt CIkev1NokiaNattKeepAlive::PeriodicCallback(TAny *aPtr)
	{
	CIkev1NokiaNattKeepAlive *self = static_cast<CIkev1NokiaNattKeepAlive*>(aPtr);
	self->Send();
	return KErrNone;
	}

