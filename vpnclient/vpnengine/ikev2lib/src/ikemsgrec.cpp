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
* Description: Received IKE message handling rules.
*
*/

#include "ikev2SAdata.h"
#include "ikemsgrec.h"
#include "ikecrypto.h"
//
//  CIkev2Payloads
//

CIkev2Payloads::CIkev2Payloads(const TIkev2SAData& aIkeV2SaData)
:iIkeV2SaData(aIkeV2SaData)
{
}

CIkev2Payloads::~CIkev2Payloads()
{
	delete iProps;
	delete iTrans;  	
	delete iCerts;
	delete iCertReqs;
	delete iNotifs;
	delete iDeletes; 	
	delete iVids;
	delete iGenPlds;
}

CIkev2Payloads* CIkev2Payloads::NewL(const ThdrISAKMP &aHdr, const TIkev2SAData& aIkeV2SaData)
{
	CIkev2Payloads* Payloads = new (ELeave) CIkev2Payloads(aIkeV2SaData);
	CleanupStack::PushL(Payloads);

	Payloads->ConstructL();

	Payloads->iIkeMsg = (ThdrISAKMP*)&aHdr;  // Save pointer to IKE message data
	TInt Lth = aHdr.GetLength() - ThdrISAKMP::Size();
	Payloads->ParsePayloadsL(TPayloadIkev2::Cast(aHdr.Next()), Lth, aHdr.GetPayload(), IKEV2_PAYLOAD_NONE);
	
	CleanupStack::Pop(Payloads);
	
	return Payloads;
}

CIkev2Payloads* CIkev2Payloads::NewL(TPayloadIkev2* aPayload, TUint8 aPayloadType, TIkev2SAData& aIkeV2SaData)
{
	User::LeaveIfNull((TAny*)aPayload);
	CIkev2Payloads* Payloads = new (ELeave) CIkev2Payloads(aIkeV2SaData);
	CleanupStack::PushL(Payloads);

	Payloads->ConstructL();

	Payloads->ParsePayloadsL(aPayload, aPayload->GetLength(), aPayloadType, aPayloadType);	

	CleanupStack::Pop(Payloads);

	return Payloads;
}

TBool CIkev2Payloads::ParsePayloadL(TPayloadIkev2* aPayload, TUint16 aPlType)
{
	User::LeaveIfNull((TAny*)aPayload);

	if ( aPlType == IKEV2_PAYLOAD_SA )
	    iProps->Reset();
	else if ( aPlType == IKEV2_PAYLOAD_PROP )
		iTrans->Reset();		
	if ( ParsePayloadsL(aPayload, aPayload->GetLength(), aPlType, aPlType) != 0 )
		 return EFalse;
	else return ETrue;
}

void CIkev2Payloads::ConstructL()
{
	iProps    = new (ELeave) CArrayFixFlat<TProposalIkev2 *>(4);
	iTrans    = new (ELeave) CArrayFixFlat<TTransformIkev2 *>(4);		
	iCerts    = new (ELeave) CArrayFixFlat<TCertPayloadIkev2 *>(4);
	iCertReqs = new (ELeave) CArrayFixFlat<TCReqPayloadIkev2 *>(2);
	iNotifs   = new (ELeave) CArrayFixFlat<TNotifPayloadIkev2 *>(2);	
	iDeletes  = new (ELeave) CArrayFixFlat<TDeletePlIkev2 *>(2);		
	iVids     = new (ELeave) CArrayFixFlat<TVendorPlIkev2 *>(2);
	iGenPlds  = new (ELeave) CArrayFixFlat<TPayloadIkev2 *>(2);
}


