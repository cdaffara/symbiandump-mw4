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
* Description:  IKEv2 Acquire definition
*                Class CIkev2Acquire is a IKEv2 specific data structure
*                containing information needed, when establishing a new 
*                IPsec SA
*/

#include <ipsecpolapi.h>

#include "ikev2acquire.h"
#include "pfkeymsg.h"
#include "ipsecproposal.h"
#include "ipsecselectors.h"
#include "ikev2payloads.h"
#include "ikev2ipsecsarekeydata.h"

_LIT8(KZeroDesc, "");
//
//
//  CIkev2Acquire
//
//  This class is used to handle PFKEY Acquire primitives received from
//  Ipsec plug-in. 
//  
//
CIkev2Acquire::CIkev2Acquire(TInt aId) 
:iId(aId), iSPIIn(0), iSPIOut(0), iSPIToBeRekeyed(0), iDHGroup(0), 
 iTransport(EFalse), iResponse(EFalse), iSrcSpecific(ETrue), iForVirtualIp(EFalse), 
 iReplayWindow(0), iProtocol(SADB_SATYPE_ESP), iPfKeyPid(0), iPfKeySeq(0), 
 iHard(0,0,0,0), iSoft(0,0,0,0),
 iSA(0), iTS_i(0), iTS_r(0), iLocalId(0), iRemoteId(0), iNext(0) 
    {    
    }      


CIkev2Acquire::~CIkev2Acquire()
{
    delete iSA;
    iSA = NULL;
    delete iTS_i;
    iTS_i = NULL;
    delete iTS_r;
    iTS_r = NULL;
    delete iLocalId;
    iLocalId = NULL;
    delete iRemoteId;   
    iRemoteId = NULL;
}

void CIkev2Acquire::ConstructL(const TPfkeyMessage& aReq, const TInetAddr& aLocalAddr, 
                          TUint16 aDHGroup, TBool aImplicitSa, 
                          const TIpsecSaSpec* aSaSpec, const CIpsecSARekeyData* aRekeyData)
{
    SetIpsecProtocol(aReq.iBase.iMsg->sadb_msg_satype); 
    SetPid(aReq.iBase.iMsg->sadb_msg_pid);
    SetSeq(aReq.iBase.iMsg->sadb_msg_seq);
    iSrcSpecific = !(aReq.iSrcAddr.iAddr->IsUnspecified());

    if (!aRekeyData)
        {
        // SADB_ACQUIRE was received
        if ( aReq.iProposal.iComb->sadb_comb_flags & SADB_SAFLAGS_PFS )
            {
            iDHGroup = (TUint32)aDHGroup;
            }
    
        if ( aReq.iProposal.iComb->sadb_comb_flags & SADB_SAFLAGS_TUNNEL )
            {
            iTransport = EFalse;        
            }
        else 
            {
            iTransport = ETrue;
            }

        iHard.iAllocations = aReq.iProposal.iComb->sadb_comb_hard_allocations;
        iHard.iBytes = aReq.iProposal.iComb->sadb_comb_hard_bytes;
        iHard.iAddtime = aReq.iProposal.iComb->sadb_comb_hard_addtime;
        iHard.iUsetime = aReq.iProposal.iComb->sadb_comb_hard_usetime;
    
        iSoft.iAllocations = aReq.iProposal.iComb->sadb_comb_soft_allocations;
        iSoft.iBytes = aReq.iProposal.iComb->sadb_comb_soft_bytes;
        iSoft.iAddtime = aReq.iProposal.iComb->sadb_comb_soft_addtime;
        iSoft.iUsetime = aReq.iProposal.iComb->sadb_comb_soft_usetime;

        SetReplayWindow(aReq.iProposal.iExt->sadb_prop_replay);

        //
        //  Build SA payload (including Proposal and Transform payload)
        //  using parameters in TPfkeyMessage
        //
        if ( aImplicitSa )
            {
            aDHGroup = 0; // No D-H group used with ipmplicit SA although PFS defined   
            }
        iSA = IpsecProposal::BuildIpsecSaRequestL(aReq, aDHGroup);
    
        //
        //  Build Traffic selectors using parameters in TPfkeyMessage.
        //  For some reason the selectors are build using identity 
        //  fields.
        //  
        IpsecSelectors::BuildTrafficSelectorsL(this, aLocalAddr,
                                               aReq.iSrcIdent, aReq.iDstIdent,
                                               aReq.iDstAddr.iExt->sadb_address_proto);
        //
        //  Store identity information from TPfkeyMessage to Acquire
        //
        if ( aReq.iSrcIdent.iExt )
            {            
            iLocalId = HBufC8::NewL(aReq.iSrcIdent.iData.Length());
            iLocalId->Des().Copy(aReq.iSrcIdent.iData);
            }
        else
            {
            iLocalId = KZeroDesc().AllocL();
            }

        if ( aReq.iDstIdent.iExt )
            {            
            iRemoteId = HBufC8::NewL(aReq.iDstIdent.iData.Length());
            iRemoteId->Des().Copy(aReq.iDstIdent.iData);
            }
        else
            {
            iRemoteId = KZeroDesc().AllocL();
            }
        }
    else
        {
        // SADB_EXPIRE was received due to soft lifetime expiration
        TUint16 flags = (TUint16)(aReq.iSa.iExt->sadb_sa_flags | (aSaSpec->iTransportMode ? 0 : SADB_SAFLAGS_TUNNEL));
        if ( flags & SADB_SAFLAGS_PFS )
            {
            iDHGroup = (TUint32)aDHGroup;
            }
    
        if ( flags & SADB_SAFLAGS_TUNNEL )
            {
            iTransport = EFalse;        
            }
        else 
            {
            iTransport = ETrue;
            }
                    
        if ( aImplicitSa )
            {
            aDHGroup = 0; // No D-H group used with ipmplicit SA although PFS defined   
            } 
        
        iSA = IpsecProposal::BuildIpsecSaRequestL(iProtocol, 
                                                  aSaSpec->iEalg, aSaSpec->iEalgLen, aSaSpec->iAalg, 
                                                  flags, aDHGroup);
                                                    
        iReplayWindow = aRekeyData->ReplayWindow();
        iHard = aRekeyData->HardLifetime();
        iSoft = aRekeyData->SoftLifetime();
        
        iTS_i = aRekeyData->TsIL();
        iTS_r = aRekeyData->TsRL();

        
        iLocalId = aRekeyData->LocalId().AllocL();
        iRemoteId = aRekeyData->RemoteId().AllocL();

        iResponse = EFalse;
        }
}


