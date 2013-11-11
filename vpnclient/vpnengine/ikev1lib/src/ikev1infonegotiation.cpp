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
* Description:   CIkev1InfoNegotiation class
*
*/

#include <vpnlogmessages.rsg>

#include "ikev1infonegotiation.h"
#include "ikev1negotiation.h"
#include "ikev1SAdata.h"
#include "ikev1isakmpct.h"
#include "ikedebug.h"
#include "ikev1pluginsession.h"
#include "ikev1negotiation.h"
#include "ikev1crypto.h"
#include "ikev1payload.h"
#include "ikev1crack.h"
#include "ikev1trans.h"
#include "kmdapi.h"
#include "kmdeventloggerif.h"


CIkev1InfoNegotiation::CIkev1InfoNegotiation( CIkev1PluginSession& aPluginSession,
                                              CIkev1Negotiation& aNegotiation,
                                              MIkeDebug& aDebug )
 : iPluginSession( aPluginSession ),
   iNegotiation( aNegotiation ),
   iDebug( aDebug )
{        
}

#ifdef _DEBUG
void CIkev1InfoNegotiation::ExecuteL( const ThdrISAKMP& aHdr,
                                      const TInetAddr& aSrcAddr,
                                      TInt aLocalPort )
#else
void CIkev1InfoNegotiation::ExecuteL( const ThdrISAKMP& aHdr,
                                      const TInetAddr& /*aSrcAddr*/,
                                      TInt /*aLocalPort*/ )
#endif
{
    const ThdrISAKMP *hdr = NULL;
    TUint8 *msg=NULL;
    TBuf8<IKEV1_MAX_IV_SIZE> tmp_IV;    //Temporal IV. Used to update the real one if the msg OK

    iMessageId = aHdr.GetMessageId();   //Saves the ID to compute IV and hash
    if (aHdr.GetFlags() & ISAKMP_HDR_EFLAG) //if encrypted
    {
        DEBUG_LOG(_L("Received message (encr)."));
        msg = new (ELeave) TUint8[aHdr.GetLength()];    //to place the new msg
		CleanupStack::PushL(msg);
		
        Mem::Copy(msg,(TUint8 *)&aHdr, sizeof(aHdr));   //The header is not encrypted

        DEBUG_LOG(_L("Message ID recv:"));
#ifdef _DEBUG        
        TUint32 swap_id = ByteOrder::Swap32(iMessageId);
        DEBUG_LOG_ARRAY((TUint8 *)&swap_id, sizeof(iMessageId));
        DEBUG_LOG(_L("Notif IV:"));
#endif // _DEBUG        
        //Notify and Phase II requires a recomputing of IV

        if (iNegotiation.iLastIV.Length() != 0)
            tmp_IV.Copy(iNegotiation.iLastIV);
        else    //iLastIV not yet computed so current iIV is used
            tmp_IV.Copy(iNegotiation.iIV);
        iNegotiation.ComputeIVL(tmp_IV, iMessageId);

        DEBUG_LOG(_L("Decrypting..."));

        DecryptL((TUint8 *)aHdr.Next(),&msg[sizeof(aHdr)], aHdr.GetLength()-sizeof(aHdr), tmp_IV, iNegotiation.iSKEYID_e, iNegotiation.iChosenProposal_I.iAttrList->iEncrAlg);
        hdr=(ThdrISAKMP *)msg;  //decrypted msg
    }
    else
        hdr = &aHdr;

    DEBUG_LOG(_L("Received message."));
#ifdef _DEBUG
    const TPtrC8 ikeMsgPtr( (TUint8*)hdr,(TUint16)hdr->GetLength() );
    TInetAddr localAddr;
    iPluginSession.GetLocalAddress( localAddr );
    localAddr.SetPort( aLocalPort );
    TRACE_MSG_IKEV1( ikeMsgPtr, aSrcAddr, localAddr );
#endif // _DEBUG    
    InfoExchangeL(*hdr);
    if (msg)    //If used erase it (when encryption)
   	   CleanupStack::PopAndDestroy();
}

MKmdEventLoggerIf& CIkev1InfoNegotiation::EventLogger()
{
    return iPluginSession.EventLogger();
}

