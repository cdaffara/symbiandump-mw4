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
* Description:   Ipsec Traffic Selector handling
*
*/

#include <in_sock.h>
#include <ipsecpolapi.h>
#include "ikedebug.h"
#include "ipsecselectors.h"
#include "ikev2payloads.h"
#include "ikev2pluginsession.h"
#include "ikev2proposal.h"
#include "ikemsgrec.h"
#include "ikev2const.h"
#include "ipsecproposal.h"
#include "pfkeymsg.h"
#include "ipsecsalist.h"
#include "ikev2trafficselector.h"
#include "ikev2acquire.h"
#include <networking/pfkeyv2.h>

CIkev2Acquire* IpsecSelectors::GetIpsecPolicyL(CIkev2PluginSession& aPluginSession, CIkev2Payloads* aIkeMsg, TInt aDhGroup)
{
    ASSERT(aIkeMsg);
	//
	// Examine is there available policy for Traffic selectors present
	// in current CREATE_CHILD_SA request. Use Initiator traffic
	// selector for policy check.
    
    //If there is no traffic selector, we stop the processing
	TTSPayloadIkev2* TsIPl = aIkeMsg->iTsI;
	if ( !TsIPl || (TsIPl->GetNumberOfTs() < 1) )
	{	
	   return NULL;
	}   
	
    TTSPayloadIkev2* TsRPl = aIkeMsg->iTsR;
    if ( !TsRPl || (TsRPl->GetNumberOfTs() < 1) )
    {   
       return NULL;
    }   
	
    //Parse both selectors
    CArrayFix<TIkeV2TrafficSelector>* proposedTsI = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(2);
    CleanupStack::PushL(proposedTsI);
    
    CArrayFix<TIkeV2TrafficSelector>* proposedTsR = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(2);
    CleanupStack::PushL(proposedTsR);

    TInt i = 0;
    const TTrafficSelector* sel = TTrafficSelector::Cast(TsIPl->TrafficSelectors());
    for (i = 0; i < TsIPl->GetNumberOfTs(); ++i)
        {
        TPtrC8 selPtr(reinterpret_cast<const TUint8*>(sel), sel->Length());
        TIkeV2TrafficSelector* selector = TIkeV2TrafficSelector::NewL(selPtr);
        CleanupStack::PushL(selector);
        proposedTsI->AppendL(*selector);
        CleanupStack::PopAndDestroy(selector);
        
        sel = reinterpret_cast<const TTrafficSelector*>(reinterpret_cast<const TUint8*>(sel) + sel->Length()); 
        }
		
    
    i = 0;
    sel = TTrafficSelector::Cast(TsRPl->TrafficSelectors());
    for (i = 0; i < TsRPl->GetNumberOfTs(); ++i)
        {
        TPtrC8 selPtr(reinterpret_cast<const TUint8*>(sel), sel->Length());
        TIkeV2TrafficSelector* selector = TIkeV2TrafficSelector::NewL(selPtr);
        CleanupStack::PushL(selector);
        proposedTsR->AppendL(*selector);
        CleanupStack::PopAndDestroy(selector);
        
        sel = reinterpret_cast<const TTrafficSelector*>(reinterpret_cast<const TUint8*>(sel) + sel->Length()); 
        }

    __ASSERT_DEBUG(proposedTsI->Count() > 0, User::Invariant());
    //The policy is retrieved by using the firts initiator selector.
    TInetAddr mask = (*proposedTsI)[0].Mask();
    
    //Takes only the network part of the address
    TInetAddr addr;    
    addr.SetAddress((*proposedTsI)[0].StartingAddress().Address() & mask.Address());
    addr.SetPort(0);
    
	TInetAddr DummyIp;
	DummyIp.SetAddress(KInetAddrNone);   // 0.0.0.0
	DummyIp.SetPort(0);
	
	CIpsecSaSpecList* SaList =  NULL;
	TRAPD(err, SaList = aPluginSession.GetIPsecSaSpecListL(DummyIp, DummyIp, // No local address/port info
                                                           addr, mask,       // for any peer address and port
                                                           (*proposedTsI)[0].ProtocolId()));  // Protocol
	
	if (err != KErrNone)
	    {
	    CleanupStack::PopAndDestroy(proposedTsR);
	    CleanupStack::PopAndDestroy(proposedTsI);	    	    
	    
	    return NULL;
	    }
	CleanupStack::PushL(SaList);
	const TIpsecSaSpec& Spec = SaList->At(0);
	
	__ASSERT_DEBUG(SaList->Count() > 0, User::Invariant());	
	HBufC8* Sa  = IpsecProposal::BuildIpsecSaFromPolicyL(*SaList, aDhGroup);
		
	CleanupStack::PushL(Sa);	
	
	CIkev2Acquire* Acquire = CIkev2Acquire::NewL(aPluginSession.GetSAId(), Sa, proposedTsI, proposedTsR);
	
	CleanupStack::Pop(Sa);    
    CleanupStack::PushL(Acquire);
	
	TIpsecSALifetime hard(Spec.iHard.iAllocations, Spec.iHard.iBytes, Spec.iHard.iAddTime, Spec.iHard.iUseTime);
    TIpsecSALifetime soft(Spec.iSoft.iAllocations, Spec.iSoft.iBytes, Spec.iSoft.iAddTime, Spec.iSoft.iUseTime);
	Acquire->SetHardLifetime(hard);
	Acquire->SetSoftLifetime(soft);
	
	//
	// Set SrcSpecific information to correspond MOBIKE configuration.
	// Actually SrcSpecific should be available in TIpsecSaSpec.
	//
	HBufC8* remoteId = NULL;
    if ( Spec.iRemoteIdentity.Length() )
    {
       //
       // Copy remote identity from policy and queue it to CIkev2Acquire
       // object
       //
       remoteId = Spec.iRemoteIdentity.AllocL();
       
    }
    else
    {
       remoteId = HBufC8::NewL(1);
    }
    Acquire->ReplaceRemoteId(remoteId);
    
    HBufC8* localId = NULL;
    if ( Spec.iLocalIdentity.Length() )
    {
       //
       // Copy remote identity from policy and queue it to CIkev2Acquire
       // object
       //
       localId = Spec.iLocalIdentity.AllocL();
       
    }
    else
    {
        localId = HBufC8::NewL(1);
    }
    Acquire->ReplaceLocalId(localId);
    
    CleanupStack::Pop(Acquire);
    CleanupStack::PopAndDestroy(SaList);
    CleanupStack::Pop(proposedTsR);
    CleanupStack::Pop(proposedTsI);
    
 	return Acquire;
}

