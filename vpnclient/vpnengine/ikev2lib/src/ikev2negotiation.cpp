/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   IKEv2/IPSEC SA negotiation
*
*/

#include <random.h>
#include <in_sock.h>

#include "ikev2Negotiation.h"
#include "ikedebug.h"
#include "ikev2natt.h"
#include "ikev2mobike.h"
#include "ikev2proposal.h"
#include "ikev2SAdata.h"
#include "ikev2pluginsession.h"
#include "ikev2pfkey.h"
#include "ikev2config.h"
#include "ikev2EapInterface.h"
#include "ikev2payloads.h"
#include "ikev2const.h"
#include "ikemsgrec.h"
#include "ipsecproposal.h"
#include "ipsecselectors.h"
#include "ikepolparser.h"
#include "kmdapi.h"
#include "ikecaelem.h"
#include "ikecalist.h"
#include "ikepkiutils.h"
#include "vpnapidefs.h"
#include "kmdeventloggerif.h"
#include "ipsecsalist.h"
#include "ikev2message.h"
#include "ikev2identity.h"
#include "ikev2acquire.h"
#include "ikev2expire.h"
#include "ikev2ipsecsarekeydata.h"
#include "ikev2messagesendqueue.h"

_LIT8(KIkev2PSKData, "Key Pad for IKEv2");
_LIT8(KZeroDesc, "");

CIkev2Negotiation* CIkev2Negotiation::NewL(CIkev2PluginSession& aIkeV2PlugInSession,
                                           CPFKeySocketIf& aPfKeySocketIf,
                                           MKmdEventLoggerIf& aEventLogger,
                                           CIkev2MessageSendQueue& aMessageSendQue,
                                           MIkeDebug& aDebug,
                                           CIkeData* aIkeData, 
                                           TUint32 aVpnIapId,
                                           TUint32 aSaId,
                                           TInetAddr aPhysicalInterfaceAddress,
                                           TInetAddr aRemoteAddress)
    {
    
    CIkev2Negotiation* self = new (ELeave) CIkev2Negotiation(aIkeV2PlugInSession, aPfKeySocketIf, 
                                                             aEventLogger, aMessageSendQue, 
                                                             aDebug, aSaId);
    CleanupStack::PushL(self);
    self->ConstructL();

    self->iHdr.iIkeData = aIkeData;
    self->iHdr.iVpnIapId = aVpnIapId;
    self->iProcessEvents     = ETrue;
    self->iHdr.iRemoteAddr = aRemoteAddress;
    self->iHdr.iRemoteAddr.SetPort(IKE_PORT);      
    
    //
    // Get IP address information for IKE SA negotiation
    // Remote address is taken from current IKE policy data (CIkeData)
    // Local address is resolved via IKE policy using policy handle
    //
    if ( self->iHdr.iRemoteAddr.IsUnspecified() )
    {        
        self->iHdr.iRemoteAddr = self->iHdr.iIkeData->iAddr;
        self->iHdr.iRemoteAddr.SetPort(IKE_PORT);
    }   
    self->iHdr.iDestinAddr = self->iHdr.iRemoteAddr;
    self->iHdr.iLocalAddr = aPhysicalInterfaceAddress;
    TInt Scope = self->iHdr.iRemoteAddr.Scope();
    if ( Scope )
        self->iHdr.iLocalAddr.SetScope(Scope); // Set local scope same with remote scope        

    CleanupStack::Pop(self);
    return self;
    }


CIkev2Negotiation* CIkev2Negotiation::NewL(CIkev2PluginSession& aIkeV2PlugInSession,
                                           CPFKeySocketIf& aPfKeySocketIf,
                                           MKmdEventLoggerIf& aEventLogger,
                                           CIkev2MessageSendQueue& aMessageSendQue,
                                           MIkeDebug& aDebug,
                                           TIkev2SAData& aIkev2SAdata)
    {
    CIkev2Negotiation* self = new (ELeave) CIkev2Negotiation(aIkeV2PlugInSession, aPfKeySocketIf, 
                                                             aEventLogger, aMessageSendQue, 
                                                             aDebug, aIkev2SAdata.SaId());
    CleanupStack::PushL(self);
    self->ConstructL();
    
    self->iHdr.Copy(aIkev2SAdata);
    self->iState = KStateIkeSaCompleted;
    
    CleanupStack::Pop(self);
    return self;
    }


CIkev2Negotiation::CIkev2Negotiation(CIkev2PluginSession& aIkeV2PlugInSession, CPFKeySocketIf& aPfKeySocketIf,
                                     MKmdEventLoggerIf& aEventLogger, CIkev2MessageSendQueue& aMessageSendQue,
                                     MIkeDebug& aDebug, TUint32 aSaId) 
: iChild(aDebug), iIkeV2PlugInSession(aIkeV2PlugInSession), iPfKeySocketIf(aPfKeySocketIf), 
  iEventLogger(aEventLogger), iMessageSendQue(aMessageSendQue),iDebug(aDebug), iDHGroupGuess(1) 
    {
    DEBUG_LOG1(_L("CIkev2Negotiation::CIkev2Negotiation: 0x%08x"), this);

    iHdr.SetSaId(aSaId);
    iHdr.iWindowSize = DEF_MSG_ID_WINDOW;        
    }   


void CIkev2Negotiation::ConstructL()
    {  
    iTimer = CIkev2RetransmitTimer::NewL(*this);    
    iSpiRetriever = CIpsecSaSpiRetriever::NewL(*this, iPfKeySocketIf);
        
    iIkeV2PlugInSession.LinkNegotiation(this); // <- takes ownership of this
    iProcessEvents = ETrue;
    }


CIkev2Negotiation::~CIkev2Negotiation()
    {
    
    delete iSpiRetriever;
    // Turn off event processing to prevent EAPVPNIF event
	iProcessEvents = EFalse;
    delete iTimer;
    
    DEBUG_LOG1(_L("CIkev2Negotiation::~CIkev2Negotiation: 0x%08x"), this);        
    iIkeV2PlugInSession.RemoveNegotiation(this);

    iHdr.CleanUp();
    
    //
    // Purge Acquire, Expire and Info message queues
    //
    CIkev2Acquire::PurgeQue(GetAcquireQue());
    CIkev2Expire::PurgeQue(GetExpireQue());
	
	delete iPeerCert;					   
    delete iSavedSaInit;
    delete iProposedSA;
    delete iDHKeys;
    delete iDHPublicPeer;
    delete iNonce_I;
    delete iNonce_R;
    delete iAuthMsgInit;
    delete iAuthMsgResp;
    delete iRemoteIdentity;
    delete iLocalIdentity;
    delete iNatNotify;
    delete iConfigMode;
    delete iEapPlugin;
    delete iPkiService; 
    delete iPresharedKey;
    delete iChildSaRequest;
    }

void CIkev2Negotiation::StartIkeSANegotiationL()
    {
    __ASSERT_DEBUG(iChildSaRequest == NULL, User::Invariant());
    
    //This method should be called only if we have IA in use.
    //Otherwise the negotiation should start with ProcessAcquire
    __ASSERT_DEBUG(iHdr.iIkeData->iUseInternalAddr, User::Invariant());
    
	//
	// This method is called when an IKE SA negotiation is started due
	// a RKMD::Activate() request with policy that uses IA.   
	//    
	iHdr.iInitiator = ETrue;
    LoadEapPluginL();
    
    GetNonceDataL(ETrue);

    CIkev2Acquire* Acquire = IpsecSelectors::BuildVirtualAcquireL(iIkeV2PlugInSession);
    CleanupStack::PushL(Acquire);
           
	if ( !InitPkiServiceL() )
	    {
	    //No PkiService Needed.
	    //Continue by requesting SPI for IPsecSA.	    
	    CIkev2Acquire::Link(Acquire, GetAcquireQue());    
        //
        // Get SPI for inbound SA with PFKEY GETSPI primitive
        //
        GetIpsecSPI(Acquire);
	    }
	else
	    {
	    iChildSaRequest = Acquire;
	    }
	CleanupStack::Pop(Acquire);	
	}

TBool CIkev2Negotiation::StartRespondingL(const ThdrISAKMP& aIkeMessage)
    {
	//
	// This method is called when local end is going to ACT as a
	// responder of an IKE SA negotiation.
	// Initialize PKI service usage, if needed. Because PKI service
	// initialisation is an asynchronous operation we must take a copy
	// of incoming IKE message from where it is processed when PKI
	// service initialisation is completed.
	//
	TBool Status( InitPkiServiceL() );
	if ( Status )
	    {
	    TInt MsgLth = (TInt)aIkeMessage.GetLength(); 	
	    delete iSavedSaInit;
	    iSavedSaInit = NULL;
	    iSavedSaInit = HBufC8::NewL(MsgLth);
	    iSavedSaInit->Des().Copy((TUint8*)&aIkeMessage, MsgLth);
	    }
	return !Status;
    }

void CIkev2Negotiation::StartIkeSADeleteL()
{
	//
	// This method is called when an IKE SA shall be deleted either due
	// IKE SA timeout or due a RKMD::Deactivate() request
	//
	BuildDeleteRequestL(NULL);
}


void CIkev2Negotiation::IkeSaCompletedL()
{

	//
	// This method is when an IKE SA negotiation has been succesfully
	// completed.
	// The following actions are taken:
	// -- Get Virtual IP from iConfigMode object, if present and
	//    modify IKE SA lifetime if Virtual Ip expiration time is
	//    shorter than configured iKE SA lifetime  
	// -- Create a new IKE SA object, if not a rekeyd IKE SA
	// -- If activation going, call IkeSaCompleted method in plug-in
	//
    TVPNAddress VirtualIp;
	if ( iConfigMode )
	{
	   VirtualIp = iConfigMode->VirtualIp();
	   iHdr.StoreVirtualIp(VirtualIp.iVPNIfAddr);
	   TUint32 ExpireTime = iConfigMode->ExpireTime();
	   if ( ExpireTime && (ExpireTime < iHdr.iLifetime) )
		   iHdr.iLifetime = ExpireTime;
	}	
	
	if(!iIkeV2PlugInSession.FindIkev2SA(iHdr.SaId(), KSaStateNotDefined, KSaStateNotDefined))
	    {
	    iIkeV2PlugInSession.CreateIkev2SAL(iHdr);	
	    }
	
	iIkeV2PlugInSession.IkeSaCompleted(KErrNone, VirtualIp);
	
	iEventLogger.LogEvent(MKmdEventLoggerIf::KLogInfo, R_VPN_MSG_VPN_GW_AUTH_OK, KErrNone, 
	                      iHdr.iVpnIapId, &iHdr.iRemoteAddr);
	iEventLogger.LogEvent(MKmdEventLoggerIf::KLogInfo, R_VPN_MSG_ADDR_INFO_FOR_VPN_AP,
                          iHdr.iNATFlags, iHdr.iVpnIapId,
                          (!VirtualIp.iVPNIfAddr.IsUnspecified() ? &(VirtualIp.iVPNIfAddr) : NULL));			

	if ( iChildSaRequest )
	{	
	   IpsecSANegotiatedL();
	}	
    if ( RequestsPending() )
	{	
	   ContinueIkeNegotiationL();
	}   
	else
	{	
       if ( !iHdr.iInitiator )
	   {	   
	       iIkeV2PlugInSession.StopResponding();
	       delete this;   // Current negotiation can be deleted
	   }
	   else iStopped = ETrue;
	}
}

void CIkev2Negotiation::IkeSaFailed(TInt Status)
    {
	//
	// This method is when a IKE SA negotiation has failed
	// The following actions are taken:
	//

    TVPNAddress dummyVirtualIp;

	iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);	
	
	if ( !iHdr.iInitiator )
	    iIkeV2PlugInSession.StopResponding();
	
	if ( (iSendAttempt <= KMaxSendAttemps ) &&
	     ((iState == KStateIkeSaEapStarted) || 
	      (iState == KStateIkeSaEapGoing)))
		 iDeleteIkeSA = ETrue;
	else iStopped = ETrue;

	iEventLogger.LogEvent(MKmdEventLoggerIf::KLogError, R_VPN_MSG_REAL_IAP_ACT_FAILED, Status, 
	                      iHdr.iVpnIapId, &iHdr.iRemoteAddr);
	
    iIkeV2PlugInSession.IkeSaCompleted(Status, dummyVirtualIp);
    }

void CIkev2Negotiation::IpsecSANegotiatedL()
{
	//
	// This method is when an Ipsec SA negotiation has been succesfully
	// completed.
	// -- Update Ipsec SADB using PFKEY Update and Add primitives	
	// -- Find a new IKE SA object and queue Ipsec SA data into it
	// -- Try to start a new exchange from queue, if there is nothing
	//    to start in queues mark current negotiation stopped
	//
	iChild.iSrcSpecific = iChildSaRequest->SrcSpecific();  
	Ikev2Pfkey::UpdateIpsecSaDataBaseL(iHdr, iChild, iIkeV2PlugInSession, *iChildSaRequest);
	
	CIpsecSARekeyData* rekeyData = 
	    CIpsecSARekeyData::NewL(iChildSaRequest->ReplayWindow(),
                                iChildSaRequest->HardLifetime(),
                                iChildSaRequest->SoftLifetime(),
                                iChildSaRequest->TS_i(),
                                iChildSaRequest->TS_r(),
                                *iChildSaRequest->LocalId(),
                                *iChildSaRequest->RemoteId());
	
	iChild.PurgeKeyMaterial();	// Ipsec Keymaterial not saved into IKE SA
	iChild.DeleteRekeyData();
	iChild.iRekeyData = rekeyData;
	
	iIkeV2PlugInSession.UpdateIkev2SAL(&iHdr, &iChild);
	
	delete iChildSaRequest;
	iChildSaRequest = NULL;
	
	if ( RequestsPending() )	
	     ContinueIkeNegotiationL();
	else
	{	if ( iState == KStateIkeChildSAResponse )
		     delete this;
		else iStopped = ETrue;
	}	
}


void CIkev2Negotiation::ProcessIkeMessageL(const ThdrISAKMP& aIkeMessage, 
                                           const TInetAddr& aRemote, 
                                           TUint16 aLocalPort)
    {
	//
	// Start to process received IKE message by constructing a
	// CIkev2Payloads object. CIkev2Payloads construction takes also
	// care of the decryption of an Encrypted payload if present.  
	//    
	TBool Status( ETrue );
	
	CIkev2Payloads* IkeMsg = CIkev2Payloads::NewL(aIkeMessage, iHdr);
	CleanupStack::PushL(IkeMsg);				 	
	
    DEBUG_LOG2(_L("Process IKE message, SAID=%d, Msg ID=%d"),
            iHdr.SaId(), aIkeMessage.GetMessageId());	
	if ( IkeMsg->Status() )
	    {
	    //
	    //  An error occurred during IKE message parsing
	    //
	    SetNotifyCode(IkeMsg->Status());
        DEBUG_LOG1(_L("Error in parsing of received IKE message: %d"), IkeMsg->Status());

	    if ( !iHdr.iInitiator && iState == KStateIdle )
	        {
		    iStopped = ETrue;   // Negotiation object shall be released
	        }
	    else 
	        {
	        CheckNotifyCodeL(IkeMsg);
	        }
	    CleanupStack::PopAndDestroy(IkeMsg); // IkeMsg
	    return;
	    }
	
	if ( (iHdr.iNATFlags & (REMOTE_END_NAT + MOBIKE_USED)) && 
	    IkeMsg->Encrypted() )
	    {
	    //
	    // Received IKE message contains Encrypted payload. Save source
	    // IP as new destination IP to negotiation object
	    //
	    iHdr.iDestinAddr = aRemote;
	    iHdr.iDestinAddr.SetPort(FLOATED_IKE_PORT);
	    }	
	
	TPtrC8 ikeMsgDes((TUint8*)&aIkeMessage, aIkeMessage.GetLength());
	
	TInetAddr localAddr(iHdr.iLocalAddr);
	localAddr.SetPort(aLocalPort);
	TRACE_MSG(ikeMsgDes, aRemote, localAddr, 
              (CIkePcapTrace::TEncryptionType)iHdr.iEncrAlg);

	//
	// Process received IKE message according to Exchange type
	//
	switch ( aIkeMessage.GetExchange() )
	{
		case IKE_SA_INIT:
		    DEBUG_LOG(_L("IKE_SA_INIT message received"));
			Status = ProcessIkeSaInitL(IkeMsg, aRemote);
			if ( !Status )
			   IkeSaFailed(KKmdIkeNegotFailed);	
			break;
			
		case IKE_AUTH:
		    DEBUG_LOG(_L("IKE_AUTH message received"));
			Status = ProcessIkeAuthL(IkeMsg);
			if ( !Status )
			   IkeSaFailed(KKmdIkeAuthFailedErr);
			break;
			
		case CREATE_CHILD_SA:
		    DEBUG_LOG(_L("CREATE_CHILD_SA message received"));
			Status = ProcessChildSaL(IkeMsg);
			break;

		case INFORMATIONAL:
		    DEBUG_LOG(_L("INFORMATION message received"));
			Status = ProcessInfoMsgL(IkeMsg);			
			break;

		default:
		    DEBUG_LOG(_L("UNKNOWN message received\n"));
			Status = EFalse;  // Negotiation object shall be released			
			break;
	}

	if ( !Status )
	{
	   if ( iDeleteIkeSA )
	   {
		  //
		  // Used IKE SA shall be deleted due the fatal error occurred.
		  //
		   iDeleteIkeSA = EFalse;
		   iIkeV2PlugInSession.DeleteIkev2SA(iHdr.SaId());			   
		   BuildDeleteRequestL(NULL);
	   }
	   else
	   {	   
	       CheckNotifyCodeL(IkeMsg);
	   }	  
	}			
	CleanupStack::PopAndDestroy(IkeMsg);	
}

void CIkev2Negotiation::ProcessAcquireL(const TPfkeyMessage &aPfkeyMsg)
    {
	//
	// Process received PFKEY Acquire primitive
	// There is now the following possibilities:
	// -- There already exists an IKE SA so new IPSEC SA is negotiated
	//    using IKE_CHILD_SA exchange
	// -- The is no IKE SA yet.
	//    IPSEC SA can be negotiated concatenated during IKE_AUTH.	
	//    If Virtual IP is specified, the CP payload is used to get
	//    that virtual IP address. 
	//
	CIkev2Acquire* Acquire = CIkev2Acquire::NewL(aPfkeyMsg, iIkeV2PlugInSession.GetSAId(), 
	    GetLocalAddr(),
		Ikev2Proposal::GetDHGroup(iHdr.iIkeData->iGroupDesc_II), ImplicitChildSa());	

	if ( iState == KStateIdle )
	    {
		CleanupStack::PushL(Acquire);				 								
		LoadEapPluginL();		
		iHdr.iInitiator = ETrue;
		GetNonceDataL(ETrue);  // For IKE SA		
		if ( iHdr.iIkeData->iUseInternalAddr )
		    {
		    CArrayFix<TIkeV2TrafficSelector>* TsI = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(1);
		    CleanupStack::PushL(TsI);
		    
		    TInetAddr StartIp;
			TInetAddr EndIp;	
			StartIp.SetAddress(KInetAddrNone);    // 0.0.0.0
			StartIp.SetPort(0);
			EndIp.SetAddress(KInetAddrAll);	      // 255.255.255.255
			EndIp.SetPort(0xffff);
			
			TIkeV2TrafficSelector ts(StartIp, EndIp, 
                                     aPfkeyMsg.iDstAddr.iExt->sadb_address_proto);
			TsI->AppendL(ts);			
			CleanupStack::Pop(TsI);
			Acquire->ReplaceTS_i(TsI);						
			Acquire->SetVirtualIp();						     						
		    }
		
		if ( InitPkiServiceL() )
		    {
		    // Store Acquire to wait PKI service init
			iChildSaRequest = Acquire;
			CleanupStack::Pop(Acquire); 			 							
			return;
		    }
		CleanupStack::Pop(Acquire);	
	    }
	CIkev2Acquire::Link(Acquire, GetAcquireQue());
	GetIpsecSPI(Acquire);
    }


