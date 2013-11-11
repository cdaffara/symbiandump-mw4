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
* Description: IKEv2 Proposal handling.
*
*/


#include <e32math.h>
#include <in_sock.h>
#include "ikedebug.h"
#include "ikev2proposal.h"
#include "ikev2SAdata.h"
#include "ikev2payloads.h"
#include "ikemsgrec.h"
#include "ikev2const.h"
#include "ikev2plugin.h"
#include "ikepolparser.h"
#include "ikev2identity.h"
#include "ikev2ipsecsadata.h"
#include "ikev2Negotiation.h"
#include <networking/pfkeyv2.h>

HBufC8* Ikev2Proposal::FromPolicyToProposaL(TIkev2SAData& aIkeSaData, 
                                            const TDesC8& aRekeySpi, 
                                            TInt aDHGroupGuess, 
                                            TBool aRekey)
{
	//
	// Build IKE SA proposal from IKE policy data
	// Because proposal information is presented as "IKEv1"
	// proposals in policy these are presented as sequence of
	// proposals. All these transforms contains 4 different type transform
	// payloads.
	//
	TProposalData* PropList = aIkeSaData.iIkeData->iPropList;
	
	if ( !aRekey )
	{
        aIkeSaData.iEAPType = aIkeSaData.iIkeData->iEAPProtocol;
	}
	
	if (!PropList)
	{		
		User::LeaveIfNull(PropList);		
	}
	
	HBufC8* saData = HBufC8::NewL(512);    //512 should be enough for all Proposals
	
	TUint8  PropNum = 1;
	TUint16 SaLth   = 0;
	TUint16 PropLth;
	TUint16 TranLth;
	TUint16 PRF;
	TUint16 DHGroup;		

	TProposalIkev2*  Proposal = TProposalIkev2::Cast(saData->Ptr());
	TProposalIkev2*  Next = Proposal;
	TTransformIkev2* Transform;
	TDataAttributes* Attributes;

	while ( PropList )
	{

		Proposal = Next;
		TPayloadIkev2::Cast(Proposal)->Init();   // Initialize Payload general header
		TPayloadIkev2::Cast(Proposal)->SetNextPayload(IKEV2_PAYLOAD_PROP);		
		Proposal->SetNum(PropNum);
		Proposal->SetProtocol(IKEV2_PROTOCOL);
		if ( aRekey )
		{
			Proposal->SetSPISize(IKEV2_SPI_SIZE);
			Mem::Copy(Proposal->SPI(), aRekeySpi.Ptr(), IKEV2_SPI_SIZE);
		}
		else Proposal->SetSPISize(0);
		Proposal->SetNumTrans(4);
		PropLth = (TUint16)Proposal->PropHdrLth();	

		Transform = Proposal->TransformPl();
		TPayloadIkev2::Cast(Transform)->Init();   // Initialize Payload general header		
		TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);
		Transform->SetType(IKEV2_ENCR);   // Encryption Algorithm transform (1)
		Transform->SetReserved();
		TranLth = (TUint16)Transform->Size();

		switch ( PropList->iEncrAlg )
		{
			case IKE_PARSER_DES_CBC:
				Transform->SetID(ENCR_DES);				
				break;
			case IKE_PARSER_DES3_CBC:
				Transform->SetID(ENCR_3DES);				
				break;
			case IKE_PARSER_AES_CBC:
				Transform->SetID(ENCR_AES_CBC);
				//
				// Add key length attribute to transform data
				//
				Transform->SetID(ENCR_AES_CBC);
				Attributes = Transform->Attributes();
				Attributes->SetType(IKEV2_ENCR_KEY_LTH);
				Attributes->SetBasic();
				if (PropList->iEncrKeyLth)
					 Attributes->SetValue((TUint16)PropList->iEncrKeyLth);
				else Attributes->SetValue(128);	//default AES key size
				TranLth = (TUint16)(TranLth + Attributes->Size());
				break;
			default:
				Transform->SetID(ENCR_3DES);	// Use 3DES as default									
				break;
		}
		TPayloadIkev2::Cast(Transform)->SetLength(TranLth);		
		PropLth = (TUint16)(PropLth + TranLth);

		Transform = (TTransformIkev2*)TPayloadIkev2::Cast(Transform)->Next();
		TPayloadIkev2::Cast(Transform)->Init();   // Initialize Payload general header				
		TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);
		Transform->SetType(IKEV2_INTEG);   // Integrity Algorithm (3)
		Transform->SetReserved();
		TranLth = (TUint16)Transform->Size();

		switch ( PropList->iHashAlg )
		{
			case IKE_PARSER_MD5:
				Transform->SetID(AUTH_HMAC_MD5_96);
				PRF = IKE_PARSER_MD5;
				break;
			case IKE_PARSER_SHA1:
				Transform->SetID(AUTH_HMAC_SHA1_96);
				PRF = IKE_PARSER_SHA1;
				break;
			default:
				Transform->SetID(AUTH_HMAC_SHA1_96);
				PRF = IKE_PARSER_SHA1;				
				break;
		}
		TPayloadIkev2::Cast(Transform)->SetLength(TranLth);				
		PropLth = (TUint16)(PropLth + TranLth);

		Transform = (TTransformIkev2*)TPayloadIkev2::Cast(Transform)->Next();		
		TPayloadIkev2::Cast(Transform)->Init();   // Initialize Payload general header		
		TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);
		Transform->SetType(IKEV2_PRF);   // Pseudo-random Function (2)
		Transform->SetReserved();
		TranLth = (TUint16)Transform->Size();

		if ( PropList->iPRF )
			PRF = PropList->iPRF;	
		switch ( PRF )
		{
			case IKE_PARSER_MD5:
				Transform->SetID(PRF_HMAC_MD5);
				break;
			case IKE_PARSER_SHA1:
				Transform->SetID(PRF_HMAC_SHA1);
				break;
			default:
				Transform->SetID(AUTH_HMAC_SHA1_96);
				break;			
		}
		TPayloadIkev2::Cast(Transform)->SetLength(TranLth);
		PropLth = (TUint16)(PropLth + TranLth);
		

		Transform = (TTransformIkev2*)TPayloadIkev2::Cast(Transform)->Next();				
		TPayloadIkev2::Cast(Transform)->Init();   // Initialize Payload general header
		TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_NONE);
		Transform->SetType(IKEV2_DH);   // Diffie-Hellman Group (4)
		Transform->SetReserved();
		TranLth = (TUint16)Transform->Size();
		
		
		DHGroup = (PropList->iGroupDesc == 0) ? aIkeSaData.iIkeData->iGroupDesc_II : 
                                                PropList->iGroupDesc;
		
		DHGroup = Ikev2Proposal::GetDHGroup(DHGroup);
		Transform->SetID(DHGroup);
		TPayloadIkev2::Cast(Transform)->SetLength(TranLth);

		if ( PropNum == aDHGroupGuess)
		{
		   if (aIkeSaData.iDHGroup == 0) 
		       aIkeSaData.iDHGroup = DHGroup;  // Preferred group for initial KE payload
		   switch ( PropList->iAuthMeth )
		   {	   
			     case IKE_PARSER_DSS_SIG:
			         aIkeSaData.iAuthMethod = DSS_DIGITAL_SIGN;
					 break;
					 
				 case IKE_PARSER_RSA_SIG:
				 case IKE_PARSER_RSA_REV_ENCR:					 
				     aIkeSaData.iAuthMethod = RSA_DIGITAL_SIGN;
					 break;
					 
				 default:
				     aIkeSaData.iAuthMethod = PRESHARED_KEY;
					 break;

		   }		 
		}	
		
		if ( aIkeSaData.iLifetime == 0 ) 
		    aIkeSaData.iLifetime = PropList->iLifetimeSec; // Init lifetime
		else if ( PropList->iLifetimeSec && (aIkeSaData.iLifetime > PropList->iLifetimeSec) )
		    aIkeSaData.iLifetime = PropList->iLifetimeSec; // Take shorter time				

		PropLth = (TUint16)(PropLth + TranLth);
		SaLth   = (TUint16)(SaLth + PropLth);
		TPayloadIkev2::Cast(Proposal)->SetLength(PropLth);		

		PropNum ++;
		Next     = (TProposalIkev2*)TPayloadIkev2::Cast(Proposal)->Next();	
		PropList = PropList->iNext;
	}

	if ( aIkeSaData.iLifetime == 0 )
	    aIkeSaData.iLifetime = IKEV2_DEF_LIFETIME;	
	TPayloadIkev2::Cast(Proposal)->SetNextPayload(IKEV2_PAYLOAD_NONE);
	
	saData->Des().SetLength(SaLth);

	return saData;

}

