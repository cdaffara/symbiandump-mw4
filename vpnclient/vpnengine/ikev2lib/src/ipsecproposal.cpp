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
* Description:   Ipsec Proposal handling
*
*/
#include <ipsecpolapi.h>
#include "ikedebug.h"
#include "ipsecproposal.h"
#include "ikev2proposal.h"
#include "ikev2payloads.h"
#include "ikemsgrec.h"
#include "ikev2const.h"
#include <networking/pfkeyv2.h>
#include "pfkeymsg.h"
#include "ipsecsalist.h"

HBufC8* IpsecProposal::BuildIpsecSaRequestL(const TPfkeyMessage& aPfkeyMessage, TUint16 aDHGroup)
{
    return BuildIpsecSaRequestL(aPfkeyMessage.iBase.iMsg->sadb_msg_satype,
                                aPfkeyMessage.iProposal.iComb->sadb_comb_encrypt,
                                aPfkeyMessage.iProposal.iComb->sadb_comb_encrypt_maxbits,
                                aPfkeyMessage.iProposal.iComb->sadb_comb_auth,
                                aPfkeyMessage.iProposal.iComb->sadb_comb_flags,
                                aDHGroup);
}


HBufC8* IpsecProposal::BuildIpsecSaRequestL(const TUint8 aSaType, const TUint8 aEncryptAlg, 
                                            const TUint16 aEncryptMaxbits, 
                                            const TUint8 aAuthAlg, const TUint16 aFlags,
                                            TUint16 aDHGroup)
{
	//
	// Build Ipsec SA proposal from PFKEY acquire primitive policy data
	// In this phase PFKEY Aqcuire contains only one proposal
	// (transform)
	//
    HBufC8* proposal = HBufC8::NewL(1024);   
    
	TUint8  TransCnt = 0;
	TBool   IntegAlg = EFalse;
	TUint16 SaLth    = 0;
	TUint16 TranLth  = 0;	
	TUint16 PropLth  = 0;

	TProposalIkev2*  Proposal  = TProposalIkev2::Cast(const_cast<TUint8*>(proposal->Ptr()));
	TTransformIkev2* Transform = NULL;
	TDataAttributes* Attributes;
	
	TPayloadIkev2::Cast(Proposal)->Init();   // Initialize Payload general header
	TPayloadIkev2::Cast(Proposal)->SetNextPayload(IKEV2_PAYLOAD_NONE);		
	Proposal->SetNum(1);
	Proposal->SetSPISize(4); // SPI value shall be added later to proposal
	PropLth = (TUint16)Proposal->PropHdrLth();

	switch ( aSaType )
	{
		case SADB_SATYPE_AH:
			Proposal->SetProtocol(IKEV2_IPSEC_AH);
			IntegAlg = ETrue;
			break;

		case SADB_SATYPE_ESP:
			TransCnt ++;
			Proposal->SetProtocol(IKEV2_IPSEC_ESP);
			Transform = Proposal->TransformPl();
			TPayloadIkev2::Cast(Transform)->Init();   // Initialize Payload general header
			TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);	
			Transform->SetReserved();
			Transform->SetType(IKEV2_ENCR);   // Encryption Algorithm transform (1)
			TranLth = (TUint16)Transform->Size();
			
			switch ( aEncryptAlg )
			{
				case ENCR_DES:
					Transform->SetID(ENCR_DES);
					break;

				case ENCR_3DES:
					Transform->SetID(ENCR_3DES);
					break;

				case ENCR_NULL:
					Transform->SetID(ENCR_NULL);
					break;

				case ENCR_AES_CBC:
					Transform->SetID(ENCR_AES_CBC);
					//
					// Variable key length algorithm. Get key length
					// attribute to transform data.
					//
					Attributes = Transform->Attributes();
					Attributes->SetType(IKEV2_ENCR_KEY_LTH);
					Attributes->SetBasic();
					if ( aEncryptMaxbits )
						 Attributes->SetValue(aEncryptMaxbits);
					else Attributes->SetValue(128);	//default AES key size
					TranLth = (TUint16)(TranLth + Attributes->Size());
					break;

				default:
					User::Leave(KErrNotSupported);
                    break;
			}
			TPayloadIkev2::Cast(Transform)->SetLength(TranLth);		
			PropLth = (TUint16)(PropLth + TranLth);
			if ( aAuthAlg != SADB_AALG_NONE )
				IntegAlg = ETrue;	
			break;

		default:
			User::Leave(KErrNotSupported);
            break;
	}
	
	if ( IntegAlg )
	{
		TransCnt ++;				
		if ( Transform )
		    {
		    Transform = (TTransformIkev2*)TPayloadIkev2::Cast(Transform)->Next();
		    }
		else 
		    {
		    Transform = Proposal->TransformPl();
		    }
		TPayloadIkev2::Cast(Transform)->Init();   // Initialize Payload general header				
		TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);
		Transform->SetType(IKEV2_INTEG);          // Integrity Algorithm transform
		Transform->SetReserved();		
		TranLth = (TUint16)Transform->Size();		
		switch ( aAuthAlg )
		{
			case SADB_AALG_MD5HMAC:
				Transform->SetID(AUTH_HMAC_MD5_96);
				break;

			case SADB_AALG_SHA1HMAC:
				Transform->SetID(AUTH_HMAC_SHA1_96);
				break;

			default:
				User::Leave(KErrNotSupported);
		}
		TPayloadIkev2::Cast(Transform)->SetLength(TranLth);		
		PropLth = (TUint16)(PropLth + TranLth);
	}

	if ( (aDHGroup != 0 ) && (aFlags & SADB_SAFLAGS_PFS) )
	{
	   TransCnt ++;
	   Transform = (TTransformIkev2*)TPayloadIkev2::Cast(Transform)->Next();
	   TPayloadIkev2::Cast(Transform)->Init();   // Initialize Payload general header
	   TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);	   
	   Transform->SetType(IKEV2_DH);   // Diffie-Hellman Group (4)
	   Transform->SetReserved();
	   Transform->SetID(aDHGroup);	   
	   TranLth = (TUint16)Transform->Size();
	   TPayloadIkev2::Cast(Transform)->SetLength(TranLth);		
	   PropLth = (TUint16)(PropLth + TranLth);
	}
	//
	// Add ESN trasnform to Proposal with fixed value not used (0) 
	//
	TransCnt ++;
	Transform = (TTransformIkev2*)TPayloadIkev2::Cast(Transform)->Next();
	TPayloadIkev2::Cast(Transform)->Init();   // Initialize Payload general header
	TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_NONE);
	Transform->SetType(IKEV2_ESN);   // ESN (5) [64-bit sequence numbers with ESP]
	Transform->SetReserved();
	Transform->SetID(0);	   
	TranLth = (TUint16)Transform->Size();
	TPayloadIkev2::Cast(Transform)->SetLength(TranLth);		
	PropLth = (TUint16)(PropLth + TranLth);
	
	TPayloadIkev2::Cast(Transform)->SetNextPayload(IKEV2_PAYLOAD_NONE);	
	Proposal->SetNumTrans(TransCnt);
	TPayloadIkev2::Cast(Proposal)->SetLength(PropLth);			
	SaLth  = (TUint16)(SaLth + PropLth);
		
	proposal->Des().SetLength(SaLth);
	
	HBufC8 *reAllocatedProposal = proposal->ReAlloc(proposal->Length()); 
	if (reAllocatedProposal != NULL)
	    {
	    proposal = reAllocatedProposal;
	    }
	
	return proposal;
}