void CIkev2Negotiation::ProcessExpireL(const TPfkeyMessage &aPfkeyMsg)
    {
	//
	// Process received PFKEY Expire primitive
	// Try to find first IPSEC SA data from the "parent" IKE SA and set
	// inbound SA to zero in TIpsecSAData 
	//
    TPtrC8 spi(reinterpret_cast<const TUint8*>(&aPfkeyMsg.iSa.iExt->sadb_sa_spi),
               sizeof(aPfkeyMsg.iSa.iExt->sadb_sa_spi));    
    
	TIkeV2IpsecSAData* SaData = 
                iIkeV2PlugInSession.FindIpsecSAData(iHdr.SaId(), spi, ETrue);
	if ( !SaData )
	    {
	    DEBUG_LOG(_L("PFKEY Expire received but no SA data found, stop negotiation"));    	

	    iStopped = ETrue;	
	    return;	
	    }	
	SaData->iSPI_In.Zero();	
	CIkev2Expire* Expire = CIkev2Expire::NewL(aPfkeyMsg);	
	CIkev2Expire::Link(Expire, GetExpireQue());
	
	ContinueIkeNegotiationL();
}

void CIkev2Negotiation::StartIpsecSaRekeyingL(const TPfkeyMessage &aPfkeyMsg)
{

    TPtrC8 spi(reinterpret_cast<const TUint8*>(&aPfkeyMsg.iSa.iExt->sadb_sa_spi),
               sizeof(aPfkeyMsg.iSa.iExt->sadb_sa_spi));    
    TIkeV2IpsecSAData* SaData = 
                iIkeV2PlugInSession.FindIpsecSAData(iHdr.SaId(), spi, ETrue);
	if ( !SaData )
	{
	    DEBUG_LOG(_L("No IPSec SA data found, stop rekeying"));    	
	    iStopped = ETrue;	
	    return;	
    }

	iStopped = ETrue;  
    
    CArrayFix<TIkeV2TrafficSelector>* tsIArray = SaData->iRekeyData->TsIL();
    CleanupStack::PushL(tsIArray);

    CArrayFix<TIkeV2TrafficSelector>* tsRArray = SaData->iRekeyData->TsRL();
    CleanupStack::PushL(tsRArray);
    
    __ASSERT_DEBUG(tsIArray->Count() > 0, User::Invariant());
    __ASSERT_DEBUG(tsRArray->Count() > 0, User::Invariant());

    TIkeV2TrafficSelector tsI = (*tsIArray)[0];
    TIkeV2TrafficSelector tsR = (*tsRArray)[0];

    CleanupStack::PopAndDestroy(tsRArray);
    CleanupStack::PopAndDestroy(tsIArray);
    
    
    TInetAddr localSelector;
    TInetAddr localSelectorMask;
    
    TInetAddr remoteSelector;
    TInetAddr remoteSelectorMask;

    if (iHdr.iInitiator)
        {
        localSelector = tsI.StartingAddress();
        localSelectorMask = tsI.Mask();
        
        remoteSelector = tsR.StartingAddress();
        remoteSelectorMask = tsR.Mask(); 
        }
    else
        {
        localSelector = tsR.StartingAddress();
        localSelectorMask = tsR.Mask();
        
        remoteSelector = tsI.StartingAddress();
        remoteSelectorMask = tsI.Mask();        
        }
    
    CIpsecSaSpecList* SaList = iIkeV2PlugInSession.GetIPsecSaSpecListL(localSelector, localSelectorMask, //local address/port info
                                                                       remoteSelector, remoteSelectorMask,
                                                                       aPfkeyMsg.iDstAddr.iExt->sadb_address_proto);

    
    CleanupStack::PushL(SaList);
    __ASSERT_DEBUG(SaList != NULL, User::Invariant());
    __ASSERT_DEBUG(SaList->Count() > 0, User::Invariant());
    iStopped = EFalse;  
    
    const TIpsecSaSpec& saSpec = SaList->At(0);
	    
	CIkev2Acquire* Acquire = CIkev2Acquire::NewL(aPfkeyMsg, iIkeV2PlugInSession.GetSAId(), GetLocalAddr(),
									   Ikev2Proposal::GetDHGroup(iHdr.iIkeData->iGroupDesc_II), ImplicitChildSa(), 
									   &saSpec, SaData->iRekeyData);	
	CleanupStack::PopAndDestroy(SaList); //SaList
	
	Acquire->SetSPI_ToBeRekeyed(spi);

	if ( iState == KStateIdle )
	{
		CleanupStack::PushL(Acquire);				 								
		LoadEapPluginL();		
		iHdr.iInitiator = ETrue;
		GetNonceDataL(ETrue);  // For IKE SA		
		if ( iHdr.iIkeData->iUseInternalAddr )
		{			
			CArrayFix<TIkeV2TrafficSelector>* TsI = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(1);
			CleanupStack::PushL(TsI);

			TInetAddr StartIp;
            TInetAddr EndIp;    
            StartIp.SetAddress(KInetAddrNone);    // 0.0.0.0
            StartIp.SetPort(0);
            EndIp.SetAddress(KInetAddrAll);       // 255.255.255.255
            EndIp.SetPort(0xffff);
			
            TIkeV2TrafficSelector ts(StartIp, EndIp, 
                                     aPfkeyMsg.iDstAddr.iExt->sadb_address_proto);
            TsI->AppendL(ts);
			Acquire->ReplaceTS_i(TsI);			
			CleanupStack::Pop(TsI);
			Acquire->SetVirtualIp();
        }

		if ( InitPkiServiceL() )
		{
			iChildSaRequest = Acquire;  // Store Acquire to wait PKI service init
			CleanupStack::Pop(Acquire); 			 							
			return;
		}
		CleanupStack::Pop(Acquire); 	
	}
	CIkev2Acquire::Link(Acquire, GetAcquireQue());
	GetIpsecSPI(Acquire);
}

void CIkev2Negotiation::GetIpsecSPI(CIkev2Acquire* aAcquire)
    {
    ASSERT(aAcquire);
	//
	// Get SPI for inbound SA with PFKEY GETSPI primitive
	//
	TInetAddr DstAddr;
	if ( aAcquire->SrcSpecific() )
		 DstAddr = iHdr.iLocalAddr;
	else DstAddr.Init(0); 
	DstAddr.SetPort(0);	
	TInetAddr SrcAddr = iHdr.iRemoteAddr;
	SrcAddr.SetPort(0);

	iSpiRetriever->GetIpsecSaSpi(aAcquire->Id(),
                                 aAcquire->IpsecProtocol(),
                                 SrcAddr, DstAddr);
    }


void CIkev2Negotiation::IpsecSaSpiRetrieved(TUint32 aSpiRequestId, 
                                            TInt aStatus, 
                                            TUint32 aSpi)
    {
    if (aStatus == KErrNone)
        {
        TRAP(aStatus, IpsecSaSpiRetrievedL(aSpiRequestId, aSpi));
        }
    
    if (aStatus != KErrNone)
        {
        //Leave that we have not been able to handle
        //above layers. We bail out and report error.
        iIkeV2PlugInSession.DeleteIkev2SA(iHdr.SaId());                         
        iIkeV2PlugInSession.IkeSaDeleted(aStatus); 
        delete this;
        }
    }

void CIkev2Negotiation::CancelOperation()
    {
    if ( iTimer != NULL )
        {
        iTimer->Cancel();
        }
    if ( iSpiRetriever != NULL )
        {
        iSpiRetriever->Cancel();
        }
    }

void CIkev2Negotiation::IpsecSaSpiRetrievedL(TUint32 aSpiRequestId, TUint32 aSpi)
    {
    DEBUG_LOG(_L("CIkev2Negotiation::SpiRetrievedL"));
    
	//
	// Ipsec SPI received. Find an Acquire object for received SPI and
	// save SPI into found object.
	//
	CIkev2Acquire* Acquire = CIkev2Acquire::Find(aSpiRequestId, GetAcquireQue());
	__ASSERT_DEBUG(Acquire, User::Invariant());

	TPtrC8 spiPtr(reinterpret_cast<TUint8*>(&aSpi), sizeof(aSpi));
	Acquire->SetSPI_In(spiPtr);
	//
	// Ipsec SPI received. Find an Acquire object for received SPI and
	// save SPI into found object.
	//	
    ContinueIkeNegotiationL();
    }

void CIkev2Negotiation::ContinueIkeNegotiationL()
{
	//
	// This method takes actions according to current state (iState) of
	// the negotiation.     
	//
	CIkev2Acquire* Acquire;
	CIkev2Expire*  Expire;

	switch ( iState )
	{
    case KStateIdle:
         //
         // Start IKE_SA_INIT exchange
         //
         StartIkeSaInitL();			 
         break;

    case KStateIkeSaAuthWaitSpi:
         {			    			 
         //
         // Complete IKE_AUTH exchange (with concatenated Child SA)
         //
         iChildSaRequest = CIkev2Acquire::GetNext(GetAcquireQue(), ETrue);
                     
         DEBUG_LOG(_L("CIkev2Negotiation::ContinueIkeNegotiationL"));
         DEBUG_LOG1(_L("iChildSaRequest is %d"), (TInt)iChildSaRequest);
         
         SendIkeAuthMessageL();			 
         }
         break;
         
    case KStateIkeSaCompleted:
         //
         // There is no activity going on this negotiation
         // If there is something in request queues start process
         // them in the following order:
         // -- Check if there is something in info queue (NIY)
         // -- Check if there is something in expire queue (NIY)
         // -- Check if there is ready responses in acquire queue
         // -- Check if there is ready request in acquire queue			 
         //
         Expire = CIkev2Expire::GetNext(GetExpireQue());
         if ( Expire )
         {
            CleanupStack::PushL(Expire);				 
            BuildDeleteRequestL(Expire);
            CleanupStack::PopAndDestroy(Expire);								
         }	
         else 
         {
            Acquire = CIkev2Acquire::GetNext(GetAcquireQue(), ETrue);
            if  ( Acquire )
            {
                BuildChildSAMessageL(Acquire, EFalse);
            }
            else
            {
                Acquire = CIkev2Acquire::GetNext(GetAcquireQue(), EFalse);				 
                BuildChildSAMessageL(Acquire, ETrue);
            }
         }
         break;
    
    default:
         break;
	}		
}	

void CIkev2Negotiation::StartIkeSaInitL()
    {
	//
	// Create Initiator SPI for the new IKE SA
	//
	CreateIkeSPI(iHdr.SpiI());

	//
	// Get required peer identity from policy (IDr)
	//
	iRemoteIdentity = Ikev2Proposal::GetRemoteIdentityL(iHdr.iIkeData);
	
	__ASSERT_ALWAYS(iHdr.iInitiator, User::Invariant());
	//
	// Build and send the first IKE_SA_INIT message (request)
	// HDR, SAi1, KEi, Ni, N[NAT_SRC], N[NAT_DST]
	//
	CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(),
                                                iHdr.SpiR(),
                                                IKE_SA_INIT,
                                                iHdr.iInitiator,
                                                EFalse,
                                                iHdr.NextRequestId(),
                                                iDebug); 
	CleanupStack::PushL(ikeMsg);
	
    HBufC8* saBfr = Ikev2Proposal::FromPolicyToProposaL(iHdr, iSPI_Rekey, iDHGroupGuess);
    CleanupStack::PushL(saBfr);
	ikeMsg->AppendSaPayloadL(*saBfr);
	CleanupStack::Pop(saBfr);
	SetProposedSa(saBfr);

    AppendKEPayloadL(*ikeMsg, iHdr.iDHGroup);
    ikeMsg->AppendNoncePayloadL(*iNonce_I);
	if ( !iHdr.iIkeData->iUseNatProbing )
	    {
	    delete iNatNotify;
	    iNatNotify = NULL;
	    
	    TInetAddr LocalIp;	
	    if ( iHdr.iIkeData->iUseMobIke )
		    LocalIp.SetAddress(KInetAddrNone);
	    else LocalIp = iHdr.iLocalAddr; 	   
	    iNatNotify = CIkev2NatT::NewL(
	        LocalIp, iHdr.iRemoteAddr, IKE_PORT, ikeMsg->InitiatorSpi(), ikeMsg->ResponderSpi());
	    	    	   
	    ikeMsg->AppendNotifyPayloadL(IKEV2_PROTOCOL, KZeroDesc, NAT_DETECTION_SOURCE_IP, 
                                     iNatNotify->SourceNofify());
	    ikeMsg->AppendNotifyPayloadL(IKEV2_PROTOCOL, KZeroDesc, NAT_DETECTION_DESTINATION_IP, 
                                     iNatNotify->DestinNofify());		  
	    }	
	CleanupStack::Pop(ikeMsg);
	
	SendIkeMsgL(ikeMsg);	
    iState = KStateIkeSaInitRequest;
    }

void CIkev2Negotiation::SendIkeAuthMessageL()
{
	//
	// Build and send IKE_AUTH message
	// IKE_AUTH message sent by the initiator is the following:
	//  HDR(A,B), SK {IDi, [CERT] [CERTREQ], [IDr], [AUTH], [CP], [SAi2,
	//  TSi, TSr]} 
	// IKE_AUTH message sent by the responder is the following:
	//  HDR(A,B), SK {IDr, [CERT,] AUTH, [CP], [SAr2, TSi, TSr]}
	// CERT and CERTREQ payloads are added into message on when needed.
	// AUTH payload is missing from initiators message when EAP in use.
	// IPSEC SA:s are not always negotiated within IKE_AUTH messages.
	// In this sitution SAx2, TSi and TSr payloads shall be missing.
	// CP payload is used the Virtual IP address (secure network DNS
	// IP:s) for client Virtual IP interface.
	// Initiators CP payload shall contain CFG_REQUEST and and
	// responders CP payload CFG_REPLY.
	// When CP payload is used IKE_AUTH message MUST always contain
	// IPSEC SA negotiation payloads within. 
	// In case INITIAL_CONTACT is used, the first IKE_AUTH request on given 
	// IKE SA contains INITIAL_CONTACT Notification Payload that is added in
	// the end of the IKE_AUTH message.
	//

	if ( !iLocalIdentity )
	{
	   //
	   // Own identity does not exists yet. Do not build IKE_AUTH
	   // message now
	   //
	   iState = KStateIkeWaitingId;
	   return;
	}
	
	TUint32 MsgId = (iHdr.iInitiator) ? iHdr.NextRequestId() : iHdr.ExpectedRequestId();	
    CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(),
                                                iHdr.SpiR(),
                                                IKE_AUTH,
                                                iHdr.iInitiator,
                                                !iHdr.iInitiator, //Initiator sends only requests
                                                MsgId,
                                                iDebug); 
    CleanupStack::PushL(ikeMsg);
	
    ikeMsg->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);

    __ASSERT_DEBUG(iLocalIdentity != NULL, User::Invariant());
    if (iHdr.iInitiator)
        {
        ikeMsg->AppendIdiPayloadL(*iLocalIdentity);
        }
    else
        {        
        ikeMsg->AppendIdrPayloadL(*iLocalIdentity);
        }

	if ( iPkiService &&
	     iPkiService->UserCertificateData().Length() > 0)
	    {        
        ikeMsg->AppendCertPayloadL(iPkiService->UserCertificateData());
        }
	
	if ( iPkiService &&
	     iPkiService->I2CertificateData().Length() > 0)
	    {
	    ikeMsg->AppendCertPayloadL(iPkiService->I2CertificateData());
	    }
	
	if ( iPkiService &&
	     iPkiService->I1CertificateData().Length() > 0)
	    {
	    ikeMsg->AppendCertPayloadL(iPkiService->I1CertificateData());
	    }
	  
	if ( iHdr.iInitiator && iHdr.iIkeData->iInitialContact )
        {
        ikeMsg->AppendNotifyPayloadL(IKEV2_PROT_NONE, KZeroDesc, INITIAL_CONTACT, KZeroDesc);
        }


    if ( iHdr.iInitiator && iPkiService != NULL &&  iPkiService->CaList().Count() > 0)  
       {	    
       ikeMsg->AppendCertReqPayloadL(iPkiService->CaList());
       }

	if ( iHdr.iInitiator && iRemoteIdentity )
	    {
	    //
	    // Add IDr payload 
	    //
	    ikeMsg->AppendIdrPayloadL(*iRemoteIdentity);
	    }	
    if ( !iEapPlugin )
	    {	 
        HBufC8* authData = NULL; 
	    if ( iHdr.iInitiator )
	        {
	        authData = SignAuthDataL(*iAuthMsgInit, (TUint8)iHdr.iAuthMethod);
	        }
	    else 
	        {
	        authData = SignAuthDataL(*iAuthMsgResp, (TUint8)iHdr.iAuthMethod);
	        }
	    CleanupStack::PushL(authData);
	    ikeMsg->AppendAuthPayloadL(iHdr.iAuthMethod, *authData);	   
	    CleanupStack::PopAndDestroy(authData);
	    }
	if ( iHdr.iIkeData->iUseMobIke )
	    {
	    //
	    // Add MOBIKE_SUPPORTED notify payload
	    //
	    ikeMsg->AppendNotifyPayloadL(IKEV2_PROT_NONE,
                                     KZeroDesc,
                                     MOBIKE_SUPPORTED,
                                     KZeroDesc);
	    }	


	//
	// Add Child SA and Traffic selector payloads into IKE_AUTH message
	// if required
	//
	if ( iChildSaRequest )
	{
		iChild.iSPI_In = iChildSaRequest->SPI_In();
		iChildSaRequest->AddIpsecSpiToSa(iChild.iSPI_In);
		if ( iChildSaRequest->ForVirtualIp() )
		    {
		    //
		    // As Virtual Ip from peer SGW using Config Payload
		    // Build CP request data by constructing CIkev2Config Object
		    //
		    if ( !iConfigMode )
		        iConfigMode = CIkev2Config::NewL(iChildSaRequest);

            ikeMsg->AppendConfigurationPayloadL(iConfigMode->CpType(), iConfigMode->Cp());
		    }	
		ikeMsg->AppendSaPayloadL(*iChildSaRequest->SA());
	 
		ikeMsg->AppendTsiPayloadL(iChildSaRequest->TS_i());
		ikeMsg->AppendTsrPayloadL(iChildSaRequest->TS_r());
	}
	
	CleanupStack::Pop(ikeMsg);
	SendIkeMsgL(ikeMsg);
	
	if ( iHdr.iInitiator )
	{	
	   if ( iEapPlugin )
	        iState = KStateIkeSaEapStarted;			   
	   else iState = KStateIkeSaAuthRequest;	
	}
	else 
	{    
       iState = KStateIkeSaCompleted;
       IkeSaCompletedL();
	}

}

