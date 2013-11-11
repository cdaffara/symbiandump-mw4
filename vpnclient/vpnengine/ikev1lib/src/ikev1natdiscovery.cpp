/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Negotiation of NAT-Traversal in the IKE
*
*/


#include "ikev1natdiscovery.h"
#include "ikev1crypto.h"
#include "ikev1negotiation.h"
#include "ikev1isakmpstream.h"

//  "03" version hash data
//const TUint8  IETF_NATT_VID_DATA[16] = {0x7d, 0x94, 0x19, 0xa6, 0x53, 0x10, 0xca, 0x6f,
//                                        0x2c, 0x17, 0x9d, 0x92, 0x15, 0x52, 0x9d, 0x56};
_LIT8(KIetfNatTHashSeed,"draft-ietf-ipsec-nat-t-ike-03");
_LIT8(KIetfRfcNatTHashSeed,"RFC 3947");

CIkev1NatDiscovery* CIkev1NatDiscovery::NewL(TUint32 aNatFlags)
{
    CIkev1NatDiscovery* NatDiscovery = new (ELeave)CIkev1NatDiscovery();

    if ( aNatFlags )
    {
    	NatDiscovery->iSupport = ETrue;  // Caller forces support indicator to OK
    	NatDiscovery->iRfcSupport= ETrue;	
    }
    else 
    {
    	NatDiscovery->iSupport = EFalse;
    	NatDiscovery->iRfcSupport= EFalse;
    }
   

    //
    // Build Vendor string for NAT discovery. This string is used later in
    // a ISAKMP phase 1 Vendor Id payload to inform remote host that local
    // end supprts NAT Traversal.
    // The vendor string is produced as the following hash:
    // Vendor Id string = MD5("draft-ietf-ipsec-nat-t-ike-03")
    //
    MD5HashL(KIetfNatTHashSeed, NatDiscovery->iIetfNattVidHash);  // Calculate hash value        
    MD5HashL(KIetfRfcNatTHashSeed, NatDiscovery->iIetfRfcNattVidHash);  // Calculate hash value 
    
    return NatDiscovery;
}

void CIkev1NatDiscovery::BuildNatVendorId(TIkev1IsakmpStream &aMsg)
{
/**---------------------------------------------------------------------------------------
 *
 *  This method builds a NAT traversal related Vendor ID payload and adds it into 
 *  the IKE message. The vendor id content is the following:
 *  MD5 hash of "draft-ietf-ipsec-nat-t-ike-05" (calculated earlier in NewL())
 *  
 *---------------------------------------------------------------------------------------*/
    TInetAddr DummyAddr;

    aMsg.IsakmpVendorId(IETF_NATT_VENDOR_ID,
                        NULL, NULL, DummyAddr, // These parameters has no relevance with IETF_NATT_VID_DATA
                        (TUint8*)iIetfNattVidHash.Ptr(), iIetfNattVidHash.Length());

     
}

void CIkev1NatDiscovery::BuildRfcNatVendorId(TIkev1IsakmpStream &aMsg)
{
/**---------------------------------------------------------------------------------------
 *
 *  This method builds a NAT traversal related Vendor ID payload and adds it into 
 *  the IKE message. The vendor id content is the following:
 *  MD5 hash of "RFC 3947" (calculated earlier in NewL())
 *  
 *---------------------------------------------------------------------------------------*/
    TInetAddr DummyAddr;

    aMsg.IsakmpVendorId(IETF_RFC_NATT_VENDOR_ID,
                        NULL, NULL, DummyAddr, // These parameters has no relevance with IETF_NATT_VID_DATA
                        (TUint8*)iIetfRfcNattVidHash.Ptr(), iIetfRfcNattVidHash.Length());

     
}

