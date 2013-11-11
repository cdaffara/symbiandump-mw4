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
* Description: IKE transaction exchange implementation.
*
*/

/**-------------------------------------------------------------------
 *
 * Class CTransNegotiation
 * This class is used to handle ISAKMP Transaction exchange messages.
 * Transaction exchange has been defined in the IETF draft which specifies
 * The ISAKMP Configuration Method <draft-dukes-ike-mode-cfg-01.txt>.
 * This same two message configuration transaction is also used in IETF draft
 * Extended Authentication within IKE (XAUTH) <draft-beaulieu-ike-xauth-02.txt>.
 * CTransNegotiation class implements these IETF drafts, too.
 *
 *--------------------------------------------------------------------*/

#include "ikev1trans.h"
#include "ikedebug.h"
#include "ikev1pluginsession.h"
#include "ikev1negotiation.h"
#include "ikev1payload.h"
#include "ikev1timeout.h"
#include "ikev1crack.h"
#include "ikev1isakmpstream.h"
#include "ikev1crypto.h"

const TUint8  XAUTH_VID_DATA[8] = {0x09, 0x00, 0x26, 0x89, 0xdf, 0xd6, 0xb7, 0x12};
const TUint8  CISCO_UNITY_VID_DATA[16] = {0x12, 0xf5, 0xf2, 0x8c, 0x45, 0x71, 0x68, 0xa9,
                                          0x70, 0x2d, 0x9f, 0xe2, 0x74, 0xcc, 0x01, 0x00};
                                          
                                          
CTransNegotiation::CTransNegotiation( TInt aGranularity,
                                      TBool aUseXauth,
                                      TBool aUseCfgMode, 
                                      CIkev1PluginSession* aPluginSession,
                                      CIkev1Negotiation* aNegotiation,
                                      MIkeDebug& aDebug ) 
    :CArrayFixFlat<TTransExchange*>(aGranularity),
     iPluginSession(aPluginSession),
     iNegotiation(aNegotiation),
     iUseXauth(aUseXauth),
     iUseCfgMode(aUseCfgMode),
     iDebug(aDebug)
{
}
                                          
                                          
/**-------------------------------------------------------------------
 *
 * Method New()
 * Creates an instance of CTransNegotiation class if either 
 * usage of XAUTH or CFG-MODE has been requested.
 *
 *--------------------------------------------------------------------*/
CTransNegotiation* CTransNegotiation::NewL(TBool aUseXauth, TBool aUseCfgMode,
                                           CIkev1PluginSession* aPluginSession, 
                                           CIkev1Negotiation* aNegotiation,
                                           MIkeDebug& aDebug )
{    
    CTransNegotiation* Neg = new (ELeave) CTransNegotiation( 1,
                                                             aUseXauth,
                                                             aUseCfgMode,
                                                             aPluginSession,
                                                             aNegotiation,
                                                             aDebug );
    CleanupStack::PushL(Neg);
    Neg->ConstructL();
    CleanupStack::Pop(Neg);
    return Neg;        
}
/**-------------------------------------------------------------------
 *
 * Deconstruct method
 *
 *--------------------------------------------------------------------*/
CTransNegotiation::~CTransNegotiation()
{
    DEBUG_LOG(_L("Transaction exchange object deleted"));  
        
    delete iInternalAddr;
    delete iDialog;
    delete iDialogInfo;
	delete iUserName; 	
            
    for ( TInt i = 0; i < Count(); i++ )
    {
        delete At(i);
    }
}

/**-------------------------------------------------------------------
 *
 * Method ConstructL()
 * -- Links CKmdEngine- and CNegotiation object pointers to CTransNegotiation
 * -- If only CONFIG-MODE requested, start corresponding transaction exchange.
 *
 *--------------------------------------------------------------------*/
void CTransNegotiation::ConstructL()
{
    if ( !iPluginSession || !iNegotiation || (!iUseXauth && !iUseCfgMode)) 
    {
        User::Leave(KErrArgument);   
    }
       
    DEBUG_LOG(_L("Transaction exchange object constructed"));  
    if ( !iUseXauth ) 
    {
       iXauthCompleted = ETrue;
       iNegotiation->iTimer->Cancel();  // Stop retransmission timer   
    }
    else 
    {
       if ( !iUseCfgMode ) 
           iCfgModeCompleted = ETrue;
       DEBUG_LOG(_L("Starting to Wait XAUTH request"));  
    }    
}

/**-------------------------------------------------------------------
 *
 * Method GetAuthMethod()
 * This static method converts the authentication method value from
 * "normal" IKE attribute value (specified in RFC2409) to the attribute
 * value indicate XAUTH usage after IKE phase 1. This conversion is done
 * into the opposite direction when call parameter (aAuthMethod) have
 * already value indicating Xauth usage.
 *
 *--------------------------------------------------------------------*/
TUint16 CTransNegotiation::GetAuthMethod(TUint16 aAuthMethod, TBool aXauthUsed, TInt aRole)
{
    if ( aXauthUsed ) {
       if ( aAuthMethod >= XAUTHInitPreShared && aAuthMethod <= XAUTHRespRSARevisedEncr) {
          aAuthMethod -= XAUTHMethodBase;
          aAuthMethod = (TUint16)((aAuthMethod >> XAUTHScaler) | XAUTHScaler);
       }
       else {
          if ( aAuthMethod >= PRE_SHARED && aAuthMethod <= RSA_REV_ENCR ) {
             aAuthMethod = (TUint16)((aAuthMethod << XAUTHScaler) + XAUTHMethodBase);
             if ( aRole == INITIATOR )
                aAuthMethod -= XAUTHScaler;
          }
       }       
    }
    return aAuthMethod;
}   