void CIkev2Negotiation::SendKeepAliveMsgL()
    {
    CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(), 
                                                iHdr.SpiR(), 
                                                INFORMATIONAL, 
                                                iHdr.iInitiator,
                                                EFalse,
                                                iHdr.NextRequestId(), 
                                                iDebug);
    
    ikeMsg->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);            
    SendIkeMsgL(ikeMsg);
    iState = KStateIkeInfoRequest;
    
	DEBUG_LOG(_L("CIkev2Negotiation::SendKeepAliveMsgL"));
    }


void CIkev2Negotiation::BuildChildSAMessageL(
    CIkev2Acquire* aAcquire, TBool aInitiator)
    {
    ASSERT(aAcquire);
	//
	// Build and send CREATE_CHILD_SA message
	// CREATE_CHILD_SA request message sent is the following:
	//  HDR(A,B), SK {[N], SA, Ni, [KEi], [TSi, TSr]}
	// CREATE_CHILD_SA response message is the following:
	//  HDR(A,B), SK {SA, Nr, [KEi], [TSi, TSr]}
	//
	iChild.iSPI_In  = aAcquire->SPI_In();	
	iChildSaRequest = aAcquire;
	//TPayloadIkev2* PreviousPayload;
	//TPayloadIkev2* EncrPayload;
	GetNonceDataL(aInitiator);
	aAcquire->AddIpsecSpiToSa(aAcquire->SPI_In());
		
	TUint32 MsgId = (aInitiator) ? iHdr.NextRequestId() : iHdr.ExpectedRequestId();
	
	CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(),
                                                iHdr.SpiR(),
                                                CREATE_CHILD_SA,
                                                iHdr.iInitiator,
                                                !aInitiator,
                                                MsgId,
                                                iDebug);
	
    ikeMsg->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);            
	
	if (aInitiator && aAcquire->SPI_ToBeRekeyed().Length() > 0)
        {         
        ikeMsg->AppendNotifyPayloadL(aAcquire->IpsecProtocol(),
                                     aAcquire->SPI_ToBeRekeyed(),
                                     REKEY_SA, KZeroDesc);        
        }
	ikeMsg->AppendSaPayloadL(*aAcquire->SA());
	
	if ( aInitiator )
	    {
	    ikeMsg->AppendNoncePayloadL(*iNonce_I);
	    }
	else 
	    {
	    ikeMsg->AppendNoncePayloadL(*iNonce_R);	
	    }
	
	delete iDHKeys;   // Delete old DH object 
	iDHKeys = NULL;
	if ( aAcquire->DHGroup() )
	    {	    
	    AppendKEPayloadL(*ikeMsg, aAcquire->DHGroup());
	    }
	ikeMsg->AppendTsiPayloadL(aAcquire->TS_i());
	ikeMsg->AppendTsrPayloadL(aAcquire->TS_r());
	      
	SendIkeMsgL(ikeMsg);

	if ( aInitiator	)
	{	
	   iState = KStateIkeChildSARequest;	   
    }
	else
	{
        if (iDHKeys && iDHPublicPeer)
            {
            HBufC8* g_ir = iDHKeys->ComputeAgreedKeyL(iDHPublicPeer->Des());
            CleanupStack::PushL(g_ir);
        
            iChild.GenerateIpsecKeysL(iHdr.iSK_d, *g_ir, 
                                      *iNonce_I, *iNonce_R, iHdr.iPRFAlg);
                
            g_ir->Des().FillZ(); // Wipe out shared secret value from buffer
            CleanupStack::PopAndDestroy();  //g_ir
            }
        else
            {
            iChild.GenerateIpsecKeysL(iHdr.iSK_d, KZeroDesc, 
                                      *iNonce_I, *iNonce_R, iHdr.iPRFAlg);            
            }
        
	   IpsecSANegotiatedL();
	   iState = KStateIkeChildSAResponse;
	}	
}

void CIkev2Negotiation::BuildDeleteRequestL(CIkev2Expire* aExpire)
{
	//
	//  Build and send INFORMATIONAL exchange message with delete payload
	//  HDR(A,B), SK {D}
	//  If CIkev2Expire object defined, build a Delete payload with Ipsec
	//  SPI and protocl stored into CIkev2Expire object. If no CIkev2Expire build
	//  Delete payload for IKE SA.
	//    
    CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(), 
                                                iHdr.SpiR(), 
                                                INFORMATIONAL, 
                                                iHdr.iInitiator,
                                                EFalse,
                                                iHdr.NextRequestId(), 
                                                iDebug);
    
    ikeMsg->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);            

    CDesC8Array* spiArray = new (ELeave) CDesC8ArrayFlat(2);
    CleanupStack::PushL(spiArray);
	if ( aExpire )
	    {	    	    
	    spiArray->AppendL(aExpire->SPI());
	    ikeMsg->AppendDeletePayloadL(aExpire->Protocol(), *spiArray);	    	        
	    }
	else
	    {	    
	    ikeMsg->AppendDeletePayloadL(IKEV2_PROTOCOL, *spiArray);
	    }
	CleanupStack::PopAndDestroy(spiArray);
	
	SendIkeMsgL(ikeMsg);
    DEBUG_LOG(_L("CIkev2Negotiation::BuildDeleteRequestL() Delete send OK"));
    
    if ( aExpire ) 
        {
        iState = KStateChildDeleteRequest;
        }
    else 
        {
        iState = KStateIkeDeleteRequest;
        }
}

void CIkev2Negotiation::BuildIkeSaRekeyMsgL(TBool aRequest)
{
	//
	//  Build and send CHILD_SA exchange message which contains IKE SA
	//  rekey message (either request or response)
	//  HDR, SA, Nonce, KE	
	//
	HBufC8* SaBfr;		
	HBufC8* Nonce;	
	TUint32 MsgId;
	
	if ( aRequest )
	    {
	    // Get a new SA Id for rekeyed IKE SA		   
		iSAid_Rekey = iIkeV2PlugInSession.GetSAId(); 
		CreateIkeSPI(iSPI_Rekey, ETrue);
		SaBfr = Ikev2Proposal::FromPolicyToProposaL(iHdr, iSPI_Rekey, iDHGroupGuess, ETrue);
		SetProposedSa(SaBfr); // Save SA payload buffer
		GetNonceDataL(ETrue);
		Nonce = iNonce_I;
		MsgId = iHdr.NextRequestId();
	    }
	else
	    {
		SaBfr = PeekProposedSa();			
		Ikev2Proposal::ChangeSpiInProposal(SaBfr, iSPI_Rekey);
		GetNonceDataL(EFalse);
		Nonce = iNonce_R;
        MsgId = iHdr.ExpectedRequestId();
	    }	

	CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(), 
                                                iHdr.SpiR(), 
                                                CREATE_CHILD_SA, 
                                                iHdr.iInitiator, 
                                                !aRequest,
                                                MsgId, 
                                                iDebug);

    ikeMsg->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);            
    ikeMsg->AppendSaPayloadL(*SaBfr);
    ikeMsg->AppendNoncePayloadL(*Nonce);	    
    AppendKEPayloadL(*ikeMsg, iHdr.iDHGroup);
	     	
	SendIkeMsgL(ikeMsg);

	if ( aRequest )
	{	
       iState = KStateIkeSARekeyRequest;
	}
}

void CIkev2Negotiation::CheckNotifyCodeL(CIkev2Payloads* aIkeMsg)
    {
    ASSERT(aIkeMsg);
	//
	// Some error has occurred during incoming IKE message handling
	// Build an error response with specified Notify message type
	//
	TInt MsgType( GetNotifyCode() );
	
	if ( MsgType )
	    {
        //
        // Build and error response/request with Notify payload
        // If received message with error condition is a request
        // Notify payload is transmitted in the response IKE message
        // of ongoing exchange (with erronous request message id)
        // If received message with error conditions is a response
        // an informational exchange is initiated with Notify payload
        //
		CIkeV2Message* XmitHdr = NULL;
		TBool Response(aIkeMsg->GetIkeMsg()->GetFlags() & IKEV2_RESPONSE_MSG);		  
		if ( Response )
		    {
		    iState  = KStateIkeInfoRequest;
            TUint32 MsgId = aIkeMsg->GetIkeMsg()->GetMessageId();		  
		    XmitHdr = CIkeV2Message::NewL(iHdr.SpiI(), 
                                          iHdr.SpiR(),
                                          INFORMATIONAL, 
                                          iHdr.iInitiator,
                                          EFalse,
                                          MsgId,
                                          iDebug); 
		    		   
		    }
		else
		    {		    
            XmitHdr = CIkeV2Message::NewL(iHdr.SpiI(), 
                                          iHdr.SpiR(),
                                          aIkeMsg->GetIkeMsg()->GetExchange(), 
                                          iHdr.iInitiator,
                                          ETrue,
                                          iHdr.ExpectedRequestId(),
                                          iDebug); 
		    }
		
		if (aIkeMsg->Encrypted())
		    {
		    XmitHdr->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);            		    
		    }
		
		TInt notifyDataLength = 0;
		TUint8* notifyData = NotifyData(notifyDataLength);
		
		if (notifyDataLength == 0)
		    {
		    XmitHdr->AppendNotifyPayloadL(IKEV2_PROT_NONE, KZeroDesc, MsgType, KZeroDesc);
		    }
		else
		    {
		    TPtrC8 notifyDataPtrC(notifyData, notifyDataLength);
		    XmitHdr->AppendNotifyPayloadL(IKEV2_PROT_NONE, KZeroDesc, MsgType, notifyDataPtrC);
		    iNotifyDataLth = 0; //Reset notifydata
		    }
		
		SendIkeMsgL(XmitHdr);
		
		iEventLogger.LogEvent(MKmdEventLoggerIf::KLogError, R_VPN_MSG_SENT_ERROR_RESPONSE, 
		    MsgType, iHdr.iVpnIapId, &iHdr.iRemoteAddr);		
	    }	
    }


void CIkev2Negotiation::GetNatStatus(TBool aSupported, const TInetAddr& aRemote)
{
	//
	// Examine NAT discovery status (from iHdr.iNATFlags) and set
	// floated port usage indicator, if required.
	//
	if ( aSupported )
	{
		if ( iHdr.iNATFlags & (REMOTE_END_NAT + LOCAL_END_NAT) )
		{
			if ( iHdr.iNATFlags & REMOTE_END_NAT )
			{	
			   //
	           // Remote end is behind NAT. Save current source IP to be
	           // used as further destination address. 
			   // When remote and is behind NAT it is supposed that it
			   // must be pure mapping between public- and private IP
			   // addresses (remote NAPT is NOT supported) 
			   // 
			   DEBUG_LOG(_L("Remote end is behind NAT"));
			   iHdr.iDestinAddr = aRemote; // Remote end behind NAT, use current source IP as destin
		    }
			
			if ( iHdr.iNATFlags & LOCAL_END_NAT ) 
			    {
			    DEBUG_LOG(_L("NAT discovery result: Local end is behind NAT"));
			    }
			iHdr.iFloatedPort = ETrue;			
		    iHdr.iDestinAddr.SetPort(FLOATED_IKE_PORT);			
		}
		else
		{
		   if ( iHdr.iMobikeUsed )
		   {
			   iHdr.iFloatedPort = ETrue;			
			   iHdr.iDestinAddr.SetPort(FLOATED_IKE_PORT);			
		   }	   
		   DEBUG_LOG(_L("NAT discovery result: There is no NAT between negotiating ends"));
		}			   
	}
	else
    {
        DEBUG_LOG(_L("NAT discovery operation failed"));
    }		
}

void CIkev2Negotiation::CreateIkeSPI(TIkeSPI& aSPI, TBool aRekey)
{
	//
	// Create IKE SPI for local end.
	// The SPI value is created from the following "parameters" in
	// IKEv2 negotiation object:
	// - The first 4 octets of SPI value are the SAId (32 bit value)
	// - The last 4 octets of SPI contains "pseudo random" value:
	//   X = (SAId + negotiation object pointer) >> (SAId & 3)
	//
	TUint32 SpiValue1;
	TUint32 SpiValue2;
	if ( aRekey )
		 SpiValue1 = iSAid_Rekey;		
	else SpiValue1 = iHdr.SaId();		
	Mem::Copy((TUint8*)&SpiValue2, (TUint8*)this, 4);
	SpiValue2 = (SpiValue2 + SpiValue1) >> (SpiValue1 & 3);  
	PUT32(aSPI.Ptr(), SpiValue1);	
	PUT32((aSPI.Ptr() + 4), SpiValue2);
	aSPI.SetLength(IKEV2_SPI_SIZE);
}

void CIkev2Negotiation::LoadEapPluginL()
{
	//
	// If EAP configured in policy, construct EAP interface object to
	// communicate EAP ECOM plug-in
	// If consruction causes an error, stop negotiation request
	//
	iHdr.iEAPType = iHdr.iIkeData->iEAPProtocol;
	if ( !iEapPlugin && iHdr.iEAPType )
	{	
	   iEapPlugin = CIkev2EapIf::NewL(*this, (TUint8)iHdr.iEAPType, iHdr.iIkeData, iDebug);
	   TInt Status = iEapPlugin->Status();
	   if ( Status != KErrNone )
	   {
		  iStopped = ETrue;
	   }
	   else iEapPlugin->QueryIdentity();
	}									  
}

TBool CIkev2Negotiation::InitPkiServiceL()
{
    DEBUG_LOG(_L("-> CIkev2Negotiation::InitPkiServiceL"));
	//
	// If EAP configured in policy, construct EAP interface object to
	// communicate EAP ECOM plug-in
	// If consruction causes an error, return corresponding error code
	// to stop negotiation request
	//
	TBool Status = EFalse;
	if ( !iPkiService && Ikev2Proposal::PkiServiceNeeded(iHdr.iIkeData) )
	{
	   iPkiService = CIkeV2PkiService::NewL(*this, iDebug);
	   
	   if (iHdr.iIkeData->iCAList->Count() == 0)
    	   {
    	   User::Leave(KVpnErrInvalidCaCertFile);
    	   }
    	   
       iPkiService->InitIkeV2PkiService(iHdr.iIkeData);    	   
       iState = KStateIkeInitPkiService;
       Status = ETrue;
	}	

    DEBUG_LOG(_L("<- CIkev2Negotiation::InitPkiServiceL"));
	return Status;
}


void CIkev2Negotiation::SendIkeMsgL(CIkeV2Message* aMsg)
{
    ASSERT(aMsg);
    
    TPtrC8 encryptionKey;
    TPtrC8 integrityKey;
    if ( iHdr.iInitiator )
    {   
        encryptionKey.Set(iHdr.iSK_ei);
        integrityKey.Set(iHdr.iSK_ai);
    }
    else
    {
        encryptionKey.Set(iHdr.iSK_er);
        integrityKey.Set(iHdr.iSK_ar);    
    }   

    TInetAddr sourceAddr(iHdr.iLocalAddr);
    if (iHdr.iFloatedPort)
        {
        sourceAddr.SetPort(FLOATED_IKE_PORT);
        }
    else
        {
        sourceAddr.SetPort(IKE_PORT);
        }
    aMsg->PrepareIkeMessageDatagramL(iHdr.iEncrAlg, encryptionKey,
                                     iHdr.iIntegAlg, integrityKey, 
                                     sourceAddr, iHdr.iDestinAddr);
    iMessageSendQue.SendIkeMessageL(aMsg->IkeMessageDatagram(), iHdr.iFloatedPort);     
    	
	if (aMsg->Flags() & IKEV2_RESPONSE_MSG )
	{
        iHdr.SaveRespMsg(aMsg);
        iHdr.iRespRetryCount = 0;
	}
	else
	{
        iSendAttempt = 1;
        iTimer->Cancel();
        iTimer->IssueRequest(iSendAttempt);     // Start retry timer
    
        iHdr.SaveRequestMsg(aMsg);
	}
}

void CIkev2Negotiation::RetransmitRequest()
    {
    TRAPD(err, DoRetransmitL(EFalse));
    if ( err != KErrNone )
        {
        iIkeV2PlugInSession.IkeSaDeleted( err );
        }
    }

void CIkev2Negotiation::DoRetransmitL(TBool aResponse)
{	
	if ( aResponse )
	{
		//
		// Peer has retransmitted a request, retransmit last response
		// message saved.  
		//
		if ( iHdr.iLastResponse && (iHdr.iRespRetryCount <= KMaxSendAttemps) )
		{
			iHdr.iRespRetryCount ++;
			//iHdr.iLastResponse = NULL;
			DEBUG_LOG3(_L("IKE response message rexmitted on SAId: %d , Retry: %d  , State: %d"), iHdr.SaId(), iHdr.iRespRetryCount, iState );

			iMessageSendQue.SendIkeMessageL(iHdr.iLastResponse->IkeMessageDatagram(), 
                                            iHdr.iFloatedPort);
		}
		else iStopped = ETrue;
	}	
	else
	{	
	    //
	    // No response received to a transmitted IKE request message
	    // Retransmit message if retry count not exhausted
	    //	
	    DEBUG_LOG(_L("No response received for transmitted IKE request."));

	    iSendAttempt++;            
	    iMessageSendQue.CancelSend(iHdr.iLastRequest->IkeMessageDatagram());
				
		if ( iSendAttempt <= KMaxSendAttemps )
		{	   		  
           DEBUG_LOG3(_L("IKE Message rexmitted on SAId: %d , State: %d , Retry: %d"),iHdr.SaId(), iState, iSendAttempt );		   
           iMessageSendQue.SendIkeMessageL(iHdr.iLastRequest->IkeMessageDatagram(), 
                                           iHdr.iFloatedPort);
	       iTimer->IssueRequest(iSendAttempt); 	// Restart retry timer
		}
		else
		    {
		    DEBUG_LOG3(_L("Transmit retry count reached on SAId: %d , State: %d , retry: %d"),iHdr.SaId(), iState, iSendAttempt );
		    if ( iState < KStateIkeSaCompleted )
		        {
			    IkeSaFailed(KKmdIkeNegotFailed);  // IKE SA negotiation going 
		        }
		    else
		       {
			   iIkeV2PlugInSession.DeleteIkev2SA(iHdr.SaId());			   			   
			   iIkeV2PlugInSession.IkeSaDeleted(KKmdIkeNoResponseErr);	 //IKE SA deletion going
			   delete this;
		       }	    
		    }	
	    }	
    }


void CIkev2Negotiation::IkeV2PkiInitCompleteL(TInt aStatus)
    {

    DEBUG_LOG(_L("CIkev2Negotiation::IkeV2PkiInitCompleteL"));
	//
	// The implementation for class MPkiServiceComplete virtual function
	// This method is called when a PKI service operation is
	// completed.
	//
	
	__ASSERT_ALWAYS( iPkiService != NULL, User::Invariant());            	    
    __ASSERT_ALWAYS(iState == KStateIkeInitPkiService, User::Invariant());

    switch(aStatus)
        {
        case KErrNone:
            //
            // PKI service object has been constructed
            // Start IKE_SA_INIT exchange
            //
            iState = KStateIdle;
            if ( iChildSaRequest )
                {	   
                CIkev2Acquire* Acquire = iChildSaRequest;
                iChildSaRequest = NULL;				   
                CIkev2Acquire::Link(Acquire, GetAcquireQue());
                GetIpsecSPI(Acquire);
                }
            else if ( iSavedSaInit )
                {
                TPtr8 IkeMsg(iSavedSaInit->Des());
                const ThdrISAKMP* IkeMessage = ThdrISAKMP::Ptr(IkeMsg);
                ProcessIkeMessageL(*IkeMessage, iHdr.iRemoteAddr, IKE_PORT);
                if ( Stopped() )
                   delete this;
                }	   
            break;                
        case KErrNotFound:       
            DEBUG_LOG(_L("IKEv2 CA certificate retrieve failed. Certificate not found"));
            IkeSaFailed(KVpnErrInvalidCaCertFile);
            break;
        default:                
            {
            DEBUG_LOG1(_L("IKEv2 CA certificate retrieve failed (%d)"), aStatus);
            IkeSaFailed(aStatus);
            }               		   
            break;
        }
        
    }
    

