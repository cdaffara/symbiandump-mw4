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
* This module contains the private vendor specific extension of IKE.
* All of the current private extensions are related to Nokia VPN gateway
* and shall be used ONLY when the EPOC IKE is acting as a Nokia VPN remote
* access client.
* The following private extension are implemented:
*
* 1) Internal Address payload usage
*    Internal address payload is used to the deliver a secure network
*    adderess and secure network DNS address(es) from VPN gateway to a client.
*    The Internal address payloads are used in the last two IKE main mode
*    messages as follows:
*
*    Client (initiator)                           Gateway (responder)
*    .. SA, KE ...             --->
*                              <---               ..SA, KE ...
*    HDR*, INT_ADDR            --->
*                              <---               HDR*, INT_ADDR
*
*    Client sends an INT_ADDR payload with PRI_INTERNAL_ADDRESS attribute
*    Attribute value is 0.0.0.0.
*
*    Gateway responds with an INT_ADDR payload with PRI_INTERNAL_ADDRESS
*    attribute containing client internal address x.y.z.w
*    Gateway INT_ADDR payload may also contain attributes PRI_INTERNAL_DNS and
*    PRI_INTERNAL_WINS. PRI_INTERNAL_DNS contains a list of DNS IP addresses and
*    PRI_INTERNAL_WINS a list of WINS IP addresses.
*       
*
* 2) The NAT Traversal probing
*    The expanded Vendor-Id payload usage for the NAT Traversal probing.
*    The expanded Vendor-Id payloads contains the following information:
*
*    Client (initiator)                           Gateway (responder)
*    VID(hash, ip_addr, port)  --->
*                              <---               VID(hash, detected_ip_addr,
*                                                           detected_port)
*
*    Client sends a expanded Vendor-Id payload containing the following information:
*    hash    = Nokia VPN vendor specific hash data (used to recognize peer)
*    ip_addr = Client IKE own IP address
*    port    = Client IKE own port (=500)
*
*    Gateway responds with expanded Vendor-Id payload containing the following information:
*    hash    = Nokia VPN vendor specific hash data (used to recognize peer)
*    detected_ip_addr = Client IP address as detected in received IKE message
*                       IP header (=source IP address)
*    detected_port    = Client port as detected in received IKE message
*                       UDP header (=source port)
*
*    Both client and gateway do the following examination
*    if ( ip_addr != detected_ip_addr ) || ( port != detected_port )
*    then NAT Traversal shall be used IPSEC ESP traffic between
*    the client and gateway
*
*    Nokia VPN specific NAT Traversal means that IPSEC ESP traffic shall be
*    capsulated with UDP header.
*    The used UDP port for that purpose is 9872
*
*/

#include "ikev1private.h"
#include "ikev1dialog.h"
#include "ikev1negotiation.h"
#include "ikev1isakmpstream.h"

#include "ikepolparser.h"

const TUint8  BASE_VID_DATA[16] = {0x06, 0x3d, 0xf4, 0x13, 0x91, 0xa9, 0x19, 0xa2,
                                   0x5a, 0x61, 0xa8, 0x7c, 0x45, 0x02, 0x5f, 0xaf};

const TUint8  DPD_VID_DATA[16]  = {0xAF, 0xCA, 0xD7, 0x13, 0x68, 0xA1, 0xF1, 0xC9,
                                   0x6B, 0x86, 0x96, 0xFC, 0x77, 0x57, 0x01, 0x00};

TInt BuildVendorIdHash(TUint8 *aICOOKIE, TUint8 *aRCOOKIE, TUint8 *hash_data)
{
/*--------------------------------------------------------------------------------
 *
 *  Build Vendor Id hash data
 *
 *------------------------------------------------------------------------*/
//
// base = MD5("Network Alchemy, Inc., Version 1.0"); /* ASCII-Z end null included)
//
TInt i;
  
    Mem::Copy(hash_data, &BASE_VID_DATA[0], 16);  /* Hash base (MD5) */
    
    for ( i = 0; i < (ISAKMP_COOKIE_SIZE * 2); i++ ) {

        if ( i < ISAKMP_COOKIE_SIZE ) 
             *(hash_data + i) ^= *(aICOOKIE + i);
        else *(hash_data + i) ^= *(aRCOOKIE + (i - ISAKMP_COOKIE_SIZE));

    }
    
    return 16;
}   


