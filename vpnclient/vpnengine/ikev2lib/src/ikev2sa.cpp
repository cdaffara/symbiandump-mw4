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
* Description:  IKEv2 SA
*
*/


#include "ikedebug.h"
#include "ikev2SA.h"
#include "ikepolparser.h"
#include "ikev2ipsecsadata.h"
#include "ikev2pluginsession.h"

static const TUint32 KMinRekeyingThreshold = 70;
static const TUint32 KMaxRekeyingThreshold = 95;

CIkev2SA* CIkev2SA::NewL(CIkev2PluginSession& aIkeV2PluginSession, TIkev2SAData& aIkev2SAdata, MIkeDebug& aDebug)
{
	CIkev2SA *sa = new (ELeave) CIkev2SA(aIkeV2PluginSession, aDebug);
	sa->ConstructL(aIkev2SAdata);
	return sa;
}


CIkev2SA::CIkev2SA(CIkev2PluginSession& aIkeV2PluginSession, MIkeDebug& aDebug)
:CTimer(EPriorityStandard), iIkeV2PluginSession(aIkeV2PluginSession), iDebug(aDebug)
{
    CActiveScheduler::Add(this);
}


void CIkev2SA::ConstructL(TIkev2SAData& aIkev2SAdata)
{
	CTimer::ConstructL();		
	iIkeV2SaData.Copy(aIkev2SAdata);
	//
	// Calculate lifetime value for the new IKE SA
	// The jitter value is adjusted from SA internal ID (SAId mod 8) 
	//
	
	if (iIkeV2SaData.iIkeData->iRekeyingThreshold != 0)
	    {
        if (iIkeV2SaData.iIkeData->iRekeyingThreshold < KMinRekeyingThreshold)
            {
            iRekeyingThreshold = KMinRekeyingThreshold;
            }
        else if (iIkeV2SaData.iIkeData->iRekeyingThreshold > KMaxRekeyingThreshold)
            {
            iRekeyingThreshold = KMaxRekeyingThreshold;
            }
        else
            {
            iRekeyingThreshold = iIkeV2SaData.iIkeData->iRekeyingThreshold;
            }
        TReal lifeTime = (TReal)iIkeV2SaData.iLifetime * ((TReal)iRekeyingThreshold / 100.0);
        iRemainingTime = (TUint32)lifeTime + (iIkeV2SaData.SaId() % 8);
	    }
	else
	    {
        iRemainingTime = iIkeV2SaData.iLifetime + (iIkeV2SaData.SaId() % 8);
	    }
	    
	iIkeV2SaData.iSAState  = KSaStateReady;

	
	TInt DPDHeartbeat = 0;
	if ( iIkeV2SaData.iIkeData->iDPDHeartBeat )
	   DPDHeartbeat = iIkeV2SaData.iIkeData->iDPDHeartBeat;      

	if ( DPDHeartbeat )
	{
		iIkeKeepAlive = CIkeV2KeepAlive::NewL(DPDHeartbeat, *this);
	}
										  
	DEBUG_LOG2(_L("IKEv2 SA constructed, SAId: %d, Lifetime: %d"), iIkeV2SaData.SaId(), iRemainingTime);
	
	StartTimer();
}


CIkev2SA::~CIkev2SA()
{
    if (IsActive())
        Cancel();
	PurgeIpsecDataQue();  	
	iIkeV2SaData.CleanUp();
	delete iIkeKeepAlive;	
}


void CIkev2SA::DoCancel()
{
    CTimer::DoCancel();
}

void CIkev2SA::UpdateL(TIkev2SAData* aIkev2SAdata, TIkeV2IpsecSAData* aIpsecSAdata)
{
    DEBUG_LOG(_L("CIkev2SA::UpdateL"));

	if ( aIkev2SAdata )
	{
	   //
	   // Update IKE Sa data information and reset/restart IKE SA lifetime
	   //
	    iIkeV2SaData.Copy(*aIkev2SAdata);
	    iIkeV2SaData.iSAState = KSaStateReady;		
	}
	if ( aIpsecSAdata )
	{
	   //
	   // Link an Ipsec SA pair information into IKE SA
	   //
		TIkeV2IpsecSAData* IpsecSA = new (ELeave) TIkeV2IpsecSAData(iDebug);
		IpsecSA->Copy(*aIpsecSAdata);
		LinkIpsecSa(IpsecSA);						  
		DEBUG_LOG3(_L("Ipsec SA added into IKE SA, SAId: %d, In SPI: %d, Out SPI: %d"),
				   iIkeV2SaData.SaId(), &aIpsecSAdata->iSPI_In, &aIpsecSAdata->iSPI_Out);
	}
}

TBool CIkev2SA::RemoteAddrChanged(TInetAddr& aNewIp)
{

#if __DEBUG
	TBuf<40> txt_addr;				
	aNewIp.OutputWithScope(txt_addr);
	DEBUG_LOG2(_L("Remote IP changed IKE SA: %d, new address %S"), iIkeV2SaData.SaId(), &txt_addr);
#endif //__DEBUG
	aNewIp = aNewIp; //To silence UREL warnings: contents of DEB macro invisible in UREL builds
	
	return ETrue;
}


TIkeV2IpsecSAData* CIkev2SA::RemoveIpsecSaData(const TDesC8& aInSpi, const TDesC8& aOutSpi)
{ 
    return FindIpsecSaData(aInSpi, aOutSpi, ETrue); 
} 