CIkev2Acquire* IpsecSelectors::BuildVirtualAcquireL(CIkev2PluginSession& aPluginSession)
{
	//
	// Build CIkev2Acquire object and related Ipsec SA- ja Traffic selector
	// Payloads. These payload is used in conjunction with CP payload
	// in IKE_AUTH exchange.  
	// To get an Virtual Ip with CP payload we must also create (for
	// some unintelligible reason) create an pair of Ipsec SA:s.
	// The following actions are taken:
	// -- Try first if the "all host" selector is available in
	// current Ipsec policy and if it build Virtual acquire for full
	// address range (0.0.0.0 - 255.255.255.255)
	// -- If "all host" selector is not available, we going to use
	// "UMA"  Ipsec ESP profiles as Ipsec SA proposal.
	// The traffic selectors are set so that we asking Ipsec SA:s
	// between requested Virtual Ip and Remote SGW Ip
	// (single address all ports and protocols)
	//
	HBufC8* Sa = 0;
	
	TIpsecSALifetime hard(0,0,0,0);
	TIpsecSALifetime soft(0,0,0,0);
	
	TInetAddr StartIp(KInetAddrNone, 0);    // 0.0.0.0
	TInetAddr EndIp(KInetAddrAll, 0xFFFF);	      // 255.255.255.255	
  
    CIpsecSaSpecList* SaList = aPluginSession.GetIPsecSaSpecListL(StartIp, StartIp,     // for any local address and port
                                                                  StartIp, StartIp,     // for any peer address and port
                                                        	      0);  // Any protocol
    CleanupStack::PushL(SaList);
    __ASSERT_DEBUG(SaList->Count() > 0, User::Invariant());
    
   //
   // Build Ipsec proposal for implicit SA negotiatited within IKE
   // SA AUTH exchange
   //
	const TIpsecSaSpec& Spec = SaList->At(0);		   
    Sa = IpsecProposal::BuildIpsecSaFromPolicyL(*SaList, 0); // 0 = DH Group
    CleanupStack::PushL(Sa);
    
    CArrayFix<TIkeV2TrafficSelector>* TsI = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(1);
    CleanupStack::PushL(TsI);
    TIkeV2TrafficSelector selector(StartIp, EndIp, 0);
    TsI->AppendL(selector);
    
    CArrayFix<TIkeV2TrafficSelector>* TsR = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(1);
    CleanupStack::PushL(TsR);
    selector = TIkeV2TrafficSelector(StartIp, EndIp, 0);
    TsR->AppendL(selector);
                
    hard.iAllocations = Spec.iHard.iAllocations;
    hard.iBytes = Spec.iHard.iBytes;
    hard.iAddtime = Spec.iHard.iAddTime;
    hard.iUsetime = Spec.iHard.iUseTime;
    soft.iAllocations = Spec.iSoft.iAllocations;
    soft.iBytes = Spec.iSoft.iBytes;
    soft.iAddtime = Spec.iSoft.iAddTime;
    soft.iUsetime = Spec.iSoft.iUseTime;

	CIkev2Acquire* Acquire = CIkev2Acquire::NewL(aPluginSession.GetSAId(), Sa, TsI, TsR);
	CleanupStack::Pop(TsR); 
	CleanupStack::Pop(TsI); 
	CleanupStack::Pop(Sa); 
	
	CleanupStack::PushL(Acquire);
	HBufC8* identity = Spec.iRemoteIdentity.AllocL();
	Acquire->ReplaceRemoteId(identity);    
	identity = NULL;
	
	identity = Spec.iLocalIdentity.AllocL();
	Acquire->ReplaceLocalId(identity);	
	identity = NULL;
		    
	Acquire->SetVirtualIp();
	Acquire->SetHardLifetime(hard);
	Acquire->SetSoftLifetime(soft);
		
	CleanupStack::Pop(Acquire);
	CleanupStack::PopAndDestroy(); // SaList
	
	return Acquire;			
}