TInt CIkev2Payloads::ParsePayloadsL(TPayloadIkev2* aPayload, TInt aLength, TUint16 aPlType, TUint16 aRefPlType )
{
	ASSERT(aPayload);
	TBool  Critical;
	TInt   PlLth;
	TInt   RefLth;	
	
	while ( aPlType != IKEV2_PAYLOAD_NONE )  
	{
		PlLth  = aPayload->GetLength();
		RefLth = TPayloadIkev2::Size();		
		
		if ( ( aLength < TPayloadIkev2::Size() ) || (aLength < PlLth) )
		{
			if ( aLength && ( aRefPlType != IKEV2_PAYLOAD_SA )) 			
			   SetStatus(INVALID_SYNTAX); // Payload length mismatch !!
			return aLength;		   
		}
		
		if ( (aRefPlType != IKEV2_PAYLOAD_NONE) && (aRefPlType != aPlType) )
		{	
		   SetStatus(INVALID_SYNTAX); // Illegal payload type !!
		   return aLength;		   
	    }
		
		Critical = aPayload->GetCritical();

		switch ( aPlType )
		{
			case IKEV2_PAYLOAD_PROP:
				iProps->AppendL(TProposalIkev2::Cast(aPayload));
			    //
			    // Parse Transform payloads within a Proposal payload
				// (recursively)
				//
				ParsePayloadsL(TPayloadIkev2::Cast(TProposalIkev2::Cast(aPayload)->TransformPl()),
							  (PlLth - TProposalIkev2::Cast(aPayload)->PropHdrLth()),
							   IKEV2_PAYLOAD_TRANS, IKEV2_PAYLOAD_TRANS);
				if ( Status() )
					return aLength;
				break;

			case IKEV2_PAYLOAD_TRANS:
				RefLth = TTransformIkev2::Cast(aPayload)->Size();
				iTrans->AppendL(TTransformIkev2::Cast(aPayload));
				break;
			
			case IKEV2_PAYLOAD_SA: //also includes proposal and transform
				if ( !iSa ) // Only one SA payload (The first)
				{ 
				   iSa = aPayload;
				   //
				   // Parse Proposal payloads within a SA payload
				   // (recursively)
				   //
				   ParsePayloadsL(TPayloadIkev2::Cast(aPayload->PayloadData()), aPayload->PlDataLen(),
								  IKEV2_PAYLOAD_PROP, IKEV2_PAYLOAD_PROP);
				   if ( Status() )
					   return aLength;
				}   
				break;

			case IKEV2_PAYLOAD_KE:
				RefLth = TKEPayloadIkev2::Size();				
				if ( !iKe )	 // Only one KE payload (The first)
				{
				   iKe = TKEPayloadIkev2::Cast(aPayload);
				}   
				break;

			case IKEV2_PAYLOAD_ID_I:
				if ( !Encrypted() )
				{
				    SetStatus(INVALID_SYNTAX);  // ID payload MUST be encrypted
				    return aLength;
				}
				RefLth = TIDPayloadIkev2::Size();
				if ( !iIdI )	 // Only one Initiator ID payload (The first)
				{
				    iIdI = TIDPayloadIkev2::Cast(aPayload);
				}   
				break;

			case IKEV2_PAYLOAD_ID_R:
				if ( !Encrypted() )
				{
					SetStatus(INVALID_SYNTAX);  // ID payload MUST be encrypted
					return aLength;
				}
				RefLth = TIDPayloadIkev2::Size();
				if ( !iIdR )	 // Only one Responder ID payload (The first)
				{
				    iIdR = TIDPayloadIkev2::Cast(aPayload);
				}   
				break;

			case IKEV2_PAYLOAD_CERT:
				RefLth = TCertPayloadIkev2::Size();
				iCerts->AppendL(TCertPayloadIkev2::Cast(aPayload));
				break;
				
			case IKEV2_PAYLOAD_CR:
				RefLth = TCReqPayloadIkev2::Size();	
				iCertReqs->AppendL(TCReqPayloadIkev2::Cast(aPayload));
				break;

			case IKEV2_PAYLOAD_AUTH:
				if ( !Encrypted() )
				{
				    SetStatus(INVALID_SYNTAX);  // Auth payload MUST be encrypted
				    return aLength;
				}
				RefLth = TAuthPayloadIkev2::Size();
				if ( !iAuth )	 // Only one Authentication payload (The first)	
				{
					iAuth = TAuthPayloadIkev2::Cast(aPayload);
				}   
				break;

			case IKEV2_PAYLOAD_NONCE:
				if ( !iNonce )
				{ // Only one SA payload (The first)	
					iNonce = aPayload;
				}   
				break;

			case IKEV2_PAYLOAD_NOTIF:
				RefLth = TNotifPayloadIkev2::Size();
				iNotifs->AppendL(TNotifPayloadIkev2::Cast(aPayload));						
				break;

			case IKEV2_PAYLOAD_DELETE:
				RefLth = TDeletePlIkev2::Size();				
				iDeletes->AppendL(TDeletePlIkev2::Cast(aPayload));						
				break;
				
			case IKEV2_PAYLOAD_VID:
				iVids->AppendL(aPayload);
				break;

			case IKEV2_PAYLOAD_TS_I:
				if ( !Encrypted() )
				{
					SetStatus(INVALID_SYNTAX);  // ID payload MUST be encrypted
					return aLength;
				}
				RefLth = TTSPayloadIkev2::Size();
				if ( !iTsI )	 // Only one Initiator ID payload (The first)
				{
					iTsI = TTSPayloadIkev2::Cast(aPayload);
				}   
				break;

			case IKEV2_PAYLOAD_TS_R:
				if ( !Encrypted() )
				{
					SetStatus(INVALID_SYNTAX);  // ID payload MUST be encrypted
					return aLength;
				}
				RefLth = TTSPayloadIkev2::Size();
				if ( !iTsR )	 // Only one Initiator ID payload (The first)
				{
					iTsR = TTSPayloadIkev2::Cast(aPayload);
				}   
				break;

			case IKEV2_PAYLOAD_ENCR:
				if ( !iEncr )	 // Only one Initiator ID payload (The first)
				{
				   DecryptEncrPayloadL(aPayload);
				   iEncr = aPayload;
				}
				else SetStatus(INVALID_SYNTAX);  // Only ONE encrypted payload per message
				if ( Status() )
					return aLength;
				break;

			case IKEV2_PAYLOAD_CONFIG:
				if ( !Encrypted() )
				{
					SetStatus(INVALID_SYNTAX);  // ID payload MUST be encrypted
					return aLength;
				}
				RefLth = TCPPayloadIkev2::Size();				
				if ( !iCp )	 // Only one Config payload (The first)
				{
					iCp = TCPPayloadIkev2::Cast(aPayload);
				}
				break;

			case IKEV2_PAYLOAD_EAP:
				if ( !Encrypted() )
				{
					SetStatus(INVALID_SYNTAX);  // ID payload MUST be encrypted
					return aLength;
				}	
				if ( !iEap )	 // Only one Config payload (The first)
				{
					iEap = aPayload;
				}
				break;
				
			//
			// Unknown payload detected. If Critical bit is not set
			// 
			//
			default:
				if ( Critical )
				{	
				   SetStatus(UNSUPPORTED_CRITICAL_PAYLOAD);
				   return aLength;
				}		 
				else iGenPlds->AppendL(aPayload);
				break;
				
		}

		if ( PlLth < RefLth )
		{	
			SetStatus(INVALID_SYNTAX);  // Length mismatch
			return aLength;
		}
		
		aLength  -= PlLth;
		aPlType   = aPayload->GetNextPayload();
		aPayload  = aPayload->Next();
	}

	if ( aLength )
		SetStatus(INVALID_SYNTAX);  // Length mismatch
		
	return aLength;
}	

