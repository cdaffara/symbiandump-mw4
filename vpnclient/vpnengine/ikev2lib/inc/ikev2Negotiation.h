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
* Description:   IKEv2 negotiation
*
*/
#ifndef _IKEV2NEGOTIATION_H_
#define _IKEV2NEGOTIATION_H_

#include <x509cert.h>

#include "ikev2SAdata.h"
#include "ikev2natt.h"
#include "pfkeymsg.h"
#include "ikev2payloads.h"
#include "ikev2mobike.h"
#include "ikev2retransmittimer.h"
#include "ikecrypto.h"
#include "ikemsgrec.h"
#include "kmdapi.h"
#include "ikev2pkiservice.h"
#include "ikecert.h"
#include "ipsecsaspiretriever.h"
#include "ikev2ipsecsadata.h"
#include "ikev2EapInterface.h"

static const TUint16 KMaxSendAttemps = 6;

#define DEF_MSG_ID_WINDOW  1

#define IKEV2_DEF_NONCE_SIZE   (160/8)   //160 bits = 20 bytes
#define IKEV2_MIN_NONCE_SIZE   (128/8)   //128 bits = 16 bytes
#define IKEV2_DEF_LIFETIME     14400   //4 hours

//
//  IKE/IPSEC SA negotiation states
//
const TInt KStateIdle                = 0;
const TInt KStateIkeInitPkiService   = 1;
const TInt KStateIkeSaInitRequest    = 2;
const TInt KStateIkeSaAuthRequest    = 4;
const TInt KStateIkeWaitingId        = 5;
const TInt KStateIkeSaEapStarted     = 6;
const TInt KStateIkeSaEapGoing       = 7;

const TInt KStateIkeSaInitResponse   = 11;
const TInt KStateIkeSaAuthWaitSpi    = 12;
const TInt KStateIkeSaAuthResponse   = 14;


const TInt KStateIkeSaCompleted      = 20;

const TInt KStateIkeChildSARequest   = 21;
const TInt KStateIkeChildSAResponse  = 22;

const TInt KStateIkeSARekeyRequest   = 23;
const TInt KStateIkeSARekeyResponse  = 24;

const TInt KStateIkeInfoRequest      = 31;
const TInt KStateIkeDeleteRequest    = 33;
const TInt KStateIkeDeleteResponse   = 34;
const TInt KStateChildDeleteRequest  = 35;

class CIkev2PluginSession;
class CPFKeySocketIf;
class CIkev2Config;
class CIkeData;
class CIkev2Acquire;
class CIkev2Expire;
class MKmdEventLoggerIf;
class MIkeDebug;
class CIkeV2Message;
class CIkeV2Identity;
class CIkev2MessageSendQueue;

/**
 *  CIkev2Negotiation
 *
 *  @lib internal (ikev2lib.lib)
 */