TBool IpsecSelectors::VerifyTrafficSelectorsL(CIkev2Acquire* aAcquire, TTSPayloadIkev2* aTsI, TTSPayloadIkev2* aTsR )
{
    ASSERT(aAcquire);
        
    if (aTsI == NULL || aTsR == NULL)
        {
        return EFalse;
        }
    //
	// Compare Traffic selectors CIkev2Acquire object to Traffic selectors
	// received in CREATE_CHILD_SA response.
	//
    const CArrayFix<TIkeV2TrafficSelector>& TsI_Ref = aAcquire->TS_i();
    const CArrayFix<TIkeV2TrafficSelector>& TsR_Ref = aAcquire->TS_r();
    __ASSERT_DEBUG(TsI_Ref.Count() > 0 && TsR_Ref.Count() > 0, User::Invariant());
    
    //
	// Check has the peer been narrowed requested Traffic selectors
	//
    CArrayFix<TIkeV2TrafficSelector>* TsI = new (ELeave)CArrayFixFlat<TIkeV2TrafficSelector>(2);
    CleanupStack::PushL(TsI);
    CArrayFix<TIkeV2TrafficSelector>* TsR = new (ELeave)CArrayFixFlat<TIkeV2TrafficSelector>(2);
    CleanupStack::PushL(TsR);
    	
    TInt i = 0;
    const TTrafficSelector* sel = TTrafficSelector::Cast(aTsI->TrafficSelectors());
    const TUint8* payloadEnd = reinterpret_cast<TUint8*>(aTsI) + TPayloadIkev2::Cast(aTsI)->GetLength();
    for (i = 0; i < aTsI->GetNumberOfTs(); ++i)
        {
        if (reinterpret_cast<const TUint8*>(sel) > payloadEnd || 
            reinterpret_cast<const TUint8*>(sel) + sel->Length() > payloadEnd)
            {
            CleanupStack::PopAndDestroy(TsR);
            CleanupStack::PopAndDestroy(TsI);
            return EFalse;
            }
        
        TPtrC8 selPtr(reinterpret_cast<const TUint8*>(sel), sel->Length());
        TIkeV2TrafficSelector* selector = TIkeV2TrafficSelector::NewL(selPtr);
        CleanupStack::PushL(selector);
        TsI->AppendL(*selector);
        CleanupStack::PopAndDestroy(selector);
        
        sel = reinterpret_cast<const TTrafficSelector*>(reinterpret_cast<const TUint8*>(sel) + sel->Length());
        }

	
    sel = TTrafficSelector::Cast(aTsR->TrafficSelectors());
    payloadEnd = reinterpret_cast<TUint8*>(aTsR) + TPayloadIkev2::Cast(aTsR)->GetLength();
    for (i = 0; i < aTsR->GetNumberOfTs(); ++i)
        {
        if (reinterpret_cast<const TUint8*>(sel) > payloadEnd || 
            reinterpret_cast<const TUint8*>(sel) + sel->Length() > payloadEnd)
            {
            CleanupStack::PopAndDestroy(TsR);
            CleanupStack::PopAndDestroy(TsI);
            return EFalse;
            }
        TPtrC8 selPtr(reinterpret_cast<const TUint8*>(sel), sel->Length());
        TIkeV2TrafficSelector* selector = TIkeV2TrafficSelector::NewL(selPtr);
        CleanupStack::PushL(selector);
        TsR->AppendL(*selector);
        CleanupStack::PopAndDestroy(selector);
        
        sel = reinterpret_cast<const TTrafficSelector*>(reinterpret_cast<const TUint8*>(sel) + sel->Length());
        }    
        
    if ( !ValidataTs(TsI_Ref, *TsI) || 
         !ValidataTs(TsR_Ref, *TsR) )
        {
        delete TsI;
        delete TsR;        
		return EFalse;
        }
    aAcquire->ReplaceTS_i(TsI);     
    aAcquire->ReplaceTS_r(TsR);     

    CleanupStack::Pop(TsR);
    CleanupStack::Pop(TsI);
    
	return ETrue;
}



