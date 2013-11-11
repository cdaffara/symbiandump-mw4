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
* Description:  Timer to tricket IKE datagram resend, if do not obtain a reply
*
*/

#include "ikev2retransmittimer.h"

//The actual time out value is calculated:
//timeout = send_attempt * KRetryTimeOutBaseMicroSeconds;
//So the time out value increases by geometric serie,
//not exponenially like recommended by the RFC
static const TUint KRetryTimeOutBaseMicroSeconds = 1 * 1000000; //microseconds  


CIkev2RetransmitTimer* CIkev2RetransmitTimer::NewL( MIkev2RetransmitTimerCallback& aCallback )
    {
    CIkev2RetransmitTimer* self = new (ELeave) CIkev2RetransmitTimer( aCallback );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);    
    return self;
    }

CIkev2RetransmitTimer::CIkev2RetransmitTimer( MIkev2RetransmitTimerCallback& aCallback )
 : CTimer(EPriorityStandard),
   iCallback(aCallback)
    {
	CActiveScheduler::Add(this);   
    }

CIkev2RetransmitTimer::~CIkev2RetransmitTimer()
    {
	Cancel();
    }

void CIkev2RetransmitTimer::IssueRequest( TUint16 aSendAttempt )
    {
	TTimeIntervalMicroSeconds32 interval = (aSendAttempt * KRetryTimeOutBaseMicroSeconds);	
	After(interval);
    }

void CIkev2RetransmitTimer::RunL()
    {
	iCallback.RetransmitRequest();
    }

