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
* Description: 
* Class CIkev2NatT implements NAT Traversal functionality specified in IKEv2.
*
*/
#include <in_sock.h>
#include "ikev2natt.h"
#include "ikev2SAdata.h"
#include "ikev2payloads.h"
#include "ikev2const.h"
#include "ikecrypto.h"

CIkev2NatT* CIkev2NatT::NewL(const TInetAddr& aSourceAddr, const TInetAddr& aDestinationAddr, TUint16 aPort, 
                             const TDesC8& aInitiatorSpi, const TDesC8& aResponderSpi)
{
    CIkev2NatT* self = new (ELeave)CIkev2NatT();
	CleanupStack::PushL(self);		
	self->ConstructL(aSourceAddr, aDestinationAddr, aPort, aInitiatorSpi, aResponderSpi);
	CleanupStack::Pop(self);			
    
    return self;
}

void CIkev2NatT::ConstructL(const TInetAddr& aSourceAddr, const TInetAddr& aDestinationAddr, TUint16 aPort, 
                            const TDesC8& aInitiatorSpi, const TDesC8& aResponderSpi)
{
	iSrcNotify = GenerateNatDetectionHashL(aInitiatorSpi, aResponderSpi,
                                           aSourceAddr, aPort);
	
	iDstNotify = GenerateNatDetectionHashL(aInitiatorSpi, aResponderSpi,
                                           aDestinationAddr, aPort);
}

TUint32 CIkev2NatT::CheckPeerNotifysL(const CArrayFixFlat<TNotifPayloadIkev2*>& aNotifys, 
                                      const TInetAddr& aLocalAddr, const  TInetAddr& aRemoteAddr, TUint16 aPort, 
									  const TDesC8& aInitiatorSpi, const TDesC8& aResponderSpi, TBool& aSupported)
{    
  //
  // Check does there exists NAT_DETECTION_SOURCE_IP and NAT_DETECTION_DESTINATION_IP
  // Notify payload. If found compare payload data to local end NAT
  // traversal data as follows:
  // -- NAT_DETECTION_SOURCE_IP Notifys are examined against local Notify
  //    payload iDstIdentiy data: If no match found
  //    ==> Peer is behind  NAT
  // -- NAT_DETECTION_DESTINATION_IP Notify is examined against local Notify
  //    payload iSrcIdentiy data: If no match found
  //    ==> Local end behind NAT
  //
    aSupported = EFalse;
	TInt Count = aNotifys.Count();

	CIkev2NatT* RefObj = CIkev2NatT::NewL(aLocalAddr, aRemoteAddr, aPort, 
                                          aInitiatorSpi, aResponderSpi);
	 
	TNotifPayloadIkev2* PeerNotify;
	TUint32 PeerLth;
	TBool SrcMatch   = EFalse;
	TBool DstMatch   = EFalse;	
    TUint32 NatFlags = 0;
    TInt i = 0;

	while ( i < Count )
	{
		PeerNotify = aNotifys.At(i);
		if ( (PeerNotify->GetMsgType() == NAT_DETECTION_SOURCE_IP) && !SrcMatch )
		{
			NatFlags |= REMOTE_END_NAT;
			PeerLth = TPayloadIkev2::Cast(PeerNotify)->GetLength() - (TUint32)(PeerNotify->Size() + PeerNotify->GetSPISize());
			const TPtrC8 peerNotify(PeerNotify->NotifData(), PeerLth);
			if ( RefObj->DestinNofify().Compare(peerNotify) == 0)
			{
				SrcMatch  = ETrue;
				NatFlags &= ~REMOTE_END_NAT;
			}	
		}
		else if ( (PeerNotify->GetMsgType() == NAT_DETECTION_DESTINATION_IP) && !DstMatch )
		{
			aSupported = ETrue;						
			NatFlags  |= LOCAL_END_NAT;
			PeerLth = TPayloadIkev2::Cast(PeerNotify)->GetLength() - (TUint32)(PeerNotify->Size() + PeerNotify->GetSPISize()); 
			const TPtrC8 peerNotify(PeerNotify->NotifData(), PeerLth);
			if ( RefObj->SourceNofify().Compare(peerNotify) == 0 )
			{
				DstMatch  = ETrue;							
				NatFlags &= ~LOCAL_END_NAT;
			}	
		}			
	    i ++;
    }

	delete RefObj;	
    return NatFlags;     
}

HBufC8* CIkev2NatT::GenerateNatDetectionHashL(const TDesC8& aInitiatorSpi, const TDesC8& aResponderSpi,
                                              TInetAddr aIpAddress, TUint16 aPort ) const
{
    __ASSERT_DEBUG( aInitiatorSpi.Length() == IKEV2_SPI_SIZE, User::Invariant());
    __ASSERT_DEBUG( aResponderSpi.Length() == IKEV2_SPI_SIZE, User::Invariant());
    
    //
    //  Calculate HASH = SHA1(SPIS | IP | Port) both for local- and remote IP address/port
    //
    TBuf8<64> hashInData;  
        
    hashInData.Append(aInitiatorSpi);
    hashInData.Append(aResponderSpi);
    
    if ( aIpAddress.Family() == KAfInet )
    {
        TUint32 ipv4addr = ByteOrder::Swap32(aIpAddress.Address());//Put in network order
        hashInData.Append(reinterpret_cast<TUint8*>(&ipv4addr), sizeof(ipv4addr));
    }   
    else
    {
        if ( aIpAddress.IsV4Mapped() )
        {           
           aIpAddress.ConvertToV4();  // IPv4 format
           TUint32 ipv4addr = ByteOrder::Swap32(aIpAddress.Address());//Put in network order           
           hashInData.Append(reinterpret_cast<TUint8*>(&ipv4addr), sizeof(ipv4addr));
        }   
        else
        {
           const TUint8* addr = &aIpAddress.Ip6Address().u.iAddr8[0];  //Address in a bytestream
           hashInData.Append(addr, 16);
        }   
    }
    
    aPort = ByteOrder::Swap16(aPort);
    hashInData.Append(reinterpret_cast<TUint8*>(&aPort), sizeof(aPort));
    
    return IkeCrypto::PrfL(hashInData, PRF_HMAC_SHA1);
}
