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
#ifndef _IKEMSGREC_H_
#define _IKEMSGREC_H_
#include "ikev2payloads.h"

class TIkev2SAData;

NONSHARABLE_CLASS(CIkev2Payloads) : public CBase
    {
public:
    static CIkev2Payloads* NewL(const ThdrISAKMP &aHdr, const TIkev2SAData& aIkeV2SaData);
    static CIkev2Payloads* NewL(TPayloadIkev2* aPayload, TUint8 aPayloadType, TIkev2SAData& aIkeV2SaData);	    
    ~CIkev2Payloads();

    ThdrISAKMP* GetIkeMsg();				
    TInt Status();
    void SetStatus(TInt aStatus);		
    TBool Encrypted();
    TInt  ParsePayloadL(TPayloadIkev2* aPayload, TUint16 aPlType);
    
private:
    CIkev2Payloads(const TIkev2SAData& aIkeV2SaData);
    void  ConstructL();
    
    TInt  ParsePayloadsL(TPayloadIkev2* aPayload, TInt aLength, TUint16 aPlType, TUint16 aRefPlType );		
    void  DecryptEncrPayloadL(TPayloadIkev2* aPayload);		
    
public:
    TPayloadIkev2*      iSa;
    TKEPayloadIkev2*    iKe;
    TPayloadIkev2*      iNonce;
    TAuthPayloadIkev2*  iAuth;
    TIDPayloadIkev2*    iIdI;
    TIDPayloadIkev2*    iIdR;		
    TTSPayloadIkev2*    iTsI;
    TTSPayloadIkev2*    iTsR;
    TPayloadIkev2*      iEncr;   	
    TCPPayloadIkev2*    iCp;
    TPayloadIkev2*      iEap;

    CArrayFixFlat<TProposalIkev2*>*       iProps;
    CArrayFixFlat<TTransformIkev2*>*      iTrans; 				
    CArrayFixFlat<TCertPayloadIkev2*>*    iCerts;   	
    CArrayFixFlat<TCReqPayloadIkev2*>*    iCertReqs;
    CArrayFixFlat<TNotifPayloadIkev2*>*   iNotifs;
    CArrayFixFlat<TDeletePlIkev2*>*       iDeletes;		
    CArrayFixFlat<TVendorPlIkev2*>*       iVids;

    CArrayFixFlat<TPayloadIkev2*>*        iGenPlds;

private:
    const TIkev2SAData&  iIkeV2SaData;
    ThdrISAKMP*          iIkeMsg;
    TBool                iEncrypted;
    TInt                 iStatus;
	
    };
#endif