/**-------------------------------------------------------------------
 *
 * Method BuildXauthVendorId()
 * This method builds a XAUTH related Vendor ID payload and adds it into 
 * the IKE message. The vendor id is specified in the draft
 * <draft-beaulieu-ike-xauth-02.txt> and its content is the following:
 * ["0x09002689DFD6B712"])
 * Both ISAKMP mode-cfg and extended authentication (XAUTH) can be
 * implemented in some VPN SGWs according to the older mode-cfg and
 * xauth drafts:
 * <draft-ietf-ipsec-isakmp-mode-cfg-04.txt> and
 * <draft-ietf-ipsec-isakmp-xauth-04.txt>
 *
 *--------------------------------------------------------------------*/
void CTransNegotiation::BuildXauthVendorId(TIkev1IsakmpStream &aMsg)
{
    TInetAddr DummyAddr;
    
    aMsg.IsakmpVendorId(IETF_NATT_VENDOR_ID,
                        NULL, NULL, DummyAddr, // These parameters has no relevance with IETF_NATT_VID_DATA
                        (TUint8*)XAUTH_VID_DATA, sizeof(XAUTH_VID_DATA));
	
	aMsg.IsakmpVendorId(IETF_NATT_VENDOR_ID,
						NULL, NULL, DummyAddr, // These parameters has no relevance with IETF_NATT_VID_DATA
						(TUint8*)CISCO_UNITY_VID_DATA,
						sizeof(CISCO_UNITY_VID_DATA));
}

/**-------------------------------------------------------------------
 *
 * Method GetIV()
 * Get IV for transaction exchange specified with message id parameter:
 * Find corresponding exchange structure and copy IV to caller
 * If no exchange found, return EFALSE status to indicate error.    
 *
 *--------------------------------------------------------------------*/
TBool CTransNegotiation::GetIV(TUint32 aMsgId, TDes8& aIV)
{
    TBool status = ETrue;
    TTransExchange *exchange = FindExchange(aMsgId);
    if ( exchange )
         aIV.Copy(exchange->iIV);
    else status = EFalse;

    return status;
}   

/**-------------------------------------------------------------------
 *
 * Method SetIV()
 * Set IV for transaction exchange specified with message id parameter:
 * Find corresponding exchange structure and store specified IV to
 * exchange structure
 * If no exchange found, return EFALSE status to indicate error.    
 *
 *--------------------------------------------------------------------*/
TBool CTransNegotiation::SetIV(TUint32 aMsgId, TDes8& aIV)
{
    TBool status = ETrue;
    TTransExchange *exchange = FindExchange(aMsgId);
    if ( exchange )
         exchange->iIV.Copy(aIV);
    else status = EFalse;

    return status;
}   

/**-------------------------------------------------------------------
 *
 * Method ProcessUserResponseL()
 * ProcessUserResponseL() builds a XAUTH reply message from authentication
 * credentials linked into the current CAuthDialogInfo object.
 *
 *--------------------------------------------------------------------*/
TInt CTransNegotiation::ProcessUserResponseL(CAuthDialogInfo *aDialogInfo )
{
    //
    // Find a transaction exchange structure for current message 
    //
    TInt lth = 0;
    iCurrExchange = FindExchange(aDialogInfo->GetMsgId());
    
    if ( iCurrExchange && iRequestFlags ) {
       //
       // Allocate a buffer for Attribute payload.
       // Calculate first required buffer length
       //
       if ( aDialogInfo->iUsername )    
          lth += (aDialogInfo->iUsername->Length() + 4);
       if ( aDialogInfo->iSecret )  
          lth += (aDialogInfo->iSecret->Length() + 4);
       
       HBufC8 *attributes = HBufC8::NewL(lth + 4);
       CleanupStack::PushL(attributes);
       TPtr8 attr_ptr(attributes->Des());
	   TUint16 AttrType;
       
       if ( iRequestFlags & (1 << (ATTR_PASSWORD - ATTR_XAUTH_TYPE)) ) {
          //
          // Add Xauth type attribute. Value is taken from current exchange structure
          //
		  if ( iUseOlderPIXXauth )
			   AttrType = ATTR_PIX_XAUTH_TYPE;
          else AttrType = ATTR_XAUTH_TYPE;						 
          AddAttributeData(attr_ptr, AttrType, 2, (TUint8*)&iCurrExchange->iXauthType);
       }
       
       if ( aDialogInfo->iUsername ) {
          //
          // Add user name attribute. 
          //
	      if ( iUseOlderPIXXauth )
			   AttrType = ATTR_PIX_USER_NAME;
		  else AttrType = ATTR_USER_NAME;						 
		  
          AddAttributeData(attr_ptr, AttrType, aDialogInfo->iUsername->Length(),
                           (TUint8*)aDialogInfo->iUsername->Ptr());
          //
	      // Take a copy of user name buffer in dialog info. This user name
	      // is cached into user name file if current CRACK negotiation is
	      // succeeded
	      //
		  delete iUserName; // Delete old user name buffer for sure
  	      iUserName = HBufC8::New(aDialogInfo->iUsername->Length() + 16); // 16 bytes space for padding
		  if ( iUserName ) {
		     iUserName->Des().Copy(aDialogInfo->iUsername->Des()); 
		  } 	   
       }

       if ( aDialogInfo->iSecret ) {
          //
          // Add either password, passcode or next pin attribute.
          // Check from iRequestFlags which one was requested by the gateway
          //
	      if ( iUseOlderPIXXauth )
			   AttrType = ATTR_PIX_PASSWORD; // default;
		  else AttrType = ATTR_PASSWORD; // default

          switch ( iRequestFlags ) {

              case (1 << (ATTR_PASSCODE - ATTR_XAUTH_TYPE)):
				  if ( iUseOlderPIXXauth )
					   AttrType = ATTR_PIX_PASSCODE;
				  else AttrType = ATTR_PASSCODE;
                  break;

              case (1 << (ATTR_NEXT_PIN - ATTR_XAUTH_TYPE)):
                  AttrType = ATTR_NEXT_PIN;
                  break;

              default:
                  break;
                   
          }   
          AddAttributeData(attr_ptr, AttrType, aDialogInfo->iSecret->Length(),
                          (TUint8*)aDialogInfo->iSecret->Ptr());           
       }
       
       BuildAndSendMessageL(attr_ptr, ISAKMP_CFG_REPLY);
       
       CleanupStack::PopAndDestroy(); //attributes 
       
       iRequestFlags = 0;

    }
    
    delete iDialog;  // delete dialog object
    delete aDialogInfo;  // release dialog info object  
    iDialog = NULL;
    iDialogInfo = NULL;  
     
    return TRANSACTION_CONTINUE;
}   