//No phase dependant. May inform of an error or general status info
void CIkev1InfoNegotiation::InfoExchangeL(const ThdrISAKMP &aHdr)
{
	iNegotiation.iLengthLeft = aHdr.GetLength();  //Used to check the size in the payload are OK
	
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, iNegotiation, iDebug);
	if (!payload)
		return;
	CleanupStack::PushL(payload);

	TInt i;			
	TBool notif_ok = EFalse;    
    //If the message contains a hash
    if ( payload->iHash )
    {
        if ( payload->iNotifs->Count() )
        {
            //Checks if the hash value is OK. Here because need the notification payload
            if (!iNegotiation.VerifyInformationalHashL(payload->iHash, payload->iNotifs->At(0), iMessageId))
            {
                DEBUG_LOG(_L("AUTHENTICATION_FAILED (Informational hash)"));
                iNegotiation.SendNotifyL(AUTHENTICATION_FAILED);
            }
            else    //Hash OK
			{
				i = 0;
				while ( i < payload->iNotifs->Count() )
				{	
                    notif_ok = ProcessNotificationL(payload->iNotifs->At(i), ETrue);
					if ( !notif_ok )
					   break;	
					i ++;
				}	
			}	
        }
        else if ( payload->iDeletes->Count() )
        {
            if (!iNegotiation.VerifyInformationalHashL(payload->iHash, payload->iDeletes->At(0), iMessageId))
            {
                DEBUG_LOG(_L("AUTHENTICATION_FAILED (Informational hash)"));
                iNegotiation.SendNotifyL(AUTHENTICATION_FAILED);
            }
            else
            {   //Hash OK
                if ( !iNegotiation.iAutoLogin && iNegotiation.iCRACKneg )
                   iNegotiation.iCRACKneg->CrackAuthenticationFailedL(NULL);
                if ( !iNegotiation.iAutoLogin && iNegotiation.iTransactionNeg )
                   iNegotiation.iTransactionNeg->TransactionFailedL(NULL);
				i = 0;
				while ( i < payload->iDeletes->Count() )
				{	
					notif_ok = ProcessDeleteL(payload->iDeletes->At(0));
					if (!notif_ok)
					   break;	
					i ++;
				}	
            }   
        }
        else
        {
            DEBUG_LOG(_L("PAYLOAD_MALFORMED (no hash or delete payload)"));
            iNegotiation.SendNotifyL(PAYLOAD_MALFORMED);
        }
    }
    else    //No hash sent  
    {
        if (aHdr.GetFlags() & ISAKMP_HDR_EFLAG) //if encrypted
        {
            DEBUG_LOG(_L("PAYLOAD_MALFORMED (Hash required)"));
            iNegotiation.SendNotifyL(PAYLOAD_MALFORMED);
        }
        else    //Not encrypted so not hash required
        {
			i = 0;
			while ( i < payload->iNotifs->Count() )
			{	
				notif_ok = ProcessNotificationL(payload->iNotifs->At(i), EFalse);
				if ( !notif_ok )
					break;	
				i ++;
			}	
        }
    }
    
    if ( notif_ok ) {
	   const TNotificationISAKMP* notif = NULL;	
	   if (	payload->iNotifs->Count() )
		   notif = payload->iNotifs->At(0); 
       if ( iNegotiation.iCRACKneg ) {
		  if ( !iNegotiation.iAutoLogin )
             iNegotiation.iCRACKneg->CrackAuthenticationFailedL(notif);
		  iNegotiation.SetErrorStatus(KKmdIkeAuthFailedErr);
		  iNegotiation.AcquireSAErrorResponse(KKmdIkeAuthFailedErr);
	   }	 
       if ( iNegotiation.iTransactionNeg ) {
	      if ( !iNegotiation.iAutoLogin )		   
             iNegotiation.iTransactionNeg->TransactionFailedL(notif);
		  iNegotiation.SetErrorStatus(KKmdIkeAuthFailedErr);		  
		  iNegotiation.AcquireSAErrorResponse(KKmdIkeAuthFailedErr);
	   }	  
    }   

	CleanupStack::PopAndDestroy();  //payload
	
}


//Handles Notification Payload
TBool CIkev1InfoNegotiation::ProcessNotificationL(const TPayloadISAKMP *aPayload, TBool aEncrypted)
{
#ifdef _DEBUG
    TBuf<80> str;
#endif // _DEBUG    
    TNotificationISAKMP *notif = TNotificationISAKMP::Ptr(aPayload);
    if (!iNegotiation.CheckDOI(notif->GetDOI()))
    {
        DEBUG_LOG(_L("DOI_NOT_SUPPORTED in NOT Payload Message."));
        return EFalse;
    }
	TBool   Status;
	TUint16 MsgType = notif->GetMsgType();
	
	if ( (MsgType == DPD_R_U_THERE || MsgType == DPD_R_U_THERE_ACK) && aEncrypted )
	{
	   return ProcessDPDNotifyL(notif);	
	}
	
	if ( MsgType <= UNEQUAL_PAYLOAD_LENGTHS )
	{	
#ifdef _DEBUG	
        str.Copy(_L("Error/Status Type: "));
#endif // _DEBUG        
        Status = ETrue;
		iNegotiation.SetNotifyStatus(MsgType);		
		
	    LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
	                   R_VPN_MSG_VPN_GW_ERR_RESP_RECEIVED,
	                   MsgType, 
	                   iPluginSession.VpnIapId(),
	                   &(iNegotiation.iRemoteAddr) );
	}
	else
	{
#ifdef _DEBUG	
		str.Copy(_L("Unexpected info notification: "));
#endif // _DEBUG		
		Status = EFalse;  	
	}
