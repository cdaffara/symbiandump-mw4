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
* Description:  IKEv1 SA data definition
*
*/


#ifndef T_IKEV1SADATA_H
#define T_IKEV1SADATA_H

#include "internaladdress.h"
#include "ikemsgheader.h"
#include "ikev1payloads.h"

class CIkeData;

#define   IKEV1_KEY_MATERIAL_SIZE   64
#define   IKEV1_MAX_IV_SIZE         20        //Max hash algorithm output size
#define   DEFAULT_MAX_ISAKMP_LIFETIME 28000   //for security reasons

// Class TIkev1SAData is the IKEv1 SA parameter definition 
// which is used to pass SA information between IKE server and IKEv1
// plug-in.
class TIkev1SAData 
{
public:
    TIkev1SAData()
    :iCookie_I(),
	 iCookie_R(),
     iSAId(0),
     iSAState(0),
     iInitiator(EFalse),
     iDPDSupported(EFalse),
     iFamiliarPeer(EFalse),
     iNAT_T_Required(EFalse),
     iAutoLogin(EFalse),
     iNAT_D_Flags(0),
     iIkeData(NULL),
     iLocalAddr(),
	 iRemoteAddr(),
	 iDestinAddr(),
     iVirtualIp(NULL),
     iSeq(0),
     iPrevExchange(0),
     iFlags(0),
     iEncrAlg(0),
     iHashAlg(0),
     iAuthMethod(0),
     iGroupDesc(0),
     iGroupType(0),
     iKeyLength(0),
     iPRF(0),
     iLifeTimeSecs(0),
     iLifeTimeKB(0),
     iDPDRetry(0),
     iDPDSequence(0),
     iPendingDPDSequence(0),
     iExpectedDPDSequence(0),
     iLastIKEMsgInfo(),
     iLastMsg(NULL)
         {}
	inline void CleanUp()
	{
	    delete iVirtualIp;
        iVirtualIp = NULL;
        delete iLastMsg;
        iLastMsg = NULL;
    }
	inline void StoreVirtualIp(CInternalAddress* aVirtualIp)
	{
		delete iVirtualIp;
		iVirtualIp = aVirtualIp;
	}
	inline void CopyL(TIkev1SAData& aSrc)
	{
	   CInternalAddress* SavedVip = iVirtualIp;
	   HBufC8* SavedLastMsg = iLastMsg;
	   Mem::Copy((TUint8*)&iCookie_I, (TUint8*)&aSrc.iCookie_I, sizeof(TIkev1SAData));
	   iVirtualIp = SavedVip;
	   iLastMsg = SavedLastMsg;
	   if ( aSrc.iVirtualIp )
	       {
	       StoreVirtualIp(CInternalAddress::NewL(*(aSrc.iVirtualIp)));
	       }
	   if ( aSrc.iLastMsg &&
	        aSrc.iLastMsg != iLastMsg )
	       {
	       delete iLastMsg;
	       iLastMsg = aSrc.iLastMsg->AllocL();
	       }
	}

public:
	TCookie   iCookie_I;        // Initiator Cookie
	TCookie   iCookie_R;        // Responder Cookie
	TUint32   iSAId;            // Internal negotiation Id
	
	TInt      iSAState;         // IKE SA State
	TBool     iInitiator;       // TRUE if local end is initiator
	TBool     iDPDSupported;    // Both ends support DPD 	
	TBool     iFamiliarPeer;    // Nokia VPN implementation detected in peer    
	TBool     iNAT_T_Required;  // True when NAT detected between local end and peer
	TBool     iAutoLogin;       // SA negotiated due RKMD::Activate request
	TUint32   iNAT_D_Flags;     // If not zero, there is NAT between sites	
	
	CIkeData* iIkeData;     

	TInetAddr iLocalAddr;   
	TInetAddr iRemoteAddr;      // Remote Address ("From Policy")
	TInetAddr iDestinAddr;      // Current peer destination address and port
	CInternalAddress* iVirtualIp; // Virtual IP address (and DNS addresses)