HBufC8* Ikev2Proposal::GetPSKFromPolicyL(CIkeData* aHostData)
{
    ASSERT(aHostData);
	//
	// Get Preshared Key from IKE policy and return in to caller in
	// HBufc8.
	//
	HBufC8 *PSK = NULL;
	if ( aHostData->iPresharedKey.iFormat ==  STRING_KEY )
	{
	    PSK = HBufC8::NewL(aHostData->iPresharedKey.iKey.Length());
		PSK->Des().Copy(aHostData->iPresharedKey.iKey);
	}
    else if ( aHostData->iPresharedKey.iFormat == HEX_KEY ) 
    {
        PSK = HBufC8::NewL(aHostData->iPresharedKey.iKey.Length() / 2);
        
        
        for(TInt i = 0; i < aHostData->iPresharedKey.iKey.Length(); i += 2)
        {        
            TPtrC hexByte(aHostData->iPresharedKey.iKey.Mid(i, 2));
            TLex lex(hexByte);
            TUint8 value;
            User::LeaveIfError(lex.Val(value, EHex));
            
            PSK->Des().Append(&value, 1);
        }
        
    }

	return PSK;
}

TUint16 Ikev2Proposal::GetDHGroup(TInt aDHGroup)
{
	//
	// Map DH group Enum value used in IKE policy to the real DH group
	// transform type value used in IKEv2 negotiation
	// If aDHGroup parameter is not defined mapping is done to
	// iGroupDesc_II data member value in CIkeData    
	//
	TUint16 DHTransId = 0;
	switch ( aDHGroup )
	{
		case IKE_PARSER_MODP_768:
			DHTransId = DH_GROUP_768;						    
			break;
		case IKE_PARSER_MODP_1024:
			DHTransId = DH_GROUP_1024;
			break;
		case IKE_PARSER_MODP_1536:
			DHTransId = DH_GROUP_1536;
			break;
	  case IKE_PARSER_MODP_2048:
			DHTransId = DH_GROUP_2048;
			break;
		default:
			break;
	}
	
	return DHTransId;
}

