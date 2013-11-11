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
* Description:  Stream class definition for ISAKMP
*
*/


#ifndef IKEV1ISAKMPSTREAM_H
#define IKEV1ISAKMPSTREAM_H

#include <e32base.h>


class CIkev1Negotiation;
class TInetAddr;
class TAttrib;
class TIdentISAKMP;
class TAttrib_II;
class MIkeDebug;

//
// Constructing outgoing message
//
class TIkev1IsakmpStream
{
public:
    TIkev1IsakmpStream( MIkeDebug& aDebug );
    void IsakmpInit(CIkev1Negotiation *aSession);
    void IsakmpSa();
    void IsakmpKeyL();
    void IsakmpOwnIdentL();
    void IsakmpPeerIdentL();
    void IsakmpCertificateL();
    void IsakmpCertificateReqL();
    void IsakmpHashL();
    void IsakmpHashContL();
    void IsakmpSignatureL();
    void IsakmpNonce();
    void IsakmpNotification(TUint16 aType, TUint8 aProtocol, TUint8* aNotifData = NULL, TInt aLth = 0);
    void IsakmpReplayStatus(TUint8 aProtocol, TUint32 aSPI, TUint8 aReplayWindowLength);
    void IsakmpResponderLifetime(TUint8 aProtocol, TUint32 aSPI, const TDesC8 &aLifetime, const TDesC8 &aLifesize);
    void IsakmpDelete(TDesC8 &aSPI, TUint8 aProtocol);
    void IsakmpChre(TUint16 aLAMType, TUint16 aAttr1, HBufC8 *aBfr1,
                    TUint16 aAttr2, HBufC8 *aBfr2, TUint16 aAttr3, HBufC8 *aBfr3);
    void IsakmpVendorId(TInt aID_Type, TUint8 *aICOOKIE, TUint8 *aRCOOKIE, TInetAddr &aLocalAddr,
                        TUint8 *aGenericVidData = NULL, TInt aGenericVidLth = 0);
    void IsakmpIntnet(TUint32 aIpv4Addr);
    void IsakmpNatD(TBool aRfcNatt,TDesC8 &aHash);       
    void IsakmpAttributes(TUint8 aMsgType, TUint16 aIdentifier, TDesC8 &aAttributes);  
    
private:
    void IsakmpProposal();
    void IsakmpTransform(TUint8 *aTransform, TUint8 aNumTransforms);
    void IsakmpAttrib(TUint8 *aTransform);
    void IsakmpAttrib1(TAttrib *aTransform);
    void IsakmpAttrib2(TAttrib_II *aTransform);
    void IsakmpChreAttrib(TUint16 aType, HBufC8 *aBfr);
    void IsakmpIdentL(TBool aIsOwn);    //Own or Peer
	void Isakmp_Phase1_IdL(TDes8& aIdData, TIdentISAKMP& aIdPayload);	

private:
    CIkev1Negotiation *iNegotiation;//const 
    TUint8 *iNextPayload;
    TUint8 *iPropNextPayload;   //Used for Proposals to avoid overwritting the SA next 
                                //field which has to be filled by other payloads
    TUint8 *iTransfNextPayload; //For Transforms 
    TInt iHash_pos;             //Stores a position to insert the hash in PHASE_II
    MIkeDebug&      iDebug;
        
public:
    TBuf8<4096>    iBuf;
    TBool          iError;      //ETrue if any error building the message
    };



#endif // IKEV1ISAKMPSTREAM_H
