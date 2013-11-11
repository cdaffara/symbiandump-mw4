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
* Description: Utility methods to handle PFKEY messaging
*
*/

#include "ikev2pfkey.h"
#include "ikev2pluginsession.h"
#include "ikev2SAdata.h"
#include "ipsecsadata.h"
#include "pfkeyextdatautil.h"
#include "ikev2acquire.h"
#include "ikev2trafficselector.h"
#include "ikev2ipsecsadata.h"
#include <networking/pfkeyv2.h>
#include "ikev2const.h"

static const TUid KIkeV2PlugInUid3 = { 0x10206993 };

void Ikev2Pfkey::UpdateIpsecSaDataBaseL(const TIkev2SAData& aIkev2SA, 
                                        const TIkeV2IpsecSAData& aChild, 
                                        CIkev2PluginSession& aIkePluginSession,
                                        CIkev2Acquire& aAcquire)
{
	//
	//  Fill Ipsec SA info int TSAData object for PFKEY Update/Add
	//  primitives. Take local and remote identity data from CIkev2Acquire object
	//
    __ASSERT_DEBUG(aChild.iKeyMaterial != NULL, User::Invariant());

	TUint16 LocalPort    = 0;
	TUint16 RemotePort   = 0;	
	TUint8  LocalIdType  = 0;
	TUint8  RemoteIdType = 0;	
	TUint8  Protocol     = 0;	
	const CArrayFix<TIkeV2TrafficSelector>& TsI = aAcquire.TS_i();
	const CArrayFix<TIkeV2TrafficSelector>& TsR = aAcquire.TS_r();
	
	__ASSERT_DEBUG(TsI.Count() > 0, User::Invariant());
	__ASSERT_DEBUG(TsR.Count() > 0, User::Invariant());
		
    HBufC8* localId  = aAcquire.LocalId();
    HBufC8* remoteId = NULL;		
    
    
    if ( aAcquire.RemoteId() )
        {
        remoteId = aAcquire.RemoteId()->AllocLC();
        }
    else
        {            
        if ( aAcquire.Response() )
            {
	        remoteId = TsI[0].IdFromTsL();
	        CleanupStack::PushL(remoteId);
	        }
        else
            {
            remoteId = TsR[0].IdFromTsL();
            CleanupStack::PushL(remoteId);            
            }
	}

	//
	//  Build Encryption and integrity keys for Ipsec SA:s
    //	Keying material MUST be taken from the expanded KEYMAT in the
	//	following order:
	//	- All keys for SAs carrying data from the initiator to the responder
	//	  are taken before SAs going in the reverse direction.
	//  If a single protocol has both encryption and authentication keys,
	//  the encryption key is taken from the first octets of KEYMAT and
	//	the authentication key is taken from the next octets.
	//
    const TUint8* KeyMat = aChild.iKeyMaterial->Des().Ptr();
	TPtrC8 InAuthKey(NULL,0), OutAuthKey(NULL,0);
	TPtrC8 InEncrKey(NULL,0), OutEncrKey(NULL,0);

	if ( aChild.iSaType == SADB_SATYPE_ESP )
	{
		if ( aAcquire.Response() )
		{	
		   KeyMat = Ikev2Pfkey::GetIpsecKeys(&InEncrKey, &InAuthKey, KeyMat, aChild.iCipherKeyLth, aChild.iIntegKeyLth);
		   KeyMat = Ikev2Pfkey::GetIpsecKeys(&OutEncrKey, &OutAuthKey, KeyMat, aChild.iCipherKeyLth, aChild.iIntegKeyLth);		   
		}   
        else
		{
		   KeyMat = Ikev2Pfkey::GetIpsecKeys(&OutEncrKey, &OutAuthKey, KeyMat, aChild.iCipherKeyLth, aChild.iIntegKeyLth);
		   KeyMat = Ikev2Pfkey::GetIpsecKeys(&InEncrKey, &InAuthKey, KeyMat, aChild.iCipherKeyLth, aChild.iIntegKeyLth);		   
		}	
	}	
	else     
	{  // SADB_SATYPE_AH
		if ( aAcquire.Response() )
		{	
			KeyMat = Ikev2Pfkey::GetIpsecKeys(&InEncrKey, &InAuthKey, KeyMat, 0, aChild.iIntegKeyLth);
			KeyMat = Ikev2Pfkey::GetIpsecKeys(&OutEncrKey, &OutAuthKey, KeyMat, 0, aChild.iIntegKeyLth);		   			
		}   
		else
		{
			KeyMat = Ikev2Pfkey::GetIpsecKeys(&OutEncrKey, &OutAuthKey, KeyMat, 0, aChild.iIntegKeyLth);
			KeyMat = Ikev2Pfkey::GetIpsecKeys(&InEncrKey, &InAuthKey, KeyMat, 0, aChild.iIntegKeyLth);
		}	
	}
	TIpsecSAData SaData;
	TUint32 SadbFlags;
	if (aAcquire.DHGroup())
		 SadbFlags = SADB_SAFLAGS_PFS;
	else SadbFlags = 0;
	
	if ( !aChild.iTransport  )
	{
	    SaData.iInternalAddress.Init(KAfInet6);
	    SaData.iInternalAddress.SetScope(aIkePluginSession.VpnInterfaceIndex());
		SadbFlags |= SADB_SAFLAGS_INT_ADDR; 
	}
	
	//
	// Inbound SA
	//
	SaData.iSAType = aChild.iSaType;
	if ( aAcquire.Response() )		
		 SaData.iSeq = aAcquire.Id();
	else SaData.iSeq = aAcquire.Seq();
	SaData.iPid = KIkeV2PlugInUid3.iUid;
	SaData.iSrc = aIkev2SA.iRemoteAddr;
	SaData.iSrc.SetPort(RemotePort);
	if ( aAcquire.SrcSpecific() )
	{	
	   SaData.iDst = aIkev2SA.iLocalAddr;
	   SaData.iDst.SetPort(LocalPort);
	}
	else SaData.iDst.Init(0);
	SaData.iProtocol  = Protocol;
	SaData.iSrcIdType = RemoteIdType;
	SaData.iDstIdType = LocalIdType;
    SaData.iSrcIdent.Set(remoteId->Des());
	if ( !aAcquire.Response() && localId != NULL)		
       SaData.iDstIdent.Set(localId->Des());
	
    TUint32 spi;
    TPtr8 spiPtr(reinterpret_cast<TUint8*>(&spi), sizeof(spi));
    spiPtr = aChild.iSPI_In;
	SaData.iSPI = spi;
	switch ( aChild.iIntegAlg )
	{
		case AUTH_HMAC_MD5_96:
			SaData.iAuthAlg = SADB_AALG_MD5HMAC;
			break;

		case AUTH_HMAC_SHA1_96:
			SaData.iAuthAlg = SADB_AALG_SHA1HMAC;
			break;

		default:
			SaData.iAuthAlg = 0;
            break;	
	}	
	SaData.iEncrAlg = aChild.iEncrAlg;    // Should correspond PFKEY2.H !
	SaData.iAuthKey.Set(InAuthKey);
	SaData.iEncrKey.Set(InEncrKey);
    SaData.iHard = aAcquire.HardLifetime();
    SaData.iSoft = aAcquire.SoftLifetime();
	SaData.iReplayWindowLength = aAcquire.ReplayWindow();
	SaData.iFlags = SadbFlags;
	
    //
    //  Get the following implementation specific information for PFKEY
    //  primitives:
    //  -- ESP UDP encapsulation info, if a NAT device detected 
    //  -- Interface index for  tunnel mode inbound SA
    //
	HBufC8* GenExt = NULL;
    if ( aIkev2SA.iNATFlags )
    {
        GenExt = HBufC8::NewLC(128);
        TPtr8   GenExtPtr = GenExt->Des();        
        TInetAddr DummyAddr;
        DummyAddr.SetFamily(KAFUnspec);  
        PFKeyExtDataUtil::BuildUdpEncExtensionData(GenExtPtr, aIkev2SA.iNATFlags,
                                                   EFalse, EFalse, 0, 0, 
                                                   aIkev2SA.iDestinAddr, DummyAddr);                
    }
    else
    {
        GenExt = HBufC8::NewLC(1);
    }
    SaData.iGenericExtension.Set(*GenExt);
	
    aIkePluginSession.UpdateSAL(SaData);

	SaData.iFlags &= ~SADB_SAFLAGS_INT_ADDR; //No VPN interface index to outbound SA
	//
	// Outbound SA. Some changes in the SA, the rest is the same
	//
	if ( !aAcquire.Response() )	
	   SaData.iPid = aAcquire.Pid();  // Use Acquire PID for OUT SA when initiator
	if ( aAcquire.SrcSpecific() )
	{	
		SaData.iSrc = aIkev2SA.iLocalAddr;
		SaData.iSrc.SetPort(LocalPort);
	}
	else SaData.iSrc.Init(0);
	SaData.iDst = aIkev2SA.iRemoteAddr;
	SaData.iDst.SetPort(RemotePort);	
	SaData.iSrcIdType = LocalIdType;
	SaData.iDstIdType = RemoteIdType;
	SaData.iSrcIdent.Set(localId->Des());
	SaData.iDstIdent.Set(remoteId->Des());
	
    spiPtr = aChild.iSPI_Out;
	SaData.iSPI = spi;
	SaData.iAuthKey.Set(OutAuthKey);
	SaData.iEncrKey.Set(OutEncrKey);

	aIkePluginSession.AddSAL(SaData);
	
	CleanupStack::PopAndDestroy(GenExt);
	CleanupStack::PopAndDestroy(remoteId);
	
}

