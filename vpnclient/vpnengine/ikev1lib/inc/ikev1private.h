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
*/

#ifndef IKEV1PRIVATE_H
#define IKEV1PRIVATE_H

#include <f32file.h>
#include "ikemsgheader.h"


class TIkev1IsakmpStream;
class CIkeIPSocket;
class CIkeData;
class TVendorISAKMP;
class TINTNETISAKMP;
class CInternalAddress;
class TInetAddr;

TInt ConstructVendorId(TBool  aNATProbe,
                       TUint8 *aICOOKIE,
                       TUint8 *aRCOOKIE,
                       TInetAddr &aLocalAddr,
                       TVendorISAKMP *vendor_payload);
TInt BuildVendorIdHash(TUint8 *aICOOKIE,
                       TUint8 *aRCOOKIE,
                       TUint8 *hash_data);
TBool ProcessVendorId(TBool  *aFamiliarPeer,
                      TUint8 *aICOOKIE,
                      TUint8 *aRCOOKIE,
                      TInetAddr &aLocalAddr,
                      TVendorISAKMP *aVendorPayload);
void BuildDPDVendorId(TIkev1IsakmpStream &aMsg);
TBool CheckDPDVendorId(const TVendorISAKMP *aVendorPayload);
CInternalAddress* ProcessIntNetL(TINTNETISAKMP *aIntNetpayload);

TBool InsertVPNInterfaceL(CInternalAddress *aInternalAddr, CIkeIPSocket *aSocket, TUint32 *aIfNbr, RFs *aFS);
TBool RemoveVPNInterfaceL(CInternalAddress *aInternalAddr, CIkeIPSocket *aSocket, TUint32 *aIfNbr, RFs *aFS);
TBool AddVPNRoute(CInternalAddress *aInternalAddr, CIkeIPSocket *aSocket,
                  TInetAddr &aDstAddr, TInetAddr &aDstMask);
TInt CheckCredentials(CIkeData *aHostData);
/*---------------------------------------------------------------------------
 *
 *  Expanded Vendor Id payload option VENDOR_OPTION_NAT_TRAVERSAL handling:
 *                      1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  !    sin_lth    !   sin_family  !           sin_port            !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  !                        sin_addr                               !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  !                                                               !
 *  .                     Zero * 2(?)                               .
 *  !                                                               ! 
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 *---------------------------------------------------------------------------*/
#define SIN_LTH        16   // VENDOR_OPTION_NAT_TRAVERSAL data part length
#define SIN_FAMILY      2   // = IPv4 Address 


class TNATTOption
    {
public:
    inline void InitOption() { Mem::FillZ(&u.iData8[0], SIN_LTH);
                               u.iData8[0] = SIN_LTH; u.iData8[1] = SIN_FAMILY;}
    inline void SetPort(TUint16 aPort) { PUT16(&u.iData8[2], aPort);}   
    inline void SetAddress(TUint32 aAddr) { PUT32(&u.iData8[4], aAddr);}
    inline TUint16 GetPort() {return (TUint16)GET16(&u.iData8[2]);} 
    inline TUint32 GetAddress() {return (TUint32)GET32(&u.iData8[4]);}  
private:
    union
        {
        TUint32 iData32[4];
        TUint16 iData16[8];
        TUint8  iData8[16];
        } u;
    };

/*---------------------------------------------------------------------------
 *
 *  Expanded Vendor Id payload option VENDOR_OPTION_VERSION handling:
 *                      1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
 *  !        MAJOR VERSION          !         MINOR VERSION         !
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *
 *---------------------------------------------------------------------------*/
#define VERSION_LTH     4   // VENDOR_OPTION_VERSION data part length
#define MAJOR_VERSION   3   // ???????????!!!!!!!!!!!!!!!!????????????
#define MINOR_VERSION   3   // ???????????!!!!!!!!!!!!!!!!????????????

class TVersionOption
    {
public:
    inline void SetVersion(TUint16 aMajor,
                           TUint16 aMinor) { PUT16(&u.iData8[0], aMajor);
                                             PUT16(&u.iData8[2], aMinor);}   
private:
    union
        {
        TUint32 iData32[1];
        TUint16 iData16[2];
        TUint8  iData8[4];
        } u;
    };

#endif // IKEV1PRIVATE_H