NONSHARABLE_CLASS(CIkev2Negotiation) : public CBase, 
                                       public MIkeV2PkiServiceObserver,
                                       public MIpsecSaSpiRetrieverCallback,
                                       public MIkev2EapIfObserver,
                                       public MIkev2RetransmitTimerCallback
    {
	friend class Ikev2MobIke;	
	
public:	
	
    /**
     * NewL
     *
     * @since S60 ?S60_version
     * @param aControl Plugin control. Must be != NULL
     *          ownership not taken.     
     * @param aIkeData ?description
     * @param aVpnIapId ?description
     * @param aPhysicalInterfaceAddress IP address of the physical connection with scope.
     * @param aRemote ?description          
     * @return self
     */		    		
	static CIkev2Negotiation* NewL(CIkev2PluginSession& aIkeV2PlugInSession,
                                   CPFKeySocketIf& aPfKeySocketIf,
	                               MKmdEventLoggerIf& aEventLogger,
	                               CIkev2MessageSendQueue& aMessageSendQue,
	                               MIkeDebug& aDebug,
	                               CIkeData* aIkeData,
		                           TUint32 aVpnIapId,
		                           TUint32 aSAId,
		                           TInetAddr aPhysicalInterfaceAddress,
		                           TInetAddr aRemoteAddress);	
    
    /**
     * NewL
     *
     * @since S60 ?S60_version
     * @param aControl Plugin control. Must be != NULL
     *          ownership not taken.     
     * @param aSAId ?description
     * @return self
     */			
	static CIkev2Negotiation* NewL(CIkev2PluginSession& aIkeV2PlugInSession,
                                   CPFKeySocketIf& aPfKeySocketIf,
	                               MKmdEventLoggerIf& aEventLogger,
	                               CIkev2MessageSendQueue& aMessageSendQue,
	                               MIkeDebug& aDebug, 
	                               TIkev2SAData& aIkev2SAdata);	
		
    ~CIkev2Negotiation();
    
	void StartIkeSANegotiationL();
	TBool StartRespondingL(const ThdrISAKMP& aIkeMessage);	
	void StartIkeSADeleteL();	
	void ProcessIkeMessageL(const ThdrISAKMP& aIkeMessage, const TInetAddr& aRemote, TUint16 aLocalPort);	
    void ProcessAcquireL(const TPfkeyMessage& aPfkeyMessage);
	void ProcessExpireL(const TPfkeyMessage& aPfkeyMessage);	
	void StartIpsecSaRekeyingL(const TPfkeyMessage &aPfkeyMsg);	
	void BuildIkeSaRekeyMsgL(TBool aRequest);		
	
    void SendKeepAliveMsgL();

	TBool Stopped();
		
	void CancelOperation();
	
// from base class MIkeV2PkiServiceObserver
    void IkeV2PkiInitCompleteL(TInt aStatus);
    
// from base class MIpsecSaSpiRetrieverCallback    
    void IpsecSaSpiRetrieved(TUint32 aSpiRequestId, 
                             TInt aStatus, 
                             TUint32 aSpi);
    
	
// from base class MIkev2EapIfObserver  
    void SendEapDataL(HBufC8* aEapData);
    void EapEventL(TInt aEvent);

// from base class MIkev2RetransmitTimerCallback
    
    /**
     * From MIkev2RetransmitTimerCallback
     * Request IKE request resend.
     */
    void RetransmitRequest();	
	
private: // implementation
    CIkev2Negotiation(CIkev2PluginSession& aIkeV2PlugInSession, 
                      CPFKeySocketIf& aPfKeySocketIf,
                      MKmdEventLoggerIf& aEventLogger, 
                      CIkev2MessageSendQueue& aMessageSendQue,
                      MIkeDebug& aDebug, 
                      TUint32 aSaId);
    void ConstructL();
    
    TBool ImplicitChildSa();    
    HBufC8* PeekProposedSa();
    HBufC8* GetProposedSa();    
    void SetProposedSa(HBufC8* aSaPl);

    CIkev2Acquire** GetAcquireQue();
    CIkev2Expire** GetExpireQue();      
    TBool RequestsPending();
    void SetNotifyCode(TInt aMsgType);
    TInt GetNotifyCode();
    void StoreNotifyData32(TUint32 aData);              
    void StoreNotifyData16(TUint16 aData);              
    TUint8* NotifyData(TInt& aDataLth);
    TInetAddr GetLocalAddr() const;

	void IkeSaCompletedL();
	void IkeSaFailed(TInt aStatus = KKmdIkeNoResponseErr);
	void IpsecSANegotiatedL();
	void CheckNotifyCodeL(CIkev2Payloads* IkeMsg);	
	void CreateIkeSPI(TIkeSPI& aSPI, TBool aRekey=EFalse);
	void GetNatStatus(TBool aSupported, const TInetAddr& aRemote);
	void GetIpsecSPI(CIkev2Acquire* aAcquire);	

	void GetNonceDataL(TBool aInitiator);
	void GetOwnIdentityL(TBool aEapIdResponse=EFalse);		

	void  LoadEapPluginL();
	TBool InitPkiServiceL();	
	void StartIkeSaInitL();
	void SendIkeAuthMessageL();

	void ContinueIkeNegotiationL();
	void BuildChildSAMessageL(CIkev2Acquire* aAcquire, TBool aInitiator);
	void BuildDeleteRequestL(CIkev2Expire* aExpire);
	
	//Send the ike message. Claims the ownership of the message.
	void SendIkeMsgL(CIkeV2Message* aMsg);	
	
	TBool ProcessIkeSaInitL(CIkev2Payloads* aIkeMsg, const TInetAddr& aRemote);
	TBool ProcessIkeAuthL(CIkev2Payloads* aIkeMsg);
	TBool ProcessChildSaL(CIkev2Payloads* aIkeMsg);
	TBool ProcessInfoMsgL(CIkev2Payloads* aIkeMsg);
	TBool ProcessIkeSARekeyL(CIkev2Payloads* aIkeMsg);	
	TBool ProcessDeletePayloadsL(const CArrayFixFlat<TDeletePlIkev2*>& aDeletes, TBool aRequest);
	TBool ProcessNotifyPayloadsL(const CArrayFixFlat<TNotifPayloadIkev2*>& aNotifys, TBool aRequest, TInt aExchange);	
	TBool ProcessCookieL(const CArrayFixFlat<TNotifPayloadIkev2*>& aNotifys, TBool aRequest);
	void ProcessInvalidKePayloadNotifyL();
	
	void GenerateIkeKeysL(TIkev2SAData* aRekeydSaData=NULL);	

	void SaveSignedDataL(TBool aLocal, const TDesC8& aIkeMsg);
	void AddIdToSignedDataL(TBool aLocal, HBufC8* aSigned, const TDesC8& aIdData);	
	HBufC8* SignAuthDataL(const TDesC8& aAuthData, TUint8 aAuthMethod);
	TBool AddIdAndAuthenticatePeerL(CIkev2Payloads* aIkeMsg);	
	TBool AuthenticatePeerL(TAuthPayloadIkev2* aAuth);
	TBool VerifyPeerCertificateL(CArrayFixFlat<TCertPayloadIkev2*>* aCerts, TIDPayloadIkev2* aId );	
	
	TBool ProcessKeyExchangeL(TKEPayloadIkev2* aKePayload, TUint16 aGroup);		

	void AppendKEPayloadL(CIkeV2Message& aIkeMsg, TUint16 aDHGroup);

	TPayloadIkev2* PadEncrPayload(HBufC8* aMsg, TPayloadIkev2* aEncrPl);	
	TUint16 TotalLength( TPayloadIkev2* aLoad, TPayloadIkev2* aLoad2 );
	TBool CheckPayloadsOrder(CIkev2Payloads* aIkeMsg, TUint8 aExchange, TBool aResponse);
	TPayloadIkev2* BuildI2CertPayload(HBufC8* aMsg, TPayloadIkev2* aPrevPl);
	TPayloadIkev2* BuildI1CertPayload(HBufC8* aMsg, TPayloadIkev2* aPrevPl);
	
	void IpsecSaSpiRetrievedL(TUint32 aSpiRequestId, TUint32 aSpi);
	
    /**
     * Handles IKE datagram resend.
     */
    void DoRetransmitL(TBool aResponse=EFalse);  
	
public:	// Data
	//
	// Header Data (Common with IKEv2 SA)
	//
	TIkev2SAData          iHdr;       // Common negotiation info for IKE SA
	TIkeV2IpsecSAData     iChild;     // Common negotiation info for IPSEC SA	
	CIkev2PluginSession&  iIkeV2PlugInSession;    // IKEv2 plugin engine
	
	
	CIkev2Negotiation*    iNext;  // A link field to maintain negotiations.
	
private: // Data
    
    CPFKeySocketIf&     iPfKeySocketIf;
    MKmdEventLoggerIf&  iEventLogger;
    CIkev2MessageSendQueue& iMessageSendQue;
    MIkeDebug&          iDebug;
    
    CIpsecSaSpiRetriever* iSpiRetriever;
    
    TInt           iState;     // Negotiation State
	TUint32        iSAid_Rekey;// Rekeyed SAid
	TIkeSPI        iSPI_Rekey; // Local SPI for rekeyed IKE SA	
	TBool          iStopped;   // Negotiation failed indicator
	TBool          iDeleteIkeSA;     // A notify message received from peer which requires IKE SA to be deleted
	TBool          iChildSARejected; // A notify message received which indicates that Child SA is not accepted
	 
	TBool          iDeactivateGoing; // Negotiation started due deactivate
	TBool          iCookieReturned;  // Cookie returned to responder
	TBool          iPkiAuthRequired; // Private key signature is required as authentication
	TBool          iEapCompleted;    // EAP authentication completed succesfully
	TBool          iPeerIdInSignedData; // Peer id has already been added to signed data
	TBool          iRekeyCollision;  // Rekey collision occurred
	TBool          iProcessEvents;  // Indicates whether the object is accepting events (not accepting when destructor called)

	CIkev2RetransmitTimer*  iTimer;         // Timer to retransmit IKE request message
	CDHKeys*        iDHKeys;        // Diffie-Hellman calculation object
	CIkev2NatT*     iNatNotify;     // Pointer to IKEv2 NAT Traversal object
	CIkev2Config*   iConfigMode;    // Pointer to IKEv2 Config object
	CIkev2EapIf*    iEapPlugin;     // Pointer to IKEv2 EAP interface object
	CIkeV2PkiService* iPkiService;    // Pointer to PKI service Interface object
	HBufC8*         iSavedSaInit;   // Saved IKE_SA_INIT request (waiting PKI service init)
	HBufC8*         iProposedSA;    // Proposed IKE/IPSEC SA payload content	
	HBufC8*         iDHPublicPeer;  // Diffie-Hellman Public value (peer)	
	HBufC8*         iNonce_I;       // Initiator Nonce 
	HBufC8*         iNonce_R;	    // Responder Nonce
	HBufC8*         iAuthMsgInit;   // Signed octet buffer (for initiator AUTH payload)
	HBufC8*         iAuthMsgResp;   // Signed octet buffer (for responder AUTH payload)
	HBufC8*         iPresharedKey;  // Configured preshared key- or EAP MSK data buffer
		
	CIkeV2Identity* iLocalIdentity;
	CIkeV2Identity* iRemoteIdentity;
	
	CX509Certificate*      iPeerCert;      // Verified peer certificate
	  		
	TUint16        iSendAttempt;
	TInt           iNotifyCode;    // Error status for Notification payload
	TInt           iNotifyDataLth; // Notify data length
	TUint8         iNotifyData[4]; // Short notify data packed into network order

	CIkev2Acquire*      iChildSaRequest;// On going Child SA request
    CIkev2Acquire*      iAcquireFirst;  // Pending PFKEY Acquire request 
	CIkev2Expire*       iExpireFirst;   // Pending PFKEY Expire request
	
	TInt iDHGroupGuess; //Number of DH group guess retries for IKE_SA_INIT resquest
    };
#endif // _IKEV2NEGOTIATION_H_
