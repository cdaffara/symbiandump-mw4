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
* Description:  IKEv2 SA data definition
*                Class TIkev2SAData is the IKEv2 SA parameter definition which
*                is used to pass SA information between IKE server and IKEv2
*                plug-in.
*
*/
#ifndef __IKEV2SADATA_H__
#define __IKEV2SADATA_H__
#include <e32def.h>
#include <in_sock.h>
#include "ikemsgheader.h"
#include "internaladdress.h"
#include "ipsecsadata.h"
#include "ipsecsalifetime.h"
#include "ikev2message.h"

class CIkeData;

#define SECOND 1000000  // One second is 1000000 us. (1 us. per tick)

//
// Private generic PFKEY extension type value 
//
#define   IKEV2_KEY_MATERIAL_SIZE   40
#define   IKEV2_MAX_IV_SIZE         16

class TIkev2SAData 
{
public:
    TIkev2SAData();

    void CleanUp();
	void FreeRespMsg();
    void FreeRequestMsg();
	void StoreVirtualIp(const TInetAddr& aVirtualAddr);
	void SaveRespMsg(CIkeV2Message* aRespMsg);
	void SaveRequestMsg(CIkeV2Message* aRequestMsg);
	void Copy(TIkev2SAData& aSrc);
    TUint32 SaId() const; 
    void SetSaId(TUint32 aSaId);
    TIkeSPI& SpiI();
    void SetSpiI(const TIkeSPI& aSpiI);
    TIkeSPI& SpiR();
    void SetSpiR(const TIkeSPI& aSpiR);

    void GenerateIkeKeyDerivatesL(const TDesC8& aSKEYSEED,TUint16 aPrfAlg, 
                                  const TDesC8& aNonceI, const TDesC8& aNonceR);
    
    /**
     * Gets the request message ID we should use in our next
     * request.
     */
    TUint32 NextRequestId() const;
    
    /**
     * Get the message ID we expecting see
     * in a next received response.
     */    
    TUint32 ExpectedResponseId() const;
    
    /**
     * Get the message ID we expecting see
     * in a next received request. We should
     * also use this message ID in our
     * corresponding response. 
     */
    TUint32 ExpectedRequestId() const;
    
    
private:
	TUint32 iSAId;          // Internal negotiation Id
	TIkeSPI iSPI_I;         // Initiator SPI 
	TIkeSPI iSPI_R;         // Responder SPI

public:	

	TInt    iSAState;       // IKE SA State
	TBool   iInitiator;     // TRUE if local end is initiator
	
	CIkeData* iIkeData;     
	TUint32   iVpnIapId;    
	
	TInetAddr iLocalAddr;   // The address of the physical interface we are using.
	TInetAddr iVirtualAddr; // The address assigned to the VPN interface by the SGW.
	TInetAddr iRemoteAddr;  // Remote Address ("From Policy")
	TInetAddr iDestinAddr;  // Current peer destination address and port.
	                        // (Is different that iRemoteAddr, if the SGW is behind NAT)
    
	TUint32   iNATFlags;    // If not zero, there is NAT between sites
	TBool     iFloatedPort; // If true floated port used (and NON-ESP-MARKER)

	TUint32 iWindowSize;    // Message ID window size (currently 1)
	CIkeV2Message* iLastResponse; // The last IKE response message buffer
	CIkeV2Message* iLastRequest; // The last IKE response message buffer
	TInt    iRespRetryCount;// Count of response retries tranmitted in sequence
	
	//
	// Selected IKE SA proposal 
	//
	TUint16 iEncrAlg;       // Encryption algorithm (transform ID 1)
	TUint16 iPRFAlg;        // Pseudo Random function (transform ID 2)
	TUint16 iIntegAlg;      // Integrity algorithm  (transform ID 3)
	TUint16 iDHGroup;       // Diffie Hellmann Group(transform ID 4)
	
	TUint16 iEAPType;       // EAP type if any	
	TUint16 iAuthMethod;    // Authentication method selected
	TUint32 iLifetime;      // Local lifetime in seconds
	TInt    iCipherKeyLth;  // Cipher key length
	TInt    iCipherBlkLth;  // Cipher block length
	TInt    iIntChkSumLth;  // Integrity checksum length
	TBool   iMobikeUsed;    // MOBIKE protocol supported by both ends
    //
	// IKEv2 keymaterial
	//
	TBuf8<IKEV2_KEY_MATERIAL_SIZE> iSK_d;
	TBuf8<IKEV2_KEY_MATERIAL_SIZE> iSK_ai;
	TBuf8<IKEV2_KEY_MATERIAL_SIZE> iSK_ar;	
	TBuf8<IKEV2_KEY_MATERIAL_SIZE> iSK_ei;
	TBuf8<IKEV2_KEY_MATERIAL_SIZE> iSK_er;	
	TBuf8<IKEV2_KEY_MATERIAL_SIZE> iSK_pi;
	TBuf8<IKEV2_KEY_MATERIAL_SIZE> iSK_pr;	
};


//
//  IKE SA states
//
const TInt KSaStateNotDefined     = 0;
const TInt KSaStateReady          = 1;
/*
const TInt KSaStateRekeying       = 2;
const TInt KSaStateWaitingChildSa = 3;
const TInt KSaStateDeleting       = 4;
const TInt KSaStateNotifying      = 5;
const TInt KSaStateRoaming        = 6;
*/

#endif