/**-------------------------------------------------------------------
 *
 * Method TransactionFailedL()
 * TransactionFailedL() is called when a notificatio/delete payload 
 * has been received in the middle of a transaction exchange.
 *
 *--------------------------------------------------------------------*/
TInt CTransNegotiation::TransactionFailedL(const TNotificationISAKMP *aNotifPayload)
{

    (void)aNotifPayload;
    iNegotiation->iTimer->Cancel();   //Cancel timer because authentication failed
    DEBUG_LOG(_L("Transaction exchange stopped by the gateway!"));
    // 
    // Dialog object shall be delete in Dialog->RunL when dialog completed
    //
    CIkev1Dialog* Dialog  = CIkev1Dialog::NewL(iPluginSession, iPluginSession->DialogAnchor(), iDebug);
    Dialog->ShowErrorDialogL(TVpnNoteDialog::EKmdAuthenticationFailed, NULL, NULL);
    
    return TRANSACTION_FAILED;
}

/**-------------------------------------------------------------------
 *
 * Method ExecuteL()
 * Processes a received ISAKMP transaction exchange message.
 * The received message MUST be an encrypted transaction exchange message
 * otherwise it is silently discarded.
 * Current TTransExchange structure is found, IV value calculated and
 * ISAKMP message decrypted.
 * TransactionExchangeL() method returns to the caller the following status codes:
 * (Corresponding CRACK status codes  defined in ike_crack.h)
 * -- TRANSACTION_SUCCESS (0) =
 *    Transaction exchange(s) has been succesfully completed.
 *    Normal operation can continue and CTransNegotiation object can be deleted.
 * -- TRANSACTION_CONTINUE (1) =
 *    Received message succesfully processed.
 *    Transaction exchange(s) shall still continue. 
 * -- TRANSACTION_IGNORE   (2) =
 *    Received message ignored. Transaction exchange(s) shall still continue. 
 * -- TRANSACTION_FAILED   (4) =
 *    Transaction exchange(s) has been failed (either CONFIG-MODE or XAUTH).
 *    Current CNegotiation object as well as CTransNegotiation object can
 *    be deleted. (= corresponding ISAKMP phase 1 negotiation shall be deleted).
 *
 *--------------------------------------------------------------------*/
#ifdef _DEBUG
TInt CTransNegotiation::ExecuteL( const ThdrISAKMP& aHdr,
                                  const TInetAddr& aSrcAddr,
                                  TInt aLocalPort )
#else
TInt CTransNegotiation::ExecuteL( const ThdrISAKMP& aHdr,
                                  const TInetAddr& /*aSrcAddr*/,
                                  TInt /*aLocalPort*/ )