void CIkev2Negotiation::SendEapDataL(HBufC8* aEapData)
{
	//
	// Send an IKE containing an EAP payload (within Encrypted Payload)
	// The entire EAP payload data is provided in aEapData buffer
	//
    CleanupStack::PushL(aEapData);
	if ( iState == KStateIkeSaEapGoing )
	{
        __ASSERT_DEBUG(iHdr.iInitiator, User::Invariant());
	
        CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(), iHdr.SpiR(),
                                                    IKE_AUTH, 
                                                    iHdr.iInitiator,
                                                    EFalse,
                                                    iHdr.NextRequestId(), 
                                                    iDebug);
        CleanupStack::PushL(ikeMsg);               
        ikeMsg->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);
        ikeMsg->AppendEapPayloadL(*aEapData);
        CleanupStack::Pop(ikeMsg);
		SendIkeMsgL(ikeMsg);
	}	
	CleanupStack::PopAndDestroy(aEapData);
}

void CIkev2Negotiation::EapEventL(TInt aEvent)
    {
    // See whether the object is accepting any events
    // (it is, by default, but will not take events during destruction phase)
    if (!iProcessEvents) 
    {
        return;
    }
	//
	// An event idicated by the EAP plugin process event according to
	// event type
	//
	switch ( aEvent )
	    {
		case KEapEventSuccess:
			if ( (iState == KStateIkeSaEapGoing) || (iState == KStateIkeSaEapStarted) )
			    {			    
			    //
			    // EAP auhtentication succeeded.
			    // Build IKE message HDR, SK {AUTH}
			    //
			    __ASSERT_DEBUG( iHdr.iInitiator, User::Invariant());
			    
			    CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(), iHdr.SpiR(),
                                                            IKE_AUTH, 
                                                            iHdr.iInitiator,
                                                            EFalse,
                                                            iHdr.NextRequestId(), 
                                                            iDebug);
			    CleanupStack::PushL(ikeMsg);               
                ikeMsg->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);
								
		        
		        HBufC8* authData = SignAuthDataL(*iAuthMsgInit, (TUint8)iHdr.iAuthMethod);	           
		        CleanupStack::PushL(authData);
		        ikeMsg->AppendAuthPayloadL(iHdr.iAuthMethod, *authData);       
		        CleanupStack::PopAndDestroy(authData);				

				CleanupStack::Pop(ikeMsg);
				SendIkeMsgL(ikeMsg);
				iState = KStateIkeSaAuthRequest;
				iEapCompleted = ETrue;				
			    } 
			break;

		case KEapEventGetIdentity:
			GetOwnIdentityL(ETrue);  // Gets the Identity from EAP plugin			
			if ( iState == KStateIkeWaitingId )
			    {
			    //
			    // Identity data provided by the EAP plug-in
			    // Complete local signed data and send the first
			    // IKE_AUTH message
			    //
			    AddIdToSignedDataL(ETrue, iAuthMsgInit, iLocalIdentity->PayloadData()); 
			    SendIkeAuthMessageL();			    
			    }	
			break;

		case KEapEventGetPSK:
			if ( iState == KStateIkeSaEapGoing )
			    {
			    //
			    // Preshared key provided by the EAP plug-in
			    // Get key data and link it into negotiation object
			    //
				iPresharedKey = iEapPlugin->MSK();
			    }	
			break;

		default:  // = KEapEventFailed     
		    //
		    // EAP authentication is failed. Stop negotiation
		    //
		    IkeSaFailed(KKmdIkeAuthFailedErr);  // IKE SA negotiation going 
			break;
	    }	
    }

TBool CIkev2Negotiation::ProcessIkeSaInitL(CIkev2Payloads* aIkeMsg, const TInetAddr& aRemote)
{
    ASSERT(aIkeMsg);
	//
	// Process IKE message of exchange type IKE_SA_INIT
	//
	ThdrISAKMP* IkeHdr = aIkeMsg->GetIkeMsg();  // IKE Message fixed header
	TBool   Response   = IkeHdr->GetFlags() & IKEV2_RESPONSE_MSG;
	TBool   Initiator  = IkeHdr->GetFlags() & IKEV2_INITIATOR;
	TUint32 MsgId      = IkeHdr->GetMessageId();
			
	if ( iHdr.iInitiator )
	{
		if ( Initiator ) {
			DEBUG_LOG1(_L("IKEv2 Message with Orig_Init-bit in wrong state: %d"), iState);
			return ETrue;		
		}	
		if ( !Response )
		{
			DEBUG_LOG1(_L("IKEv2 Message is not response; state: %d"), iState);
			return ETrue;		
		}
		if ( MsgId != iHdr.ExpectedResponseId() )
		{
			DEBUG_LOG1(_L("Wrong message id in response; state: %d"), iState);
			return ETrue;		
		}
		
		if (iState == KStateIkeSaInitRequest)
		{
            //record responder SPI
            aIkeMsg->GetIkeMsg()->GetSPI_R(iHdr.SpiR());			    
            
            //
            // Received message should be a response to a
            // IKE_SA_INIT request transmitted.
            //
            if (IkeHdr->GetPayload() == IKEV2_PAYLOAD_NOTIF)
            {
                return ProcessNotifyPayloadsL(*aIkeMsg->iNotifs, EFalse, IKE_SA_INIT);		
            }	
            //
            // Response message should be format:
            // HDR(A,B), SAr1, KEr, Nr, [CERTREQ]
            //
            if ( !CheckPayloadsOrder(aIkeMsg, IKE_SA_INIT, ETrue) )
            {
                DEBUG_LOG1(_L("Erroneous IKE_SA_INIT response: %d"), iState);
                return EFalse;
            }
            if ( !Ikev2Proposal::VerifySaResponseL(iHdr, iChild, *PeekProposedSa(), *aIkeMsg) )
            {
                DEBUG_LOG1(_L("Unaccepted SA content in IKE_SA_INIT response: %d"),iState);				    
                return EFalse;
            }
            if ( aIkeMsg->iNonce->PlDataLen() < IKEV2_MIN_NONCE_SIZE )
            {
                DEBUG_LOG1(_L("Nonce data too short %d"), iState);
                return EFalse;
            }
    
            if ( iNatNotify )
            {
                TBool Supported;
                TInetAddr LocalIp;	
                if ( iHdr.iIkeData->iUseMobIke )
                     LocalIp.SetAddress(KInetAddrNone);
                else LocalIp = iHdr.iLocalAddr; 	   
                
#ifdef _DEBUG
                TBuf<80> debugBuf;
                DEBUG_LOG(_L("Calculating NAT detection:"));
                LocalIp.Output(debugBuf);					
                DEBUG_LOG2(_L("LocalIp %S:%d"), &debugBuf, IKE_PORT);
                iHdr.iRemoteAddr.Output(debugBuf);
                DEBUG_LOG2(_L("RemoteIp %S:%d"), &debugBuf, IKE_PORT);
#endif
                
                iHdr.iNATFlags = CIkev2NatT::CheckPeerNotifysL(*aIkeMsg->iNotifs, LocalIp, iHdr.iRemoteAddr, IKE_PORT,
                                                              iHdr.SpiI(), iHdr.SpiR(), Supported);
                GetNatStatus(Supported, aRemote); 
            }	
    
            delete iNonce_R;
            iNonce_R = NULL;
    
            iNonce_R = HBufC8::NewL(aIkeMsg->iNonce->PlDataLen());
            iNonce_R->Des().Copy(aIkeMsg->iNonce->PayloadData(), aIkeMsg->iNonce->PlDataLen());				
            if ( !ProcessKeyExchangeL((TKEPayloadIkev2*)aIkeMsg->iKe, iHdr.iDHGroup) )
            {
               return EFalse;
            }   
    
            //
            // IKE_SA_INIT request is completed enter IKE_AUTH
            //
            GenerateIkeKeysL();				
            TPtrC8 ikeHdrPtr((TUint8*)IkeHdr, IkeHdr->GetLength());
            SaveSignedDataL(EFalse, ikeHdrPtr);  // Save IKE_AUTH message 2
            
            //We ignore possible cert req payloads and just work
            //according our policy
            if ( !iHdr.iEAPType &&
                 (iHdr.iAuthMethod == RSA_DIGITAL_SIGN || iHdr.iAuthMethod == DSS_DIGITAL_SIGN) )
            {                    			
                SaveSignedDataL(ETrue, iHdr.iLastRequest->IkeMessageDatagram()); // Own identity not yet saved to signed data																			
    
                GetOwnIdentityL();    // Get own Identity from Certificate (or policy)                    
                AddIdToSignedDataL(ETrue, iAuthMsgInit, iLocalIdentity->PayloadData());    			    
            }
            else
            {
                //
                // Check if "implicit" Child SA exchange required
                // by getting request CIkev2Acquire object from queue
                //
                GetOwnIdentityL();
                SaveSignedDataL(ETrue, iHdr.iLastRequest->IkeMessageDatagram()); // Own identity saved to signed data   				            
            }	
            iChildSaRequest = CIkev2Acquire::GetNext(GetAcquireQue(), EFalse);				
            SendIkeAuthMessageL();
		}
		else
		{
            //
            // Ignore received message silently
            //
            DEBUG_LOG1(_L("IKE_SA_INIT response received in state %d"), iState);
		}	
	}
	else {
		if ( !Initiator )
		{
			DEBUG_LOG1(_L("IKEv2 Message without Orig_Init-bit in wrong, state: %d"), iState);
			return ETrue;		
		}	
		if ( Response )
		{
		    DEBUG_LOG1(_L("IKEv2 Message is not request, state: %d"), iState);			
			return ETrue;		
		}
		
		switch ( iState )
		{
			case KStateIdle:
			case KStateIkeSaInitResponse:
			    //Record Initiator SPI
			    aIkeMsg->GetIkeMsg()->GetSPI_I(iHdr.SpiI());
			    iHdr.SpiI().SetLength(IKEV2_SPI_SIZE);
			    
				//
				// Received message should be an IKE_SA_INIT request
				// Request message should be format:
				// HDR(A,0), SAi1, KEi, Ni, [CERTREQ]
				//
			    {
				if ( !CheckPayloadsOrder(aIkeMsg, IKE_SA_INIT, EFalse) )
				{
				    DEBUG_LOG1(_L("Erroneous IKE_SA_INIT request: %d"), iState);
					return EFalse;	
				}
				if ( MsgId != iHdr.ExpectedRequestId() ) {
					if ( iHdr.iLastResponse != NULL && 
					     MsgId == iHdr.iLastResponse->MessageId() && 
					     iState == KStateIkeSaInitResponse )
					{
					   //
					   // Retransmission of an earlier IKE_SA_INIT
					   // request. Retransmit current IKE_SA_INIT
					   // response (if retry count not exhausted)
					   //
					   DoRetransmitL(ETrue);
					   return ETrue;
					}
					else {	
					   DEBUG_LOG1(_L("Wrong message id in request, state: %d"), iState);					   
					   return EFalse;
					}  		   
				}
				if ( iState == KStateIkeSaInitResponse )
				   return EFalse; // IKE_SA_INIT request retry with a new message ID
				iIkeV2PlugInSession.StartResponding();

				//
				// Build a SA payload from current IKE policy and
				// verify received IKE SA request with it
				//
				HBufC8* SaBfr = Ikev2Proposal::FromPolicyToProposaL(iHdr, iSPI_Rekey, iDHGroupGuess);
				CleanupStack::PushL(SaBfr);
				HBufC8* proposedSa = NULL;
				TBool SaOk = Ikev2Proposal::VerifySaRequestAndGetProposedSaBufferL(iHdr, iChild, 
                                                                                   *SaBfr, *aIkeMsg, 
                                                                                   proposedSa);
				CleanupStack::PopAndDestroy();				
				if ( !SaOk )
				{
				    DEBUG_LOG1(_L("Unaccepted SA content in IKE_SA_INIT request: %d"),iState);
					SetNotifyCode(NO_PROPOSAL_CHOSEN);
					return EFalse;  							
				}
				SetProposedSa(proposedSa);
				proposedSa = NULL;
				if ( aIkeMsg->iNonce->PlDataLen() < IKEV2_MIN_NONCE_SIZE )
				{
				    DEBUG_LOG1(_L("Nonce data too short %d"), iState);
					return EFalse;	
				}
		
				//Check peer NAT status
				TBool useNatDetection = EFalse;
                if ( !iHdr.iIkeData->iUseNatProbing )
                {
                    TInetAddr LocalIp;  
                    if ( iHdr.iIkeData->iUseMobIke )
                         LocalIp.SetAddress(KInetAddrNone);
                    else LocalIp = iHdr.iLocalAddr;        
                    iHdr.iNATFlags = CIkev2NatT::CheckPeerNotifysL(*aIkeMsg->iNotifs, LocalIp, 
                                                                   iHdr.iRemoteAddr, IKE_PORT,
                                                                   iHdr.SpiI(), iHdr.SpiR(), useNatDetection);
                }

                if ( !ProcessKeyExchangeL((TKEPayloadIkev2*)aIkeMsg->iKe, iHdr.iDHGroup) )
                    return EFalse;

                
				//
				// Create own SPI (responder)
				//
  				CreateIkeSPI(iHdr.SpiR());
				delete iNonce_I;
				iNonce_I = NULL;
				iNonce_I = HBufC8::NewL(aIkeMsg->iNonce->PlDataLen());
				iNonce_I->Des().Copy(aIkeMsg->iNonce->PayloadData(), aIkeMsg->iNonce->PlDataLen());
				GetNonceDataL(EFalse);

				TPtrC8 ikeHdrPtr((TUint8*)IkeHdr, IkeHdr->GetLength());
				SaveSignedDataL(EFalse, ikeHdrPtr);  // Save IKE_AUTH message 2 
				
				//
				// Build IKE_SA_INIT response message: HDR, SAr1, KEr, Nr, [CERTREQ]
				//
				__ASSERT_DEBUG(!iHdr.iInitiator, User::Invariant());				
				CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(), iHdr.SpiR(),
                                                            IKE_SA_INIT, 
                                                            iHdr.iInitiator,
                                                            ETrue, 
                                                            iHdr.ExpectedRequestId(),
                                                            iDebug);
				CleanupStack::PushL(ikeMsg);

                HBufC8* saBfr = Ikev2Proposal::FromPolicyToProposaL(iHdr, iSPI_Rekey, iDHGroupGuess);
                CleanupStack::PushL(saBfr);
                ikeMsg->AppendSaPayloadL(*saBfr);
                CleanupStack::Pop(saBfr);
                SetProposedSa(saBfr);
				
                AppendKEPayloadL(*ikeMsg, iHdr.iDHGroup);
                ikeMsg->AppendNoncePayloadL(*iNonce_R);
                
				if ( iPkiService )
				    {
				    ikeMsg->AppendCertReqPayloadL(iPkiService->CaList());
				    }
				
				if ( useNatDetection )
				{   
                    delete iNatNotify;
                    iNatNotify = NULL;
				
					TInetAddr LocalIp;	
					if ( iHdr.iIkeData->iUseMobIke )
						 LocalIp.SetAddress(KInetAddrNone);
					else LocalIp = iHdr.iLocalAddr; 	   
                    
                    
					iNatNotify = CIkev2NatT::NewL(LocalIp, iHdr.iRemoteAddr, IKE_PORT, ikeMsg->InitiatorSpi(), ikeMsg->ResponderSpi());				       
				    ikeMsg->AppendNotifyPayloadL(IKEV2_PROTOCOL, KZeroDesc, NAT_DETECTION_SOURCE_IP, 
				                                 iNatNotify->SourceNofify());
			        ikeMsg->AppendNotifyPayloadL(IKEV2_PROTOCOL, KZeroDesc, NAT_DETECTION_DESTINATION_IP, 
				                                 iNatNotify->DestinNofify());         					
				}
				GetNatStatus(useNatDetection, aRemote);
				CleanupStack::Pop(ikeMsg);
				SendIkeMsgL(ikeMsg);
				GenerateIkeKeysL();								
				
				SaveSignedDataL(ETrue, ikeMsg->IkeMessageDatagram()); // Own identity is not yet saved to signed data
			    iState = KStateIkeSaInitResponse;
				}
                break;

			default:
				//
				// Ignore received message silently
				//
			    DEBUG_LOG1(_L("IKE_SA_INIT message received in state %d"), iState);
				break;
				
		}	
	}
	
	return ETrue;
}

