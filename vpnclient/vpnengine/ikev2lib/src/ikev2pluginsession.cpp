/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <random.h>

#include "ikev2pluginsession.h"
#include "ikev2plugin.h"
#include "ikev2Negotiation.h"
#include "ikepolparser.h"
#include "ikedebug.h"
#include "ikev2SA.h"
#include "ikev2SAdata.h"
#include "ikedatainterface.h"
#include "ipsecsadata.h"
#include "ikev2pfkey.h"
#include "ipsecsalist.h"
#include "ipsecpolicyutil.h"
#include "ikev2messagesendqueue.h"


CIkev2PluginSession* CIkev2PluginSession::NewL( TUint32 aVpnIapId,
                                                TUint32 aVpnNetId,
                                                TUint32 aVpnInterfaceIndex,
                                                MIkeDataInterface& aDataInterface,
                                                CIkev2PlugIn& aPlugin, 
                                                CPFKeySocketIf& aPfKeySocketIf,
                                                CIpsecPolicyUtil& aIpsecPolicyUtil,
                                                MKmdEventLoggerIf& aEventLogger,
                                                MIkeDebug& aDebug )
    {
    CIkev2PluginSession* self = new (ELeave) CIkev2PluginSession( aVpnIapId, aVpnNetId, 
                                                                  aVpnInterfaceIndex, aDataInterface,
                                                                  aPlugin, aPfKeySocketIf, aIpsecPolicyUtil,
                                                                  aEventLogger, aDebug );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }


CIkev2PluginSession::CIkev2PluginSession( TUint32 aVpnIapId,
                                          TUint32 aVpnNetId,
                                          TUint32 aVpnInterfaceIndex,
                                          MIkeDataInterface& aDataInterface,
                                          CIkev2PlugIn& aPlugin,
                                          CPFKeySocketIf& aPfKeySocketIf,
                                          CIpsecPolicyUtil& aIpsecPolicyUtil,
                                          MKmdEventLoggerIf& aEventLogger,
                                          MIkeDebug& aDebug )
: iVpnIapId(aVpnIapId), iVpnNetId(aVpnNetId),  iDataInterface(aDataInterface), iPlugin(aPlugin), 
  iPfKeySocketIf(aPfKeySocketIf), iIpsecPolicyUtil(aIpsecPolicyUtil), iEventLogger(aEventLogger), 
  iDebug(aDebug), iVpnInterfaceIndex(aVpnInterfaceIndex) 
    {
    }


void CIkev2PluginSession::ConstructL()
    {
    TPtr8 ptr((TUint8*)&iSAIdSeed, sizeof(iSAIdSeed));
    ptr.SetLength(sizeof(iSAIdSeed));
    TRandom::RandomL(ptr);  
    iSAIdSeed &= 0x7fffffff;  // Reset the most significant bit
    DEBUG_LOG1(_L("CIkev2Plugin::ConstructL, SAId seed: %d"), iSAIdSeed );
    }


CIkev2PluginSession::~CIkev2PluginSession()
    {
    //Makes sure that all the negotiations and
    //Sa data structures are deleted:
    while ( iFirstNegotiation )
        {
        CIkev2Negotiation* negotiation = iFirstNegotiation;
        iFirstNegotiation = iFirstNegotiation->iNext;
        
        delete negotiation;
        }

    while(iFirstIkev2SA)
        {
        CIkev2SA* ikeV2Sa = iFirstIkev2SA;
        iFirstIkev2SA = ikeV2Sa->iNext;
        
        delete ikeV2Sa;
        }        
    
    delete iMessageSendQue;
    delete iReceiver;    
    delete iIkeData; 
    delete iDeactivationTimer;    
    
    iPlugin.PluginSessionDeleted(this);
    }


void CIkev2PluginSession::NegotiateWithHost( const CIkeData& aIkeData,
                                             TVPNAddress& aInternalAddress,
                                             TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG(iClientStatusNegotiate == NULL,
                   User::Invariant());
    
    iClientStatusNegotiate = &aStatus;
    *iClientStatusNegotiate = KRequestPending;
        
    iInternalAddress = &aInternalAddress;
    
    TRAPD(err, DoNegotiateWithHostL(aIkeData));
    if (err != KErrNone)
        {
        DoCompleteNegotiateWithHost(err);
        }    
    }


void CIkev2PluginSession::DoNegotiateWithHostL( const CIkeData& aIkeData )
    {
    iIkeData = CIkeData::NewL(&aIkeData);
    
    iReceiver = CIkev2Receiver::NewL( iDataInterface,
                                      *this );   
    
    iMessageSendQue = CIkev2MessageSendQueue::NewL(iDataInterface, 
                                                   iIkeData->iAddr, 
                                                   iIkeData->iDscp,
                                                   iIkeData->iNatKeepAlive,
                                                   iDebug);    
    
    
    TInetAddr physicalAddr;
    iDataInterface.GetLocalAddress(physicalAddr);
    TInetAddr sgwAddr(iIkeData->iAddr);       
    
    // Negotiation ownership is transferred to the plugin
    // before leave can occur.
    iSAIdSeed++;
    
    if (aIkeData.iUseInternalAddr)
        {
        CIkev2Negotiation* Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, iEventLogger,
                                                                 *iMessageSendQue, iDebug, iIkeData, 
                                                                 iVpnIapId, iSAIdSeed,
                                                                 physicalAddr, sgwAddr);

        Negotiation->StartIkeSANegotiationL();
        }
    else
        {
        //If internall addressing is not in use, we do not do anything else
        //in this phase. The actual IKE negotiation is trickered by an Acquire
        //PFKEY message from the IPsec, when there is actual data between the SGW and
        //the phone.
        DoCompleteNegotiateWithHost( KErrNone);
        }
    }