CIkev2Acquire* CIkev2Acquire::NewL(const TPfkeyMessage& aPfkeyMessage, TUint32 aId, 
                         const TInetAddr& aLocalAddr, TUint16 aDHGroup, TBool aImplicitSa,
                         const TIpsecSaSpec* aSaSpec, const CIpsecSARekeyData* aRekeyData)
{
    CIkev2Acquire* Acquire = new (ELeave)CIkev2Acquire(aId);
    CleanupStack::PushL(Acquire);
    Acquire->ConstructL(aPfkeyMessage, aLocalAddr, aDHGroup, aImplicitSa, aSaSpec, aRekeyData);
    CleanupStack::Pop(Acquire);     
    return Acquire;
}


void CIkev2Acquire::AddIpsecSpiToSa(const TDesC8& aSpi)
    {
    __ASSERT_DEBUG(aSpi.Length() == 4, User::Invariant());

    TUint8* saBuffer = const_cast<TUint8*>(iSA->Ptr());
    TProposalIkev2* Prop = TProposalIkev2::Cast(saBuffer);

    while ( Prop )
        {
        TUint32 spiValue = 0;
        TPtr8 spiValueDesc(reinterpret_cast<TUint8*>(&spiValue), sizeof(spiValue));
        spiValueDesc = aSpi;
        Prop->SetIpsecSPI(spiValue);
        if ( !Prop->Last() )
            {
            Prop = TProposalIkev2::Cast(TPayloadIkev2::Cast(Prop)->Next());
            }  
        else
            {
            Prop = NULL;
            }
        }       
    }