TBool CIkev2Negotiation::ProcessIkeAuthL(CIkev2Payloads* aIkeMsg)
{
    ASSERT(aIkeMsg);
	//
	// Process IKE message of exchange type IKE_AUTH
	//
	ThdrISAKMP* IkeHdr = aIkeMsg->GetIkeMsg();  // IKE Message fixed header
	TBool   Response   = IkeHdr->GetFlags() & IKEV2_RESPONSE_MSG;
	TBool   Initiator  = IkeHdr->GetFlags() & IKEV2_INITIATOR;
	TUint32 MsgId      = IkeHdr->GetMessageId();

	if ( iHdr.iInitiator )
	{
		if ( Initiator )
		{
		    DEBUG_LOG1(_L("IKEv2 Message with Orig_Init-bit in wrong state: %d"), iState);
			return ETrue;  	
		}	
		if ( !Response )
		{
		    DEBUG_LOG1(_L("IKEv2 Message is not response; state: %d"), iState);
			return ETrue;
		}
		if ( MsgId != iHdr.ExpectedResponseId() )
		{
		    DEBUG_LOG1(_L("Wrong message id in response; state: %d"), iState);
		    return ETrue;		
		}

		switch ( iState )
		{
			case KStateIkeSaAuthRequest:
			    DEBUG_LOG(_L("Handling IKE_AUTH response"));
				//
				// Received message should be a response to a
				// IKE_AUTH request transmitted.
				// Response message should be format:
				// HDR(A,B), SK {IDr, [CERT,] AUTH, [CP], SAr2, TSi, TSr}				
				//
				if ( aIkeMsg->iEncr )
				{	
				   ProcessNotifyPayloadsL(*aIkeMsg->iNotifs, EFalse, IKE_AUTH);
				   if ( iDeleteIkeSA )
				   {
				      DEBUG_LOG1(_L("Error Notify in IKE_AUTH response: %d"), iState);			
					  
		              //Because we are just in IKE_AUTH no IKE_SAs exists --> we don't 
    		          //want to delete one. So we set iDeleteIkeSA back to false.			          
			          iDeleteIkeSA = EFalse;
					 	
					  return EFalse;
				   }
				}   
				if ( !CheckPayloadsOrder(aIkeMsg, IKE_AUTH, ETrue) )
				{
				    DEBUG_LOG1(_L("Erroneous IKE_AUTH response: %d"), iState);
					SetNotifyCode(INVALID_SYNTAX);
				
				    if ( iChildSaRequest && !iChildSARejected && !aIkeMsg->iSa )
				        {
						iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);	
						}
					
				    return EFalse;	
				}
				DEBUG_LOG(_L("IKE_AUTH payload order check passed"));
				TBool Status;
				if ( iEapCompleted )
				{	
				    Status = AuthenticatePeerL(aIkeMsg->iAuth);
				}   
				else
				{
					if ( iPkiService && !VerifyPeerCertificateL(aIkeMsg->iCerts, aIkeMsg->iIdR) )
					{
						SetNotifyCode(AUTHENTICATION_FAILED);					
						return EFalse;
					}
				    Status = AddIdAndAuthenticatePeerL(aIkeMsg);
				}	
				if ( !Status )
				{
					SetNotifyCode(AUTHENTICATION_FAILED);
					return EFalse;
				}
				//
				// If implicit Child SA negotiation requested,
				// verify IPSEC SA- and Traffic selector payloads, too
				//
				if ( iChildSaRequest )
				{
                    DEBUG_LOG(_L("Processing CHILD_SA creation"));
					if ( !iChildSARejected )
					{
						if ( !Ikev2Proposal::VerifySaResponseL(iHdr, iChild, *iChildSaRequest->SA(), *aIkeMsg) )
						{
							DEBUG_LOG1(_L("Unaccepted SA payload content in IKE_AUTH response: %d"),iState);
							SetNotifyCode(NO_PROPOSAL_CHOSEN);
							iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);							
							return EFalse;
						}
						DEBUG_LOG(_L("SA response verified"));
						if ( !IpsecSelectors::VerifyTrafficSelectorsL(iChildSaRequest, (TTSPayloadIkev2*)aIkeMsg->iTsI, (TTSPayloadIkev2*)aIkeMsg->iTsR ) )
						{
						    DEBUG_LOG1(_L("Unaccepted Traffic Selectors in IKE_AUTH response: %d"),iState);
							SetNotifyCode(TS_UNACCEPTABLE);
							iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);
							return EFalse;
						}
						DEBUG_LOG(_L("Traffic selectors verified"));
						if ( aIkeMsg->iCp )
						{
							if ( iConfigMode )
							    {
								iConfigMode->ProcessCpL(aIkeMsg->iCp);
							    }
							else
							    {
							    DEBUG_LOG(_L("Unsolicited CP payload in IKE_AUTH response"));
							    }							
						}
						iChildSaRequest->SetSPI_Out(iChild.iSPI_Out);	
						DEBUG_LOG(_L("Generating IPsec keys"));
						iChild.GenerateIpsecKeysL(iHdr.iSK_d, KZeroDesc, 
                                                  *iNonce_I, *iNonce_R, iHdr.iPRFAlg);
						DEBUG_LOG(_L("IPsec keys generated"));
					}	
					else	
					{
					    DEBUG_LOG1(_L("Implicit CHILD_SA rejected Notify in IKE_AUTH response: %d"), iState);
						iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);
					}	
				}	
				//
				// IKE_AUTH request is completed and IKE SA has been
				// negotiated
				//
				IkeSaCompletedL();				   
				break;

			case KStateIkeSaEapStarted:
				//
				// Received message should be an IKE_AUTH response
				// containing an EAP payload.
				// The content of received IKE message shall be:
				// HDR, SK {IDr, [CERT,] AUTH, EAP }
				//
				if ( !aIkeMsg->iEncr || !aIkeMsg->iIdR || !aIkeMsg->iAuth || !aIkeMsg->iEap )
				{
					DEBUG_LOG1(_L("Erroneous IKE_AUTH response: %d"), iState);
					SetNotifyCode(INVALID_SYNTAX);			
					return EFalse;	
				}
				if ( iPkiService && !VerifyPeerCertificateL(aIkeMsg->iCerts, aIkeMsg->iIdR) )
				{
					SetNotifyCode(AUTHENTICATION_FAILED);										
					return EFalse;
				}
                if ( !AddIdAndAuthenticatePeerL(aIkeMsg) )
                {
                    SetNotifyCode(AUTHENTICATION_FAILED);										
                    return EFalse;
                }
				iState = KStateIkeSaEapGoing;
                iEapPlugin->EapDataInbound(aIkeMsg->iEap);
                break;
				
			case KStateIkeSaEapGoing:
				//
				// Received message should be an IKE_AUTH response
				// containing an EAP payload.
				// The content of received IKE message shall be:
				// HDR, SK {EAP}				
                //
				if ( !aIkeMsg->iEncr || !aIkeMsg->iEap )
				{
					DEBUG_LOG1(_L("Erroneous IKE_AUTH response: %d"), iState);
					SetNotifyCode(INVALID_SYNTAX);				
					return EFalse;	
				}
				iEapPlugin->EapDataInbound(aIkeMsg->iEap);
				break;

			default:
				//
				// Ignore received message silently
				//
				DEBUG_LOG1(_L("IKE_AUTH response received in state %d"), iState);
				break;

		}	
	}
	else
	{
		if ( !Initiator )
		{
			DEBUG_LOG1(_L("IKEv2 Message without Orig_Init-bit in wrong, state: %d"), iState);
			return ETrue;		
		}	
		if ( Response )
		{
			DEBUG_LOG1(_L("IKEv2 Message is not request, state: %d"), iState);
			return ETrue;		
		}
		switch ( iState )
		{
			case KStateIkeSaInitResponse:
			case KStateIkeSaCompleted:	
				//
				// Received message should be an IKE_AUTH request
				// Request message should be format:
				// HDR(A,B), SK {IDi, [CERT,] [CERTREQ,] [IDr,] AUTH, SAi2, TSi, TSr} 
				//
				if ( !CheckPayloadsOrder(aIkeMsg, IKE_AUTH, EFalse) )
				{
					DEBUG_LOG1(_L("Erroneous IKE_AUTH request: %d"), iState);
					SetNotifyCode(INVALID_SYNTAX);
					return EFalse;	
				}
				if ( MsgId != iHdr.ExpectedRequestId() ) {
					if ( iHdr.iLastResponse != NULL &&
					     MsgId == iHdr.iLastResponse->MessageId() && 
					     iState == KStateIkeSaCompleted )
					{
					   //
					   // Retransmission of an earlier IKE_SA_INIT
					   // request. Retransmit current IKE_SA_INIT
					   // response (if retry count not exhausted)
					   //
						DoRetransmitL(ETrue);
						return ETrue;
					}
					else {	
						DEBUG_LOG1(_L("Wrong message id in request, state: %d"), iState);					
						SetNotifyCode(INVALID_MESSAGE_ID);
						StoreNotifyData32(MsgId);
						return EFalse;  								   
					}  		   
				}
				if ( iState == KStateIkeSaCompleted )
				   return EFalse; // IKE_AUTH request retry with a new message ID

				//if ( aIkeMsg->iEncr )
				//{	
					ProcessNotifyPayloadsL(*aIkeMsg->iNotifs, EFalse, IKE_AUTH);
					if ( iDeleteIkeSA )
					{
						DEBUG_LOG1(_L("Error Notify in IKE_AUTH response: %d"), iState);
						return EFalse;
					}
				//}
				
				if ( iPkiService && !VerifyPeerCertificateL(aIkeMsg->iCerts, aIkeMsg->iIdI) )
				{
				    DEBUG_LOG(_L("Peer certificate validation failed."));
					SetNotifyCode(AUTHENTICATION_FAILED);
					return EFalse;
				}
				if ( !AddIdAndAuthenticatePeerL(aIkeMsg) )
				{	
					SetNotifyCode(AUTHENTICATION_FAILED);
					return EFalse;
		        }
				//
				// Process "concatenated" Child SA- and Traffic
				// Selector payloads if present  
				//
				if ( aIkeMsg->iSa )
				{
				    DEBUG_LOG(_L("IKE_AUTH request has SA and TS payload."));				
					CIkev2Acquire* Acquire = IpsecSelectors::GetIpsecPolicyL(iIkeV2PlugInSession, aIkeMsg);
					if ( !Acquire )
					{
					    DEBUG_LOG1(_L("Unaccepted Traffic Selectors in IKE_AUTH request: %d"),iState);
						SetNotifyCode(TS_UNACCEPTABLE);						
						return EFalse;
					}
					CleanupStack::PushL(Acquire);						
					HBufC8* proposedSaBuffer = NULL;
					if (!Ikev2Proposal::VerifySaRequestAndGetProposedSaBufferL(iHdr, iChild, 
                                                                               *Acquire->SA(), *aIkeMsg,
                                                                               proposedSaBuffer))
					{
						CleanupStack::PopAndDestroy(Acquire);  
						DEBUG_LOG1(_L("Unaccepted SA content in IKE_AUTH request: %d"),iState);
						SetNotifyCode(NO_PROPOSAL_CHOSEN);						
						return EFalse;  							
					}
					SetProposedSa(proposedSaBuffer);
					proposedSaBuffer = NULL;
					//
					// Replace SA payload buffer in CIkev2Acquire with
					// selected SA payload built in VerifySaRequestL
					//
					CleanupStack::Pop(Acquire); 							
					Acquire->ReplaceSA(GetProposedSa()); 
					Acquire->SetSPI_Out(iChild.iSPI_Out);
					Acquire->SetResponse();
					if ( iChild.iTransport )
    					{
					    
						Acquire->SetTransport();				
    					}
 					CIkev2Acquire::Link(Acquire, GetAcquireQue());
 					DEBUG_LOG(_L("Acquire linked."));
 					
					if ( aIkeMsg->iCp )
					{
						//
						// CP payload received as IKE SA responder
						// Handle CP payload and return "dummy"
						// virtual IP to initiator. 
						//
						delete iConfigMode;
						iConfigMode = NULL;
						iConfigMode = CIkev2Config::NewL(Acquire, (TInetAddr*)&iHdr.iRemoteAddr);
						iConfigMode->ProcessCpL(aIkeMsg->iCp);
						Acquire->SetVirtualIp();
					}
	        		//
			        // Get SPI for new inbound SA
        			//
                    iChild.GenerateIpsecKeysL(iHdr.iSK_d, KZeroDesc, 
                                               *iNonce_I, *iNonce_R, iHdr.iPRFAlg);
					
				    if ( iPkiService && !iEapPlugin && 
				         aIkeMsg->iCertReqs && 
				         aIkeMsg->iCertReqs->Count() )
                	{
     			       GetOwnIdentityL();    // Get own Identity from Certificate (or policy)    			       
    			       AddIdToSignedDataL(ETrue, iAuthMsgResp, iLocalIdentity->PayloadData());
            					
        			   CIkev2Acquire* Acquire = CIkev2Acquire::PeekFirst(GetAcquireQue());
        			   if ( Acquire )
        			   {
        				   GetIpsecSPI(Acquire);
        				   iState = KStateIkeSaAuthWaitSpi;
        			   }
        			   else 
        			   {       
        			       DEBUG_LOG(_L("CIkev2Acquire::PeekFirst returned NULL."));			    
        			       DEBUG_LOG(_L("Sending IKE_AUTH response."));			    
        			       SendIkeAuthMessageL();
        			   }

                	}	
                    else
					{
						GetOwnIdentityL();
						AddIdToSignedDataL(ETrue, iAuthMsgResp, iLocalIdentity->PayloadData());
						GetIpsecSPI(Acquire);						
					    iState = KStateIkeSaAuthWaitSpi;
					}	
				}
				else
				{
					if ( iPkiService && !iEapPlugin && 
					     aIkeMsg->iCertReqs && 
					     aIkeMsg->iCertReqs->Count() )
                    {                       
       			        GetOwnIdentityL();    // Get own Identity from Certificate (or policy)
        			    AddIdToSignedDataL(ETrue, iAuthMsgResp, iLocalIdentity->PayloadData());
        			               						
        			    CIkev2Acquire* Acquire = CIkev2Acquire::PeekFirst(GetAcquireQue());
        			    if ( Acquire )
        			    {
        				   GetIpsecSPI(Acquire);
        				   iState = KStateIkeSaAuthWaitSpi;
        			    }
        			    else 
        			    {        			        
        			       SendIkeAuthMessageL();
        			    }
					}
					else
					{	
				       //
				       // Build and send an IKE_AUTH response
				       //
						GetOwnIdentityL();
						AddIdToSignedDataL(ETrue, iAuthMsgResp, iLocalIdentity->PayloadData());
				        SendIkeAuthMessageL();
					}		
				}   
				break;

			default:
				//
				// Ignore received message silently
				//
				DEBUG_LOG1(_L("IKE_SA_INIT message received in state %d"), iState);
				break;
		}	
	}
	
	return ETrue;
}

TBool CIkev2Negotiation::ProcessChildSaL(CIkev2Payloads* aIkeMsg)
{
    ASSERT(aIkeMsg);
	//
	// Process IKE message of exchange type CREATE_CHILD_SA
	//
	TUint16 PfsDHGroup;
	ThdrISAKMP* IkeHdr = aIkeMsg->GetIkeMsg();  // IKE Message fixed header
	TBool   Response   = IkeHdr->GetFlags() & IKEV2_RESPONSE_MSG;
	TBool   Initiator  = IkeHdr->GetFlags() & IKEV2_INITIATOR;
	TUint32 MsgId      = IkeHdr->GetMessageId();
			
	if ( iHdr.iInitiator )
	{
		if ( Initiator )
		{
			DEBUG_LOG1(_L("IKEv2 Message with Orig_Init-bit in wrong state: %d"), iState);
			SetNotifyCode(INVALID_SYNTAX);		
			return EFalse;  	
		}	
	}
	else
	{
		if ( !Initiator )
		{
			DEBUG_LOG1(_L("IKEv2 Message without Orig_Init-bit in wrong state: %d"), iState);
			SetNotifyCode(INVALID_SYNTAX);		
			return EFalse;  	
		}	
	}

	if ( Response )
	{
	   //
	   // CREATE_CHILD_SA response message received
	   //
        switch ( iState )
	    {
		   case KStateIkeChildSARequest:
			  //
			  // Received message should be a response to a
			  // CREATE_CHILD_SA request transmitted.
			  // Response message should be format:
			  // HDR(A,B), SK { SA, Nr, [KEr], [TSi, TSr]}
			  //
			  if ( MsgId != iHdr.ExpectedResponseId() )
			  {
				   DEBUG_LOG1(_L("Wrong message id in response; state: %d"), iState);

				   SetNotifyCode(INVALID_MESSAGE_ID);
				   StoreNotifyData32(MsgId);	 
				   return EFalse; 	
			  }
			  if ( aIkeMsg->iEncr )
			  {	
			      ProcessNotifyPayloadsL(*aIkeMsg->iNotifs, EFalse, CREATE_CHILD_SA);
			      if ( iDeleteIkeSA )
			      {
				     DEBUG_LOG1(_L("Error Notify in CREATE_CHILD_SA response: %d"), iState);
				     return EFalse;
				  }
				  if ( iChildSARejected )
				  {
					  DEBUG_LOG1(_L("CHILD_SA rejected Notify in CREATE_CHILD_SA response: %d"), iState);					  
					  iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);
					  iStopped = ETrue;
					  return EFalse;
				  } 
			  }	
			  if ( !CheckPayloadsOrder(aIkeMsg, CREATE_CHILD_SA, ETrue) )
			  {
			     DEBUG_LOG1(_L("Erroneous CREATE_CHILD_SA response: %d"), iState);
				 SetNotifyCode(INVALID_SYNTAX);
				 iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);
				 return EFalse;	
			  }
	  		  if ( aIkeMsg->iNonce->PlDataLen() < IKEV2_MIN_NONCE_SIZE )
			  {
			     DEBUG_LOG1(_L("Nonce data too short %d"), iState);
				 iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);
				 iStopped = ETrue;
				 return EFalse;
			  }
			  
		      if ( !Ikev2Proposal::VerifySaResponseL(iHdr, iChild, *iChildSaRequest->SA(), *aIkeMsg) )
			  {
			     DEBUG_LOG1(_L("Unaccepted SA content in CREATE_CHILD_SA response: %d"),iState);				
				 SetNotifyCode(NO_PROPOSAL_CHOSEN);
				 iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);
			     return EFalse;
			  }
			  if ( !IpsecSelectors::VerifyTrafficSelectorsL(iChildSaRequest, (TTSPayloadIkev2*)aIkeMsg->iTsI, (TTSPayloadIkev2*)aIkeMsg->iTsR ) )
			  {
			     DEBUG_LOG1(_L("Unaccepted Traffic Selectors in CREATE_CHILD_SA response: %d"),iState);	  						 
				 SetNotifyCode(TS_UNACCEPTABLE);
				 iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);
				 return EFalse;
			  }
			  delete iNonce_R;
			  iNonce_R = NULL;
			  iNonce_R = HBufC8::NewL(aIkeMsg->iNonce->PlDataLen());
			  iNonce_R->Des().Copy(aIkeMsg->iNonce->PayloadData(), aIkeMsg->iNonce->PlDataLen());
			  PfsDHGroup = iChildSaRequest->DHGroup(); 
			  if ( PfsDHGroup  )
			  {
		         if ( !ProcessKeyExchangeL((TKEPayloadIkev2*)aIkeMsg->iKe, PfsDHGroup) )
				 {	 					 
					 iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);
					 return EFalse;
				 }	
                 HBufC8* g_ir = iDHKeys->ComputeAgreedKeyL(iDHPublicPeer->Des());
		         CleanupStack::PushL(g_ir);

		         iChild.GenerateIpsecKeysL(iHdr.iSK_d, *g_ir, 
                                           *iNonce_I, *iNonce_R, iHdr.iPRFAlg);

		         
		         g_ir->Des().FillZ(); // Wipe out shared secret value from buffer
		         CleanupStack::PopAndDestroy();  //g_ir

			  }
			  else if ( aIkeMsg->iKe )
			  {
			      DEBUG_LOG1(_L("Unsolicted Key Exchange payload present in CREATE_CHILD_SA response: %d"),iState);
				  SetNotifyCode(INVALID_KE_PAYLOAD);
				  iChildSaRequest = Ikev2Pfkey::DeleteInboundSPI(iHdr, iIkeV2PlugInSession, iChildSaRequest);				  
				  return EFalse;
			  }
			  else
              {
                  iChild.GenerateIpsecKeysL(iHdr.iSK_d, KZeroDesc, 
                                            *iNonce_I, *iNonce_R, iHdr.iPRFAlg);
              
              }
			  //
			  //  CREATE_CHILD_SA request is completed Update 
			  //			  
			  IpsecSANegotiatedL();
			  break;

		   case KStateIkeSARekeyRequest:
			  //
			  // Received message should be a response to a
			  // IKE SA rekey CHILD_SA request transmitted.
			  // Response message should be format:
			  // HDR(A,B), SK { SA, Nr, KEr }
			  //
			  if ( CheckPayloadsOrder(aIkeMsg, CREATE_CHILD_SA, ETrue) && aIkeMsg->iKe )
			  {
			     DEBUG_LOG1(_L("IKE SA rekey message received as CHILD_SA response: %d"), iState);
				 ProcessIkeSARekeyL(aIkeMsg);
			  }
			  else
		      {
                  DEBUG_LOG1(_L("Erroneous IKE SA rekey message received as CHILD_SA response: %d"),iState);		      
		      }
			  //iIkeV2PlugInSession.UpdateIkev2SAL(&iHdr, NULL, EFalse);								  
			  BuildDeleteRequestL(NULL);  // Delete IKE SA rekeyed
			  iIkeV2PlugInSession.DeleteIkev2SA(iHdr.SaId());			  
			  break; 

		   default:
			  //
			  // Ignore received message silently
			  //
			  DEBUG_LOG1(_L("CREATE_CHILD_SA response received in state %d"), iState);
			  break;

	    }	
	}
	else
	{
	    //
	    // CREATE_CHILD_SA request message received
	    //
		if ( MsgId != iHdr.ExpectedRequestId() ) {
			if ( iHdr.iLastResponse != NULL &&
			     MsgId == iHdr.iLastResponse->MessageId() )
			{
			   //
			   // Retransmission of an earlier request.
			   // Retransmit current response 
			   //
				iState = KStateIkeChildSAResponse;
				DoRetransmitL(ETrue);
				return ETrue;
			}
			else {	
				DEBUG_LOG1(_L("Wrong message id in request, state: %d"), iState);
				SetNotifyCode(INVALID_MESSAGE_ID);
				StoreNotifyData32(MsgId);
				return EFalse;  								   
			}  		   
		}
	   
		if ( iState >= KStateIkeSaCompleted )
		{
		   //
		   // Received CREATE_CHILD_SA message can be one of the
		   // following:
		   // -- Create new Ipsec SA request:  
	       //    HDR(A,B), SK { SA, Nr, [KEi], [TSi, TSr]}
		   // -- Rekey Ipsec SA request:
		   //    HDR(A,B), SK { N, SA, Ni, [KEi], [TSi, TSr]} 
		   // -- Rekey IKE SA request: 
		   //    HDR(A,B), SK { SA, Ni, KEi} 
		   //   
			if ( !CheckPayloadsOrder(aIkeMsg, CREATE_CHILD_SA, EFalse) )
			{
				DEBUG_LOG1(_L("Erroneous CREATE_CHILD_SA request: %d"), iState);
				SetNotifyCode(INVALID_SYNTAX);						
				return EFalse;	
			}
		   //
		   // Check is the current request an IKE SA rekey by checking
		   // Proposal payload protocol value
		   //
            if ( Ikev2Proposal::IkeSaRekey(aIkeMsg) ) 
			{
				TBool Status;
				if ( iState == KStateIkeSARekeyRequest )
				{
				   DEBUG_LOG1(_L("IKE SA Rekey collision for SAID: %d"), iHdr.SaId());										   
				   SetNotifyCode(NO_ADDITIONAL_SAS);
				   Status = EFalse;
				}
				else
				{	
				   DEBUG_LOG1(_L("IKE SA Rekey started by peer for SAID: %d"), iHdr.SaId());							   						   
				   iState = KStateIkeSARekeyResponse;
				   Status = ProcessIkeSARekeyL(aIkeMsg);
				}
				return Status;
			}
			if ( CIkev2Acquire::Responding(GetAcquireQue()) )
			{
				DEBUG_LOG1(_L("CREATE_CHILD_SA IKE SA request already pending: %d"), iState);
				SetNotifyCode(NO_ADDITIONAL_SAS);
				return EFalse;	
			}
		    	
		   //
		   // Get acceptable Ipsec policy for peer defined traffic
		   // selectors (and peer address)
		   //
		    CIkev2Acquire* Acquire = IpsecSelectors::GetIpsecPolicyL(iIkeV2PlugInSession, aIkeMsg, 
                                                                     iHdr.iIkeData->iGroupDesc_II);
			if ( !Acquire )
			{
				DEBUG_LOG1(_L("Unaccepted Traffic Selectors in CREATE_CHILD_SA request: %d"),iState);
				SetNotifyCode(TS_UNACCEPTABLE);						
				return EFalse;
			}
			CleanupStack::PushL(Acquire);			
			HBufC8* proposedSaBuffer = NULL;
			if (!Ikev2Proposal::VerifySaRequestAndGetProposedSaBufferL(iHdr, iChild, *Acquire->SA(), 
                                                                       *aIkeMsg, proposedSaBuffer))
			{
				CleanupStack::PopAndDestroy(Acquire);				
				DEBUG_LOG1(_L("Unaccepted SA content in CREATE_CHILD_SA request: %d"),iState);
				SetNotifyCode(NO_PROPOSAL_CHOSEN);						
				return EFalse;  							
			}
			this->SetProposedSa(proposedSaBuffer);
			proposedSaBuffer = NULL;
			delete iNonce_I;
			iNonce_I = NULL;
			iNonce_I = HBufC8::NewL(aIkeMsg->iNonce->PlDataLen());
			iNonce_I->Des().Copy(aIkeMsg->iNonce->PayloadData(), aIkeMsg->iNonce->PlDataLen());
			if ( aIkeMsg->iKe )
			{
				PfsDHGroup = Acquire->DHGroup();
				if ( PfsDHGroup == 0 )
				{	
					PfsDHGroup = Ikev2Proposal::GetDHGroup(iHdr.iIkeData->iGroupDesc_II);
					Acquire->DHGroup(PfsDHGroup);
				}	
				if ( !ProcessKeyExchangeL((TKEPayloadIkev2*)aIkeMsg->iKe, PfsDHGroup) )
				    {
				    CleanupStack::PopAndDestroy(Acquire);
					return EFalse;
				    }
			}
			CleanupStack::Pop(Acquire);						
			Acquire->SetSPI_Out(iChild.iSPI_Out);
			Acquire->SetResponse();
			if ( iChild.iTransport )
				Acquire->SetTransport();				
			CIkev2Acquire::Link(Acquire, GetAcquireQue());			
		    //
		    // Get SPI for new inbound SA
		    //
			GetIpsecSPI(Acquire);									
		}
		else
		{
		    //
			// Ignore received message silently
			//
			DEBUG_LOG1(_L("CREATE_CHILD_SA request received in state %d"), iState);
		}	
	}	

	return ETrue;
}