void CIkev2PluginSession::CancelNegotiateWithHost()
    {
    
    if (iClientStatusNegotiate != NULL)
        {
        //If the Negotiate with host is cancelled we pretty much do a silent close
        //for the connection

        while ( iFirstNegotiation )
            {
            CIkev2Negotiation* negotiation = iFirstNegotiation;
            iFirstNegotiation = iFirstNegotiation->iNext;
            
            delete negotiation;
            }

        while(iFirstIkev2SA)
            {
            CIkev2SA* ikeV2Sa = iFirstIkev2SA;
            iFirstIkev2SA = ikeV2Sa->iNext;
            
            delete ikeV2Sa;
            }
        DoCompleteNegotiateWithHost(KErrCancel);
        }            
    }


void CIkev2PluginSession::DeleteSession( const TBool aSilentClose,
                                         TRequestStatus& aStatus )
    {
    DEBUG_LOG1(_L("Deactivating IKE SA:s for vpn iap %d"), iVpnIapId);
    
    __ASSERT_DEBUG(iClientStatusDelete == NULL, User::Invariant());
    iClientStatusDelete = &aStatus;
    *iClientStatusDelete = KRequestPending;
    
    TInt err = KErrNone;
    TBool doSilentClose = aSilentClose;
    //Deletes all ongoing ike negotiations    
    while ( iFirstNegotiation )
        {
        CIkev2Negotiation* negotiation = iFirstNegotiation;
        iFirstNegotiation = iFirstNegotiation->iNext;
        
        delete negotiation;
        }
    
    TBool deactivating = EFalse;
    while(iFirstIkev2SA)
        {
        CIkev2SA* ikeV2Sa = iFirstIkev2SA;
        iFirstIkev2SA = ikeV2Sa->iNext;
        
        if (!doSilentClose)
            {
            TRAP(err, DoDeleteIkeSAExhangeL(ikeV2Sa->iIkeV2SaData));
            if (err == KErrNone)
                {
                deactivating = ETrue;
                }
            else
                {
                //If we can't start the IKE SA delete exhange, 
                //we do following expection handling:
                //1. Possible already active delete exhanges can continue as they were.
                //2. The IKE SA, which delete exchange failured, is deleted silently.
                //3. The rest of the IKE SAs are deleted silently.
                //4. The caller is notified with the error returned by the failed delete 
                //   exchange attempt, if no delete exhanges are in progress.
                //5. If there is ongoing delete exhange(s), the caller is notified with the
                //   status of last delete exhange, which completes.
                DEBUG_LOG1(_L("CIkev2PluginSession::DeleteSession: Can't start IKE SA delete exhange (%d)"), 
                           err );
                doSilentClose = ETrue;
                }
            }
        delete ikeV2Sa;
        }
    
    if (deactivating)
        {
        TRAP( err, iDeactivationTimer = CIkev2DeactivationTimer::NewL(*this) );
        }
    
    if (deactivating &&
        err == KErrNone)
        {
        iDeactivationTimer->IssueRequest();
        }
    else
        {
        delete iIkeData;
        iIkeData = NULL;
        DoCompleteDeleteSession(err);        
        }    
    }


void CIkev2PluginSession::DoDeleteIkeSAExhangeL(TIkev2SAData& aIkev2SAdata)
    {
    DEBUG_LOG1(_L("Deleting IKE SA SAID =  %d"), aIkev2SAdata.SaId());
                    
    CIkev2Negotiation* negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, 
                                                             iEventLogger, *iMessageSendQue, 
                                                             iDebug, aIkev2SAdata);
    CleanupStack::PushL(negotiation);
    negotiation->StartIkeSADeleteL();
    CleanupStack::Pop(negotiation);
    __ASSERT_DEBUG( !negotiation->Stopped(), User::Invariant() );    
    }


void CIkev2PluginSession::CancelDeleteSession()
    {
    if (iClientStatusDelete != NULL)
        {
        //If the delete sessionis cancelled we pretty much do a silent close
        //for the connection
        iMessageSendQue->CancelAll();
        iReceiver->Cancel();    
        delete iDeactivationTimer;
        iDeactivationTimer = NULL;
        
        while ( iFirstNegotiation )
            {
            CIkev2Negotiation* negotiation = iFirstNegotiation;
            iFirstNegotiation = iFirstNegotiation->iNext;
            
            delete negotiation;
            }

        while(iFirstIkev2SA)
            {
            CIkev2SA* ikeV2Sa = iFirstIkev2SA;
            iFirstIkev2SA = ikeV2Sa->iNext;
            
            delete ikeV2Sa;
            }
        DoCompleteDeleteSession(KErrCancel);
        }            
    }