#endif
{
    DEBUG_LOG(_L("Received message (encr)."));
    
    TLastIKEMsg msg_info(aHdr); //For retransmitted IKE msg detection
    if ( iLastTransMsgInfo.IsReTransmit(msg_info) ) {
       DEBUG_LOG(_L("Retransmitted Transaction message received, silently discarded !"));
       return TRANSACTION_IGNORE;              
    }
    TUint32 status = TRANSACTION_IGNORE; // default
    TUint32 msg_id;
    TBuf8<IV_LTH> tmp_IV;   //Temporal IV. Used to update the real one if the msg OK    
    const ThdrISAKMP *hdr = NULL;
    TUint8 *msg = NULL;
    msg_id = aHdr.GetMessageId();   //Saves the ID to compute IV and hash
    
    if (aHdr.GetFlags() & ISAKMP_HDR_EFLAG) //if encrypted
    {
        msg = new (ELeave) TUint8[aHdr.GetLength()];    //to place the new msg
		CleanupStack::PushL(msg);
		
        Mem::Copy(msg, (TUint8 *)&aHdr, sizeof(aHdr));  //The header is not encrypted

#ifdef _DEBUG        
        DEBUG_LOG(_L("Message ID recv:"));        
        TUint32 swap_id = ByteOrder::Swap32(msg_id);
        DEBUG_LOG_ARRAY((TUint8 *)&swap_id, sizeof(msg_id));
        DEBUG_LOG(_L("Transaction IV:"));
#endif // _DEBUG        
        //
        // Find a transaction exchange structure for current message 
        //
        iCurrExchange = FindExchange(msg_id);
        if ( !iCurrExchange )
           iCurrExchange = AddExchangeL(msg_id, RESPONDER); // Add a new transaction exchange
        //
        // Adjust IV value for transaction exchange.
        // There is now two situations:
        // 1) There already exists an IV in exchange structure
        //    (received message is a reply for an earlier sent request)
        // 2) There is no IV in exchange structure
        //    (received message is a new request/set message from peer)
        //    A new IV is built from CNegotiation.iLastIV and current message ID
        //
        if ( iCurrExchange->iIV.Length() == 0 ) {
           iCurrExchange->iIV.Copy(iNegotiation->iLastIV);          
           iNegotiation->ComputeIVL(iCurrExchange->iIV, msg_id);        
        }
        tmp_IV.Copy(iCurrExchange->iIV); // Make a copy of current IV                       

        DEBUG_LOG(_L("Decrypting..."));

        DecryptL((TUint8 *)aHdr.Next(),&msg[sizeof(aHdr)], (aHdr.GetLength()-sizeof(aHdr)),
                 iCurrExchange->iIV, iNegotiation->iSKEYID_e,
                 iNegotiation->iChosenProposal_I.iAttrList->iEncrAlg);
        hdr = (ThdrISAKMP *)msg;  //decrypted msg

#ifdef _DEBUG   
        const TPtrC8 ikeMsgPtr( (TUint8*)hdr,(TUint16)hdr->GetLength() );
        TInetAddr dstAddr;
        iPluginSession->GetLocalAddress( dstAddr );
        dstAddr.SetPort( aLocalPort );
        TRACE_MSG_IKEV1( ikeMsgPtr, aSrcAddr, dstAddr );                        
#endif // _DEBUG                        
        
        status = TransactionExchangeL(*hdr);

        if ( status == TRANSACTION_IGNORE ) {
           //
           // Current message ignored, restore saved IV to exchange structure
           //   
           iCurrExchange->iIV.Copy(tmp_IV);
        }
    }
    else
        hdr = &aHdr;

    if (msg)    //If used erase it (when encryption)
        CleanupStack::PopAndDestroy();
	
    if ( status == TRANSACTION_CONTINUE )
       msg_info.Store(iLastTransMsgInfo); // store new last received IKE message info
    
    return status;
}

/**-------------------------------------------------------------------
 *
 * Method TransactionExchangeL()
 * The ISAKMP transaction exchange message MUST be the following format:
 * HDR*, HASH, ATTR
 * Where the HASH payload contains the prf output, using SKEYID_a as
 * the key, and the M-ID (ISAKMP header Message ID) unique to this
 * exchange concatenated with all of the payloads after the HASH
 * payload. In other words, the hash for the above exchange is:
 * HASH = prf( SKEYID_a, M-ID | ATTR )
 * Multiple ATTR payloads MAY NOT be present in the Transaction Exchange.
 *
 *--------------------------------------------------------------------*/
TInt CTransNegotiation::TransactionExchangeL(const ThdrISAKMP &aHdr)
{
    TUint32 status;
	iNegotiation->iLengthLeft = aHdr.GetLength(); //Used to check the size in the payload are OK
	
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *iNegotiation, iDebug);
	if (!payload)
	{	
		return TRANSACTION_FAILED;    
	}
	CleanupStack::PushL(payload);

	if ( payload->iHash && payload->iAttr )
	{
	   // 
	   // Check if the hash value is OK. 
	   // 
	   if (!iNegotiation->VerifyInformationalHashL(payload->iHash, payload->iAttr,
		                                           iCurrExchange->iMessageId))
	   {	   
	      DEBUG_LOG(_L("AUTHENTICATION_FAILED (Transaction hash)"));
		  CleanupStack::PopAndDestroy();  //payload					  
		  return TRANSACTION_FAILED;                    
   	   }
	   status = ProcessAttributesL(payload->iAttr);
	   CleanupStack::PopAndDestroy();  //payload
	   return status;
	}
	CleanupStack::PopAndDestroy();  //payload    	
   	DEBUG_LOG(_L("Erroneous Transaction Exchange message received"));		
	return TRANSACTION_FAILED;    
}

/**-------------------------------------------------------------------
 *
 * Method ProcessAttributesL()
 * ProcessAttributesL() method parses the data attributes in received
 * attribute payload. If the iRole data member of current exchange structure
 * contains value INITIATOR, attribute payload is a CONFIG-MODE Reply
 * which should contain CONFIG-MODE attributes.
 * If the iRole data member of current exchange structure
 * contains value RESPONDER, attribute payload is either a XAUTH Request or Set.
 * These primitives should contain XAUTH attributes.
 *
 *--------------------------------------------------------------------*/
TInt CTransNegotiation::ProcessAttributesL(const TAttributeISAKMP *aAttr)
{
    TInt length = (TInt)aAttr->GetLength();
    if ( STATIC_CAST(TUint, length) < sizeof(TAttributeISAKMP) ) {
       return TRANSACTION_FAILED; 
    }

    TInt status;
    TUint8  cfg_msg_type = aAttr->CfgMsgType(); 
    TUint16 identifier   = aAttr->Identifier();
    
    if ( iCurrExchange->iRole == INITIATOR ) {
       //
       // Config mode transaction. The current message should be a reply.
       // Identifier value must also match to value in current exchange structure.
       //
       if ( cfg_msg_type != ISAKMP_CFG_REPLY ) {
//          ||
//          ( iCurrExchange->iIdentifier != identifier ) ) { 
          return TRANSACTION_FAILED; 
       }       
       status = ProcessCfgModeAttrsL(aAttr->AttrData(), aAttr->AttrDataLen());
    }
    else {
       //
       // XAUTH mode transaction. The current message should be either request
       // or set.
       //
       if ( (cfg_msg_type != ISAKMP_CFG_REQUEST) && (cfg_msg_type != ISAKMP_CFG_SET) ) {
          return TRANSACTION_FAILED; 
       }
       iCurrExchange->iIdentifier = identifier;
       if ( cfg_msg_type == ISAKMP_CFG_REQUEST ) 
            status = ProcessXauthRequestL(aAttr->AttrData(), aAttr->AttrDataLen());
       else status = ProcessXauthStatusL(aAttr->AttrData(), aAttr->AttrDataLen());    
    }

    return CheckTransactionStatusL(status);
    
}   