HBufC8* Ikev2Proposal::BuildSaResponseL(TProposalIkev2* aAcceptedProp, CIkev2Payloads* aAcceptedTrans)
{
    ASSERT(aAcceptedProp && aAcceptedTrans);
    HBufC8* SaRespBfr = HBufC8::NewL(256);  //256 should be enough response
        
	//
	// Build SA response payload based on Transform payloads which are
	// marked to be "SELECTED" in request proposal
	//
	TProposalIkev2*  Proposal = TProposalIkev2::Cast(const_cast<TUint8*>(SaRespBfr->Ptr()));
	TUint16 PropLth = (TUint16)aAcceptedProp->PropHdrLth();	
	Mem::Copy((TUint8*)Proposal, (TUint8*)aAcceptedProp, PropLth);

	TTransformIkev2* Transform = Proposal->TransformPl();
	TTransformIkev2* LastTrans = Transform;
	TTransformIkev2* AccTransform;
	TUint8 NbrOfTransforms = 0;
	TInt TranCount  = aAcceptedTrans->iTrans->Count();
	for ( TInt i = 0; i < TranCount; ++i )		   	
	{
		AccTransform = (TTransformIkev2*)aAcceptedTrans->iTrans->At(i);
		if ( AccTransform->IsSelected() )
		{
		   NbrOfTransforms ++;
		   Mem::Copy((TUint8*)Transform, (TUint8*)AccTransform, TPayloadIkev2::Cast(AccTransform)->GetLength());
		   Transform->NotSelected(); // Reset selected bit !		   		   
		   PropLth = (TUint16)(PropLth + TPayloadIkev2::Cast(AccTransform)->GetLength());
		   TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);
		   LastTrans = Transform;
		   Transform = (TTransformIkev2*)TPayloadIkev2::Cast(Transform)->Next();
		}	
	}
	TPayloadIkev2::Cast(LastTrans)->SetNextPayload(IKEV2_PAYLOAD_NONE);		   	
	TPayloadIkev2::Cast(Proposal)->SetNextPayload(IKEV2_PAYLOAD_NONE);	
	TPayloadIkev2::Cast(Proposal)->SetLength(PropLth);
	Proposal->SetNumTrans(NbrOfTransforms);
	SaRespBfr->Des().SetLength(PropLth);
	
	return SaRespBfr;
}


