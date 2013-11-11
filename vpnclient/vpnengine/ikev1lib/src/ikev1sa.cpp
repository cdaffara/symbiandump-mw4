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
* Description:  IKEv1 SA
*
*/


#include "ikedebug.h"
#include "ikev1SA.h"
#include "ikev1SAdata.h"
#include "ikev1keepalive.h"
#include "ikev1nokianattkeepalive.h" // CIkev1NokiaNattKeepAlive
#include "ikepolparser.h"
#include "ikesocketdefs.h"
#include "ikev1pluginsession.h"

CIkev1SA* CIkev1SA::NewL( CIkev1PluginSession& aPluginSession,
                          TIkev1SAData& aIkev1SAdata,
                          CSARekeyInfo* aSaRekey,
                          MIkeDebug& aDebug )
{
	CIkev1SA *sa = new (ELeave) CIkev1SA( aPluginSession, aDebug );
	sa->ConstructL( aIkev1SAdata, aSaRekey );
	return sa;
}


//Constructor
CIkev1SA::CIkev1SA( CIkev1PluginSession& aPluginSession,
                    MIkeDebug& aDebug )
 : CTimer( EPriorityStandard ),
   iPluginSession( aPluginSession ),
   iDebug( aDebug )
{
    CActiveScheduler::Add(this);
}

void CIkev1SA::ConstructL(TIkev1SAData& aIkev1SAdata, CSARekeyInfo* aSaRekey)
{
	CTimer::ConstructL();   			
	iHdr.CopyL(aIkev1SAdata);
	
	if ( aSaRekey )
	{
	   //
	   // Rekeyed IKE SA. Try to find "original" IKE SA and move IPSEC
	   // SPI list from that SA to the new rekeyed one.
	   // If "original" IKE SA is found, (re)start expiration timer
	   // with rekey "left over" time.
	   //
	   iRekeyed = ETrue;
	   CIkev1SA *OrigSA = iPluginSession.FindIkev1SA(aSaRekey->GetCookieI(), aSaRekey->GetCookieR());
	   if ( OrigSA )
	   {
		  DEBUG_LOG(_L("ISAKMP SA Rekeyed, SPI list moved from original SA"));		   
		  iSPIList = OrigSA->iSPIList;
		  OrigSA->iSPIList = NULL;
		  OrigSA->iSPIList = new (ELeave) CIpsecSPIList(1);  // Dummy
		  if ( OrigSA->IsActive() )
		  {
			 OrigSA->Cancel();  		  
			 OrigSA->iRemainingTime = 0;
			 OrigSA->iLeftOverTime = 0;
			 DEBUG_LOG1(_L("Rekeyed SA expiration time set to %u"),OrigSA->iRemainingTime);
		     OrigSA->StartTimer();
		  }	 
	   }
	}
	
	if ( !iSPIList ) 
       iSPIList = new (ELeave) CIpsecSPIList(4);

	TInt DPDHeartbeat;

	if ( iHdr.iDPDSupported && iHdr.iIkeData->iDPDHeartBeat )
	     DPDHeartbeat = iHdr.iIkeData->iDPDHeartBeat;      
	else DPDHeartbeat = 0;

	TInt KeepAliveTimeout = 0;
	TInt port = IkeSocket::KIkePort500;
	TUint32 NATKeepAlive = (iHdr.iNAT_D_Flags & LOCAL_END_NAT);
	if ( NATKeepAlive || iHdr.iNAT_T_Required )
	{
		KeepAliveTimeout = (TInt)iHdr.iIkeData->iNatKeepAlive; 
		if ( NATKeepAlive )
		{	   
		    port = IkeSocket::KIkePort4500;
			if ( KeepAliveTimeout == 0 )
				KeepAliveTimeout = 120;  // If not configured use 2 minutes  
		}	  
	}

	if ( DPDHeartbeat || KeepAliveTimeout )
	{
		iIkeKeepAlive = CIkeV1KeepAlive::NewL( iPluginSession,
		                                       port,
		                                       (TInetAddr&)iHdr.iDestinAddr,
		                                       KeepAliveTimeout,
		                                       DPDHeartbeat,
										       (MDpdHeartBeatEventHandler*)this );
	}
	
	// Nokia NAT-T needed
	if (!NATKeepAlive &&
	    iHdr.iNAT_T_Required &&
	    (KeepAliveTimeout > 0) )
		{
		// Start Nokia IPsec over NAT keepalive handler
		TInetAddr addr = (TInetAddr)iHdr.iDestinAddr;
		
		// NAT-T default ESP UDP port
		TInt port(KNokiaNattDefaultPort);
		if (iHdr.iIkeData->iEspUdpPort)
			port = iHdr.iIkeData->iEspUdpPort;

		iNokiaNatt = CIkev1NokiaNattKeepAlive::NewL( iPluginSession,
		                                             addr,
		                                             port,
		                                             KeepAliveTimeout,
		                                             iDebug );
		}
	
	if ( !iHdr.iVirtualIp && aSaRekey )
	{
	   //
	   // Rekeyed IKE SA. No virtual IP address received in IKE SA
	   // negotiation. Get "old" virtual IP address saved into
	   // CSARekeyInfo object (if any).
	   //
		iHdr.StoreVirtualIp(aSaRekey->GetInternalAddr());
	}	

	
    //Lifetime in seconds
    iRemainingTime = iHdr.iLifeTimeSecs;
    if ( iRemainingTime == 0 ) 
        iRemainingTime = DEFAULT_MAX_ISAKMP_LIFETIME;

	//
	//  Check if IKE SA rekeying threshold value (per cent) defined
	//  If it is (value is between 70 - 95), use that per cent value
	//  as IKE SA timeout (Rekey for a new IKE SA is started then)
	//  "Left over" time is the expiration timeout for rekeyed IKE SA
	//  value which is used when rekey negotiation is started.
	//  The minimum value for that is set to 30 seconds
	//
	TInt RekeyThreshold = iHdr.iIkeData->iRekeyingThreshold;
	if ( RekeyThreshold != 0 )	
	{
	   if ( RekeyThreshold < 70 )
		    RekeyThreshold = 70;
	   else if ( RekeyThreshold > 95 )
		    RekeyThreshold = 95;
	   DEBUG_LOG1(_L("Negotiated ISAKMP Lifetime set to %u"),iRemainingTime);
	   iLeftOverTime   = iRemainingTime - ((iRemainingTime/100.0) * RekeyThreshold); 	
	   iRemainingTime -= iLeftOverTime;
	   if ( iLeftOverTime < 30 )
		   iLeftOverTime = 30;  
	} 		

    DEBUG_LOG1(_L("ISAKMP Lifetime set to %u"),iRemainingTime);
			
    //Lifetime in Kb
    iRemainingKB = iHdr.iLifeTimeKB;
    DEBUG_LOG1(_L("ISAKMP KB Lifetime set to %u"),iRemainingKB);

	StartTimer();			
			
}

