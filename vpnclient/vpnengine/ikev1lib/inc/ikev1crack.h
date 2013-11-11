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
* Description:   IKEv1 Crack authentication
*
*/

#ifndef IKEV1CRACK_H
#define IKEV1CRACK_H

#include <e32base.h>
#include "ikev1dialog.h"

/*---------------------------------------------------------------------------
 *
 *  CRACK return codes for public methods
 *
 *---------------------------------------------------------------------------*/
#define CRACK_SUCCESS       0
#define CRACK_CONTINUE      0x1
#define CRACK_IGNORE_MSG    0x2
#define CRACK_FAILED        0x4

/*---------------------------------------------------------------------------
 *
 *  CRACK iState flags                          
 *
 *---------------------------------------------------------------------------*/
#define WAITING_USER_RSP      0x1
#define WAITING_PEER_RSP      0x2
#define CHALLENGE_RECEIVED    0x4
#define CRACK_AUTHENTICATED   0x8 
#define SHOW_ERROR_DIALOG     0x10
#define SECURID_NEXT_PIN_MODE 0x20

/*---------------------------------------------------------------------------
 *
 *  CAuthDialogInfo class
 *
 *---------------------------------------------------------------------------*/
#define DIALOG_INFO_ID        0xfedcba98
#define XAUTH_DIALOG_ID       0x76543210

class CIkev1Negotiation;
class TNotificationISAKMP;
class TCHREISAKMP;
class ThdrISAKMP;
class CIkev1PluginSession;
class MIkeDebug;

class CAuthDialogInfo : public CBase
    {
public:
    CAuthDialogInfo(CIkev1PluginSession* aPluginSession, TUint32 aObjId, TUint32 aSAId, TUint32 aMsgId)
                    { iPluginSession = aPluginSession; iSAId = aSAId; iObjId  = aObjId; iMsgId  = aMsgId; }
    ~CAuthDialogInfo() {iObjId = 0;}    
    inline CIkev1PluginSession* PluginSession() { return iPluginSession;}
    inline TUint32     SAId() { return iSAId;}  
    inline TUint32     GetObjId() { return iObjId;}
    inline TUint32     GetMsgId() { return iMsgId;}     
    
private:
    TUint32    iObjId;      // Object identifier 
    CIkev1PluginSession* iPluginSession;  // Plugin session pointer  
    TUint32    iSAId;       // SA id of CIkev1Negotiation
    TUint32    iMsgId;      // Transaction exchange message ID
    
public: 
//  Credentials data get from user with asynchronous dialog 
	HBufC8     *iUsername;     
    HBufC8     *iSecret;
    HBufC8     *iDomain;    
    };


NONSHARABLE_CLASS(CIKECRACKNegotiation) : public CBase, public MIkeDialogComplete
{
public:
    CIKECRACKNegotiation( MIkeDebug& aDebug );
    ~CIKECRACKNegotiation();    
    TInt   ConstructL(TInt aLAMType, CIkev1Negotiation *aNegotiation, const TDesC &aDomain);
    TInt   ExecuteCRACKMsgL(const ThdrISAKMP &aHdr);
    TInt   ProcessUserResponseL(CAuthDialogInfo *aUserInfo);
    TInt   CrackAuthenticationFailedL(const TNotificationISAKMP *aNotifPayload);
	
	TInt   DialogCompleteL(CIkev1Dialog* /*aDialog*/, TAny* aUserInfo, HBufC8* aUsername, HBufC8* aSecret, HBufC8* aDomain);
	
    
private:
    TInt    GetDataL(HBufC8* aChallenge);
    TInt    GetDatafromUserL(HBufC8 *aChallenge);
    TInt    GetUNPWDFromPolicyL();
    TInt    GetUNPWDFromNegotiationL();
    TInt    ProcessCHREAttibutesL(const TCHREISAKMP *aCHRE_PAYLOAD);
    void    SendCredentialsL(TUint16 aAttr1, TUint16 aAttr2, TUint16 aAttr3,
                             HBufC8* aBfr1, HBufC8* aBfr2, HBufC8* aBfr3);

private:
    TInt       iLAMType;    // Legacy Authentication Method type code
    TInt       iState;      // CRACK negotiation state
    TInt       iMsgCount;   // CRACK message count xmitted (does not contain retries)

    CIkev1PluginSession*      iPluginSession;      // IKEv1 plugin session 
    CIkev1Negotiation* iNegotiation; // Current negotiation object (related to CRACK)
    CIkev1Dialog*      iDialog;      // Pending dialog object
    CAuthDialogInfo*   iDialogInfo;  // Dialog info object

	HBufC8           *iUserName;    // Saved for User name caching 	
	HBufC8           *iDomain;      // Fixed domain value for this CRACK negotiation
	
	MIkeDebug& iDebug;
};


#endif // IKEV1CRACK_H