/**-------------------------------------------------------------------
 *
 * Method ProcessCfgModeAttrs()
 * ProcessCfgModeAttrs parses  CONFIG-MODE reply message attributes 
 * received from gateway. In this phase the following attributes are used:
 * -- INTERNAL_IP4_ADDRESS  = Client virtual IPv4 address in secure network
 * -- INTERNAL_IP6_ADDRESS  = Client virtual IPv6 address in secure network
 * -- INTERNAL_IP4_DNS      = DNS address(es) in secure network
 *
 * All other attributes are silently discarded
 *
 *--------------------------------------------------------------------*/
TInt CTransNegotiation::ProcessCfgModeAttrsL(TDataISAKMP* aAttr, TInt aLth)
{
    
    TBool   ia_received = EFalse;
    TUint32  ipv4_addr; 
    TIp6Addr ipv6_addr;    //IPV6 raw address
    TInetAddr *dns_addr;
    
    delete iInternalAddr;  // delete old CInternalAddress for sure
    iInternalAddr = NULL;
    CInternalAddress *InternalAddr = new (ELeave)CInternalAddress(1);   
    CleanupStack::PushL(InternalAddr);
    
    while ( aLth > 0 ) {
        
        aLth = aLth - aAttr->Size();
        if ( aLth < 0 ) {
           DEBUG_LOG(_L("CONFIG-MODE REPLY ERROR (Length mismatch in the attibutes)"));
           CleanupStack::PopAndDestroy(); // InternalAddr
           return TRANSACTION_FAILED;
        }
        switch ( aAttr->Type() ) {

           case ATTR_INTERNAL_IP4_ADDR:
                //
                // A Virtual IPv4 address received.
                // Store value to CInternalAddress object
                // 
                if ( !aAttr->IsBasic() && (aAttr->Length() == 4) ) {
                   if ( !ia_received ) {
                      ia_received = ETrue;                    
                      ipv4_addr = GET32(aAttr->VarValue()); 
                      InternalAddr->iClientIntAddr.SetAddress(ipv4_addr);
                   }   
                }
                break;

           case ATTR_INTERNAL_IP6_ADDR:
                //
                // A Virtual IPv6 address received.
                // Store value to CInternalAddress object
                // 
                if ( !aAttr->IsBasic() && (aAttr->Length() == 16) ) {
                   if ( !ia_received ) {
                      ia_received = ETrue;
                      Mem::Copy(&ipv6_addr.u.iAddr8, aAttr->VarValue(), sizeof(ipv6_addr.u.iAddr8));
                      InternalAddr->iClientIntAddr.SetAddress(ipv6_addr);
                   }   
                }
                break;

           case ATTR_INTERNAL_IP4_DNS:
                //
                // Internal DNS address received.
                // Add value to CInternalAddress object
                // 
                if ( !aAttr->IsBasic() && (aAttr->Length() == 4) ) {
                   ipv4_addr = GET32(aAttr->VarValue());
                   dns_addr  = new(ELeave)TInetAddr;
                   CleanupStack::PushL(dns_addr);                  
                   dns_addr->SetAddress(ipv4_addr);
                   InternalAddr->AppendL(dns_addr);
                   CleanupStack::Pop();  // dns_addr
                }
                break;
            
           default:
                break;
        }
        
        aAttr = aAttr->Next();
    }
    
    CleanupStack::Pop(); // InternalAddr
    iInternalAddr = InternalAddr;   

    iCfgModeCompleted = ETrue;

    DEBUG_LOG(_L("CONFIG-MODE completed, reply received!"));       
    
    return TRANSACTION_SUCCESS;
}

/**-------------------------------------------------------------------
 *
 * Method ProcessXauthRequest()
 * ProcessXauthRequest parses XAUTH request message attributes 
 * received from gateway. 
 *
 *--------------------------------------------------------------------*/