void CIkev2PluginSession::NotifyError( TRequestStatus& aStatus )
    {
    aStatus = KRequestPending;
    iClientStatusNotifyError = &aStatus;
    }

void CIkev2PluginSession::CancelNotifyError()
    {    
    if (iClientStatusNotifyError != NULL)
        {
        DoCompleteNotifyError(KErrCancel);
        }
    }


void CIkev2PluginSession::NotifyInternalAddressChanged( TVPNAddress& aInternalAddress,
                                                        TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG(iClientStatusInternalAddressChange == NULL,
                    User::Invariant());    
    
    __ASSERT_DEBUG(iChangedInternalAddress == NULL,
                    User::Invariant());    
    

    iClientStatusInternalAddressChange = &aStatus;
    *iClientStatusInternalAddressChange = KRequestPending;
    
    iChangedInternalAddress = &aInternalAddress;
    }


void CIkev2PluginSession::CancelNotifyInternalAddressChanged()
    {
    if (iClientStatusInternalAddressChange != NULL)
        {
        __ASSERT_DEBUG(iChangedInternalAddress != NULL, User::Invariant());
        iChangedInternalAddress = NULL;
        User::RequestComplete(iClientStatusInternalAddressChange, KErrCancel);
        }
    }


void CIkev2PluginSession::LinkNegotiation(CIkev2Negotiation* aNegotiation)
{  
    ASSERT(aNegotiation); 
    aNegotiation->iNext = iFirstNegotiation;  
    iFirstNegotiation = aNegotiation; 
}


void CIkev2PluginSession::RemoveNegotiation(CIkev2Negotiation* aNegotiation)
    {
        CIkev2Negotiation* Prev = NULL;
        CIkev2Negotiation* Neg  = iFirstNegotiation;
        
        while ( Neg )
        {
            if ( Neg == aNegotiation )
            {
               if ( Prev )
                    Prev->iNext = Neg->iNext;
               else iFirstNegotiation = Neg->iNext;
               break;  
            }
            Prev = Neg;
            Neg  = Neg->iNext;
        }   
    }

//
// Find an IKEv2 SA using SA Id as search argument
//
CIkev2SA* CIkev2PluginSession::FindIkev2SA(TUint32 aSAId, TInt aRequiredState, TInt aNewState)
{
    CIkev2SA* Sa = iFirstIkev2SA;
    while ( Sa )
    {
        if ( ( Sa->iIkeV2SaData.SaId() == aSAId )
              &&
           ( ( aRequiredState == KSaStateNotDefined) ||
             ( aRequiredState == Sa->iIkeV2SaData.iSAState ) ) )
        {
           if ( aNewState != KSaStateNotDefined )
               Sa->iIkeV2SaData.iSAState = aNewState;  
           break;
        }   
        Sa = Sa->iNext;
    }   
    return Sa;
}


TBool CIkev2PluginSession::UpdateIkev2SAL(TIkev2SAData* aIkev2SAData, TIkeV2IpsecSAData* aIpsecSAData)
    {
    ASSERT(aIkev2SAData);
    CIkev2SA* Ikev2SA = FindIkev2SA(aIkev2SAData->SaId(), KSaStateNotDefined, KSaStateNotDefined);
    if ( Ikev2SA )
        {
        Ikev2SA->UpdateL(aIkev2SAData, aIpsecSAData);
        return ETrue;
        }
    else 
        {
        return EFalse;
        }
    }

TIkeV2IpsecSAData* CIkev2PluginSession::FindIpsecSAData(TUint32 aSAId, const TDesC8& aSpi, TBool aInbound)
    {
    __ASSERT_ALWAYS(aSpi.Length() == 4, User::Invariant());
    
    _LIT8(KZeroSpi, "");
    TIkeV2IpsecSAData* SaData = NULL;        
    CIkev2SA* Ikev2SA = FindIkev2SA(aSAId, KSaStateNotDefined, KSaStateNotDefined);
    if ( Ikev2SA )
        {
        if ( aInbound ) 
            SaData = Ikev2SA->FindIpsecSaData(aSpi, KZeroSpi, EFalse); 
        else SaData = Ikev2SA->FindIpsecSaData(KZeroSpi, aSpi, EFalse);    
        }   
    return SaData;
    }


//
// Delete an IKEv2 SA using SA Id as search argument
//
void CIkev2PluginSession::DeleteIkev2SA(TUint32 aSAId)
{
    CIkev2SA* Sa     = iFirstIkev2SA;
    CIkev2SA* PrevSa = NULL;
    while ( Sa )
    {
        if ( Sa->iIkeV2SaData.SaId() == aSAId )
        {
            if ( PrevSa )
                {
                PrevSa->iNext = Sa->iNext;
                }
            else
                {
                iFirstIkev2SA = Sa->iNext;
                }
            if (Sa->iIkeV2SaData.iFloatedPort)
                {
                iMessageSendQue->SaBehindNatDeleted(Sa->iIkeV2SaData.SaId());
                }
            delete Sa;
            break;
        }
        PrevSa = Sa;
        Sa     = Sa->iNext;
    }   
}