TInt ConstructVendorId(TBool  aNATProbe,
                       TUint8 *aICOOKIE,
                       TUint8 *aRCOOKIE,
                       TInetAddr &aLocalAddr,
                       TVendorISAKMP *aVendorPayload)
{
/*------------------------------------------------------------------------
 *
 *  This method constructs a Vendor ID payload. If aNATProbe is TRUE
 *  an expanded format Vendor ID is constructed.
 *  Both Vendor ID formats contains a Nokia VPN vendor specific hash data
 *  which constructed as follows:
 *  base = MD5("Network Alchemy, Inc., Version 1.0");  ASCII-Z end null included)
 *  base = BASE_VID_DATA;
 *  Then the Vendor ID hash is consructed xor:ing ISAKMP cookies to hash as follows:
 *
 *  for ( i = 0; i < (ISAKMP_COOKIE_SIZE * 2); i++ ) {
 *      if ( i < ISAKMP_COOKIE_SIZE ) 
 *           base[i] ^= ICOOKIE[i];
 *      else base[i] ^= RCOOKIE[i - ISAKMP_COOKIE_SIZE];
 *  }
 *
 *  The expanded vendor ID payload looks like so:
 *
 *  General payload header   (next payload is "real" next payload)
 *    General payload header (next payload is "VENDOR_OPTION_NAT_TRAVERSAL")
 *      option hash
 *    General payload header (next payload is "VENDOR_OPTION_VERSION")
 *      option VENDOR_OPTION_NAT_TRAVERSAL
 *    General payload header (next payload is "NULL")
 *      option VENDOR_OPTION_VERSION
 *
 *  Expanded vendor id format is format is as follows:
 *
 *                      1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  ! Next Payload  !   RESERVED    !         Payload Length = 44   !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  ! OPTION_NAT_T  !   RESERVED    !  Hash_lth + 4 = 20            !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  !                Nokia VPN Vendor specific hash                 !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  ! OPTION_VERSION!   RESERVED    !  OPTION_NAT_T_LTH + 4 = 20    !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  !    sin_lth    !   sin_family  !           sin_port            !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  !                        sin_addr                               !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  !                                                               !
 *  .                     Zero * 2(?)                               .
 *  !                                                               ! 
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  !       0       !   RESERVED    !  OPTION_VERSION + 4 = 8       !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  !        MAJOR VERSION          !         MINOR VERSION         !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *  
 *------------------------------------------------------------------------*/
    TVendorISAKMP  *nat_vendor_id;
    TVendorISAKMP  *ver_vendor_id;       
    TNATTOption    *nat_t_option;
    TVersionOption *version_option; 
    TUint8         *next_payload;
    TUint32        vid_lth;
    TInetAddr      own_addr = aLocalAddr;

    next_payload  = (TUint8 *)aVendorPayload;   
    *next_payload = ISAKMP_PAYLOAD_NONE;   /* zeroe next payload field for sure */
    
    if ( aNATProbe ) {
       /*------------------------------------------------------------
        *
        *  Build expanded Vendor Id payload
        *  Build first VENDOR_OPTION_NAT_TRAVERSAL payload
        *
        *-----------------------------------------------------------*/
       nat_vendor_id = (TVendorISAKMP*)((TUint8*)aVendorPayload +
                                         sizeof(TPayloadISAKMP) +
                                         sizeof(TPayloadISAKMP) + 16); //bypass hash
       next_payload  = (TUint8 *)nat_vendor_id;
       *next_payload = VENDOR_OPTION_VERSION;
       nat_vendor_id->SetReserved(0);
       nat_vendor_id->SetLength(sizeof(TPayloadISAKMP) + SIN_LTH);
       nat_t_option = (TNATTOption*)nat_vendor_id->VIDData();
       nat_t_option->InitOption();
       nat_t_option->SetPort(500);
       if ( own_addr.IsV4Mapped() )
          own_addr.ConvertToV4();
       nat_t_option->SetAddress(own_addr.Address());
       /*------------------------------------------------------------
        *
        *  Build next VENDOR_OPTION_VERSION payload
        *  Set major version X and minor Y.
        *
        *-----------------------------------------------------------*/
       ver_vendor_id = (TVendorISAKMP*)((TUint8*)nat_vendor_id  +
                                         sizeof(TPayloadISAKMP) + SIN_LTH); //bypass NAT-T 
       next_payload  = (TUint8 *)ver_vendor_id;
       *next_payload = ISAKMP_PAYLOAD_NONE;
       ver_vendor_id->SetReserved(0);
       ver_vendor_id->SetLength(sizeof(TPayloadISAKMP) + VERSION_LTH);
       version_option = (TVersionOption*)ver_vendor_id->VIDData();
       version_option->SetVersion(MAJOR_VERSION, MINOR_VERSION);
       
       /*------------------------------------------------------------
        *
        *  Build "upper" Vendor Id payload general header
        *
        *-----------------------------------------------------------*/
       vid_lth = sizeof(TPayloadISAKMP) +      /* "outer" Vendor ID payload */
                 sizeof(TPayloadISAKMP) + 16 + /* VENDOR_OPTION_HASH */
                 sizeof(TPayloadISAKMP) + SIN_LTH +   /* VENDOR_OPTION_NAT_TRAVERSAL */
                 sizeof(TPayloadISAKMP) + VERSION_LTH;/* VENDOR_OPTION_VERSION */      
       aVendorPayload->SetLength((TUint16)vid_lth);         
       aVendorPayload->SetReserved(0);

       aVendorPayload = (TVendorISAKMP*)((TUint8*)aVendorPayload + sizeof(TPayloadISAKMP));
       next_payload   = (TUint8 *)aVendorPayload;
       *next_payload  = VENDOR_OPTION_NAT_TRAVERSAL;
    }
    else {
       vid_lth = sizeof(TPayloadISAKMP) + 16;
    }   
   /*------------------------------------------------------------
    *
    *  Store Hash data into Vendor Id payload
    *
    *-----------------------------------------------------------*/
    aVendorPayload->SetReserved(0);
    aVendorPayload->SetLength((TUint16)sizeof(TPayloadISAKMP) + 16);
    
    BuildVendorIdHash(aICOOKIE, aRCOOKIE,
                      aVendorPayload->VIDData());

    return vid_lth;
    
}


