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
* Description:  Negotiation of NAT-Traversal in the IKE
*
*/


#ifndef C_IKEV1NATDISCOVERY_H
#define C_IKEV1NATDISCOVERY_H

#include <e32base.h>

class TIkev1IsakmpStream;
class CProposal_IIList;
class TVendorISAKMP;
class TNATDISAKMP;
class TNATOaISAKMP;
class TInetAddr;

//    Class CIkev1NatDiscovery implents negotiation of NAT-Traversal.
//    The current implementation follows IETF <draft-ietf-ipsec-nat-t-ike-03.txt>.
class CIkev1NatDiscovery : public CBase
    {
public:
    CIkev1NatDiscovery(){};  
    static CIkev1NatDiscovery* NewL(TUint32 aNatFlags);
    ~CIkev1NatDiscovery() {};
    void    BuildNatVendorId(TIkev1IsakmpStream &aMsg);
    void	BuildRfcNatVendorId(TIkev1IsakmpStream &aMsg);
    TBool   CheckNatVendorId(const TVendorISAKMP *aVendorPayload);
    TBool   CheckRfcNatVendorId(const TVendorISAKMP *aVendorPayload);
    void    BuildDiscoveryPayloadsL(TIkev1IsakmpStream &aMsg, TUint16 aHashType,
                                    TUint8 *aICOOKIE, TUint8 *aRCOOKIE,
                                    TInetAddr &aLocalAddr, TInetAddr &aRemoteAddr);
    TUint32 CheckDiscoveryPayloadsL(const CArrayFixFlat<const TNATDISAKMP *> *aNatDPayloadArray,
                                    TUint16 aHashType, TUint8 *aICOOKIE, TUint8 *aRCOOKIE,
                                    TInetAddr &aLocalAddr, TInetAddr &aRemoteAddr);
    
    void  BuildNatOaPayload(TIkev1IsakmpStream &aMsg, TInetAddr &aLocalAddr, CProposal_IIList *aProposalList);
    TBool GetPeerOriginalAddress(const TNATOaISAKMP *aNatOaPayload, TInetAddr& aRemoteOrigAddr, CProposal_IIList *aProposalList);

private:
    void    CalculateAddrPortHashL(TUint16 aHashType,
                                   TUint8 *aICOOKIE, TUint8 *aRCOOKIE,
                                   TInetAddr &aLocalAddr, TInetAddr &aRemoteAddr);
    TBool   CompareHashData(TUint8 *aHashData, TUint32 aHashLth, TDesC8 &aReferenceHash);   


    TBool        iSupport;  // ETrue = Both ends supports current draft
    TBool		 iRfcSupport; //ETrue = Both ends support current RFC 
    TBuf8<16>    iIetfNattVidHash; // NAT-T vendor id string (= md5("draft-ietf-ipsec-nat-t-ike-05")
    TBuf8<16>    iIetfRfcNattVidHash; // NAT-T vendor id string (= md5("RFC 3947")
    TBool        iHashExists; // ETrue = Hash data below has been calculated
    TBuf8<32>    iLocalAddrPortHash;  // value of the HASH(CKY-I | CKY-R | Local_IP  | Port)
    TBuf8<32>    iRemoteAddrPortHash; // value of the HASH(CKY-I | CKY-R | Remote_IP | Port)        
    
    };


#endif // C_IKEV1NATDISCOVERY_H