TUint32 CIkev2PluginSession::GetSAId()    
    { 
    iSAIdSeed++; 
    return iSAIdSeed; 
    }

TBool CIkev2PluginSession::CreateIkev2SAL(TIkev2SAData& aIkev2SAData)
    {
    CIkev2SA* Ikev2SA = CIkev2SA::NewL(*this, aIkev2SAData, iDebug);
    if (aIkev2SAData.iFloatedPort)
        {
        CleanupStack::PushL(Ikev2SA);
        iMessageSendQue->NewSaBehindNatL(aIkev2SAData.SaId());
        CleanupStack::Pop(Ikev2SA);
        }
    Ikev2SA->iNext = iFirstIkev2SA; 
    iFirstIkev2SA = Ikev2SA; 
 
    return ETrue;
    }

void CIkev2PluginSession::IkeSaCompleted(TInt aStatus, TVPNAddress& aInternalAddress)
{       
   if (iClientStatusNegotiate != NULL)
       {
       //This is the first IKE sa of this session
       if (!aInternalAddress.iVPNIfAddr.IsUnspecified())
           {
           *iInternalAddress = aInternalAddress;
           }
       
       // Completion is postponed, if IPsec SAs have not yet been updated.
       if (iActivated ||
           aStatus != KErrNone)
           {           
           DoCompleteNegotiateWithHost(aStatus);
           }
       }
   else if (aStatus == KErrNone)
       {
       //This is not the first IKE SA in this session.
       //If IA has changed we notify the possible address change
       if(!aInternalAddress.iVPNIfAddr.IsUnspecified())
           {
           VirtualIpChanged(aInternalAddress);
           }
       }
   else if(iClientStatusNotifyError != NULL)
       {
       //Ike sa establishmet has failed.
       DoCompleteNotifyError(aStatus);
       }
}


void CIkev2PluginSession::VirtualIpChanged(TVPNAddress& aVirtualIp)
    {    
    if (iClientStatusInternalAddressChange != NULL)
        {
        __ASSERT_DEBUG(iChangedInternalAddress != NULL, User::Invariant());
        *iChangedInternalAddress = aVirtualIp;
        User::RequestComplete(iClientStatusInternalAddressChange, KErrNone);
        iChangedInternalAddress = NULL;
        }
    }

void CIkev2PluginSession::StartResponding()
    { 
    iCurrIkeSaRespCount++; 
    }


void CIkev2PluginSession::StopResponding() 
    { 
    if (iCurrIkeSaRespCount)
        {
        iCurrIkeSaRespCount--; 
        }
    }


void CIkev2PluginSession::DeleteIpsecSAData(TUint32 aSAId, const TDesC8& aSpi, TBool aInbound)
    {
    __ASSERT_DEBUG(aSpi.Length() == 4, User::Invariant());
    _LIT8(KZeroSpi, "");
    CIkev2SA* Ikev2SA = FindIkev2SA(aSAId, KSaStateNotDefined, KSaStateNotDefined);
    if ( Ikev2SA )
        {
        if ( aInbound ) 
             Ikev2SA->DeleteIpsecSaData(aSpi, KZeroSpi);   
        else Ikev2SA->DeleteIpsecSaData(KZeroSpi, aSpi);   
        }   
    }

void CIkev2PluginSession::IkeSaDeleted(TInt aStatus)
    {
    if (iClientStatusDelete != NULL)
        {
        DoCompleteDeleteSession(aStatus);           
        }
    else if (aStatus != KErrNone && iClientStatusNotifyError != NULL)
        {
        DoCompleteNotifyError(aStatus);
        }
    else if (aStatus != KErrNone && iClientStatusNegotiate != NULL)
        {
        TVPNAddress dummyVirtualIp;
        IkeSaCompleted(aStatus,dummyVirtualIp);        
        }
    }


CIpsecSaSpecList* CIkev2PluginSession::GetIPsecSaSpecListL( const TInetAddr& aLocalAddr, const TInetAddr& aLocalMask, 
                                                             const TInetAddr& aRemoteAddr, const TInetAddr& aRemoteMask,
                                                             TInt aProtocol )
    {
    CIpsecSaSpecList* saSpecList = iIpsecPolicyUtil.GetIpseSaSpecListLC( aLocalAddr, aLocalMask, 
                                                                          aRemoteAddr, aRemoteMask,
                                                                          aProtocol, iVpnNetId );
    CleanupStack::Pop(saSpecList);
    
    return saSpecList;
    }


