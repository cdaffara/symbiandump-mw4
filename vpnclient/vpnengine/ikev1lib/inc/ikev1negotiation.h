/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: IKEv1 negotiation.
*
*/

#ifndef C_IKEV1NEGOTIATION_H
#define C_IKEV1NEGOTIATION_H

#include <e32base.h>

#include "ikev1payloads.h"
#include "ikev1extra.h"
#include "ikev1SAdata.h"
#include "ikepolparser.h"
#include "ipsecsaspiretriever.h"
#include "ikev1dialog.h"


#define INITIATOR   0
#define RESPONDER   1

#define PHASE_I     1   //Used to identify the PHASE number
#define PHASE_II    2

#define OAKLEY_DEFAULT_NONCE_SIZE   128/8   //128 bits = 16 bytes
#define ISAKMP_HEADER_SIZE 28
#define ISAKMP_HASH_SIZE    24      //Size for 3des_cbc return value
#define MAX_PRF_LENGTH  24  //In bytes corresponds to 3DES_CBC
#define MAX_RETRANS_TIMER   10  //Isakmp packet retransmission timer
#define MAX_RETRANS_COUNT    7 //Maximum retransmission count

//Vendor ID type codes values
#define HASH_VENDOR_ID      0   //Contains only a vendor specific hash
#define EXPANDED_VENDOR_ID  1   //Contains NAT probing data and a vendor specific hash
#define IETF_NATT_VENDOR_ID 2   //Vendor ID for draft <draft-ietf-ipsec-nat-t-ike-03>
#define IETF_RFC_NATT_VENDOR_ID 3

const TInt KMaxDpdRetryCount( 2 );

struct TSPINode
{
	TInt iPropNum;  //Proposal #
	TUint32 iSeq;   //Sequence # in the GETSPI msg
	TUint32 iSPI;   //Received SPI
};

class TPfkeyMessage;
class CIkev1PluginSession;
class CAuthDialogInfo;
class CIkev1Timeout;
class CX509Certificate;
class CIkeKeys;
class CIkeV1PkiService;
class TSAISAKMP;
class TPayloadISAKMP;
class TKeyISAKMP;
class TIdentISAKMP;
class TCertificateISAKMP;
class TSignatureISAKMP;
class THashISAKMP;
class TNotificationISAKMP;
class TVendorISAKMP;
class TINTNETISAKMP;
class TProposalISAKMP;
class CIkev1NatDiscovery;
class CIKECRACKNegotiation;
class CTransNegotiation;
class TIkev1IsakmpStream;
class MIkeDebug;
class MKmdEventLoggerIf;
class CPFKeySocketIf;