TIkeV2IpsecSAData* CIkev2SA::FindIpsecSaData(const TDesC8& aInSpi, const TDesC8& aOutSpi, TBool aRemove)
{

    __ASSERT_DEBUG( aInSpi.Length() == 4 || aInSpi.Length() == 0, User::Invariant() );
    __ASSERT_DEBUG( aOutSpi.Length() == 4 || aOutSpi.Length() == 0, User::Invariant() );
    __ASSERT_DEBUG( aInSpi.Length() != 0 || aOutSpi.Length() != 0, User::Invariant() );

	TInt Found = 0;
	if ( aInSpi.Length() > 0 )
		Found ++;
	if ( aOutSpi.Length() > 0 )
		Found ++;
	if ( Found == 0 )
	   return NULL;	

	TInt Match;
	TIkeV2IpsecSAData* Prev = NULL;
	TIkeV2IpsecSAData* Sa   = iIpsecSaQue;

	while ( Sa )
	{
		Match = 0;
		if ( aInSpi.Length() > 0 && (aInSpi.Compare(Sa->iSPI_In) == 0))
           Match ++;   		
		if ( aOutSpi.Length() > 0 && (aOutSpi.Compare(Sa->iSPI_Out) == 0) )
		   Match ++;
		if ( Match == Found )
		{
		   if ( aRemove )
		   {
			  if ( Prev )
				   Prev->iNext = Sa->iNext;
			  else iIpsecSaQue = Sa->iNext;
		   }	   
		   break;
		}
		Prev = Sa;
		Sa   = Sa->iNext;
	}
	return Sa;
}

void CIkev2SA::DeleteIpsecSaData(const TDesC8& aInSpi, const TDesC8& aOutSpi)
{
	TIkeV2IpsecSAData* SaData = FindIpsecSaData(aInSpi, aOutSpi, ETrue);
	if ( SaData )
	{
		SaData->PurgeKeyMaterial();
		SaData->DeleteRekeyData();
		DeleteIpsecSas(SaData);		
		delete SaData;
	}	
}

void CIkev2SA::LinkIpsecSa(TIkeV2IpsecSAData* aSa) 
{
    ASSERT(aSa);
    aSa->iNext = iIpsecSaQue; 
    iIpsecSaQue = aSa;
}   

void CIkev2SA::PurgeIpsecDataQue()
{
	
	TIkeV2IpsecSAData* Sa = iIpsecSaQue;
	while ( iIpsecSaQue )
	{
		iIpsecSaQue = Sa->iNext;
		Sa->PurgeKeyMaterial();
		Sa->DeleteRekeyData();
		DeleteIpsecSas(Sa);
		delete Sa;
		Sa = iIpsecSaQue;
	}	
}

void CIkev2SA::DeleteIpsecSas(TIkeV2IpsecSAData* aSa)
{
    ASSERT(aSa);
	TInetAddr LocalAddr;
	if ( aSa->iSrcSpecific )
		 LocalAddr = iIkeV2SaData.iLocalAddr;
	else LocalAddr.Init(0); 
	LocalAddr.SetPort(0);	
	TInetAddr RemoteAddr = iIkeV2SaData.iRemoteAddr;
	RemoteAddr.SetPort(0);
	if ( aSa->iSPI_In.Length() > 0 ) 
	{
        TUint32 spi;
        TPtr8 spiPtr(reinterpret_cast<TUint8*>(&spi), sizeof(spi));
        spiPtr = aSa->iSPI_In;
		iIkeV2PluginSession.DeleteIpsecSA(spi, RemoteAddr, LocalAddr, aSa->iSaType);
	}
	if ( aSa->iSPI_Out.Length() > 0 )
	{
        TUint32 spi;
        TPtr8 spiPtr(reinterpret_cast<TUint8*>(&spi), sizeof(spi));
        spiPtr = aSa->iSPI_Out;
        iIkeV2PluginSession.DeleteIpsecSA(spi, LocalAddr, RemoteAddr, aSa->iSaType);
	}
}


void CIkev2SA::RunL()
{
	//
	// If IKE SA lifetime expired, delete IKE SA if there is no
	// active IPSEC SA:s alive. If there is rekey IKE SA.
	//
    DEBUG_LOG2(_L("CIkev2SA::RunL, SAId=%d, remaining time=%d"),
            iIkeV2SaData.SaId(), iRemainingTime );
	if (iRemainingTime == 0) 
	{										 		
		if ( iIpsecSaQue && iRekeyingThreshold != 0)
		{
		    iIkeV2PluginSession.RekeyIkeSAL(&iIkeV2SaData);
		}
		else 
		{		 
		    iIkeV2PluginSession.DeleteIkeSAL(&iIkeV2SaData, ETrue); // "Normal" close
		}
	}
	else StartTimer();
	
}

void CIkev2SA::StartTimer()
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


void CIkev2SA::EventHandlerL()
{
	//
	// The implementation for class MDpdHeartBeatEventHandler virtual function
	// This method is called by an CIkeKeepAlive object instance when
	// DPD heartbeat timeout has elapsed.
	//
    iIkeV2PluginSession.KeepAliveIkeSAL(&iIkeV2SaData);
}


TIkeV2IpsecSAData* CIkev2SA::GetIpsecSaQue()
{ 
    TIkeV2IpsecSAData* Que = iIpsecSaQue; 
    iIpsecSaQue = NULL; 
    return Que; 
}


void CIkev2SA::SetIpsecSaQue(TIkeV2IpsecSAData* aQue) 
{ 
    if (aQue != NULL) 
        { 
        LinkIpsecSa(aQue); 
        } 
}