TBool CIkev2Negotiation::ProcessIkeSARekeyL(CIkev2Payloads* aIkeMsg)
{	
    ASSERT(aIkeMsg);
	//
	// Process IKE SA rekey message (as IKE_CHILD_SA exchange)
    // HDR(A,B), SK { SA, Nonce, KE}	
	//

			
	if ( (iState == KStateIkeSARekeyRequest) || (iState == KStateIkeSARekeyResponse) )
	{
	   //
	   // Received CREATE_CHILD_SA message for IKE SA rekey must
	   // look like the following:  HDR(A,B), SK { SA, Ni, [KEi]}
	   // Allocate a new CIkev2Negotiation object for new IKE SA
	   //
	   //
        CIkev2Negotiation* NewSA  = new (ELeave) CIkev2Negotiation(iIkeV2PlugInSession, iPfKeySocketIf, 
                                                                   iEventLogger, iMessageSendQue, iDebug, 0);
    	CleanupStack::PushL(NewSA);
    	
    	//Do not copy the previous sent request and response:
   	    CIkeV2Message* lastResponse = iHdr.iLastResponse;
   	    iHdr.iLastResponse = NULL;
  	    CIkeV2Message* lastRequest = iHdr.iLastRequest; 
  	    iHdr.iLastRequest = NULL;  	    
		NewSA->iHdr.Copy(iHdr);
        iHdr.iLastResponse = lastResponse; 
        iHdr.iLastRequest = lastRequest;       
		
		
		NewSA->iHdr.iWindowSize   = 1;
		NewSA->iHdr.iEncrAlg = 0;
		NewSA->iHdr.iPRFAlg = 0;
		NewSA->iHdr.iIntegAlg = 0;
		NewSA->iHdr.iDHGroup = 0;
		NewSA->iHdr.iAuthMethod = 0;
		NewSA->iHdr.iCipherKeyLth = 0;
		NewSA->iHdr.iCipherBlkLth = 0;
		NewSA->iHdr.iIntChkSumLth = 0;
				
		if ( iState == KStateIkeSARekeyRequest )
		{
		   NewSA->iHdr.iInitiator = ETrue;
		   NewSA->iHdr.SetSaId(iSAid_Rekey);		   
		   NewSA->iHdr.SetSpiI(iSPI_Rekey);
		   NewSA->iNonce_I    = iNonce_I; // Nonce was created in BuildIkeSaRekeyMsgL() earlier
		   NewSA->iDHKeys     = iDHKeys;  // DH keys object was created in BuildIkeSaRekeyMsgL() earlier		   
		   iNonce_I = NULL;
		   iDHKeys  = NULL;
		}
		else
		{
		   NewSA->iHdr.iInitiator = EFalse;
		   NewSA->iHdr.SetSaId(iIkeV2PlugInSession.GetSAId()); // Get a new SA Id		   
		   NewSA->CreateIkeSPI(NewSA->iHdr.SpiR());
		} 
		//
		// Build a SA payload from current IKE policy and
		// verify received IKE SA request with it
		//
		HBufC8* SaBfr = Ikev2Proposal::FromPolicyToProposaL(NewSA->iHdr, NewSA->iSPI_Rekey, NewSA->iDHGroupGuess, ETrue);
		CleanupStack::PushL(SaBfr);
		HBufC8* proposedSaBuffer = NULL;
		TBool SaOk = Ikev2Proposal::VerifySaRequestAndGetProposedSaBufferL(NewSA->iHdr, NewSA->iChild, 
                                                                           *SaBfr, *aIkeMsg, proposedSaBuffer);
		CleanupStack::PopAndDestroy();
		if ( iState == KStateIkeSARekeyRequest )
	      	 SaOk &= Ikev2Proposal::GetRekeySpi(aIkeMsg, NewSA->iHdr.SpiR());			
	    else SaOk &= Ikev2Proposal::GetRekeySpi(aIkeMsg, NewSA->iHdr.SpiI());			
		if ( !SaOk )
		{
			DEBUG_LOG1(_L("Unaccepted SA content in IKE_SA Rekey request: %d"), iState);
			SetNotifyCode(NO_PROPOSAL_CHOSEN);
	    	CleanupStack::PopAndDestroy(NewSA);					
			return EFalse;  							
		}
		NewSA->SetProposedSa(proposedSaBuffer);
		proposedSaBuffer = NULL;
		if ( aIkeMsg->iNonce->PlDataLen() < IKEV2_MIN_NONCE_SIZE )
		{
		    DEBUG_LOG1(_L("Nonce data too short in IKE_SA Rekey request %d"), iState);
			SetNotifyCode(INVALID_SYNTAX);				
	    	CleanupStack::PopAndDestroy(NewSA);
			return EFalse;	
		}
		if ( iState == KStateIkeSARekeyRequest )
		{
		    NewSA->iNonce_R = HBufC8::NewL(aIkeMsg->iNonce->PlDataLen());
		    NewSA->iNonce_R->Des().Copy(aIkeMsg->iNonce->PayloadData(),	aIkeMsg->iNonce->PlDataLen()); 
	    }
	    else
	    {
		    NewSA->iNonce_I = HBufC8::NewL(aIkeMsg->iNonce->PlDataLen());
		    NewSA->iNonce_I->Des().Copy(aIkeMsg->iNonce->PayloadData(),	aIkeMsg->iNonce->PlDataLen()); 
		}

		if ( !NewSA->ProcessKeyExchangeL((TKEPayloadIkev2*)aIkeMsg->iKe, NewSA->iHdr.iDHGroup) )
		{
		    //If there was notify code set, copy it to current negotiation before destroying NewSa
		    if(NewSA->GetNotifyCode())
		        {
		        SetNotifyCode(NewSA->GetNotifyCode());
		        }
		    TInt dataLth(0);
		    TUint8* notifyData = NewSA->NotifyData(dataLth);
            if(dataLth == 2)
                {
                StoreNotifyData16(GET16(notifyData));
                }
            else if(dataLth == 4)
                {
                StoreNotifyData32(GET32(notifyData));
                }
	        CleanupStack::PopAndDestroy(NewSA);
		    return EFalse;
		}

		if ( iState == KStateIkeSARekeyResponse )
		{
		    //
		    // Build IKE SA rekey response (CHILD_SA response):
		    // HDR, SAr, Nr, KEr
		    //
		   iDHKeys    = NewSA->iDHKeys; // To calculate own DH value
		   iSPI_Rekey = NewSA->iHdr.SpiR();	
		   SetProposedSa(NewSA->GetProposedSa());	
		   BuildIkeSaRekeyMsgL(EFalse);
		   NewSA->iNonce_R = iNonce_R;  // Nonce is created in BuildIkeSaRekeyMsgL()
		   iNonce_R   = NULL;
		   iDHKeys    = NULL;		    
		}   
		//
		// Generate key material for new IKE SA
		//
		NewSA->GenerateIkeKeysL(&iHdr);
		
		//
		// Create a new IKE SA and swap IPSec SA from rekeyed IKE SA
		// to the new just created SA
		//
       	iIkeV2PlugInSession.CreateIkev2SAL(NewSA->iHdr);
		iIkeV2PlugInSession.InheritIpsecSas(NewSA->iHdr.SaId(), iHdr.SaId());
		
		CleanupStack::PopAndDestroy(NewSA);		
	}

	return ETrue;
}

TBool CIkev2Negotiation::ProcessInfoMsgL(CIkev2Payloads* aIkeMsg)
{
    ASSERT(aIkeMsg);
	//
	// Process IKE message of exchange type INFORMATIONAL
	// HDR, SK {[N,] [D,] [CP,] ...}
	// Only encyrpted and authenitcated are processed.
	//
	if ( !aIkeMsg->Encrypted() )
	{	
	   if ( iState == KStateIdle)
		  iStopped = ETrue;
       return EFalse;
	}
	ThdrISAKMP* IkeHdr = aIkeMsg->GetIkeMsg();  // IKE Message fixed header
	TBool   Response   = IkeHdr->GetFlags() & IKEV2_RESPONSE_MSG;
	TUint32 MsgId      = IkeHdr->GetMessageId();
	
    if ( Response )
	{
	   if ( (iState == KStateIkeInfoRequest) || (iState == KStateIkeDeleteRequest) )
	   {
          //
		  // A response received to a transmitted Informational request
		  //
	      DEBUG_LOG(_L("Response received to a transmitted Informational request"));
	   
		  if ( MsgId == iHdr.ExpectedResponseId() )
		  {
			 iIkeV2PlugInSession.UpdateIkev2SAL(&iHdr, NULL);			 
			 if ( iState == KStateIkeDeleteRequest )
			 {	 
			    iIkeV2PlugInSession.IkeSaDeleted(KErrNone); //IKE SA deletion going
			 }	
			 else
			 {
				 if ( aIkeMsg->iNotifs->Count() )
				 {
					 ProcessNotifyPayloadsL(*aIkeMsg->iNotifs, EFalse, INFORMATIONAL);   
				 }
			 }
			 iStopped = ETrue;
			 return EFalse;
		  }
	   }
	   else if ( iState == KStateChildDeleteRequest )
	   {
		  //
		  // A response received to a transmitted Child SA delete request
		  //
		   if ( aIkeMsg->iDeletes->Count() )
		   {
			  ProcessDeletePayloadsL(*aIkeMsg->iDeletes, EFalse);   
		   }
		   
		   //
		   // Update sequence numbers and IKE SA data
		   //
		   iIkeV2PlugInSession.UpdateIkev2SAL(&iHdr, NULL);
		   iStopped = ETrue;		   
		   return EFalse;
	   }	   
	}
	else
	{
	   //
	   // A Informational request received. Process request according
	   // to payload content and send informational response.       
	   // 
	   DEBUG_LOG1(_L("INFORMATIONAL request received in state %d"), iState);
	   if ( MsgId == iHdr.ExpectedRequestId() )
	   {
		  TBool BuildResponse = ETrue;
		  if ( aIkeMsg->iDeletes->Count() )
		  {
             BuildResponse = ProcessDeletePayloadsL(*aIkeMsg->iDeletes, ETrue);   
		  }
		  if ( aIkeMsg->iNotifs->Count() )
		  {
			 BuildResponse = ProcessNotifyPayloadsL(*aIkeMsg->iNotifs, ETrue, INFORMATIONAL);   
		  }
		  if ( BuildResponse )
		  {	  		                                
              CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(),
                                                          iHdr.SpiR(),
                                                          INFORMATIONAL,
                                                          iHdr.iInitiator,
                                                          ETrue,
                                                          iHdr.ExpectedRequestId(), 
                                                          iDebug);
             CleanupStack::PushL(ikeMsg);
             ikeMsg->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);            
		     CleanupStack::Pop(ikeMsg);
		     SendIkeMsgL(ikeMsg);
             if ( (iState != KStateIkeInfoRequest) && (iState != KStateIkeDeleteRequest) && (iState != KStateIkeDeleteResponse) )
                 {
                 iState = KStateIkeSaCompleted;
                 }
		  }
	   }
	}	

	return ETrue;
}

TBool CIkev2Negotiation::ProcessNotifyPayloadsL(const CArrayFixFlat<TNotifPayloadIkev2*>& aNotifys, 
                                                TBool aRequest, TInt aExchange)
{
	if  ( Ikev2MobIke::ProcessNotifysL(this, aNotifys, aRequest, aExchange) )
	{	
		return EFalse; // Notify payload(s) was processed by MOBIKE protocol 
	}
	
	TInt MsgType;			
	TNotifPayloadIkev2* Payload;  
	TInt Count = aNotifys.Count();
	TInt i     = 0;
	
	while ( i < Count )
	{
	    Payload = aNotifys.At(i);
		MsgType = (TInt)Payload->GetMsgType();
		DEBUG_LOG1(_L("Received Notify payload message type %d"), MsgType);
        // 
		//  Process possible error type Notify messages 
		//
		if (aExchange == IKE_SA_INIT)
		    {
		    switch ( MsgType )
    		    {	
    			case INVALID_SYNTAX:
    			    //Fall through
    			case NO_PROPOSAL_CHOSEN:
    			    return EFalse;
    		    case INVALID_KE_PAYLOAD:
    		        ProcessInvalidKePayloadNotifyL();
    		        return ETrue;
    		    case COOKIE:
    		    	return ProcessCookieL(aNotifys, aRequest);
    			default:
    				break;	
    		    }
		    }
		else
		    {
		    switch ( MsgType )
    		    {	
    			case UNSUPPORTED_CRITICAL_PAYLOAD:
    			case INVALID_SYNTAX:
    			case INVALID_MESSAGE_ID:
    			case AUTHENTICATION_FAILED:
    			case INTERNAL_ADDRESS_FAILURE:
    			case FAILED_CP_REQUIRED:
    				//
    				// When some of these error types received IKE SA shall
    				// corresponding IKE SA shall be deleted
    				//
    				iDeleteIkeSA = ETrue;
    				break;

    			case NO_PROPOSAL_CHOSEN:
    			case SINGLE_PAIR_REQUIRED:
    			case NO_ADDITIONAL_SAS:				
    			case TS_UNACCEPTABLE:
    			case INVALID_SELECTORS:
    				//
    				// When some of these error types received within
    				// IKE_AUTH or CREATE_CHILD_SA exchange (in response)
    				// Child SA request is interpreted to be failed 
    				//
    				if ( ((aExchange == IKE_AUTH) || (aExchange == CREATE_CHILD_SA) ) && !aRequest )
    				   iChildSARejected = ETrue;
    				break;
    				
    			default:
    				break;	
    		    }
		    }
	
	    i++;   
	}	
			
	return ETrue;
}