TBool ProcessVendorId(TBool  *aFamiliarPeer,
                      TUint8 *aICOOKIE,
                      TUint8 *aRCOOKIE,
                      TInetAddr &aLocalAddr,
                      TVendorISAKMP *aVendorPayload)
{
/*-------------------------------------------------------------------------
 *
 *  Process Vendor Id payload received from peer.
 *  The following actions taken:
 *  -- Check if a Nokia VPN implementation i peer (recognize hash in Vendor Id)
 *  -- If Nokia VPN implementation detected process possible
 *     VENDOR_OPTION_NAT_TRAVERSAL in expanded Vendor Id payload
 *
 *------------------------------------------------------------------------*/
    TBool         nokia_vpn_peer = EFalse;
    TBool         nat_t_required = EFalse;
    TVendorISAKMP *option_payload;
    TNATTOption   *nat_t_option;    
    TInt          vid_lth;
    TInt          tmp_lth;  
    TInt          hash_lth;
    TUint16       ptype;
    TUint16       detected_port;
    TUint8        ref_hash[20];
    TInetAddr     detected_addr;
    TInetAddr     reference_addr = aLocalAddr;

    vid_lth = aVendorPayload->GetLength() - sizeof(TPayloadISAKMP);
    if ( vid_lth > 15 ) {
       /*-------------------------------------------------------
        *
        * Check if expanded Vendor Id format
        *
        *-------------------------------------------------------*/
       tmp_lth  = vid_lth;  
       ptype    = ISAKMP_PAYLOAD_NONE;      
       hash_lth = BuildVendorIdHash(aICOOKIE, aRCOOKIE, ref_hash);
       option_payload = aVendorPayload;        
       if ( vid_lth > hash_lth ) {
          /*---------------------------------------------------------------------
           *
           * An expanded format Vendor Id, bypass "outer" payload general header
           * And do sanity check for VENDOR_OPTION_HASH option payload
           *
           *--------------------------------------------------------------------*/
          option_payload = (TVendorISAKMP*)((TUint8*)option_payload + sizeof(TPayloadISAKMP));
          ptype   = option_payload->GetPayload();                     
          tmp_lth = option_payload->GetLength();                  
          if ( tmp_lth == (sizeof(TPayloadISAKMP) + 16 ) ) 
//            &&
//           ( option_payload->GetReserved() == 0 ) ) {  //Must be always 0
               tmp_lth -= sizeof(TPayloadISAKMP);
          else tmp_lth  = 0;
       }
       
       if ( tmp_lth == hash_lth ) {
          /*---------------------------------------------
           *
           *  Check that Vendor Id hash match
           * 
           *---------------------------------------------*/
           if ( Mem::Compare(option_payload->VIDData(), tmp_lth, ref_hash, hash_lth) == 0 ) {
              /*-----------------------------------------------------------
               *
               * Process other Vendor Id option payload(s)
               * In this phase only VENDOR_OPTION_NAT_TRAVERSAL is processed
               * other options are ignored 
               *
               *-----------------------------------------------------------*/
              nokia_vpn_peer = ETrue;              
              tmp_lth       += sizeof(TPayloadISAKMP); 
              option_payload = (TVendorISAKMP*)((TUint8*)option_payload + tmp_lth);
              
              while ( ptype != ISAKMP_PAYLOAD_NONE ) {
                  
                  if ( vid_lth <= tmp_lth ) {
                     break; 
                  }
                  hash_lth = option_payload->GetLength();
                  tmp_lth += hash_lth;                
                  if ( ( hash_lth < (MIN_ISAKMP_PAYLOAD_SIZE + SIN_LTH) ) ) {
//                     &&
//                     ( option_payload->GetReserved() != 0 ) ) } //Must be always 0
                     break; 
                  }   
                  if ( ptype == VENDOR_OPTION_NAT_TRAVERSAL ) {
                     if ( reference_addr.IsV4Mapped() )
                        reference_addr.ConvertToV4();
                     hash_lth -= sizeof(TPayloadISAKMP);   /* option data length */
                     nat_t_option = (TNATTOption*)((TUint8*)option_payload + sizeof(TPayloadISAKMP));
                     detected_port = nat_t_option->GetPort();
                     detected_addr.SetAddress(nat_t_option->GetAddress());
                     if ( (detected_port != 500)         /* Port changed */
                          ||
                          !(detected_addr.Match(reference_addr))) { /* address changed */
                        nat_t_required = ETrue; 
                     }   
                     break; 
                  }

                  ptype          = option_payload->GetPayload();  //Next payload                  
                  option_payload = (TVendorISAKMP*)((TUint8*)option_payload + hash_lth);

              }   
           }       
       }
           
    }   

    if ( aFamiliarPeer )
       *aFamiliarPeer = nokia_vpn_peer;

    return nat_t_required;
        
}

