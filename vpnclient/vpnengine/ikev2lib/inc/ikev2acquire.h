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
* Description:  IKEv2 Acquire definition
*                Class CIkev2Acquire is a IKEv2 specific data structure
*                containing information needed, when establishing a new 
*                IPsec SA
*/


#ifndef __IKEV2ACQUIRE_H__
#define __IKEV2ACQUIRE_H__

#include <e32base.h>

#include "ipsecsalifetime.h"

class TPfkeyMessage;
class TInetAddr;
struct TIpsecSaSpec;
class CIpsecSARekeyData;
class TIkeV2TrafficSelector;

//
//
//  CIkev2Acquire
//  This class is used to handle PFKEY Acquire primitives received from
//  Ipsec plug-in. 
//

NONSHARABLE_CLASS(CIkev2Acquire) : public CBase
{
    public:
        static CIkev2Acquire* NewL(const TPfkeyMessage& aPfkeyMessage, TUint32 aId, 
                              const TInetAddr& aLocalAddr, TUint16 aDHGroup, TBool aImplicitSa,
                              const TIpsecSaSpec* aSaSpec = 0, const CIpsecSARekeyData* aRekeyData = 0);        
        static CIkev2Acquire* NewL(TUint32 aId, HBufC8* aSa, 
                              CArrayFix<TIkeV2TrafficSelector>* aTS_i, 
                              CArrayFix<TIkeV2TrafficSelector>* aTS_r );
        static void Link(CIkev2Acquire* aAcquire, CIkev2Acquire** aAnchor);
        static CIkev2Acquire* Find(TUint32 aId, CIkev2Acquire** aAnchor, TBool aRemove=EFalse);        
        static void PurgeQue(CIkev2Acquire** aAnchor);
        static TBool Responding(CIkev2Acquire** aAnchor);                
        static CIkev2Acquire* GetNext(CIkev2Acquire** aAnchor, TBool aResponse);
        static void SetFirst(CIkev2Acquire* aAcquire, CIkev2Acquire** aAnchor);
        static CIkev2Acquire* PeekFirst(CIkev2Acquire** aAnchor);      
        static CIkev2Acquire* RemoveFromQue(TUint32 aId, CIkev2Acquire** aAnchor);

        TUint32 Id();
        TPtrC8 SPI_In();
        TPtrC8 SPI_Out();          
        TPtrC8 SPI_ToBeRekeyed();
        void SetSPI_In(const TDesC8& aSPI);
        void SetSPI_Out(const TDesC8& aSPI);
        void SetSPI_ToBeRekeyed(const TDesC8& aSPI);
        TUint16 DHGroup();
        void DHGroup(TUint16 aDHGroup);               
        TBool Transport();
        void SetTransport();       
        TBool Response();
        void SetResponse();
        void SetHardLifetime(const TIpsecSALifetime& aHard );
        TIpsecSALifetime* HardLifetime();
        void SetSoftLifetime(const TIpsecSALifetime& aSoft );
        TIpsecSALifetime* SoftLifetime();
        TUint8 ReplayWindow();
        void SetReplayWindow(TUint8 aReplayWindow);
        TUint32 Pid();
        void SetPid(TUint32 aPfKeyPid);
        TUint32 Seq();
        void SetSeq(TUint32 aPfKeySeq);
        void SetVirtualIp();
        TBool ForVirtualIp();               
        TBool SrcSpecific();
        void SetSrcSpecific(TBool aSrcSpecific);      
        TUint8 IpsecProtocol();
        void SetIpsecProtocol(TUint8 aProtocol);                      

        HBufC8* LocalId();
        HBufC8* RemoteId();      
        HBufC8* SA()const;
        void AddIpsecSpiToSa(const TDesC8& aSpi);
        const CArrayFix<TIkeV2TrafficSelector>& TS_i();
        const CArrayFix<TIkeV2TrafficSelector>& TS_r();
        void ReplaceSA(HBufC8* aSA);
        void ReplaceTS_i(CArrayFix<TIkeV2TrafficSelector>* aTS);
        void ReplaceTS_r(CArrayFix<TIkeV2TrafficSelector>* aTS);
        void ReplaceLocalId(HBufC8* aId);
        void ReplaceRemoteId(HBufC8* aId);

        CIkev2Acquire(TInt aId);
        ~CIkev2Acquire();
        
    private:
        void ConstructL(const TPfkeyMessage& aReq, const TInetAddr& aLocalAddr, TUint16 aDHGroup, TBool aImplicitSa,
                        const TIpsecSaSpec* aSaSpec, const CIpsecSARekeyData* aRekeyData);      

    private:        
        TUint32   iId;            // Unique sequence number for GETSPI
        TBuf8<4>  iSPIIn;         // Ipsec SPI value in (local SPI)
        TBuf8<4>  iSPIOut;        // Ipsec SPI value out (remote SPI)
        TBuf8<4>  iSPIToBeRekeyed;// Ipsec SPI value of IPSec SA to be rekeyed
        TUint32   iDHGroup;       // DH group for PFS       
        TBool     iTransport;     // Transport Mode requested
        TBool     iResponse;      // Object is for Child SA response
        TBool     iSrcSpecific;   // Requested SA is "local address" specific
        TBool     iForVirtualIp;  // This Acquire is just for getting virtual IP    
        TInt      iReplayWindow;  // Ipsec replay window value
        TInt      iProtocol;      // Ipsec protocol
        TUint32   iPfKeyPid;      // Saved from PFKEY Acquire 
        TUint32   iPfKeySeq;      // Saved from PFKEY Acquire 
        TIpsecSALifetime iHard;
        TIpsecSALifetime iSoft;
        HBufC8*   iSA;            // Ipsec SA payload built from Pfkey Acquire
        
        CArrayFix<TIkeV2TrafficSelector>* iTS_i;   // Local Traffic selector info
        CArrayFix<TIkeV2TrafficSelector>* iTS_r;   // Remote Traffic selector info
        HBufC8*   iLocalId;       // Local Id data, if any
        HBufC8*   iRemoteId;      // Remote Id data, if any
        CIkev2Acquire* iNext;          // Next pending acquire
};

#endif /* __IKEV2ACQUIRE_H__ */