//Destructor
CIkev1SA::~CIkev1SA()
{
    Cancel();
    
    //Delete the IPSEC SAs as well if desired
    if ( iHdr.iIkeData && iSPIList)
        {
        for (TInt i = 0; i < iSPIList->Count(); i++)
            {
            TIpsecSPI* spi_node = iSPIList->At(i);
            iPluginSession.DeleteIpsecSA( spi_node->iSPI,
                                          spi_node->iSrcAddr, 
                                          spi_node->iDstAddr,
                                          spi_node->iProtocol );
            }
        }   
        
	iHdr.CleanUp();	
    //Deletes the SPI List
    delete iSPIList;
	delete iIkeKeepAlive;
	delete iNokiaNatt;
}


void CIkev1SA::SetExpired()
{
    DEBUG_LOG(_L("CIkev1SA::SetExpired"));

	if ( !iExpired )  //If already expired do nothing to avoid renewing the expiration timer.
	{
	    DEBUG_LOG(_L("SA is still active. Expiring it..."));
	
		iExpired = ETrue;
		//if ( iHdr.iIkeData->iIpsecExpires )
		//{	
	    //DEB(iEngine->PrintText(_L("iIpsecExpires is ETrue\n"));)
		for (TInt i = 0; i < iSPIList->Count(); i++)
		{
		    DEBUG_LOG(_L("Deleting IPsec SA"));
			TIpsecSPI* spi_node = iSPIList->At(i);
			iPluginSession.DeleteIpsecSA( spi_node->iSPI,
			                              spi_node->iSrcAddr,
			                              spi_node->iDstAddr,
			                              spi_node->iProtocol );
		}
		//}	
		Cancel();   //Cancel the current timer
		After(ISAKMP_DELETE_TIME);
	}
}