TInt CTransNegotiation::ProcessXauthRequestL(TDataISAKMP* aAttr, TInt aLth)
{
    TInt     status        = TRANSACTION_CONTINUE;
    TUint32  request_flags = 0;
    TPtr8    challenge(NULL, 0);
	TUint16  attr_type;
	
    while ( aLth > 0 ) {
        
        aLth = aLth - aAttr->Size();
        if ( aLth < 0 ) {
           DEBUG_LOG(_L("XAUTH REQUEST ERROR (Length mismatch in the attibutes)"));
           return TRANSACTION_FAILED;
        }
		attr_type = aAttr->Type();
		//
		// Check does the VPN gateway support older XAUTH draft version
		// draft-ietf-ipsec-isakmp-xauth-04.txt.
		// The check is based on attribute type values. In the older
		// draft attribute values are defined in range (13-20) and in the newer
		// "de-facto" draft-beaulieu-ike-xauth-02.txt the same
		// attribute values are in "private use" range (16520-16529)
		//
		if ( attr_type < ATTR_XAUTH_TYPE )
			iUseOlderPIXXauth = ETrue;	
 
        switch ( attr_type ) {

           case ATTR_XAUTH_TYPE:
		   case ATTR_PIX_XAUTH_TYPE:			   
                //
                // Extended authentication type requested
                //
                if ( aAttr->IsBasic() ) { // Basic attribute
                   request_flags |= (1 << (ATTR_XAUTH_TYPE - ATTR_XAUTH_TYPE));                 
                   iCurrExchange->iXauthType = aAttr->Value();
                }
                break;

           case ATTR_USER_NAME:
           case ATTR_PASSWORD:                         
           case ATTR_PASSCODE:
		   case ATTR_PIX_USER_NAME:
		   case ATTR_PIX_PASSWORD:
		   case ATTR_PIX_PASSCODE:
                //
                // Handles the following attribute values:
                // -- User name
                // -- Password
                // -- Passcode
                // Set a corresponding bit request flags. Parameter contents has
                // no meaning in request
                // 
                if ( !aAttr->IsBasic() ) {  // Variable length
				   if ( attr_type < ATTR_USER_NAME )
					    request_flags |= (1 << (attr_type - ATTR_PIX_XAUTH_TYPE)); 					   
                   else request_flags |= (1 << (attr_type - ATTR_XAUTH_TYPE)); 
                }
                break;

           case ATTR_MESSAGE:
		   case ATTR_PIX_MESSAGE:
                //
                // Message data attribute (NOT USED IN THIS PHASE)
                // 
                break;

           case ATTR_CHALLENGE:
		   case ATTR_PIX_CHALLENGE:
                //
                // Challenge data attribute
                //
                if ( !aAttr->IsBasic() && aAttr->Length() ) {
                   request_flags |= (1 << (ATTR_CHALLENGE - ATTR_XAUTH_TYPE));
                   challenge.Set(aAttr->VarValue(), aAttr->Length(), aAttr->Length());    
                }
                break;

           case ATTR_DOMAIN:
		   case ATTR_STATUS:
		   case ATTR_PIX_DOMAIN:			   
		   case ATTR_PIX_STATUS:
                //
                // Domain and status attributes (NOT USED IN THIS PHASE)
                // 
                break;

           case ATTR_NEXT_PIN:                          
                if ( !aAttr->IsBasic() ) {  // Variable length
                   request_flags |= (1 << (ATTR_NEXT_PIN - ATTR_XAUTH_TYPE)); 
                }
                break;

           case ATTR_ANSWER:
                //
                // Answer data attribute (NOT USED IN THIS PHASE)
                // 
                break;
            
           default:
                break;
        }
        
        aAttr = aAttr->Next();
    }

    //
    // Check if there already exist a authentication credentials request active
    // (= iRequestFlags are not zero). If there is ignore current message.
    //
    if ( iRequestFlags == 0 ) {
       iRequestFlags = request_flags;
    }    
    else {
       request_flags = 0;
       status        = TRANSACTION_IGNORE;
    }   
    //
    // Examine request_flags and show appropriate dialog to get requested
    // authentication credentials from user
    //
    switch ( request_flags & ~(1 << (ATTR_XAUTH_TYPE - ATTR_XAUTH_TYPE)) ) {

        case ( (1 << (ATTR_USER_NAME - ATTR_XAUTH_TYPE)) | (1 << (ATTR_PASSWORD - ATTR_XAUTH_TYPE))):
            //
            //  User name/Password authentication required
            //
			iDialog     = CIkev1Dialog::NewL(iPluginSession, iPluginSession->DialogAnchor(), iDebug);			
            iDialogInfo = new(ELeave) CAuthDialogInfo(iPluginSession, XAUTH_DIALOG_ID, iNegotiation->SAId(), iCurrExchange->iMessageId);
            iDialog->GetAsyncUNPWDialogL(iDialogInfo, (MIkeDialogComplete*)this);          
            break;

        case ( (1 << (ATTR_USER_NAME - ATTR_XAUTH_TYPE)) | (1 << (ATTR_PASSCODE - ATTR_XAUTH_TYPE))):
            //
            //  User name/Secure ID authentication required
            //
            iDialog     = CIkev1Dialog::NewL(iPluginSession, iPluginSession->DialogAnchor(), iDebug);
            iDialogInfo = new(ELeave) CAuthDialogInfo(iPluginSession, XAUTH_DIALOG_ID, iNegotiation->SAId(), iCurrExchange->iMessageId);
            iDialog->GetAsyncSecureidDialogL(iDialogInfo, (MIkeDialogComplete*)this);          
            break;

        case ( (1 << (ATTR_USER_NAME - ATTR_XAUTH_TYPE)) | (1 << (ATTR_NEXT_PIN - ATTR_XAUTH_TYPE))):
            //
            //  User name/Secure ID next pin required
            //
			iDialog     = CIkev1Dialog::NewL(iPluginSession, iPluginSession->DialogAnchor(), iDebug);			
			iDialogInfo = new(ELeave) CAuthDialogInfo(iPluginSession, XAUTH_DIALOG_ID, iNegotiation->SAId(), iCurrExchange->iMessageId);
            iDialog->GetAsyncSecureNextPinDialogL(iDialogInfo, (MIkeDialogComplete*)this);
            break;

        default:
            break;

    }   
    
    return status;

}

/**-------------------------------------------------------------------
 *
 * Method ProcessXauthStatus()
 * ProcessXauthStatus parses XAUTH Set message attributes received from gateway.
 * Only Status attribute has any relevance in Set message.
 *
 *--------------------------------------------------------------------*/