TBool CIkev2Negotiation::ProcessCookieL(const CArrayFixFlat<TNotifPayloadIkev2*>& aNotifys, TBool aRequest)
{

	//
	// Special handling for COOKIE Notify payload.
	// The following actions are taken:
	// - Assure that the first Notify payload in array is cookie
	// - When the COOKIE is received in response (aRequest = EFalse)
	//   - Retransmit IKE_SA_INIT request again in format:
	//     HDR(A,0), N(COOKIE), SAi1, KEi, Ni, [Nat Notifies]
	// - When the COOKIE is received in request (aRequest = ETrue)
	//   - Assure that COOKIE returned by the initiator is the we
	//     have earlier transmitted.  
	//			   		
	if ( aNotifys.Count() )
	{
		const TNotifPayloadIkev2* NotifyPayload = aNotifys.At(0);
		if ( NotifyPayload->GetMsgType() == COOKIE && !aRequest)
		{
        //
        // Local end COOKIE usage has not been implemented yet
        //

            //
            // Init a new IKE message buffer and copy received COOKIE
            // Notify to the first payload. Concatenate then all
            // payloads from original IKE_SA_INIT request to this new
            // IKE message (and set next payload field in Notify)
            //
            DEBUG_LOG1(_L("Cookie received, IKE_SA_INIT repeated: %d"), iState);
            if ( iCookieReturned )
            {	
               //
               // One cookie already returned. Avoid cookie-loop
               // by stopping ongoing IKE_SA_INIT exchange  
               //
               DEBUG_LOG(_L("Cookie already returned once, IKE_SA_INIT exchange stopped"));				   
               return EFalse;
            }		
            
            CIkeV2Message* originalIkeSaInitRequest = iHdr.iLastRequest;            
            
            const TPtrC8 cookieData(NotifyPayload->NotifData(), NotifyPayload->NotifDataLength());
            
            // Use protocol id sent by gateway. RFC says id should be zero, but some GWs
            // work against the spec.
            TUint8 protocolId = NotifyPayload->GetProtocolId();
            
            originalIkeSaInitRequest->PrependCookieNotifyPayloadL(protocolId, cookieData);
            iHdr.iLastRequest = NULL; //claims the ownership of the message    
            
            SendIkeMsgL(originalIkeSaInitRequest);
            iTimer->Cancel();    // Reset transmit retry timer                      
            iTimer->IssueRequest(iSendAttempt);     // Start retry timer                        
            iCookieReturned = ETrue;
            return ETrue;
        }
	}   
	DEBUG_LOG1(_L("Cookie required, NO COOKIE Notify found: %d"), iState);
	
	return EFalse;
}



TBool CIkev2Negotiation::ProcessDeletePayloadsL(const CArrayFixFlat<TDeletePlIkev2*>& aDeletes, 
                                                TBool aRequest)
{
	//
	// Process delete payloads received.
	//
	CDesC8ArrayFlat* SpiList = NULL;
	TUint8  Protocol = IKEV2_PROT_NONE;

	for (TInt i = 0; i < aDeletes.Count(); ++i)
	{
		const TDeletePlIkev2* Payload  = aDeletes.At(i);
		Protocol = Payload->GetProtocolId();
		switch ( Protocol )
		{
			case IKEV2_PROTOCOL:
				//
				// Deletion of current existing IKE SA. All IPSEC SA:s
				// negotiated within IKE SA are deleted implicitly 
				//
				iIkeV2PlugInSession.DeleteIkev2SA(iHdr.SaId());
				delete SpiList;
				SpiList = NULL;
				i = aDeletes.Count();   // Stop outer while loop
				iState = KStateIkeDeleteResponse;  // Set next state here
				break;

			case IKEV2_IPSEC_AH:
			case IKEV2_IPSEC_ESP:
				if ( Payload->GetSPISize() == 4 )
				{
				   //
				   // Delete Ipsec SPI:s from IKE SA (and IPSEC SADB)
				   // If Delete payload received within Info request
				   // build inbound SPI list of corresponding inbound
				   // SA:s
				   //
				   TUint SpiCount = (TInt)Payload->GetNbrOfSpis();
				   if ( TPayloadIkev2::Cast(Payload)->GetLength() ==
					  ( TDeletePlIkev2::Size() + 4*SpiCount) )
				   {
                      const TUint8* Spis = Payload->SPIs();
					  if ( aRequest && !SpiList )
					      {
					      SpiList = new (ELeave) CDesC8ArrayFlat(2);
					      CleanupStack::PushL(SpiList);
					      }
					  while ( SpiCount )
					  {	                            
                          TPtrC8 Spi(Spis, 4);
						  if ( SpiList )
						  {	                               
                             const TIkeV2IpsecSAData* IpsecSa = 
                                 iIkeV2PlugInSession.FindIpsecSAData(iHdr.SaId(), Spi, EFalse);
							 if ( IpsecSa && IpsecSa->iSPI_In.Length() > 0 )
							 {                                
                                SpiList->AppendL(IpsecSa->iSPI_In);
							 }	
						  }	 
						  iIkeV2PlugInSession.DeleteIpsecSAData(iHdr.SaId(), Spi, EFalse);
						  Spis += 4;
						  SpiCount--;
					  }	  
				   }
				}   
				break;
			default:
			    break;				
		}
	}
		
	if ( SpiList )
	{
	    //
	    // Build Informational exchange response with a
	    // Delete payload containing SPI:s of corresponding
	    // inbound SA:s.  
	    //        
        CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(),
                                                    iHdr.SpiR(),
                                                    INFORMATIONAL,
                                                    iHdr.iInitiator,
                                                    ETrue,
                                                    iHdr.ExpectedRequestId(), iDebug);
        CleanupStack::PushL(ikeMsg);
        ikeMsg->AppendEncryptedPayloadL(iHdr.iCipherBlkLth);          
        ikeMsg->AppendDeletePayloadL(Protocol, *SpiList);
        CleanupStack::Pop(ikeMsg);
        SendIkeMsgL(ikeMsg);
    	CleanupStack::PopAndDestroy(SpiList); 
		iState = KStateIkeSaCompleted;		
		aRequest = EFalse;
	}
		
	return aRequest;
}

void CIkev2Negotiation::ProcessInvalidKePayloadNotifyL()
{
    // Build and send new IKE_SA_INIT message (request) with another DH group #
    // HDR, SAi1, KEi, Ni, N[NAT_SRC], N[NAT_DST]
    //
    iDHGroupGuess++;
    delete iDHKeys;   // Delete old DH object 
    iDHKeys = NULL;
    iHdr.iDHGroup = 0;
    
    TUint32 lastRequestMsgId = 0;
    if(iHdr.iLastRequest != NULL)
        {
        lastRequestMsgId = iHdr.iLastRequest->MessageId();
        }    
    
    CIkeV2Message* ikeMsg = CIkeV2Message::NewL(iHdr.SpiI(),
                                                iHdr.SpiR(),
                                                IKE_SA_INIT,
                                                iHdr.iInitiator,
                                                EFalse,
                                                lastRequestMsgId,
                                                iDebug); 
    CleanupStack::PushL(ikeMsg);

    HBufC8* saBfr = Ikev2Proposal::FromPolicyToProposaL(iHdr, iSPI_Rekey, iDHGroupGuess);
    CleanupStack::PushL(saBfr);
    ikeMsg->AppendSaPayloadL(*saBfr);
    CleanupStack::Pop(saBfr);
    SetProposedSa(saBfr);

    AppendKEPayloadL(*ikeMsg, iHdr.iDHGroup);
    ikeMsg->AppendNoncePayloadL(*iNonce_I);
    if ( !iHdr.iIkeData->iUseNatProbing )
        {
        delete iNatNotify;
        iNatNotify = NULL;
        
        TInetAddr LocalIp;  
        if ( iHdr.iIkeData->iUseMobIke )
            LocalIp.SetAddress(KInetAddrNone);
        else LocalIp = iHdr.iLocalAddr;        
        iNatNotify = CIkev2NatT::NewL(
            LocalIp, iHdr.iRemoteAddr, IKE_PORT, ikeMsg->InitiatorSpi(), ikeMsg->ResponderSpi());
                       
        ikeMsg->AppendNotifyPayloadL(IKEV2_PROTOCOL, KZeroDesc, NAT_DETECTION_SOURCE_IP, 
                                     iNatNotify->SourceNofify());
        ikeMsg->AppendNotifyPayloadL(IKEV2_PROTOCOL, KZeroDesc, NAT_DETECTION_DESTINATION_IP, 
                                     iNatNotify->DestinNofify());         
        }   
          
    CleanupStack::Pop(ikeMsg);
    SendIkeMsgL(ikeMsg);
    
    iState = KStateIkeSaInitRequest;
}



void CIkev2Negotiation::GetNonceDataL(TBool aInitiator)
{
	//
	//  Get random data for local nonce
	//
	HBufC8* Nonce;
	if ( aInitiator )
	{
		Nonce = HBufC8::NewL(IKEV2_DEF_NONCE_SIZE);
		delete iNonce_I;		
		iNonce_I = Nonce;
	}
	else
	{
		Nonce = HBufC8::NewL(IKEV2_DEF_NONCE_SIZE);
		delete iNonce_R;		
		iNonce_R = Nonce;
	}
    TPtr8 RandOctet(Nonce->Des());	
    RandOctet.SetLength(IKEV2_DEF_NONCE_SIZE);
    TRandom::RandomL(RandOctet);
}

void CIkev2Negotiation::GetOwnIdentityL(TBool aEapIdResponse)
{
    if ( iLocalIdentity )
    {
        return;   // We already have own identity data
    }      

    //
    // The own IKE identity data is built with the following system:
    // -- If Own Certificate exist take try to get identity data from it
    // If no Certificate or identity cannot be get from certificate
    // -- If EAP used use identity speficied by the EAP plugin
    // -- If EAP not used, get own identity data information from current IKE policy
    // If no identity information found use local IP address as
    // identity.  Own IKE Identity information is stored in iLocalIdentity
    // buffer (linked into negotiation object) in format of Identity
    // payload (TIDPayloadIkev2)
    //
  

    HBufC8* IdBfr = NULL;
    TUint8 IdType = ID_NOT_DEFINED;  
    
    
    //Try to get the identity from the user certificate
    if ( iPkiService && iPkiService->UserCertificateData().Length() > 0  )
    {
       IdType = iHdr.iIkeData->iIdType;
       IdBfr  = IkePkiUtils::GetIdentityFromCertL(IdType, iPkiService->UserCertificateData());
       if ( IdBfr != NULL)
       {
           if (IdType == ID_NOT_DEFINED)
           {
               IdType = ID_DER_ASN1_DN;
           }
       }   
       else
       {
           //We didn't get the ID data from the user certificate.
           //Make sure that the type is set to not defined.
           IdType = ID_NOT_DEFINED;
       }
    }
    
    //If we didn't get the identity from the user certificate,
    //try to get it from the EAP plugin. 
    if ( IdType == ID_NOT_DEFINED && iEapPlugin )
    {
        __ASSERT_DEBUG(IdBfr == NULL, User::Invariant());
        //
        // Try to get Own identity data from EAP Plug-in
        //
        IdBfr = iEapPlugin->Identity();
        if ( IdBfr != NULL && IdBfr->Length() )
        {
            //
            // Identity data provided by EAP plug-in. Define IKE Id type
            // value according to Identity data content. If Id data
            // contains realm (format username@realm) Id type
            // ID_RFC822_ADDR  is used. If no realm in ID use type ID_KEY_ID
            //
            TInt offset = IdBfr->Find(_L8("@"));
            IdType = ( offset == KErrNotFound ) ? ID_KEY_ID : ID_RFC822_ADDR; 
        }   
        else
        {
            delete IdBfr;
            IdBfr = NULL;
            if ( !aEapIdResponse )
                return;   // Identity not yet available continue waiting             
        }   
    }
    
    //If we don't have identity so far, try to get it from the
    //policy:
    if ( IdType == ID_NOT_DEFINED && 
         iHdr.iIkeData->iIdType != ID_NOT_DEFINED &&
         iHdr.iIkeData->iFQDN.Length() > 0)
    {   
        __ASSERT_DEBUG(IdBfr == NULL, User::Invariant());
        IdBfr = HBufC8::NewL(iHdr.iIkeData->iFQDN.Length());
        IdBfr->Des().Copy(iHdr.iIkeData->iFQDN);
        IdType = iHdr.iIkeData->iIdType;            
    }
    
    
    //If we have not been able to get the identity so far, we are using the default
    //identity, which is our own IP address.
    if ( IdType == ID_NOT_DEFINED)
    {
        __ASSERT_DEBUG(IdBfr == NULL, User::Invariant());
        if ( (iHdr.iLocalAddr.Family()== KAfInet) || iHdr.iLocalAddr.IsV4Mapped() )
        {
            TUint32 num = ByteOrder::Swap32(iHdr.iLocalAddr.Address());//Put in network order
            IdBfr = HBufC8::NewL(sizeof(num));            
            IdBfr->Des().Append(reinterpret_cast<TUint8*>(&num), sizeof(num));
            IdType = ID_IPV4_ADDR;    
        }
        else   
        {
            IdBfr = HBufC8::NewL(16);  
            const TUint8* pnum = &iHdr.iLocalAddr.Ip6Address().u.iAddr8[0];  //Address in a bytestream
            IdBfr->Des().Append(pnum, 16);
            IdType = ID_IPV6_ADDR;                      
        }          
    }
    
    __ASSERT_DEBUG((IdType != ID_NOT_DEFINED && IdBfr != NULL), User::Invariant());
    CleanupStack::PushL(IdBfr);
    iLocalIdentity = CIkeV2Identity::NewL(IdType, *IdBfr);
    CleanupStack::PopAndDestroy(IdBfr); 
}


void CIkev2Negotiation::GenerateIkeKeysL(TIkev2SAData* aRekeydSaData)
{
	//
	//  Generate IKE keying material. Start by calculating
	//  Diffie-Hellman secret.
	//
	User::LeaveIfNull(iDHPublicPeer);		
	if ( !iDHKeys )
	{	
		iDHKeys = CDHKeys::CreateDHKeyL(iHdr.iDHGroup);
		iDHKeys->XValueL(); // Calculate own DH public value	   
	}
	HBufC8* g_ir = iDHKeys->ComputeAgreedKeyL(iDHPublicPeer->Des());
	CleanupStack::PushL(g_ir);
	delete iDHKeys;
	iDHKeys = NULL;

	HBufC8* Ni_Nr;
	HBufC8* SKEYSEED;
	TUint16 prfAlg(0);
	
	if ( aRekeydSaData )
	{
	    //
	    //  Calculate IKE keying material seed SKEYDSEED = prf(SK_d(old), [g^ir (new)] | Ni | Nr) 
        //
		Ni_Nr = HBufC8::NewL(g_ir->Length() + iNonce_I->Length() + iNonce_R->Length());
		CleanupStack::PushL(Ni_Nr);		
		Ni_Nr->Des().Copy(g_ir->Des());		
		Ni_Nr->Des().Append(iNonce_I->Des());
		Ni_Nr->Des().Append(iNonce_R->Des());	

		prfAlg = aRekeydSaData->iPRFAlg;
		SKEYSEED = IkeCrypto::PrfhmacL(*Ni_Nr, aRekeydSaData->iSK_d, prfAlg);
		CleanupStack::PushL(SKEYSEED);
	}
	else
	{	
	    //
	    //  Calculate IKE keying material seed SKEYDSEED = prf(Ni | Nr, g^ir)
	    //
		Ni_Nr = HBufC8::NewL(iNonce_I->Length() + iNonce_R->Length());
		CleanupStack::PushL(Ni_Nr);
		Ni_Nr->Des().Copy(iNonce_I->Des());
		Ni_Nr->Des().Append(iNonce_R->Des());	
		
		prfAlg = iHdr.iPRFAlg;
		SKEYSEED = IkeCrypto::PrfhmacL(*g_ir, *Ni_Nr, prfAlg);
		CleanupStack::PushL(SKEYSEED);		
	}  

	g_ir->Des().FillZ(); // Wipe out shared secret value from buffer
	
	iHdr.GenerateIkeKeyDerivatesL(SKEYSEED->Des(),prfAlg, *iNonce_I, *iNonce_R);	
	SKEYSEED->Des().FillZ(); // Wipe out SKEYSEED value from buffer	
	
	CleanupStack::PopAndDestroy(3);  //g_ir , Ni_Nr and SKEYSEED
}


void CIkev2Negotiation::SaveSignedDataL(TBool aLocal,  const TDesC8& aIkeMsg)
{ 
	//
	//  Allocate buffer for signed octets needed for IKE SA
	//  authentication with AUTH payload.
	//  The signed octet contains the following data:
	//  Initiator:
	//  - IKE_SA_INIT message content (message number 1)
	//    concatenated with responder nonce data and value
	//    prf(SK_pi,IDi") where IDi" is initiator ID data without fixed
	//    payload header
	//
	//  Responder:
	//  - IKE_SA_INIT message content (message number 2)
	//    concatenated with initiator nonce data and value
	//    prf(SK_pr,IDr") where IDr" is responder ID data without fixed
	//    payload header
	//
	TInt SignedLth = aIkeMsg.Length(); // Initial value
	HBufC8*  Nonce;
	HBufC8** SignedBfrPtr;	
	if ( aLocal )
	{	
	   if ( iHdr.iInitiator )
	   {
		  SignedBfrPtr = &iAuthMsgInit;
	 	  Nonce = iNonce_R;
	   }	  
	   else {
		  SignedBfrPtr = &iAuthMsgResp;		   
		  Nonce = iNonce_I;
	   }		  
	}
	else
	{
	   if ( iHdr.iInitiator )
	   {
		  SignedBfrPtr = &iAuthMsgResp;		   
		  Nonce = iNonce_I;
	   }		
	   else
	   {
		  SignedBfrPtr = &iAuthMsgInit;		   
		  Nonce = iNonce_R;
	   }	  
	}

	SignedLth += Nonce->Length() + IkeCrypto::AlgorithmInfo(IKEV2_PRF, iHdr.iPRFAlg);
	HBufC8* Signed = HBufC8::NewL(SignedLth);
	Signed->Des().Copy(aIkeMsg);
	Signed->Des().Append(Nonce->Des());
	
	if ( aLocal && iLocalIdentity )
	{
	   //
	   // Add value prf(SK_px,IDx") into local signed data buffer end
	   //
	   AddIdToSignedDataL(ETrue, Signed, iLocalIdentity->PayloadData());
	}
	
	delete *SignedBfrPtr;
	*SignedBfrPtr = Signed;
}


void CIkev2Negotiation::AddIdToSignedDataL(TBool aLocal, HBufC8* aSigned, const TDesC8& aIdData)
{
    ASSERT(aSigned);
    //
    // Add value prf(SK_px,IDx") into signed data buffer end
    //

	HBufC8* signedIdData = NULL;;
	if ( iHdr.iInitiator )
	{
		if ( aLocal )
		    {
		    signedIdData = IkeCrypto::PrfhmacL(aIdData, iHdr.iSK_pi, iHdr.iPRFAlg);
		    }
		else 
		    {
		    signedIdData = IkeCrypto::PrfhmacL(aIdData, iHdr.iSK_pr, iHdr.iPRFAlg);
		    }
	}		
	else
	{
		if ( aLocal )
		    {
		    signedIdData = IkeCrypto::PrfhmacL(aIdData, iHdr.iSK_pr, iHdr.iPRFAlg);
		    }
		else 
		    {
		    signedIdData = IkeCrypto::PrfhmacL(aIdData, iHdr.iSK_pi, iHdr.iPRFAlg);
		    }
	}
	aSigned->Des().Append(*signedIdData);
	delete signedIdData;	
}	

