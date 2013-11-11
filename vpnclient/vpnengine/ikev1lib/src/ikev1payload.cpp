/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIkev1payload class
*
*/

#include "ikev1payload.h"
#include "ikev1negotiation.h"
#include "ikedebug.h"

//
//  CIkePayloads
//

CIkev1Payloads::CIkev1Payloads( CIkev1Negotiation& aNegotiation,
                                MIkeDebug& aDebug )
 : iNegotiation( aNegotiation ),
   iDebug( aDebug )
{	
}	


CIkev1Payloads::~CIkev1Payloads()
{
	delete iIds;	
	delete iCerts;
	delete iCertReqs;
	delete iNotifs;
	delete iDeletes; 	
	delete iVids;
	delete iNatDs;
	delete iGenPlds;				
}


CIkev1Payloads* CIkev1Payloads::NewL( const ThdrISAKMP& aHdr,
                                      CIkev1Negotiation& aNegotiation,
                                      MIkeDebug& aDebug )
{
	CIkev1Payloads* Payloads = new (ELeave) CIkev1Payloads( aNegotiation,
	                                                        aDebug );
	CleanupStack::PushL(Payloads);	

	Payloads->iIds      = new (ELeave) CArrayFixFlat<const TIdentISAKMP *>(4);	
	Payloads->iCerts    = new (ELeave) CArrayFixFlat<const TCertificateISAKMP *>(4);
	Payloads->iCertReqs = new (ELeave) CArrayFixFlat<const TCertificateReqISAKMP *>(4);
	Payloads->iNotifs   = new (ELeave) CArrayFixFlat<const TNotificationISAKMP *>(4);
	Payloads->iDeletes  = new (ELeave) CArrayFixFlat<const TDeleteISAKMP *>(4);		
	Payloads->iVids     = new (ELeave) CArrayFixFlat<const TVendorISAKMP *>(4);
	Payloads->iNatDs    = new (ELeave) CArrayFixFlat<const TNATDISAKMP *>(4);
	Payloads->iGenPlds  = new (ELeave) CArrayFixFlat<const TPayloadISAKMP *>(4);

	if ( !Payloads->ParsePayloadsL(aHdr) )
	{
	   CleanupStack::PopAndDestroy();		
	   Payloads = NULL;
	}	 
	else CleanupStack::Pop();
	
	return Payloads;
}

TBool CIkev1Payloads::ParsePayloadsL(const ThdrISAKMP &aHdr)
{

	TBool   Status = ETrue;
	TUint16 ptype  = aHdr.GetPayload();
	const TPayloadISAKMP* payload = (const TPayloadISAKMP*)aHdr.Next();
	iPadding = aHdr.GetLength() - sizeof(aHdr);
	
	while ( ptype != ISAKMP_PAYLOAD_NONE )
	{
		
		if (!iNegotiation.CheckGenericPayloadL(payload))  //Checks the generic payload is OK
		{
			Status = EFalse;
			break;
		}

		switch (ptype)
		{
			
			case ISAKMP_PAYLOAD_SA: //also includes proposal and transform
				DEBUG_LOG(_L("[SA]"));
				if ( !iSa )	 // Only one SA payload (The first)	
				   iSa = TSAISAKMP::Ptr(payload);
				break;

			case ISAKMP_PAYLOAD_KE: 
				DEBUG_LOG(_L("[KE]"));
				if ( !iKe )	 // Only one KE payload (The first)	
				   iKe = TKeyISAKMP::Ptr(payload);
				break;

			case ISAKMP_PAYLOAD_ID: 
				DEBUG_LOG(_L("[ID]"));
				iIds->AppendL(TIdentISAKMP::Ptr(payload));
				break;

			case ISAKMP_PAYLOAD_CERT:
				DEBUG_LOG(_L("[CERT]"));
				iCerts->AppendL(TCertificateISAKMP::Ptr(payload));
				break;
				
			case ISAKMP_PAYLOAD_CR: 
				DEBUG_LOG(_L("[CR]"));
				iCertReqs->AppendL(TCertificateReqISAKMP::Ptr(payload));
				break;

			case ISAKMP_PAYLOAD_HASH:
				DEBUG_LOG(_L("[HASH]"));
				if ( !iHash )	 // Only one HASH payload (The first)	
				   iHash = THashISAKMP::Ptr(payload);
				break;

			case ISAKMP_PAYLOAD_SIG:
				DEBUG_LOG(_L("[SIG]"));
				if ( !iSign )	 // Only one HASH payload (The first)	
				   iSign = TSignatureISAKMP::Ptr(payload);
				break;

			case ISAKMP_PAYLOAD_NONCE:
				DEBUG_LOG(_L("[NONCE]"));
				if ( !iNonce )	 // Only one NONCE payload (The first)	
				   iNonce = TNonceISAKMP::Ptr(payload);
				break;

			case ISAKMP_PAYLOAD_NOTIF:
				DEBUG_LOG(_L("[NOTIF]"));
				iNotifs->AppendL(TNotificationISAKMP::Ptr(payload));						
				break;

			case ISAKMP_PAYLOAD_D:
				DEBUG_LOG(_L("[DELETE]"));
				iDeletes->AppendL(TDeleteISAKMP::Ptr(payload));						
				break;
				
			case ISAKMP_PAYLOAD_VID:
				DEBUG_LOG(_L("[VID]"));
				iVids->AppendL(TVendorISAKMP::Ptr(payload));
				break;
				
            //
			// Extensions payloads  
			//
			case ISAKMP_PAYLOAD_ATTRIBUTES:
				DEBUG_LOG(_L("[ATTR]"));
				if ( !iAttr )	 // Only one ATTR payload (The first)	
				   iAttr = TAttributeISAKMP::Ptr(payload);
				break;
			
			case ISAKMP_PAYLOAD_CHRE:
				DEBUG_LOG(_L("[CHRE]"));
				if ( !iChre )	 // Only one CHRE payload (The first)	
			       iChre = TCHREISAKMP::Ptr(payload);
                break;

			case ISAKMP_INT_NETWORK:
				DEBUG_LOG(_L("[IA]"));
				if ( !iIaddr )	 // Only one IA payload (The first)	
				   iIaddr = TINTNETISAKMP::Ptr(payload);
				break;  		
				
			case IETF_NAT_DISCOVERY:
				DEBUG_LOG(_L("[NAT-D]"));
				iNatDs->AppendL(TNATDISAKMP::Ptr(payload));
				break;
				
			case IETF_RFC_NAT_DISCOVERY:
				DEBUG_LOG(_L("[NAT-D]"));
				iNatDs->AppendL(TNATDISAKMP::Ptr(payload));
				break;	

			case IETF_NAT_ORIG_ADDR:
				DEBUG_LOG(_L("[NAT-OA]"));
				if ( !iNatOa )	 // Only one NONCE payload (The first)	
				   iNatOa = TNATOaISAKMP::Ptr(payload);
				break;
			
			case IETF_RFC_NAT_ORIG_ADDR:
				DEBUG_LOG(_L("[NAT-OA]"));
				if ( !iNatOa )	 // Only one NONCE payload (The first)	
				   iNatOa = TNATOaISAKMP::Ptr(payload);
				break;
			//
			// Unknown payloads are queued into iGenPlds array
			//
			default: 
				DEBUG_LOG1(_L("[PL TYPE (%d)]"),ptype);
				iGenPlds->AppendL(payload);												
				break;
				
		}

		iPadding -= payload->GetLength(); 
		ptype     = payload->GetPayload();
		payload   = payload->Next();
	}

	return Status;
}	