void IpsecSelectors::BuildTrafficSelectorsL(CIkev2Acquire* aAcquire, const TInetAddr& aLocalAddr,
											const TPfkeyIdentity& aSrcIdent, const TPfkeyIdentity& aDstIdent,
										    TUint8 aProtocol)
{
	//
	// Build Traffic Selectors payload from PFKEY Acquire primitive
	// Identity data
	//
    CArrayFix<TIkeV2TrafficSelector>* TsIBfr = NULL;
    CArrayFix<TIkeV2TrafficSelector>* TsRBfr = NULL;
	if ( aDstIdent.iExt && (aDstIdent.iExt->sadb_ident_type == SADB_IDENTTYPE_PREFIX))
	{	
		TsRBfr = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(1);
		CleanupStack::PushL(TsRBfr);
		if ( ( !aLocalAddr.IsUnspecified()) ||
			 ( aSrcIdent.iExt && (aSrcIdent.iExt->sadb_ident_type == SADB_IDENTTYPE_PREFIX)))
		{	
			TsIBfr = new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(1);
			CleanupStack::PushL(TsIBfr);
		}	
	}

	if ( TsIBfr )
	{
	    ASSERT(aAcquire);
    	//
	    // If local address pointer defined, local address is used as
	    // initiator Traffic selector (single address "range")
		// Else Use Source Identity data to build inititor Traffic selector
	    //
		if ( !aLocalAddr.IsUnspecified() )
		{
            TInetAddr startAddress = aLocalAddr;
            startAddress.SetPort(0);
            
            TInetAddr endAddress = aLocalAddr;
            endAddress.SetPort(0xffff);
                        
            TIkeV2TrafficSelector selector(startAddress, endAddress, aProtocol);
            TsIBfr->AppendL(selector);
		}	 
		else
		{
            TIkeV2TrafficSelector selector = IpsecSelectors::IdentityToSelectorL(aSrcIdent.iData, aProtocol);
            TsIBfr->AppendL(selector);
            
		}
		CleanupStack::Pop(TsIBfr); 							
		aAcquire->ReplaceTS_i(TsIBfr);
	}	

	if ( TsRBfr )
	{	
	    ASSERT(aAcquire);
	   //
	   // Build responder Traffic selector from destination Identity data
	   //	   
	   TIkeV2TrafficSelector selector = IpsecSelectors::IdentityToSelectorL(aDstIdent.iData, aProtocol);
	   TsRBfr->AppendL(selector);
	   CleanupStack::Pop(TsRBfr);  // TsRBfr									
	   aAcquire->ReplaceTS_r(TsRBfr);
	}   
}