TBool CIkev1NatDiscovery::CheckNatVendorId(const TVendorISAKMP *aVendorPayload)
{
/**---------------------------------------------------------------------------------------
 *
 *  This method checks does the remote end support IETF NAT traversal <draft-ietf-ipsec-nat-t-ike-03>
 *  The vendor id content MUST be the following:
 *  
 *---------------------------------------------------------------------------------------*/
    TInt vid_lth = aVendorPayload->GetLength() - sizeof(TPayloadISAKMP);
    if ( vid_lth == iIetfNattVidHash.Length() ) {
       if ( Mem::Compare(aVendorPayload->VIDData(), vid_lth, iIetfNattVidHash.Ptr(), vid_lth) == 0 ) {
          iSupport = ETrue;  // Remote end supports IETF NAT traversal
       }    
    }

    return iSupport;

}

TBool CIkev1NatDiscovery::CheckRfcNatVendorId(const TVendorISAKMP *aVendorPayload)
{
/**---------------------------------------------------------------------------------------
 *
 *  This method checks does the remote end support IETF NAT traversal RFC 3947
 *  The vendor id content MUST be the following:
 *  
 *---------------------------------------------------------------------------------------*/
    TInt vid_lth = aVendorPayload->GetLength() - sizeof(TPayloadISAKMP);
    if ( vid_lth == iIetfRfcNattVidHash.Length() ) {
       if ( Mem::Compare(aVendorPayload->VIDData(), vid_lth, iIetfRfcNattVidHash.Ptr(), vid_lth) == 0 ) {
          //iSupport = ETrue;  // Remote end supports IETF NAT traversal according to IETF draft 03
          iRfcSupport= ETrue; // Remote end supports IETF NAT traversal according to RFC 3947
       }    
    }
    return iRfcSupport;
}

   
void CIkev1NatDiscovery::BuildDiscoveryPayloadsL(TIkev1IsakmpStream &aMsg, TUint16 aHashType,
                                                 TUint8 *aICOOKIE, TUint8 *aRCOOKIE,
                                                 TInetAddr &aLocalAddr, TInetAddr &aRemoteAddr)
{
/**---------------------------------------------------------------------------------------
 *
 *  This builds NAT Discovery payloads for negotiation.
 *  from draft-ietf-ipsec-nat-t-ike-03;
 *  "
 *  The purpose of the NAT-D payload is twofold, It not only detects the
 *  presence of NAT between two IKE peers, it also detects where the NAT is.
 *  The location of the NAT device is important in that the keepalives need
 *  to initiate from the peer "behind" the NAT.
 *
 *  To detect the NAT between the two hosts, we need to detect if the IP
 *  address or the port changes along the path. This is done by sending the
 *  hashes of IP address and port of both source and destination addresses
 *  from each end to another. When both ends calculate those hashes and get
 *  same result they know there is no NAT between. If the hashes do not
 *  match, somebody translated the address or port between, meaning we need
 *  to do NAT-Traversal to get IPsec packet through.
 *
 *  If the sender of the packet does not know his own IP address (in case of
 *  multiple interfaces, and implementation don't know which is used to
 *  route the packet out), he can include multiple local hashes to the
 *  packet (as separate NAT-D payloads). In this case the NAT is detected if
 *  and only if none of the hashes match.
 *
 *  The hashes are sent as a series of NAT-D (NAT discovery) payloads.  Each
 *  payload contains one hash, so in case of multiple hashes, multiple NAT-D
 *  payloads are sent. In normal case there is only two NAT-D payloads.
 * 
 *   The format of the NAT-D packet is
 *
 *    1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8
 *   +---------------+---------------+---------------+---------------+
 *   | Next Payload  |    RESERVED   |        Payload length         |
 *   +---------------+---------------+---------------+---------------+
 *   ~               HASH of the address and port                    ~
 *   +---------------+---------------+---------------+---------------+
 *
 *  The payload type for the NAT discovery payload is 130 (XXX CHANGE).
 *
 *  The HASH is calculated as follows:
 *
 *   HASH = HASH(CKY-I | CKY-R | IP | Port)
 *
 *  using the negotiated HASH algorithm. All data inside the HASH is in the
 *  network byte-order. The IP is 4 octets for the IPv4 address and 16
 *  octets for the IPv6 address. The port number is encoded as 2 octet
 *  number in network byte-order. The first NAT-D payload contains the
 *  remote ends IP address and port (i.e the destination address of the UDP
 *  packet). The rest of the NAT-D payloads contain possible local end IP
 *  addresses and ports (i.e all possible source addresses of the UDP packet)."
 *  
 *---------------------------------------------------------------------------------------*/
    if ( iSupport || iRfcSupport) {
       CalculateAddrPortHashL(aHashType, aICOOKIE, aRCOOKIE, aLocalAddr, aRemoteAddr);
	   	
       aMsg.IsakmpNatD(iRfcSupport, iRemoteAddrPortHash); // NAT-D payload with HASH(CKY-I | CKY-R | Remote_IP | Remote_Port)
       aMsg.IsakmpNatD(iRfcSupport, iLocalAddrPortHash);  // NAT-D payload with HASH(CKY-I | CKY-R | Local_IP | Local_Port)          
    }   
}

