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
* Description: IKEv2 Mobility and Multihoming Protocol.
*
*/

#include <es_sock.h>
#include <in_sock.h>
#include "ikedebug.h"
#include "ikev2mobike.h"
#include "ikev2SAdata.h"
#include "ikemsgrec.h"
#include "ikev2payloads.h"
#include "ikev2const.h"
#include "ikev2Negotiation.h"
#include "ikev2plugin.h"
#include "ikev2natt.h"
#include "ikev2proposal.h"
#include "ikev2pluginsession.h"

_LIT8(KZeroDesc, "");

TBool Ikev2MobIke::ProcessNotifysL(CIkev2Negotiation* aNegotiation, 
                                   const CArrayFixFlat<TNotifPayloadIkev2*>& aNotifys, 
                                   TBool aRequest, TInt aExchange)
{
    __ASSERT_DEBUG(aNegotiation, User::Invariant());
	if ( !aNegotiation->iHdr.iIkeData->iUseMobIke )
	{
		return EFalse; 
	}
	 
	TInt MsgType;
	TNotifPayloadIkev2* Payload;
	TPtrC8 Cookie2(NULL,0);
	TBool  NatDetection = EFalse;
	TBool  Status        = EFalse;
	TInt   Count = aNotifys.Count();
	TInt i      = 0;

	while ( i < Count )
	{
		
		Payload = aNotifys.At(i);
		MsgType = (TInt)Payload->GetMsgType();
		// 
		//  Process possible MOBIKE Notify messages 
		//
		switch ( MsgType )
		{

			case MOBIKE_SUPPORTED:
				//
				// Remote end supports MOBIKE protocol
				//
			    aNegotiation->iHdr.iMobikeUsed  = ETrue;
			    aNegotiation->iHdr.iFloatedPort = ETrue;  // Floated port used for now 			
			    aNegotiation->iHdr.iDestinAddr.SetPort(FLOATED_IKE_PORT);			
				break;

			case ADDITIONAL_IPV4_ADDRESS:
			case ADDITIONAL_IPV6_ADDRESS:				
				//
				// Additional IP addresses Notify
				//
				if ( aNegotiation->iHdr.iMobikeUsed && (aExchange == INFORMATIONAL) )
				{
					Status = ETrue;		
				}	
				break;

			case UPDATE_SA_ADDRESS:
				//
				// Peer informs about the IP address change
				//
				if ( aNegotiation->iHdr.iMobikeUsed && aRequest && (aExchange == INFORMATIONAL) )
				{
					aNegotiation->iIkeV2PlugInSession.RemoteAddrChanged(&aNegotiation->iHdr, aNegotiation->iHdr.iDestinAddr); 		
					Status = ETrue;		
				}	
				break;

			case COOKIE2:
				//
				// Peer informs about the IP address change
				//
				if ( aNegotiation->iHdr.iMobikeUsed && aRequest && (aExchange == INFORMATIONAL) )
				{
					Cookie2.Set(Payload->NotifData(), Payload->NotifDataLength());
					Status = ETrue;		
				}	
				break;

			case NAT_PREVENTION:
				//
				// NAT Prevention Notify
				//
				if ( aNegotiation->iHdr.iMobikeUsed  )
				{
					if ( aExchange == INFORMATIONAL ) 
					   Status = ETrue;		
				}	
				break;

			case UNACCPETABLE_ADDRESSES:
			case NAT_PREVENTED:
				if ( aNegotiation->iHdr.iMobikeUsed  )
				{					
					if ( aExchange == INFORMATIONAL ) 
					   Status = ETrue;		
				}	
				break;

			case NAT_DETECTION_SOURCE_IP:
			case NAT_DETECTION_DESTINATION_IP:
				if ( aNegotiation->iHdr.iMobikeUsed && (aExchange == INFORMATIONAL))
				{
					NatDetection = ETrue;		
					Status = ETrue;		
				}	
				break;
				
			default:
				break;
		}	

		i ++;   
	}

	if ( Status && aRequest && (aExchange == INFORMATIONAL) )
	{
	    //
	    // Build informational response to MOBIKE request
	    //                
        CIkeV2Message* ikeMsg = CIkeV2Message::NewL(aNegotiation->iHdr.SpiI(),
                                                    aNegotiation->iHdr.SpiR(),
                                                    INFORMATIONAL,
                                                    aNegotiation->iHdr.iInitiator, 
                                                    ETrue,
                                                    aNegotiation->iHdr.ExpectedRequestId(),
                                                    aNegotiation->iDebug);
        
        ikeMsg->AppendEncryptedPayloadL(aNegotiation->iHdr.iCipherBlkLth);            
        
	    if ( Cookie2.Ptr() )
		{
            //
            // Peer is using COOKIE2. Return COOKIE2 payload data as such 
            //		
            ikeMsg->AppendNotifyPayloadL(IKEV2_PROTOCOL, KZeroDesc, COOKIE, Cookie2);
		}
		if ( NatDetection )
		{
		   //
		   // Peer is using NAT_DETECTION_*_IP payloads. Build corresponding response
		   //
			TBool NatDetectOk;
			TInetAddr DummyIp;
			DummyIp.SetAddress(KInetAddrNone);   // 0.0.0.0
			TUint32 NATFlags = CIkev2NatT::CheckPeerNotifysL(aNotifys, DummyIp, aNegotiation->iHdr.iDestinAddr, FLOATED_IKE_PORT,
				                                             ikeMsg->InitiatorSpi(), ikeMsg->ResponderSpi(), 
				                                             NatDetectOk);
			if ( NatDetectOk )
			{
			   aNegotiation->iHdr.iNATFlags = NATFlags;
			   aNegotiation->GetNatStatus(NatDetectOk, aNegotiation->iHdr.iDestinAddr);				   
			}	
			CIkev2NatT* NatNotify = CIkev2NatT::NewL(DummyIp, aNegotiation->iHdr.iDestinAddr, 
                                                     FLOATED_IKE_PORT, 
                                                     ikeMsg->InitiatorSpi(), ikeMsg->ResponderSpi());
			CleanupStack::PushL(NatNotify);
                       
           ikeMsg->AppendNotifyPayloadL(IKEV2_PROTOCOL, KZeroDesc, NAT_DETECTION_SOURCE_IP, 
                                        NatNotify->SourceNofify());
           ikeMsg->AppendNotifyPayloadL(IKEV2_PROTOCOL, KZeroDesc, NAT_DETECTION_DESTINATION_IP, 
                                        NatNotify->DestinNofify());
														
			CleanupStack::PopAndDestroy(NatNotify); 			 				   			   				
		}				
		
		aNegotiation->SendIkeMsgL(ikeMsg);
		aNegotiation->iIkeV2PlugInSession.UpdateIkev2SAL(&aNegotiation->iHdr, NULL);
		if ( (aNegotiation->iState != KStateIkeInfoRequest) && (aNegotiation->iState != KStateIkeDeleteRequest) && (aNegotiation->iState != KStateIkeDeleteResponse) )
		   aNegotiation->iState = KStateIkeSaCompleted;
	}
   //	
   // else
   // Currently there is no need to examine any MOBIKE Notify payloads
   // present in Informational response
   //

	return Status;
}

TBool Ikev2MobIke::SendUpdateSaAddrNotifyL(CIkev2Negotiation* aNegotiation)
{
    ASSERT(aNegotiation);

    //we support only changing of our address.   
    CIkeV2Message* ikeMsg = CIkeV2Message::NewL(aNegotiation->iHdr.SpiI(),
                                                aNegotiation->iHdr.SpiR(),
                                                INFORMATIONAL,
                                                aNegotiation->iHdr.iInitiator,
                                                EFalse,
                                                aNegotiation->iHdr.NextRequestId(),
                                                aNegotiation->iDebug);
    
    ikeMsg->AppendEncryptedPayloadL(aNegotiation->iHdr.iCipherBlkLth);            
    ikeMsg->AppendNotifyPayloadL(IKEV2_PROT_NONE, KZeroDesc, UPDATE_SA_ADDRESS, KZeroDesc);
    
	aNegotiation->SendIkeMsgL(ikeMsg);
	
	aNegotiation->iState = KStateIkeInfoRequest;

	return ETrue;
}