TBool CIkev2PluginSession::InheritIpsecSas(TUint32 aDstSAId, TUint32 aSrcSAId)
    {
    CIkev2SA* DstIkev2SA = FindIkev2SA(aDstSAId, KSaStateNotDefined, KSaStateNotDefined);
    if ( DstIkev2SA )
        {
        CIkev2SA* SrcIkev2SA = FindIkev2SA(aSrcSAId, KSaStateNotDefined, KSaStateNotDefined);
        if ( SrcIkev2SA )
            {   
            DstIkev2SA->SetIpsecSaQue(SrcIkev2SA->GetIpsecSaQue());
            return ETrue;
            }   
        }   
    return EFalse;
    }   


TUint32 CIkev2PluginSession::VpnInterfaceIndex() const
    {
    return iVpnInterfaceIndex;
    }

TBool CIkev2PluginSession::RemoteAddrChanged(TIkev2SAData* aIkev2SAData, TInetAddr& aNewIp)
    {
    __ASSERT_DEBUG(aIkev2SAData, User::Invariant());
    CIkev2SA* Ikev2SA = FindIkev2SA(aIkev2SAData->SaId(), KSaStateNotDefined, KSaStateNotDefined);
    if ( Ikev2SA )
         return Ikev2SA->RemoteAddrChanged(aNewIp);
    else return ETrue;
    }

void CIkev2PluginSession::KeepAliveIkeSAL(TIkev2SAData* aIkev2SAdata)
    {
    ASSERT(aIkev2SAdata);
    CIkev2Negotiation* Negotiation = FindNegotiation(aIkev2SAdata->SaId(), KSaStateNotDefined);
    if ( Negotiation )
        {
        //There is already some negotiation going on this SA, don't send keep-alive
        return; 
        }        
    
    Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, 
                                          iEventLogger, *iMessageSendQue, 
                                          iDebug, *aIkev2SAdata);
    CleanupStack::PushL(Negotiation);
    Negotiation->SendKeepAliveMsgL();
    if ( Negotiation->Stopped() )
        {
        CleanupStack::PopAndDestroy(Negotiation);
        }
    else
        {
        CleanupStack::Pop(Negotiation);
        }
    }

CIkev2Negotiation* CIkev2PluginSession::FindNegotiation(TUint32 aSAId, TInt aRequiredState)
    {
        //
        // Find IKEv2 negotiation object using SAId as search argument 
        //
        CIkev2Negotiation* Neg = iFirstNegotiation;
        while ( Neg )
        {
            if ( ( Neg->iHdr.SaId() == aSAId )
                   &&
                 ( ( aRequiredState == KSaStateNotDefined) ||
                   ( aRequiredState == Neg->iHdr.iSAState ) ) )
            {   
                break;
            }   
            
            Neg = Neg->iNext;
        }   
        return Neg;     
    }

TBool CIkev2PluginSession::DeleteIkeSAL(TIkev2SAData* aIkev2SAdata, TBool aNormal)
    {
    ASSERT(aIkev2SAdata);
    //
    // An IKE SA delete request received
    // Check first does there exists an ongoing negotiation on this IKE
    // SA deleted and delete this block. 
    // Allocate a new negotiation with TIkev2SAData and initiate IKE SA
    // deletion request
    //
        DEBUG_LOG1(_L("Deleting IKE SA SAID =  %d"), aIkev2SAdata->SaId());
                
        CIkev2Negotiation* Negotiation = FindNegotiation(aIkev2SAdata->SaId(), KSaStateNotDefined);
        while ( Negotiation )
        {
            delete Negotiation; // destructor removes object from queue, too
            Negotiation = FindNegotiation(aIkev2SAdata->SaId(), KSaStateNotDefined);            
        }

        TBool Started = EFalse;
        if ( aNormal )
        {   
           Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, 
                                                 iEventLogger, *iMessageSendQue, 
                                                 iDebug, *aIkev2SAdata);
           CleanupStack::PushL(Negotiation);
           Negotiation->StartIkeSADeleteL();
           CleanupStack::Pop(Negotiation);
           if ( Negotiation->Stopped() )
                delete Negotiation;
           else Started = ETrue;   
        }
        else
        {
          DEBUG_LOG(_L("Forced close, no delete payload(s) sent"));
        }
                
        DeleteIkev2SA(aIkev2SAdata->SaId());

        return Started;
    }

void CIkev2PluginSession::RekeyIkeSAL(TIkev2SAData* aIkev2SAdata)
    {
    ASSERT(aIkev2SAdata);
      //
      // Rekey specified IKE SA
      //
        DEBUG_LOG1(_L("Starting to rekey IKE SA SAID =  %d"), aIkev2SAdata->SaId());
        CIkev2Negotiation* Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, 
                                                                 iEventLogger, *iMessageSendQue, 
                                                                 iDebug, *aIkev2SAdata);
        CleanupStack::PushL(Negotiation);
        Negotiation->BuildIkeSaRekeyMsgL(ETrue);        
        if ( Negotiation->Stopped() )
             CleanupStack::PopAndDestroy(Negotiation);
        else CleanupStack::Pop(Negotiation);
    }