TIkeV2TrafficSelector IpsecSelectors::IdentityToSelectorL(const TDesC8& aIdentity, TUint8 aProtocol)
{
	//
	// Convert text format Identity data to Range start and end address
	// needed into Traffic selector 
	//
	TInetAddr StartAddr;
	TInetAddr EndAddr;
	TInt Lth = aIdentity.Length();

    if (aIdentity.Length() == 0)
        {
        User::Leave(KErrArgument);
        }
	
    TInt offset = aIdentity.Find(_L8("/"));
    switch (offset)
    {
        case KErrNotFound:  //Simple address
        { 			
            HBufC *unibuf = HBufC::NewL(aIdentity.Length());
            unibuf->Des().Copy(aIdentity);
            if ( StartAddr.Input(unibuf->Des()) != KErrNone )
            {
                delete unibuf;
                User::Leave(KErrArgument);
            }  
            delete unibuf;
            EndAddr = StartAddr;  // Range start and end addresses are same
            break;
        }

        default:    //Subnet
        {
            //addr1 - Start address of range
            TInt prefix_len;	 
            HBufC *unibuf = HBufC::NewL(aIdentity.Length());
            unibuf->Des().Copy(aIdentity);			
            TPtrC addr_buf(unibuf->Ptr(), offset);
            if ( StartAddr.Input(addr_buf) != KErrNone )
            {
                delete unibuf;				
                User::Leave(KErrArgument);
            }
            TPtrC prefix_ptr(unibuf->Ptr() + offset + 1, unibuf->Length() - offset - 1);
            //addr2 - End address of range
            TLex lex(prefix_ptr);
            if ( lex.Val(prefix_len) != KErrNone )
            {
               delete unibuf;
               User::Leave(KErrArgument);
            }
            delete unibuf;
            if ( !IpsecSelectors::GetRangeEndAddresses(StartAddr, EndAddr, prefix_len) )
                User::Leave(KErrArgument);	
        }

    } //end switch
    
    StartAddr.SetPort(0x0);
    EndAddr.SetPort(0xffff);
    return TIkeV2TrafficSelector(StartAddr, EndAddr, aProtocol); 
}