CIkev2Acquire* Ikev2Pfkey::DeleteInboundSPI(const TIkev2SAData& aIkev2SA, 
                                       CIkev2PluginSession& aIkePluginSession, 
                                       CIkev2Acquire* aAcquire)
{
	//
	//  Issue PFKEY Delete SA for Inbound SPI in CIkev2Acquire object and
	//  delete CIkev2Acquire object then
	//
	if ( aAcquire )
	{
		TInetAddr LocalAddr;
		if ( aAcquire->SrcSpecific() )
			 LocalAddr = aIkev2SA.iLocalAddr;
		else LocalAddr.Init(0); 
		LocalAddr.SetPort(0);	
		TInetAddr RemoteAddr = aIkev2SA.iRemoteAddr;
		RemoteAddr.SetPort(0);
		
		TUint32 spi;
		TPtr8 spiPtr(reinterpret_cast<TUint8*>(&spi), sizeof(spi));
		spiPtr = aAcquire->SPI_In();
		aIkePluginSession.DeleteIpsecSA(spi, RemoteAddr, LocalAddr, aAcquire->IpsecProtocol());
		delete aAcquire;
	}
	
	return NULL;
}	

const TUint8* Ikev2Pfkey::GetIpsecKeys(TPtrC8* aEncrKey, TPtrC8* aIntegKey, const TUint8* aKeyMaterial, TInt aCipherKeyLth, TInt aIntegKeyLth )
{
	if ( aCipherKeyLth && aKeyMaterial)
	{
	   ASSERT(aEncrKey);
	   aEncrKey->Set(aKeyMaterial, aCipherKeyLth);
	   aKeyMaterial += aCipherKeyLth;
	}
	
    if ( aIntegKeyLth )
    {	
       ASSERT(aIntegKey && aKeyMaterial);
	   aIntegKey->Set(aKeyMaterial, aIntegKeyLth);
	   aKeyMaterial += aIntegKeyLth;
	}

	return aKeyMaterial;
}	