void CIkev2PluginSession::IkeMsgReceived( const ThdrISAKMP& aIkeMsg,
                                          const TInetAddr& aSrcAddr,
                                          TInt aLocalPort)
    {       
      TRAPD(err, IkeMessageReceivedL(aIkeMsg, aSrcAddr, aLocalPort));
      if (err != KErrNone)
        {
        //Leave that we have not been able to handle
        //above layers. We close the connection and report an error.
        IkeSaDeleted(err);
        }
    }

// ---------------------------------------------------------------------------
// From class MIkev2ReceiverCallback
// Handles notification about receive error. 
// ---------------------------------------------------------------------------
//
void CIkev2PluginSession::ReceiveError( TInt aError )
    {
    IkeSaDeleted( aError );
    }

void CIkev2PluginSession::IkeMessageReceivedL(const ThdrISAKMP& aIkeMessage, 
                                              const TInetAddr &aRemote, 
                                              TUint16 aLocalPort)
	{
	
		//
		// Do sanity check Parse incoming IKE message 
		//
		TUint32 NegotiationId;
		if ( !CheckIkeMessageHeader(aIkeMessage, NegotiationId) )
			return; // Format error in received IKE message header

		TBool CleanUpUsed = EFalse;
		CIkev2Negotiation* Negotiation;		
		if ( NegotiationId )
		{
		   //
		   // Try to find ongoing IKEv2 negotiation with Id
		   //
		   Negotiation = FindNegotiation(NegotiationId, KSaStateNotDefined);
		   if ( !Negotiation )
		   {
               if (!(aIkeMessage.GetFlags() & IKEV2_RESPONSE_MSG))
               {
                  //
                  // Try to find an IKEv2 SA with negotiation ID
                  //
                  TIkev2SAData* Ikev2SAdata = FindIkev2SAData(NegotiationId,
                                                              KSaStateNotDefined, KSaStateNotDefined);
                  if ( Ikev2SAdata )
                  {
                     Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, 
                                                           iEventLogger, *iMessageSendQue, 
                                                           iDebug, *Ikev2SAdata);
                     CleanupStack::PushL(Negotiation);
                     CleanUpUsed = ETrue;
                  }
                  else
                  {								  
                     DEBUG_LOG(_L("Receive IKE message cannot be associated"));					 
                     return;			 
                  }
               }
               else
               {
                   DEBUG_LOG(_L("Received response message, but we don't have associated negotiation"));
                   DEBUG_LOG(_L("--> Message silently discarded."));
                   return;
               }
		   }	   
		}
		else
		{
		   //
		   // Negotiation ID has zero value. This must be an IKE_SA_INIT
		   // message from peer where Responder SPI has zero value
		   // Get a new negotiation object
		   //
		   
		   TInetAddr localAddr;
		   iDataInterface.GetLocalAddress(localAddr);
		   Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, iEventLogger, 
		                                         *iMessageSendQue, iDebug,
                                                 iIkeData, iVpnIapId, this->GetSAId(), 
                                                 localAddr,
                                                 aRemote);
		   CleanupStack::PushL(Negotiation);
		   if ( !Negotiation->StartRespondingL(aIkeMessage) )
		   {
			  if ( Negotiation->Stopped() )
				   CleanupStack::PopAndDestroy(Negotiation);
			  else CleanupStack::Pop(Negotiation);
			  return;
		   }	   
		   CleanUpUsed = ETrue;
		}
		   
		Negotiation->ProcessIkeMessageL(aIkeMessage, (TInetAddr&)aRemote, aLocalPort);
		if ( CleanUpUsed )
		    CleanupStack::Pop(Negotiation);
		
		if ( Negotiation->Stopped() )
			delete Negotiation;
	}

TBool CIkev2PluginSession::CheckIkeMessageHeader(const ThdrISAKMP& aIkeMessage, TUint32& NegotiationId)
    {
        //
        // Do the following sanity checks to incoming IKE message fixed
        // header
        // -- Check that Exchange type has some value specified in IKEv2
        // -- Check that Next Payload has some value specified in IKEv2
        // -- Check that Inititor SPI has not "zero" value
        //              
       TUint8 ExchangeType = aIkeMessage.GetExchange();
       if ( (ExchangeType < IKE_SA_INIT) || (ExchangeType > INFORMATIONAL) )
       {
          DEBUG_LOG1(_L("Unsupported Exchange Type: %d"),ExchangeType);
          return EFalse;                  
       }
       
       TUint32 SPI_I_Low       =  aIkeMessage.GetSPI_I_Low32();
       TUint32 NegotiationId_I = aIkeMessage.GetNegotiationID_I();
       if ( (SPI_I_Low == 0 ) && ( NegotiationId_I == 0 ) )
       {
           DEBUG_LOG(_L("Initiator SPI has zero value !\n"));  
           return EFalse;                 
       }
        //
        // The negotiation id is a 32-bit (not zero) id value which
        // unambiguously identiefies an IKEv2 negotiation object (CIkev2Negotiation).
        // This negotiation id is packed into the SPI value ( 32 most
        // significant bits of SPI) defined by the local end (=us).
        // Get the negotiation id from local SPI in IKE message
        // according to Initiator Bit in received IKE message header
        // flags.
        // Initiator = 1 ==> Get negotiation id from responder SPI
        // Initiator = 0 ==> Get negotiation id from initiator SPI
        //
       aIkeMessage.GetFlags();
       if ( aIkeMessage.GetFlags() & IKEV2_INITIATOR )
            NegotiationId = aIkeMessage.GetNegotiationID_R();
       else NegotiationId = NegotiationId_I;                   

       return ETrue;
    }