TInt CTransNegotiation::ProcessXauthStatusL(TDataISAKMP* aAttr, TInt aLth)
{
    TBuf8<16> attributes;    
    TInt      status = TRANSACTION_CONTINUE;
    TInt16    attr_status;
    
    while ( aLth > 0 ) {
        
        aLth = aLth - aAttr->Size();
        if ( aLth < 0 ) {
           DEBUG_LOG(_L("XAUTH SET ERROR (Length mismatch in the attibutes)"));
           return TRANSACTION_FAILED;
        }

        switch ( aAttr->Type() ) {

           case ATTR_STATUS:
		   case ATTR_PIX_STATUS:			   
                //
                // Status code from gateway
                // 
                if ( aAttr->IsBasic() ) { // Basic attribute
                   attr_status = aAttr->Value();    
                   if ( attr_status == ATTR_STATUS_OK )
                        status = TRANSACTION_SUCCESS;
                   else status = TRANSACTION_FAILED;   
                }
                break;
            
           default:
                break;
        }
        
        aAttr = aAttr->Next();
    }
    
    if ( status != TRANSACTION_CONTINUE ) {
       //
       // Send Transaction exchange ACK
       //
	   TUint16 AttrType;
	   if ( iUseOlderPIXXauth )
		    AttrType = ATTR_PIX_STATUS;
	   else AttrType = ATTR_STATUS;
	   
       AddAttributeData(attributes, AttrType, 2, (TUint8*)&attr_status);
       BuildAndSendMessageL(attributes, ISAKMP_CFG_ACK);
       if ( status == TRANSACTION_SUCCESS ) {
          DEBUG_LOG(_L("XAUTH authentication succeeded!"));
          iXauthCompleted = ETrue;
		  if ( iUserName ) {
		     //
    		 // Cache user name into user name file
			 //
		     CIkev1Dialog* Dialog = CIkev1Dialog::NewL(iPluginSession, iPluginSession->DialogAnchor(), iDebug);
             CleanupStack::PushL(Dialog);
			 TInt err(KErrNone);
			 TRAP(err, Dialog->StoreUserNameL(iUserName->Des()));
#ifdef _DEBUG			 
			 if (err == KErrNone)
			     DEBUG_LOG(_L("User Name caching succeeded"));
			 else DEBUG_LOG(_L("User Name caching failed"));
#endif // _DEBUG			 
			 CleanupStack::PopAndDestroy();					 
		  }	   
       }            
       else {
          DEBUG_LOG(_L("XAUTH authentication failed!"));
	   // Dialog object shall be delete in Dialog->RunL when dialog completed				  
          CIkev1Dialog* Dialog  = CIkev1Dialog::NewL(iPluginSession, iPluginSession->DialogAnchor(), iDebug);		  
          Dialog->ShowErrorDialogL(TVpnNoteDialog::EKmdAuthenticationFailed, NULL, NULL);
       }
    }
    
    return status;
}

/**--------------------------------------------------------------------------------
 *
 * Method CheckTransactionStatusL()
 * CheckTransactionStatus is after an incoming ISAKMP transaction exchange message
 * has been processed. This method decides the actions shall be taken next:
 * -- If current status (= call parameter) is continue, ignore or failed
 *    ==> Same status is returned
 * -- If current status is success and XAUTH completed.     
 *    ==> CONFIG MODE actions are started (= Config mode request is transmitted)
 * -- If current status is success and CONFIG MODE completed.     
 *    ==> XAUTH actions are started. (= We shall just wait for XAUTH request)
 * -- If current status is success and both CONFIG-MODE and XAUTH completed
 *    ==> TRANSACTION_SUCCESS status is returned
 *
 *--------------------------------------------------------------------*/
TInt CTransNegotiation::CheckTransactionStatusL(TInt aStatus)
{
    if ( aStatus == TRANSACTION_SUCCESS || aStatus == TRANSACTION_CONTINUE ) {
       //
       // Stop retransmission timer 
       //
       iNegotiation->iTimer->Cancel();
       
       if ( aStatus == TRANSACTION_SUCCESS ) {
          if ( iXauthCompleted ) {
             if ( !iCfgModeCompleted ) {
                aStatus = BuildConfigRequestL();
             }    
          }
          else {
             if ( !iXauthCompleted ) {
                aStatus = TRANSACTION_CONTINUE;
             }    
          }    
       }
    }
    
    return aStatus;
}   

/**-------------------------------------------------------------------
 *
 * Method BuildConfigRequestL()
 * BuildConfigRequestL() builds the CONFIG-MODE request message. 
 * In this phase requests the following parameters from gateway:
 * -- Client virtual IP in secure network = INTERNAL_IP4_ADDRESS, INTERNAL_IP4_NETMASK
 *                                         (INTERNAL_IP6_ADDRESS, INTERNAL_IP6_NETMASK)
 * -- DNS address(es) in secure network   = INTERNAL_IP4_DNS  
 *
 *--------------------------------------------------------------------*/
TInt CTransNegotiation::BuildConfigRequestL()
{

    TBuf8<16> attributes;
    
    TUint32  message_id = iNegotiation->RandomMessageId();

    iCurrExchange = AddExchangeL(message_id, INITIATOR); //Add a new transaction exchange
    iCurrExchange->iIdentifier = GetIdentifier();
    
    iCurrExchange->iIV.Copy(iNegotiation->iLastIV);      // Calculate base IV for ..
    iNegotiation->ComputeIVL(iCurrExchange->iIV, message_id); // transaction message

    AddAttributeData(attributes, ATTR_INTERNAL_IP4_ADDR, 0, NULL);
    AddAttributeData(attributes, ATTR_INTERNAL_IP4_DNS, 0, NULL);
	
    BuildAndSendMessageL(attributes, ISAKMP_CFG_REQUEST);
    DEBUG_LOG(_L("CONFIG-MODE started, request xmitted!")); 

    return TRANSACTION_CONTINUE;

    
}