void CIkev2Payloads::DecryptEncrPayloadL(TPayloadIkev2* aPayload)
{
    ASSERT(aPayload);
	//
	//  Process Encrypted Payload
	//                      1                   2                   3
    //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  ! Next Payload  !C!  RESERVED   !         Payload Length        !
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  !                     Initialization Vector                     !
	//	!         (length is block size for encryption algorithm)       !
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	!                    Encrypted IKE Payloads                     !
	//	+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	!               !             Padding (0-255 octets)            !
	//	+-+-+-+-+-+-+-+-+                               +-+-+-+-+-+-+-+-+
	//	!                                               !  Pad Length   !
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//	~                    Integrity Checksum Data                    ~
	//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	//  Save integrity checksum from encrypted payload tail the
	//  integrity checksum length is defined by used integrity algorithm
	//
	TInt CheksumLth  = iIkeV2SaData.iIntChkSumLth;
	TInt CbcLth      = iIkeV2SaData.iCipherBlkLth;	
	TInt EmbeddedLth = aPayload->GetLength();
	if ( EmbeddedLth <  (CheksumLth + CbcLth) )
	{
		SetStatus(INVALID_SYNTAX); 
		return;
	}
	TUint8* MsgChecksum = (TUint8*)aPayload + EmbeddedLth - CheksumLth; 
	TBuf8<IKEV2_KEY_MATERIAL_SIZE> CheckSum;
	//
	//  Decrypt encryption payload content and payload tail the
	//  integrity checksum length is defined by used integrity algorithm
	//
	
    TInt EntireLth           = iIkeMsg->GetLength() - iIkeV2SaData.iIntChkSumLth;   
    TInt EncryptedDataLength = aPayload->GetLength() - (iIkeV2SaData.iIntChkSumLth + iIkeV2SaData.iCipherBlkLth + TPayloadIkev2::Size());
    TUint8* IvPtr            = aPayload->PayloadData();  
    TUint8* EncryptedData    = IvPtr + iIkeV2SaData.iCipherBlkLth;
            
    TPtrC8 entireDataPtr((TUint8*)iIkeMsg, EntireLth);
    if ( iIkeV2SaData.iInitiator )
    {   
       IkeCrypto::IntegHMACL(entireDataPtr, CheckSum, iIkeV2SaData.iSK_ar, iIkeV2SaData.iIntegAlg);        
       IkeCrypto::DecryptL(EncryptedData, EncryptedData, EncryptedDataLength, IvPtr, iIkeV2SaData.iSK_er, iIkeV2SaData.iEncrAlg);
    }
    else
    {
       IkeCrypto::IntegHMACL(entireDataPtr, CheckSum, iIkeV2SaData.iSK_ai, iIkeV2SaData.iIntegAlg);        
       IkeCrypto::DecryptL(EncryptedData, EncryptedData, EncryptedDataLength, IvPtr, iIkeV2SaData.iSK_ei, iIkeV2SaData.iEncrAlg);
    }   
	
	
	//
	//  Check that integrity cheksum is correct
	//
	if ( Mem::Compare(MsgChecksum, CheksumLth ,CheckSum.Ptr(), CheksumLth) != 0 )
	{
	   SetStatus(INVALID_SYNTAX);  // Length mismatch
       return;
	}
	//
	//  Process embedded payloads inside the encrypted payload
	//  - Bypass IV in the begin of encrypted payload data
	//  - Assure that padded "embedded" payloads have length which
	//    equals with multiple of CBC block length
	//  - Ignore padding bytes from that length 
	//
	EmbeddedLth -= (CheksumLth + CbcLth + TPayloadIkev2::Size());
	if ( EmbeddedLth % CbcLth )
	{
		SetStatus(INVALID_SYNTAX); 
		return;
	}
	MsgChecksum --;  // Move pointer to padding length
	if ( EmbeddedLth < (TInt)*MsgChecksum ) 
	{
		SetStatus(INVALID_SYNTAX); 
		return;
	}
	EmbeddedLth -= (TInt)*MsgChecksum;
	EmbeddedLth --;  // Pad Length itself
	TPayloadIkev2* EmbeddedPl = TPayloadIkev2::Cast(aPayload->PayloadData() + CbcLth);
	
	iEncrypted = ETrue;   // Set encrypted indicator
	
	ParsePayloadsL(EmbeddedPl, EmbeddedLth,
				   aPayload->GetNextPayload(), IKEV2_PAYLOAD_NONE);

}

ThdrISAKMP* CIkev2Payloads::GetIkeMsg() 
{
    return iIkeMsg;
}


TInt CIkev2Payloads::Status() 
{
    return iStatus;
}


void CIkev2Payloads::SetStatus(TInt aStatus) 
{ 
    if ( iStatus == 0) iStatus = aStatus;
}


TBool CIkev2Payloads::Encrypted() 
{
    return iEncrypted;
}