TUint32 CIkev1NatDiscovery::CheckDiscoveryPayloadsL(const CArrayFixFlat<const TNATDISAKMP *> *aNatDPayloadArray,
                                                    TUint16 aHashType, TUint8 *aICOOKIE, TUint8 *aRCOOKIE,
                                                    TInetAddr &aLocalAddr, TInetAddr &aRemoteAddr)
{
/**---------------------------------------------------------------------------------------
 *
 *  This check NAT Discovery payloads received from remote end.
 *  from draft-ietf-ipsec-nat-t-ike-03;
 *  "
 *  If there is no NAT between then the first NAT-D payload should match one
 *  of the local NAT-D packet (i.e the local NAT-D payloads this host is
 *  sending out), and the one of the other NAT-D payloads must match the
 *  remote ends IP address and port. If the first check fails (i.e first
 *  NAT-D payload does not match any of the local IP addresses and ports),
 *  then it means that there is dynamic NAT between, and this end should
 *  start sending keepalives as defined in the <draft-ietf-ipsec-udp-encaps-03.txt>.
 *
 *---------------------------------------------------------------------------------------*/
    TUint32 NatFlags = 0;
    
    if ( iSupport || iRfcSupport ) {
        
       TInt count = aNatDPayloadArray->Count();
       if ( count > 1 ) {
          //
          // Check that the first hash corresponds current local address port pair
          //
          CalculateAddrPortHashL(aHashType, aICOOKIE, aRCOOKIE, aLocalAddr, aRemoteAddr);
          
          const TNATDISAKMP *NatDPayload = aNatDPayloadArray->At(0);
          if ( !CompareHashData(NatDPayload->HashData(), NatDPayload->HashLth(), iLocalAddrPortHash) ) {
             NatFlags |= LOCAL_END_NAT; //Local end is behind a NAT device 
          }
          
          //
          // Check the rest of NAT discovery payloads. One of them must correspond remote hash data
          // calculated in local end
          //
          NatFlags |= REMOTE_END_NAT;  // Remote end is behind a NAT device (as default)
          
          for ( TInt i = 1; (i < count); i++ )
          {
              NatDPayload = aNatDPayloadArray->At(i);
              if ( CompareHashData(NatDPayload->HashData(), NatDPayload->HashLth(), iRemoteAddrPortHash) ) {
                 NatFlags &= ~REMOTE_END_NAT; //Remote end is NOT behind a NAT device
                 break;
              }
              
          }
       }
        
    }

    return NatFlags;
     
}


void CIkev1NatDiscovery::BuildNatOaPayload(TIkev1IsakmpStream &aMsg, TInetAddr &aLocalAddr, CProposal_IIList *aProposalList)
{
(void)aMsg; (void)aLocalAddr; (void)aProposalList; 
    return;
}