HBufC8* IpsecProposal::BuildIpsecSaFromPolicyL(const CIpsecSaSpecList& aSaList, TUint16 aDhGroup)
{
    __ASSERT_DEBUG(aSaList.Count() > 0, User::Invariant());

    static const TUint KProposalMaxLength = 64;
    static const TUint KSpiSize = 4;
    HBufC8* saData = HBufC8::NewL(KProposalMaxLength);
    TPtr8 saDataPtr = saData->Des();

    const TIpsecSaSpec& saSpec = aSaList.At(0);
    TProposalIkev2* proposal  = TProposalIkev2::Cast(saDataPtr.Ptr());
    TPayloadIkev2::Cast(proposal)->Init();   // Initialize Payload general header
    TPayloadIkev2::Cast(proposal)->SetNextPayload(IKEV2_PAYLOAD_NONE);      
    proposal->SetNum(1);
    proposal->SetSPISize(KSpiSize); // SPI value shall be added later to proposal

    
    TTransformIkev2* transform = NULL;
    
    TUint8 transformCount = 0;
    switch ( saSpec.iType )
    {
        case SADB_SATYPE_AH:
            proposal->SetProtocol(IKEV2_IPSEC_AH);
            saDataPtr.SetLength(proposal->Size() + KSpiSize);
            break;

        case SADB_SATYPE_ESP:
            {
            transformCount++;
            proposal->SetProtocol(IKEV2_IPSEC_ESP);
            transform = proposal->TransformPl();
            TPayloadIkev2::Cast(transform)->Init();   // Initialize Payload general header
            TPayloadIkev2::Cast(transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);    
            transform->SetReserved();
            transform->SetType(IKEV2_ENCR);   // Encryption Algorithm transform (1)
            
            TUint16 tranformLength = transform->Size();

            transform->SetID(saSpec.iEalg);
            if ( saSpec.iEalg == ENCR_AES_CBC )
            {
                //
                // Variable key length algorithm. Get key length
                // attribute to transform data.
                //
                TDataAttributes* attributes = transform->Attributes();
                attributes->SetType(IKEV2_ENCR_KEY_LTH);
                attributes->SetBasic();
                if ( saSpec.iEalgLen )
                     attributes->SetValue((TUint16)saSpec.iEalgLen);
                else attributes->SetValue(128); //default AES key size
                tranformLength += (TUint16)attributes->Size();
            }   
            TPayloadIkev2::Cast(transform)->SetLength(tranformLength);
            saDataPtr.SetLength(proposal->Size() + KSpiSize + tranformLength);            
            }
            break;

        default:
        	  User::Leave(KErrNotSupported);
            break;

    }   
    
    if (  saSpec.iType == SADB_SATYPE_AH ||
         (saSpec.iType == SADB_SATYPE_ESP && saSpec.iAalg != SADB_AALG_NONE) )
     {
         transformCount++;                
         if ( transform )
             {
             transform = (TTransformIkev2*)TPayloadIkev2::Cast(transform)->Next();
             }
         else 
             {         
             transform = proposal->TransformPl();
             }
         
         TPayloadIkev2::Cast(transform)->Init();   // Initialize Payload general header              
         TPayloadIkev2::Cast(transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);
         transform->SetType(IKEV2_INTEG);          // Integrity Algorithm transform
         transform->SetReserved();                  
         switch ( saSpec.iAalg )
         {
             case SADB_AALG_MD5HMAC:
                 transform->SetID(AUTH_HMAC_MD5_96);
                 break;

             case SADB_AALG_SHA1HMAC:
                 transform->SetID(AUTH_HMAC_SHA1_96);
                 break;

             default:
                 break;
         }
         TPayloadIkev2::Cast(transform)->SetLength(transform->Size());     
         saDataPtr.SetLength(saDataPtr.Length() + transform->Size()); 
     }    
    
    if ( (aDhGroup != 0 ) && saSpec.iPfs )
    {
        transformCount++;
        transform = (TTransformIkev2*)TPayloadIkev2::Cast(transform)->Next();
        TPayloadIkev2::Cast(transform)->Init();   // Initialize Payload general header
        TPayloadIkev2::Cast(transform)->SetNextPayload(IKEV2_PAYLOAD_TRANS);       
        transform->SetType(IKEV2_DH);   // Diffie-Hellman Group (4)
        transform->SetReserved();
        transform->SetID(aDhGroup);            
        TPayloadIkev2::Cast(transform)->SetLength(transform->Size());     
        saDataPtr.SetLength(saDataPtr.Length() + transform->Size());
    }
    
    transformCount++;
    transform = (TTransformIkev2*)TPayloadIkev2::Cast(transform)->Next();
    TPayloadIkev2::Cast(transform)->Init();   // Initialize Payload general header
    TPayloadIkev2::Cast(transform)->SetNextPayload(IKEV2_PAYLOAD_NONE);
    transform->SetType(IKEV2_ESN);   // ESN (5) [64-bit sequence numbers with ESP]
    transform->SetReserved();
    transform->SetID(0);       
    TPayloadIkev2::Cast(transform)->SetLength(transform->Size());     
    saDataPtr.SetLength(saDataPtr.Length() + transform->Size());

    proposal->SetNumTrans(transformCount);
    TPayloadIkev2::Cast(proposal)->SetLength(saDataPtr.Length());
            
    return saData;
}