/**-------------------------------------------------------------------
 *
 * Function BuildDPDVendorId()
 * This method builds a Dead Peer Detection (DPD) related Vendor ID
 * payload  and adds it into the IKE message. The vendor id is
 * specified  in the draft <draft-ietf-ipsec-dpd-04.txt> and its
 * content is the following:
 *                       1 
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
 * !                           !M!M!  
 * !      HASHED_VENDOR_ID     !J!N! 
 * !                           !R!R! 
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
 *
 * Hash data is, 
 * {0xAF, 0xCA, 0xD7, 0x13, 0x68, 0xA1, 0xF1, 0xC9, 0x6B, 0x86, 0x96,
 * 0xFC, 0x77, 0x57}, and MJR and MNR
 * MJR = 1 and MNR = 0 
 *
 *--------------------------------------------------------------------*/
void BuildDPDVendorId(TIkev1IsakmpStream &aMsg)
{
	TInetAddr DummyAddr;

	aMsg.IsakmpVendorId(IETF_NATT_VENDOR_ID,
					    NULL, NULL, DummyAddr, // These parameters has no relevance with IETF_NATT_VID_DATA
						(TUint8*)DPD_VID_DATA,
						sizeof(DPD_VID_DATA));
}

TBool CheckDPDVendorId(const TVendorISAKMP *aVendorPayload)
{
/**---------------------------------------------------------------------------------------
 *
 *  This method checks does the remote end support DPD draft <draft-ietf-ipsec-dpd-04.txt>
 *  
 *---------------------------------------------------------------------------------------*/
	TInt vid_lth = aVendorPayload->GetLength() - sizeof(TPayloadISAKMP);
	if ( vid_lth == sizeof(DPD_VID_DATA) )
	{
		if ( Mem::Compare(aVendorPayload->VIDData(), vid_lth, (TUint8*)DPD_VID_DATA, vid_lth) == 0 ) 
			return ETrue;  // Remote end supports DPD draft
	}
	return EFalse;
}