void CIkev2PluginSession::DeleteIpsecSA( const TUint32 aSPI, const TInetAddr& aSrc,
                                  const TInetAddr& aDst, const TUint8 aProtocol )
    {
    iPfKeySocketIf.DeleteSA(aSPI, aSrc, aDst, aProtocol); 
    }


void CIkev2PluginSession::AddSAL( const TIpsecSAData& aSAData )    
    {
    iPfKeySocketIf.AddSAL( aSAData );
    }


void CIkev2PluginSession::UpdateSAL( const TIpsecSAData& aSAData )
    {
    iPfKeySocketIf.UpdateSAL( aSAData );
    }


TIkev2SAData* CIkev2PluginSession::FindIkev2SAData(TUint32 aSAId, TInt aRequiredState, TInt aNewState)
    {
    TIkev2SAData* SaData = NULL;        
    CIkev2SA* Ikev2SA = FindIkev2SA(aSAId, aRequiredState, aNewState);
    if ( Ikev2SA )
        SaData = (TIkev2SAData*)&Ikev2SA->iIkeV2SaData; 
    return SaData;
    }

void CIkev2PluginSession::PfkeyMessageReceived(const TPfkeyMessage& aPfkeyMessage)
    {
    TRAPD(err, PfkeyMessageReceivedL(aPfkeyMessage));
    if (err != KErrNone)
        {
        //Leave that we have not been able to handle
        //above layers. We close the connection and report an error.
        IkeSaDeleted(err);   
        }
    }

void CIkev2PluginSession::PfkeyMessageReceivedL(const TPfkeyMessage& aPfkeyMessage)
    {
        //
        //  Process received PFKEY message according to message type
        //                 
        TIkev2SAData* Ikev2SAdata = NULL;
        CIkev2Negotiation* Negotiation = NULL;     
        TBool CleanUpUsed = EFalse;
        
        __ASSERT_DEBUG(aPfkeyMessage.iBase.iMsg->sadb_msg_type != SADB_GETSPI, User::Invariant());
        switch ( aPfkeyMessage.iBase.iMsg->sadb_msg_type )
        {            
            case SADB_ADD:
                {
                if ( !iActivated )
                    {
                    DEBUG_LOG(_L("Updating of IPsec SAs completed"));
                    iActivated = ETrue;
                    TVPNAddress dummyVirtualIp;
                    IkeSaCompleted(KErrNone,dummyVirtualIp);
                    }
                break;
            case SADB_ACQUIRE:
            if ( iClientStatusDelete != NULL )
                {
                DEBUG_LOG(_L("Acquire ignored because of ongoing deactivation."));
                return;                    
                }
                if (iFirstIkev2SA != NULL)
                    {
                    Ikev2SAdata = &(iFirstIkev2SA->iIkeV2SaData);
                    }
                 if ( Ikev2SAdata )
                 {
                    DEBUG_LOG(_L("Found IKE SA for the acquire"));
                    //
                    // An IKE SA found for Acquire. Get a negotiation
                    // object for IKE Child SA exchange 
                    //
                     Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf,
                                                           iEventLogger, *iMessageSendQue, 
                                                           iDebug,*Ikev2SAdata);
                     CleanupStack::PushL(Negotiation);
                     CleanUpUsed = ETrue;
                 }
                 else
                 {
                     DEBUG_LOG(_L("No IKE SA for the Acquire. Creating new."));
                    //
                    // No IKE SA found for Acquire not ongoing
                    // negotiation found for defined destination
                    // address.
                    // We shall start a new IKE SA negotiation to
                    // defined destination address. Find first the IKE
                    // policy for that destination address.
                    //
                    TInetAddr localAddr;
                    this->iDataInterface.GetLocalAddress(localAddr);                     
                     Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, iEventLogger, 
                                                           *iMessageSendQue, iDebug, iIkeData, 
                                                           iVpnIapId, GetSAId(),
                                                           localAddr,
                                                           *(aPfkeyMessage.iDstAddr.iAddr));
                     CleanupStack::PushL(Negotiation);
                     CleanUpUsed = ETrue;
                 }
                 Negotiation->ProcessAcquireL(aPfkeyMessage);
                 if ( CleanUpUsed )
                    CleanupStack::Pop(Negotiation);
                 if ( Negotiation->Stopped() )
                    delete Negotiation;
                 break;

            case SADB_EXPIRE:                  
                  if (aPfkeyMessage.iSoft.iExt)
                    {
                    //
                    // An IPSEC SA soft lifetime has expired.
                    //
                    // Try to find an existing IKE SA with remote address 
                    //
                    if (iFirstIkev2SA != NULL)
                        {
                        Ikev2SAdata = &(iFirstIkev2SA->iIkeV2SaData);
                        }
                    if ( Ikev2SAdata )
                        {
                        //
                        // An IKE SA found for soft expire. Get a negotiation
                        // object for IKE Child SA exchange 
                        //
                        Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, iEventLogger, 
                                                              *iMessageSendQue, iDebug, *Ikev2SAdata);
                        CleanupStack::PushL(Negotiation);
                        DEBUG_LOG(_L("IKE SA found for soft expire IP."));
                        
                        Negotiation->StartIpsecSaRekeyingL(aPfkeyMessage);
                        CleanupStack::Pop(Negotiation);
                        if ( Negotiation->Stopped() )
                            delete Negotiation;
                        }
                    else
                        {
                        DEBUG_LOG(_L("No IKE SA found for soft expire IP"));
                        }                                      
                    }
                  else
                    {
                    //
                    // An IPSEC SA has been expired.
                    // Try to find an existing IKE SA with remote address 
                    //
                    if (iFirstIkev2SA != NULL)
                        {
                        Ikev2SAdata = &(iFirstIkev2SA->iIkeV2SaData);
                        }
                    if ( Ikev2SAdata )
                        {
                        //
                        // An IKE SA found for Expire. Get a negotiation
                        // object for IKE Informational exchange 
                        //
                        Negotiation = CIkev2Negotiation::NewL(*this, iPfKeySocketIf, iEventLogger, 
                                                              *iMessageSendQue, iDebug, *Ikev2SAdata);
                        CleanupStack::PushL(Negotiation);
                        DEBUG_LOG(_L("IKE SA found for Expire IP"));
                        
                        Negotiation->ProcessExpireL(aPfkeyMessage);
                        CleanupStack::Pop(Negotiation);
                        if ( Negotiation->Stopped() )
                            delete Negotiation;                     
                        }
                    else
                        {
                        DEBUG_LOG(_L("No IKE SA found Expire IP"));                            
                        }                        
                    }                   
                  break;  
                }
            default:
                 break;
        }
    }