TBool Ikev2Proposal::GetSelectedProposalData(TIkev2SAData& aIkev2SaData,
                                             TIkeV2IpsecSAData& aChildSaData,
                                             const CIkev2Payloads& aAcceptedProp, 
                                             const TProposalIkev2& aProp)
{
	//
	// Get IKE SA algorithm information from Transform payload which are
	// marked to be "SELECTED"
	//
	TTransformIkev2* Transform;
	TDataAttributes* Attribute;
	TUint16 EncrAlg;
	TInt    KeyLth;
	TUint8 ExistingTypes = 0;
	TUint8 RequiredTypes;
	TUint8 Protocol = aProp.GetProtocol();
	switch ( Protocol )
	{
		case IKEV2_IPSEC_AH:
		    {
			RequiredTypes  = (1 << IKEV2_INTEG);
			aChildSaData.iSaType = SADB_SATYPE_AH;
			TUint32 spi = 0;
			aProp.GetIpsecSPI(&spi);			
			aChildSaData.iSPI_Out = TPtrC8(reinterpret_cast<TUint8*>(&spi), sizeof(spi));
		    }
			break;
		case IKEV2_IPSEC_ESP:
		    {
			RequiredTypes = (1 << IKEV2_ENCR);
			aChildSaData.iSaType = SADB_SATYPE_ESP;
            TUint32 spi = 0;
            aProp.GetIpsecSPI(&spi);           			
            aChildSaData.iSPI_Out = TPtrC8(reinterpret_cast<TUint8*>(&spi), sizeof(spi));
		    }
			break;
		default:  //IKEV2_PROTOCOL:
			RequiredTypes = ((1 << IKEV2_ENCR) | (1 << IKEV2_PRF) | (1 << IKEV2_INTEG) | (1 << IKEV2_DH));
			break;			
	}	
	
	TInt TranCount  = aAcceptedProp.iTrans->Count();
	
	for ( TInt i = 0; i < TranCount; ++i )		   	
	{
		Transform = (TTransformIkev2*)aAcceptedProp.iTrans->At(i);
		if ( Transform->IsSelected() )
		{
			Transform->NotSelected(); // Reset private "selected" bit
			switch ( Transform->GetType() )
			{
				case IKEV2_ENCR:
					ExistingTypes |= (1 << IKEV2_ENCR);
					EncrAlg = Transform->GetID();
					if ( Protocol == IKEV2_PROTOCOL )
					    aIkev2SaData.iEncrAlg   = EncrAlg;
					else aChildSaData.iEncrAlg = EncrAlg;
					if ( EncrAlg == ENCR_AES_CBC )
					{
					    //
						// Get encryption key length from attributes
						// (or use default key length 128 bit)
						//
					   if ( TPayloadIkev2::Cast(Transform)->GetLength() > Transform->Size() )
					   {
						  Attribute = Transform->Attributes();
						  KeyLth    = (Attribute->GetValue() >> 3); // byte length
					   }
					   else KeyLth = 16;  // default: 16 bytes = 128 bits
					   if ( Protocol == IKEV2_PROTOCOL )					   
					       aIkev2SaData.iCipherKeyLth   = KeyLth;
					   else aChildSaData.iCipherKeyLth = KeyLth;
					}
					break;

				case IKEV2_PRF:
					ExistingTypes |= (1 << IKEV2_PRF);
					if ( Protocol == IKEV2_PROTOCOL )					   
					    aIkev2SaData.iPRFAlg = Transform->GetID();
					break;

				case IKEV2_INTEG:
					ExistingTypes |= (1 << IKEV2_INTEG);
					if ( Protocol == IKEV2_PROTOCOL )
					    aIkev2SaData.iIntegAlg   = Transform->GetID();
					else aChildSaData.iIntegAlg = Transform->GetID();
					break;

				case IKEV2_DH:
					ExistingTypes |= (1 << IKEV2_DH);
					if ( Protocol == IKEV2_PROTOCOL )
					    aIkev2SaData.iDHGroup   = Transform->GetID();
					break;

				case IKEV2_ESN:
					ExistingTypes |= (1 << IKEV2_ESN);
					if ( Protocol != IKEV2_PROTOCOL )
					    aChildSaData.iESN = (TUint8)Transform->GetID();
					break;
					
				default:
					break;
					
			}	
		}	

	}
	
 	return ((RequiredTypes & ExistingTypes) == RequiredTypes);
}	

