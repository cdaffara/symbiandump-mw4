/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   "intermediate" class between
*       CIkev2Negotiation class of ikev2plugin and ECOM plug-in
*
*/
#ifndef __IKEV2EAPINTERFACE_H_
#define __IKEV2EAPINTERFACE_H_

#include <e32base.h>
#include <abs_eap_vpn_if.h> // MAbsEapVpnInterface

class CIkeData;
class TPayloadIkev2;
class CEapVpnInterface;
class MIkeDebug;

class MIkev2EapIfObserver
    {
public:
    virtual void SendEapDataL(HBufC8* aEapData) = 0;
    virtual void EapEventL(TInt aEvent) = 0;
    };

/**
 *  CIkev2EapIf
 *
 *  CIkev2EapIf is an "intermediate" class between
 *  CIkev2Negotiation class of ikev2plugin and ECOM plug-in (eapplugin)
 *  which provides access into general EAP protocol implementation in
 *  system. CIkev2EapIf implements also the callback functions required
 *  by the EAP ECOM plug-in.
 *
 *  @lib internal ikev2lib.lib
 *  @since S60 3.0
 */
NONSHARABLE_CLASS(CIkev2EapIf) : public CBase, public MAbsEapVpnInterface
    {
public:

    /**
     * NewL
     *
     * @since S60 3.0
     * @param aNeg IKE negotiation. Must be != NULL. Ownership not taken.
     * @param aEapType EAP type.
     * @param aIkeData Ref to IKE data. Ownership not taken.
     * @param aDebug Debug trace interface 
     * @return self
     */
    static CIkev2EapIf* NewL(MIkev2EapIfObserver& aEapIfObserver, 
                             TUint8 aEapType, 
                             CIkeData* aIkeData, 
                             MIkeDebug& aDebug);
     
    ~CIkev2EapIf();
    inline TInt Status() {return iErrorStatus;}
    inline HBufC8* Identity()
        {
        HBufC8* Id = (HBufC8*)iIdentity;
        iIdentity  = 0;
        return Id;
        }
    inline HBufC8* MSK()
        {
        HBufC8* msk = (HBufC8*)iMSK;
        iMSK = NULL;
        return msk;
        }
    void ErrorStopL() {};
    void EapDataInbound(TPayloadIkev2* aEapPayload);
    void QueryIdentity();       
    
    /**
     * From MAbsEapVpnInterface.
     * Implementation of pure virtual methods
     */   
    void EapOutboundL(HBufC8* aResponse);
    void EapIdentityResponseL(HBufC8* aIdentity);
    void EapSharedKeyL(HBufC8* aSharedKey);
    void EapIndication(TNotification aNotification);    
    
private: // implementation

    CIkev2EapIf(MIkev2EapIfObserver& aEapIfObserver, TUint8 aEapType, MIkeDebug& aDebug);
    void ConstructL(CIkeData* aIkeData);

private: // data
    MIkev2EapIfObserver& iEapIfObserver;   
    CEapVpnInterface* 	 iEapPlugin;     // Own: ECOM plug-in
    HBufC8*              iIdentity;      // Identity buffer
    HBufC8*              iMSK;           // Preshared key
    TInt                 iErrorStatus;   // Error status, if construct fails
    TUint8               iEapType;
    TUint8               iReserved[3];   // Dummy for alignment
    
    /**
     * Debug trace interface.
     * Not own.
     */    
    MIkeDebug&                          iDebug;    
};

//
//  Internal EAP event codes used in IKEv2 plug-in EapEventL() calls
//
const TInt KEapEventSuccess          = 0;
const TInt KEapEventGetIdentity      = 1;
const TInt KEapEventGetPSK           = 2;
const TInt KEapEventFailed           = 3;

#endif // __IKEV2EAPINTERFACE_H_

