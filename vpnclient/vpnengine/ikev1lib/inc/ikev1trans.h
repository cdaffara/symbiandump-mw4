/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: IKE transaction exchange implementation.
*
*/


#ifndef IKEV1TRANS_H
#define IKEV1TRANS_H

#include "ikev1dialog.h"
#include "ikev1payloads.h"

/*---------------------------------------------------------------------------
 *
 *  Transaction exchange return codes (returned by ExecuteL() method)
 *
 *---------------------------------------------------------------------------*/
#define TRANSACTION_SUCCESS   0
#define TRANSACTION_CONTINUE  0x1
#define TRANSACTION_IGNORE    0x2
#define TRANSACTION_FAILED    0x4

#define IV_LTH   20
/**--------------------------------------------------------------------------
 *
 * Structure TTransExchange
 * Contains parameter data related one ISAKMP Transaction exchange
 *
 *--------------------------------------------------------------------------*/
struct TTransExchange
{
    TUint32       iMessageId;
    TUint16       iIdentifier;  
    TUint16       iXauthType; 
    TInt          iRole;
    TBuf8<IV_LTH> iIV;
    TTransExchange()
        {
        iMessageId = 0;
        iIdentifier = 0;
        iXauthType = 0;
        iRole = 0;
        };
};


class CIkev1Negotiation;
class ThdrISAKMP;
class CAuthDialogInfo;
class CInternalAddress;
class TIkev1IsakmpStream;
class MIkeDebug;
class TInetAddr;

/**--------------------------------------------------------------------------
 *
 * CTransNegotiation class 
 * Handles ISAKMP transaction exchange messages
 * Both the ISAKMP Configuration Method <draft-dukes-ike-mode-cfg-01.txt> and
 * Extended Authentication within IKE (XAUTH) <draft-beaulieu-ike-xauth-02.txt>
 * has been implemented using ISAKMP transaction exchange messages.
 * This class handles both the transaction exchange message handling and
 * config mode/XAUTH protocol processing.  
 *
 *--------------------------------------------------------------------------*/
NONSHARABLE_CLASS(CTransNegotiation) : public CArrayFixFlat<TTransExchange *>, public MIkeDialogComplete
{
public:
    ~CTransNegotiation();           
    static CTransNegotiation* NewL( TBool aUseXauth,
                                    TBool aUseCfgMode,
                                    CIkev1PluginSession* aPluginSession,
                                    CIkev1Negotiation* aNegotiation,
                                    MIkeDebug& aDebug );              
    TInt ExecuteL( const ThdrISAKMP& aHdr,
                   const TInetAddr& aSrcAddr,
                   TInt aLocalPort );
    TInt ProcessUserResponseL(CAuthDialogInfo *aDialogInfo );   
    TInt TransactionFailedL(const TNotificationISAKMP *aNotifPayload);  
    TBool GetIV(TUint32 aMsgId, TDes8& aIV);
    TBool SetIV(TUint32 aMsgId, TDes8& aIV);    
    CInternalAddress* GetInternalAddr()
    {
        CInternalAddress* ia = iInternalAddr;
        iInternalAddr = NULL;
        return ia;
    }
    static TUint16 GetAuthMethod(TUint16 aAuthMethod, TBool aXauthUsed, TInt aRole);
    static void BuildXauthVendorId(TIkev1IsakmpStream &aMsg);

	TInt DialogCompleteL(CIkev1Dialog* aDialog, TAny* aUserInfo, HBufC8* aUsername,
	                     HBufC8* aSecret, HBufC8* aDomain);	
	TInt BuildConfigRequestL();
    
private:
    TTransExchange* FindExchange(TUint32 aMsgId);
    TTransExchange* AddExchangeL(TUint32 aMsgId, TUint8 aRole);
    TUint16 GetIdentifier()
    {
        iIdentifierBase --;
        if ( (iIdentifierBase & 0xffff) == 0 )
           iIdentifierBase --;  
        return (TUint16)iIdentifierBase;
    }
    TInt TransactionExchangeL(const ThdrISAKMP &aHdr);   
    TInt ProcessAttributesL(const TAttributeISAKMP *aAttr);
    TInt ProcessCfgModeAttrsL(TDataISAKMP* aAttr, TInt aLth);
    TInt ProcessXauthRequestL(TDataISAKMP* aAttr, TInt aLth);    
    TInt ProcessXauthStatusL(TDataISAKMP* aAttr, TInt aLth);
    TInt CheckTransactionStatusL(TInt aStatus); 
    void AddAttributeData(TDes8& aAttrBfr, TInt aType, TInt aLth, TUint8* aData);
    void BuildAndSendMessageL(TDesC8& aAttrBfr, TUint8 aMsgType); 
    CTransNegotiation( TInt aGranularity,
                       TBool aUseXauth,
                       TBool aUseCfgMode,
                       CIkev1PluginSession* aPluginSession,
                       CIkev1Negotiation* aNegotiation,
                       MIkeDebug& aDebug );
    void ConstructL(); 

private:
    CIkev1PluginSession* iPluginSession;
    CIkev1Negotiation* iNegotiation;
    CInternalAddress*  iInternalAddr;
    TTransExchange*    iCurrExchange;
    CIkev1Dialog*      iDialog;
    CAuthDialogInfo*   iDialogInfo;
	HBufC8*            iUserName;    // Saved for User name caching
	
    TBool   iUseXauth;
    TBool   iUseCfgMode;    
    TUint32 iIdentifierBase;
    TUint32 iRequestFlags;
    TBool   iXauthCompleted;
    TBool   iCfgModeCompleted;
	TBool   iUseOlderPIXXauth;     // use draft-ietf-ipsec-isakmp-xauth-04.txt  
                                   
    TLastIKEMsg iLastTransMsgInfo;    
    MIkeDebug& iDebug;
};


#endif // IKEV1TRANS_H