TBool Ikev2Proposal::VerifySaResponseL(TIkev2SAData& aIkeSaData, 
                                       TIkeV2IpsecSAData& aIpsecSaData,  
                                       const TDesC8& aReferenceSaData, 
                                       const CIkev2Payloads& aRespProp)
{
	//
	// Verify content of an IKE SA response to proposed IKE SA transform
	// list. The IKE SA proposal selected by peer MUST contain one
	// proposal and transform selected from our SA proposal  
	//
	if ( aRespProp.iProps->Count() != 1 )
		return EFalse;
	
	TBool Status = EFalse;
	TPtrC8 unprocessedReferenceSaData(aReferenceSaData);
	
	while(!Status && unprocessedReferenceSaData.Length() > 0)
	    {	    
        TPayloadIkev2* referenceProposal = TPayloadIkev2::Cast(unprocessedReferenceSaData.Ptr());
        CIkev2Payloads* OwnProp = CIkev2Payloads::NewL(referenceProposal, IKEV2_PAYLOAD_PROP, aIkeSaData);        
        CleanupStack::PushL(OwnProp);
        
        //Something is seriously wrong, if we can't parse our own reference data
        __ASSERT_DEBUG(OwnProp->Status() == KErrNone, User::Invariant());
    
        TProposalIkev2* Prop = (TProposalIkev2*)aRespProp.iProps->At(0); 	  	
        Status = Ikev2Proposal::VerifyProposaL(OwnProp, Prop, aIkeSaData);
        if ( Status )
        {	   
           Status = Ikev2Proposal::GetSelectedProposalData(aIkeSaData, aIpsecSaData, aRespProp, *Prop);
        }	     
        CleanupStack::PopAndDestroy(OwnProp); 
        unprocessedReferenceSaData.Set(unprocessedReferenceSaData.Mid(referenceProposal->GetLength()));
	    }
	
	return Status;
}