CIkev2Acquire* CIkev2Acquire::NewL(TUint32 aId, HBufC8* aSa, 
                         CArrayFix<TIkeV2TrafficSelector>* aTS_i, 
                         CArrayFix<TIkeV2TrafficSelector>* aTS_r )
{
    CIkev2Acquire* Acquire = new (ELeave)CIkev2Acquire(aId);
    Acquire->iSA   = aSa;
    Acquire->iTS_i = aTS_i;
    Acquire->iTS_r = aTS_r;     
    return Acquire;
}

void CIkev2Acquire::Link(CIkev2Acquire* aAcquire, CIkev2Acquire** aAnchor)
{
    ASSERT(aAcquire && aAnchor);
    aAcquire->iNext = NULL;
    CIkev2Acquire* Last  = *aAnchor;
    if ( Last )
    {   
        while ( Last->iNext )
        {
            Last = Last->iNext;
        }
        Last->iNext = aAcquire;
    }
    else *aAnchor = aAcquire;   
}

CIkev2Acquire* CIkev2Acquire::Find(TUint32 aId, CIkev2Acquire** aAnchor, TBool aRemove)
{
    ASSERT(aAnchor);
    CIkev2Acquire* Prev = NULL;
    CIkev2Acquire* Elem = *aAnchor;
    while ( Elem )
    {
        if ( Elem->iId == aId )
        {
           if ( aRemove )
           {
              if ( Prev )
                   Prev->iNext = Elem->iNext;
              else *aAnchor = Elem->iNext;
           }       
           break;
        }
        Prev = Elem;
        Elem = Elem->iNext;
    }
    return Elem;
}

CIkev2Acquire* CIkev2Acquire::GetNext(CIkev2Acquire** aAnchor, TBool aResponse)
{
    ASSERT(aAnchor);
    CIkev2Acquire* Elem = *aAnchor;
    while ( Elem )
    {
        if ( Elem->SPI_In().Length() > 0 && (Elem->Response() == aResponse) )
        {
            RemoveFromQue(Elem->Id(), aAnchor);
            break;
        }
        Elem = Elem->iNext;
    }
    return Elem;
}

TBool CIkev2Acquire::Responding(CIkev2Acquire** aAnchor)
{
    ASSERT(aAnchor);
    CIkev2Acquire* Elem = *aAnchor;
    while ( Elem )
    {
        if ( Elem->Response() )
        {
            return ETrue;
        }
        Elem = Elem->iNext;
    }
    return EFalse;
}


void CIkev2Acquire::PurgeQue(CIkev2Acquire** aAnchor)
{
    ASSERT(aAnchor);
    CIkev2Acquire* Elem = *aAnchor;
    while ( Elem )
    {
        RemoveFromQue(Elem->Id(), aAnchor);
        delete Elem;
        Elem = *aAnchor;
    }   
}

void CIkev2Acquire::SetFirst(CIkev2Acquire* aAcquire, CIkev2Acquire** aAnchor)
    { 
    ASSERT(aAcquire && aAnchor); 
    aAcquire->iNext = *aAnchor; 
    *aAnchor = aAcquire; 
    }

CIkev2Acquire* CIkev2Acquire::PeekFirst(CIkev2Acquire** aAnchor) 
    { 
    return *aAnchor; 
    }

CIkev2Acquire* CIkev2Acquire::RemoveFromQue(TUint32 aId, CIkev2Acquire** aAnchor)
    { 
    return CIkev2Acquire::Find(aId, aAnchor, ETrue);
    }

TUint32 CIkev2Acquire::Id() 
    { 
    return iId;
    }

TPtrC8 CIkev2Acquire::SPI_In() 
    { 
    return iSPIIn;
    }

TPtrC8 CIkev2Acquire::SPI_Out() 
    { 
    return iSPIOut;
    }          

TPtrC8 CIkev2Acquire::SPI_ToBeRekeyed() 
    { 
    return iSPIToBeRekeyed;
    }

void CIkev2Acquire::SetSPI_In(const TDesC8& aSPI) 
    { 
    iSPIIn = aSPI;
    }

void CIkev2Acquire::SetSPI_Out(const TDesC8& aSPI) 
    { 
    iSPIOut = aSPI;
    }

void CIkev2Acquire::SetSPI_ToBeRekeyed(const TDesC8& aSPI) 
    { 
    iSPIToBeRekeyed = aSPI;
    }

