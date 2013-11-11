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

#ifndef IKEV1PAYLOAD_H
#define IKEV1PAYLOAD_H

#include <e32base.h>


class ThdrISAKMP;
class CIkev1Negotiation;
class TSAISAKMP;
class TKeyISAKMP;
class TNonceISAKMP;
class THashISAKMP;
class TSignatureISAKMP;
class TNATOaISAKMP;
class TCHREISAKMP;
class TINTNETISAKMP;
class TAttributeISAKMP;
class TIdentISAKMP;
class TCertificateISAKMP;
class TCertificateReqISAKMP;
class TNotificationISAKMP;
class TDeleteISAKMP;
class TVendorISAKMP;
class TNATDISAKMP;
class TPayloadISAKMP;
class MIkeDebug;

//
// Processing incoming  message
//
NONSHARABLE_CLASS(CIkev1Payloads) : public CBase
    {
public:
	~CIkev1Payloads();
	static CIkev1Payloads* NewL( const ThdrISAKMP &aHdr,
	                             CIkev1Negotiation& aNegotiation,
	                             MIkeDebug& aDebug );
	TBool ParsePayloadsL(const ThdrISAKMP &aHdr);		

	const TSAISAKMP*        iSa;
	const TKeyISAKMP*       iKe;
	const TNonceISAKMP*     iNonce;
	const THashISAKMP*      iHash;
	const TSignatureISAKMP* iSign;

	const TNATOaISAKMP*     iNatOa;
	const TCHREISAKMP*      iChre;
	const TINTNETISAKMP*    iIaddr;
	const TAttributeISAKMP* iAttr;

	CArrayFixFlat<const TIdentISAKMP*>*          iIds;   		
	CArrayFixFlat<const TCertificateISAKMP*>*    iCerts;   	
	CArrayFixFlat<const TCertificateReqISAKMP*>* iCertReqs;
	CArrayFixFlat<const TNotificationISAKMP*>*   iNotifs;
	CArrayFixFlat<const TDeleteISAKMP*>*         iDeletes;		
	CArrayFixFlat<const TVendorISAKMP*>*         iVids;
	CArrayFixFlat<const TNATDISAKMP*>*           iNatDs;

	CArrayFixFlat<const TPayloadISAKMP*>*        iGenPlds;

	TUint                                        iPadding;

private:
    CIkev1Payloads( CIkev1Negotiation& aNegotiation,
                    MIkeDebug& aDebug );

    CIkev1Negotiation& iNegotiation;		
    MIkeDebug& iDebug;
    };

#endif //IKEV1PAYLOAD_H