TBool CIkev2PluginSession::MatchDestinationAddress( const TInetAddr& aDestAddr ) const
    {
    TBool match( EFalse );
    
    if ( iIkeData )
        {
        match = iIkeData->iAddr.Match( aDestAddr );
        }    
    return match;
    }

void CIkev2PluginSession::DeactivationTimeout()
    {
    IkeSaDeleted(KErrTimedOut);
    }

// ---------------------------------------------------------------------------
// Handles completion of client's negotiate request.
// ---------------------------------------------------------------------------
//
void CIkev2PluginSession::DoCompleteNegotiateWithHost( TInt aStatus )
    {    
    if ( aStatus != KErrNone )
        {
        DoCancelActiveOperations();       
        }
    else
        {
        iActivated = ETrue;       
        }
    
    User::RequestComplete( iClientStatusNegotiate, aStatus );
    }

// ---------------------------------------------------------------------------
// Handles completion of client's delete session request.
// ---------------------------------------------------------------------------
//
void CIkev2PluginSession::DoCompleteDeleteSession( TInt aStatus )
    {       
    delete iIkeData;
    iIkeData = NULL;
    delete iDeactivationTimer;
    iDeactivationTimer = NULL;
    
    if ( aStatus != KErrCancel )
        {
        DoCancelActiveOperations();
        }
    User::RequestComplete( iClientStatusDelete, aStatus );
    }

// ---------------------------------------------------------------------------
// Handles completion of client's notify error request.
// ---------------------------------------------------------------------------
//
void CIkev2PluginSession::DoCompleteNotifyError( TInt aStatus )
    {
    if ( aStatus != KErrCancel )
        {
        DoCancelActiveOperations();
        }
    User::RequestComplete( iClientStatusNotifyError, aStatus );    
    }

// ---------------------------------------------------------------------------
// Cancels active operations.
// ---------------------------------------------------------------------------
//
void CIkev2PluginSession::DoCancelActiveOperations()
    {
    // Cancel active negotiation operations.
    CIkev2Negotiation* negotiation = iFirstNegotiation;
    while ( negotiation != NULL )
        {
        negotiation->CancelOperation();
        negotiation = negotiation->iNext;
        }

    // Cancel active IKE SA operations.
    CIkev2SA* ikev2Sa = iFirstIkev2SA;
    while( ikev2Sa != NULL )
        {
        ikev2Sa->Cancel();
        ikev2Sa = ikev2Sa->iNext;
        }                        
    
    DoCancelDataTransfer();
    }

// ---------------------------------------------------------------------------
// Cancels data transfer.
// ---------------------------------------------------------------------------
//
void CIkev2PluginSession::DoCancelDataTransfer()
    {
    if ( iReceiver != NULL )
        {
        iReceiver->StopReceive();
        }
    if ( iMessageSendQue != NULL )
        {
        iMessageSendQue->Cancel();
        iMessageSendQue->CancelAll();
        }        
    }