TUint16 CIkev2Acquire::DHGroup() 
    { 
    return (TUint16)iDHGroup;
    }

void CIkev2Acquire::DHGroup(TUint16 aDHGroup) 
    { 
    iDHGroup = aDHGroup;
    }

TBool CIkev2Acquire::Transport() 
    { 
    return iTransport;
    }

void CIkev2Acquire::SetTransport() 
    { 
    iTransport = ETrue;
    }       

TBool CIkev2Acquire::Response() 
    { 
    return iResponse;
    }

void CIkev2Acquire::SetResponse() 
    { 
    iResponse = ETrue;
    }

void CIkev2Acquire::SetHardLifetime(const TIpsecSALifetime& aHard ) 
    { 
    iHard = aHard;
    }

TIpsecSALifetime* CIkev2Acquire::HardLifetime() 
    { 
    return &iHard;
    }

void CIkev2Acquire::SetSoftLifetime(const TIpsecSALifetime& aSoft ) 
    { 
    iSoft = aSoft;
    }

TIpsecSALifetime* CIkev2Acquire::SoftLifetime() 
    { 
    return &iSoft;
    }

TUint8 CIkev2Acquire::ReplayWindow() 
    { 
    return (TUint8)iReplayWindow;
    }

void CIkev2Acquire::SetReplayWindow(TUint8 aReplayWindow) 
    { 
    iReplayWindow = (TInt)aReplayWindow;
    }

TUint32 CIkev2Acquire::Pid() 
    { 
    return iPfKeyPid;
    }

void CIkev2Acquire::SetPid(TUint32 aPfKeyPid) 
    { 
    iPfKeyPid = aPfKeyPid;
    }

TUint32 CIkev2Acquire::Seq() 
    { 
    return iPfKeySeq;
    }

void CIkev2Acquire::SetSeq(TUint32 aPfKeySeq) 
    { 
    iPfKeySeq = aPfKeySeq;
    }

void CIkev2Acquire::SetVirtualIp() 
    { 
    iForVirtualIp = ETrue; 
    }

TBool CIkev2Acquire::ForVirtualIp() 
    { 
    return iForVirtualIp; 
    }

TBool CIkev2Acquire::SrcSpecific() 
    { 
    return iSrcSpecific;
    }

void CIkev2Acquire::SetSrcSpecific(TBool aSrcSpecific) 
    { 
    iSrcSpecific = aSrcSpecific;
    }

TUint8 CIkev2Acquire::IpsecProtocol() 
    { 
    return (TUint8)iProtocol;
    }

void CIkev2Acquire::SetIpsecProtocol(TUint8 aProtocol) 
    { 
    iProtocol = (TInt)aProtocol;
    }                      

HBufC8* CIkev2Acquire::LocalId() 
    { 
    return iLocalId;
    }

HBufC8* CIkev2Acquire::RemoteId() 
    { 
    return iRemoteId;
    }

HBufC8* CIkev2Acquire::SA()const 
    { 
    return iSA;
    }

const CArrayFix<TIkeV2TrafficSelector>& CIkev2Acquire::TS_i() 
    { 
    return *iTS_i;
    }

const CArrayFix<TIkeV2TrafficSelector>& CIkev2Acquire::TS_r() 
    { 
    return *iTS_r;
    }

void CIkev2Acquire::ReplaceSA(HBufC8* aSA) 
    { 
    delete iSA; 
    iSA = aSA; 
    }

void CIkev2Acquire::ReplaceTS_i(CArrayFix<TIkeV2TrafficSelector>* aTS) 
    { 
    delete iTS_i; 
    iTS_i = aTS; 
    }

void CIkev2Acquire::ReplaceTS_r(CArrayFix<TIkeV2TrafficSelector>* aTS) 
    { 
    delete iTS_r; 
    iTS_r = aTS; 
    }

void CIkev2Acquire::ReplaceLocalId(HBufC8* aId) 
    { 
    delete iLocalId; 
    iLocalId = aId; 
    }

void CIkev2Acquire::ReplaceRemoteId(HBufC8* aId) 
    { 
    delete iRemoteId; 
    iRemoteId = aId; 
    }