//
//  Class CIkev1Negotiation: Contains all the info for each negotiation in progress
//
NONSHARABLE_CLASS(CIkev1Negotiation) : public CBase,
                                       public MIpsecSaSpiRetrieverCallback,
                                       public MIkeDialogComplete
    {
    friend class CIkev1InfoNegotiation;
    friend class TIkev1IsakmpStream;
    friend class CIKECRACKNegotiation;
    friend class CTransNegotiation;
	friend class CIkev1Payloads;

public:	
    static CIkev1Negotiation* NewL( CIkev1PluginSession* aPluginSession,
                                    CPFKeySocketIf& aPFKeySocketIf,
                                    MIkeDebug& aDebug,
                                    const TInetAddr& aRemote,
                                    const TCookie& aInitiator,
                                    const TCookie& aResponder );  

	static CIkev1Negotiation* NewL( CIkev1PluginSession* aPluginSession,
                                    CPFKeySocketIf& aPFKeySocketIf,
	                                MIkeDebug& aDebug,
	                                const TInetAddr& aRemote,
	                                const TCookie& aInitiator,
	                                TBool aAutoLogin );

	static CIkev1Negotiation* NewL( CIkev1PluginSession* aPluginSession,
                                    CPFKeySocketIf& aPFKeySocketIf,
	                                MIkeDebug& aDebug,
	                                TIkev1SAData* aIkev1SAdata,
                                    TUint aRole,
                                    const TPfkeyMessage *aReq = NULL );
    
    static CIkev1Negotiation* NewL( CIkev1PluginSession* aPluginSession,
                                    CPFKeySocketIf& aPFKeySocketIf,
                                    MIkeDebug& aDebug,
	                                const TInetAddr& aRemote,
	                                TBool aAutoLogin );	

    static CIkev1Negotiation* NewL( CIkev1PluginSession* aPluginSession,
                                    CPFKeySocketIf& aPFKeySocketIf,
                                    MIkeDebug& aDebug,
                                    const TInetAddr& aRemote,
                                    const TPfkeyMessage& aReq ); 

    ~CIkev1Negotiation();
    
    TBool ExecuteL( const ThdrISAKMP& aHdr,
                    const TInetAddr& aRemote,
                    TInt aLocalPort );
    TBool ExecutePhase2L( const ThdrISAKMP& aHdr,
                          const TInetAddr& aRemote,
                          TInt aLocalPort );
    TBool ExecuteTransactionL( const ThdrISAKMP &aHdr,
                               const TInetAddr& aRemote,
                               TInt aLocalPort );  
    void InitNegotiationL();
    void InitPhase2L();
    //used by the timer
    void ReSendL();
    
    inline TUint32 AcqSeq() {return iAcquireSeq;}
	inline TInt GetNotifyStatus() {return iReceivedNotify;}	
	inline void SetNotifyStatus(TInt aStatus) {if (iReceivedNotify == KErrNone) iReceivedNotify = aStatus;}
	inline TBool Autologin() {return iAutoLogin;}
	inline TBool Finished() {return iFinished;}
	inline void SetFinished() {iFinished = ETrue;}
	inline void SetRekeyInfo(CSARekeyInfo* aSARekeyInfo) { iSARekeyInfo = aSARekeyInfo;}			
	inline TUint32 SAId() {return iSAId;}			
	TIkev1IsakmpStream* SaveIkeMsgBfr(TIkev1IsakmpStream* aMsg);
    void AuthDialogCompletedL(CAuthDialogInfo *aUserInfo);
	TInt32 RandomMessageId();
	void SendDeleteL(TUint8 aProtocol, TUint32 aIpsecSPI = 0);   //Send a Delete payload for the negotiation
	void SendKeepAliveMsgL(TIkev1SAData* aSa);

    TBool IsRekeyingIkeSa();    
    void PreparePhase2L(const TPfkeyMessage &aReq);
   
	MKmdEventLoggerIf& EventLogger();
	
// from base class MIpsecSaSpiRetrieverCallback
	
    /**
     * Notification about completion of IPsec SPI SA retrieve.
     * 
     * @param aSpiRequestId Id of the SPI retrieve request.
     * @param aStatus Completion status
     * @param aSpi SPI value
     */    
    void IpsecSaSpiRetrieved(TUint32 aSpiRequestId, 
                             TInt aStatus, 
                             TUint32 aSpi);	
    
    TInt   ProcessUserResponseL(CAuthDialogInfo *aUserInfo);
    TInt   DialogCompleteL(CIkev1Dialog* /*aDialog*/, TAny* aUserInfo, HBufC8* aUsername, HBufC8* aSecret, HBufC8* aDomain);

	
private:

    CIkev1Negotiation( CIkev1PluginSession* aPluginSession,
                       CPFKeySocketIf& aPFKeySocketIf,
                       MIkeDebug& aDebug );
	CIkev1Negotiation( CIkev1PluginSession* aPluginSession,
                       CPFKeySocketIf& aPFKeySocketIf,
	                   MIkeDebug& aDebug,
	                   const TInetAddr& aRemote,
	                   const TCookie& aInitiator );
	CIkev1Negotiation( CIkev1PluginSession* aPluginSession,
                       CPFKeySocketIf& aPFKeySocketIf,
	                   MIkeDebug& aDebug,
	                   const TInetAddr& aRemote );	

    void ConstructL( TBool aAutoLogin );
    //When receiving an acquire
    void ConstructL( const TPfkeyMessage& aReq );
    // To create a negotiation form an existing ISAKMP SA either as negotiator or responder
    void ConstructL( TIkev1SAData* aIkev1SAdata,
                     TUint aRole,
                     const TPfkeyMessage *aReq = NULL );
	
    void GetAcquireDataL(const TPfkeyMessage &aReq);
    void CommonConstructL();
    TBool BuildProposals1L();
    TInt BuildProposals2L();
    //Packet processing functions
    TBool ProcessHeaderL(const ThdrISAKMP &aHdr);
    //received packet processing depending on the stage
    TBool ProcessStage1L(const ThdrISAKMP &aHdr);   //Checks SA from INITIATOR
    TBool ProcessStage2L(const ThdrISAKMP &aHdr);   //Checks SA from RESPONDER
    TBool ProcessStage3MainL(const ThdrISAKMP &aHdr);   //Checks KE,NONCE from INITIATOR
    TBool ProcessStage3AggrL(const ThdrISAKMP &aHdr);//Checks ID,AUTH from RESPONDER
    TBool ProcessStage4L(const ThdrISAKMP &aHdr);   //Checks KE,NONCE from RESPONDER
    TBool ProcessStage5L(const ThdrISAKMP &aHdr);   //Checks ID,AUTH from INITIATOR
    TBool ProcessStage6L(const ThdrISAKMP &aHdr); //Checks ID,AUTH from RESPONDER
    TBool ProcessStage7L(const ThdrISAKMP &aHdr); //CRACK authentication going
    TBool ProcessStage1Phase2L(const ThdrISAKMP &aHdr);//Checks HASH(1),SA,KE,NONCE,ID from INITIATOR
    TBool ProcessStage2Phase2L(const ThdrISAKMP &aHdr);//Checks HASH(2),SA from RESPONDER
    TBool ProcessStage3Phase2L(const ThdrISAKMP &aHdr);//Checks HASH(3) from INITIATOR/RESPONDER
    TBool ProcessCONNECTEDL(const ThdrISAKMP &aHdr);//Checks HASH(INF, NOTIF) from RESPONDER
    TBool ProcessSAL(const TSAISAKMP *aPayload, TUint8 *aRecvProposals);
    TBool ProcessProposalsL(const TPayloadISAKMP *aPayload,TUint32 aLengthLeft, TUint8 *aRecvProposals);
    TInt ProcessTransformsL(const TPayloadISAKMP *aPayload,TUint32 aLengthLeft);
    TInt ProcessTransforms2L(const TPayloadISAKMP *aPayload,CProposal_II *aProp,TUint32 aLengthLeft);
    TInt ProcessAttributesL(const TPayloadISAKMP *aPayload,TAttrib *aAttrib); //checks and return the SA attributes
    TInt ProcessAttributes2L(const TPayloadISAKMP *aPayload,TAttrib_II *aAttrib,TUint8 aProtocol); //checks and return the SA attributes
    TBool ProcessKeyL(const TKeyISAKMP *aKey);   //check payload and stores public key value
    TBool ProcessNonceL(const TPayloadISAKMP *aPayload); 
    TBool ProcessStage1_II_IDsL(const TIdentISAKMP *aInit_ID_payload,const TIdentISAKMP *aResp_ID_payload, CProposal_IIList *aRecv_proposals);
    TBool ProcessStage2_II_IDsL(const TIdentISAKMP *aInit_ID_payload,const TIdentISAKMP *aResp_ID_payload);//, CProposal_IIList *aRecv_proposals);
    TBool CheckIdentL(const TPayloadISAKMP *aPayload);
    TBool ProcessCertificateReqL(const TCertificateReqISAKMP *aCertReq);
    TBool ProcessCertificateReqArrayL(const CArrayFixFlat<const TCertificateReqISAKMP *> *aCRPayloadArray);
	TBool ProcessCertificateArrayL(CArrayFixFlat<const TCertificateISAKMP *> *aCertArray);	
    TBool ProcessSignatureL(const TSignatureISAKMP *aPayload);
    TBool ProcessHashL(const THashISAKMP *aHashPayload);
    TBool ProcessHash2L(const ThdrISAKMP &aHdr, const THashISAKMP *aHashPayload, TUint aPadding);
    TBool ProcessNotificationL(const TNotificationISAKMP *aNotifPayload);
    TBool ProcessResponderLifetimeL(const TNotificationISAKMP *aNotifPayload);
    TBool ProcessReplayStatus(const TNotificationISAKMP *aNotifPayload);
    TBool ProcessInitialContactL(const TNotificationISAKMP *notif_payload);
    void  ProcessVendorL(CArrayFixFlat<const TVendorISAKMP*>* aVids);
    void  ProcessIntAddrL(const TINTNETISAKMP *aIntnetPayload);

    TBool Phase_IExchangeL(const ThdrISAKMP &aHdr);
    TBool Phase_IIExchangeL(const ThdrISAKMP &aHdr);
    void QuickModeReplyL();
    void MainModeReplyL();
    void AggressiveReplyL();

    //Certificate reading functions
    TBool ReadCAsL(CArrayFixFlat<TCertInfo*> *aCAList);  
    TBool ReadOwnCertL();  
    
    TCookie CreateCookieL() const; 
    TBool CheckCookies(const TCookie& aInit, const TCookie& aResp);
    TBool CheckPayloadCode(TUint8 aPayload);
    TBool CheckVersionL(TUint8 aVersion);
    TBool CheckExchangeTypeL(TUint8 aType);
    TBool CheckFlagsL(TUint8 aFlags);
    TBool CheckMessageIdL(TUint32 aId);
    TBool CheckDOI(TUint32 aDOI);
    TBool CheckSituationL(TUint32 aSIT);
    TBool CheckGenericPayloadL(const TPayloadISAKMP *aPayload);
    TBool CheckProtocolL(TUint8 aProtocol);  //checks if protocol supported
    TBool CheckSPIL(const TProposalISAKMP *aProposal);   //checks if the SPI is valid
    TBool CheckTransformID(TUint8 aProtocol,TUint8 aID);

    //SA Attributes checking functions
    TBool CheckEncrAlg(TUint16 aValue);
    TBool CheckHashAlg(TUint16 aValue);
    TBool CheckAuthMethod(TUint16 aValue);
    TBool CheckGroupDesc(TUint16 aValue);
    TBool CheckGroupType(TUint16 aValue);
    TBool CheckGroupPrime(const TUint8* aValue, TUint16 length);
    TBool CheckGroupGen(const TUint8* aValue, TUint16 length);
    TBool CheckGroupCurve(const TUint8* aValue, TUint16 length);
    TBool CheckLifeType(TUint16 aValue);
    TBool CheckLifeDuration(const TUint8* aValue, TUint16 length);
    TBool CheckPRF(TUint16 aValue);
    TBool CheckKeyLength(TUint16 aValue,TUint8 aID,TUint8 aProtocol);
    TBool CheckFieldSize(TUint16 aValue);
    TBool CheckGroupOrder(const TUint8* aValue, TUint16 length);
    TBool CheckEncMode(TUint16 aValue);
    TBool CheckAuthAlg(TUint16 aValue);

    //Certificate payload checks
    TBool CertifyRemoteIdentityL(const TIdentISAKMP *aIdPayload);    
    TInt CheckEncodingL(TUint8 aEncoding);
       

    //Signatures
    TBool VerifySignatureL(CX509Certificate *aCert,TUint8 *iHash, TInt aLength,TUint8 *aSig,TUint aSigLength);
    

    //Diffie-Hellman Exchange Routines
    TBool ComputeDHPublicValueL();
    void ComputeNonceL();

    //Hash and Signature computation
    void ComputeHashrL(TDes8 &aHash);
    void ComputeHash1L(TDes8 &aHash);
    void ComputeHash2L(TDes8& aHash,TInt aStage,const TUint8 *aHashMsg=NULL,TInt aHashMsgLen=0);
    TBool VerifyHash2L(const THashISAKMP *iHash,const TUint8 *aHashMsg=NULL,TInt aHashMsgLen=0);
    TBool VerifyInformationalHashL(const THashISAKMP *aHash,const TPayloadISAKMP *aPayload, TUint32 iMessageId);
    void ComputeHashInfL(TDes8& aHash,const TUint8 *aHashMsg,TInt aHashMsgLen);

    //Send a Notification packet informing of an error.
    void SendNotifyL(TUint16 aError);

    void CheckSendResponderLifetime(TIkev1IsakmpStream &aMsg);
    static TPtrC ExchangeType(TUint8 aExchange);
    static void TextPayload(TDes &aBuf, TUint8 aPayload);
    static TPtrC TextNotifyType(TUint16 aNotif);

    //PFKEY related functions
    void AcquireSAErrorResponse(TInt aError);
    void GetSPIL();
    void ReceiveSPIL(TUint32 aSPI, TUint32 aSeq);

    void UpdateSADatabaseL();

    void ComputeLifetimes_II(const TDesC8 &aLifetime, const TDesC8 &aLifesize, TInt64 &aTime, TInt64 &aBytes);

    //CRACK related functions
    TBool StartCRACKAuthL();

    TBool IsakmpPhase1CompletedL(); 

    //Socket
    void SendL(TIkev1IsakmpStream &aMsg);

    inline TUint Role() {return iRole;}
    inline CIkev1PluginSession* PluginSession()  {return iPluginSession;}
    TInt HashLength();  //Output size for PRF algorithm
    
    TUint32 ISAKMPEncrKeyLength(TUint8 aAlgId) const;   //in bytes
    TUint32 HMAC_KeyLength(TUint8 aId) const;
    
    //builds the correct key using the values in iNegotiation
    TBool ComputeKeysL();
    void ComputeKeys2L(const CProposal_II *aProp, TInt aKeyLen, TSPINode &aInboundSpiNode, TDes8& aOutboundKey_II, TDes8& aInboundKey_II);
    void ComputePRFL(TDes8 &prf_output, const TDesC8 &prf_key, const TDesC8 &prf_data);
    TBool InitIVL();    //Initial IV value
    TBool ComputeIVL(TDes8 &aIV, TInt32 aMessageId); //subsequent IV computations    
    //Encryption routines
    void AppendAttributeError(TInt aErr, TDes &aBuf) const;
    void SaveISAKMPSAL();
    void CreateChosenProposalL(CProposal_IIList* aPropList, TInt aPropNum, CTransModifierList *aTransArray);
	TBool ExamineRemoteIdentity(const TDesC8& aRemoteIdInPolicy);
	TBool ProcessIdentityData(const TDesC8& aIdentity, TUint8* aToIdType, TInetAddr* aToIpAddr1, TInetAddr* aToIpAddr2);

	void DpdNotifyMessageReceivedL(TIkev1SAData* aSa, TUint16 aMsgType, TUint32 aSequence);
	TUint32 GetNextSequence(TUint32 aSequence);
	void SendDpdNotifyMessageL(TUint16 aMsgType, TUint32 aSequence);	

    TInt ErrorStatus();
    void SetErrorStatus(TInt aStatus);
    
    void SendAndSaveIkeMsgL( const TDesC8& aIkeMsg,
                             TInetAddr& aDestAddr,
                             TBool aUseNatPort );
    
    TBool IsRetransmit(TLastIKEMsg& aRef);    
    void SaveRetransmitInfo(TLastIKEMsg& aRef);
    void SaveLastMsgL();
    
    HBufC8* GetPskFromPolicyL();
    
    void SetPhase2LifeDurations( const TInt64 aSoftAddTime,
                                 const TInt64 aHardAddTime,
                                 const TInt64 aSoftBytes,
                                 const TInt64 aHardBytes,
                                 TAttrib_II& aAttr_II,
                                 TBool aResponder );    

public:
	TCookie iCookie_I;      // Initiator Cookie (Used with responder to create KEYID)
	TCookie iCookie_R;      // Responder Cookie
	TUint32 iSAId;
	TUint32 iMessageId;     // Message Id. o during Phase I	
	CIkev1Negotiation *iNext;  // A link field to maintain negotiations.	
	
private:
    //Data relevant to the host
    CIkeData *iHostData;

    //Internal Data
    TInetAddr iLocalAddr;
    TInetAddr iRemoteAddr;     // Remote Address   (Gateway or end host)
    

    TUint iRole;            // If we are PHASE I Initiator or Responder
    TUint iPhase;           // Phase I or II
    TUint iStage;           // Negotiation Stage

    //Header Data
    TUint8 iExchange;       // IKE mode in use (Main, agressive,...)
    TUint8 iPrevExchange;   // Used to process the last msg of Phase I

    TUint8  iFlags;         // Own flags to use when sending a msg header
    TUint8  iRecvFlags;     // Received flags in the last msg header
    TBool iCommitBitSet;        // If true the Commit bit has been set by this host
    TUint32 iNotifyMessageId;   // Message Id. for Informational Exchanges
    TLastIKEMsg iLastIKEMsgInfo;// Information of the last received IKE message

    //SA Data
    TUint8 *iSAPayload;     // Initiator SA payload (used in the hash computation). Generic payload NOT included
    TInt iSAPayloadSize;
    TUint32 iDOI;           // Must be IPSEC = 1

    //Proposal/transform list Phase I and II
    TProposal iProposal_I;      // Contains proposed SA attrib for Phase I with it's attr list
    TProposal iChosenProposal_I;
    CProposal_IIList *iProposal_IIList; //Contains the Phase_II Proposal List (PROPOSED)
    CProposal_IIList *iChosenProp_IIList;   //Contains the Phase_II Proposal List (Received from the remote peer)


    TInt iProposalNum;  //Accepted proposed proposal. Needed to know which are the valid SPIs in the next list
    CArrayFixFlat<TSPINode> *iInboundSPIList;
    
    //Keys  (DH Generated public value when own)
    CIkeKeys *iOwnKeys; //Contains own public and private keys
    HBufC8 *iOwnPublicKey;
    TPtrC8 iOwnPublicKey_ptr;
    TBuf8<MAX_PRIME_SIZE> iPeerPublicKey;   //(gxr)
    TBuf8<IKEV1_KEY_MATERIAL_SIZE> iSKEYID;      //KEY used for encryption/decryption of messages
    TBuf8<IKEV1_KEY_MATERIAL_SIZE> iSKEYID_d;    //KEY used to derive keys for non-ISAKMP SAs
    TBuf8<IKEV1_KEY_MATERIAL_SIZE> iSKEYID_a;    //KEY used for authentication of ISAKMP messages
    TBuf8<IKEV1_KEY_MATERIAL_SIZE> iSKEYID_e;    //KEY used for encryption/decryption of ISAKMP messages

    //IV used by des_cbc and des3_cbc is 8 and for AES 16
    TBuf8<IKEV1_MAX_IV_SIZE> iIV;       //normal IV
    TBuf8<IKEV1_MAX_IV_SIZE> iLastIV;   //Saves the last IV of PHASE_I to compute iNotifIV everytime and the first IV in Quick mode
    TInt                     iIVSize;   //Current IV/cipher block size
    
    //Perfect Forward Secrecy
    TBool iPFS;

    //Replay Window Length
    //TInt8 iReplayWindowLength;

    //Nonces
    TBuf8<MAX_PRIME_SIZE> iNONCE_I;
    TBuf8<MAX_PRIME_SIZE> iNONCE_R;

    //IDENTITY info
    TUint8 *iPeerIdentPayload;  //Full Peer ID payload for HASH_I/R computation.Generic payload NOT included
    TInt iPeerIdentPayloadSize;
    TUint8 *iOwnIdentPayload;   //Full own ID payload for HASH_I/R computation.Generic payload NOT included
    TInt iOwnIdentPayloadSize;
    //TUint8 iIDType;
    TUint16 iIDLocalPort;
    TUint16 iIDRemotePort;
    TUint8 iIDProtocol;
    TBool iIDReceived;

	TBool iSwapRemoteIdType;	
    TBool iDefaultRemoteID;     //To avoid updating the Remote ID if a default is being used
    TInetAddr iRemoteAddr1_ID_II;// Remote proxy Address, subnet or initial range
    TInetAddr iRemoteAddr2_ID_II;// Remote proxy mask or end range
    TUint8 iRemoteIDType_II;
    TBool iDefaultLocalID;      //To avoid updating the Local ID if a default is being used
    TInetAddr iLocalAddr1_ID_II;   // Local proxy Address, subnet or initial range
    TInetAddr iLocalAddr2_ID_II;   // Local proxy mask or end range if implemented
    TUint8 iLocalIDType_II;
	
    //
    //IETF NAT Traversal information 
    //
    CIkev1NatDiscovery  *iNatDiscovery;  // IETF NAT traversal control object pointer    
    TUint32 iNAT_D_Flags;           // If not zero, there is NAT between sites
    TInetAddr iLastRemoteAddr;      // Last detected remote address
    TInetAddr iRemoteOriginalAddr;  // Private address of the remote end
    
    //
    //NAT Traversal information 
    //
    TBool     iFamiliarPeer;    // Nokia VPN implementation detected in peer    
    TBool     iNAT_T_Required;  // True when NAT detected between local end and peer

	//
	// Dead peer detection support 
	//
	TBool     iDPDSupported;    // Both ends support DPD 
	
    //
    //Internal address information received from Nokia VPN gateway
    //
    CInternalAddress *iInternalAddr; //Internal address and possible DNS addresses

    //
    // Certificates
    //
    TUint8 iEncoding;
    CX509Certificate *iPeerX509Cert;    //Received X509 certificate (or read in RSA encryption if already stored).
    
    //
    // Trusted CA certificates list 
    //
    HBufC8* iPeerTrustedCA; //Trusted CA (of peer)
    HBufC8* iICA1;  //Level 1 Intermediate certificate
    HBufC8* iICA2;  //Level 2 Intermediate certificate
    HBufC8* iCA;
    HBufC8* iOwnCert;    //Certificate in use

    TBool iSendCert;        //Tells if is Required to send our cert in next stage (Told in a Cert Req.)
    TBool iCertRequested;   //Tells whether we've requested a CERT through a CR

    //PFKEY related data
    TBool iAcquirePending; //Informs if Acquire is pending
    TUint32 iAcquireSeq;    //Seq num received in the acquire msg. will be used for UPDATE
    TUint32 iSeq;           //Current Seq. num will be used in GETSPI
    TUint32 iPfkeyAcquirePID;   //PID in the acquire msg. will be used in GETSPI
    TUint8 iPendingSPI;     // Used for GetSPI and Receive in case there are many SPI to request
    
    //Miscellanious data
    HBufC8* iLastMsg;   // Last message sent (Used for retransmissions).
    CIkev1Timeout *iTimer;     // Timer to retry sending a message
    TInt iRetryNum;         // Number of retry in the current message
    TBool iFinished;        //Tells if the negotiation is finished to destroy it
    TBool iAutoLogin;       //If ETrue, Negotiation started by policy activation
	TInt  iReceivedNotify;  //Status value of (the first) received Notify payload
    TUint32 iLengthLeft;    //Use to check the size of a payload is not bigger than the data left to process

    //References to objects
    CIkev1PluginSession* iPluginSession;    // IKEv1 plugin session
    CIkeV1PkiService* iPkiService;  
    CIKECRACKNegotiation* iCRACKneg;
    CTransNegotiation* iTransactionNeg;
	CSARekeyInfo* iSARekeyInfo;
	TIkev1IsakmpStream* iSavedIkeMsgBfr;
	TBool iVendorIDRfc;
	
	TBool iPhaseIIAfterIkeSaRekey; // Start PhaseII after IKE SA rekeying 
	TInt64 iHardLifetime;
	
	CIpsecSaSpiRetriever* iIpsecSaSpiRetriever;
	CPFKeySocketIf& iPFKeySocketIf;	
	MIkeDebug& iDebug;
	
	HBufC8* iCRACKLAMUserName;
	HBufC8* iCRACKLAMPassword;
    CIkev1Dialog*      iDialog;      // Pending dialog object
    CAuthDialogInfo*   iDialogInfo;  // Dialog info object
    };

#endif // C_IKEV1NEGOTIATION_H