TBool Ikev2Proposal::VerifySaRequestAndGetProposedSaBufferL(TIkev2SAData& aIkeSaData, 
                                                            TIkeV2IpsecSAData& aIpsecSaData,
                                                            const TDesC8& aReferenceSaData, 
                                                            const CIkev2Payloads& aProposed,
                                                            HBufC8*& aProposedSaBuffer)
{
    __ASSERT_DEBUG(aReferenceSaData.Length() > 0, User::Invariant());
    
    
	//
	// Verify content of an IKE SA request against a reference
	// proposals built according to the local policy.
	//
	if ( !aProposed.iSa )
		return EFalse;
	
	TBool Status = EFalse;
	TPtrC8 unprocessedReferenceSaData(aReferenceSaData);
	while (!Status && unprocessedReferenceSaData.Length() > 0)
	{	
        TPayloadIkev2* referenceSa = TPayloadIkev2::Cast(unprocessedReferenceSaData.Ptr());
        unprocessedReferenceSaData.Set(unprocessedReferenceSaData.Mid(referenceSa->GetLength()));
        CIkev2Payloads* OwnProp = CIkev2Payloads::NewL(referenceSa, IKEV2_PAYLOAD_PROP, aIkeSaData);
        //If we can't parse our own reference proposal something is seriously wrong.
        __ASSERT_DEBUG(OwnProp->Status() == KErrNone, User::Invariant());
        CleanupStack::PushL(OwnProp);
        
    
       CIkev2Payloads* PeerProp = CIkev2Payloads::NewL((TPayloadIkev2*)aProposed.iSa, IKEV2_PAYLOAD_SA, aIkeSaData);
       CleanupStack::PushL(PeerProp);
       Status = (PeerProp->Status() == KErrNone);
       if ( Status )
       {	   
          Status = EFalse; 		
          TInt PropCount = PeerProp->iProps->Count();
          for ( TInt i = 0; i < PropCount; ++i )		   
          {
              TProposalIkev2* Prop = (TProposalIkev2*)PeerProp->iProps->At(i);
              Status = Ikev2Proposal::VerifyProposaL(OwnProp, Prop, aIkeSaData);
              if ( Status )
              {
                 //
                 // Build SA response payload and pick up algorithm
                 // information into negotiation object
                 //
                 
                 HBufC8* SaRespBfr = NULL; 
                 TRAPD(err, SaRespBfr = Ikev2Proposal::BuildSaResponseL(Prop, PeerProp));
                 if (err == KErrNone)
                     {
                     aProposedSaBuffer = SaRespBfr;				 
                     Status = Ikev2Proposal::GetSelectedProposalData(aIkeSaData, aIpsecSaData, *PeerProp, *Prop);
                     }
                 else
                     {
                     Status = EFalse;
                     }
                 break;
              }	 
          }	
       }
       CleanupStack::PopAndDestroy(PeerProp); 	   
       CleanupStack::PopAndDestroy(OwnProp); 
	}
	return Status;
}	

TBool Ikev2Proposal::IkeSaRekey(CIkev2Payloads* aIkeMsg)
{
    ASSERT(aIkeMsg);
	//
	// Check is the current IKE message an IKE SA rekey request
	// Should be format: HDR(A,B), SK { SA, Ni, KEi }
	// where proposal protcol should be IKEV2_PROTOCOL
	//
	TBool Status = EFalse;
	if ( aIkeMsg->iProps->Count() && !aIkeMsg->iTsI && !aIkeMsg->iTsR )
	{
		TProposalIkev2* Prop = (TProposalIkev2*)aIkeMsg->iProps->At(0);
		Status = (Prop->GetProtocol() == IKEV2_PROTOCOL);
	}	
	return Status;
}	