void CIkev1SA::UpdateSAL(TBool aExpired, TIkev1SAData* aIkev1SAdata)
{
    DEBUG_LOG(_L("CIkev1SA::UpdateSAL"));

    if ( aExpired )
    {	    
        ExpireSA();
    }
    else
    {
        DEBUG_LOG(_L("Not expiring SA"));
        if ( aIkev1SAdata ) 
        {            
            iHdr.CopyL(*aIkev1SAdata);
        }
    }	
}

void CIkev1SA::ExpireSA()
    {
    DEBUG_LOG(_L("Expiring SA"));
    SetExpired();
    }

void CIkev1SA::DoCancel()
{
    CTimer::DoCancel();
}

void CIkev1SA::RunL()
{

    DEBUG_LOG(_L("CIkev1SA::RunL"));
    if (!iExpired)  //Still alive so that's a normal Lifetime Expiration
    {
        DEBUG_LOG(_L("Sa is not expired"));
    
		if (iRemainingTime > 0) //Timer still no finished
		{
			StartTimer();
			return;
		}
		
		if ( iLeftOverTime )
		{
		    //
			// Start IKE phase 1 rekey operation
			//
			iRemainingTime = iLeftOverTime;
			iLeftOverTime  = 0;
			CSARekeyInfo* SARekeyInfo = CSARekeyInfo::NewL(iHdr.iCookie_I, iHdr.iCookie_R, iHdr.iVirtualIp);
				
		    iHdr.iVirtualIp = NULL; //Exclusive ownership of the object moved to TSARekeyInfo
		    DEBUG_LOG(_L("Starting ISAKMP SA rekeying "));
		    CleanupStack::PushL(SARekeyInfo);					   
		    iPluginSession.RekeyIkeSAL(&iHdr, SARekeyInfo);
		    CleanupStack::Pop(SARekeyInfo);					   			   
		    StartTimer();  
		}
		else
		{	
            DEBUG_LOG(_L("**\n---ISAKMP SA Deleted---- Lifetime expired**"));
			iPluginSession.DeleteIkeSA(&iHdr, EFalse);  // "Normal" close
			SetExpired();
		}   
    }
    else
	{	//Expired must be erased Completely after the default waiting time
	
	    DEBUG_LOG(_L("Deleting IKE Sa"));
	    iPluginSession.RemoveIkeSA( this, iStatus.Int() );
	}	
	
}

TInt CIkev1SA::RunError(TInt aError)
    {
    DEBUG_LOG1(_L("CIkev1SA::RunError, err=%d"), aError);
    iPluginSession.HandleError(aError);
    return KErrNone;
    }

void CIkev1SA::StartTimer()
{
	if (iRemainingTime > KMaxTInt/SECOND)   //To avoid overflowing the Timer
	{
		iRemainingTime -= KMaxTInt/SECOND;
		After(KMaxTInt);
	}
	else    //No overflow
	{
		After(iRemainingTime*SECOND);
		iRemainingTime = 0;
	}
}

//Adds a new node to the List of SPIs to know the direction if it has to be deleted.
void CIkev1SA::AddIpsecSPIL(TIpsecSPI& aIpsecSpi)
{
    TIpsecSPI* spi_node =  new (ELeave) TIpsecSPI;
    CleanupStack::PushL(spi_node);
    iSPIList->AppendL(spi_node);
    CleanupStack::Pop();
    spi_node->iSrcAddr  = aIpsecSpi.iSrcAddr;
    spi_node->iDstAddr  = aIpsecSpi.iDstAddr;
    spi_node->iSPI      = aIpsecSpi.iSPI;
    spi_node->iProtocol = aIpsecSpi.iProtocol;
	spi_node->iInbound  = aIpsecSpi.iInbound;
}

TBool CIkev1SA::FindIpsecSPI(TUint32 aSPI, TBool aInbound)
{
    TIpsecSPI *spi_node;
    for (TInt i = 0; i < iSPIList->Count(); i++)
    {
        spi_node = iSPIList->At(i);
        if ( (spi_node->iSPI == aSPI) && (spi_node->iInbound == aInbound) )
        {
            return ETrue;
        }
    }

    return EFalse;
}