/**-------------------------------------------------------------------
 *
 * Method AddAttributeData()
 * AddAttributeData() method adds one attribute data to an attribute buffer
 *
 *--------------------------------------------------------------------*/
void CTransNegotiation::AddAttributeData(TDes8& aAttrBfr, TInt aType, TInt aLth, TUint8* aData)
{
    TDataISAKMP attr;
    if ( aType == ATTR_STATUS     || aType == ATTR_XAUTH_TYPE ||
		 aType == ATTR_PIX_STATUS || aType == ATTR_PIX_XAUTH_TYPE) {
       //
       // Add a basic length attribute
       //
       attr.SetBasic(ETrue);
       attr.SetType((TUint16)aType);
       if ( aData ) 
          attr.SetValue(*(TUint16*)aData);
       aAttrBfr.Append((TUint8 *)&attr, sizeof(attr));
    }
    else {
       //
       // Add a variable length attribute
       //
       attr.SetBasic(EFalse);
       attr.SetType((TUint16)aType);
       attr.SetLength((TUint16)(aLth));
       aAttrBfr.Append((TUint8 *)&attr, sizeof(attr));
       if ( aLth ) 
          aAttrBfr.Append(aData, aLth);       
    }   
}

/**-------------------------------------------------------------------
 *
 * Method BuildAndSendMessageL()
 * BuildAndSendMessage() method builds ISAKMP transaction exchange message
 * and transmits it using CNegotiation class send() method.
 * The payload format of a transaction exchange message is the following:
 * HDR*, HASH, ATTR
 * Where the HASH payload contains the prf output, using SKEYID_a as
 * the key, and the M-ID (ISAKMP header Message ID) unique to this
 * exchange concatenated with all of the payloads after the HASH
 * payload. In other words, the hash for the above exchange is:
 * HASH = prf( SKEYID_a, M-ID | ATTR )
 *
 *--------------------------------------------------------------------*/
void CTransNegotiation::BuildAndSendMessageL(TDesC8& aAttrBfr, TUint8 aMsgType)
{
	TIkev1IsakmpStream* msg = iNegotiation->SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );
	
    TUint32 saved_msg_id     = iNegotiation->iMessageId;
    TUint8  saved_exchange   = iNegotiation->iExchange;
    iNegotiation->iMessageId = iCurrExchange->iMessageId; // used in method Isakmp_INIT()
    iNegotiation->iExchange  = ISAKMP_EXCHANGE_TRANSACT;  // used in method Isakmp_INIT()
    
    msg->IsakmpInit(iNegotiation);
    msg->IsakmpHashL();
    msg->IsakmpAttributes(aMsgType, iCurrExchange->iIdentifier, aAttrBfr);  
    msg->IsakmpHashContL();
    
    iNegotiation->SendL(*msg);

    iNegotiation->iMessageId = saved_msg_id;
    iNegotiation->iExchange  = saved_exchange;
    
}

/**-------------------------------------------------------------------
 *
 * Method FindExchange()
 * FindExchange() method finds a exchange strcuture for a specified message id 
 *
 *--------------------------------------------------------------------*/
TTransExchange* CTransNegotiation::FindExchange(TUint32 aMsgId)
{
    TTransExchange *exchange;
    TInt i = 0;

    while ( i < Count() )
    {
        exchange = At(i);
        if ( exchange->iMessageId == aMsgId )
           return exchange;
        i ++;
    }   

    return NULL;
}

/**-------------------------------------------------------------------
 *
 * Method AddExchangeL()
 * AddExchangeL() method allocates a new exchange structure and adds it
 * to exchange array. 
 *
 *--------------------------------------------------------------------*/
TTransExchange* CTransNegotiation::AddExchangeL(TUint32 aMsgId, TUint8 aRole )
{
    
    TTransExchange *exchange =  new(ELeave)TTransExchange;
    exchange->iMessageId     = aMsgId;
    exchange->iRole          = aRole;
    exchange->iIV.SetLength(0);
    AppendL(exchange);
    
    return exchange;
}

//
// The implementation for class MIkeDialogComplete virtual function
//
TInt CTransNegotiation::DialogCompleteL(CIkev1Dialog* /*aDialog*/, TAny* aUserInfo,
								        HBufC8* aUsername, HBufC8* aSecret, HBufC8* aDomain)
{
/*---------------------------------------------------------------------------
 *  
 *  A response received from client user (through asynchronous dialog)
 *  This method is introduced as a TUserCallback for CGetIKEPassword dialog
 *  object is created. When the dialog is completed this callback function
 *  is called to deliver Credentials data for CHRE payload attributes.
 *  Store credential buffers to CAuthDialogInfo object and call engine
 *  entry  
 *  
 *-------------------------------------------------------------------------*/
	TUint32 obj_id = 1;
	CAuthDialogInfo* info = (CAuthDialogInfo*)aUserInfo;
	DEBUG_LOG1(_L("CIKECRACKNegotiation::DialogCompleteL(), aUserInfo =  %x"), aUserInfo);

	if ( info )
	{
		obj_id = info->GetObjId();
		DEBUG_LOG1(_L("Preparing to call AuthDialogCompletedL(), ObjId = %x"), obj_id);
		if ( obj_id == XAUTH_DIALOG_ID )
		{
			info->iUsername = aUsername;
			info->iSecret   = aSecret;
			info->iDomain   = aDomain;
			obj_id = info->PluginSession()->AuthDialogCompletedL(info);
		}   
	}

	return obj_id;
}