	TUint32 iSeq;               // For PFKEY API
	TUint8  iPrevExchange;      // Used to process the last msg of Phase I
	TUint8  iFlags;             // Flags in the msg header
	//
	// Selected IKE SA proposal 
	//
	TUint16 iEncrAlg;           //OAKLEY encryption function    
	TUint16 iHashAlg;           //OAKLEY hash function          
	TUint16 iAuthMethod;        //OAKLEY authentication function
	TUint16 iGroupDesc;         //OAKLEY GROUP
	TUint16 iGroupType;         //OAKLEY GROUP type
	TUint16 iKeyLength;         //Encryption key length
	TUint16 iPRF;               //Reserved
	TUint32 iLifeTimeSecs;      //SA lifetime seconds
	TUint32 iLifeTimeKB;        //SA lifetime KiloBytes
	
	//
	// IKE SA DPD (keep alive) protocol parameters
	//
	TInt    iDPDRetry;
	TUint32 iDPDSequence;
	TUint32 iPendingDPDSequence;
	TUint32 iExpectedDPDSequence;
	
	// Last IKE msg info
	TLastIKEMsg iLastIKEMsgInfo;
	HBufC8*     iLastMsg;
	TInetAddr   iLastRemoteAddr;
	
    //
	// IKEv1 keymaterial
	//
	TBuf8<IKEV1_KEY_MATERIAL_SIZE> iSKEYID;      //KEY used for encryption/decryption of messages
	TBuf8<IKEV1_KEY_MATERIAL_SIZE> iSKEYID_d;    //KEY used to derive keys for non-ISAKMP SAs
	TBuf8<IKEV1_KEY_MATERIAL_SIZE> iSKEYID_a;    //KEY used for authentication of ISAKMP messages
	TBuf8<IKEV1_KEY_MATERIAL_SIZE> iSKEYID_e;    //KEY used for encryption/decryption of ISAKMP messages

	TBuf8<IKEV1_MAX_IV_SIZE>       iIV;          //normal IV
	TBuf8<IKEV1_MAX_IV_SIZE>       iLastIV;      //Saves the last IV of PHASE_I to compute iNotifIV everytime and the first IV in Quick mode

};


class CSARekeyInfo : public CBase
{
	public:
		static CSARekeyInfo* NewL(const TCookie& aICookie, const TCookie& aRCookie, 
		                          CInternalAddress* aInternalAddr)
		{
			return new (ELeave) CSARekeyInfo(aICookie, aRCookie, aInternalAddr);
		}
		~CSARekeyInfo()
		{   
		    delete iInternalAddr;
		}
	
		inline const TCookie&  GetCookieI() { return iCookie_I;}  
		inline const TCookie&  GetCookieR() { return iCookie_R;}
		inline CInternalAddress* GetInternalAddr()
		{
			CInternalAddress* VirtualIp = iInternalAddr;
			iInternalAddr = NULL;
			return VirtualIp;
		}		

    private:
   		CSARekeyInfo(const TCookie& aICookie, const TCookie& aRCookie, 
   		             CInternalAddress* aInternalAddr)
   		:iCookie_I(aICookie),
		 iCookie_R(aRCookie),
		 iInternalAddr(aInternalAddr)
   		{}

	private:
		TCookie iCookie_I;      // Initiator Cookie of the IKE SA rekeyed
		TCookie iCookie_R;      // Responder Cookie of the IKE SA rekeyed
		CInternalAddress* iInternalAddr; 
	
};


class TIpsecSPI
{
	public:
		TIpsecSPI()
		:iSrcAddr(), iDstAddr(), iSPI(0), iInbound(EFalse), iProtocol(0)
		    {};
	public:
		TInetAddr iSrcAddr;
		TInetAddr iDstAddr;
		TUint32   iSPI;
		TBool     iInbound;    // Inbound = ETrue
		TUint8    iProtocol;
		TUint8    iReserved[3];
};

#endif // T_IKEV1SADATA_H