//
//Deletes a TIpsecSPI matching aSPI 
// 
TBool CIkev1SA::DeleteIpsecSPI(TUint32 aSPI, TBool aInbound)
{
    TIpsecSPI *spi_node;
    for (TInt i = 0; i < iSPIList->Count(); i++)
    {
        spi_node = iSPIList->At(i);
        if ( (spi_node->iSPI == aSPI) && (spi_node->iInbound == aInbound) )
        {
            delete spi_node;
            iSPIList->Delete(i);
            return ETrue;
        }
    }

    return EFalse;
}

//
// Flush all Ipsec SA:s bound to this IKE SA from SADB and send Delete
// payload for all inbound SAs
// 
void CIkev1SA::DeleteIpsecSAs()
{
    TIpsecSPI* spi_node;
	TInt c = iSPIList->Count();
    for (TInt i = 0; i < c; i++)
    {
        spi_node = iSPIList->At(i);
        if ( spi_node->iInbound )
		{	
            //Only the inbound ones notified to avoid receiving packets using an expired SA
            //The opposite if receiving a Delete
			DEBUG_LOG1(_L("Sending ISAKMP Delete payload for IPSec SPI %x"),
			        (int)ByteOrder::Swap32(spi_node->iSPI));

            // Call to delete may fail (delete sends DELETE payloads, and the data connection 
            // may not be open anymore). This is non-fatal, however.
            TRAPD(err, iPluginSession.DeleteIpsecSAL(&iHdr, spi_node));
            if (err == KErrNone) 
                {
                // DELETE sent successfully
    			DEBUG_LOG(_L("CIkev1SA::DeleteIpsecSAsL() IPsec SA delete OK"));
                }
            else if (err == KErrNotFound) 
                {
                // Non-fatal leave occured (couldn't send DELETE due to invalid connection)
                // We can still continue purging IPSEC SAs.
    			DEBUG_LOG(_L("CIkev1SA::DeleteIpsecSAsL() IPsec SA delete failed due non-existing connection. Non-fatal, continuing"));
                }
            else
                {
                // Fatal leave (e.g. out of memory etc)
    			DEBUG_LOG(_L("CIkev1SA::DeleteIpsecSAsL() IPsec SA deletion error. Fatal."));
    			iPluginSession.HandleError(err);
                return;
                }
		}
	    iPluginSession.DeleteIpsecSA(spi_node->iSPI, spi_node->iSrcAddr, spi_node->iDstAddr, spi_node->iProtocol);		
		delete spi_node;
    }
    iSPIList->Reset();  //Empties the full list at once
}

//
// void CIkev1SA::DeleteIpsecSAsForced()
// 
void CIkev1SA::DeleteIpsecSAsForced()
{
    TIpsecSPI* spi_node;
	TInt c = iSPIList->Count();
    for (TInt i = 0; i < c; i++)
    {
        spi_node = iSPIList->At(i);
       	iPluginSession.DeleteIpsecSA( spi_node->iSPI,
       	                              spi_node->iSrcAddr,
       	                              spi_node->iDstAddr,
       	                              spi_node->iProtocol );		
		delete spi_node;
    }
    iSPIList->Reset();
}

void CIkev1SA::EventHandlerL()
{
	//
	// The implementation for class MDpdHeartBeatEventHandler virtual function
	// This method is called by an CIkeKeepAlive object instance when
	// DPD heartbeat timeout has elapsed.
	//
	if ( !iExpired && iSPIList->Count() )
	   iPluginSession.KeepAliveIkeSAL(&iHdr);	
}

void CIkev1SA::CancelRekey()
    {
    if ( iLeftOverTime != 0 )
        {
        DEBUG_LOG1(_L("CIkev1SA::CancelRekey, remaining time=%d"), iLeftOverTime );
        iRemainingTime = iLeftOverTime;
        iLeftOverTime = 0;
        }        
    }

//
//class CIpsecSPIList : public CArrayPtr<TIpsecSPI>
//
CIpsecSPIList::CIpsecSPIList(TInt aGranularity) : CArrayPtrFlat<TIpsecSPI>(aGranularity){}
CIpsecSPIList::~CIpsecSPIList() {ResetAndDestroy();}