#ifdef _DEBUG	
	str.Append(CIkev1Negotiation::TextNotifyType(MsgType));
	DEBUG_LOG(str);
#endif // _DEBUG	
    return Status;	
}

//
// Process DPD R-U_THERE / R-U-THERE-ACK
// When a R-U-THERE notify received it processes as follows:
// -- Find an ISAKMP SA with SPI in notify message
// -- Pass DPD notify message for further processing into 
//    CIkev1Negotiation::NotifyMessageReceived() via iNegotiation reference 
//
TBool CIkev1InfoNegotiation::ProcessDPDNotifyL(TNotificationISAKMP* aNotify)
{

    if ( aNotify->GetSPISize() == (2 * ISAKMP_COOKIE_SIZE) && (aNotify->GetNotifDataSize() == 4)) 
	{
	   TCookie cookie_I, cookie_R;
	   cookie_I.Copy(aNotify->GetSPI(), ISAKMP_COOKIE_SIZE);
	   cookie_R.Copy((aNotify->GetSPI() + ISAKMP_COOKIE_SIZE), ISAKMP_COOKIE_SIZE);
	   
	   TIkev1SAData* Sa = iPluginSession.FindIkev1SAData(cookie_I, cookie_R);
	   if ( !Sa )
	   {	   
		  Sa = iPluginSession.FindIkev1SAData(cookie_R, cookie_I);
#ifdef _DEBUG		  
		  if (Sa) DEBUG_LOG(_L("ISAKMP SA found for DPD notify message with CKY_R+CKY_I"));
#endif // _DEBUG		  
	   }	  
	   if ( Sa && Sa->iDPDSupported )
	   {
		   TUint32 Sequence = GET32(aNotify->GetNotifData());
		   iNegotiation.DpdNotifyMessageReceivedL(Sa, aNotify->GetMsgType(), Sequence);
	   }
#ifdef _DEBUG	   
	   else DEBUG_LOG(_L("No ISAKMP SA found for DPD notify message"));
#endif // _DEBUG	   
	}
#ifdef _DEBUG    
	else DEBUG_LOG(_L("Illegal SPI- or notify data length in DPD message"));
#endif // _DEBUG	
  		
	return EFalse;
}

//Handles Delete Payload
TBool CIkev1InfoNegotiation::ProcessDeleteL(const TPayloadISAKMP *aPayload)
{
TDeleteISAKMP *delete_payload = TDeleteISAKMP::Ptr(aPayload);

#ifdef _DEBUG
    TBuf<1200> msg;    
    DEBUG_LOG(_L("Delete Payload received!!!"));
    delete_payload->String(msg);
    DEBUG_LOG(msg);
#endif // _DEBUG    
    if (!iNegotiation.CheckDOI(delete_payload->DOI()))
    {
        DEBUG_LOG(_L("DOI_NOT_SUPPORTED in delete payload"));
        return EFalse;
    }
    
    TUint8 protocol = delete_payload->Protocol();
    TUint32 spi;
    TInetAddr remote_addr(iNegotiation.iRemoteAddr);
    remote_addr.SetPort(KInetPortAny);

    TInt err = KErrNone;
    if ( protocol == PROTO_ISAKMP )
	{	
        iPluginSession.DeleteISAKMPSAsL( delete_payload, iNegotiation );
	}	
    else    //IPSEC AH or ESP (others will be discarded by the kernel)
    {
        if (delete_payload->SPISize() != sizeof(TUint32))
        {
            DEBUG_LOG(_L("Bad SPI Size for a IPsec SA. (SA Not deleted)"));
        }
		TIpsecSPI IpsecSpi;
        for (TInt i=0; i < delete_payload->NumSPI(); i++)   //Shouldn't be more than one
        {
            Mem::Copy((TUint8*)&spi, delete_payload->SPI(i),sizeof(TUint32));
            if (err == KErrNone)
			{	
                //The right one is the Outbound(Local->Remote) one to avoid sending when deleted at the other side
                //The opposite if sending a Delete
				IpsecSpi.iSrcAddr  = iNegotiation.iLocalAddr;
				IpsecSpi.iDstAddr  = remote_addr;
				IpsecSpi.iSPI      = spi;
				IpsecSpi.iProtocol = protocol;
				IpsecSpi.iInbound  = EFalse;
				if (iPluginSession.DeleteIpsecSpi(iNegotiation.SAId(), spi, EFalse))
				    {
				    DEBUG_LOG(_L("Deleting IPsec SA"));
                    iPluginSession.DeleteIpsecSA(IpsecSpi.iSPI, IpsecSpi.iSrcAddr, IpsecSpi.iDstAddr, 
                                                     IpsecSpi.iProtocol);
				    }
			}	
            else
            {
                DEBUG_LOG1(_L("IPsec SA with SPI=%x not deleted"), ByteOrder::Swap32(spi));
            }
        }
    }

	return ETrue;
}
