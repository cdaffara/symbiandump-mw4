/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common keep alive object
*
*/


#include "ikev2keepalive.h"
#include "ikemsgheader.h"
#include "ikev2pluginsession.h"

CIkeV2KeepAlive* CIkeV2KeepAlive::NewL(TInt DpdKeepAlive, MIkeV2DpdHeartBeatEventHandler& aHandler)
{
    CIkeV2KeepAlive* self = new (ELeave) CIkeV2KeepAlive(DpdKeepAlive, aHandler);
    CleanupStack::PushL(self);                        
    self->ConstructL();
    CleanupStack::Pop(self);                          
    return self;
}


//Constructor
CIkeV2KeepAlive::CIkeV2KeepAlive(TInt aDpdKeepAlive, 
                                 MIkeV2DpdHeartBeatEventHandler& aHandler) 
: CTimer(EPriorityStandard), iCallback(aHandler), iDpdKeepAlive(aDpdKeepAlive)
{	
    __ASSERT_DEBUG(iDpdKeepAlive > 0, User::Invariant());
    CActiveScheduler::Add(this);
}

//Destructor
CIkeV2KeepAlive::~CIkeV2KeepAlive()
{
    if (IsActive())
        Cancel();
}


void CIkeV2KeepAlive::ConstructL()
{
   //
   // If NAT keepalive timoeut allocate (and initialize) a TIkeXmitBfr data buffer for
   // NAT keepalive. Allocate a data buffer for special "Echo request"
   // keepalive message, if proprietary "Nokia NAT Traversal is used".
   // Start keep alive timer
   //
   CTimer::ConstructL();
   
   iRemainingTime = iDpdKeepAlive;
   StartTimer();			
}


void CIkeV2KeepAlive::DoCancel()
{
    CTimer::DoCancel();
}

void CIkeV2KeepAlive::RunL()
{
	if ( iRemainingTime == 0 && iDpdKeepAlive) 
	{	
        iCallback.EventHandlerL();
        iRemainingTime = iDpdKeepAlive;				 			  		 	   
	}	
    StartTimer();	
}

void CIkeV2KeepAlive::StartTimer()
{
	
	if ( iRemainingTime > KMaxTInt/1000000 )   //To avoid overflowing the Timer
	{
		iRemainingTime -= KMaxTInt/1000000;
		After(KMaxTInt);
	}
	else    //No overflow
	{
		if ( iRemainingTime )
		   After(iRemainingTime*1000000);
		iRemainingTime = 0;
	}
}

