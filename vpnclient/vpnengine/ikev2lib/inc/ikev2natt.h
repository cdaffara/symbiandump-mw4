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
#ifndef _IKEV2NATT_H_
#define _IKEV2NATT_H_

class TNotifPayloadIkev2;
class TInetAddr;

class CIkev2NatT : public CBase
    {
public:
    CIkev2NatT(){};  
    static CIkev2NatT* NewL(const TInetAddr& aSourceAddr, const TInetAddr& aDestinationAddr, TUint16 aPort, 
                            const TDesC8& aInitiatorSpi, const TDesC8& aResponderSpi);
    ~CIkev2NatT() {delete iSrcNotify; delete iDstNotify; }
	
    static TUint32 CheckPeerNotifysL(const CArrayFixFlat<TNotifPayloadIkev2*>& aNotifys, 
                                     const TInetAddr& aLocalAddr, const TInetAddr& aRemoteAddr, TUint16 aPort, 
									 const TDesC8& aInitiatorSpi, const TDesC8& aResponderSpi, TBool& aSupported);
	TPtrC8 SourceNofify() const { return *iSrcNotify;}
	TPtrC8 DestinNofify() const { return *iDstNotify;}
			
private:
	void    ConstructL(const TInetAddr& aSourceAddr, const TInetAddr& aDestinationAddr, TUint16 aPort, 
                       const TDesC8& aInitiatorSpi, const TDesC8& aResponderSpi);
	
	HBufC8* GenerateNatDetectionHashL(const TDesC8& aInitiatorSpi, const TDesC8& aResponderSpi,
                                      TInetAddr aIpAddress, TUint16 aPort) const;
   
    HBufC8*      iSrcNotify;
	HBufC8*      iDstNotify;    
    };

#endif
