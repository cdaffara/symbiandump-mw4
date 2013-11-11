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
* Description:  IKEv1 keep alive object
*
*/


#include <eikenv.h>
#include <es_sock.h>
#include <e32math.h>
#include "ikedebug.h"
#include "ikev1keepalive.h"
#include "ikev1pluginsession.h"

_LIT8(KMsgContent, "\xff");

CIkeV1KeepAlive* CIkeV1KeepAlive::NewL( CIkev1PluginSession& aPluginSession,
                                        TInt aPort,
                                        TInetAddr& aDestAddr, 
                                        TInt NatKeepAlive,
                                        TInt DpdKeepAlive,
                                        MDpdHeartBeatEventHandler* aHandler,
                                        TUint8 aDscp )
    {
	CIkeV1KeepAlive* self = new (ELeave) CIkeV1KeepAlive( aPluginSession,
	                                                      aPort,
	                                                      aDestAddr,
	                                                      NatKeepAlive, 
	                                                      DpdKeepAlive,
	                                                      aHandler,
	                                                      aDscp );
	CleanupStack::PushL( self );					   	
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }



CIkeV1KeepAlive::CIkeV1KeepAlive( CIkev1PluginSession& aPluginSession,
                                  TInt aPort,
                                  TInetAddr& aDestAddr, 
                                  TInt aNatKeepAlive,
                                  TInt aDpdKeepAlive, 
                                  MDpdHeartBeatEventHandler* aHandler,
                                  TUint8 aDscp ) 
 : CTimer( EPriorityStandard ),
   iPluginSession( aPluginSession ),
   iPort( aPort ),
   iMsg( KMsgContent )
    {
	iDestAddr     = aDestAddr;
	iNatKeepAlive = aNatKeepAlive;
	iDpdKeepAlive = aDpdKeepAlive;
	iCallback     = aHandler;
	iDscp         = aDscp;
    CActiveScheduler::Add( this );
    }

//Destructor
CIkeV1KeepAlive::~CIkeV1KeepAlive()
    {
    Cancel();
    }


void CIkeV1KeepAlive::ConstructL()
    {
    //
    // If NAT keepalive timoeut allocate (and initialize) a TIkeXmitBfr data buffer for
    // NAT keepalive. Allocate a data buffer for special "Echo request"
    // keepalive message, if proprietary "Nokia NAT Traversal is used".
    // Start keep alive timer
    //
    CTimer::ConstructL();
   
    if ( (iNatKeepAlive >= iDpdKeepAlive) && (iDpdKeepAlive != 0) && iCallback )
        iNatKeepAlive	= 0;  // Not needed

    if ( iNatKeepAlive )
        {
        iDestAddr.SetPort(iPort);
	   
        iRemainingTime = iNatKeepAlive;
        if ( iCallback )
            iCurrDPDTimeLeft = iDpdKeepAlive - iNatKeepAlive;
        else iDpdKeepAlive = 0;
        }
    else
        {
        iRemainingTime = iDpdKeepAlive;
        }	    

    StartTimer();
    }


void CIkeV1KeepAlive::DoCancel()
    {
    CTimer::DoCancel();
    }

void CIkeV1KeepAlive::RunL()
    {
	if ( iRemainingTime == 0 ) 
	    {
	    TBool DpdEvent = EFalse;	
	    if ( iDpdKeepAlive )
	        {
	        if ( ( iCurrDPDTimeLeft == 0 ) && iCallback )
	            {
	            //
	            // Call DPD event handler 
	            //
	            DpdEvent = ETrue; 
	            iCallback->EventHandlerL();
	            iRemainingTime = iDpdKeepAlive;				 			  
	            }		 
	        }	   

	    if ( iNatKeepAlive )
	        {
	        //
	        // Send a NAT keepalive message(s)
	        //
	        if ( !DpdEvent )
	            {	   
	            iPluginSession.SendNatKeepAliveL( iDestAddr,
	                                              iMsg,
	                                              iDscp );
	            }
		   
	        iRemainingTime = iNatKeepAlive;
		   
	        if ( iDpdKeepAlive )
	            {
	            if ( DpdEvent )
	                {	  
	                iCurrDPDTimeLeft = iDpdKeepAlive - iNatKeepAlive;
	                }	 
	            else
	                {
	                if ( iCurrDPDTimeLeft < iNatKeepAlive )
	                    {
	                    iRemainingTime   = iCurrDPDTimeLeft; 
	                    iCurrDPDTimeLeft = 0;					 
	                    }	 
	                else iCurrDPDTimeLeft -= iNatKeepAlive;
	                }	  
	            }
	        }	   
	    }
	
	StartTimer();	
    }

TInt CIkeV1KeepAlive::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

void CIkeV1KeepAlive::StartTimer()
    {	
    if ( iRemainingTime > KMaxTInt/1000000 )   //To avoid overflowing the Timer
        {
        iRemainingTime -= KMaxTInt/1000000;
		After(KMaxTInt);
        }
    else    //No overflow
        {
		if ( iRemainingTime )
		    {
		    After(iRemainingTime*1000000);
		    }
		iRemainingTime = 0;
        }
    }