TBool IpsecSelectors::GetRangeEndAddresses(TInetAddr& aStartAddr, TInetAddr& aEndAddr, TInt aPrefixLen) 
{
	//
	// Convert start address / prefix length to range start address /
	// end address pair
	//
	if ( aStartAddr.Family() == KAfInet )
	{
		TUint32 Mask;		
		if ( aPrefixLen > 32 )	
			return EFalse;
		if ( aPrefixLen )
			 Mask = (~0UL << ((32 - (aPrefixLen & 31)) & 31));			
		else Mask = 0;
        TUint32 Start = (aStartAddr.Address() & Mask);
		TUint32 End   = Start | (~Mask);
		aStartAddr.SetAddress(Start);
		aEndAddr.SetAddress(End);						
	}
	else    //KAfInet6
	{
		if ( aPrefixLen > 128 )	
			return EFalse;
		aStartAddr.Prefix(aStartAddr, aPrefixLen); // For sure
		TUint32 M  = (~0UL >> (aPrefixLen & 31));		
		TIp6Addr S = aStartAddr.Ip6Address();
		TIp6Addr E;
		aPrefixLen >>= 5;
		TInt i;		
		for (i = 0; i < aPrefixLen; i++)
			E.u.iAddr32[i] = S.u.iAddr32[i];

		i <<= 2;
		E.u.iAddr8[i] = (TUint8)(S.u.iAddr8[i] | (M >> 24)); i++;
		E.u.iAddr8[i] = (TUint8)(S.u.iAddr8[i] | (M >> 16)); i++;
		E.u.iAddr8[i] = (TUint8)(S.u.iAddr8[i] | (M >> 8)); i++;
		E.u.iAddr8[i] = (TUint8)(S.u.iAddr8[i] |  M); i++;

		i >>= 2;
		while (i < 4)
			E.u.iAddr32[i++] = ~0UL;
		
		aStartAddr.SetAddress(S);
		aEndAddr.SetAddress(E);						
	}

	return ETrue;

}

			
TBool IpsecSelectors::ValidataTs(const CArrayFix<TIkeV2TrafficSelector>& aTsRef, 
                                 const CArrayFix<TIkeV2TrafficSelector>& aTs)
{
    //For every selector in aTs, there must be a same or wider selector in aTsRef.
    for (TInt i = 0; i < aTs.Count(); ++i)
        {        
        TInt j;
        const TIkeV2TrafficSelector& selector = aTs[i];
        for (j = 0; j < aTsRef.Count(); ++j)
            {
            const TIkeV2TrafficSelector& refSelector = aTsRef[j];
            if (selector <= refSelector)
                {
                break;
                }
            }
        if (j == aTsRef.Count())
            {
            //No selector found
            return EFalse;
            }
        }
    return ETrue;    
}	


TBool IpsecSelectors::CheckPorts(TUint16 aStartRef, TUint16 aEndRef, TUint16 aStart, TUint16 aEnd )
{
	//
	// Check that current port range narrowed intersection of reference
	// port range
	//
	if ( (aStartRef > aStart) || (aEndRef < aEnd) || (aStart > aEnd) )
		return EFalse;
	
	return ETrue;
	
}

TBool IpsecSelectors::CheckAddresses(TUint8 aType, TUint8* aRefAddresses, TUint8* aAddresses )
{
	//
	// Check that current address range narrowed intersection of reference
	// address range
	//
	if ( aType == TS_IPV4_ADDR_RANGE )
	{
        ASSERT(aRefAddresses && aAddresses);

	   //
	   // Comparison is done as 32 bit integers
	   //
		TUint32 StartRef = GET32(aRefAddresses);
		TUint32 EndRef   = GET32(aRefAddresses + 4);		
		TUint32 Start    = GET32(aAddresses);
		TUint32 End      = GET32(aAddresses + 4);
		if ( (StartRef > Start) || (EndRef < End) || (Start > End) )
			return EFalse;
	}
	
	return ETrue;  
}