TBool CIkev1NatDiscovery::GetPeerOriginalAddress(const TNATOaISAKMP *aNatOaPayload, TInetAddr& aRemoteOrigAddr, CProposal_IIList *aProposalList)
{
(void)aNatOaPayload; (void)aRemoteOrigAddr; (void)aProposalList;    
    aRemoteOrigAddr.Init(KAFUnspec);  // Set address value undefined
    return EFalse;
}   



void CIkev1NatDiscovery::CalculateAddrPortHashL(TUint16 aHashType,
                                                TUint8 *aICOOKIE, TUint8 *aRCOOKIE,
                                                TInetAddr &aLocalAddr, TInetAddr &aRemoteAddr)
{
    if ( iHashExists ) {
       return;  //Hash has been already calculated
    }   
/**---------------------------------------------------------------------------------------
 *
 *  Calculate HASH = HASH(CKY-I | CKY-R | IP | Port) both for local- and remote IP address/port
 *
 *---------------------------------------------------------------------------------------*/
    TBuf8<64> in_data;
    const TUint8 *pnum;
    TUint32 ipv4addr;
    TUint16 port;
    
    in_data.Append(aICOOKIE, ISAKMP_COOKIE_SIZE);
    in_data.Append(aRCOOKIE, ISAKMP_COOKIE_SIZE);

	TInetAddr HashAddr = aLocalAddr;
	HashAddr.SetPort(500);  //Set local port to default IKE port value
	TInt i = 0;
	
	while ( i < 2 ) {
	
    	if ( HashAddr.Family() == KAfInet ) {
		    ipv4addr = ByteOrder::Swap32(HashAddr.Address());//Put in network order
		    pnum = (TUint8*)&ipv4addr;
		    in_data.Append(pnum, sizeof(TUint32));
	    }   
	    else {
		    if ( HashAddr.IsV4Mapped() ) {
    	       HashAddr.ConvertToV4();  // IPv4 format
		       ipv4addr = ByteOrder::Swap32(HashAddr.Address());//Put in network order
		       pnum = (TUint8*)&ipv4addr;
		       in_data.Append(pnum, sizeof(TUint32));
		    }   
		    else {
               pnum = &HashAddr.Ip6Address().u.iAddr8[0];  //Address in a bytestream
		       in_data.Append(pnum, 16);
			}   
		}

		port = ByteOrder::Swap16(HashAddr.Port());//Put in network order
		pnum = (TUint8*)&port;  
		in_data.Append(pnum, sizeof(TUint16));

		if ( i ) {
    	   if ( aHashType == HASH_MD5 )
	            MD5HashL(in_data, iRemoteAddrPortHash);  // Calculate hash value
		   else SHA1HashL(in_data, iRemoteAddrPortHash);
		}
		else {
			if ( aHashType == HASH_MD5 )
				 MD5HashL(in_data, iLocalAddrPortHash);  // Calculate hash value
			else SHA1HashL(in_data, iLocalAddrPortHash);
		}		
		in_data.SetLength(ISAKMP_COOKIE_SIZE + ISAKMP_COOKIE_SIZE);  // Reset lenght to Icookie + Rcookie
		HashAddr = aRemoteAddr; // Process remote address next
					 
		i ++;
	}

    iHashExists = ETrue;    
    
}

TBool CIkev1NatDiscovery::CompareHashData(TUint8 *aHashData, TUint32 aHashLth, TDesC8 &aReferenceHash)
{
/**---------------------------------------------------------------------------------------
 *
 *  Compare current hash data to the reference hash data provided
 *
 *---------------------------------------------------------------------------------------*/
    TBool result = EFalse;
                  
    if ( (TInt)aHashLth == aReferenceHash.Length() ) {
       if ( Mem::Compare(aHashData, aHashLth, aReferenceHash.Ptr(), aHashLth) == 0 ) {
          result = ETrue; 
       }       
    }   

    return result;
}