TBool Ikev2Proposal::GetRekeySpi(CIkev2Payloads* aIkeMsg, TIkeSPI& aSPI)
{
    ASSERT(aIkeMsg);
	//
	// Get remote ends IKE SPI from the first Proposal of IKE message
	//
	TBool Status = EFalse;
	if ( aIkeMsg->iProps->Count() )
	{
		TProposalIkev2* Proposal = (TProposalIkev2*)aIkeMsg->iProps->At(0);
		if ( Proposal->GetSPISize() == IKEV2_SPI_SIZE )
		{	
		   Mem::Copy( (TUint8*)aSPI.Ptr(), Proposal->SPI(), IKEV2_SPI_SIZE);
		   Status = ETrue;
		}   
	}
	return Status;
}	

void Ikev2Proposal::ChangeSpiInProposal(HBufC8* aSaBfr, TIkeSPI& aSPI)
{
    ASSERT(aSaBfr);	
	TProposalIkev2*  Proposal  = TProposalIkev2::Cast(aSaBfr->Ptr());
	Mem::Copy(Proposal->SPI(), (TUint8*)aSPI.Ptr(), IKEV2_SPI_SIZE);
}

TBool Ikev2Proposal::VerifyProposaL(CIkev2Payloads* aReference, TProposalIkev2* aProposal, TIkev2SAData& aIkev2SaData)  
{
	//
	// Find a matching proposal for "candidate" from reference payload
	// chain. This implementation does not support the AND of sequental
	// proposals (for example proposal which defines (ESP and AH))
	//
	if ( !aProposal || !aReference || (aReference->iProps->Count() == 0) )
	    return EFalse;

	CIkev2Payloads* Prop = CIkev2Payloads::NewL(TPayloadIkev2::Cast(aProposal), IKEV2_PAYLOAD_PROP, aIkev2SaData);
	CleanupStack::PushL(Prop);
	TBool Status = ( Prop->Status() == KErrNone );
	if ( Status )
	{
		
	   Status = EFalse; 
	   TInt PropCount = aReference->iProps->Count();
	   TProposalIkev2* RefProp;
	   
	   for ( TInt i = 0; i < PropCount; ++i )
	   {
		   RefProp = (TProposalIkev2*)aReference->iProps->At(i);		
    	   if ( !aReference->ParsePayloadL(TPayloadIkev2::Cast(RefProp), IKEV2_PAYLOAD_PROP ))// Transforms from Proposal
			  break;
		   if ( aReference->iTrans->Count() == 0 )
			  break;
		   if ( RefProp->GetProtocol() != aProposal->GetProtocol() )
		      continue;
		   //
		   // Compare transforms within proposals 
		   //
    	   Status = Ikev2Proposal::CompareTransforms(aReference->iTrans, Prop->iTrans);
		   if ( Status )
		      break;   // Match found
	   }
	}
	CleanupStack::PopAndDestroy(Prop); 
	
	return Status;
}      