TInt CheckCredentials(CIkeData *aHostData )
{
/*-------------------------------------------------------------------------
 *
 *  This function is called by CNegotiation::InitNegotiationL() method
 *  when the current IKE proposal defines aggresssive mode exchange with
 *  pre-shared key authentication.
 *  The following special actions are taken:
 *  -- If no pre-shared key data is defined, launch a dialog where
 *     user name and password information is asked from the user.
 *  -- User name information is store to current CIkeData iFQDN field
 *     (represent IKE identification)
 *  -- Password data shall be stored to current CIkeData iPresharedKey field
 *
 *  This functionality is related to Checkpoint gateway.
 *  To use Aggressive mode exchange and pre-shared key authentication like
 *  this implement kind of "legacy authentication method" for IKE where
 *  client (=initiator) authentication is based on user name/password pair.
 *  User name is sent from initiator (=client) to responder (=Checkpoint GW)
 *  in the IKE ID payload. However, the password data is NOT transmitted in
 *  any payload, but it is used as pre-shared key in both ends.
 *  (= Checkpoint gateway shall use user name data received in IKE ID payload
 *     as a reference to the correct pre-shared key)
 *
 *------------------------------------------------------------------------*/
    if ( !aHostData || aHostData->iPresharedKey.iKey.Length() )
       return KErrNone;
    
    aHostData->iPresharedKey.iFormat = STRING_KEY;
    aHostData->iFQDN.SetLength(0); // Override FQDN in host data with user name
    
    return CIkev1Dialog::GetSyncUNPWDialog(aHostData->iFQDN, aHostData->iPresharedKey.iKey);
}


CInternalAddress* ProcessIntNetL(TINTNETISAKMP *aIntNetpayload)
{
/*-------------------------------------------------------------------------
 *
 *  Process Internal address payload received (sanity check already done)
 *  Process payload attributes as follows:
 *  -- Parse PRI_INTERNAL_ADDRESS attribute and store value to aInternalAddr
 *  -- Parse PRI_INTERNAL_DNS attributes and build list of DNS addresses
 *     There exists an own attribute for all DNS addresses
 *  -- Ignore other attributes (=PRI_INTERNAL_WINS)
 *
 *  In this phase only IPv4 Internal addresses are supported by the
 *  Nokia VPN gateway
 *
 *------------------------------------------------------------------------*/
    TInt length = (TInt)aIntNetpayload->GetLength();
    if ( STATIC_CAST(TUint, length) < sizeof(TINTNETISAKMP) ) {
       return NULL;
    }
    
    length -= sizeof(TINTNETISAKMP);  /* Attribute data lengt in payload */

    TUint32   ipv4_addr;
    TBool     internal_address = EFalse;
    TInetAddr *dns_addr;
    CInternalAddress *InternalAddr = new (ELeave)CInternalAddress(1);
    CleanupStack::PushL(InternalAddr);    
    TDataISAKMP *attr  = aIntNetpayload->INTNETAttrib();
    
    while ( length > 0 ) {
        
        length = length - attr->Size();
        if ( length < 0 ) {
           CleanupStack::PopAndDestroy();  /* delete InternalAddr */
           return NULL;
        }
        switch ( attr->Type() ) {

           case PRI_INTERNAL_ADDRESS:
                /*-----------------------------------------------------------
                 * Internal address received from gateway. If several
                 * Internal address attributes detected use the first address
                 *------------------------------------------------------------*/
                if ( attr->IsBasic() || ( attr->Length() != 4) ) {
                   CleanupStack::PopAndDestroy();  /* delete InternalAddr */                    
                   return NULL;
                }
                if ( !internal_address ) {
                   internal_address = ETrue;                    
                   ipv4_addr = GET32(attr->VarValue()); 
                   ipv4_addr = ByteOrder::Swap32(ipv4_addr); //NOT IN NETWORK ORDER !!!!                   
                   InternalAddr->iClientIntAddr.SetAddress(ipv4_addr);
                }   
                break;
            
           case PRI_INTERNAL_DNS:
                /*-----------------------------------------------------------
                 * Internal DNS address received from gateway 
                 *------------------------------------------------------------*/
                if ( attr->IsBasic() || ( attr->Length() != 4 ) ) {
                   CleanupStack::PopAndDestroy();  /* delete InternalAddr */
                   return NULL;
                }
                ipv4_addr = GET32(attr->VarValue()); 
                ipv4_addr = ByteOrder::Swap32(ipv4_addr); //NOT IN NETWORK ORDER !!!!
                dns_addr   = new(ELeave)TInetAddr;
                CleanupStack::PushL(dns_addr);                  
                dns_addr->SetAddress(ipv4_addr);
                InternalAddr->AppendL(dns_addr);
                CleanupStack::Pop();  /* delete dns_addr */               
                break;
        
           default:
                /*-----------------------------------------------------------
                 * Other attributes (WINS address) are ignored
                 *------------------------------------------------------------*/
                break; 
        }
        
        attr = attr->Next();
    }

    if ( !internal_address ) {
       /*-----------------------------------------------------
        * No client internal address defined.
        * Internal address negotiation failed
        *----------------------------------------------------*/
       delete InternalAddr;
       InternalAddr = NULL;
    }
    
    CleanupStack::Pop();  // Remove InternalAddr from cleanup stack
    
    return InternalAddr;
}