HBufC8* CIkev2Negotiation::SignAuthDataL(const TDesC8& aAuthData, TUint8 aAuthMethod)
{
	//
	//  Sign aMsgData according to authentication method parameter
	//
	HBufC8* signedAuthData = NULL;
	
	if ( iPkiService && 
	     iPkiService->TrustedCaName().Length() > 0 &&
	     iPkiService->UserCertificateData().Length() > 0 )
	{		
	   //
	   // Message data <msg octets> is signed using private key
	   //		
		TPtrC8 TrustedCa(iPkiService->TrustedCaName());
		signedAuthData = HBufC8::NewLC(320); // reserved for sign (aware for 2048 bits signatures)
		TPtr8 signedAuthDataPtr(signedAuthData->Des()); 
		
		iPkiService->Ikev2SignatureL(TrustedCa, iHdr.iIkeData->iOwnCert, aAuthData, signedAuthDataPtr, aAuthMethod);
		CleanupStack::Pop(signedAuthData);
	}
	else		
	{
	   //
	   // Message data is signed using negotiated PRF function as
	   // follows:
	   // AUTH =
	   // prf(prf(Shared Secret,"Key Pad for IKEv2"), <msg octets>)
	   // If EAP method that creates a shared key as a side effect of
	   // authentication used, this shared key is used as Shared Secret
	   // Otherwise preshared key configured into policy is used as
	   // Shared secret.
	   //
	   if ( !iPresharedKey )
		  iPresharedKey = Ikev2Proposal::GetPSKFromPolicyL(iHdr.iIkeData);
	   //
	   // Calculate KEY = prf(Shared Secret,"Key Pad for IKEv2")
	   //   			
	   HBufC8* PskKey = IkeCrypto::PrfhmacL(KIkev2PSKData, *iPresharedKey, iHdr.iPRFAlg);
	   CleanupStack::PushL(PskKey);
	   //
	   // Calculate prf(KEY, <msg octets>)
	   //       	   
	   signedAuthData = IkeCrypto::PrfhmacL(aAuthData, *PskKey, iHdr.iPRFAlg);
 	   CleanupStack::PopAndDestroy(PskKey);
	}
	return signedAuthData;		
}

TBool CIkev2Negotiation::AddIdAndAuthenticatePeerL(CIkev2Payloads* aIkeMsg)
{
    ASSERT(aIkeMsg);
	//
	// Verify that authentication payload of peer is correct
	// To do this the signed data octets of peer must be filled with
	// value: prf(SK_px,IDx")
	// So the peer ID payload must be verified first.
	//
	HBufC8* Signed;
	TIDPayloadIkev2* Id;
	if ( iHdr.iInitiator )
	{
	   Signed = iAuthMsgResp;	
	   Id = (TIDPayloadIkev2*)aIkeMsg->iIdR;
	}   
	else
	{
	   Signed = iAuthMsgInit;			
	   Id = (TIDPayloadIkev2*)aIkeMsg->iIdI;
	}	
    if ( !Signed || !Id )
	   return EFalse;	

	if ( !iPeerIdInSignedData )
	{	
	   TUint16 IdLth = TPayloadIkev2::Cast(Id)->GetLength();
	   if ( IdLth < TIDPayloadIkev2::Size() ) 
	   {
		  DEBUG_LOG1(_L("Peer ID payload too short; Length %d"), IdLth);		  
		  return EFalse;
	   }
       //
	   // Add value prf(SK_px,IDx") into peer signed data buffer end
	   //
	   TPayloadIkev2* idPayload = TPayloadIkev2::Cast(Id);
	   TPtrC8 idPtr(idPayload->PayloadData(), (idPayload->GetLength() - TPayloadIkev2::Size()));
	   AddIdToSignedDataL(EFalse, Signed, idPtr);
	   iPeerIdInSignedData = ETrue;
	}
	
	return AuthenticatePeerL(aIkeMsg->iAuth); 

}

TBool CIkev2Negotiation::AuthenticatePeerL(TAuthPayloadIkev2* aAuth)
{
	//
	// Authenticate peer tication payload of peer is correct
	// To do this the signed data octets of peer must be filled with
	// value: prf(SK_px,IDx")
	// So the peer ID payload must be verified first.
	//
	HBufC8* Signed;	
	if ( iHdr.iInitiator )
		 Signed = iAuthMsgResp;	
	else Signed = iAuthMsgInit;
	
	if ( !Signed || !aAuth )
	   return EFalse;

	TUint16 AuthLth = TPayloadIkev2::Cast(aAuth)->GetLength();	
	if ( AuthLth < TAuthPayloadIkev2::Size() ) 
	{					
		DEBUG_LOG1(_L("Peer Auth payload too short; Length %d"), AuthLth);
		return EFalse;
	}
	AuthLth = (TUint16)(AuthLth - TAuthPayloadIkev2::Size());
	TBool Status = EFalse;
	
	if ( aAuth->GetAuthMethod() == PRESHARED_KEY )
	{
       DEBUG_LOG(_L("Authenticating SGW with PSK"));
	   //
	   // Pre shared key authentication is not accepted for peer if we
	   // have requested an certificate from peer (PKI authentication
	   // required) 
	   // 
		if ( !iPkiAuthRequired )
		{
			HBufC8* AuthRef = SignAuthDataL(*Signed, PRESHARED_KEY);
			CleanupStack::PushL(AuthRef);			
			if (  AuthRef->Length() == AuthLth )
			{
				Status = (Mem::Compare(AuthRef->Ptr(), AuthRef->Length(), aAuth->AuthData(), AuthLth ) == 0);		   
			}
			CleanupStack::PopAndDestroy();   // AuthRef			
		}	  
	}
	else
	{
	   //
	   // Authentication based on PKI (private key signature)
	   // 
		if ( iPkiService && iPeerCert )
		{
		    DEBUG_LOG(_L("Authenticating SGW with certs"));
		    
			TPtrC8 AuthData(Signed->Des());
			TPtrC8 Signature(aAuth->AuthData(), AuthLth);	   						
			Status = IkePkiUtils::VerifyIkev2SignatureL(Signature, AuthData, *iPeerCert);
			iPkiAuthRequired = EFalse;
		}			  
	}
	if (Status)
	    {
	    DEBUG_LOG(_L("SGW authentication success"));
	    }
	else
	    {
	    DEBUG_LOG(_L("SGW authentication failed"));
	    }
	
	return Status; 
}


TBool CIkev2Negotiation::VerifyPeerCertificateL(CArrayFixFlat<TCertPayloadIkev2*>* aCerts, TIDPayloadIkev2* aId )
{
	TBool Status        = EFalse;
	
	const CIkeCaList& trustedCaList = iPkiService->CaList();
	CX509Certificate* PeerCert = IkePkiUtils::VerifyCertificateL(*aCerts, trustedCaList);
	
	if ( PeerCert && aId )
	{
		CleanupStack::PushL(PeerCert);					 		
		TPtrC8 IdData(aId->IdData(), (TPayloadIkev2::Cast(aId)->GetLength() - TIDPayloadIkev2::Size()));	
		Status = IkePkiUtils::CertifyIdentityL(PeerCert, IdData, (TInt)aId->GetIdType());
		if ( Status )
		{
		    DEBUG_LOG(_L("IDr matches the SGW certificate"));
		    if (iRemoteIdentity && !iHdr.iIkeData->iSkipRemoteIdCheck ) //iRemoteIdentity if the REMOTE_IF from the policy
		        {
		        //TIDPayloadIkev2* peerIdentityPayload = TIDPayloadIkev2::Cast(iRemoteIdentity->Ptr());  
		        if (iRemoteIdentity->IdType() == aId->GetIdType())
		            {
		            TPtrC8 idPtr(aId->IdData(),
		                         TPayloadIkev2::Cast(aId)->GetLength() - TIDPayloadIkev2::Size());
		            TPtrC8 peerIdentityPtr(iRemoteIdentity->Identity());
		            
		            //Check if we accept partial remote id
		            if (iHdr.iIkeData->iAcceptPartialRemoteId && 
		                iRemoteIdentity->IdType() == ID_FQDN &&
		                peerIdentityPtr.Length() > idPtr.Length())
		                {
		                DEBUG_LOG(_L("Using PARTIAL_REMOTE_ID_CHECK"));
		                peerIdentityPtr.Set(peerIdentityPtr.Right(idPtr.Length()));
		                }
		            if (idPtr.Compare(peerIdentityPtr) == 0)
		                {
		                DEBUG_LOG(_L("IDr matches the REMOTE_ID"));
		                Status = ETrue;
		                }
		            else
		                {
		                DEBUG_LOG(_L("IDr does not match the REMOTE_ID"));
		                Status = EFalse;
		                }
		            }		        
		        else
		            {
		            DEBUG_LOG(_L("IDr payload ID does not match REMOTE_ID_TYPE"));
		            Status = EFalse;
		            }
		        }
		}	
	    else 
	    {
	        DEBUG_LOG(_L("IDr does not match the SGW certificate"));
	    }
		
		if ( Status )
		{
		   CleanupStack::Pop(PeerCert);		
		   delete iPeerCert;
		   iPeerCert = PeerCert;			
		}
		else CleanupStack::PopAndDestroy(PeerCert);
	}
	return Status;
}


TBool CIkev2Negotiation::ProcessKeyExchangeL(TKEPayloadIkev2* aKePayload, TUint16 aGroup)
{
	//
	//  Process key exchange payload received from peer
	//
	if ( !aKePayload )
	{
		DEBUG_LOG1(_L("Key Exchange payload not present, required Group %d"), aGroup);
		SetNotifyCode(INVALID_KE_PAYLOAD);
		StoreNotifyData16(aGroup);
		return EFalse;
	}	 
	TUint16 PlLth = TPayloadIkev2::Cast(aKePayload)->GetLength();
	if (( PlLth <= TKEPayloadIkev2::Size() ) || ( aKePayload->GetDHGroup() != aGroup ))
	{
		DEBUG_LOG1(_L("Peer Key Exchange DH group does not match, Group %d"), aKePayload->GetDHGroup());
		SetNotifyCode(INVALID_KE_PAYLOAD);
		StoreNotifyData16(aGroup);
	    return EFalse;
	}	
	if ( !iDHKeys )
		iDHKeys = CDHKeys::CreateDHKeyL(aGroup);
	PlLth = (TUint16)(PlLth - TKEPayloadIkev2::Size());
	if ( PlLth != iDHKeys->ModulusLength() )
	{
		DEBUG_LOG1(_L("Peer DH public value length does not match group, Length %d"), PlLth);
		SetNotifyCode(INVALID_KE_PAYLOAD);
        StoreNotifyData16(aGroup);
		return EFalse;
	}	
	delete iDHPublicPeer;
	iDHPublicPeer = NULL;
	iDHPublicPeer = HBufC8::NewL(PlLth);
	iDHPublicPeer->Des().Copy(aKePayload->DHPublic(), PlLth);

	return ETrue;
}

void CIkev2Negotiation::AppendKEPayloadL(CIkeV2Message& aIkeMsg, TUint16 aDHGroup)
{    
	if ( !iDHKeys )
       iDHKeys = CDHKeys::CreateDHKeyL(aDHGroup);

	iDHKeys->XValueL(); // Calculate own DH public value
	HBufC8* dHPublic = iDHKeys->GetPubKey();    //save the public key in a buffer to have easy access
	User::LeaveIfNull(dHPublic);
	CleanupStack::PushL(dHPublic);
	
	TInt modulusLength = iDHKeys->ModulusLength();	
	HBufC8* kePayloadData = HBufC8::NewLC(modulusLength);
	TPtr8 kePayloadDataPtr(kePayloadData->Des());
	
	__ASSERT_DEBUG(modulusLength == dHPublic->Length(), User::Invariant());	

	kePayloadDataPtr.Append(*dHPublic);	
	kePayloadDataPtr.SetLength(modulusLength); //adds zero padding, if needed

	aIkeMsg.AppendKePayloadL(aDHGroup, *kePayloadData);
	
	CleanupStack::PopAndDestroy(kePayloadData);  
	CleanupStack::PopAndDestroy(dHPublic);
}

     

TBool CIkev2Negotiation::CheckPayloadsOrder(CIkev2Payloads* aIkeMsg, TUint8 aExchange, TBool aResponse)
    {
	switch ( aExchange )
	    {
		case IKE_SA_INIT:
		    if(!aIkeMsg->iSa || !aIkeMsg->iKe || !aIkeMsg->iNonce) return EFalse;
            if(aIkeMsg->iSa->GetNextPayload() != IKEV2_PAYLOAD_KE) return EFalse;
            if(aIkeMsg->iKe->GetNextPayload() != IKEV2_PAYLOAD_NONCE) return EFalse;
			break;
			
		case IKE_AUTH:
		    if(!iEapPlugin)
		        {
		        if(!aIkeMsg->iEncr || !aIkeMsg->iAuth || !aIkeMsg->iSa || !aIkeMsg->iTsI || !aIkeMsg->iTsR)
		            {
           		    DEBUG_LOG(_L("1"));
		            return EFalse;
		            }
                if(aIkeMsg->iSa->GetNextPayload() != IKEV2_PAYLOAD_TS_I)
                    {
           		    DEBUG_LOG(_L("2"));                    
                    return EFalse;
                    }
                if(aIkeMsg->iTsI->GetNextPayload() != IKEV2_PAYLOAD_TS_R)
                    {
           		    DEBUG_LOG(_L("3"));      
                    return EFalse;
                    }

		        if(aResponse)
		            {
		            if(!aIkeMsg->iIdR)
		                {
           		        DEBUG_LOG(_L("4"));		                
		                return EFalse;
		                }
                    if(!aIkeMsg->iCerts || aIkeMsg->iCerts->Count() == 0)
                        {    
                        if(aIkeMsg->iIdR->GetNextPayload() != IKEV2_PAYLOAD_AUTH)
                            {
                   		    DEBUG_LOG(_L("5"));                    
                            return EFalse;
                            }
                        }
		            else
		                {
                        if(aIkeMsg->iIdR->GetNextPayload() != IKEV2_PAYLOAD_CERT)
                            {
                   		    DEBUG_LOG(_L("6"));                    
                            return EFalse;
                            }
                        TInt c = aIkeMsg->iCerts->Count();
                        if(aIkeMsg->iCerts->At(c-1)->GetNextPayload() != IKEV2_PAYLOAD_AUTH)
                            {
                   		    DEBUG_LOG(_L("7"));
                            return EFalse;
                            }
                        }
		            }
		        else
		            {
		            if(!aIkeMsg->iIdI)
		                {
               		    DEBUG_LOG(_L("8"));	                
		                return EFalse;
		                }
		            if(aIkeMsg->iCerts && aIkeMsg->iCerts->Count() != 0)
		                {
		                if(aIkeMsg->iIdI->GetNextPayload() != IKEV2_PAYLOAD_CERT)
		                    {
                   		    DEBUG_LOG(_L("9"));		                    
		                    return EFalse;
		                    }
		                }
                    if(aIkeMsg->iCertReqs && aIkeMsg->iCertReqs->Count() != 0)
                        {
                        TInt c = aIkeMsg->iCertReqs->Count();
                        if(aIkeMsg->iIdR && aIkeMsg->iCertReqs->At(c-1)->GetNextPayload() != IKEV2_PAYLOAD_ID_R)
                            {
                   		    DEBUG_LOG(_L("10"));                            
                            return EFalse;
                            }
                        if(!aIkeMsg->iIdR && aIkeMsg->iCertReqs->At(c-1)->GetNextPayload() != IKEV2_PAYLOAD_AUTH)
                            {
                   		    DEBUG_LOG(_L("11"));                            
                            return EFalse;
                            }
                        }
                    if(aIkeMsg->iIdR && aIkeMsg->iIdR->GetNextPayload() != IKEV2_PAYLOAD_AUTH)
                        {
               		    DEBUG_LOG(_L("12"));                        
                        return EFalse;
                        }
		            }
		        }
			break;
			
		case CREATE_CHILD_SA:
            if(!aIkeMsg->iEncr || !aIkeMsg->iSa || !aIkeMsg->iNonce) return EFalse;
            if(aIkeMsg->iSa->GetNextPayload() != IKEV2_PAYLOAD_NONCE) return EFalse;
            if(aIkeMsg->iKe && aIkeMsg->iNonce->GetNextPayload() != IKEV2_PAYLOAD_KE) return EFalse;
            if(aIkeMsg->iTsI)
                {
                if(aIkeMsg->iKe && aIkeMsg->iKe->GetNextPayload() != IKEV2_PAYLOAD_TS_I) return EFalse;
                if(!aIkeMsg->iKe && aIkeMsg->iNonce->GetNextPayload() != IKEV2_PAYLOAD_TS_I) return EFalse;
                if(aIkeMsg->iTsI->GetNextPayload() != IKEV2_PAYLOAD_TS_R) return EFalse;
                }
			break;

		default:
			break;
	    }
    DEBUG_LOG(_L("13"));
	return ETrue;
    }


TBool CIkev2Negotiation::Stopped() 
    { 
    return iStopped; 
    }


TBool CIkev2Negotiation::ImplicitChildSa() 
    { 
    return (iState < KStateIkeSaCompleted); 
    }


HBufC8* CIkev2Negotiation::PeekProposedSa() 
    { 
    return iProposedSA; 
    }


HBufC8* CIkev2Negotiation::GetProposedSa()  
    { 
    HBufC8* Sa = iProposedSA; 
    iProposedSA = NULL; 
    return Sa; 
    }


void CIkev2Negotiation::SetProposedSa(HBufC8* aSaPl) 
    { 
    delete iProposedSA; 
    iProposedSA = aSaPl; 
    }


CIkev2Acquire** CIkev2Negotiation::GetAcquireQue() 
    { 
    return &iAcquireFirst; 
    }


CIkev2Expire** CIkev2Negotiation::GetExpireQue() 
    { 
    return &iExpireFirst; 
    }


TBool CIkev2Negotiation::RequestsPending() 
    { 
    return (iAcquireFirst || iExpireFirst); 
    }


void CIkev2Negotiation::SetNotifyCode(TInt aMsgType) 
    { 
    if (iNotifyCode == 0) 
        iNotifyCode = aMsgType; 
    }


TInt CIkev2Negotiation::GetNotifyCode() 
    { 
    return iNotifyCode; 
    }


void CIkev2Negotiation::StoreNotifyData32(TUint32 aData) 
    {
    PUT32(iNotifyData, aData); 
    iNotifyDataLth = 4;
    }


void CIkev2Negotiation::StoreNotifyData16(TUint16 aData) 
    {
    PUT16(iNotifyData, aData); 
    iNotifyDataLth = 2;
    }               


TUint8* CIkev2Negotiation::NotifyData(TInt& aDataLth)
    {
    aDataLth = iNotifyDataLth;
    if ( iNotifyDataLth )
        return iNotifyData;
    else return NULL;
    }


TInetAddr CIkev2Negotiation::GetLocalAddr() const
    {
    if ( iHdr.iVirtualAddr.IsUnspecified() )
        {
        return iHdr.iLocalAddr;
        }
    else
        {
         return iHdr.iVirtualAddr;
        }
    }