TBool Ikev2Proposal::CompareTransforms(CArrayFixFlat<TTransformIkev2*>* aRefTrans,
									   CArrayFixFlat<TTransformIkev2*>* aTrans)  
{
    ASSERT(aTrans && aRefTrans);
	//
	// "Select" matching transforms from transform list (aTrans).
	// Transforms from aTrans array is marked "SELECTED" if there is a
	// matching transform in aRefTrans array for existing transform
	// types.
	//
	TUint8 TransType;
	TTransformIkev2* Trans;
	TTransformIkev2* RefTrans;
	TDataAttributes* Attribute;
	
	TInt TranCount2;
	TUint16 Lth;
	TUint8 ExistingTypes = 0;
	TUint8 MatchingTypes = 0;
	TInt TranCount  = aTrans->Count();
	TInt i;
	
	for ( i = 0; i < TranCount; ++i )		
	{
 		Trans = aTrans->At(i);
		TransType = Trans->GetType();
		if ( (TransType < IKEV2_ENCR ) || (TransType > IKEV2_ESN) )
		   break;  // Unknown transform type (error)

		ExistingTypes |= (1 << TransType);
		TranCount2 = aRefTrans->Count();

		for ( TInt j = 0; j < TranCount2; ++j )				
		{
			RefTrans = aRefTrans->At(j);
			
			if ( (TransType != RefTrans->GetType()) || (Trans->GetID() != RefTrans->GetID()) )
			   continue;
			//
			// Matching transform type and ID. Check is there any
			// attributes in transform (in this phase only IKEV2_ENCR
			// transform type can contain an attribute AES key length) 
			//
			Lth = TPayloadIkev2::Cast(Trans)->GetLength();
			if ( Lth >= Trans->Size() )
			{
			   if (( TransType == IKEV2_ENCR ) && (Trans->GetID() == ENCR_AES_CBC) )
			   {
				  TUint16 KeyLth    = 128;     
				  TUint16 RefKeyLth = 128;  
				  if ( Lth > Trans->Size() )
				  {
					 Attribute = Trans->Attributes(); 
					 Lth = (TUint16)(Lth - Trans->Size());
					 if ( (Lth == Attribute->Size()) && Attribute->IsBasic() && (Attribute->GetType() == IKEV2_ENCR_KEY_LTH) )
						KeyLth = Attribute->GetValue();
				  }	  
				  if ( TPayloadIkev2::Cast(RefTrans)->GetLength() > Trans->Size() )
				  {
					 Attribute = RefTrans->Attributes();
					 RefKeyLth = Attribute->GetValue();
				  }		  
				  if ( KeyLth != RefKeyLth ) 
					 continue;  // Not matching attribute
			   }   
			}
			//
			// Mark current transform "SELECTED"
			//
			if ( (MatchingTypes & (1 << TransType) ) == 0 )
			{	
			   Trans->Selected();
			   MatchingTypes |= (1 << TransType);
			}   
			break;
		}   	
	}

	TBool Status = (ExistingTypes == MatchingTypes);
	if ( !Status )
	{
	   //
	   // No match ! Reset "SELECTED" indicator from transforms
	   //
	    i = 0;
		while ( i < TranCount )
		{
			Trans = (TTransformIkev2*)aTrans->At(i);
			Trans->NotSelected();
			i ++;
		}	   
	}	

	return Status;

}	

CIkeV2Identity* Ikev2Proposal::GetRemoteIdentityL(CIkeData* aHostData)
{
    ASSERT(aHostData);
    CIkeV2Identity* identity = NULL;   
	
	if ( aHostData->iRemoteIdentity )
	{
		TPtrC16 idData = aHostData->iRemoteIdentity->GetData();	
	    TUint8 idType = aHostData->iRemoteIdType; 	
		if ( (idType == ID_IPV4_ADDR) || (idType == ID_IPV6_ADDR) )
		{
		    //
		    // If configured remote id type is either IPv4- or IPv6 address
		    // convert ASCII format address data into hexa octet string IP
		    // address format: IPv4 address shall be represented as four
		    // octet string and Ipv6 address as 16 octet string
		    //
		    TInetAddr ipAddr;
			if ( ipAddr.Input(idData) == KErrNone )
			{
				if ( idType == ID_IPV4_ADDR )
				{
				    TUint32 ipv4 = ByteOrder::Swap32(ipAddr.Address());
				    TPtrC8 ipv4Ptr(reinterpret_cast<TUint8*>(&ipv4), sizeof(ipv4));
				    identity = CIkeV2Identity::NewL(idType, ipv4Ptr);
				}
				else
				{
                    TPtrC8 IPv6Ptr(&ipAddr.Ip6Address().u.iAddr8[0], 16);
                    identity = CIkeV2Identity::NewL(idType, IPv6Ptr);
				}
			}
		}
		else
		{
            if ( (idType != ID_FQDN) && (idType != ID_RFC822_ADDR) )
                {
                idType = ID_FQDN; // Default
                }
            
            HBufC8* id = HBufC8::NewLC(idData.Length());
            TPtr8 idPtr(id->Des());
            idPtr.Copy(idData);
            identity = CIkeV2Identity::NewL(idType, *id);
            CleanupStack::PopAndDestroy(id);
		}
	}
	
	return identity;
}
