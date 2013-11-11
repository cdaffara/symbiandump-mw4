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
* Description:   IKEv1/IPSEC SA negotiation 
*
*/

#include <x509cert.h>
#include <x500dn.h>
#include <random.h>
#include <vpnlogmessages.rsg>

#include "ikev1negotiation.h"
#include "ikedebug.h"
#include "ikev1isakmpstream.h"
#include "ikev1timeout.h"
#include "ikev1payload.h"
#include "ikev1plugin.h"
#include "ikev1crack.h"
#include "ikev1trans.h"
#include "ipsecsadata.h"
#include "ipsecsalifetime.h"
#include "pfkeymsg.h"
#include "kmdapi.h"
#include "ikev1pkiservice.h"
#include "ikev1crypto.h"
#include "ikev1natdiscovery.h"
#include "ikev1private.h"
#include "vpnapidefs.h"
#include "ikepkiutils.h"
#include "vpnclientuids.h"
#include "ikecalist.h"
#include "ikecaelem.h"
#include "ikecert.h"
#include "ikev1pluginsession.h"
#include "ikesocketdefs.h"
#include "kmdeventloggerif.h"
#include "pfkeyextdatautil.h"
#include "ipsecsalist.h"

const TInt KSubjectName              = 1;

/////////////////////////////////////////////////////////////////////////////////
// CIkev1Negotiation related stuff
/////////////////////////////////////////////////////////////////////////////////


//
//Proto negotiation to send a informational payload and be destroyed
//
CIkev1Negotiation* CIkev1Negotiation::NewL( CIkev1PluginSession* aPluginSession,
                                            CPFKeySocketIf& aPFKeySocketIf,
                                            MIkeDebug& aDebug,
                                            const TInetAddr& aRemote,
                                            const TCookie& aInitiator,
                                            const TCookie& aResponder )
    {   	
	CIkev1Negotiation* self = new ( ELeave ) CIkev1Negotiation( aPluginSession,
                                                                aPFKeySocketIf,
                                                                aDebug );
	CleanupStack::PushL( self );
	self->iRemoteAddr = aRemote;
	self->iCookie_I = aInitiator;
	self->iCookie_R = aResponder;     
	self->iExchange = ISAKMP_EXCHANGE_INFO;   
	self->iDOI = IPSEC_DOI;
	self->iTimer = CIkev1Timeout::NewL( *self );	
	CleanupStack::Pop( self );	
	return self;
    }

CIkev1Negotiation* CIkev1Negotiation::NewL( CIkev1PluginSession* aPluginSession,
                                            CPFKeySocketIf& aPFKeySocketIf,
                                            MIkeDebug& aDebug,
                                            const TInetAddr& aRemote,
                                            const TCookie& aInitiator,
                                            TBool aAutoLogin ) 
    {
	CIkev1Negotiation* self = new ( ELeave ) CIkev1Negotiation( aPluginSession,
                                                                aPFKeySocketIf,
	                                                            aDebug,
	                                                            aRemote,
	                                                            aInitiator );
	CleanupStack::PushL( self );
    self->ConstructL( aAutoLogin );
    CleanupStack::Pop( self );
	return self;
    }

CIkev1Negotiation* CIkev1Negotiation::NewL( CIkev1PluginSession* aPluginSession,
                                            CPFKeySocketIf& aPFKeySocketIf,
                                            MIkeDebug& aDebug,
                                            TIkev1SAData* aIkev1SAdata,
                                            TUint aRole,
                                            const TPfkeyMessage *aReq )
    {
    CIkev1Negotiation* self = new ( ELeave ) CIkev1Negotiation( aPluginSession,
                                                                aPFKeySocketIf,
                                                                aDebug );
    CleanupStack::PushL( self );
    self->ConstructL( aIkev1SAdata, aRole, aReq );
    CleanupStack::Pop( self );
    return self;
    }


CIkev1Negotiation* CIkev1Negotiation::NewL( CIkev1PluginSession* aPluginSession,
                                            CPFKeySocketIf& aPFKeySocketIf,
                                            MIkeDebug& aDebug,
                                            const TInetAddr& aRemote,
                                            TBool aAutoLogin ) 
    {
	CIkev1Negotiation* self = new ( ELeave ) CIkev1Negotiation( aPluginSession,
                                                                aPFKeySocketIf,
	                                                            aDebug,
	                                                            aRemote );
	CleanupStack::PushL( self );
    self->iCookie_I = self->CreateCookieL();
    self->ConstructL( aAutoLogin );
    CleanupStack::Pop( self );
	return self;
    }

CIkev1Negotiation* CIkev1Negotiation::NewL( CIkev1PluginSession* aPluginSession,
                                            CPFKeySocketIf& aPFKeySocketIf,
                                            MIkeDebug& aDebug,
                                            const TInetAddr& aRemote,
                                            const TPfkeyMessage& aReq )
    {    
    CIkev1Negotiation* self = new (  ELeave ) CIkev1Negotiation( aPluginSession,
                                                                 aPFKeySocketIf,
                                                                 aDebug,
                                                                 aRemote );
    CleanupStack::PushL( self );
    self->iCookie_I = self->CreateCookieL();
    self->ConstructL( aReq );
    CleanupStack::Pop( self );
    return self;
    }

//
// Destructor
//
CIkev1Negotiation::~CIkev1Negotiation()
    {
    DEBUG_LOG1( _L("CIkev1Negotiation::~CIkev1Negotiation this=0x%x"), this );
    
    
    if ( iRole == INITIATOR &&
         iAcquirePending )
        {
        // Response with error to pending Acquire.
        AcquireSAErrorResponse( KKmdIkeNegotFailed );
        }
    
    delete iIpsecSaSpiRetriever;
    delete iSavedIkeMsgBfr;
    
    if( iPluginSession )
        { 		
	    iPluginSession->RemoveNegotiation( this );  
        }
		
    iCookie_I.FillZ(ISAKMP_COOKIE_SIZE);
    
    if(iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        }

    delete iOwnCert;
    delete iPeerX509Cert;
    delete iPeerTrustedCA;  // Peer trusted CA name 
    delete iICA1;
    delete iICA2;

    delete iPkiService;     // Trusted CA certificate list

    delete[] iSAPayload;
    delete[] iPeerIdentPayload;
    delete iOwnIdentPayload;
    
    //Keys
    delete iOwnKeys;        //structure containing the public and private keys
    delete iOwnPublicKey;   //Public Key

    //Phase_I Proposal
    TAttrib *attr;
    while ((attr = iProposal_I.iAttrList) != NULL)
    {
        iProposal_I.iAttrList = attr->iNext;
        delete attr;
    }

    delete iChosenProposal_I.iAttrList; //Only one transformation chosen

    //Phase_II proposals (include transformations)
    delete iProposal_IIList;

    //Phase_II chosen Proposal
    delete iChosenProp_IIList;
    delete iInboundSPIList;
    delete iCRACKneg;
    delete iTransactionNeg; // Transaction exchange 
    delete iInternalAddr;
    delete iNatDiscovery;
	delete iSARekeyInfo;
	delete iLastMsg;

	delete iDialog;
    delete iDialogInfo;
}


//
// CIkev1Negotiation
//  Constructor for remote initiated negotiation
//
CIkev1Negotiation::CIkev1Negotiation( CIkev1PluginSession* aPluginSession,
                                      CPFKeySocketIf& aPFKeySocketIf,
                                      MIkeDebug& aDebug,
                                      const TInetAddr& aRemote,
                                      const TCookie& aInitiator )
 : iCookie_I( aInitiator ),
   iRemoteAddr( aRemote ),
   iPluginSession( aPluginSession ),
   iPFKeySocketIf( aPFKeySocketIf ),
   iDebug( aDebug )
{

    DEBUG_LOG1( _L("CIkev1Negotiation::CIkev1Negotiation, RESPONDER this=0x%x"), this );
    iCookie_R.FillZ(ISAKMP_COOKIE_SIZE);
    iSAId = iPluginSession->GetSAId();
    iPluginSession->LinkNegotiation(this);         
    iRole = RESPONDER;
    iStage = 1; //next phase for the responder is 2
}

//
//  Constructor for local initiated negotiation
//

CIkev1Negotiation::CIkev1Negotiation( CIkev1PluginSession* aPluginSession,
                                      CPFKeySocketIf& aPFKeySocketIf,
                                      MIkeDebug& aDebug,
                                      const TInetAddr& aRemote )
 : iRemoteAddr( aRemote ),
   iPluginSession( aPluginSession ),
   iPFKeySocketIf( aPFKeySocketIf ),
   iDebug( aDebug )
{
    DEBUG_LOG1( _L("CIkev1Negotiation::CIkev1Negotiation, INITIATOR this=0x%x"), this );
    iCookie_I.FillZ(ISAKMP_COOKIE_SIZE);
    iCookie_R.FillZ(ISAKMP_COOKIE_SIZE);
    iSAId = iPluginSession->GetSAId();
    iPluginSession->LinkNegotiation(this);     
    iRole = INITIATOR;
    iStage = 0;
}

//
//  Constructor for Phase II initiated negotiations
//

CIkev1Negotiation::CIkev1Negotiation( CIkev1PluginSession* aPluginSession,
                                      CPFKeySocketIf& aPFKeySocketIf,
                                      MIkeDebug& aDebug )
 : iPluginSession( aPluginSession ),
   iPFKeySocketIf( aPFKeySocketIf ),
   iDebug( aDebug )
{ 
    DEBUG_LOG1( _L("CIkev1Negotiation::CIkev1Negotiation, this=0x%x"), this );
    iCookie_I.FillZ(ISAKMP_COOKIE_SIZE);
    iCookie_R.FillZ(ISAKMP_COOKIE_SIZE);    
    iSAId = iPluginSession->GetSAId();
    iPluginSession->LinkNegotiation(this); 
}

//Used when creating a negotiation as a RESPONDER!
//Used also when Initiator (= Phase 1 negotiation stated by the policy activation)
//Leaves if error detected
void CIkev1Negotiation::ConstructL(TBool aAutoLogin)
{
    iIpsecSaSpiRetriever = CIpsecSaSpiRetriever::NewL( *this,
                                                       iPFKeySocketIf );

    iAutoLogin = aAutoLogin;
    CommonConstructL();
}


//Used when an Acquire is received
//Leaves if error detected
void CIkev1Negotiation::ConstructL(const TPfkeyMessage &aReq)
{
    iIpsecSaSpiRetriever = CIpsecSaSpiRetriever::NewL( *this,
                                                       iPFKeySocketIf );

    CommonConstructL();     
    GetAcquireDataL(aReq);
}

//Used when initiating a new Phase II negotiation from a negotiated ISAKMP SA.
void CIkev1Negotiation::ConstructL( TIkev1SAData* aSA,
                                    TUint aRole,
                                    const TPfkeyMessage *aReq )
{

    iIpsecSaSpiRetriever = CIpsecSaSpiRetriever::NewL( *this,
                                                       iPFKeySocketIf );

    iPhase = PHASE_II;

    //Almost like common construct
    iTimer = CIkev1Timeout::NewL(*this);    

    //Phase II proposal lists
    iProposal_IIList = new (ELeave) CProposal_IIList(1);

    iRemoteAddr.SetPort(IKE_PORT);

    iSeq = aSA->iSeq;
    iLocalAddr  = aSA->iLocalAddr;
    iRemoteAddr = aSA->iRemoteAddr;         // Remote Address from peer
    iLastRemoteAddr = aSA->iDestinAddr;     // Remote Address last transmitted
    
    iLastIKEMsgInfo = aSA->iLastIKEMsgInfo;
    iLastMsg = HBufC8::NewL(4096);
    if ( aSA->iLastMsg != NULL )
        {        
        *iLastMsg = *aSA->iLastMsg;        
        }
    
    iRole = aRole;          // If we are initiator or responder

    //Header Data
    iCookie_I = aSA->iCookie_I; // Initiator Cookie (Used with responder to create KEYID)
    iCookie_R = aSA->iCookie_R; // Responder Cookie
    iPrevExchange = aSA->iPrevExchange;
    iExchange = IKE_QUICK_MODE;

    iMessageId = RandomMessageId();

    iFlags = aSA->iFlags;           // Flags in the msg header
    
//  iNotifyMessageId = aSA->iHdr.iNotifyMessageId;   // Message Id. for Informational Exchanges

    //SA Data
    iDOI = IPSEC_DOI;
    
    iChosenProposal_I.iProtocol = PROTO_ISAKMP;
    iChosenProposal_I.iProposalNum = 0;
//    iChosenProposal_I.iSPI.Copy(aSA->iChosenProposal_I.iSPI);
    iChosenProposal_I.iNumTransforms = 1;
    iChosenProposal_I.iAttrList = new (ELeave) TAttrib; //allocated now and deleted when destroying the obj
//  *iChosenProposal_I.iAttrList = *aSA->iChosenProposal_I.iAttrList;
    iChosenProposal_I.iAttrList->iEncrAlg = aSA->iEncrAlg;
    iChosenProposal_I.iAttrList->iHashAlg = aSA->iHashAlg;
    iChosenProposal_I.iAttrList->iGroupDesc = aSA->iGroupDesc;
    iChosenProposal_I.iAttrList->iGroupType = aSA->iGroupType;
    iChosenProposal_I.iAttrList->iKeyLength = aSA->iKeyLength;  
    if ( aSA->iLifeTimeSecs )
       iChosenProposal_I.iAttrList->iLifeDurationSecs.Copy((TUint8 *)&aSA->iLifeTimeSecs, sizeof(TUint32)); 
    if ( aSA->iLifeTimeKB )
        iChosenProposal_I.iAttrList->iLifeDurationKBytes.Copy((TUint8 *)&aSA->iLifeTimeKB, sizeof(TUint32));    

 
    iHostData = aSA->iIkeData;
    if ( iHostData->iCAList && iHostData->iCAList->Count() )
    {
        iPkiService = CIkeV1PkiService::NewL(iHostData, iDebug);                                           
    }   
    
    if (aRole == INITIATOR)
    {
        GetAcquireDataL(*aReq); //Data needed from the acquire
    }
    else    //RESPONDER 
    {
        iAcquireSeq = 1;        //msg Seq. number
    }

    //
    //NAT Traversal data
    //
    iFamiliarPeer   = aSA->iFamiliarPeer;
    iNAT_T_Required = aSA->iNAT_T_Required;
    
    //
    //Copy an Internal address object (if exists) to the new negotiation 
    //
    if ( aSA->iVirtualIp )
    {
       iInternalAddr = CInternalAddress::NewL(*(aSA->iVirtualIp));    
    }

    iNAT_D_Flags  = aSA->iNAT_D_Flags;  
    if ( iNAT_D_Flags ) {
       //
       // Use IETF specified NAT traversal
       //
       iNatDiscovery = CIkev1NatDiscovery::NewL(aSA->iNAT_D_Flags);
    }
    
    //Keys  (DH Generated public value when own)
    iSKEYID.Copy(aSA->iSKEYID);
    iSKEYID_d.Copy(aSA->iSKEYID_d);
    iSKEYID_a.Copy(aSA->iSKEYID_a);
    iSKEYID_e.Copy(aSA->iSKEYID_e);
    
    //IV used by des_cbc and des3_cbc is 8 but digest returns 16 bytes for MDx and 20 for SHS (first 8 used)
    iIV = aSA->iIV;     //normal IV
    iIVSize = iIV.Length(); 

    iLastIV = aSA->iLastIV; //Saves the last IV of PHASE_I to compute iNotifIV everytime

    //Nonces
//    iNONCE_I = aSA->iNONCE_I;
//    iNONCE_R = aSA->iNONCE_R;
    //
    // If the ISAKMP SA (=aSA) has been originally negotiated due the policy activation (iAutoLogin = ETrue)
    // The iLocalAddr maybe then undefined in aSA.
    // If local end is acting as an initiator, method GetAcquireDataL updates iLocalAddr in CIkev1Negotiation object.
    // In that case take iLocalAddr value from there and store it to CIsakmpSa (aSA) object.
    // If local end is acting as a responder, resolve iLocalAddr value.
    //
    if ( aSA->iAutoLogin )
    {
        if ( aRole == INITIATOR)    
             iLocalAddr = aSA->iLocalAddr;
        else User::LeaveIfError( iPluginSession->GetLocalAddress( iLocalAddr ) );
    }
    iDPDSupported = aSA->iDPDSupported;
    
    iSAId = aSA->iSAId;    // Reference to existin ISAKMP SA
    iStage = 1; //if initiator the stage will be set in a subsequent call to InitPhase_IIL()
}

//Leaves if error detected
void CIkev1Negotiation::CommonConstructL()
{
    iPhase = PHASE_I;
    iTimer = CIkev1Timeout::NewL(*this);    

    //Phase II proposal lists
    iProposal_IIList = new (ELeave) CProposal_IIList(1);

    iRemoteAddr.SetPort(IKE_PORT);
    
    iHostData = &iPluginSession->IkeData();
    if (!iHostData)
    {
        DEBUG_LOG(_L("The host has no data. Negotiation aborted"));
        User::Leave(KKmdIkeNoPolicyErr);
    }   

	if ( iHostData->iCAList && iHostData->iCAList->Count() )
	{
		iPkiService = CIkeV1PkiService::NewL(iHostData, iDebug);
		TInt stat ( EFalse );
		stat=ReadCAsL(iHostData->iCAList);  // Build trusted CA list
		if ( !stat )
		{
			SetErrorStatus( KVpnErrInvalidCaCertFile );
			User::Leave(KVpnErrInvalidCaCertFile);
		}
	}	
    iSendCert = iHostData->iAlwaysSendCert; //If true will always be sent, otherwise only with a CR

    //Proposals
    if (!BuildProposals1L())
    {
        SetFinished();
        User::Leave(KKmdIkePolicyFileErr); //if any error returns
    }

    iEncoding = X509_CERT_SIG;  //Only type in use

    iChosenProposal_I.iAttrList = new (ELeave) TAttrib; //allocated now and deleted when destroying the obj   
    User::LeaveIfError( iPluginSession->GetLocalAddress( iLocalAddr ) );

    //PFKEY data (is overwritten if initiator because we use the data given in the acquire

    iAcquireSeq = 1;//msg Seq. number of acquire needed for the UPDATE
    iSeq = 1;       //Sequence number for pfkey messages

	iLastRemoteAddr = iRemoteAddr;   // Used as destination address when sending data
#ifdef _DEBUG	
	TBuf<40> txt_addr;
    iRemoteAddr.OutputWithScope(txt_addr);
#endif            
    
    iLastMsg = HBufC8::NewL(4096);
    
    DEBUG_LOG1( _L("New negotiation with Host %S"),
            &txt_addr );

    if ( !iHostData->iUseNatProbing && iHostData->iEspUdpPort == 0 )
	{
       //
       // Use IETF specified NAT traversal
       //
       iNatDiscovery = CIkev1NatDiscovery::NewL(0);   
    }
	
	if ( iRole == RESPONDER )
	{
	   //
	   // Get base value internal address (=VPN virtual IP)
	   //
	   iInternalAddr = iPluginSession->InternalAddressL();
	}	
}


TBool CIkev1Negotiation::ReadCAsL(CArrayFixFlat<TCertInfo*> *aCAList)
{

	TBool Status = iPkiService->ImportCACertsL(aCAList);

#ifdef _DEBUG	
	if ( !Status )
	    {
	    DEBUG_LOG( _L("Trusted CA list creation failed!") );
	    }
#endif // _DEBUG
	
	return Status;
			   
}

TBool CIkev1Negotiation::ReadOwnCertL()
{
    //
    // Read own certificate from PKI store using own trusted CA as
    // specified issuer
    //
    TBool Status = EFalse;
    _LIT(KVpnApplUid, "101F7993");
    if ( iPkiService && iHostData->iCAList )
		{
       TCertInfo* CertInfo;
	   HBufC8* CAName = HBufC8::NewLC(256);  	   
       TInt i = 0;
       if ( iHostData->iCAList->At(0)->iData!=KVpnApplUid )
          {
	       while ( i < iHostData->iCAList->Count() )
			   {

	           CertInfo = iHostData->iCAList->At(i);
	           CAName->Des().Copy(CertInfo->iData); // Assure that CA name is in ASCII format
               if ( iPkiService->ReadUserCertWithNameL(CAName->Des(), iHostData, EFalse) == KErrNone )
				   {
				   Status = ETrue;			   
				   delete iOwnCert;  // delete old if exists			   
				   iOwnCert = iPkiService->GetCertificate();
				   delete iPeerTrustedCA;  // for sure
				   iPeerTrustedCA = iPkiService->GetTrustedCA();
                   CleanupStack::PopAndDestroy(CAName);   //CAName
                   CAName=NULL;
				   break;
				   }
               i ++;           
               }
              if (!Status)
                  {
                  CleanupStack::PopAndDestroy(CAName);
                  CAName=NULL;
                  }
           }
       else
           {
           CIkeCaList* trustedCaList = iPkiService->CaList();
           CleanupStack::PopAndDestroy(CAName);        
           CAName=NULL;
           while ( i < trustedCaList->Count() )
               {
               
               CIkeCaElem* CaElem = (*trustedCaList)[i];                     
               CAName = IkeCert::GetCertificateFieldDERL(CaElem->Certificate(), KSubjectName);
               if (CAName == NULL)
                  {
                   User::Leave(KErrArgument);
                  }
               CleanupStack::PushL(CAName);
               if ( iPkiService->ReadUserCertWithNameL(*CAName, iHostData, ETrue) == KErrNone)
                   {
                   Status = ETrue;             
                   delete iOwnCert;  // delete old if exists               
                   iOwnCert = iPkiService->GetCertificate();
                   delete iPeerTrustedCA;
                   iPeerTrustedCA = iPkiService->GetTrustedCA();
                   CleanupStack::PopAndDestroy(CAName);        
                   CAName=NULL;
                   
                   
                   break;
                   }
               i ++;
               
               CleanupStack::PopAndDestroy(CAName);
               CAName=NULL;
               } 
           }
       if ( !Status )
		   {	   
          DEBUG_LOG( _L("Error loading Own Certificate!") );
		   }		   
    }
    return Status;
}



void CIkev1Negotiation::GetAcquireDataL(const TPfkeyMessage &aReq)
{
    // ACQUIRE Contains: <Base, Addr(Src & Dest) (Addr(Proxy)), (Ident(S & D)), (Sensitivity), Proposal>
    
    iLocalAddr = *aReq.iSrcAddr.iAddr;    //Copies our own address because it's the only way to know it

    //Phase II proposals
    //Only one combination received so only one transform
    CProposal_II *prop_II = new (ELeave) CProposal_II();
    CleanupStack::PushL(prop_II);
    prop_II->ConstructL();
    iProposal_IIList->AppendL(prop_II);
    CleanupStack::Pop();            //prop_II safe in case of leave

    prop_II->iProposalNum   = FIRST_IPSEC_PROPOSAL;
    prop_II->iNumTransforms = 1;
    prop_II->iReplayWindowLength = aReq.iProposal.iExt->sadb_prop_replay;
    TAttrib_II *attr_II = new (ELeave) TAttrib_II();
    CleanupStack::PushL(attr_II);
    prop_II->iAttrList->AppendL(attr_II);   //added into the proposal so it's safe if function leaves
    CleanupStack::Pop();            //attr_II safe in case of leave

    attr_II->iTransformNum = FIRST_IPSEC_TRANSFORM;

    if (aReq.iBase.iMsg->sadb_msg_satype==SADB_SATYPE_AH)
    {
        prop_II->iProtocol = PROTO_IPSEC_AH;
        if (aReq.iProposal.iComb->sadb_comb_auth == SADB_AALG_MD5HMAC)
        {
            attr_II->iTransformID = AH_MD5;
            attr_II->iAuthAlg = DOI_HMAC_MD5;
        }
        else if (aReq.iProposal.iComb->sadb_comb_auth == SADB_AALG_SHA1HMAC)
        {
            attr_II->iTransformID=AH_SHA;
            attr_II->iAuthAlg = DOI_HMAC_SHA;
        }
        else
        {
            DEBUG_LOG(_L("Unsupported Authentication Algorithm in Acquire"));
			SetFinished();					
            return;
        }
        // No auth with variable encryption
    }
    else if (aReq.iBase.iMsg->sadb_msg_satype == SADB_SATYPE_ESP)
    {
        prop_II->iProtocol=PROTO_IPSEC_ESP;
        /* Request ESP from peer host */

        attr_II->iTransformID = aReq.iProposal.iComb->sadb_comb_encrypt;
		switch ( attr_II->iTransformID )
		{
			case ESP_DES_CBC:
			case ESP_3DES_CBC:
			case ESP_NULL:
				break;
				
			case ESP_AES_CBC:
				attr_II->iKeyLength = aReq.iProposal.iComb->sadb_comb_encrypt_maxbits;
				break;

			default:
				DEBUG_LOG(_L("IPsec Encryption algorithm is not implemented. Wrong algorithms file"));
				SetFinished();
				return;
				
		}	
        if (aReq.iProposal.iComb->sadb_comb_auth != SADB_AALG_NONE)
        {
            if (aReq.iProposal.iComb->sadb_comb_auth == SADB_AALG_MD5HMAC)
                attr_II->iAuthAlg = DOI_HMAC_MD5;
            else if (aReq.iProposal.iComb->sadb_comb_auth == SADB_AALG_SHA1HMAC)
                attr_II->iAuthAlg = DOI_HMAC_SHA;
            else
            {
                DEBUG_LOG(_L("Unsupported Authentication Algorithm in Acquire"));
				SetFinished();
				return;
            }
        }
    }
    
    //Check if PFS in use...
    if (aReq.iProposal.iComb->sadb_comb_flags & SADB_SAFLAGS_PFS)
    {
        iPFS=ETrue;
        switch (iHostData->iGroupDesc_II)
        {
        case IKE_PARSER_MODP_768:
            attr_II->iGroupDesc = MODP_768;
            break;
        case IKE_PARSER_MODP_1024:
            attr_II->iGroupDesc = MODP_1024;
            break;
        case IKE_PARSER_MODP_1536:
            attr_II->iGroupDesc = MODP_1536;
            break;
        case IKE_PARSER_MODP_2048:
            attr_II->iGroupDesc = MODP_2048;
            break;
        default:
            DEBUG_LOG(_L("ISAKMP Proposals error (Bad Group description)"));
			SetFinished();
			return;
        }
    }
    else
    {
        iPFS=EFalse;
        attr_II->iGroupDesc = 0;    //No group assigned because no PFS
    }

    if (aReq.iProposal.iComb->sadb_comb_flags & SADB_SAFLAGS_TUNNEL)
    {
        attr_II->iEncMode = DOI_TUNNEL;
    }
    else
    {
        attr_II->iEncMode = DOI_TRANSPORT;
    }

    iIDLocalPort = (TUint16)aReq.iSrcAddr.iAddr->Port();
    iIDRemotePort = (TUint16)aReq.iDstAddr.iAddr->Port();
    iIDProtocol = aReq.iDstAddr.iExt->sadb_address_proto;   //May be needed for phase II ID

    //Source Identity
    if (aReq.iSrcIdent.iExt)
    {
        if (aReq.iSrcIdent.iExt->sadb_ident_type == SADB_IDENTTYPE_PREFIX)
        {
			if ( !ProcessIdentityData(aReq.iSrcIdent.iData, &iLocalIDType_II,
								      &iLocalAddr1_ID_II, &iLocalAddr2_ID_II) )
			{
				SetFinished();
				return;
			}	
		}
        else    //Invalid identity type
        {
            DEBUG_LOG1(_L("Invalid Local identity type (%d)"), aReq.iSrcIdent.iExt->sadb_ident_type);
			SetFinished();
			return;
        }
    }
    else
    {
        //We need to assign a default address if other info is to be sent in the same payload
        if (attr_II->iEncMode == DOI_TUNNEL || iIDLocalPort != 0 || iIDRemotePort != 0 ||
            iIDProtocol != 0 || aReq.iDstIdent.iExt)
        {
            DEBUG_LOG(_L("Local Identity not defined and needed. Using Own address as local identity."));
            iLocalAddr1_ID_II = iLocalAddr;
            if ((iLocalAddr.Family() == KAfInet) || iLocalAddr.IsV4Mapped() )
                iLocalIDType_II = ID_IPV4_ADDR;
            else
                iLocalIDType_II = ID_IPV6_ADDR;
            iDefaultLocalID = ETrue;    //Must be sent but won't be used when updating the SAD
        }
    }

    //Destination Identity
    if (aReq.iDstIdent.iExt)
    {
        if (aReq.iDstIdent.iExt->sadb_ident_type == SADB_IDENTTYPE_PREFIX)
        {
			if ( !ProcessIdentityData(aReq.iDstIdent.iData, &iRemoteIDType_II,
									  &iRemoteAddr1_ID_II, &iRemoteAddr2_ID_II) )
			{
				SetFinished();
				return;
			}
        }
        else    //Invalid identity type
        {
            
            DEBUG_LOG1( _L("Invalid Destination identity type (%d)"), aReq.iDstIdent.iExt->sadb_ident_type );
			SetFinished();
			return;
        }
    }
    else
    {
        //
        // RemoteID_II is required only if LocalID_II already exists
		// If transports mode
        //      Build RemoteID_II for Quick mode negotiation from specified remote IP address
        // else Use subnet 0/0 as remote id
		//
        if ( iLocalIDType_II != 0 )
        {
		   if (	attr_II->iEncMode == DOI_TUNNEL ) {
			  if ( iLocalIDType_II == ID_IPV4_ADDR || iLocalIDType_II == ID_IPV4_ADDR_SUBNET ) {
				 iRemoteAddr1_ID_II.Init(KAfInet);
				 iRemoteAddr2_ID_II.Init(KAfInet); 				 
				 iRemoteIDType_II = ID_IPV4_ADDR_SUBNET;
			  }	   
			  else {
			     iRemoteAddr1_ID_II.Init(KAfInet6);
				 iRemoteAddr2_ID_II.Init(KAfInet6); 				 				 
				 iRemoteIDType_II = ID_IPV6_ADDR_SUBNET;
			  }	 
		   }
		   else {
              iRemoteAddr1_ID_II = *aReq.iDstAddr.iAddr;
			  if ( iRemoteAddr1_ID_II.Family() == KAfInet6 )			  
				   iRemoteIDType_II = ID_IPV6_ADDR;
			  else iRemoteIDType_II = ID_IPV4_ADDR;
		   }   
           iDefaultRemoteID = ETrue;  //Must be sent but won't be used when updating the SAD   
        }  
    }

    SetPhase2LifeDurations( aReq.iProposal.iComb->sadb_comb_soft_addtime,
                            aReq.iProposal.iComb->sadb_comb_hard_addtime,
                            aReq.iProposal.iComb->sadb_comb_soft_bytes,
                            aReq.iProposal.iComb->sadb_comb_hard_bytes,
                            *attr_II,
                            EFalse );

    //Save some pf_key data to use later in PFKEY_UPDATE msg
    iAcquireSeq = aReq.iBase.iMsg->sadb_msg_seq;        //msg Seq. number
    iPfkeyAcquirePID = aReq.iBase.iMsg->sadb_msg_pid;   //msg PID.
    iAcquirePending = ETrue;
    DEBUG_LOG2( _L("Acq seq= %d , PID= %d"), iAcquireSeq, iPfkeyAcquirePID );
    
}

//
//  CIkev1Negotiation::ExecuteL()
//  An ISAKMP message has been received belonging to this negotiation.
//  Process the message and advance the negotiation session to appropriate
//  next state/stage.
//
TBool CIkev1Negotiation::ExecuteL( const ThdrISAKMP& aHdr,
                                   const TInetAddr& aRemote,
                                   TInt aLocalPort )
{
    aLocalPort = aLocalPort;

    TBool ret=EFalse;
    const ThdrISAKMP *hdr;
    TUint8 *msg = NULL;   //to place the new msg
    TBuf8<IKEV1_MAX_IV_SIZE> tmp_IV(iIV);   //Temporal IV. Used to update the real one if the msg OK
    
    TLastIKEMsg msg_info(aHdr); //For retransmitted IKE msg detection
    if ( IsRetransmit(msg_info) ) {
       DEBUG_LOG(_L("Retransmitted IKE message received."));       
       TBool FloatedPort = EFalse; 
       if ( iNAT_D_Flags & (REMOTE_END_NAT + LOCAL_END_NAT) )
           FloatedPort = ETrue;
       TPtr8 lastMsg(iLastMsg->Des());
       iPluginSession->SendIkeMsgL(lastMsg, iLastRemoteAddr, FloatedPort);        
       return EFalse;              
    }

    if ( iPhase == PHASE_II &&
         aHdr.GetExchange() != IKE_QUICK_MODE )
    {
        DEBUG_LOG(_L("Bad packet (retransmission?)"));
#ifdef _DEBUG                    
        const TPtrC8 ikeMsgPtr( (TUint8 *)&aHdr, (TUint16)aHdr.GetLength() );
        TInetAddr dstAddr;
        iPluginSession->GetLocalAddress( dstAddr );
        dstAddr.SetPort( aLocalPort );
        TRACE_MSG_IKEV1( ikeMsgPtr, iRemoteAddr, dstAddr );
#endif // _DEBUG                            

        SetFinished();
        return EFalse;  //Bad packet, is a retransmission
    }    
    
	iLastRemoteAddr = aRemote;  //Save last remote address (used in NAT cases)	

    iLengthLeft = aHdr.GetLength(); //Used to check the size in the payload are OK

    DEBUG_LOG2( _L("---------- Phase %d - Stage %d ----------"), iPhase, iStage );

    if ((iStage==1) && (iPhase==PHASE_I))   //Only saved for the first message
        iExchange = aHdr.GetExchange();

    DEBUG_LOG1( _L("Exchange %d"), aHdr.GetExchange() );

    if (aHdr.GetFlags() & ISAKMP_HDR_EFLAG) //if encrypted
    {
        //before anything, prints the packet
        DEBUG_LOG(_L("Received message (encr)."));
        DEBUG_LOG(_L("Decrypting..."));
        msg = new (ELeave)(TUint8[aHdr.GetLength()]);    //to place the new msg
		CleanupStack::PushL(msg);
		
        Mem::Copy(msg,(TUint8 *)&aHdr,sizeof(aHdr));    //The header is not encrypted

        if ((iPhase==PHASE_II) && (iStage == 1))
        {
            iMessageId = aHdr.GetMessageId();   //Saves the ID to compute IV
            DEBUG_LOG(_L("Quick IV:"));
            ComputeIVL(iIV, iMessageId);
        }

        DecryptL((TUint8 *)aHdr.Next(),&msg[sizeof(aHdr)], aHdr.GetLength()-sizeof(aHdr),iIV, iSKEYID_e, iChosenProposal_I.iAttrList->iEncrAlg);
        if ((iStage == 6 && iExchange == ISAKMP_EXCHANGE_ID) || 
            (iStage == 3 && iExchange == ISAKMP_EXCHANGE_AGGR))
        {
            iLastIV.Copy(iIV);  //Saves last IV in Phase 1
            DEBUG_LOG(_L("Last IV Saved!"));
        }
        hdr=(ThdrISAKMP *)msg;  //decrypted msg

    }
    else if (iFlags & ISAKMP_HDR_EFLAG)  // IKE message SHOULD be encrypted
    {
        hdr=&aHdr;  //no encryption    
        DEBUG_LOG(_L("Received message."));
#ifdef _DEBUG                    
        const TPtrC8 ikeMsgPtr( (TUint8 *)hdr, (TUint16)hdr->GetLength() );
        TInetAddr dstAddr;
        iPluginSession->GetLocalAddress( dstAddr );
        dstAddr.SetPort( aLocalPort );
        TRACE_MSG_IKEV1( ikeMsgPtr, iRemoteAddr, dstAddr );
#endif // _DEBUG                            
        DEBUG_LOG(_L("The message is NOT encrypted (ignored)"));
        return EFalse;
    }
    else
        hdr=&aHdr;  //no encryption

    DEBUG_LOG(_L("Received message."));
#ifdef _DEBUG                    
    const TPtrC8 ikeMsgPtr( (TUint8 *)hdr, (TUint16)hdr->GetLength() );
    TInetAddr dstAddr;
    iPluginSession->GetLocalAddress( dstAddr );
    dstAddr.SetPort( aLocalPort );
    TRACE_MSG_IKEV1( ikeMsgPtr, iRemoteAddr, dstAddr );
#endif // _DEBUG                            

    if (iPhase==PHASE_I)
        ret = Phase_IExchangeL(*hdr);//MAIN MODE && AGGRESSIVE MODE
    else
    {
        ret = Phase_IIExchangeL(*hdr);//QUICK MODE 
    }
    if (!ret)   //Incorrect packet. Restore the IV
    {
        DEBUG_LOG(_L("Restoring previous IV"));
        iIV.Copy(tmp_IV);
    }
    else    //correct packet
    {
        SaveRetransmitInfo(msg_info); // store new last received IKE message info        
    }

    if ( msg )    //If used erase it (when encryption)
 	   CleanupStack::PopAndDestroy(); 

    return ret;
}

TBool CIkev1Negotiation::ExecutePhase2L( const ThdrISAKMP &aHdr,
                                         const TInetAddr &aRemote,
                                         TInt aLocalPort )
{
    return ExecuteL( aHdr, aRemote, aLocalPort );
}

//
// CIkev1Negotiation::ExecuteTransactionL
// An ISAKMP Transaction exchange message received.
// Call CTransNegotiation::ExecuteL method and process returned status
//
TBool CIkev1Negotiation::ExecuteTransactionL( const ThdrISAKMP& aHdr,
                                              const TInetAddr& aRemote,
                                              TInt aLocalPort )
{
    TInt  status;
	TBool ret = ETrue;
    if ( iTransactionNeg ) {
       status = iTransactionNeg->ExecuteL( aHdr, aRemote, aLocalPort );
       if ( status == TRANSACTION_SUCCESS ) {
          //
          // XAUTH / CONFIG-MODE completed succesfully
          //
          IsakmpPhase1CompletedL(); 
       }
       else {
          if ( status == TRANSACTION_FAILED ) {
             //
             // XAUTH / CONFIG-MODE completed succesfully
             //
			 LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
			                R_VPN_MSG_VPN_GW_AUTH_FAIL,
			                KKmdIkeAuthFailedErr,
			                iPluginSession->VpnIapId(),
			                &iRemoteAddr );
			 SendDeleteL( PROTO_ISAKMP );
			 SetErrorStatus( KKmdIkeAuthFailedErr );
          }
       }    
    }
	else ret = EFalse;

    return ret;
}


//
// CIkev1Negotiation::AuthDialogCompletedL
// Authentication dialog is completed. Check CAuthDialogInfo object ID
// and call eithet CIKECRACKNegotiation::ProcessUserResponseL or
// CTransNegotiation::ProcessUserResponseL to handle dialog data
//
void CIkev1Negotiation::AuthDialogCompletedL(CAuthDialogInfo *aUserInfo)
{
	if ( !aUserInfo || (!aUserInfo->iUsername && !aUserInfo->iSecret) ) 
	{
	   DEBUG_LOG(_L("Legacy authentication cancelled by user!"));		
	   SendDeleteL(PROTO_ISAKMP);		
	   LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
	                  R_VPN_MSG_VPN_GW_AUTH_FAIL,
	                  KErrCancel,
	                  iPluginSession->VpnIapId(),
	                  &iRemoteAddr );
	   SetErrorStatus(KErrCancel);
	   return;
	}	
		
    TInt status;

    if ( aUserInfo->GetObjId() == XAUTH_DIALOG_ID ) 
    {
       if ( iTransactionNeg )
            status = iTransactionNeg->ProcessUserResponseL(aUserInfo);       
       else status = TRANSACTION_FAILED;
       
       if ( status == TRANSACTION_FAILED ) 
       {
         /*--------------------------------------------------------
          *
          *  XAUTH negotiation failed. Negotiation shall be deleted
          *
          *--------------------------------------------------------*/
		  LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
		                 R_VPN_MSG_VPN_GW_AUTH_FAIL,
		                 status,
		                 iPluginSession->VpnIapId(),
		                 &iRemoteAddr );
		  SetErrorStatus(KKmdIkeAuthFailedErr);
          AcquireSAErrorResponse(KKmdIkeAuthFailedErr);
       }      
    }
    else 
    {
       if ( iCRACKneg )
       {
            status = iCRACKneg->ProcessUserResponseL(aUserInfo);       

           if ( status == CRACK_FAILED ) 
           {
             /*--------------------------------------------------------
              *
              *  Crack negotiation failed. Negotiation shall be deleted
              *
              *--------------------------------------------------------*/
              LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
                             R_VPN_MSG_VPN_GW_AUTH_FAIL,
                             status,
                             iPluginSession->VpnIapId(),
                             &iRemoteAddr );		   
              SetErrorStatus(KKmdIkeAuthFailedErr);		  
              AcquireSAErrorResponse(KKmdIkeAuthFailedErr);
           }
       }
       else 
           status = ProcessUserResponseL(aUserInfo);;
           
    }
}


//
// CIkev1Negotiation::StartCRACKAuthL
//      Start CRACK authentication phase of IKE phase I negotiation
//      - Create CIKECRACKNegotiation object and call it`s ConstructL
//      - Set iStage variable to 7 to indicate that CRACK authentication
//        is going
//
TBool CIkev1Negotiation::StartCRACKAuthL()
{
TInt status;    

    iStage = 7;
    
    if ( !iCRACKneg ) {
       iCRACKneg = new(ELeave) CIKECRACKNegotiation( iDebug );
	   TBuf<2> DummyDomain;
	   //
	   // If the IKE Id-type value is configured to value "Opaque
	   // String" and iFQDN length is larger than zero in the current
	   // policy ==> iFQDN value contains "Group name" value
	   // which shall be conveyed to the peer in CRACK "Domain name"
	   // attribute  
	   //
       iFlags |= ISAKMP_HDR_EFLAG; //From now on encryption is used    
	   
	   if ( (iHostData->iIdType == ID_KEY_ID) && (iHostData->iFQDN.Length() > 0 ) )
            status = iCRACKneg->ConstructL(iHostData->iCRACKLAMType, this, iHostData->iFQDN);
	   else status = iCRACKneg->ConstructL(iHostData->iCRACKLAMType, this, DummyDomain);
       if ( status == CRACK_FAILED ) {
          return EFalse;
       }
    }
    
    return ETrue;
}

//
// CIkev1Negotiation::IsakmpPhase1CompletedL
// This method is called when an ISAKMP phase 1 negotiation is succesfully
// completed. The following actions are taken:
// -- If either Extended authentication or/and Config mode required
//    If iTransactionNeg data member exists it means that XAUTH/CONFIG mode
//    has been succesfully completed.
//    If iTransactionNeg data member is NULL, XAUTH/CONFIG mode shall be initiated
// -- If No XAUTH/CONFIG mode (or if XAUTH/CONFIG mode already completed).
//    If iAutoLogin is TRUE save ISAKMP SA and deconstruct negotiation.
//    If iAutoLogin is False, save ISAKMP SA and;
//       if negotiation role is initiator continue with Quick mode exchange. 
//       if negotiation role is responder deconstruct negotiation.
//
TBool CIkev1Negotiation::IsakmpPhase1CompletedL()
{
    TBool Status;

    if ( iTransactionNeg ) {
       //
       // Try to get Internal address information and delete iTransactionNeg
       //
       delete iInternalAddr;  
       iInternalAddr = iTransactionNeg->GetInternalAddr();
       delete iTransactionNeg;
       iTransactionNeg = NULL;
    }
    else {
       if ( (iRole == INITIATOR) && (iHostData->iUseXauth || iHostData->iUseCfgMode) )
	   {
		  TBool useModeCfg;
		  if ( iSARekeyInfo )
		       useModeCfg = EFalse;  // Use existing virtual Ip, if any
		  else 
		      useModeCfg = iHostData->iUseCfgMode; 
		  
          iTransactionNeg = CTransNegotiation::NewL( iHostData->iUseXauth,
                                                     useModeCfg,
                                                     iPluginSession,
                                                     this,
                                                     iDebug );
          
          // If only MODE_CFG is needed a request is sent
          if(useModeCfg && !iHostData->iUseXauth)
              iTransactionNeg->BuildConfigRequestL();
          
          iStage = 8;                    
          return ETrue; 
       }       
    }   

	SaveISAKMPSAL();

    if ( iAutoLogin ) {
       //
       // ISAKMP Phase 1 completed. Quick mode is NOT started because there is no acquire pending
       // but phase 1 negotiation is started by the policy activation.   
       //
       CInternalAddress* internalAddr = NULL;
       if ( iInternalAddr != NULL )
           {
           internalAddr = CInternalAddress::NewL(*iInternalAddr);        
           }
       iPluginSession->IkeSaCompleted(KErrNone, internalAddr);
	   SetFinished(); // Causes negotiation object destruction	   
	   iAutoLogin = EFalse; 	   
       Status     = EFalse;
    }
    else {
	   if ( iRole == INITIATOR ) {
	      TBool internalAddressChanged = EFalse;
		  if ( iInternalAddr ) {
		     //
		     // Report internal IP address changed event
		     //
	         internalAddressChanged = iPluginSession->InternalAddressChangedL(*iInternalAddr);
		  }
		  if ( (!iSARekeyInfo ||
		       iPhaseIIAfterIkeSaRekey) &&
		       !internalAddressChanged )
		  {	  
             iPhaseIIAfterIkeSaRekey = EFalse;
             iPrevExchange = iExchange;  //Needed to know how to begin Phase II (Sending or receiving)
		     iExchange  = IKE_QUICK_MODE;
		     iPhase     = PHASE_II;
		     iStage     = 1;
 	         Status     = ETrue;
             iMessageId = RandomMessageId(); 
             InitPhase2L();  //Immediately inits PHASE_II. No reply expected.
		  }
		  else {
			  //
			  // Rekeyed IKE SA or internal address changed. No IKE quick mode started
		      // Pending acquire will be failed, if internal address has changed.
			  //
			  SetFinished(); 
			  Status    = EFalse;
		  }	  
	   }
	   else {
	      SetFinished(); // Causes negotiation object destruction
		  Status    = EFalse;
	   }	   
    }
	
	LOG_KMD_EVENT( MKmdEventLoggerIf::KLogInfo,
	               R_VPN_MSG_VPN_GW_AUTH_OK,
	               KErrNone,
	               iPluginSession->VpnIapId(),
	               &iRemoteAddr );

	TInt NatStatus = iNAT_D_Flags;   // Record "IETF" NAT status  
	if ( NatStatus == 0 )
	{	
	   if ( iNAT_T_Required )
			NatStatus = 4;           // Local end is NAT:ted (discovered via "Nokia" NAT-T)    
	   else if ( !iHostData->iUseNatProbing )
		       NatStatus = iHostData->iEspUdpPort; // "Forced" ESP UDP encapsulation ?
	}

	TInetAddr localAddr;
	iPluginSession->GetLocalAddress( localAddr );
    LOG_KMD_EVENT2( MKmdEventLoggerIf::KLogInfo,
                    R_VPN_MSG_ADDR_INFO_FOR_VPN_AP,
                    NatStatus, iPluginSession->VpnIapId(),
                    (iInternalAddr ? &iInternalAddr->iClientIntAddr : NULL),
                    &localAddr );	

    return Status;
}   


//Sends the initial IKE packets to start the negotiation. PHASE I
void CIkev1Negotiation::InitNegotiationL()   //Equiv. to stage 1
{
    
    if (iProposal_I.iAttrList->iAuthMethod == IKE_A_CRACK &&
        !iHostData->iCRACKLAMUserName && 
        !iHostData->iCRACKLAMPassword &&
        !iCRACKLAMUserName && 
        !iCRACKLAMPassword)
        {
            
            iDialog     = CIkev1Dialog::NewL( iPluginSession, iPluginSession->DialogAnchor(), iDebug );
            iDialogInfo = new(ELeave) CAuthDialogInfo(iPluginSession, DIALOG_INFO_ID, SAId(), 0);
            iDialog->GetAsyncUNPWDialogL(iDialogInfo, (MIkeDialogComplete*)this);
            return;
    }
    TIkev1IsakmpStream* msg = SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );
	
    TInt  vendor_id_type;

    //Main mode stage 1

    switch (iHostData->iMode)
    {
        case IKE_PARSER_MAIN:
            iExchange = ISAKMP_EXCHANGE_ID;     // Identity Protection (Main mode in IKE)
            DEBUG_LOG(_L("IKE: Initiating negotiation (Main Mode)"));
            break;
        case IKE_PARSER_AGGRESSIVE:
            iExchange = ISAKMP_EXCHANGE_AGGR;   // Agressive
            DEBUG_LOG(_L("IKE: Initiating negotiation (Aggressive Mode)"));
            break;
        default:
            DEBUG_LOG1(_L("Bad Mode used (%d)"), iHostData->iMode);
            return;
    }
    iStage = 1;
    DEBUG_LOG2(_L("---------- Phase %d - Stage %d ----------"),iPhase, iStage);

    iCookie_R.FillZ(ISAKMP_COOKIE_SIZE);  //Set responder Cookie to 0

    iDOI = IPSEC_DOI;
    iEncoding = X509_CERT_SIG;  //Only cert Allowed
    msg->IsakmpInit(this);
    msg->IsakmpSa();

    const TUint8 *ptr = msg->iBuf.Ptr() + sizeof(ThdrISAKMP);
    const TSAISAKMP *sa = (TSAISAKMP*)ptr;
    //Generic payload NOT included
    iSAPayloadSize = sa->GetLength() - sizeof(TPayloadISAKMP);
    delete iSAPayload;
	iSAPayload = NULL;
    iSAPayload = new (ELeave) TUint8[iSAPayloadSize];   
    ptr += sizeof(TPayloadISAKMP);  
    Mem::Copy(iSAPayload, ptr,iSAPayloadSize);

    TBool cert_required = EFalse;   //If any proposal requires a cert to send a CR if needed
    TBool preshared_key = EFalse;   //Preshared key authentication        
    
    TAttrib *transf = iProposal_I.iAttrList;
    for (TInt i=0; (i < iProposal_I.iNumTransforms) && (!cert_required); i++)
    {
        switch (transf->iAuthMethod)
        {
        case RSA_SIG:   //Proposals involving certificates
        case DSS_SIG:
            cert_required = ETrue;
            break;
        case IKE_A_CRACK:           
            cert_required = ETrue;
            break;
        default:    // No cert involved
            preshared_key = ETrue;
            transf = transf->iNext;
        }
    }
    

    if (iExchange == ISAKMP_EXCHANGE_AGGR) //Aggressive contains more payloads
    {
        if ( preshared_key && !cert_required ) {
           //
           // Only pre-shared key authentication proposal(s) exists
           // Check if is necessary to ask user name/password (= IKE ID/preshared key)
           // from user
           //
           if ( CheckCredentials(iHostData) != KErrNone ) {
              DEBUG_LOG(_L("Failed to get credentials for Aggressive pre-shared auth!"));
              SetFinished();
              return;
           }
        }   
        ComputeNonceL();         //Nonce to be sent
        if ( cert_required && !iOwnCert ) 
           ReadOwnCertL();    // For possible DER ASN1 distuingish name Ident
        msg->IsakmpKeyL();
        msg->IsakmpNonce();
        msg->IsakmpOwnIdentL();
		
        //For aggressive mode we send a CR if a cert is going to be needed
        if ((!iPeerX509Cert) && (cert_required))
        {
           msg->IsakmpCertificateReqL();   
        }
        if ( iHostData->iUseNatProbing )
             vendor_id_type = EXPANDED_VENDOR_ID;
        else vendor_id_type = HASH_VENDOR_ID;           
        msg->IsakmpVendorId(vendor_id_type,
                            (TUint8*)iCookie_I.Ptr(),
                            (TUint8*)iCookie_R.Ptr(), iLocalAddr);
            
    }
	
	if ( iHostData->iDPDHeartBeat != 0 )
	   BuildDPDVendorId(*msg);
	
    if ( iNatDiscovery ) {
       iNatDiscovery->BuildNatVendorId(*msg);
       iNatDiscovery->BuildRfcNatVendorId(*msg);
    }
	
    if ( iHostData->iUseXauth || iHostData->iUseCfgMode ) {  
       CTransNegotiation::BuildXauthVendorId(*msg);  
    }

    SendL(*msg);
    iStage = 2;
}

//Builds the proposal list from the structure in the engine
TBool CIkev1Negotiation::BuildProposals1L()
{
    TProposalData *p_list = iHostData->iPropList;

    if (!p_list)
    {
        DEBUG_LOG(_L("ERROR: No proposals in the configuration file. Negotiation Cancelled"));
        return EFalse;
    }
    iProposal_I.iSPI.FillZ(4);  //filled with 0 (not send anyway)
    iProposal_I.iProposalNum   = FIRST_ISAKMP_PROPOSAL; 
    iProposal_I.iProtocol      = PROTO_ISAKMP;
    iProposal_I.iNumTransforms = 0;
    
    TUint8 trans_num = FIRST_ISAKMP_TRANSFORM;

    TAttrib *prev=NULL,*attrlist=NULL;
    
    while ( p_list )
    {
        iProposal_I.iNumTransforms ++;
        attrlist = new (ELeave) TAttrib;
        if (!iProposal_I.iAttrList) //First transform
            iProposal_I.iAttrList=attrlist; //attrlist safe
        if (prev)
            prev->iNext = attrlist;
        prev = attrlist;
        
        attrlist->iTransformID  = KEY_IKE; //only one allowed by Protocol ISAKMP (KEY_IKE)
        attrlist->iTransformNum = trans_num;
        trans_num++;        
        switch (p_list->iEncrAlg)
        {
        case IKE_PARSER_DES_CBC:
            attrlist->iEncrAlg=DES_CBC;     //DES_CBC
            break;
        case IKE_PARSER_DES3_CBC:
            attrlist->iEncrAlg=DES3_CBC;    //DES3_CBC
            break;
        case IKE_PARSER_AES_CBC:
            attrlist->iEncrAlg   = AES_CBC;     //AES_CBC
            attrlist->iKeyLength = (TUint16)p_list->iEncrKeyLth;
            if (!attrlist->iKeyLength)
               attrlist->iKeyLength = 128; //default AES key size   
            break;
        default:
            DEBUG_LOG(_L("ISAKMP Proposals error (Bad Encryption algorithm)"));
            return EFalse;
        }
        switch (p_list->iHashAlg)
        {
        case IKE_PARSER_MD5:
            attrlist->iHashAlg=HASH_MD5;    //HASH_MD5
            break;
        case IKE_PARSER_SHA1:
            attrlist->iHashAlg=HASH_SHA1;   //HASH_SHA1
            break;
        default:
            DEBUG_LOG(_L("ISAKMP Proposals error (Bad Hash algorithm)"));
            return EFalse;
        }
        switch (p_list->iAuthMeth)
        {
        case IKE_PARSER_RSA_SIG:
            attrlist->iAuthMethod=RSA_SIG;
            break;
        case IKE_PARSER_DSS_SIG:
            attrlist->iAuthMethod=DSS_SIG;
            break;
        case IKE_PARSER_PRE_SHARED:
            attrlist->iAuthMethod=PRE_SHARED;           
            break;
        case IKE_PARSER_CRACK:
            attrlist->iAuthMethod=IKE_A_CRACK; //CRACK authentication
            break;
            
        default:
            DEBUG_LOG(_L("ISAKMP Proposals error (Bad Authentication Method)"));
            return EFalse;
        }
        
        switch (p_list->iGroupDesc)
        {
        case IKE_PARSER_MODP_768:
            attrlist->iGroupDesc = MODP_768;
            break;
        case IKE_PARSER_MODP_1024:
            attrlist->iGroupDesc = MODP_1024;
            break;
        case IKE_PARSER_MODP_1536:
            attrlist->iGroupDesc = MODP_1536;
            break;
        case IKE_PARSER_MODP_2048:
            attrlist->iGroupDesc = MODP_2048;
            break;
        default:
            DEBUG_LOG(_L("ISAKMP Proposals error (Bad Group description)"));
            return EFalse;      
        }

        switch (p_list->iGroupType)
        {
        case IKE_PARSER_MODP:
            attrlist->iGroupType = MODP;
            break;
        case IKE_PARSER_DEFAULT:
            break;
        default:
            DEBUG_LOG(_L("ISAKMP Proposals error (Bad Group Type)"));
            return EFalse;
        }

        switch (p_list->iPRF)
        {
        case IKE_PARSER_DES3_CBC_MAC:
            attrlist->iPRF=OAKLEY_PRF_3DES_CBC_MAC;
            break;
        case IKE_PARSER_NONE:
            break;
        default:
            DEBUG_LOG(_L("ISAKMP Proposals error (Bad PRF specified)"));
            return EFalse;
        }

        TUint32 lifetime = ByteOrder::Swap32(p_list->iLifetimeSec);
        if (lifetime)
            attrlist->iLifeDurationSecs.Copy((TUint8 *)&lifetime, sizeof(lifetime));

        lifetime = ByteOrder::Swap32(p_list->iLifetimeKb);
        if (lifetime)
            attrlist->iLifeDurationKBytes.Copy((TUint8 *)&lifetime, sizeof(lifetime));

        //
        // Store parameters for extended authentication
        //
        attrlist->iXauthUsed = iHostData->iUseXauth;
        attrlist->iRole      = iRole;
        
        p_list = p_list->iNext;
        
    }
    attrlist->iNext=NULL;   //Last transform


    return ETrue;
}

//Builds Phase_II proposals from the config. file to when acting as RESPONDER to see if 
//proposals received are acceptable.
TInt CIkev1Negotiation::BuildProposals2L()
{
    CIpsecSaSpecList* SaList = NULL;
    TInetAddr empty_addr;  //empty address
    TInt err = KErrNone;
    
    TRAP(err,
        if ( iLocalIDType_II == 0 )  //Local ID not received (so remote neither) so it's the same as gateway
            {
            TInetAddr localSelector(iLocalAddr);
            localSelector.SetPort(iIDLocalPort);
            TInetAddr remoteSelector(iRemoteAddr);
            remoteSelector.SetPort(iIDRemotePort);
            
            SaList = iPluginSession->GetIpseSaSpecListLC(localSelector, empty_addr, 
                                                         remoteSelector, empty_addr,
                                                         iIDProtocol );
            }
        else    // either none or both (RFC 2409 5.5 so we must have both)
            {
            TInetAddr localSelector(iLocalAddr1_ID_II);
            localSelector.SetPort(iIDLocalPort);
            TInetAddr remoteSelector(iRemoteAddr1_ID_II);
            remoteSelector.SetPort(iIDRemotePort);
    
            
            SaList = iPluginSession->GetIpseSaSpecListLC(localSelector,
                                                         iLocalAddr2_ID_II,
                                                         remoteSelector,
                                                         iRemoteAddr2_ID_II,
                                                         iIDProtocol );
            }
        
        CleanupStack::Pop(SaList);
    );
    
    if (err != KErrNone)
        return err;
    
    CleanupStack::PushL(SaList);

    //Phase II proposals

    CProposal_II *prop = NULL;
    TAttrib_II *attr_II = NULL;
    const TIpsecSaSpec *spec = NULL;
    TInt count = SaList->Count();
    TInt i     = 0;

    while ( i < count ) 
    {
        prop = new (ELeave) CProposal_II();
        CleanupStack::PushL(prop);
        prop->ConstructL(1);
        iProposal_IIList->AppendL(prop);
        CleanupStack::Pop();    //prop safe
        //
        // Only 1 proposal which may be AND'd (many prop with same num) if many bundles
        // Only 1 transform because no OR'ing implemented in IPSEC
        //
        prop->iProposalNum   = FIRST_IPSEC_PROPOSAL;
        prop->iNumTransforms = 1;  

        attr_II = new (ELeave) TAttrib_II();
        CleanupStack::PushL(attr_II);
        prop->iAttrList->AppendL(attr_II);
        CleanupStack::Pop();    //attr_II safe
        
        attr_II->iTransformNum = FIRST_IPSEC_TRANSFORM;     

        spec = &(SaList->At(i));
        
        if (spec->iType == SADB_SATYPE_AH)
        {
            prop->iProtocol=PROTO_IPSEC_AH;
            if (spec->iAalg == SADB_AALG_MD5HMAC)
            {
                attr_II->iTransformID = AH_MD5;
                attr_II->iAuthAlg = DOI_HMAC_MD5;
            }
            else if (spec->iAalg == SADB_AALG_SHA1HMAC)
            {
                attr_II->iTransformID=AH_SHA;
                attr_II->iAuthAlg = DOI_HMAC_SHA;
            }
            else
            {
                DEBUG_LOG(_L("Unsupported Authentication Algorithm in IPsec Policy"));
                err = KKmdIkeNoProposalErr;
                break;
            }
            // No auth with variable encryption
        }
        else if (spec->iType == SADB_SATYPE_ESP)
        {
            prop->iProtocol = PROTO_IPSEC_ESP;
            // Request ESP from peer host

            attr_II->iTransformID = spec->iEalg;
			switch ( attr_II->iTransformID )
			{
				case ESP_DES_CBC:
				case ESP_3DES_CBC:
				case ESP_NULL:
					break;

				case ESP_AES_CBC:
					attr_II->iKeyLength = spec->iEalgLen;   //If 0 it won't be sent
					break;

				default:
					DEBUG_LOG(_L("IPsec Encryption algorithm is not implemented. Wrong algorithms file"));
	                err = KKmdIkeNoProposalErr;
	                break;
			}	
			
			if (err != KErrNone)
			    {
			    break;
			    }
			
            if (spec->iAalg != SADB_AALG_NONE)
            {
                if (spec->iAalg == SADB_AALG_MD5HMAC)
                    attr_II->iAuthAlg = DOI_HMAC_MD5;
                else if (spec->iAalg == SADB_AALG_SHA1HMAC)
                    attr_II->iAuthAlg = DOI_HMAC_SHA;
                else
                {
                    DEBUG_LOG(_L("Unsupported Authentication Algorithm in IPsec Policy"));
                    err = KKmdIkeNoProposalErr;
                    break;
                }
            }
        }

        //Check if PFS in use...
        iPFS = spec->iPfs;

        if (spec->iTransportMode)
             attr_II->iEncMode = DOI_TRANSPORT;
        else attr_II->iEncMode = DOI_TUNNEL;

        //////////////////////////////////////////////////////////
        //
        // Check if remote identity defined in Security Policy
        // If it is not set iDefaultLocalID = ETrue.
        // This prevents destination identity information to IPSEC
        // in PFKEY Update- and Add primitives
        // (see UpdateSADatabaseL() method)
        //
        //////////////////////////////////////////////////////////
        if ( spec->iRemoteIdentity.Length() == 0 )
           iDefaultLocalID = ETrue;
		if ( !ExamineRemoteIdentity(spec->iRemoteIdentity) )
		{
			DEBUG_LOG(_L("Remote Identity mismatch with IPsec Policy"));
            err = KKmdIkeNoProposalErr;
            break;
		}
		
	    SetPhase2LifeDurations( spec->iSoft.iAddTime,
                                spec->iHard.iAddTime,
                                spec->iSoft.iBytes,
                                spec->iHard.iBytes,
	                            *attr_II,
	                            ETrue );

	    if (iPFS)
        {
            switch (iHostData->iGroupDesc_II)
            {
            case IKE_PARSER_MODP_768:
                attr_II->iGroupDesc = MODP_768;
                break;
            case IKE_PARSER_MODP_1024:
                attr_II->iGroupDesc = MODP_1024;
                break;
            case IKE_PARSER_MODP_1536:
                attr_II->iGroupDesc = MODP_1536;
                break;
            case IKE_PARSER_MODP_2048:
                attr_II->iGroupDesc = MODP_2048;
                break;
            default:    //Shouldn't happen but the error will be detected later
                err = KKmdIkeNoProposalErr;
                break;
            }
            
            if (err != KErrNone)
                {
                break;
                }            
        }
        
        prop->iReplayWindowLength = spec->iReplayWindowLength;

        i ++;
    }   //while

    if (err != KErrNone)
        {
        delete iProposal_IIList;
        iProposal_IIList = NULL;        
        }
    
    CleanupStack::PopAndDestroy(SaList);  //SAList
    
    return err;
}

TBool CIkev1Negotiation::ExamineRemoteIdentity(const TDesC8& aRemoteIdInPolicy)
{
   //////////////////////////////////////////////////////////////////
   //
   // This method is called when we are acting as a Quick mode responder.   
   // The purpose of this method is compare remote Identity information
   // received in Quick Mode IKE message (=IDi) to the remote Identity
   // information in the local Ipsec Policy.
   // If Identitiea are NOT matching the FALSE response is returned.
   // There is the following special "kludge" is done in these test:
   // Identity IPv4 address and IPv4 subnet with prefix 32 are
   // interpreted to be same as well as IPv6 address and IPv6 subnet
   // with prefix 128. When this situation occurs a special flag 
   // (iSwapRemoteIdType = ETrue) is set in CIkev1Negotiation object
   //
   //////////////////////////////////////////////////////////////////
   TBool      AddrMatch;   
   TInetAddr  IpAddr;
   TInetAddr  IpMask;
   TUint8     IdType;
   TInt       PrefixLength = 0;
   TInetAddr* PrefixPtr    = NULL;      
   
   if ( !ProcessIdentityData(aRemoteIdInPolicy, &IdType, &IpAddr, &IpMask) )
	  return EFalse;
   
   AddrMatch = IpAddr.Match(iRemoteAddr1_ID_II);

   if ( AddrMatch )
   {	   
      if ( (IdType == ID_IPV4_ADDR_SUBNET ) || (IdType == ID_IPV6_ADDR_SUBNET) ) 
      {
		 // 
		 // Identity in policy is IP subnet
		 //
	     if ( iRemoteIDType_II == IdType )
		 {
		    // 
		    // Both identity in policy and identity in IKE IDi payload are
		    // IP subnets. The subnet masks MUST match, too
		    //
			AddrMatch = IpMask.Match(iRemoteAddr2_ID_II);
		 }
		 else
		 {
			 if ( ( (IdType == ID_IPV4_ADDR_SUBNET ) && (iRemoteIDType_II == ID_IPV4_ADDR)) ||
			      ( (IdType == ID_IPV6_ADDR_SUBNET ) && (iRemoteIDType_II == ID_IPV6_ADDR)))
			 {	 
			    // 
			    // Identity in IKE IDi payload is an IP address
			    // Do the special check: There must be full mask in the
			    // subnet identity configured in policy
			    //
				if ( IdType == ID_IPV6_ADDR_SUBNET )
					 PrefixLength = 128;
				else PrefixLength = 32;
				PrefixPtr = &IpMask;
				DEBUG_LOG(_L("Peer is using IP address IDi, full mask subnet required in local end!"));
			 }
			 else AddrMatch = EFalse;  
		 }	 
      }	   
      else
      {
	     // 
	     // Identity in policy is IP address.
	     //
		 if ( ( (IdType == ID_IPV4_ADDR ) && (iRemoteIDType_II == ID_IPV4_ADDR_SUBNET)) ||
			  ( (IdType == ID_IPV6_ADDR ) && (iRemoteIDType_II == ID_IPV6_ADDR_SUBNET)))
		 {	 
				// 
				// Identity in IKE IDi payload is an IP subnet
				// Do the special check: There must be full mask in the
				// subnet identity in IKE IDi payload
				//
			 if ( IdType == ID_IPV6_ADDR )
			      PrefixLength = 128;
			 else PrefixLength = 32;
			 PrefixPtr = &iRemoteAddr2_ID_II;
			 DEBUG_LOG(_L("An IP address ID used in local end, full mask subnet required in peer IDi!"));
		 }
		 else if ( iRemoteIDType_II != IdType )
				 AddrMatch = EFalse;
		 
      }
   }

   if ( PrefixPtr )
   {	   	   
	  if ( PrefixLength == PrefixLen(*PrefixPtr) )
	  {	  
	  	 iSwapRemoteIdType = ETrue;
		 DEBUG_LOG(_L("Required ID OK, modified remote IDi informed to IPsec!"));		 
	  }	 
	  else AddrMatch = EFalse;
   }
   
   return AddrMatch;
}

TBool CIkev1Negotiation::ProcessIdentityData(const TDesC8& aIdentity, TUint8* aToIdType,
										TInetAddr* aToIpAddr1, TInetAddr* aToIpAddr2)
{
   if ( !aToIdType || !aToIpAddr1 || !aToIpAddr2 )
	  return EFalse;
   
   aToIpAddr1->Init(KAFUnspec);
   aToIpAddr2->Init(KAFUnspec);
   *aToIdType = 0;
		   
   if ( aIdentity.Length() )
   {	   
      TInt offset = aIdentity.Find(_L8("/"));
   
      switch (offset)
      {
	     case KErrNotFound:  //Simple address
         { 
#ifdef _UNICODE
		    HBufC *unibuf = HBufC::New(aIdentity.Length());
			if ( !unibuf )
			    return EFalse;	
	        unibuf->Des().Copy(aIdentity);
	    	if ( aToIpAddr1->Input(unibuf->Des()) != KErrNone )
	    	{
				delete unibuf;
		    	DEBUG_LOG(_L("Bad IP address identity!"));
				return EFalse;
	    	}  
		    delete unibuf;
#else
		    if (aToIpAddr1->Input(aIdentity) != KErrNone)
		    {
				DEBUG_LOG(_L("Bad IP address identity!"));
				return EFalse;
		    }
#endif  
		    if ( aToIpAddr1->Family() == KAfInet )
			     *aToIdType = ID_IPV4_ADDR;
		    else *aToIdType = ID_IPV6_ADDR;
		    break;
     	 }
		 
	     default:    //Subnet
	     {
				//addr1 - subnet
		 TInt prefix_len;	 
#ifdef _UNICODE
		    HBufC *unibuf = HBufC::New(aIdentity.Length());
			if ( !unibuf )
				return EFalse;
			unibuf->Des().Copy(aIdentity);			
		    TPtrC addr_buf(unibuf->Ptr(), offset);
		    if (aToIpAddr1->Input(addr_buf) != KErrNone)
		    {
				delete unibuf;				
			    DEBUG_LOG(_L("Bad Subnet Identity address!"));
				return EFalse;
		    }
		    TPtrC prefix_ptr(unibuf->Ptr() + offset + 1, unibuf->Length() - offset - 1);
#else
		    TPtrC addr_buf(aIdentity.Ptr(), offset);
			if (aToIpAddr1->.Input(addr_buf) != KErrNone)
			{
				DEB(LogError(_L("Bad Subnet Identity address!"));)
				return EFalse;
			}
		    TPtrC prefix_ptr(aIdentity.Ptr() + offset + 1, aIdentity.Length() - offset - 1);
#endif  
				//addr2 - mask
		    TLex lex(prefix_ptr);
		    if (lex.Val(prefix_len) != KErrNone)
		    {
			   DEBUG_LOG(_L("Bad Subnet Identity PREFIX Length!"));
			   return EFalse;
		    }
#ifdef _UNICODE
		    delete unibuf;
#endif
			if ( aToIpAddr1->Family() == KAfInet )
			{
			   if ( prefix_len > 32 )	
			   {
				  DEBUG_LOG(_L("Bad Subnet Identity: Prefix too long!"));
				  return EFalse;
			   }
			   *aToIdType = ID_IPV4_ADDR_SUBNET;
			   PrefixMask(*aToIpAddr2, prefix_len, KAfInet);
			}
			else    //KAfInet6
			{
				if ( prefix_len > 128 )	
				{
					DEBUG_LOG(_L("Bad Subnet Identity: Prefix too long!"));
					return EFalse;
				}
				*aToIdType = ID_IPV6_ADDR_SUBNET;
				PrefixMask(*aToIpAddr2, prefix_len, KAfInet);
			}

		 }
		 
	  } //end switch
   }	  

   return ETrue;
}

//First msg of PHASE_II as initiator
void CIkev1Negotiation::InitPhase2L()
{
    //Quick mode stage 1
    DEBUG_LOG(_L("-------- IKE: Initiating PHASE II --------"));
    iPhase = PHASE_II;
    iDOI=IPSEC_DOI;
    iStage = 1;

    GetSPIL();
    //the rest will be done in ReceiveSPI
}

//Requests an SPI from the kernel

void CIkev1Negotiation::GetSPIL()
{
    CProposal_IIList *propII_List;
    CProposal_II *prop=NULL;
    TUint8 sa_type = 0;

    DEBUG_LOG1(_L("GetSPI in stage: %d"), iStage);

    iInboundSPIList = new (ELeave) CArrayFixFlat<TSPINode>(1);

    if (iRole == RESPONDER) //If Phase II proposal Chosen
        propII_List = iChosenProp_IIList;   //If RESPONDER
    else
        propII_List = iProposal_IIList; //If INITIATOR

    TInt i, count = propII_List->Count();
    for (i = 0; i < count; i++) //May have many Phase_II proposals
    {
        prop = propII_List->At(i);
        if (prop->iProtocol == PROTO_IPSEC_AH)
            sa_type = SADB_SATYPE_AH;
        else if (prop->iProtocol == PROTO_IPSEC_ESP)
            sa_type = SADB_SATYPE_ESP;
        else
            sa_type = 0;    //Unknown Protocol

        TInetAddr myAddr(iLocalAddr);
		if ( myAddr.IsUnspecified() )
		    User::LeaveIfError( iPluginSession->GetLocalAddress( myAddr ) );
        TInetAddr peerAddr(iRemoteAddr);
        peerAddr.SetPort(0);
        TSPINode node;
        node.iPropNum = prop->iProposalNum;
        node.iSeq = iSeq;
        iInboundSPIList->AppendL(node);
        
        
        __ASSERT_DEBUG( iIpsecSaSpiRetriever != NULL,
                        User::Invariant() );
        iIpsecSaSpiRetriever->Cancel();
        iIpsecSaSpiRetriever->GetIpsecSaSpi( iSeq++, sa_type, peerAddr, myAddr );        
        iPendingSPI++;      //To know when all received in ReceiveSPIL()
    }

    DEBUG_LOG1(_L("GetSPI seq= %d"), iSeq);
}

//aSPI received in Network order.
void CIkev1Negotiation::ReceiveSPIL(TUint32 aSPI, TUint32 aSeq)
{
	TIkev1IsakmpStream* msg = SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );
	
    DEBUG_LOG2(_L("ReceiveSPIL: SPI=%x, Seq=%u"), ByteOrder::Swap32(aSPI), aSeq);

    CProposal_IIList *propII_List = iProposal_IIList;
    TInt i;
    for (i = 0; i < iInboundSPIList->Count(); i++)
    {
        if (iInboundSPIList->At(i).iSeq == aSeq)
        {
            iInboundSPIList->At(i).iSPI = aSPI;
            break;
        }
        //prop = prop->iNext;   //To assign the SPI to the correct proposal
    }
    if (iRole==INITIATOR)
        propII_List->At(i)->iSPI.Copy((TUint8*)&aSPI,sizeof(TUint32));  //needed to send it to the other Peer

    if (--iPendingSPI)  // Check if still waiting for some SPI
        return;

    iExchange = IKE_QUICK_MODE; //Current mode being used.
    //Builds and send the packet

    if ((iHostData->iCommit) && (iRole==RESPONDER)) //Responder  && if commit bit used we set the flag
        iFlags |= ISAKMP_HDR_CFLAG; //Sets the Commit bit if this side set it else

    msg->IsakmpInit(this);
    msg->IsakmpHashL();
    msg->IsakmpSa();
    ComputeNonceL();     //Computes a new Nonce for PHASE_II
    msg->IsakmpNonce();
    if (iPFS)   //Only sent if PFS in use...
        msg->IsakmpKeyL();

    if (iStage==1)  //Initiator
    {
        //
        // If Internal address private extension used change iLocalAddr1_ID_II to
        // correspond that address
        //
        if ( iInternalAddr ) {
           iLocalAddr1_ID_II = iInternalAddr->iClientIntAddr;       
           iLocalIDType_II   = ID_IPV4_ADDR;
           iDefaultLocalID   = ETrue;  
        }   
        DEBUG_LOG(_L("PhaseII IV:"));   //New IV for phase II
        ComputeIVL(iIV, iMessageId);
        //IDs must be sent if TUNNEL mode or is explicitly specified in the acquire
        if ((!iLocalAddr1_ID_II.IsUnspecified()) || (!iRemoteAddr1_ID_II.IsUnspecified()) ||
            (iIDProtocol != 0) || (iIDLocalPort != 0) || (iIDRemotePort != 0))
        {
            msg->IsakmpOwnIdentL();     //Own proxy
            msg->IsakmpPeerIdentL();    //Peer Proxy
        }
    }
    else if (iStage==2) //Responder
    {
        if (iIDReceived)    //If received we send it back, otherwise no
        {
            msg->IsakmpPeerIdentL();
            msg->IsakmpOwnIdentL(); 
        }
        if (iHostData->iResponderLifetime)
            CheckSendResponderLifetime(*msg);    //Adds to the message the RESPONDER_LIFETIME payload if needed
    }
    else return;

    if (iHostData->iReplayStatus)
    {
        DEBUG_LOG(_L("Constructing REPLAY-STATUS"));
    
        TInt i;
        CProposal_II *prop;
        for (i = 0 ; i < iProposal_IIList->Count(); i++)
        {
            prop = iProposal_IIList->At(i);
            msg->IsakmpReplayStatus(prop->iProtocol, iInboundSPIList->At(i).iSPI, prop->iReplayWindowLength);
        }
    }
    msg->IsakmpHashContL(); //inserts the hash in the correct position of the buffer
    SendL(*msg);

    iStage++;   //Next stage
}

void CIkev1Negotiation::AcquireSAErrorResponse(TInt aError)
	{
	DEBUG_LOG(_L("CIkev1Negotiation::AcquireSAErrorResponse"));
		
	if ( iProposal_IIList )
		{
		for ( TInt j=0; j<iProposal_IIList->Count(); j++ )
			{			
			TIpsecSAData sa_data;	
		    sa_data.iSeq = iAcquireSeq;
		    sa_data.iPid = iPfkeyAcquirePID;
			sa_data.iSPI = 0;	
			sa_data.iDst = iRemoteAddr;
			
			CProposal_II* prop_II = iProposal_IIList->At( j );

			if ( prop_II->iProtocol == PROTO_IPSEC_AH )
				{
				sa_data.iSAType = SADB_SATYPE_AH;				
				}
	        else if ( prop_II->iProtocol == PROTO_IPSEC_ESP )
	            {
	            sa_data.iSAType = SADB_SATYPE_ESP;
	            }
			iPluginSession->AcquireSAError( sa_data, aError );
			}
		}
    iAcquirePending = EFalse;
	}

//Will update the outbound
void CIkev1Negotiation::UpdateSADatabaseL()
{
#ifdef _DEBUG
    TBuf<40> addr_buf;
#endif    
    TUint8 sa_type=0;
    TUint8 auth_alg=0;
    TUint8 encr_alg=0;
    TUint32 updateSPI,addSPI;
    TIpsecSAData sa_data;
    TBool   TunnelMode;
	//
    // Received Phase II key (Might be Auth + Encr keys)
	// (Buffers are allocated for max 1024 bits key material
	HBufC8* outboundKey_II = HBufC8::NewLC(128);
	HBufC8* inboundKey_II  = HBufC8::NewLC(128);     	
    TPtrC8 in_authKey(0,0), out_authKey(0,0);
    TPtrC8 in_encrKey(0,0), out_encrKey(0,0);
    //Identities
#ifdef _UNICODE	
	TBuf<80> id_work;
#endif						
	HBufC8* local_id   = HBufC8::NewLC(128);
	HBufC8* remote_id  = HBufC8::NewLC(128);
    // ESP UDP Encapsulation extension data	
	HBufC8* gen_ext_data = HBufC8::NewLC(128);     	
	TPtr8 GenExtData((TUint8*)gen_ext_data->Ptr(), 0, gen_ext_data->Des().MaxLength());
	
	DEBUG_LOG(_L("---UPDATING SAD---"));	
	
    if ((!iDefaultLocalID) && (iLocalIDType_II != 0))
    {
        switch (iLocalIDType_II)
        {   
        case ID_IPV4_ADDR:
        case ID_IPV6_ADDR:
#ifdef _UNICODE				
            iLocalAddr1_ID_II.OutputWithScope(id_work);
			local_id->Des().Copy(id_work);			
#else
			iLocalAddr1_ID_II.OutputWithScope(local_id->Des());			
#endif			
            break;
			
        case ID_IPV4_ADDR_SUBNET:
        case ID_IPV6_ADDR_SUBNET:
#ifdef _UNICODE							
            iLocalAddr1_ID_II.OutputWithScope(id_work);
			local_id->Des().Copy(id_work);			
#else
			iLocalAddr1_ID_II.OutputWithScope(local_id->Des());			
#endif						
            local_id->Des().AppendFormat(_L8("/%d"),PrefixLen(iLocalAddr2_ID_II));  //PrefixLen can't fail because checked before
            break;
        default:    //Should never come here
            DEBUG_LOG1(_L("Local ID type %d not supported"), iLocalIDType_II);
			CleanupStack::PopAndDestroy(5);  //key buffer, identities and ESP UDP encaps data
            return;
        }
    }

    if ((!iDefaultRemoteID) && (iRemoteIDType_II != 0) )
    {
        switch (iRemoteIDType_II)
        {   
        case ID_IPV4_ADDR:
        case ID_IPV6_ADDR:
#ifdef _UNICODE				
			iRemoteAddr1_ID_II.OutputWithScope(id_work);
			remote_id->Des().Copy(id_work);			
#else
			iRemoteAddr1_ID_II.OutputWithScope(remote_id->Des());			
#endif
			if ( iSwapRemoteIdType )
			{
			   if ( iRemoteIDType_II == ID_IPV4_ADDR )
				    remote_id->Des().AppendFormat(_L8("/%d"), 32);
			   else remote_id->Des().AppendFormat(_L8("/%d"), 128);
			}	
            break;
			
        case ID_IPV4_ADDR_SUBNET:
        case ID_IPV6_ADDR_SUBNET:
#ifdef _UNICODE				
			iRemoteAddr1_ID_II.OutputWithScope(id_work);
			remote_id->Des().Copy(id_work);			
#else
			iRemoteAddr1_ID_II.OutputWithScope(remote_id->Des());			
#endif
			if ( !iSwapRemoteIdType )
               remote_id->Des().AppendFormat(_L8("/%d"),PrefixLen(iRemoteAddr2_ID_II));    //PrefixLen can't fail because checked before
            break;
			
        default:    //Should never come here
            DEBUG_LOG1(_L("Remote ID type %d not supported"), iRemoteIDType_II);
			CleanupStack::PopAndDestroy(5);  //key buffer, identities and ESP UDP encaps data					
            return;
        }
    }

    TUint32 flags = 0;
    if (iPFS)
    {
        flags = SADB_SAFLAGS_PFS;
        DEBUG_LOG(_L("PFS enabled"));
    }

    TUint32 addPID;
    if (iRole==INITIATOR)
        addPID = iPfkeyAcquirePID;  //Require to remove the Larval SA
    else
        addPID = iPluginSession->Uid();
    
    DEBUG_LOG2(_L("SAD seq= %d , PID= %d"), iAcquireSeq, addPID);

    TInt i, j;
    for (i = 0; i < iInboundSPIList->Count(); i++)
    {
        if (iInboundSPIList->At(i).iPropNum == iProposalNum)
            break;
    }

    CProposal_II *prop;
    TSPINode inboundspi_node;
    TInt key_len, encr_len, auth_len;
    TChosenAttrib_II *attr_II;
    TInt count = iChosenProp_IIList->Count();

    for (j = 0 ; j < count; j++)
    {
        prop = iChosenProp_IIList->At(j);
        inboundspi_node = iInboundSPIList->At(i);
        attr_II = (TChosenAttrib_II *)prop->iAttrList->At(0);   //only 1 transform is chosen no matter how many there are
        if (prop->iProtocol == PROTO_IPSEC_AH)
        {
            
            sa_type  = SADB_SATYPE_AH;
            encr_alg = 0;
            auth_alg = attr_II->iTransformID;
			auth_len = (TInt)HMAC_KeyLength((TUint8)auth_alg);
			
			TPtr8 AHOutKey((TUint8*)outboundKey_II->Ptr(), 0, outboundKey_II->Des().MaxLength());
			TPtr8 AHInKey((TUint8*)inboundKey_II->Ptr(), 0, inboundKey_II->Des().MaxLength());
			
            ComputeKeys2L(prop, auth_len, inboundspi_node, AHOutKey, AHInKey);
            in_encrKey.Set(NULL, 0);
            out_encrKey.Set(NULL, 0);
            in_authKey.Set(inboundKey_II->Ptr(), auth_len/8);
            out_authKey.Set(outboundKey_II->Ptr(),auth_len/8);
        }
        else if (prop->iProtocol == PROTO_IPSEC_ESP)
        {
            sa_type = SADB_SATYPE_ESP;
            encr_alg = attr_II->iTransformID;
            if (attr_II->iKeyLength!=0)
                encr_len = attr_II->iKeyLength;
            else    //not sent means constant size or variable and use default
            {
				switch ( encr_alg )
				{
					case ESP_DES_CBC:
						encr_len = 64;
						break;
					case ESP_3DES_CBC:
						encr_len = 3*64;
						break;
					case ESP_NULL:
						encr_len = 0;
						break;
					case ESP_AES_CBC:
						encr_len = 128;
						break;
					default:	
						encr_len = 0;
						break;
						
				}	
            }

            if (attr_II->iAuthAlg==DOI_HMAC_MD5)
                auth_alg = SADB_AALG_MD5HMAC;
            else if (attr_II->iAuthAlg==DOI_HMAC_SHA)
                auth_alg = SADB_AALG_SHA1HMAC;
            else
                auth_alg = 0;

            auth_len = (TInt)HMAC_KeyLength((TUint8)auth_alg);
            key_len = encr_len + auth_len;

			TPtr8 ESPOutKey((TUint8*)outboundKey_II->Ptr(), 0, outboundKey_II->Des().MaxLength());
			TPtr8 ESPInKey((TUint8*)inboundKey_II->Ptr(), 0, inboundKey_II->Des().MaxLength());			
            ComputeKeys2L(prop, key_len, inboundspi_node, ESPOutKey, ESPInKey);

            in_encrKey.Set(inboundKey_II->Ptr(), encr_len/8);
            out_encrKey.Set(outboundKey_II->Ptr(), encr_len/8);

            //If no HMAC selected the next instr does nothing because size will be 0
            in_authKey.Set(inboundKey_II->Ptr() + in_encrKey.Length(),auth_len/8);
            out_authKey.Set(outboundKey_II->Ptr() + out_encrKey.Length(),auth_len/8);
            //
            // Nokia specific NAT traversal info (=ESP UDP tunneling)  
            // If iNAT_T_Required is true connection is over NAT:ted
            // newtork (=local end behind NAT).
			//
            if ( iNAT_T_Required ) {
               flags |= SADB_SAFLAGS_NAT_T;
            }
        }
        else
		{	
			DEBUG_LOG1(_L("Unknown IPsec protocol %d"), prop->iProtocol);
			CleanupStack::PopAndDestroy(5);  //key buffer, identities and ESP UDP encaps data					
			return;
		}	

        updateSPI = inboundspi_node.iSPI;
        Mem::Copy((TUint8*)&addSPI, prop->iSPI.Ptr(), sizeof(TUint32));

        TInetAddr local_addr(iLocalAddr);
        local_addr.SetPort(iIDLocalPort);
        TInetAddr remote_addr(iRemoteAddr);    
        remote_addr.SetPort(iIDRemotePort);
    
        //This will be always outbound
        
        TInt64 time(0), bytes(0);
        TPtrC8 time_ptr(attr_II->iLifeDurationSecs);
        TPtrC8 bytes_ptr(attr_II->iLifeDurationKBytes);
        if (attr_II->iReducedLifeSecs.Length() != 0)
            time_ptr.Set(attr_II->iReducedLifeSecs);

        if (attr_II->iReducedLifeKBytes.Length() != 0)
            bytes_ptr.Set(attr_II->iReducedLifeKBytes);

        ComputeLifetimes_II(time_ptr, bytes_ptr, time, bytes);
        if (time == 0)  //default lifetime applied
            time = DEFAULT_IPSEC_SA_LIFETIME;
        
        if (iHardLifetime > time)
            {
            DEBUG_LOG2(_L("Time %u, hard lifetime %d"),
                    I64LOW(time), I64LOW(iHardLifetime));
            time = iHardLifetime;
            }

        if (attr_II->iEncMode==DOI_TUNNEL)
        {
            TunnelMode = ETrue;
            DEBUG_LOG(_L("TUNNEL MODE"));
#ifdef _DEBUG            
            iLocalAddr1_ID_II.OutputWithScope(addr_buf);
            DEBUG_LOG1(_L("Local ID: net %S"), &addr_buf);
            
            if ((iLocalIDType_II == ID_IPV4_ADDR_SUBNET) || (iLocalIDType_II == ID_IPV6_ADDR_SUBNET))
                {
                iLocalAddr2_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG1(_L(", mask %S"), &addr_buf);
                }
            DEBUG_LOG1(_L(" (port %d)"), iIDLocalPort);
            iRemoteAddr1_ID_II.OutputWithScope(addr_buf);
            DEBUG_LOG1(_L("Remote ID: addr %S"), &addr_buf);
            if ((iRemoteIDType_II == ID_IPV4_ADDR_SUBNET) || (iRemoteIDType_II == ID_IPV6_ADDR_SUBNET))
                {
                iRemoteAddr2_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG1(_L(", mask %S"), &addr_buf);
                }
            DEBUG_LOG1(_L(" (port %d)"), iIDRemotePort);
#endif // _DEBUG            
            
        }
        else {
            TunnelMode = EFalse;        
            DEBUG_LOG(_L("TRANSPORT MODE"));
        }
        DEBUG_LOG1(_L("Sec Lifetime set to %u"),I64LOW(time));
        DEBUG_LOG1(_L("KB Lifetime set to %u"),I64LOW(bytes));

        //Inbound SA.
        sa_data.iSAType = sa_type;
        sa_data.iSeq = iAcquireSeq;
        sa_data.iSrc = remote_addr;
        sa_data.iDst = local_addr;
        sa_data.iProtocol = iIDProtocol;
        sa_data.iSrcIdType = iRemoteIDType_II;
        sa_data.iDstIdType = iLocalIDType_II;       
        sa_data.iSrcIdent.Set((TUint8 *)remote_id->Ptr(), remote_id->Length(), remote_id->Length());
        sa_data.iDstIdent.Set((TUint8 *)local_id->Ptr(), local_id->Length(), local_id->Length());
        sa_data.iPid = iPluginSession->Uid();
        sa_data.iSPI = updateSPI;
        sa_data.iAuthAlg = auth_alg;
        sa_data.iEncrAlg = encr_alg;
        sa_data.iAuthKey.Set(in_authKey);
        sa_data.iEncrKey.Set(in_encrKey);
        TIpsecSALifetime lifetime(0, bytes, time, 0);
        sa_data.iHard = &lifetime;
        sa_data.iReplayWindowLength = prop->iReplayWindowLength;

        if ( (sa_type == SADB_SATYPE_ESP) && (GenExtData.Length() == 0) ) {     
           //
           // Store possible NAT traversal info for IPSEC to do ESP UDP encapsulation correctly
           //
            PFKeyExtDataUtil::BuildUdpEncExtensionData( GenExtData,
                                                        iNAT_D_Flags,
                                                        (flags & SADB_SAFLAGS_NAT_T),
                                                        iHostData->iUseNatProbing,
                                                        (TUint16)iHostData->iEspUdpPort, 
                                                        UDP_KEEPALIVE_TIME,
                                                        iLastRemoteAddr,
                                                        iRemoteOriginalAddr );
        }
        
        if ( GenExtData.Length() ) {
           sa_data.iGenericExtension.Set(GenExtData);      
        }

        if ( TunnelMode ) {
           //
           // Get VPN interface index 
           //
           TUint32 vpnInterfaceIndex = iPluginSession->VpnInterfaceIndex();
           if ( vpnInterfaceIndex != 0 )
               {
               sa_data.iInternalAddress.Init( KAfInet6 );
               sa_data.iInternalAddress.SetScope( vpnInterfaceIndex );
               flags |= SADB_SAFLAGS_INT_ADDR;
               }
           }
        sa_data.iFlags = flags;
		iPluginSession->UpdateSAL(sa_data);
		TIpsecSPI SpiData;
		SpiData.iProtocol = sa_type;
		SpiData.iSrcAddr  = remote_addr;
		SpiData.iDstAddr  = local_addr;
		SpiData.iSPI      = updateSPI;
		SpiData.iInbound  = ETrue;
		iPluginSession->AddIpsecSPIToSAL(iSAId, SpiData);

        sa_data.iFlags &= ~SADB_SAFLAGS_INT_ADDR; //No VPN interface index to outbound SA
        //Outbound SA.
        //First check there's no other SA with the same parameters and
        //erase it if happens (very unlikely, but still possible)
		SpiData.iDstAddr = remote_addr;
		SpiData.iSrcAddr = local_addr;
		SpiData.iSPI     = addSPI;
		SpiData.iInbound = EFalse;
		if ( iPluginSession->DeleteIpsecSpi(iSAId, addSPI, EFalse) )		
        {
            DEBUG_LOG(_L("Deleting previously negotiated IPsec SA"));
			iPluginSession->DeleteIpsecSA(SpiData.iSPI, SpiData.iSrcAddr, SpiData.iDstAddr, 
                                              SpiData.iProtocol);
        }
        //Some changes in the SA, the rest is the same
        sa_data.iSrc = local_addr;
        sa_data.iDst = remote_addr;
        sa_data.iSrcIdType = iLocalIDType_II;
        sa_data.iDstIdType = iRemoteIDType_II;
        sa_data.iSrcIdent.Set((TUint8 *)local_id->Ptr(), local_id->Length(), local_id->Length());
        sa_data.iDstIdent.Set((TUint8 *)remote_id->Ptr(), remote_id->Length(), remote_id->Length());
        sa_data.iPid = addPID;
        sa_data.iSPI = addSPI;
        sa_data.iAuthKey.Set(out_authKey);
        sa_data.iEncrKey.Set(out_encrKey);
        
        iPluginSession->AddSAL(sa_data);
		iPluginSession->AddIpsecSPIToSAL(iSAId, SpiData);
        
        i++;    //To get the correct SPIs from iInboundSPIList
	} //end for

	CleanupStack::PopAndDestroy(5);  //key buffer, identities and ESP UDP encaps data
	
	iAcquirePending = EFalse;
	
}

void CIkev1Negotiation::ComputeLifetimes_II(const TDesC8 &aLifetime, const TDesC8 &aLifesize, TInt64 &aTime, TInt64 &aBytes)
{
    TInt64 maxnum = MAKE_TINT64(0x7fffffffu, 0xffffffffu);
        
    if (Desc8ToTInt64(aLifetime, aTime) != KErrNone)
        {
        DEBUG_LOG(_L("Phase_II Lifetime(sec) Overflowed Setting to maximum value"));
        }
    if (Desc8ToTInt64(aLifesize, aBytes) != KErrNone) {
        DEBUG_LOG(_L("Phase_II Lifetime(kbytes) Overflowed Setting to maximum value"));
    }       
    else 
    {   if (aBytes < maxnum / 1024) //Make sure no overflow
            aBytes = aBytes * 1024; //KB to Bytes
        else
        {
            aBytes = MAKE_TINT64(KMaxTInt, KMaxTUint);            
            DEBUG_LOG(_L("Phase_II Lifetime(kbytes) Overflowed Setting to maximum value"));
        }
    }

}


TBool CIkev1Negotiation::Phase_IExchangeL(const ThdrISAKMP &aHdr)
{       
    if (!ProcessHeaderL(aHdr))
        return EFalse;

    DEBUG_LOG2(_L("---------- Phase %d - Stage %d ----------"),iPhase, iStage);
    if (iPhase == PHASE_I)
    {
        if (aHdr.GetExchange() == ISAKMP_EXCHANGE_ID)
            MainModeReplyL();   //Main Mode
        else
            AggressiveReplyL(); //Aggressive Mode
    }
    else
    {
        QuickModeReplyL();
    }
    return ETrue;
}

TBool CIkev1Negotiation::Phase_IIExchangeL(const ThdrISAKMP &aHdr)
{       
    if (!ProcessHeaderL(aHdr))
        return EFalse;

    DEBUG_LOG2(_L("---------- Phase %d - Stage %d ----------"),iPhase, iStage);

    QuickModeReplyL();

    return ETrue;
}

void CIkev1Negotiation::QuickModeReplyL()
{

	TIkev1IsakmpStream* msg = SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );	
    switch(iStage)
    {
    //INITIATOR cases
    case 3: //Send last message (if no COMMIT)
        msg->IsakmpInit(this);
        msg->IsakmpHashL();
        SendL(*msg);
        iStage = 4;  

        if (iRecvFlags & ISAKMP_HDR_CFLAG)  //Commit Bit set
             return; //Not finished yet. We wait for CONNECTED
        
        //No Commot bit, Update SA database
        UpdateSADatabaseL();
        SetFinished();    //No more stages.
        break;
	case 1: 
		DEBUG_LOG(_L("QuickModeReplyL in Stage 1 ?"));		
		break;
    case 5: //Send last message (extra message waiting for commit(
        //No more processing required
        SetFinished();    //No more stages.
        break;
    //RESPONDER cases
    case 2:
        GetSPIL();
        //rest done in receiveSPI
        break;
    case 4:
        if (iRecvFlags & ISAKMP_HDR_CFLAG)  //Commit Bit set
        {
            DEBUG_LOG(_L("Sending CONNECTED Status message"));
            msg->IsakmpInit(this);

            //HASH Payload only if payload protected with encryption
            if (iFlags & ISAKMP_HDR_EFLAG)
                msg->IsakmpHashL();
        
            msg->IsakmpNotification(CONNECTED, iChosenProp_IIList->At(0)->iProtocol);

            if (iFlags & ISAKMP_HDR_EFLAG)
                msg->IsakmpHashContL();
                
            SendL(*msg);
        }
        SetFinished();
        break;
    }
}

//Builds and sends a Phase I reply for Main Mode
void CIkev1Negotiation::MainModeReplyL()
{
	TIkev1IsakmpStream* msg = SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );		
    TInt          vendor_type;
 
    switch(iStage)
    {
    //
    //RESPONDER replies
    //
    case 2: 
        msg->IsakmpInit(this);
        msg->IsakmpSa();
        if ( iHostData->iDPDHeartBeat != 0 )
           BuildDPDVendorId(*msg);        
        if ( iNatDiscovery ) {
           iNatDiscovery->BuildNatVendorId(*msg); 
           iNatDiscovery->BuildRfcNatVendorId(*msg);
        }   
        SendL(*msg);
        iStage = 3;
        break;
    case 4:
        ComputeNonceL();         //Nonce to be sent
        msg->IsakmpInit(this);
        msg->IsakmpKeyL();
        msg->IsakmpNonce();
        if (((iChosenProposal_I.iAttrList->iAuthMethod == RSA_SIG) ||
             (iChosenProposal_I.iAttrList->iAuthMethod == DSS_SIG)) && !iPeerX509Cert)
           msg->IsakmpCertificateReqL();
		if ( iNatDiscovery ) {
			iNatDiscovery->BuildDiscoveryPayloadsL(*msg, iChosenProposal_I.iAttrList->iHashAlg,
				                                  (TUint8*)iCookie_I.Ptr(), (TUint8*)iCookie_R.Ptr(),
				                                  iLocalAddr, iLastRemoteAddr);    
		}
		SendL(*msg);
        iStage = 5; //next stage
        iFlags |= ISAKMP_HDR_EFLAG; //From now on encryption is used
        if (!ComputeKeysL())    //Generates keying material for encryption stages
            return; //error don't send reply packet
        break;
    case 6:
        msg->IsakmpInit(this);
        
        switch(iChosenProposal_I.iAttrList->iAuthMethod)
        {
        case RSA_SIG:
        case DSS_SIG:
            msg->IsakmpOwnIdentL();
            msg->IsakmpCertificateL();
            msg->IsakmpSignatureL();
            break;
        case PRE_SHARED:
            msg->IsakmpOwnIdentL();
            msg->IsakmpHashL();
            break;
        }
        
        if ( iHostData->iInitialContact &&   
             iRole == INITIATOR &&
             iSARekeyInfo == NULL ) 
        {
           DEBUG_LOG(_L("Constructing INITIAL-CONTACT"));
           msg->IsakmpNotification(DOI_INITIAL_CONTACT, PROTO_ISAKMP); //Not protected by the hash!
        }
        
        SendL(*msg);

		IsakmpPhase1CompletedL();
        break;
    //
    //INITIATOR replies
    //
    case 3:
        ComputeNonceL();         //Nonce to be sent
        msg->IsakmpInit(this);
        msg->IsakmpKeyL();
        msg->IsakmpNonce();
        if ( iChosenProposal_I.iAttrList->iAuthMethod == IKE_A_CRACK && !iPeerX509Cert )
		{
            msg->IsakmpCertificateReqL();
        }
        if ( iHostData->iUseNatProbing )
             vendor_type = EXPANDED_VENDOR_ID;
        else vendor_type = HASH_VENDOR_ID;
        msg->IsakmpVendorId(vendor_type,       
                              (TUint8*)iCookie_I.Ptr(),
                              (TUint8*)iCookie_R.Ptr(), iLocalAddr);
        if ( iNatDiscovery )
		{
           iNatDiscovery->BuildDiscoveryPayloadsL(*msg, iChosenProposal_I.iAttrList->iHashAlg,
                                                 (TUint8*)iCookie_I.Ptr(), (TUint8*)iCookie_R.Ptr(),
                                                 iLocalAddr, iLastRemoteAddr);    
        }   
        SendL(*msg);
        iStage = 4; //next stage
        break;

    case 5:
        iFlags |= ISAKMP_HDR_EFLAG; //From now on encryption is used
        if (!ComputeKeysL())    //Generates keying material for encryption stages
            return; //error don't send reply packet
        
        msg->IsakmpInit(this);
        
        switch(iChosenProposal_I.iAttrList->iAuthMethod)
        {
        case RSA_SIG:
        case DSS_SIG:
            msg->IsakmpOwnIdentL(); //Also fills iOwnIdentPayload!
            msg->IsakmpCertificateL();
            msg->IsakmpSignatureL();
            if (!iPeerX509Cert)
               msg->IsakmpCertificateReqL(); 
            break;
        case PRE_SHARED:
            msg->IsakmpOwnIdentL();
            msg->IsakmpHashL();
            break;
        }
        
        
        if ( iFamiliarPeer && iHostData->iUseInternalAddr )
           msg->IsakmpIntnet(0);  /* null IPV4 address as parameter */

		if ( iHostData->iInitialContact &&
		    !iPluginSession->FindIkev1SADataWithAddr(iRemoteAddr) &&
            iRole == INITIATOR &&
		    iSARekeyInfo == NULL ) 
		{
			DEBUG_LOG(_L("Constructing INITIAL-CONTACT"));
			msg->IsakmpNotification(DOI_INITIAL_CONTACT, PROTO_ISAKMP); //Not protected by the hash!
		}
		
        SendL(*msg);
        iStage = 6; //next stage
        
        break;
    case 7:
        // CRACK authentication going. No actions required here !
        break;
    default:
        DEBUG_LOG1(_L("Main mode Wrong Phase number requested (%d) !!"),iStage);
    }

}


//Builds and sends a Phase I reply for Aggressive Mode
void CIkev1Negotiation::AggressiveReplyL()
{
	TIkev1IsakmpStream* msg = SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );			
	
    switch(iStage)
    {
    case 2://RESPONDER Sends the 2nd msg. of the exchange
        ComputeNonceL();         //Nonce to be sent
        msg->IsakmpInit(this);
        msg->IsakmpSa();
        msg->IsakmpKeyL();
        msg->IsakmpNonce();
        msg->IsakmpOwnIdentL();
        
        if (!ComputeKeysL()) //Needed to compute hash before computing signature
		{
		   return;
		}   
        
        if ( iHostData->iDPDHeartBeat != 0 )
           BuildDPDVendorId(*msg);        
        if ( iNatDiscovery ) {
           iNatDiscovery->BuildNatVendorId(*msg);
           iNatDiscovery->BuildRfcNatVendorId(*msg);
           iNatDiscovery->BuildDiscoveryPayloadsL(*msg, iChosenProposal_I.iAttrList->iHashAlg,
                                                 (TUint8*)iCookie_I.Ptr(), (TUint8*)iCookie_R.Ptr(),
                                                 iLocalAddr, iLastRemoteAddr);    
        }   
        
        switch (iChosenProposal_I.iAttrList->iAuthMethod)
        {
        case RSA_SIG:
        case DSS_SIG:
            msg->IsakmpCertificateL();
            msg->IsakmpSignatureL();
            if (!iPeerX509Cert) //No stored cert so send a CR
            {
                msg->IsakmpCertificateReqL();
            }
            break;
        case PRE_SHARED:
            msg->IsakmpHashL();
            break;
        }
        SendL(*msg);
		
        iStage = 3; //next stage
		
        break;
    case 3://INITIATOR
        iFlags |= ISAKMP_HDR_EFLAG; //From now on encryption is used                
        msg->IsakmpInit(this);
        if ( iNatDiscovery ) {
           iNatDiscovery->BuildDiscoveryPayloadsL(*msg, iChosenProposal_I.iAttrList->iHashAlg,
                                                 (TUint8*)iCookie_I.Ptr(), (TUint8*)iCookie_R.Ptr(),
                                                 iLocalAddr, iLastRemoteAddr);    
        }   
        
        switch (iChosenProposal_I.iAttrList->iAuthMethod)
        {
        case RSA_SIG:
        case DSS_SIG:
            msg->IsakmpCertificateL();   
            msg->IsakmpSignatureL(); 
            break;
        case PRE_SHARED:
            msg->IsakmpHashL();
            break;
        }
		if ( iHostData->iInitialContact &&
		     !iPluginSession->FindIkev1SADataWithAddr(iRemoteAddr) &&
             iRole == INITIATOR &&
		     iSARekeyInfo == NULL ) //Only sent if no ISAKMP SA established
		{
			DEBUG_LOG(_L("Constructing INITIAL-CONTACT"));
			msg->IsakmpNotification(DOI_INITIAL_CONTACT, PROTO_ISAKMP); //Not protected by the hash!
		}

        SendL(*msg);
        IsakmpPhase1CompletedL();       
        break;
	case 4:
		// Aggressive mode as responder completed
		DEBUG_LOG(_L("Aggressive mode as responder completed"));		
		break;
		
    case 7:
        // CRACK authentication going. No actions required here !
        break;
    default:
        DEBUG_LOG1(_L("Main mode Wrong Phase number requested (%d) !! "),iStage);
    }
	
}

void CIkev1Negotiation::SaveISAKMPSAL()
{
	//
	// Create a new IKEv1 SA object       
	//
	TIkev1SAData SaData;
	SaData.iCookie_I       = iCookie_I;
	SaData.iCookie_R       = iCookie_R;
	SaData.iSAId           = iSAId;
	SaData.iSAState        = 0;
	SaData.iInitiator      = (iRole == INITIATOR);
	SaData.iAutoLogin      = iAutoLogin;
	SaData.iDPDSupported   = iDPDSupported;
	SaData.iFamiliarPeer   = iFamiliarPeer;
	SaData.iNAT_T_Required = iNAT_T_Required;
	SaData.iNAT_D_Flags    = iNAT_D_Flags;
	SaData.iIkeData        = iHostData;
	SaData.iLocalAddr      = iLocalAddr;
	SaData.iRemoteAddr     = iRemoteAddr;
	SaData.iDestinAddr     = iLastRemoteAddr;
	SaData.iVirtualIp      = iInternalAddr;
	SaData.iSeq            = iSeq;
	SaData.iPrevExchange   = iPrevExchange;
	SaData.iFlags          = iFlags;
	
	SaData.iEncrAlg        = iChosenProposal_I.iAttrList->iEncrAlg;
	SaData.iHashAlg        = iChosenProposal_I.iAttrList->iHashAlg;
	SaData.iGroupDesc      = iChosenProposal_I.iAttrList->iGroupDesc;
	SaData.iGroupType      = iChosenProposal_I.iAttrList->iGroupType;				
	SaData.iKeyLength      = iChosenProposal_I.iAttrList->iKeyLength;

	TUint32 Lifetime = 0;
	TUint Len = iChosenProposal_I.iAttrList->iLifeDurationSecs.Length();
	if (Len > 0)
	{
		if (Len > sizeof(TUint32))
		{
			Lifetime = KMaxTUint32;
		}
		else    // (len <= sizeof(TUint32))
		{
			Mem::Copy(&Lifetime, iChosenProposal_I.iAttrList->iLifeDurationSecs.Ptr(), Len);
			Lifetime = ByteOrder::Swap32(Lifetime);
			Lifetime = Lifetime >> (sizeof(TUint32)*8 - Len*8);   //To set the correct value (shift in bits)
		}
	}
	SaData.iLifeTimeSecs = Lifetime;
	
	Lifetime = 0;
	Len = iChosenProposal_I.iAttrList->iLifeDurationKBytes.Length();
	if (Len > 0)
	{
		if (Len > sizeof(TUint32))
		{
			Lifetime = KMaxTUint32;
		}
		else    // (len <= sizeof(TUint32))
		{
			Mem::Copy(&Lifetime, iChosenProposal_I.iAttrList->iLifeDurationKBytes.Ptr(), Len);
			Lifetime = ByteOrder::Swap32(Lifetime);
			Lifetime = Lifetime >> (sizeof(TUint32)*8 - Len*8);   //To set the correct value (shift in bits)
		}
	}
	SaData.iLifeTimeKB = Lifetime;

	SaData.iSKEYID     = iSKEYID;
	SaData.iSKEYID_d   = iSKEYID_d;
	SaData.iSKEYID_a   = iSKEYID_a;
	SaData.iSKEYID_e   = iSKEYID_e;
	SaData.iLastIV     = iLastIV;
	SaData.iIV         = iIV;

	if ( iDPDSupported && iHostData->iDPDHeartBeat )
	{
	   //
	   //  Initialize DPD protocol parameters in TIkev1SAData
	   //
		TPtr8 ptr((TUint8*)&SaData.iDPDSequence, sizeof(TUint32));	 
		ptr.SetLength(sizeof(TUint32));
		TRandom::RandomL(ptr);	
		SaData.iDPDSequence &= 0x7fffffff;
		SaData.iDPDRetry = 0;
		SaData.iPendingDPDSequence  = 0;
		SaData.iExpectedDPDSequence = 0;		
	}	
		
	iLastIKEMsgInfo.Store(SaData.iLastIKEMsgInfo);
    SaData.iLastMsg = iLastMsg;
	iPluginSession->CreateIkev1SAL(SaData, iSARekeyInfo);   // Add rekey info later
}

TBool CIkev1Negotiation::ProcessHeaderL(const ThdrISAKMP &aHdr)
{
    //checks on the header    
    if (!CheckCookies(aHdr.GetCookieI(), aHdr.GetCookieR()))
        return EFalse;

    if (iStage == 1)
    {
        if (iPhase == PHASE_I)
        {
            iCookie_I = aHdr.GetCookieI();  //save initiator cookie
            iCookie_R = CreateCookieL(); //create responder cookie
        }
        iMessageId = aHdr.GetMessageId();
    }
    
    //checks on the header    
    if (!CheckPayloadCode(aHdr.GetPayload()))
        return EFalse;

    if (!CheckVersionL(aHdr.GetVersion()))
        return EFalse;
    
    if (!CheckExchangeTypeL(aHdr.GetExchange()))
        return EFalse;

    if (!CheckFlagsL(aHdr.GetFlags()))
        return EFalse;
    iRecvFlags = aHdr.GetFlags();   //Save the flags for later use

    if (!CheckMessageIdL(aHdr.GetMessageId()))
        return EFalse;

    iLengthLeft -= ISAKMP_HEADER_SIZE;  //Updates the length left in the buffer

    //EVEN stages RESPONDER, ODD ones INITIATOR
    DEBUG_LOG(_L("Processing packet..."));
    if (iPhase == PHASE_I)
    {
        switch (iStage)
        {
            case 1:
                if (!ProcessStage1L(aHdr))
                    return EFalse;
                break;
            case 2:
                if (!ProcessStage2L(aHdr))
                    return EFalse;
                break;
            case 3:
                if (iExchange == ISAKMP_EXCHANGE_ID)
                {
                    if (!ProcessStage3MainL(aHdr))
                        return EFalse;
                }
                else    //ISAKMP_EXCHANGE_AGGR
                {
                    if (!ProcessStage3AggrL(aHdr))
                        return EFalse;
                }
                break;
            case 4:
                if (!ProcessStage4L(aHdr))
                    return EFalse;
                break;
            case 5:
                if (!ProcessStage5L(aHdr))
                    return EFalse;
                break;
            case 6:
                if (!ProcessStage6L(aHdr))
                    return EFalse;
                break;
            case 7:
                if (!ProcessStage7L(aHdr))   /* For CRACK negotiation */
                    return EFalse;
                break;
            default:
                return EFalse;
        }
    }
    else    //PHASE_II
    {
        switch (iStage)
        {
            case 1:
                if (!ProcessStage1Phase2L(aHdr))
                    return EFalse;
                break;
            case 2:
                if (!ProcessStage2Phase2L(aHdr))
                    return EFalse;
                break;
            case 3:
                if (!ProcessStage3Phase2L(aHdr))
                    return EFalse;
                break;
            case 4:
                if (!ProcessCONNECTEDL(aHdr))
                    return EFalse;
                break;
            default:
                DEBUG_LOG(_L("Quick Bad Stage"));
        }
    }
    return ETrue;
}

//Process payloads appearing in Stage 1. Responder Role
TBool CIkev1Negotiation::ProcessStage1L(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
	   return EFalse;	

	CleanupStack::PushL(payload);
	
    //SA Payload processing (all modes)
    if (!ProcessSAL(payload->iSa, NULL))
    {
		CleanupStack::PopAndDestroy();  //payload
        return EFalse;
    }

	//Process vendor ID:s  
	ProcessVendorL(payload->iVids);
	
    TUint16 auth_method = iChosenProposal_I.iAttrList->iAuthMethod;
	
	switch (auth_method)
	{
		case RSA_SIG:
		case DSS_SIG:
			//Process the possible CR payloads
			if ( !ProcessCertificateReqArrayL(payload->iCertReqs) ) {
				CleanupStack::PopAndDestroy();  //payload
				return EFalse;
			}
			//Process the possible CERT payloads
			if ( !ProcessCertificateArrayL(payload->iCerts) ) {
				CleanupStack::PopAndDestroy();  //payload
				return EFalse;
			}
			break;

		default:
			break;
	}

	
    if (iExchange == ISAKMP_EXCHANGE_ID)
    {
		if ( payload->iKe || payload->iNonce || payload->iHash || payload->iSign ||
			 payload->iIds->Count() )
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}	
    }
    else //ISAKMP_EXCHANGE_AGGR
    {
		if ( !payload->iKe || !payload->iNonce || (payload->iIds->Count() != 1) )
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}	
		
        //Key Payload processing (all modes)
        if (!ProcessKeyL(payload->iKe))
        {
            CleanupStack::PopAndDestroy();  //payload
            return EFalse;
        }
		TIdentISAKMP* id_payload    = (TIdentISAKMP*)payload->iIds->At(0);  // The first ID		
		TNonceISAKMP* nonce_payload = (TNonceISAKMP*)payload->iNonce;
		
        //Nonce Payload processing
        if (!ProcessNonceL(nonce_payload))
        {
			CleanupStack::PopAndDestroy();  //payload
            return EFalse;
        }
        //ID Payload processing
        if (!CheckIdentL(id_payload))
        {
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
        }
        //Save the ident payload for HASH_I/R computing
        delete iPeerIdentPayload;
		iPeerIdentPayload = NULL;
        iPeerIdentPayloadSize = id_payload->GetLength() - sizeof(TPayloadISAKMP);
        iPeerIdentPayload = new (ELeave) TUint8[iPeerIdentPayloadSize];
        Mem::Copy(iPeerIdentPayload,((TUint8 *)id_payload)+sizeof(TPayloadISAKMP),iPeerIdentPayloadSize);
        
        if ( iNatDiscovery )
		{
		   if ( payload->iNatDs->Count() )
		   {	   
               iNAT_D_Flags = iNatDiscovery->CheckDiscoveryPayloadsL(payload->iNatDs, iChosenProposal_I.iAttrList->iHashAlg,
                                                                    (TUint8*)iCookie_I.Ptr(), (TUint8*)iCookie_R.Ptr(),
                                                                     iLocalAddr, iLastRemoteAddr);
		       if ( iNAT_D_Flags )
			      iLastRemoteAddr.SetPort(FLOATED_IKE_PORT);
		   }
		   else
		   {
			   if ( iLastRemoteAddr.Port() == FLOATED_IKE_PORT )
				   iNAT_D_Flags |= LOCAL_END_NAT; 
		   }	   
        }
        
    }//end aggressive

				   
    CleanupStack::PopAndDestroy();  //payload
    iStage = 2;
    return ETrue;
    
}
                
//Process payloads appearing in Stage 3. Initiator Role
TBool CIkev1Negotiation::ProcessStage2L(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;
	
	CleanupStack::PushL(payload);
	
	iCookie_R = aHdr.GetCookieR();    //Save the responder Cookie	

    //SA Payload processing (all modes)
    if (!ProcessSAL(payload->iSa, NULL)) {
        CleanupStack::PopAndDestroy();  // payload
        return EFalse;
    }

	ProcessVendorL(payload->iVids);
	
    if ( iAutoLogin && (iLocalAddr.Family() == KAFUnspec ) ) {
        User::LeaveIfError( iPluginSession->GetLocalAddress( iLocalAddr ) ); //No local address, get it now (used later with NAT traversal)
    }

    //NOW we know the auth method chosen!
    TUint16 auth_method = iChosenProposal_I.iAttrList->iAuthMethod;

    switch (auth_method)
    {
    case RSA_SIG:
    case DSS_SIG:
        //Process the possible CR payloads
        if ( !ProcessCertificateReqArrayL(payload->iCertReqs) )
		{
            CleanupStack::PopAndDestroy();  //payload
            return EFalse;
        }
	    if (!ProcessCertificateArrayL(payload->iCerts))
		{
		    CleanupStack::PopAndDestroy();  //payload
		    return EFalse;
 		}
        break;
		
    default:
		break;
    }

	if (iExchange == ISAKMP_EXCHANGE_ID)
	{
		if ( payload->iKe || payload->iNonce || payload->iHash || payload->iSign ||
			 payload->iIds->Count() )
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}	
	}
    else //ISAKMP_EXCHANGE_AGGR
    {
		if ( !payload->iKe || 
		     !payload->iNonce || 
		     (!payload->iHash && auth_method == PRE_SHARED) // hash is a must only with PSK
		     || (payload->iIds->Count() != 1) )
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}	
        //Key Payload processing (all modes)
        if (!ProcessKeyL(payload->iKe))
		{
			CleanupStack::PopAndDestroy();  //payload			
            return EFalse;
		}
		TIdentISAKMP* id_payload    = (TIdentISAKMP*)payload->iIds->At(0);  // The first ID		
		TNonceISAKMP* nonce_payload = (TNonceISAKMP*)payload->iNonce;
		
		//Nonce Payload processing
		if (!ProcessNonceL(nonce_payload))
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}
		
		//ID Payload processing
		if (!CheckIdentL(id_payload))
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}
		
        //Save the ident payload for HASH_I/R computing
        delete iPeerIdentPayload;
		iPeerIdentPayload = NULL;
        iPeerIdentPayloadSize = id_payload->GetLength()-sizeof(TPayloadISAKMP);
        iPeerIdentPayload = new (ELeave) TUint8[iPeerIdentPayloadSize];
        Mem::Copy(iPeerIdentPayload,((TUint8 *)id_payload)+sizeof(TPayloadISAKMP),iPeerIdentPayloadSize);
        
        if (!ComputeKeysL())    //Computes the keys to be used. Needed to compute HASH_R
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}	
			
        switch (auth_method)
        {
        case PRE_SHARED:
			if (!ProcessHashL(payload->iHash))
			{
				CleanupStack::PopAndDestroy();  //payload			
				return EFalse;
			}
			DEBUG_LOG(_L("HASH OK!"));
            break;
        case RSA_SIG:
        case DSS_SIG:
        case IKE_A_CRACK:
            //Signature payload processing and checking
            if (!ProcessSignatureL(payload->iSign)) {
				CleanupStack::PopAndDestroy();  //payload	
                return EFalse;
			}	
            if ( auth_method == IKE_A_CRACK ) {
			   CleanupStack::PopAndDestroy();  //payload					
               return StartCRACKAuthL();
            }
			else {
				if (!CertifyRemoteIdentityL(id_payload))
				{
					DEBUG_LOG(_L("ProcessStage2L RSA_SIG CertifyRemoteIdentityL failed"));
					DEBUG_LOG(_L("AUTHENTICATION_FAILED"));
					SetErrorStatus( KKmdIkeAuthFailedErr );
					SendNotifyL(AUTHENTICATION_FAILED);
					CleanupStack::PopAndDestroy();  //payload			
					return EFalse;
				}
			}	
            break;

		default:
			break;
			
		}
		
		if ( iNatDiscovery && payload->iNatDs->Count()) {
			iNAT_D_Flags = iNatDiscovery->CheckDiscoveryPayloadsL(payload->iNatDs, iChosenProposal_I.iAttrList->iHashAlg,
				                                                 (TUint8*)iCookie_I.Ptr(), (TUint8*)iCookie_R.Ptr(),
				                                                  iLocalAddr, iLastRemoteAddr);
			if ( iNAT_D_Flags )
				iLastRemoteAddr.SetPort(FLOATED_IKE_PORT); 
		}
    }
	
	CleanupStack::PopAndDestroy();  //payload
	
    iStage = 3;
    return ETrue;
}

// Process payloads appearing in Stage 4. Order NOT relevant. Only Main Mode
// Handles message: HDR, KE, [HASH(1),] <IDii>PubKey_r, <Ni>PubKey_r for RSAENCR
// Handles message: HDR, KE, Ni for the rest [CERTREQ] in certificates
TBool CIkev1Negotiation::ProcessStage3MainL(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;

	CleanupStack::PushL(payload);

	if ( payload->iSa || !payload->iNonce || payload->iHash || payload->iSign )
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}	
	
    TUint16 auth_method = iChosenProposal_I.iAttrList->iAuthMethod;
	
    //Process payloads
    //Main mode (ONLY)
    //Key Payload processing (all methods)
    if (!ProcessKeyL(payload->iKe))
    {
        CleanupStack::PopAndDestroy();  //payload
        return EFalse;
    }

    if ((auth_method == RSA_SIG) || (auth_method == DSS_SIG))
    {
        //Process the possible CR payloads
        if ( !ProcessCertificateReqArrayL(payload->iCertReqs) )
		{
            CleanupStack::PopAndDestroy();  //payload
            return EFalse;
        }
    }

	TNonceISAKMP* nonce_payload = (TNonceISAKMP*)payload->iNonce;
	
	ProcessVendorL(payload->iVids);
	
	if ( iNatDiscovery && payload->iNatDs->Count() )
	{ 
		iNAT_D_Flags = iNatDiscovery->CheckDiscoveryPayloadsL(payload->iNatDs, iChosenProposal_I.iAttrList->iHashAlg,
		                                                     (TUint8*)iCookie_I.Ptr(), (TUint8*)iCookie_R.Ptr(),
                                                   			  iLocalAddr, iLastRemoteAddr);
		if ( iNAT_D_Flags )
			iLastRemoteAddr.SetPort(FLOATED_IKE_PORT); 
	}
	
    //Nonce Payload processing (all methods)
	TBool Status = ProcessNonceL(nonce_payload);
	if ( Status )
	{
	   iStage = 4;
	}   
	CleanupStack::PopAndDestroy();  //payload
	
    return Status;
}


//Process payloads appearing in 3(Aggressive). Order NOT relevant
TBool CIkev1Negotiation::ProcessStage3AggrL(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;

	CleanupStack::PushL(payload);

    TUint16 auth_method = iChosenProposal_I.iAttrList->iAuthMethod;

    //Mode-dependent processing
    switch (auth_method)
    {
    case PRE_SHARED:
        //Hash payload processing
        if (!ProcessHashL(payload->iHash))
		{
			CleanupStack::PopAndDestroy();  //payload			
            return EFalse;
		}
        DEBUG_LOG(_L("HASH OK!"));
        break;
    case RSA_SIG:
    case DSS_SIG:
        //Certificate payload processing
        if (!ProcessCertificateArrayL(payload->iCerts))
		{
			CleanupStack::PopAndDestroy();  //payload
            return EFalse;
		}
        //Signature payload processing
        if (!ProcessSignatureL(payload->iSign))
		{	
			CleanupStack::PopAndDestroy();  //payload			
            return EFalse;
		}	
        break;
		
    default:
		break;
    }

	ProcessVendorL(payload->iVids);			
	
	if ( iNatDiscovery && payload->iNatDs->Count() ) { 
		iNAT_D_Flags = iNatDiscovery->CheckDiscoveryPayloadsL(payload->iNatDs, iChosenProposal_I.iAttrList->iHashAlg,
			                                                 (TUint8*)iCookie_I.Ptr(), (TUint8*)iCookie_R.Ptr(),
			                                                  iLocalAddr, iLastRemoteAddr);
		if ( iNAT_D_Flags )
			iLastRemoteAddr.SetPort(FLOATED_IKE_PORT); 
	}

	CleanupStack::PopAndDestroy();  //payload
	
    iFlags |= ISAKMP_HDR_EFLAG; //From now on encryption is used
	IsakmpPhase1CompletedL();
	iStage = 4;                 
    return ETrue;

}


//Process payloads appearing in Stage 5. Order NOT relevant. ONLY for MAIN Mode
// Preshared and signatures: HDR, KE, Nr
// RSA Encr. : HDR, <IDir>PubKey_i, <Nr_b>PubKey_i
// RSA Revised Encr. : HDR, <Nr_b>PubKey_i , <KE_b>Ke_r, <IDir>Ke_r
TBool CIkev1Negotiation::ProcessStage4L(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;

	CleanupStack::PushL(payload);

	if (  payload->iSa || !payload->iNonce || !payload->iKe )
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}	

	TUint16 auth_method = iChosenProposal_I.iAttrList->iAuthMethod;	

    if ((auth_method == RSA_SIG) || (auth_method == DSS_SIG))
    {
        //Process the possible CR payloads
        if ( !ProcessCertificateReqArrayL(payload->iCertReqs) )
		{
            CleanupStack::PopAndDestroy();  //payload
            return EFalse;
        }
    }

    //Key Payload processing (all methods)
    if (!ProcessKeyL(payload->iKe))
	{
		CleanupStack::PopAndDestroy();  //payload		
        return EFalse;
	}
	if ( auth_method != PRE_SHARED ) {
		if (!ProcessCertificateArrayL(payload->iCerts)) {
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}
	}	
	TNonceISAKMP* nonce_payload = (TNonceISAKMP*)payload->iNonce;	
	
    //Nonce Payload processing (all modes)
	TBool Status = ProcessNonceL(nonce_payload);
    if (!Status)
    {
		CleanupStack::PopAndDestroy();  //payload
        return EFalse;
    }
	ProcessVendorL(payload->iVids);
	
	if ( iNatDiscovery && payload->iNatDs->Count() )
	{ 
		iNAT_D_Flags = iNatDiscovery->CheckDiscoveryPayloadsL(payload->iNatDs, iChosenProposal_I.iAttrList->iHashAlg,
		                                                   	 (TUint8*)iCookie_I.Ptr(), (TUint8*)iCookie_R.Ptr(),
			                                                  iLocalAddr, iLastRemoteAddr);
		if ( iNAT_D_Flags )
			iLastRemoteAddr.SetPort(FLOATED_IKE_PORT); 
	}

    if ( auth_method == IKE_A_CRACK )
	{
       /*---------------------------------------------------
        * Process message: HDR, [CERT], KEr, Nr, SIG
        * - Verify CRACK signature and if OK
        * - Initialize CRACK authentication
        *---------------------------------------------------*/
		Status = ComputeKeysL();
        if ( Status )  //Generates keying material for encryption stages
		{
			Status = ProcessSignatureL(payload->iSign); 
			if ( Status ) //Signature payload processing and checking
			{
				Status = StartCRACKAuthL();   			   
			} 	   
		}
    }
	else
	{
	    iStage = 5;	    
	}	

	CleanupStack::PopAndDestroy();  //payload			
    return Status;
}


//Process payloads appearing in Stage 6. Order NOT relevant
TBool CIkev1Negotiation::ProcessStage5L(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;

	CleanupStack::PushL(payload);
	if (  payload->iSa || payload->iNonce || payload->iKe )
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}	

    TUint16 auth_method      = iChosenProposal_I.iAttrList->iAuthMethod;
	TIdentISAKMP* id_payload = NULL;
	// ID Payload processing (all modes)
	if ( payload->iIds->Count() != 1 )
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}
	id_payload = (TIdentISAKMP*)payload->iIds->At(0);  // The first ID						
	if (!CheckIdentL(id_payload))
	{
		CleanupStack::PopAndDestroy();  //payload						
		return EFalse;
	}	
		//Save the ident payload for HASH_I/R computing
	delete iPeerIdentPayload;
	iPeerIdentPayload = NULL;		
	iPeerIdentPayloadSize=id_payload->GetLength()-sizeof(TPayloadISAKMP);
	iPeerIdentPayload = new (ELeave) TUint8[iPeerIdentPayloadSize];
	Mem::Copy(iPeerIdentPayload,((TUint8 *)id_payload)+sizeof(TPayloadISAKMP),iPeerIdentPayloadSize);

    //Mode-dependent processing
    switch (auth_method)
    {
    case PRE_SHARED:
        //Hash payload processing
        if (!ProcessHashL(payload->iHash))
		{
			CleanupStack::PopAndDestroy();  //payload			
            return EFalse;
		}
		DEBUG_LOG(_L("HASH OK!"));		
        break;
    case RSA_SIG:
    case DSS_SIG:
        //Certificate payload processing
        if (!ProcessCertificateArrayL(payload->iCerts))
		{
			CleanupStack::PopAndDestroy();  //payload									
            return EFalse;
		}
		if (!CertifyRemoteIdentityL(id_payload))
		{
    		DEBUG_LOG(_L("ProcessStage5L RSA_SIG CertifyRemoteIdentityL failed"));
			DEBUG_LOG(_L("AUTHENTICATION_FAILED"));
			SetErrorStatus( KKmdIkeAuthFailedErr );
			SendNotifyL(AUTHENTICATION_FAILED);
			CleanupStack::PopAndDestroy();  //payload			
			return EFalse;
		}
        //Signature payload processing and checking
        if (!ProcessSignatureL(payload->iSign))
		{
			CleanupStack::PopAndDestroy();  //payload
            return EFalse;
		}
		//Process the possible CR payloads (needed if we are responder)
		if ( iRole == RESPONDER )
		{	
		   if ( !ProcessCertificateReqArrayL(payload->iCertReqs) )
		   {
			  CleanupStack::PopAndDestroy();  //payload
			  return EFalse;
		   }
		   if ( !iOwnCert ) 
			   ReadOwnCertL(); // Peer does not required a specific cert. Get any
		}   
        break;
		
    default:
		break;
    }//end switch

    // Process notification payloads
	TInt i = 0;
	while ( i < payload->iNotifs->Count() ) {
		if (!ProcessNotificationL(payload->iNotifs->At(i)))
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}		
        i ++; 		
	}	

	ProcessVendorL(payload->iVids);

	CleanupStack::PopAndDestroy();  //payload			
	
    if (iExchange == ISAKMP_EXCHANGE_ID)
         iStage = 6;
    else iStage = 3;

    return ETrue;
}


//Process payloads appearing in Stage 6(Main)  Order NOT relevant
TBool CIkev1Negotiation::ProcessStage6L(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;

	CleanupStack::PushL(payload);
	if (  payload->iSa || payload->iNonce || payload->iKe )
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}	
	

    TUint16 auth_method = iChosenProposal_I.iAttrList->iAuthMethod;
	TIdentISAKMP* id_payload = NULL;
    //ID Payload processing (all modes)
	if ( payload->iIds->Count() != 1 )
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}
	id_payload = (TIdentISAKMP*)payload->iIds->At(0);  // The first ID						
	if (!CheckIdentL(id_payload))
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}	
	//Save the ident payload for HASH_I/R computing
    delete iPeerIdentPayload;
	iPeerIdentPayload = NULL;		
	iPeerIdentPayloadSize=id_payload->GetLength()-sizeof(TPayloadISAKMP);
	iPeerIdentPayload = new (ELeave) TUint8[iPeerIdentPayloadSize];
	Mem::Copy(iPeerIdentPayload,((TUint8 *)id_payload)+sizeof(TPayloadISAKMP),iPeerIdentPayloadSize);

    //Mode-dependent processing
    switch (auth_method)
    {
    case PRE_SHARED:
        //Hash payload processing
        if (!ProcessHashL(payload->iHash))
		{
			CleanupStack::PopAndDestroy();  //payload			
            return EFalse;
		}
		DEBUG_LOG(_L("HASH OK!"));		
        break;
    case RSA_SIG:
    case DSS_SIG:
		//Certificate payload processing
		if (!ProcessCertificateArrayL(payload->iCerts))
		{
			CleanupStack::PopAndDestroy();  //payload									
			return EFalse;
		}
		if (!CertifyRemoteIdentityL(id_payload))
		{
			DEBUG_LOG(_L("ProcessStage6L RSA_SIG CertifyRemoteIdentityL failed"));
			DEBUG_LOG(_L("AUTHENTICATION_FAILED"));
			SetErrorStatus( KKmdIkeAuthFailedErr );
			SendNotifyL(AUTHENTICATION_FAILED);
			CleanupStack::PopAndDestroy();  //payload			
			return EFalse;
		}
		//Signature payload processing and checking
		if (!ProcessSignatureL(payload->iSign))
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}
        break;
    default:
        break;
    }

	// Process notification payloads
	TInt i = 0;
	while ( i < payload->iNotifs->Count() ) {
		if (!ProcessNotificationL(payload->iNotifs->At(i)))
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}		
		i ++; 		
	}	

	ProcessVendorL(payload->iVids);	
    // Internal address payload processing
    ProcessIntAddrL(payload->iIaddr);

	CleanupStack::PopAndDestroy();  //payload	

	return IsakmpPhase1CompletedL();	
	
}

//Process payloads appearing in Stage 7 = CRACK authentication going
TBool CIkev1Negotiation::ProcessStage7L(const ThdrISAKMP &aHdr)
{
TBool status = ETrue;
TInt  crack_status;

    if ( iCRACKneg ) {
       crack_status = iCRACKneg->ExecuteCRACKMsgL(aHdr);
       
       switch ( crack_status ) {
           
           case CRACK_SUCCESS:
               /*-------------------------------------------------------
                * CRACK authentication has been succesfully completed
                * Take actions to start Quick mode negotiation
                *------------------------------------------------------*/
               delete iCRACKneg;
               iCRACKneg = NULL;
               iLastIV.Copy(iIV);  //Saves last IV in Phase 1
               DEBUG_LOG(_L("Last IV Saved!"));
               IsakmpPhase1CompletedL();
               break;
               
           case CRACK_CONTINUE:
               /*----------------------------------------------------------
                * CRACK authentication continues, no further actions needed
                *----------------------------------------------------------*/
               break;

           case CRACK_IGNORE_MSG:
               /*----------------------------------------------------------
                * CRACK authentication continues, received message ignored
                *----------------------------------------------------------*/
               status = EFalse;            
               break;
               
           default:
               /*----------------------------------------------------------
                * CRACK authentication failed, negotiation failed
                *----------------------------------------------------------*/
			   LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
			                  R_VPN_MSG_VPN_GW_AUTH_FAIL,
			                  KKmdIkeAuthFailedErr,
			                  iPluginSession->VpnIapId(),
			                  &iRemoteAddr );		   			   
               status    = EFalse;                         
			   SetErrorStatus(KKmdIkeAuthFailedErr);
               AcquireSAErrorResponse(KKmdIkeAuthFailedErr);
               break;

       }       
    }

    return status;
}

//Called as a RESPONDER for PHASE_II
//Checks HASH(1),SA,KE,NONCE,[ID,ID] from INITIATOR
TBool CIkev1Negotiation::ProcessStage1Phase2L(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;

	CleanupStack::PushL(payload);
	
	if (  !payload->iSa || !payload->iNonce )
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}	
	
    //Payload Processing

    CProposal_IIList *recv_proposals = new (ELeave) CProposal_IIList(1);    //, *prop
    CleanupStack::PushL(recv_proposals);

    //SA
    if (!ProcessSAL(payload->iSa, (TUint8 *)recv_proposals))
    {
        CleanupStack::PopAndDestroy(2); //recv_proposals + payload
        return EFalse;
    }

    //ID payloads (if existing)
	if ( payload->iIds->Count() == 2 )
	{
       if (!ProcessStage1_II_IDsL(payload->iIds->At(0), payload->iIds->At(1), recv_proposals))
       {
          CleanupStack::PopAndDestroy(2); //recv_proposals + payload
          return EFalse;
       }
	}
	else if ( payload->iIds->Count() != 0 )
	{
		DEBUG_LOG(_L("Unsupported Phase II ID payload count"));		
		CleanupStack::PopAndDestroy(2); //recv_proposals + payload
		return EFalse;
	}	
    //After ID to know what address to search in the "SAD"
	TInt err = BuildProposals2L();
    if (err != KErrNone)
    {
        DEBUG_LOG(_L("NO_PROPOSAL_CHOSEN: No policy matching"));
        SendNotifyL(NO_PROPOSAL_CHOSEN);
        CleanupStack::PopAndDestroy(2); //recv_proposals + payload
        return EFalse;
    }

    //Contains the transform nums matching if multiple proposals
    CTransModifierList *trans_array = new (ELeave) CTransModifierList(1);
    CleanupStack::PushL(trans_array);

    TInt num = iProposal_IIList->MultiMatchL(recv_proposals, iRole == RESPONDER, trans_array);//If RESPONDER relaxed comparison (no lifetimes checked)
    iProposalNum = num; // Set to num not 1
#ifdef _DEBUG
    TBuf<128> err_buf;
#endif    
    if (num < 0)
    {
#ifdef _DEBUG    
        err_buf.Copy(_L("NO_PROPOSAL_CHOSEN: Phase II proposal not accepted - "));
        AppendAttributeError(num, err_buf);
        DEBUG_LOG(err_buf);
#endif        
        SendNotifyL(NO_PROPOSAL_CHOSEN);
        CleanupStack::PopAndDestroy(3); //transarray + recv_proposals + payload
        return EFalse;
    }

    //Copy the chosen transform
    //Actually is the same one as recv_proposals because we should always receive a single proposal with a single transform.
    CreateChosenProposalL(recv_proposals, num, trans_array);

    CleanupStack::PopAndDestroy(2); //transarray + recv_proposals

    //Process the possible NOTIFICATION payloads
    for (TInt i = 0; i < payload->iNotifs->Count(); i++)
    {
        if (!ProcessNotificationL(payload->iNotifs->At(i)))
        {
            CleanupStack::PopAndDestroy();  //payload
            return EFalse;
        }
    }
    
    if (!ProcessNonceL(payload->iNonce))
	{
		CleanupStack::PopAndDestroy();  //payload		
        return EFalse;
	}
    //HASH
    if (!ProcessHash2L(aHdr, payload->iHash, payload->iPadding))
	{
		CleanupStack::PopAndDestroy();  //payload				
        return EFalse;
	} 
    
    if (!ProcessKeyL(payload->iKe))
	{
		CleanupStack::PopAndDestroy();  //payload								
        return EFalse;
	}

	ProcessVendorL(payload->iVids);		
	
    if ( iNatDiscovery ) {
       iNAT_D_Flags |= iNatDiscovery->GetPeerOriginalAddress(payload->iNatOa, iRemoteOriginalAddr, iChosenProp_IIList);
    }
	
	CleanupStack::PopAndDestroy();  //notif_payload_array	
    iStage = 2;
    return ETrue;
}

//Creates a new proposal list from a selected part of another list (Certain proposal and transforms) 
void CIkev1Negotiation::CreateChosenProposalL(CProposal_IIList* aPropList, TInt aPropNum, CTransModifierList *aTransArray)
{

    CProposal_II *prop, *new_propII;
    TAttrib_II *attr_II;
    TChosenAttrib_II *new_attr_II;
    TInt count = aPropList->Count();
    TInt i = 0;
    TInt j = 0;

    //
    // Find selected proroposal from list
    //
    while ( i < count)
    {
        if (aPropList->At(i)->iProposalNum == aPropNum)
            break;
        i ++;
    }
    
    TTransModifier *tmodif;
    TInt64 own_time, own_bytes, peer_time, peer_bytes, responderLifetime_time, responderLifetime_bytes;
    delete iChosenProp_IIList;  //Must be erased because can contain data from previous retransmissions
	iChosenProp_IIList = NULL;	
    iChosenProp_IIList = new (ELeave) CProposal_IIList(1);
    while ( i < count )
    {
        prop = aPropList->At(i);
        if ( prop->iProposalNum != aPropNum)
            break; // Stop, another Proposal 

        new_propII = new (ELeave) CProposal_II();
        CleanupStack::PushL(new_propII);
        new_propII->ConstructL(1);
        iChosenProp_IIList->AppendL(new_propII);
        CleanupStack::Pop();    //new_propII safe

        new_propII->iProtocol = prop->iProtocol;
        new_propII->iNumTransforms = 1; //We only choose 1 transform for each proposal
        new_propII->iProposalNum   = (TUint8)aPropNum;
        new_propII->iSPI.Copy(prop->iSPI);
        tmodif = aTransArray->At(j);
        new_propII->iReplayWindowLength = tmodif->iReplayWindowLength;

        attr_II = prop->iAttrList->At(tmodif->iTransNum);   //look for the chosen transform in the prop
        ComputeLifetimes_II(tmodif->iReducedLifeSecs, tmodif->iReducedLifeKBytes, own_time, own_bytes);
        ComputeLifetimes_II(attr_II->iLifeDurationSecs, attr_II->iLifeDurationKBytes, peer_time, peer_bytes);
        ComputeLifetimes_II(tmodif->iResponderLifetimeSecs, tmodif->iResponderLifetimeKBytes, responderLifetime_time, responderLifetime_bytes);
        
        //Only copy the chosen transform
        new_attr_II = new (ELeave) TChosenAttrib_II();
        CleanupStack::PushL(new_attr_II);
        new_attr_II->Copy(*attr_II);
        if ((peer_time > own_time) && (own_time != 0))
        {
            new_attr_II->iReducedLifeSecs.Set(tmodif->iReducedLifeSecs);
            DEBUG_LOG1(_L("Lifetime bigger than the one set. Reducing to %d"), own_time);
        }
        else
            new_attr_II->iReducedLifeSecs.Set(NULL, 0);
            
        if ((peer_bytes > own_bytes) && (own_bytes != 0))
        {
            new_attr_II->iReducedLifeKBytes.Set(tmodif->iReducedLifeKBytes);
            DEBUG_LOG1(_L("Lifesize bigger than the one set. Reducing to %d"), own_bytes);
        }
        else
            new_attr_II->iReducedLifeKBytes.Set(NULL, 0);
        
        if (responderLifetime_time)
        {
            new_attr_II->iResponderLifetimeSecs = tmodif->iResponderLifetimeSecs;
            DEBUG_LOG1(_L("Responder lifetime set to %d"), responderLifetime_time);        
        }

        if (responderLifetime_bytes)
        {
            new_attr_II->iResponderLifetimeKBytes = tmodif->iResponderLifetimeKBytes;
            DEBUG_LOG1(_L("Responder lifetime in bytes set to %d"), responderLifetime_bytes);        
        }
        
        new_propII->iAttrList->AppendL(new_attr_II);
        CleanupStack::Pop();    //new_attrII safe

        j++;  // Next transform modifer
        i++;  // Next proposal
        
    }
}


//Called as a INITIATOR for PHASE_II
//Checks HASH(1),SA, ,NONCE,[KE] [ID,ID] from RESPONDER
TBool CIkev1Negotiation::ProcessStage2Phase2L(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;

	CleanupStack::PushL(payload);

	if (  !payload->iSa || !payload->iNonce  )
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}	

    CProposal_IIList *recv_proposals = new (ELeave) CProposal_IIList(1);
    CleanupStack::PushL(recv_proposals);

	//SA
	if (!ProcessSAL(payload->iSa, (TUint8 *)recv_proposals))
	{
		CleanupStack::PopAndDestroy(2); //recv_proposals + payload
		return EFalse;
	}

	//ID payloads (if existing)
	if ( payload->iIds->Count() == 2 )
	{
		if (!ProcessStage2_II_IDsL(payload->iIds->At(0), payload->iIds->At(1)))
		{
			CleanupStack::PopAndDestroy(2); //recv_proposals + payload
			return EFalse;
		}
	}
	else if ( payload->iIds->Count() != 0 )
	{
		DEBUG_LOG(_L("Unsupported Phase II ID payload count"));		
				CleanupStack::PopAndDestroy(2); //recv_proposals + payload
		return EFalse;
	}	
	
    //Contains the transform nums matching if multiple proposals
    CTransModifierList *trans_array = new (ELeave) CTransModifierList(1);
    CleanupStack::PushL(trans_array);

    //Check the received proposals match the proposed one (Got from
    //acquire msg.) 
   	TInt num = iProposal_IIList->MultiMatchL(recv_proposals, iRole == RESPONDER, trans_array);//If RESPONDER relaxed comparison (no lifetimes checked)
#ifdef _DEBUG   	
    TBuf<128> err_buf;
#endif    
    if (num < 0)
    {
#ifdef _DEBUG    
        err_buf.Copy(_L("BAD_PROPOSAL_SYNTAX: Phase II reply doesn't match proposal - "));
        AppendAttributeError(num, err_buf);
        DEBUG_LOG(err_buf);
#endif        
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(BAD_PROPOSAL_SYNTAX);
        CleanupStack::PopAndDestroy(3); //transarray + recv_proposals + payload
        return EFalse;
    }
    // iProposalNum set to correspond local proposal numbering
    iProposalNum = trans_array->At(0)->iPropNum; 
    //Copy the chosen transform
    CreateChosenProposalL(recv_proposals, num, trans_array);

    CleanupStack::PopAndDestroy(2); //transarray + recv_proposals

	//Process the possible NOTIFICATION payloads
	for (TInt i = 0; i < payload->iNotifs->Count(); i++)
	{
		if (!ProcessNotificationL(payload->iNotifs->At(i)))
		{
			CleanupStack::PopAndDestroy();  //payload
			return EFalse;
		}
	}
	//NONCE
   	if (!ProcessNonceL(payload->iNonce))
   	{
    	CleanupStack::PopAndDestroy();  //payload		
	    return EFalse;
	}
	//HASH
	if (!ProcessHash2L(aHdr, payload->iHash, payload->iPadding))
	{
		CleanupStack::PopAndDestroy();  //payload				
		return EFalse;
	} 
	//KEY (if present (PFS))
	if (!ProcessKeyL(payload->iKe))
	{
	    SetErrorStatus( KKmdIkeNegotFailed );
		SendNotifyL(PAYLOAD_MALFORMED);					
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}   

	ProcessVendorL(payload->iVids);
	
	if ( iNatDiscovery ) {
		iNAT_D_Flags |= iNatDiscovery->GetPeerOriginalAddress(payload->iNatOa, iRemoteOriginalAddr, iChosenProp_IIList);
	}

    if ( iRecvFlags & ISAKMP_HDR_CFLAG )   //Commit Bit set       
    {
        iFlags |= ISAKMP_HDR_CFLAG; //Sets the Commit bit if this side set it else
        DEBUG_LOG(_L("SAD update delayed until CONNECTED received"));
    }

	CleanupStack::PopAndDestroy();  //payload				
    iStage = 3;
    return ETrue;

}

TBool CIkev1Negotiation::ProcessStage3Phase2L(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;

	CleanupStack::PushL(payload);

	if ( payload->iSa || payload->iNonce || payload->iKe || payload->iIds->Count() != 0 ) 
	{
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}	
	
    //Hash Payload
	TBool Status = ProcessHash2L(aHdr, payload->iHash, payload->iPadding);
    if ( Status )
	{
       	//END OF THE PHASE II (Quick mode) negotiation.
		//Now we need to update the PFKEY SA database
		ProcessVendorL(payload->iVids);				
		UpdateSADatabaseL();
		iStage = 4;
	}
	CleanupStack::PopAndDestroy();  //payload		
    return Status;
}

TBool CIkev1Negotiation::ProcessCONNECTEDL(const ThdrISAKMP &aHdr)
{
	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *this, iDebug);
	if (!payload)
		return EFalse;

	CleanupStack::PushL(payload);

	if ( !payload->iHash || (payload->iNotifs->Count() != 1) || payload->iSa ||
		 payload->iNonce || payload->iKe || (payload->iIds->Count() != 0) ) 
	{
		DEBUG_LOG(_L("PAYLOAD_MALFORMED (no hash or notfic payload)"));
		SetErrorStatus( KKmdIkeNegotFailed );
		SendNotifyL(PAYLOAD_MALFORMED);
		CleanupStack::PopAndDestroy();  //payload
		return EFalse;
	}	

    //Checks if the hash value is OK. Here because need the
    //notification payload
	const TNotificationISAKMP* notif_payload = payload->iNotifs->At(0);
	TBool Status = VerifyInformationalHashL(payload->iHash, notif_payload, iMessageId);	 
    if ( Status )
    {  //Hash OK
		if ((notif_payload->GetDOI() == IPSEC_DOI) && (notif_payload->GetMsgType() == CONNECTED))
		{
			//END OF THE PHASE II (Quick mode) negotiation.
			//Now we need to update the PFKEY SA database
			DEBUG_LOG(_L("CONNECTED message received. Updating SAD"));
			UpdateSADatabaseL();
			iStage = 5;			
		}
    }
    else   
    {
		DEBUG_LOG(_L("AUTHENTICATION_FAILED (Informational hash)"));
		SetErrorStatus( KKmdIkeAuthFailedErr );
		SendNotifyL(AUTHENTICATION_FAILED);
    }
	CleanupStack::PopAndDestroy();  //payload		
    return Status;
}

//returns KErrNone if OK, otherwise error already treated.
TBool CIkev1Negotiation::ProcessSAL(const TSAISAKMP *aSA, TUint8 *aRecvProposals)
{
    //payload not present
    if (!aSA)
    {
        DEBUG_LOG(_L("NO SA PAYLOAD"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(PAYLOAD_MALFORMED);
        return EFalse;
    }

    //requires a special length check to know the proposals and transforms are not bigger than 
    //the size specified in th SA
    TUint32 SALengthLeft = aSA->GetLength() - aSA->Size();

    TUint8 next_payload = aSA->GetPayload();
    if ((next_payload == ISAKMP_PAYLOAD_P) || (next_payload == ISAKMP_PAYLOAD_T))
    {
        DEBUG_LOG(_L("INVALID_PAYLOAD_TYPE (Bad next payload for the SA)"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(INVALID_PAYLOAD_TYPE);   // Payloads and transforms are processed as a part of the SA,
        return EFalse;
    }

    TUint32 doi=aSA->GetDOI();
    if (!CheckDOI(doi))
    {
        DEBUG_LOG(_L("DOI_NOT_SUPPORTED in SA payload"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(DOI_NOT_SUPPORTED);  //send the informational exchange
        return EFalse;
    }

    if (iStage == 1)
        iDOI = doi;

    //Process SITuation depending on DOI. In this implementation always is IPSEC DOI
    if (!CheckSituationL(aSA->GetSIT()))
        return EFalse;

    if (aSA->HasLDId()) //If no Labeled Domain Identifier no more processing for the SA
    {
        DEBUG_LOG(_L("Label Domain Identifier (LDI) not supported"));
        return EFalse;
    }
    
    if (iPhase==PHASE_I)    //Only used in Phase_I. PHASE_II hashs check it directly from received paylaod
    {
        //Store the SA payload for further calculations in Hash.
        if (iRole==RESPONDER)
        {
            iSAPayloadSize = aSA->GetLength()-sizeof(TPayloadISAKMP);
            delete iSAPayload;
			iSAPayload = NULL;
            iSAPayload = new (ELeave) (TUint8[iSAPayloadSize]); //Generic payload NOT included
            Mem::Copy(iSAPayload,(((TUint8 *)aSA)+sizeof(TPayloadISAKMP)),iSAPayloadSize);
        }
    }
    const TPayloadISAKMP *payload = aSA->Payload();
    return ProcessProposalsL(payload, SALengthLeft, aRecvProposals);
}


//Do the process required for proposals and transforms. The aPayload given must be a proposal
TBool CIkev1Negotiation::ProcessProposalsL(const TPayloadISAKMP *aPayload, TUint32 aLengthLeft, TUint8 *aRecvProposals)
{
    TProposalISAKMP *proposal;
    const TPayloadISAKMP *ppayload=aPayload;
    TUint8 payType = ppayload->GetPayload();
    TUint32 len_left;
    CProposal_IIList *recv_proposals = (CProposal_IIList *)aRecvProposals;
    CProposal_II *auxProp_II;
    TBool found = EFalse;   //At least 1 transform matching

    //Many Proposals. The RESPONDER MUST choose a transform for each proposal or reject the 
    //full suite of attributes.
    do  
    {       
        //General payload check
        if ((payType != ISAKMP_PAYLOAD_NONE) && (payType != ISAKMP_PAYLOAD_P))
        {
            DEBUG_LOG(_L("INVALID_PAYLOAD_TYPE (Bad next payload for the proposal)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_PAYLOAD_TYPE);
            return EFalse;
        }

        if (aPayload->GetReserved() != 0)   //Must be always 0
        {
            DEBUG_LOG(_L("INVALID RESERVED FIELD"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(PAYLOAD_MALFORMED);
            return EFalse;
        }
        
        //requires special length check
        if ((aPayload->GetLength() < MIN_ISAKMP_PAYLOAD_SIZE) || (aPayload->GetLength() > aLengthLeft))
        {
            DEBUG_LOG(_L("BAD PAYLOAD SIZE"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(PAYLOAD_MALFORMED);
            return EFalse;
        }

        aLengthLeft -= aPayload->GetLength();   //dcreases the length of the prop. and its transforms

        proposal = TProposalISAKMP::Ptr(ppayload);
        
        
        if (!CheckProtocolL(proposal->GetProtocol()))
            return EFalse;

        if (!CheckSPIL(proposal))
            return EFalse;

        len_left = proposal->GetLength() - (proposal->Size() + proposal->GetSPISize()); 
        //len_left contains the length of the transforms only

        TInt ret = KErrNotFound;
        if (iPhase == PHASE_I)
        {
            iChosenProposal_I.iProtocol = proposal->GetProtocol();
            iChosenProposal_I.iSPI.Copy((TUint8 *)proposal->SPI(), proposal->GetSPISize());
            iChosenProposal_I.iNumTransforms = 1;   //Phase I only one transf chosen
            iChosenProposal_I.iProposalNum = proposal->GetNum();//Not compulsory but preferable to speed up the search process in the peer
            
            ret = ProcessTransformsL(ppayload,len_left);
            
            if (ret == KErrNone)
                return ETrue;//valid transform found
            else if (ret != KErrNotFound)
            {
                DEBUG_LOG(_L("ATTRIBUTES_NOT_SUPPORTED"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(ATTRIBUTES_NOT_SUPPORTED);
                return EFalse;
            }
            //If not found there may be other proposal to check
        }
        else    //PHASE_II
        {
            auxProp_II = new (ELeave) CProposal_II();
            CleanupStack::PushL(auxProp_II);
            auxProp_II->ConstructL(proposal->GetNumTrans());
            auxProp_II->iProtocol=proposal->GetProtocol();
            auxProp_II->iSPI.Copy((TUint8 *)proposal->SPI(), proposal->GetSPISize());
            auxProp_II->iNumTransforms = proposal->GetNumTrans();   //Number of transforms in the proposal
            auxProp_II->iProposalNum = proposal->GetNum();  //Proposal num
            
            ret = ProcessTransforms2L(ppayload, auxProp_II, len_left);
            if (ret==KErrNone)//valid transform found
            {   
                //Adds the new proposal to the list of chosen proposals
                recv_proposals->AppendL(auxProp_II);
                CleanupStack::Pop();    //auxProp_II safe
                found = ETrue;
                //go for the next proposal
            }
            else
            {
                DEBUG_LOG(_L("ATTRIBUTES_NOT_SUPPORTED"));
                SendNotifyL(ATTRIBUTES_NOT_SUPPORTED);
                CleanupStack::PopAndDestroy();  //delete auxProp_II;    //delete the current proposal
                return EFalse;
            }
        }

        payType = ppayload->GetPayload();
        ppayload = ppayload->Next();    //Next payload if there's any
    } while (payType!=ISAKMP_PAYLOAD_NONE); //Proposal loop

    if (!found)
    {
        DEBUG_LOG(_L("ATTRIBUTES_NOT_SUPPORTED"));
        SendNotifyL(ATTRIBUTES_NOT_SUPPORTED);
        return EFalse;
    }

    return found;
}

//processes all the  PHASE I transforms. The parameter payload is the proposal containing
//the transforms to be able to access the # of transforms.
//Returns if any transform accepted (KErrNone) or not (KErrNotFound) or processing error (KErrGeneral)
TInt CIkev1Negotiation::ProcessTransformsL(const TPayloadISAKMP *aPayload,TUint32 aLengthLeft)
{
    DEBUG_LOG(_L("-> CIkev1Negotiation::ProcessTransformsL()"));
    TUint16 reason;             
    TInt ret = KErrGeneral;
     const TTransformISAKMP *transf;
    const TProposalISAKMP *proposal=TProposalISAKMP::Ptr(aPayload);
    //First transform. Not Next() because would be the next proposal or non-sa payload
    const TPayloadISAKMP *tpayload=proposal->Payload();
    
    TInt payType = tpayload->GetPayload();  //Type of the payload following the first transform
    transf = TTransformISAKMP::Ptr(tpayload);
    
    TInt numTransf  = (TInt)transf->GetNum();  // First transform number
    TInt lastTransf = numTransf + (TInt)proposal->GetNumTrans(); // Last transform number
    
    while ( numTransf < lastTransf ) 
    {
        //only permited payload codes
        if ((payType != ISAKMP_PAYLOAD_NONE) && (payType != ISAKMP_PAYLOAD_T))
        {
            DEBUG_LOG(_L("INVALID_PAYLOAD_TYPE (Bad next payload for the transform)"));
            SendNotifyL(INVALID_PAYLOAD_TYPE);
            return KErrGeneral;
        }

        if (tpayload->GetReserved() != 0)   //Must be always 0
        {
            DEBUG_LOG(_L("INVALID RESERVED FIELD"));
            SendNotifyL(PAYLOAD_MALFORMED);
            return KErrGeneral;
        }
        
        //requires special length check
        if ((tpayload->GetLength() < MIN_ISAKMP_PAYLOAD_SIZE) || (tpayload->GetLength() > aLengthLeft))
        {
            DEBUG_LOG(_L("BAD PAYLOAD SIZE"));
            SendNotifyL(PAYLOAD_MALFORMED);
            return KErrGeneral;
        }
        
        if (!CheckTransformID(PROTO_ISAKMP,transf->GetID()))
            {
            DEBUG_LOG(_L(" Continue"));

            numTransf++;                          // Next supposed transform #              
            payType = tpayload->GetPayload();
            tpayload = tpayload->Next();    //next payload (transform)
            transf = TTransformISAKMP::Ptr(tpayload);

            continue;   //If fails, transform discarded!, not error
            }
        
        if ( transf->GetNum() != numTransf )  //Not the correct #
        {
            DEBUG_LOG1(_L("BAD_PROPOSAL_SYNTAX (Non conscutive transform number (%d)"),transf->GetNum());
            SendNotifyL(BAD_PROPOSAL_SYNTAX);
            DEBUG_LOG(_L("<- CIkev1Negotiation::ProcessTransformsL() KErrGeneral"));
            return KErrGeneral;
        }

        if (transf->GetReserved() != 0 )    //Should be always 0
        {
            DEBUG_LOG(_L("INVALID RESERVED FIELD"));
            SendNotifyL(PAYLOAD_MALFORMED);
            DEBUG_LOG(_L("<- CIkev1Negotiation::ProcessTransformsL() KErrGeneral"));
            return KErrGeneral;
        }

        numTransf++;                          // Next supposed transform #              
        //Attributes to be checked depending on Transf ID
        TAttrib attrib;
        
        ret = ProcessAttributesL(tpayload, &attrib);
        
        if (ret != KErrNone)
        {
            if (ret != KErrNotFound)
                {
                DEBUG_LOG(_L("<- CIkev1Negotiation::ProcessTransformsL() KErrGeneral"));
                return KErrGeneral; //Error in the attributes. Already reported
                }
            //Not accepted but correct
        }
        else    // Accepted attributes if (AttrChosen(attrib))
        {  
            //Checks the response or proposal is the same as one of our proposals
            TAttrib *attr_list = iProposal_I.iAttrList;
            TInt ret = KErrNotFound;
#ifdef _DEBUG            
            TBuf<256> buf;
#endif            
            while (attr_list && (ret != KErrNone) )
            {
                ret = attrib.Compare(*attr_list, iRole==RESPONDER); //If RESPONDER relaxed comparison (no lifetimes checked)                
                if (ret != KErrNone)
                {
#ifdef _DEBUG                
                    DEBUG_LOG1(_L("Transform #%d not matching proposal Reason: "), attrib.iTransformNum);
                    AppendAttributeError(ret, buf);
                    DEBUG_LOG(buf);
#endif                    
                }
                attr_list = attr_list->iNext;   //next transform proposed

            }
            if (ret == KErrNone)
            {
               *iChosenProposal_I.iAttrList = attrib;       
               DEBUG_LOG(_L("<- CIkev1Negotiation::ProcessTransformsL() KErrNone"));
               return KErrNone; //If the attibute are supported there's no need to check more SA
            }   
            else    //No proposal matches
            {
               if ( numTransf == lastTransf ) //Is there more transforms to check
               {    
                  if (iRole == INITIATOR)
                  {
                      reason = BAD_PROPOSAL_SYNTAX;
                      DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (Phase I reply don't match proposal)"));
                      SetErrorStatus( KKmdIkeNegotFailed );
                  }                           
                  else
                  {
                      reason = NO_PROPOSAL_CHOSEN;                    
                      DEBUG_LOG(_L("NO_PROPOSAL_CHOSEN (Received Proposal doesn't match accepted attributes (Check own proposals))"));
                  }           
                  SendNotifyL(reason);
                  DEBUG_LOG(_L("<- CIkev1Negotiation::ProcessTransformsL() KErrNotFound"));
                  return KErrNotFound;
               }    
            }
        }

        payType = tpayload->GetPayload();
        tpayload = tpayload->Next();    //next payload (transform)
        transf = TTransformISAKMP::Ptr(tpayload);
        
    }

    DEBUG_LOG(_L("<- CIkev1Negotiation::ProcessTransformsL() KErrNotFound"));

    //if reaches this point meanse no transform accepted in this proposal
    //Notifies to the upper level
    return KErrNotFound;

}


//Logs the error when comparing a proposal
//void CIkev1Negotiation::LogAttributeError(TInt aTransformNum, TInt aErr)
void CIkev1Negotiation::AppendAttributeError(TInt aErr, TDes &aBuf) const
{
#ifndef _DEBUG
    (void)aErr;
    (void)aBuf;
#endif

#ifdef _DEBUG
    switch (aErr)
    {
    case KErrNotFound:
        aBuf.Append(_L("No proposals\n"));
        break;
    case KErrTransformID:
        aBuf.Append(_L("Different Transform Algorithm\n"));
        break;
    case KErrEncrAlg:
        aBuf.Append(_L("Different Encryption Algorithm\n"));
        break;
    case KErrHashAlg:
        aBuf.Append(_L("Different Hash Algorithm\n"));
        break;
    case KErrAuthMethod:
        aBuf.Append(_L("Different Authentication Method\n"));
        break;
    case KErrGroupDesc:
        aBuf.Append(_L("Different Group Description\n"));
        break;
    case KErrGroupType:
        aBuf.Append(_L("Different Group Type\n"));
        break;
    case KErrGroupPrime:
        aBuf.Append(_L("Different Group Prime\n"));
        break;
    case KErrGroupGen1:
        aBuf.Append(_L("Different Group Generator 1\n"));
        break;
    case KErrGroupGen2:
        aBuf.Append(_L("Different Group Generator 2\n"));
        break;
    case KErrGroupCurveA:
        aBuf.Append(_L("Different Group Curve A\n"));
        break;
    case KErrGroupCurveB:
        aBuf.Append(_L("Different Group Curve A\n"));
        break;
    case KErrPRF:
        aBuf.Append(_L("Different PRF\n"));
        break;
    case KErrKeyLength:
        aBuf.Append(_L("Different Key Length\n"));
        break;
    case KErrFieldSize:
        aBuf.Append(_L("Different Field Size\n"));
        break;
    case KErrGroupOrder:
        aBuf.Append(_L("Different Group Order\n"));
        break;
    case KErrLifeTime:
        aBuf.Append(_L("Different Lifetime\n"));
        break;
    case KErrLifeSize:
        aBuf.Append(_L("Different LifeSize\n"));
        break;
    case KErrEncMode:
        aBuf.Append(_L("Different Encapsulation Mode\n"));
        break;
    case KErrAuthAlg:
        aBuf.Append(_L("Different Authentication Algorithm\n"));
        break;
    case KErrKeyRounds:
        aBuf.Append(_L("Different Key Rounds\n"));
        break;
    case KErrComprDicSize:
        aBuf.Append(_L("Different Compress Dictionary Size\n"));
        break;
    case KErrComprPrivAlg:
        aBuf.Append(_L("Different Compress Private Algorithm\n"));
        break;
    case KErrTransformNum:
        aBuf.Append(_L("Different Transform Num.\n"));
        break;
    case KErrPropProtocol:
        aBuf.Append(_L("Proposals have different protocol.\n"));
        break;
    case KErrNoTransforms:
        aBuf.Append(_L("Proposal has no transforms \n"));
        break;
    case KErrNoRemoteProposals:
        aBuf.Append(_L("Remote Proposals list is empty\n"));
        break;
    case KErrNoLocalProposals:
        aBuf.Append(_L("Local Proposals list is empty\n"));
        break;
    case KErrPropNumberMismatch:
        aBuf.Append(_L("The proposals lists have diferent number of AND'd proposals"));
        break;
    default:
        aBuf.Append(_L("Unknown\n"));
    }
#endif  
}

//processes all the PHASE II transforms. The parameter payload is the proposal containing
//the transforms to be able to access the # of transforms.
//Returns if any transform accepted (KErrNone) or not (KErrNotFound) or processing error (KErrGeneral)
TInt CIkev1Negotiation::ProcessTransforms2L(const TPayloadISAKMP *aPayload,CProposal_II *aProp,TUint32 aLengthLeft)
{
    DEBUG_LOG(_L("-> CIkev1Negotiation::ProcessTransforms2L()"));

    TInt ret;
    const TTransformISAKMP *transf;
    const TProposalISAKMP *proposal=TProposalISAKMP::Ptr(aPayload);
    //First transform. Not Next() because would be the next proposal or non-sa payload
    const TPayloadISAKMP *tpayload = proposal->Payload();

    TAttrib_II *attr_II = NULL; 
    TInt payType = tpayload->GetPayload();  //Type of the payload following the first transform
    transf = TTransformISAKMP::Ptr(tpayload);
    
    TInt numTransf  = (TInt)transf->GetNum();  // First transform number
    TInt lastTransf = numTransf + (TInt)proposal->GetNumTrans(); // Last transform number   

    while ( numTransf < lastTransf ) 
    {
        //only permited payload codes
        if ((payType != ISAKMP_PAYLOAD_NONE) && (payType != ISAKMP_PAYLOAD_T))
        {
            DEBUG_LOG(_L("INVALID_PAYLOAD_TYPE (Bad next payload for the transform)"));
            SendNotifyL(INVALID_PAYLOAD_TYPE);
            return KErrGeneral;
        }
        
        if (tpayload->GetReserved() != 0)   //Must be always 0
        {
            DEBUG_LOG(_L("INVALID RESERVED FIELD"));
            SendNotifyL(PAYLOAD_MALFORMED);
            return KErrGeneral;
        }
        
        //requires special length check
        if ((tpayload->GetLength() < MIN_ISAKMP_PAYLOAD_SIZE) || (tpayload->GetLength() > aLengthLeft))
        {
            DEBUG_LOG(_L("BAD PAYLOAD SIZE"));
            SendNotifyL(PAYLOAD_MALFORMED);
            return KErrGeneral;
        }
        
        if (!CheckTransformID(aProp->iProtocol,transf->GetID()))
            {
            DEBUG_LOG(_L(" Transform doesn't match, moving on to the next one"));
            payType = tpayload->GetPayload();
            tpayload = tpayload->Next();    //next payload (transform)
            transf = TTransformISAKMP::Ptr(tpayload);
            numTransf++;                          // Next supposed transform #
            continue;   //If fails, transform discarded!, not error
            }
        
        if ( transf->GetNum() != numTransf )  //Not the correct #. Must be consecutive
        {   
            DEBUG_LOG1(_L("BAD_PROPOSAL_SYNTAX (Non conscutive transform number (%d)"),transf->GetNum());
            SendNotifyL(BAD_PROPOSAL_SYNTAX);
            return KErrGeneral;
        }

        if (transf->GetReserved() != 0 )    //Must be always 0
        {
            DEBUG_LOG(_L("INVALID RESERVED FIELD"));
            SendNotifyL(PAYLOAD_MALFORMED);
            return KErrGeneral;
        }
        
        //Attributes to be checked depending on Transf ID
        attr_II = new (ELeave) TAttrib_II();
        CleanupStack::PushL(attr_II);
        ret = ProcessAttributes2L(tpayload, attr_II, aProp->iProtocol);
        if (ret != KErrNone)    //Some invalid attribute
        {
            DEBUG_LOG(_L(" Invalid attribute"));
            CleanupStack::PopAndDestroy();  //delete attr_II;
            if (ret != KErrNotFound)
                return KErrGeneral; //Error in the attributes. Already reported
            else    //Invalid transform- Ignored
            {
                DEBUG_LOG2(_L("Transform %d of proposal %d ignored"), transf->GetNum(), aProp->iProposalNum);
            }
        }
        else    //Accepted, must check if really proposed or if matches the configuration if RESPONDER
        {
            DEBUG_LOG(_L(" Adding new attribute"));
            aProp->iAttrList->AppendL(attr_II); //Add the new attribute
            CleanupStack::Pop();    //attr_II saf
        }
        payType = tpayload->GetPayload();
        tpayload = tpayload->Next();    //next payload (transform)
        transf = TTransformISAKMP::Ptr(tpayload);
        numTransf++;                          // Next supposed transform #
        
    }

    //No valid transform found
    if (aProp->iAttrList->Count() == 0)
        {
        DEBUG_LOG(_L("<- CIkev1Negotiation::ProcessTransforms2L() KErrNotFound"));
        return KErrNotFound;
        }

    DEBUG_LOG(_L("<- CIkev1Negotiation::ProcessTransforms2L() KErrNone"));
    return KErrNone;
}


//Copies and checks the values of the attributes. The parameter aPayload must be a transform
//aAttrib will contain the sent attributes if the return value is KErrNone, otherwise should be
//ignore because there was an error reading them (KErrGeneral) or the transform was not accepted (KErrNotFound)
TInt CIkev1Negotiation::ProcessAttributesL(const TPayloadISAKMP *aPayload, TAttrib *aAttrib)
{
    const TTransformISAKMP *transf = TTransformISAKMP::Ptr(aPayload);
    TInt length= aPayload->GetLength() - sizeof(*transf); //To process the attribs
    TDataISAKMP *attr= transf->SAAttrib();
    
    aAttrib->iTransformNum = transf->GetNum();
    aAttrib->iTransformID  = transf->GetID();
    //
	// Store parameters for extended authentication
	//
	aAttrib->iXauthUsed = iHostData->iUseXauth;
	aAttrib->iRole      = iRole;
	
    TUint16 lifeType  = 0; //No type assigned yet
    TUint32 lifeValue = 0; //No value assigned yet
    TUint16 val;
    
    while ( length>0 )
    {
        length = length - attr->Size();
        if (length<0)   //Mismatch between lengths!!!
        {
            DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (Length mismatch in the attibutes)"));
            SendNotifyL(BAD_PROPOSAL_SYNTAX);
            return KErrGeneral;
        }
        switch (attr->Type())
        {
        case OAKLEY_ATTR_TYPE_ENCR_ALG:
            if (!CheckEncrAlg(attr->Value()))
                return KErrNotFound;
            aAttrib->iEncrAlg=attr->Value();
            break;
        case OAKLEY_ATTR_TYPE_HASH_ALG:
            if (!CheckHashAlg(attr->Value()))
                return KErrNotFound;
            aAttrib->iHashAlg = attr->Value();
            break;
        case OAKLEY_ATTR_TYPE_AUTH_METH:
            val = CTransNegotiation::GetAuthMethod(attr->Value(), iHostData->iUseXauth, iRole);
            if (!CheckAuthMethod(val))
                return KErrNotFound;
            aAttrib->iAuthMethod = val;
            break;
        case OAKLEY_ATTR_TYPE_GROUP_DESC:
            if (!CheckGroupDesc(attr->Value()))
                return KErrNotFound;
            aAttrib->iGroupDesc = attr->Value();
            break;
        case OAKLEY_ATTR_TYPE_GROUP_TYPE:
            if (!CheckGroupType(attr->Value()))
                return KErrNotFound;
            aAttrib->iGroupType = attr->Value();
            break;
        case OAKLEY_ATTR_TYPE_GROUP_PRIME:
            if (attr->IsBasic())
            {
                val = attr->Value();
                aAttrib->iGroupPrime.Copy((TUint8*)&val, sizeof(val));
            }
            else
                aAttrib->iGroupPrime.Copy(attr->VarValue(),attr->Length());
            break;
        case OAKLEY_ATTR_TYPE_GROUP_GEN1:
            if (attr->IsBasic())
            {
                val = attr->Value();
                aAttrib->iGroupGen1.Copy((TUint8*)&val, sizeof(val));
            }
            else
                aAttrib->iGroupGen1.Copy(attr->VarValue(),attr->Length());
            break;
        case OAKLEY_ATTR_TYPE_GROUP_GEN2:
            if (attr->IsBasic())
            {
                val = attr->Value();
                aAttrib->iGroupGen2.Copy((TUint8*)&val, sizeof(val));
            }
            else
                aAttrib->iGroupGen2.Copy(attr->VarValue(),attr->Length());
            break;
        case OAKLEY_ATTR_TYPE_GROUP_CRVA:
            if (attr->IsBasic())
            {
                val = attr->Value();
                aAttrib->iGroupCurveA.Copy((TUint8*)&val, sizeof(val));
            }
            else
                aAttrib->iGroupCurveA.Copy(attr->VarValue(),attr->Length());
            break;
        case OAKLEY_ATTR_TYPE_GROUP_CRVB:
            if (attr->IsBasic())
            {
                val = attr->Value();
                aAttrib->iGroupCurveB.Copy((TUint8*)&val, sizeof(val));
            }
            else
                aAttrib->iGroupCurveB.Copy(attr->VarValue(),attr->Length());
            break;
        case OAKLEY_ATTR_TYPE_LIFE_TYPE:
            lifeType = attr->Value();
            if (!CheckLifeType(lifeType))
            {
                DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (Invalid lifetime type)"));
                SendNotifyL(BAD_PROPOSAL_SYNTAX);
                return KErrGeneral;
            }
            break;
        case OAKLEY_ATTR_TYPE_LIFE_DUR:
            if (attr->IsBasic())
            {
                lifeValue = ByteOrder::Swap32(attr->Value());
                if (lifeType == SECONDS)
                    aAttrib->iLifeDurationSecs.Copy((TUint8 *)&lifeValue, sizeof(lifeValue));
                else if (lifeType == KBYTES)
                    aAttrib->iLifeDurationKBytes.Copy((TUint8 *)&lifeValue, sizeof(lifeValue));
                else
                {
                    DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (Invalid lifetime type)"));
                    SendNotifyL(BAD_PROPOSAL_SYNTAX);
                    return KErrGeneral;
                }
            }
            else    //Not basic
            {
                if (lifeType == SECONDS)
                    aAttrib->iLifeDurationSecs.Copy(attr->VarValue(),attr->Length());
                else if (lifeType == KBYTES)
                    aAttrib->iLifeDurationKBytes.Copy(attr->VarValue(),attr->Length());
                else
                {
                    DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (Invalid lifetime type)"));
                    SendNotifyL(BAD_PROPOSAL_SYNTAX);
                    return KErrGeneral;
                }
            }
            break;
        case OAKLEY_ATTR_TYPE_PRF:
            if (!CheckPRF(attr->Value()))
                return KErrNotFound;
            aAttrib->iPRF=attr->Value();
            break;
        case OAKLEY_ATTR_TYPE_KEY_LEN:
            aAttrib->iKeyLength = attr->Value();
            break;
        case OAKLEY_ATTR_TYPE_FIELD_SIZE:
            if (!CheckFieldSize(attr->Value()))
                return KErrNotFound;
            aAttrib->iFieldSize=attr->Value();
            break;
        case OAKLEY_ATTR_TYPE_GROUP_ORDER:
            if (attr->IsBasic())
            {
                val = attr->Value();
                aAttrib->iGroupOrder.Copy((TUint8*)&val, sizeof(val));
            }
            else
                aAttrib->iGroupOrder.Copy(attr->VarValue(),attr->Length());
            break;
        default:
            DEBUG_LOG(_L("ATTRIBUTES_NOT_SUPPORTED (Invalid attribute number)"));
            SendNotifyL(ATTRIBUTES_NOT_SUPPORTED);
            return KErrGeneral;
        }
        attr = attr->Next();
    }
    
    //Done here to ensure both received
    if (aAttrib->iKeyLength !=0)
        if (!CheckKeyLength(aAttrib->iKeyLength, (TUint8)aAttrib->iEncrAlg ,PROTO_ISAKMP))
            return KErrNotFound;

    return KErrNone;
}

//Copies and checks the values of the attributes. The parameter aPayload must be a transform
//aAttrib will contain the sent attributes if the return value is KErrNone, otherwise should be
//ignore because there was an error reading them (KErrGeneral) or the transform was not accepted (KErrNotFound)
TInt CIkev1Negotiation::ProcessAttributes2L(const TPayloadISAKMP *aPayload, TAttrib_II *aAttrib,TUint8 aProtocol)
{
    const TTransformISAKMP *transf = TTransformISAKMP::Ptr(aPayload);
    TInt length= aPayload->GetLength() - sizeof(*transf); //To process the attribs
    
    aAttrib->iTransformNum = transf->GetNum();
    aAttrib->iTransformID = transf->GetID();
    TDataISAKMP *attr= transf->SAAttrib();
    TUint16 lifeType = 0;   //No type assigned yet
    TUint32 lifeValue = 0;
    while (length>0)
    {
        length = length - attr->Size();
        if (length<0)   //Mismatch between lengths!!!
        {
            DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (Length mismatch in the attibutes)"));
            SendNotifyL(BAD_PROPOSAL_SYNTAX);
            return KErrGeneral;
        }
        switch (attr->Type())
        {
        case DOI_ATTR_TYPE_LIFE_TYPE:
            lifeType=attr->Value();
            if (!CheckLifeType(lifeType))
            {
                DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (Invalid lifetime type)"));
                SendNotifyL(BAD_PROPOSAL_SYNTAX);
                return KErrGeneral;
            }
            break;
        case DOI_ATTR_TYPE_LIFE_DUR:
            if (attr->IsBasic())
            {
                lifeValue = ByteOrder::Swap32(attr->Value());
                if (lifeType==SECONDS)
                    aAttrib->iLifeDurationSecs.Copy((TUint8 *)&lifeValue, sizeof(lifeValue));
                else if (lifeType==KBYTES)
                    aAttrib->iLifeDurationKBytes.Copy((TUint8 *)&lifeValue, sizeof(lifeValue));
                else
                {
                    DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (No lifetime type received)"));
                    SendNotifyL(BAD_PROPOSAL_SYNTAX);
                    return KErrGeneral;
                }
            }
            else
            {
                if (lifeType==SECONDS)
                    aAttrib->iLifeDurationSecs.Copy(attr->VarValue(),attr->Length());
                else if (lifeType==KBYTES)
                    aAttrib->iLifeDurationKBytes.Copy(attr->VarValue(),attr->Length());
                else
                {
                    DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (No lifetime type received)"));
                    SendNotifyL(BAD_PROPOSAL_SYNTAX);
                    return KErrGeneral;
                }
            }
            lifeType = 0;   //Cannot received another lifetime without setting the type again
            break;
        case DOI_ATTR_TYPE_GROUP_DESC:
            if (!CheckGroupDesc(attr->Value()))
                return KErrNotFound;
            aAttrib->iGroupDesc=attr->Value();
            break;
        case DOI_ATTR_TYPE_ENC_MODE:    //Encapsulation Mode
            if (!CheckEncMode(attr->Value()))
                return KErrNotFound;
            aAttrib->iEncMode=attr->Value();
            if ( aAttrib->iEncMode == UDP_ENC_TUNNEL || aAttrib->iEncMode == UDP_RFC_ENC_TUNNEL  )
               aAttrib->iEncMode = DOI_TUNNEL;
            if ( aAttrib->iEncMode == UDP_ENC_TRANSPORT || aAttrib->iEncMode == UDP_RFC_ENC_TRANSPORT)
               aAttrib->iEncMode = DOI_TRANSPORT;
            break;
        case DOI_ATTR_TYPE_AUTH_ALG:
            if (!CheckAuthAlg(attr->Value()))
                return KErrNotFound;
            aAttrib->iAuthAlg=attr->Value();
            break;
        case DOI_ATTR_TYPE_KEY_LEN:
            aAttrib->iKeyLength = attr->Value();
            break;  
        case DOI_ATTR_TYPE_KEY_ROUNDS:
            aAttrib->iKeyRounds=attr->Value();
            break;
/*          
        case DOI_ATTR_TYPE_COMP_DIC_SIZE:   //Compress Dictionary size
            aAttrib->iComprDicSize=attr->Value();
            break;  
        case DOI_ATTR_TYPE_COMP_PRIV_ALG:   //Compress Dictionary size
            aAttrib->iComprPrivAlg=attr->Value();
            break;
*/          
        default:
            DEBUG_LOG(_L("ATTRIBUTES_NOT_SUPPORTED (Invalid attribute number)"));
            SendNotifyL(ATTRIBUTES_NOT_SUPPORTED);
            return KErrGeneral;
        }
        attr = attr->Next();
    }

    if (lifeType != 0)  //Type set but not sent
    {
        DEBUG_LOG(_L("Lifetime type set but value not sent!"));
        return KErrNotFound;
    }

    if (aAttrib->iKeyLength !=0)
        if (!CheckKeyLength(aAttrib->iKeyLength,transf->GetID(),aProtocol)) //Check key length correct
            return KErrNotFound;

    return KErrNone;
}


//returns KErrNone if OK, otherwise error already treated.
TBool CIkev1Negotiation::ProcessKeyL(const TKeyISAKMP *aKey)
{
    //const TKeyISAKMP *key = TKeyISAKMP::Ptr(aPayload);
    
    //payload not present
    if (iPhase==PHASE_I)
    {
        if (!aKey)
        {
            DEBUG_LOG(_L("NO KEY Payload"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(PAYLOAD_MALFORMED);
            return EFalse;
        }
        //Doesn't return yet because it needs to copy the received key
    }
    else    //PHASE_II
    {
        if (!aKey)
        {
            if (iPFS)
            {
                DEBUG_LOG(_L("KEY Payload Expected (PFS is enabled)"));
                return EFalse;
            }

            return ETrue;
            
        }
        else    // Key present
        {
            if (!iPFS)
            {
                DEBUG_LOG(_L("KEY Payload NOT Expected (PFS is disabled)"));
                return EFalse;
            }
            //Doesn't return yet because it needs to copy the received key
        }
    }
        
    //stores the public key sent by the other peer. Only if key received and PFS enabled (PHASE II only)
    iPeerPublicKey.Copy(aKey->KeyData(), aKey->GetLength() - sizeof(*aKey));
    return ETrue;
}


TBool CIkev1Negotiation::ProcessNonceL(const TPayloadISAKMP *aPayload)
{
    const TNonceISAKMP *nonce = TNonceISAKMP::Ptr(aPayload);

    //payload not present
    if (!nonce)
    {
        DEBUG_LOG(_L("NO NONCE PAYLOAD"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(PAYLOAD_MALFORMED);
        return EFalse;
    }

    //stores the nonce sent by the other peer
    if (iRole==RESPONDER)
        iNONCE_I.Copy(nonce->NonceData(),nonce->NonceDataLen());
    else
        iNONCE_R.Copy(nonce->NonceData(),nonce->NonceDataLen());

    return ETrue;

}

TBool CIkev1Negotiation::ProcessStage1_II_IDsL(const TIdentISAKMP *aInit_ID_payload,const TIdentISAKMP *aResp_ID_payload, CProposal_IIList *aRecv_proposals)
{
    //IDci
    //First we check the received IDs to be able to build the proposals for phase_II
    TInt32 addr;        //Contains a numeric IPv4 addr to be sent
    TBuf<40> addr_buf;  //Contains a text IPv4/IPv6 addr to be sent

    TIp6Addr ip6addr;   //IPV6 raw address

    //We receive the peer proxy address or gateway client
    TAttrib_II *attr_II = aRecv_proposals->At(0)->iAttrList->At(0);
    if (aInit_ID_payload)   //ID Payload received
    {
        iIDReceived = ETrue;
        if (!CheckIdentL(aInit_ID_payload))  
            return EFalse;

        iIDRemotePort = aInit_ID_payload->GetPort();
        iIDProtocol = aInit_ID_payload->GetProtocol();
        iRemoteIDType_II = aInit_ID_payload->GetIDType();

        switch (aInit_ID_payload->GetIDType())
        {
            case ID_IPV4_ADDR:
                Mem::Copy((TUint8 *)&addr, aInit_ID_payload->IDData(),sizeof(TInt32));
                iRemoteAddr1_ID_II.SetAddress(ByteOrder::Swap32(addr));
                DEBUG_LOG(_L("Remote ID received"));
                DEBUG_LOG(_L("Setting Remote ID to:"));
                iRemoteAddr1_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                if (iRemoteAddr.Match(iRemoteAddr1_ID_II))
                    iDefaultRemoteID = ETrue;   //Must be sent but won't be used when updating the SAD
                else if (attr_II->iEncMode == DOI_TRANSPORT)
                {
                    DEBUG_LOG(_L("ADDRESS_NOTIFICATION (Received ID MUST match the Remote addr in Transport mode)"));
                    SendNotifyL(ADDRESS_NOTIFICATION);
                    return EFalse;
                }
                break;
            case ID_IPV4_ADDR_SUBNET:
                Mem::Copy((TUint8 *)&addr, aInit_ID_payload->IDData(),sizeof(TInt32));  //Address
                iRemoteAddr1_ID_II.SetAddress(ByteOrder::Swap32(addr));
                Mem::Copy((TUint8 *)&addr, aInit_ID_payload->IDData() + sizeof(TInt32),sizeof(TInt32)); //Mask
                iRemoteAddr2_ID_II.SetAddress(ByteOrder::Swap32(addr));
                iRemoteAddr1_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(_L("Setting Remote ID to: addr = "));
                DEBUG_LOG(addr_buf);
                iRemoteAddr2_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(_L(" mask = "));
                DEBUG_LOG(addr_buf);
                if (PrefixLen(iRemoteAddr2_ID_II) < KErrNone)   //Invalid Mask  (can't be > 32 bec. we get only 4 bytes)
                {
                    DEBUG_LOG(_L("INVALID_ID_INFORMATION (Invalid peer proxy mask for type ID_IPV4_ADDR_SUBNET)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                if (attr_II->iEncMode == DOI_TRANSPORT)
                {
                    if (!iRemoteAddr.Match(iRemoteAddr1_ID_II, iRemoteAddr2_ID_II))
                    {
                        DEBUG_LOG(_L("ADDRESS_NOTIFICATION (Remote ID MUST match the net & mask received)"));
                        SetErrorStatus( KKmdIkeNegotFailed );
                        SendNotifyL(ADDRESS_NOTIFICATION);
                        return EFalse;
                    }
                }
                break;
            case ID_IPV6_ADDR:
                Mem::Copy(&ip6addr.u.iAddr8, aInit_ID_payload->IDData(), sizeof(ip6addr.u.iAddr8));
                iRemoteAddr1_ID_II.SetAddress(ip6addr);
                DEBUG_LOG(_L("Remote ID received"));
                DEBUG_LOG(_L("Setting Remote ID to:"));
                iRemoteAddr1_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                if (iRemoteAddr.Match(iRemoteAddr1_ID_II))
                    iDefaultRemoteID = ETrue;   //Must be sent but won't be used when updating the SAD
                else if (attr_II->iEncMode == DOI_TRANSPORT)
                {
                    DEBUG_LOG(_L("ADDRESS_NOTIFICATION (Remote ID doesn't match received IDi in Transport mode)"));
                    SendNotifyL(ADDRESS_NOTIFICATION);
                    return EFalse;
                }
                break;
            case ID_IPV6_ADDR_SUBNET:
                Mem::Copy(&ip6addr.u.iAddr8, aInit_ID_payload->IDData(), sizeof(ip6addr.u.iAddr8)); //Address               
                iRemoteAddr1_ID_II.SetAddress(ip6addr);
                Mem::Copy(&ip6addr.u.iAddr8, aInit_ID_payload->IDData() + sizeof(ip6addr.u.iAddr8), sizeof(ip6addr.u.iAddr8));  //Mask
                iRemoteAddr2_ID_II.SetAddress(ip6addr);
                DEBUG_LOG(_L("Remote ID (subnet) received"));
                DEBUG_LOG(_L("Setting Remote ID to: addr = "));
                iRemoteAddr1_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                DEBUG_LOG(_L(" mask = "));
                iRemoteAddr2_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                if (PrefixLen(iRemoteAddr2_ID_II) < 0)  //Invalid Mask
                {
                    DEBUG_LOG(_L("INVALID_ID_INFORMATION (Invalid peer proxy mask for type ID_IPV6_ADDR_SUBNET)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                if (attr_II->iEncMode == DOI_TRANSPORT)
                {
                    if (!iRemoteAddr.Match(iRemoteAddr1_ID_II, iRemoteAddr2_ID_II))
                    {
                        DEBUG_LOG(_L("ADDRESS_NOTIFICATION (Remote ID MUST match the net & mask received)"));
                        SetErrorStatus( KKmdIkeNegotFailed );
                        SendNotifyL(ADDRESS_NOTIFICATION);
                        return EFalse;
                    }
                }
                break;
            default:    //redundant. Detected in CheckIdentL()
                DEBUG_LOG(_L("INVALID_ID_INFORMATION (ID Type not supported)"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(INVALID_ID_INFORMATION);
                return EFalse;
        }//switch
    }
    else    //No id received (That means we're negotiating directly with the end host) (RFC 2409 5.5)
    {
        //For TRANSPORT we don't need to do anything
        iIDLocalPort = 0;
        iIDRemotePort = 0;
        iIDProtocol = 0;
        return ETrue;   //No need to check the Responder ID if no Initiator ID received
    }

    //IDcr
    //Receive our proxy. We don't know it because we are responders so the other peer tells us who does it
    //want to communicate with
    if (aResp_ID_payload)   //ID Payload received
    {
        if (!CheckIdentL(aResp_ID_payload))
            return EFalse;

        iIDLocalPort = aResp_ID_payload->GetPort();
        iLocalIDType_II = aResp_ID_payload->GetIDType();
        if (iIDProtocol != aResp_ID_payload->GetProtocol()) //Must be the same sent in the IDCi
        {
            DEBUG_LOG(_L("INVALID_ID_INFORMATION (Local ID Protocol different from Remote ID Protocol. Must be the same)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }

        switch (aResp_ID_payload->GetIDType())
        {
            case ID_IPV4_ADDR:
                Mem::Copy((TUint8 *)&addr, aResp_ID_payload->IDData(),sizeof(TInt32));
                iLocalAddr1_ID_II.SetAddress(ByteOrder::Swap32(addr));
                DEBUG_LOG(_L("Local ID received"));
                DEBUG_LOG(_L("Setting Local ID to:"));
                iLocalAddr1_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                if ( iInternalAddr ) {
                   //
                   // Check ID against internal address instead of local address
                   //
                   if (iInternalAddr->iClientIntAddr.Match(iLocalAddr1_ID_II))
                       iDefaultLocalID = ETrue;  //Must be sent but won't be used when updating the SAD
                }
                else {
                   if (iLocalAddr.Match(iLocalAddr1_ID_II))
                        iDefaultLocalID = ETrue; //Must be sent but won't be used when updating the SAD
                   else if (attr_II->iEncMode == DOI_TRANSPORT)
                   {
                       DEBUG_LOG(_L("ADDRESS_NOTIFICATION (Local ID MUST match the net & mask received)"));
                       SetErrorStatus( KKmdIkeNegotFailed );
                       SendNotifyL(ADDRESS_NOTIFICATION);
                       return EFalse;
                   }   
                }
                break;
            case ID_IPV4_ADDR_SUBNET:
                Mem::Copy((TUint8 *)&addr, aResp_ID_payload->IDData(),sizeof(TInt32));  //Address
                iLocalAddr1_ID_II.SetAddress(ByteOrder::Swap32(addr));
                Mem::Copy((TUint8 *)&addr, aResp_ID_payload->IDData() + sizeof(TInt32),sizeof(TInt32)); //Mask
                iLocalAddr2_ID_II.SetAddress(ByteOrder::Swap32(addr));
                DEBUG_LOG(_L("Setting Local ID to: addr = "));
                iLocalAddr1_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                DEBUG_LOG(_L(" mask = "));
                iLocalAddr2_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                if (PrefixLen(iLocalAddr2_ID_II) < 0)   //Invalid Mask
                {
                    DEBUG_LOG(_L("INVALID_ID_INFORMATION (Invalid Remote ID mask for type ID_IPV4_ADDR_SUBNET)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                if (attr_II->iEncMode == DOI_TRANSPORT)
                {
                    if (!iLocalAddr.Match(iLocalAddr1_ID_II, iLocalAddr2_ID_II))
                    {
                        DEBUG_LOG(_L("ADDRESS_NOTIFICATION (Local ID MUST match the net & mask received)"));
                        SetErrorStatus( KKmdIkeNegotFailed );
                        SendNotifyL(ADDRESS_NOTIFICATION);
                        return EFalse;
                    }
                }
                break;
            case ID_IPV6_ADDR:
                Mem::Copy(&ip6addr.u.iAddr8, aResp_ID_payload->IDData(), sizeof(ip6addr.u.iAddr8));
                //iOwnProxyAddr.SetAddress(ip6addr);
                iLocalAddr1_ID_II.SetAddress(ip6addr);
                DEBUG_LOG(_L("Own Proxy received"));
                DEBUG_LOG(_L("Setting Own Proxy address to:"));
                iLocalAddr1_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                if (iLocalAddr.Match(iLocalAddr1_ID_II))
                    iDefaultLocalID = ETrue;    //Must be sent but won't be used when updating the SAD
                else if (attr_II->iEncMode == DOI_TRANSPORT)
                {
                    DEBUG_LOG(_L("ADDRESS_NOTIFICATION (Local ID MUST match the net & mask received)"));
                    SendNotifyL(ADDRESS_NOTIFICATION);
                    return EFalse;
                }
                break;
            case ID_IPV6_ADDR_SUBNET:
                Mem::Copy(&ip6addr.u.iAddr8, aResp_ID_payload->IDData(), sizeof(ip6addr.u.iAddr8)); //Address
                iLocalAddr1_ID_II.SetAddress(ip6addr);
                Mem::Copy(&ip6addr.u.iAddr8, aResp_ID_payload->IDData() + sizeof(ip6addr.u.iAddr8), sizeof(ip6addr.u.iAddr8));  //Mask
                iLocalAddr2_ID_II.SetAddress(ip6addr);
                DEBUG_LOG(_L("Setting Own Proxy to: addr = "));
                iLocalAddr1_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                DEBUG_LOG(_L(" mask = "));
                iLocalAddr2_ID_II.OutputWithScope(addr_buf);
                DEBUG_LOG(addr_buf);
                if (PrefixLen(iLocalAddr2_ID_II) < 0)   //Invalid Mask
                {
                    DEBUG_LOG(_L("INVALID_ID_INFORMATION (Invalid Remote ID mask for type ID_IPV6_ADDR_SUBNET)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                if (attr_II->iEncMode == DOI_TRANSPORT)
                {
                    if (!iLocalAddr.Match(iLocalAddr1_ID_II, iLocalAddr2_ID_II))
                    {
                        DEBUG_LOG(_L("ADDRESS_NOTIFICATION (Local ID MUST match the net & mask received)"));
                        SetErrorStatus( KKmdIkeNegotFailed );
                        SendNotifyL(ADDRESS_NOTIFICATION);
                        return EFalse;
                    }
                }
                break;
            default:
                DEBUG_LOG(_L("INVALID_ID_INFORMATION (ID Type not supported)"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(INVALID_ID_INFORMATION);
                return EFalse;
        }//switch
    }

    return ETrue;
}


TBool CIkev1Negotiation::ProcessStage2_II_IDsL(const TIdentISAKMP *aInit_ID_payload,const TIdentISAKMP *aResp_ID_payload)//, CProposal_IIList *aRecv_proposals)
{
    TInt32 addr4_int;       //Contains a numeric IPv4 addr to be sent
    TIp6Addr ip6addr;   //IPV6 raw address
    TInetAddr tmp_addr;
    //Here we check the initator proxy (Our client) sent by us has been received correctly
    if (aInit_ID_payload)   //ID Payload received
    {
        if (!CheckIdentL(aInit_ID_payload))
            return EFalse;

        if (aInit_ID_payload->GetPort() != iIDLocalPort)
        {
            DEBUG_LOG(_L("INVALID_ID_INFORMATION (Local ID Port different from the one sent)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }

        if (aInit_ID_payload->GetProtocol() != iIDProtocol)
        {
            DEBUG_LOG(_L("INVALID_ID_INFORMATION (Local ID Protocol different from the one sent)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }

        switch (aInit_ID_payload->GetIDType())
        {
        case ID_IPV4_ADDR:
            Mem::Copy((TUint8 *)&addr4_int,aInit_ID_payload->IDData(),sizeof(TInt32));
            tmp_addr.SetAddress(ByteOrder::Swap32(addr4_int));
            if (!tmp_addr.Match(iLocalAddr1_ID_II))
            {
                DEBUG_LOG(_L("Wrong Own ID received (Different from the one sent)"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(INVALID_ID_INFORMATION);
                return EFalse;
            }
            break;
        case ID_IPV4_ADDR_SUBNET:
            //Subnet
            Mem::Copy((TUint8 *)&addr4_int,aInit_ID_payload->IDData(),sizeof(TInt32));
            tmp_addr.SetAddress(ByteOrder::Swap32(addr4_int));
            if (!tmp_addr.Match(iLocalAddr1_ID_II))
            {
                //The ID subnet is not the one we sent!
                DEBUG_LOG(_L("Wrong Own ID subnet received (Different from the one sent)"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(INVALID_ID_INFORMATION);
                return EFalse;
            }
            //Mask
            Mem::Copy((TUint8 *)&addr4_int,aInit_ID_payload->IDData() + sizeof(TInt32),sizeof(TInt32));
            tmp_addr.SetAddress(ByteOrder::Swap32(addr4_int));          
            if (!tmp_addr.Match(iLocalAddr2_ID_II))
            {
                //The ID mask is not the one we sent!
                DEBUG_LOG(_L("Wrong Own ID mask received (Different from the one sent)"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(INVALID_ID_INFORMATION);
                return EFalse;
            }
            break;
        case ID_IPV6_ADDR:
            Mem::Copy(&ip6addr.u.iAddr8, aInit_ID_payload->IDData(), sizeof(ip6addr.u.iAddr8));
            tmp_addr.SetAddress(ip6addr);
            if (!tmp_addr.Match(iLocalAddr1_ID_II))
            {
                //The ID is not the one we sent!
                DEBUG_LOG(_L("Wrong Local ID received (Different from the one sent)"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(INVALID_ID_INFORMATION);
                return EFalse;
            }
            break;
        case ID_IPV6_ADDR_SUBNET:
            //subnet
            Mem::Copy(&ip6addr.u.iAddr8, aInit_ID_payload->IDData(), sizeof(ip6addr.u.iAddr8));
            tmp_addr.SetAddress(ip6addr);
            if (!tmp_addr.Match(iLocalAddr1_ID_II))
            {
                //The ID is not the one we sent!
                DEBUG_LOG(_L("Wrong Local ID subnet received (Different from the one sent)"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(INVALID_ID_INFORMATION);
                return EFalse;
            }
            //mask
            Mem::Copy(&ip6addr.u.iAddr8, aInit_ID_payload->IDData() + sizeof(ip6addr.u.iAddr8), sizeof(ip6addr.u.iAddr8));
            tmp_addr.SetAddress(ip6addr);
            if (!tmp_addr.Match(iLocalAddr2_ID_II))
            {
                //The ID is not the one we sent!
                DEBUG_LOG(_L("Wrong Local ID mask received (Different from the one sent)"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(INVALID_ID_INFORMATION);
                return EFalse;
            }
            break;
        default:
            DEBUG_LOG(_L("INVALID_ID_INFORMATION (ID Type not supported)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }//switch
    }
    else    //No id sent (That means we're negotiating directly with the end host
    {
        if (!iLocalAddr1_ID_II.IsUnspecified())
        {
            DEBUG_LOG(_L("IDci expected and not received!"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }
    }

    //We receive the peer (responder) proxy address or gateway client
    if (aResp_ID_payload)   //ID Payload received
    {
        if (!CheckIdentL(aResp_ID_payload))
            return EFalse;

        if (aResp_ID_payload->GetPort() != iIDRemotePort)
        {
            DEBUG_LOG(_L("INVALID_ID_INFORMATION (Remote Port different from the one sent)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }

        if (aResp_ID_payload->GetProtocol() != iIDProtocol)
        {
            DEBUG_LOG(_L("INVALID_ID_INFORMATION (Responder ID Protocol different from the one sent)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }

        switch (aResp_ID_payload->GetIDType())
        {
            case ID_IPV4_ADDR:
                Mem::Copy((TUint8 *)&addr4_int,aResp_ID_payload->IDData(),sizeof(TInt32));
                tmp_addr.SetAddress(ByteOrder::Swap32(addr4_int));
                DEBUG_LOG(_L("IDcr received"));
                if (!iRemoteAddr1_ID_II.Match(tmp_addr))
                {
                    DEBUG_LOG(_L("INVALID_ID_INFORMATION (Wrong Remote ID, doesn't match sent one)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                break;
            case ID_IPV4_ADDR_SUBNET:
                //subnet address
                Mem::Copy((TUint8 *)&addr4_int,aResp_ID_payload->IDData(),sizeof(TInt32));
                tmp_addr.SetAddress(ByteOrder::Swap32(addr4_int));
                if (!tmp_addr.Match(iRemoteAddr1_ID_II))
                {
                    //The ID subnet is not the one we sent!
                    DEBUG_LOG(_L("Wrong Remote ID subnet received (Different from the one sent)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                //Mask address
                Mem::Copy((TUint8 *)&addr4_int,aResp_ID_payload->IDData() + sizeof(TInt32),sizeof(TInt32));
                tmp_addr.SetAddress(ByteOrder::Swap32(addr4_int));          
                if (!tmp_addr.Match(iRemoteAddr2_ID_II))
                {
                    //The ID mask is not the one we sent!
                    DEBUG_LOG(_L("Wrong Remote ID mask received (Different from the one sent)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                break;
            case ID_IPV6_ADDR:
                Mem::Copy(&ip6addr.u.iAddr8,aResp_ID_payload->IDData(), sizeof(ip6addr.u.iAddr8));
                tmp_addr.SetAddress(ip6addr);
                DEBUG_LOG(_L("IDcr received"));
                if (!iRemoteAddr1_ID_II.Match(tmp_addr))
                {
                    DEBUG_LOG(_L("INVALID_ID_INFORMATION (Wrong ID, doesn't match sent proxy)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                break;
            case ID_IPV6_ADDR_SUBNET:
                //subnet
                Mem::Copy(&ip6addr.u.iAddr8, aResp_ID_payload->IDData(), sizeof(ip6addr.u.iAddr8));
                tmp_addr.SetAddress(ip6addr);
                if (!tmp_addr.Match(iRemoteAddr1_ID_II))
                {
                    //The ID is not the one we sent!
                    DEBUG_LOG(_L("Wrong Remote ID subnet received (Different from the one sent)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                //mask
                Mem::Copy(&ip6addr.u.iAddr8, aResp_ID_payload->IDData() + sizeof(ip6addr.u.iAddr8), sizeof(ip6addr.u.iAddr8));
                tmp_addr.SetAddress(ip6addr);
                if (!tmp_addr.Match(iRemoteAddr2_ID_II))
                {
                    //The ID is not the one we sent!
                    DEBUG_LOG(_L("Wrong Remote ID mask received (Different from the one sent)"));
                    SetErrorStatus( KKmdIkeNegotFailed );
                    SendNotifyL(INVALID_ID_INFORMATION);
                    return EFalse;
                }
                break;
            default:    //Only these 2 modes make sense no reason for subnets or range
                DEBUG_LOG(_L("INVALID_ID_INFORMATION (Remote ID Type not supported)"));
                SetErrorStatus( KKmdIkeNegotFailed );
                SendNotifyL(INVALID_ID_INFORMATION);
                return EFalse;
        }//switch
    }
    else    //No id sent (That means we're negotiating directly with the end host. We check it's TRUE!
    {
        if (!iRemoteAddr1_ID_II.IsUnspecified())
        {
            DEBUG_LOG(_L("IDcr expected and not received!"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }
    }

    return ETrue;
}


TBool CIkev1Negotiation::CheckIdentL(const TPayloadISAKMP *aPayload)
{
    const TIdentISAKMP *ident = TIdentISAKMP::Ptr(aPayload);
    
    //payload not present
    if (!ident)
    {
        DEBUG_LOG(_L("NO ID PAYLOAD"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(PAYLOAD_MALFORMED);
        return EFalse;
    }

    if (iPhase  == PHASE_I)
    {
        TUint8 protocol = ident->GetProtocol();
        if ((protocol != KProtocolInetUdp) && (protocol != 0))
        {
            DEBUG_LOG(_L("INVALID_ID_INFORMATION: Bad Phase I Protocol (Only UDP(17) or 0 accepted)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }
        TUint16 port = ident->GetPort();
        if ((port != 0) && (port != IKE_PORT) && (port != FLOATED_IKE_PORT) )
        {
            DEBUG_LOG(_L("INVALID_ID_INFORMATION: Invalid Bad Phase I Port. (Only 0, 500 or 4500 accepted)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }
        switch (ident->GetIDType())
        {
        case ID_IPV4_ADDR:
        case ID_IPV6_ADDR:
        case ID_FQDN:                       
        case ID_USER_FQDN:
        case ID_DER_ASN1_DN:
            break;
        default:
            DEBUG_LOG(_L("INVALID_ID_INFORMATION: Invalid Type (Only IPV4/IPV6/User FQDN and DER ASN1 DN accepted in PHASE I)"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_ID_INFORMATION);
            return EFalse;
        }
    }

    return ETrue;
}


//Certificate Request Payload processing (all modes).
TBool CIkev1Negotiation::ProcessCertificateReqL(const TCertificateReqISAKMP *aCertReq)
{
    if (iChosenProposal_I.iAttrList->iAuthMethod == IKE_A_CRACK)
    {
        DEBUG_LOG(_L("CR ignored when CRACK auth !)"));
        return ETrue;
    }

    TInt ret = CheckEncodingL(aCertReq->GetEncoding());
    switch (ret)
    {
    case KErrGeneral:
        return EFalse;
    case KErrNotSupported:  //Not supported but not an error, just ignored
        return ETrue;   
    }

	if ( !iPkiService )
	   return EFalse;
	TBool Status = EFalse;
	    //
        // No specific CA asked. Find a certificate using own trusted CA list
        //
    if ( ReadOwnCertL())
        {
         Status = ETrue; 
        }
   
    return Status;

}


//Certificate Request Payload(s) processing (all modes).
TBool CIkev1Negotiation::ProcessCertificateReqArrayL(const CArrayFixFlat<const TCertificateReqISAKMP *> *aCRPayloadArray)
{
	
    TInt count = aCRPayloadArray->Count();
    if ( count == 0 )
	{
       return ETrue;   // No Certificate requests   
    }
    
    if ( ProcessCertificateReqL(aCRPayloadArray->At(0)) )
       {
        DEBUG_LOG(_L("User Certificate required by peer found"));           
        iSendCert = ETrue;  //Requires sending our cert in next interchange where allowed/expected, otherwise not sent
        return ETrue;
       }
    HBufC8* CAName = NULL;
    CIkeCaList* trustedCaList = iPkiService->CaList();
    
    TInt Status=0;
    
    for (TInt i=0; i < trustedCaList->Count(); i++)
        {
        CIkeCaElem* CaElem = (*trustedCaList)[i];                     
                
        CAName = IkeCert::GetCertificateFieldDERL(CaElem->Certificate(), KSubjectName);
        CleanupStack::PushL(CAName);
        
        TRAP_IGNORE(Status=iPkiService->ReadChainL(iHostData, CAName)); 
        
        CleanupStack::PopAndDestroy(CAName);
        CAName=NULL;
        
        if ( Status == KErrNone )
            {
             delete iOwnCert;
             iOwnCert = iPkiService->GetCertificate();
             
             iICA1 = iPkiService->GetTrustedICA1();
                    
             iICA2 = iPkiService->GetTrustedICA2();
             
             iPeerTrustedCA = iPkiService->GetTrustedCA();
             
             iSendCert = ETrue;  //Requires sending our cert in next interchange where allowed/expected, otherwise not sent
             
             DEBUG_LOG(_L("Certificate chain Found!"));
             return ETrue;
             }
        }
    
   
    if ( Status == KVpnErrInvalidCaCertFile)
        {
         SetErrorStatus(KVpnErrInvalidCaCertFile);  
         
         SendNotifyL(CERTIFICATE_UNAVAILABLE);
         
         DEBUG_LOG(_L("Certificate chain read failed!"));
           
         return EFalse;
        }
	
    SetErrorStatus(KKmdIkeNoCertFoundErr);  
    SendNotifyL(CERTIFICATE_UNAVAILABLE);
    DEBUG_LOG(_L("Certificate Chain r!"));
    
    return EFalse;
}

//Certificate Payload(s) processing (all modes).
TBool CIkev1Negotiation::ProcessCertificateArrayL(CArrayFixFlat<const TCertificateISAKMP *>* aCertArray)
{
	TBool Status;	
	if ( iCertRequested )
		 Status = EFalse;
	else Status = ETrue;

	if ( iPkiService && aCertArray->Count() )
	{	        
        const CIkeCaList* trustedCaList = iPkiService->CaList();
    	CX509Certificate* PeerCert = IkePkiUtils::VerifyCertificateL(*aCertArray,
    	                                                             *trustedCaList);
    	if ( PeerCert )
	    {
		   delete iPeerX509Cert;
		   iPeerX509Cert = PeerCert;
	       DEBUG_LOG(_L("Peer Certificate is OK"));
		   Status = ETrue; 		
	    }
		else
		{
		   Status = EFalse;					
	       DEBUG_LOG(_L("Peer Certificate is rejected"));
		}			   
	}			
			
	return Status; 
}

//Checks the signature sent by the peer host
TBool CIkev1Negotiation::ProcessSignatureL(const TSignatureISAKMP *aSigPayload)
{
    TBool ret;
    //payload not present
    if (!aSigPayload || !iPeerX509Cert )
    {
        DEBUG_LOG(_L("NO SIG PAYLOAD"));
		SetErrorStatus(KKmdIkePeerAuthFailed);  				
        SendNotifyL(PAYLOAD_MALFORMED);
        return EFalse;
    }

    //DSS only allows SHA1 as hash
    TUint16 tmp = iChosenProposal_I.iAttrList->iHashAlg;
    if (iChosenProposal_I.iAttrList->iAuthMethod==DSS_SIG)
        iChosenProposal_I.iAttrList->iHashAlg = HASH_SHA1;

    TBuf8<ISAKMP_HASH_SIZE> hash;
    //Verify the peer signature
    if (iRole==RESPONDER)
    {
        ComputeHash1L(hash);   //Computes the value of iHASH_I the signature checking
                            //Nothing else to compute.
    }
    else    //Initiator
    {
        ComputeHashrL(hash); //Computes the value of CRACK digest for signature checking
    }

    ret = VerifySignatureL(iPeerX509Cert, (TUint8 *)hash.Ptr(), hash.Length(), aSigPayload->SigData(),aSigPayload->GetDataLength());

    //restores the value of the Hash alg.
    iChosenProposal_I.iAttrList->iHashAlg = tmp;

    if (!ret)
    {
        DEBUG_LOG(_L("INVALID_SIGNATURE 2"));
		SetErrorStatus(KKmdIkePeerAuthFailed);				
        SendNotifyL(INVALID_SIGNATURE);
        return EFalse;
    }

	DEBUG_LOG(_L("Peer Signature is OK"));           	
    return ETrue;
}


TBool CIkev1Negotiation::ProcessHashL(const THashISAKMP *aHashPayload)
{
	TBool Status = EFalse;
	if (aHashPayload)
	{
        //Compute peer's hash							
		TBuf8<ISAKMP_HASH_SIZE> hash;
		if ( (iStage == 6) || ((iStage == 2) && (iExchange == ISAKMP_EXCHANGE_AGGR)))
			 ComputeHashrL(hash);
		else ComputeHash1L(hash);
		Status = (Mem::Compare((TUint8 *)hash.Ptr(), hash.Length(), aHashPayload->Data(), aHashPayload->DataLen()) == 0 );
		if ( !Status )
		{
			DEBUG_LOG(_L("INVALID_HASH_INFORMATION"));
			SendNotifyL(INVALID_HASH_INFORMATION);
		}
	}	
	
    return Status;
}


TBool CIkev1Negotiation::ProcessHash2L(const ThdrISAKMP &aHdr, const THashISAKMP *aHashPayload, TUint aPadding)
{
	TBool Status = EFalse;
    if ( aHashPayload )
	{
	   TUint8* hashMsg = (TUint8*)aHashPayload->Next();
	   TInt hashMsgLen = aHdr.GetLength() - sizeof(aHdr) - aHashPayload->GetLength() - aPadding;
	   Status = VerifyHash2L(aHashPayload, hashMsg, hashMsgLen);
	   if (!Status)
	   {
		   DEBUG_LOG(_L("INVALID_HASH_INFORMATION"));
		   SendNotifyL(INVALID_HASH_INFORMATION);
       }
	}   
	else   
    {
       DEBUG_LOG(_L("PAYLOAD_MALFORMED"));
       SendNotifyL(PAYLOAD_MALFORMED);
    }

    return Status;
}


//Check a notification Payload inserted in a normal exchange (MAIN , AGGR, QUICK)
TBool CIkev1Negotiation::ProcessNotificationL(const TNotificationISAKMP *aNotifPayload)
{
    if (!aNotifPayload)
        return ETrue;   //optional so noting happens

    if (!CheckDOI(aNotifPayload->GetDOI()))
    {
        DEBUG_LOG(_L("Bad DOI in the NOT payload"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(DOI_NOT_SUPPORTED);  //send the informational exchange
        return EFalse;
    }
    
    switch(aNotifPayload->GetMsgType())
    {
    case DOI_RESPONDER_LIFETIME:
        return ProcessResponderLifetimeL(aNotifPayload);
    case DOI_REPLAY_STATUS:
        return ProcessReplayStatus(aNotifPayload);
    case DOI_INITIAL_CONTACT:
        return ProcessInitialContactL(aNotifPayload);
    default:
        DEBUG_LOG(_L("INVALID MESSAGE TYPE in NOT payload"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(PAYLOAD_MALFORMED);
        return EFalse;
    }

}


//Processes a RESPONDER-LIFETIME NOT payload
TBool CIkev1Negotiation::ProcessResponderLifetimeL(const TNotificationISAKMP *aNotifPayload)
{
    TBuf8<2 * ISAKMP_COOKIE_SIZE> spi, own_neg_spi;
    DEBUG_LOG(_L("Processing RESPONDER-LIFETIME"));

			
    if (!((iPhase == PHASE_II) && (iStage == 2)))
    {
		if ( iPhase == PHASE_I ) {
		   DEBUG_LOG(_L("RESPONDER-LIFETIME payload in phase 1, ignored !!"));
		   return ETrue;		
		}
		else {
  		   DEBUG_LOG(_L("Unexpected RESPONDER-LIFETIME payload (Bad stage)"));
  		   SetErrorStatus( KKmdIkeNegotFailed );
		   SendNotifyL(INVALID_PAYLOAD_TYPE);
    	   return EFalse;
		}	
    }

    TUint8 protocol = aNotifPayload->GetProtocol();
    if ((protocol != PROTO_IPSEC_AH) && (protocol != PROTO_IPSEC_ESP) &&
        (protocol != PROTO_ISAKMP)   && (protocol != 0))
    {
        DEBUG_LOG(_L("Bad protocol in the RESPONDER-LIFETIME payload"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(INVALID_PROTOCOL_ID);
        return EFalse;
    }

    TUint8 spi_size = aNotifPayload->GetSPISize();
    CProposal_II *prop;
    TInt i;
    //If SPI sent
    switch (spi_size)
    {
    case 2 * ISAKMP_COOKIE_SIZE:    //ISAKMP spi
        spi.Copy(aNotifPayload->GetSPI(), aNotifPayload->GetSPISize());
        own_neg_spi.Copy(iCookie_I);
        own_neg_spi.Append(iCookie_R);

        if (spi.Compare(own_neg_spi) != 0)
        {
            DEBUG_LOG(_L("Invalid SPI size in the RESPONDER-LIFETIME payload. Payload ignored"));
            SetErrorStatus( KKmdIkeNegotFailed );
            SendNotifyL(INVALID_SPI);
            return EFalse;
        }
        if (iChosenProp_IIList->Count() > 1)    
        {
            DEBUG_LOG(_L("RESPONDER-LIFETIME ignored. More than one SA (need a IPsec SPI to know which one to use) "));
            return EFalse;
        }
        prop = iChosenProp_IIList->At(0);       //Only one proposal
        break;
    case 0: //Compatibility with Alchemy cc500
        if (iChosenProp_IIList->Count() > 1)    
        {
            DEBUG_LOG(_L("RESPONDER-LIFETIME ignored. More than one SA (need a IPsec SPI to know which one to use) "));
            return EFalse;
        }

        prop = iChosenProp_IIList->At(0);       //Only one proposal
        break;
    case sizeof(TUint32):           //IPSEC SPI
        spi.Copy(aNotifPayload->GetSPI(), aNotifPayload->GetSPISize());
        prop = NULL;    //Only for the debugger, the loop will have at least one proposal
        for (i = 0; i < iChosenProp_IIList->Count(); i++)
        {
            prop = iChosenProp_IIList->At(i);       //Only one proposal
            if (((prop->iSPI.Compare(spi) == 0) && prop->iProtocol == protocol)) //right prop   
                break;
        }

        if (i == iChosenProp_IIList->Count())   //No prop matches
        {
            DEBUG_LOG(_L("RESPONDER-LIFETIME ignored. IPsec SPI doesn't match any chosen proposal"));
            return EFalse;
        }
        break;
    default:
        DEBUG_LOG(_L("Bad SPI size in the RESPONDER-LIFETIME payload"));
        return EFalse;
    }
    TAttrib_II *transform = prop->iAttrList->At(0); //Only one transform
    TInt data_len = aNotifPayload->GetNotifDataSize();
    TUint8 *data_ptr = aNotifPayload->GetNotifData();
    TUint16 lifeType = 0;   //No type assigned yet
    TInt64 lifeValue = 0;   //No type assigned yet
    TInt64 lifeValue32;
    TInt64 curr_lifeValue = 0;
    TInt32 duration;

    TDataISAKMP *attr = (TDataISAKMP*)data_ptr;
    while (data_len > 0)
    {
        data_len = data_len - attr->Size();
        if (data_len < 0)   //Mismatch between lengths!!!
        {
            DEBUG_LOG(_L("RESPONDER-LIFETIME (Length mismatch in the attibutes)"));
            return EFalse;
        }
        switch (attr->Type())
        {
        case DOI_ATTR_TYPE_LIFE_TYPE:           
        case OAKLEY_ATTR_TYPE_LIFE_TYPE:
            lifeType = attr->Value();
            if (!CheckLifeType(lifeType))
            {
                DEBUG_LOG(_L("RESPONDER-LIFETIME (Invalid lifetime type)"));
                return EFalse;
            }
            break;
        case DOI_ATTR_TYPE_LIFE_DUR:                        
        case OAKLEY_ATTR_TYPE_LIFE_DUR:
            if (attr->IsBasic())
            {
                duration = ByteOrder::Swap32(attr->Value());
                lifeValue = MAKE_TINT64(0, duration);
                lifeValue32 = I64LOW(lifeValue);                
                if (lifeType == SECONDS)
                {
                    Desc8ToTInt64(transform->iLifeDurationSecs, curr_lifeValue);        //can't fail
                    if (lifeValue < curr_lifeValue)
                        transform->iLifeDurationSecs.Copy((TUint8 *)&lifeValue32, sizeof(lifeValue32));
                }
                else if (lifeType == KBYTES)
                {
                    Desc8ToTInt64(transform->iLifeDurationKBytes, curr_lifeValue);  //can't fail
                    if (lifeValue < curr_lifeValue)
                        transform->iLifeDurationKBytes.Copy((TUint8 *)&lifeValue32, sizeof(lifeValue32));
                }
                else
                {
                    DEBUG_LOG(_L("RESPONDER-LIFETIME (Invalid lifetime type)"));
                    return EFalse;
                }
            }
            else    //Not basic
            {
                TPtrC8 ptr(attr->VarValue(),attr->Length());

                if (lifeType == SECONDS)
                {
                    if (Desc8ToTInt64(ptr, lifeValue) != KErrNone)
                        {
                        DEBUG_LOG(_L("RESPONDER-LIFETIME Lifetime(Sec) Overflowed Setting to maximum value"));
                        }
                    Desc8ToTInt64(transform->iLifeDurationSecs, curr_lifeValue);        //can't fail
                    if (lifeValue < curr_lifeValue)
                        transform->iLifeDurationSecs.Copy(attr->VarValue(),attr->Length());
                }
                else if (lifeType == KBYTES)
                {
                    if (Desc8ToTInt64(ptr, lifeValue) != KErrNone)
                        {
                        DEBUG_LOG(_L("RESPONDER-LIFETIME Lifetime(KBytes) Overflowed Setting to maximum value"));
                        }
                    Desc8ToTInt64(transform->iLifeDurationKBytes, curr_lifeValue);  //can't fail
                    if (lifeValue < curr_lifeValue)
                        transform->iLifeDurationKBytes.Copy(attr->VarValue(),attr->Length());
                }
                else
                {
                    DEBUG_LOG(_L("RESPONDER-LIFETIME (Invalid lifetime type)"));
                    return EFalse;
                }
            }
            break;
        default:
            DEBUG_LOG1(_L("RESPONDER-LIFETIME (Invalid attribute (%d) received)"), attr->Type());
            return EFalse;
        }//switch
        attr = attr->Next();
    }//while

    return ETrue;
}

//Processes a REPLAY-STATUS NOT payload
TBool CIkev1Negotiation::ProcessReplayStatus(const TNotificationISAKMP *aNotifPayload)
{
    TBuf8<2 * ISAKMP_COOKIE_SIZE> spi, own_neg_spi;

    DEBUG_LOG(_L("Processing REPLAY-STATUS"));

    if (!((iPhase == PHASE_II) && ((iStage == 1) || (iStage == 2))))
    {
        DEBUG_LOG(_L("Unexpected REPLAY-STATUS payload (Bad stage)"));
        return EFalse;
    }

    TUint8 protocol = aNotifPayload->GetProtocol();
    if ((protocol != PROTO_IPSEC_AH) && (protocol != PROTO_IPSEC_ESP) &&
        (protocol != PROTO_ISAKMP)   && (protocol != 0))
    {
        DEBUG_LOG(_L("Bad protocol in the REPLAY-STATUS payload"));
        return EFalse;
    }
    
    TInt i;
    TUint8 spi_size = aNotifPayload->GetSPISize();
    CProposal_II *prop;
    //If SPI sent
    switch (spi_size)
    {
    case 2 * ISAKMP_COOKIE_SIZE:    //ISAKMP spi
        spi.Copy(aNotifPayload->GetSPI(), aNotifPayload->GetSPISize());
        own_neg_spi.Copy(iCookie_I);
        own_neg_spi.Append(iCookie_R);
        if (spi.Compare(own_neg_spi) != 0)
        {
            DEBUG_LOG(_L("Invalid SPI size in the REPLAY-STATUS payload. Payload ignored"));
            return EFalse;
        }
        if (iChosenProp_IIList->Count() > 1)    
        {
            DEBUG_LOG(_L("REPLAY-STATUS ignored. More than one IPsec SA (need an IPsec SPI to know which one to use) "));
            return EFalse;
        }
        break;
    case 0: //Compatibility with Alchemy cc500
        if (iChosenProp_IIList->Count() > 1)
        {
            DEBUG_LOG(_L("RESPONDER-LIFETIME ignored. More than one SA (need a IPsec SPI to know which one to use) "));
            return EFalse;
        }
        prop = iChosenProp_IIList->At(0);       //Only one proposal
        break;
    case sizeof(TUint32):           //IPSEC SPI
        spi.Copy(aNotifPayload->GetSPI(), aNotifPayload->GetSPISize());
        for (i = 0; i < iChosenProp_IIList->Count(); i++)
        {
            prop = iChosenProp_IIList->At(i);       //Only one proposal
            if (((prop->iSPI.Compare(spi) == 0) && prop->iProtocol == protocol)) //right prop
                break;
        }
        if (i == iChosenProp_IIList->Count())   //No prop matches
        {
            DEBUG_LOG(_L("REPLAY-STATUS ignored. IPsec SPI doesn't match any chosen proposal"));
            return EFalse;
        }
        break;
    default:
        DEBUG_LOG(_L("Bad SPI size in the REPLAY-STATUS payload"));
        return EFalse;
    }

    TInt data_len = aNotifPayload->GetNotifDataSize();
    TUint32 *data = (TUint32 *)aNotifPayload->GetNotifData();
    if (STATIC_CAST(TUint, data_len) < sizeof(*data))
    {
        DEBUG_LOG(_L("REPLAY-STATUS (Length mismatch in the attibutes)"));
        return EFalse;
    }

#ifdef _DEBUG
    if (ByteOrder::Swap32(*data) == 0)
        DEBUG_LOG(_L("Anti-Replay Disabled on Peer Host"));
    else
        DEBUG_LOG(_L("Anti-Replay Enabled on Peer Host"));
#endif        
    return ETrue;
}


TBool CIkev1Negotiation::ProcessInitialContactL(const TNotificationISAKMP *aNotifPayload)
{
    TBuf8<2 * ISAKMP_COOKIE_SIZE> spi, neg_spi;

    DEBUG_LOG(_L("Processing INITIAL-CONTACT"));
                                                              // 7 = CRACK 
    if (!(iPhase == PHASE_I && (iStage == 5 || iStage == 6 || iStage == 7)))
    {
        DEBUG_LOG(_L("Unexpected INITIAL-CONTACT payload (Bad stage)"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(INVALID_PAYLOAD_TYPE);
        return EFalse;
    }

    if (aNotifPayload->GetProtocol() != PROTO_ISAKMP &&
        aNotifPayload->GetProtocol() != 0 )
        
    {
        DEBUG_LOG(_L("Bad protocol in the INITIAL_CONTACT payload"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(INVALID_PROTOCOL_ID);
        return EFalse;
    }

    if (aNotifPayload->GetSPISize() != 2 * ISAKMP_COOKIE_SIZE)
    {
        DEBUG_LOG(_L("Bad SPI size in the INITIAL_CONTACT payload"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(INVALID_SPI);
        return EFalse;
    }
    spi.Copy(aNotifPayload->GetSPI(), aNotifPayload->GetSPISize());
    neg_spi.Copy(iCookie_I);
    neg_spi.Append(iCookie_R);

    if (spi.Compare(neg_spi) != 0)
    {
        DEBUG_LOG(_L("Invalid SPI size in the INITIAL_CONTACT payload"));
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(INVALID_SPI);
        return EFalse;
    }
   
    if ( iRole == RESPONDER )
        {        
        // Expired SAs are not returned.
        TIkev1SAData* sa = iPluginSession->FindIkev1SADataWithAddr( iRemoteAddr );	
        while ( sa != NULL )
            {
            iPluginSession->UpdateIkev1SAL( sa->iSAId, ETrue );
            sa = iPluginSession->FindIkev1SADataWithAddr( iRemoteAddr );
            }
        
        // Delete other ongoing negotiations.
        CIkev1Negotiation* next = iPluginSession->FirstNegotiation();    
        while ( next != NULL )
            {
            CIkev1Negotiation* current = next;
            next = current->iNext;
            if ( current != this )
                {
                delete current;
                }
            }
        }

    return ETrue;
}



void CIkev1Negotiation::ProcessVendorL(CArrayFixFlat<const TVendorISAKMP*>* aVids)
{
	TBool result;	
    TInt i = 0;

	while ( i < aVids->Count() )
	{
    	TVendorISAKMP* VendorPayload = (TVendorISAKMP*)aVids->At(i);
        DEBUG_LOG(_L("Vendor ID received!\nHex: "));
        DEBUG_LOG_ARRAY(VendorPayload->VIDData(), VendorPayload->GetLength() - sizeof(*VendorPayload));
				
        if ( iLocalAddr.Family() == KAFUnspec )
            User::LeaveIfError( iPluginSession->GetLocalAddress( iLocalAddr ) ); // No local address info, get it !

		result = EFalse;
        iNAT_T_Required = ProcessVendorId(&result,
                                          (TUint8*)iCookie_I.Ptr(),
                                          (TUint8*)iCookie_R.Ptr(),
                                          iLocalAddr,
                                          VendorPayload);
        if ( result )
		{
           iFamiliarPeer = result;
#ifdef _DEBUG           
           DEBUG_LOG(_L("Nokia VPN gateway in peer!"));
		   if ( iNAT_T_Required  ) { 
	  	      DEBUG_LOG(_L("NAT Traversal needed!"));
		      if ( !iHostData->iUseNatProbing )
		      DEBUG_LOG(_L(" NAT probe not requested! NAT-T not used!"));    
		   }
#endif // _DEBUG	   
		   iNAT_T_Required = iNAT_T_Required & iHostData->iUseNatProbing;				   
        }   
        else
		{
		   if ( CheckDPDVendorId(VendorPayload) )
		   {
			  DEBUG_LOG(_L("Peer supports IETF Dead Peer Detection!"));			   
			  iDPDSupported = ETrue;
		   }	  
		   else if ( iNatDiscovery )
		   {
              result = iNatDiscovery->CheckNatVendorId(VendorPayload);
              if ( result )
              {
              	DEBUG_LOG(_L("Peer supports IETF (draft-03) NAT Traversal!"));              	
              }
              else
              {
              	result = iNatDiscovery->CheckRfcNatVendorId(VendorPayload);
              	if ( result )
              	    {
              		iVendorIDRfc=ETrue;
              	    DEBUG_LOG(_L("Peer supports IETF NAT Traversal!"));
              	    }
              }	
		   }		  
		}

		i ++;		
	}

}

//
//Process Internal address payload received
//
void CIkev1Negotiation::ProcessIntAddrL(const TINTNETISAKMP *aIntnetPayload)
{
    if ( aIntnetPayload && iFamiliarPeer && iHostData->iUseInternalAddr ) {
       delete iInternalAddr; //delete if already exists (old)
	   iInternalAddr = NULL;
       iInternalAddr = ProcessIntNetL((TINTNETISAKMP*) aIntnetPayload);
#ifdef _DEBUG       
       if ( iInternalAddr) {  
          TBuf<80> buf;
          TBuf<40> txt_addr;
          iInternalAddr->iClientIntAddr.OutputWithScope(txt_addr);
          DEBUG_LOG1(_L("Internal address received: %S"),&txt_addr);
       }
#endif       
    }   
}

//Computes the hash for phase II
void CIkev1Negotiation::ComputeHash2L(TDes8& aHash, TInt aStage, const TUint8 *aHashMsg, TInt aHashMsgLen)
{
	
	HBufC8* prf_data =
    HBufC8::NewLC(((aHashMsgLen + iNONCE_I.Length() + iNONCE_R.Length() + (2*sizeof(TUint32))) | 0x3) + 1);

    if (aStage == 3)
        prf_data->Des().Append(0);

    TUint32 id = ByteOrder::Swap32(iMessageId);
    prf_data->Des().Append((TUint8*)&id,sizeof(iMessageId));
    DEBUG_LOG(_L("ID"));
    switch (aStage)
    {
    case 2:
        prf_data->Des().Append(iNONCE_I);
        //No break is intended
    case 1:
        prf_data->Des().Append(aHashMsg,aHashMsgLen);
        break;
    case 3:
        
        prf_data->Des().Append(iNONCE_I);
        DEBUG_LOG(_L("iNONCE_I"));
        prf_data->Des().Append(iNONCE_R);
        DEBUG_LOG(_L("iNONCE_R"));
        break;
    default:
		CleanupStack::PopAndDestroy();  //prf_data		
        return;
    }

    DEBUG_LOG1(_L("Hash_II(%d) prf"),aStage);

    ComputePRFL(aHash, iSKEYID_a, prf_data->Des());
    
    DEBUG_LOG(_L("HASH"));

	CleanupStack::PopAndDestroy();  //prf_data

}

//Computes the hash for a protected informational exchange
void CIkev1Negotiation::ComputeHashInfL(TDes8& aHash, const TUint8 *aHashMsg, TInt aHashMsgLen)
{
	
	HBufC8* prf_data =
    HBufC8::NewLC(((aHashMsgLen + sizeof(iMessageId)) | 0x3) + 1);

    //prf(SKEYID_a, M_ID | N/D)
    TUint32 id = ByteOrder::Swap32(iMessageId);
    prf_data->Des().Append((TUint8*)&id, sizeof(iMessageId));

    prf_data->Des().Append(aHashMsg, aHashMsgLen);
    
    DEBUG_LOG(_L("Hash_NOT prf"));

    ComputePRFL(aHash, iSKEYID_a, prf_data->Des());

    DEBUG_LOG(_L("HASH"));

	CleanupStack::PopAndDestroy();  //prf_data			

}

//Verifies that aHash is correct
TBool CIkev1Negotiation::VerifyHash2L(const THashISAKMP *aHash,const TUint8 *aHashMsg, TInt aHashMsgLen)
{
	TBuf8<ISAKMP_HASH_SIZE> tmp_hash;	
	
    ComputeHash2L(tmp_hash, iStage, aHashMsg, aHashMsgLen);    //Computes the specified phase II hash

    TBool Status = (Mem::Compare((TUint8*)tmp_hash.Ptr(), tmp_hash.Length(), aHash->Data(), aHash->DataLen()) == 0);

	return Status;
}

//Verifies the hash of a Notification or Delete payload
// Used also to verify the hash of Transaction exchange (Attribute payload) 
TBool CIkev1Negotiation::VerifyInformationalHashL(const THashISAKMP *aHash,const TPayloadISAKMP *aPayload, TUint32 aMessageId)
{
    TBuf8<MAX_PRF_LENGTH> tmp_hash;

    TUint32 tmp_id = ByteOrder::Swap32(aMessageId);
    HBufC8 *prf_buf = HBufC8::NewLC(sizeof(tmp_id) + aPayload->GetLength());
    prf_buf->Des().Copy((TUint8 *)&tmp_id , sizeof(tmp_id));
    prf_buf->Des().Append((TUint8 *)aPayload, aPayload->GetLength());

    ComputePRFL(tmp_hash, iSKEYID_a, prf_buf->Des());
    TPtrC8 hash_ptr(aHash->Data(),aHash->DataLen());
    TBool b = (tmp_hash.Compare(hash_ptr)==0);
    CleanupStack::PopAndDestroy();  //prf_buf
    return (b);
}


//Computes Own Nonce using current time a seed
void CIkev1Negotiation::ComputeNonceL()
{
    DEBUG_LOG(_L("Computed NONCE."));
    if (iRole==INITIATOR)
    {
        iNONCE_I.SetLength(OAKLEY_DEFAULT_NONCE_SIZE);
        TRandom::RandomL(iNONCE_I);
    }
    else
    {       
        iNONCE_R.SetLength(OAKLEY_DEFAULT_NONCE_SIZE);
        TRandom::RandomL(iNONCE_R);
    }
}

//Computes HASH_R value
void CIkev1Negotiation::ComputeHashrL(TDes8 &aHash)
{
    TInt    id_size = 0;
    TUint16 auth_method = iChosenProposal_I.iAttrList->iAuthMethod;

    DEBUG_LOG(_L("Computing HASH_R"));

    if ( auth_method != IKE_A_CRACK ) {         
       if (iRole==INITIATOR)   
       {  //peer id. payload
          id_size = iPeerIdentPayloadSize;
          DEBUG_LOG(_L("PeerID"));
       }
       else    
       {  //Own identification payload
          id_size = iOwnIdentPayloadSize;
          DEBUG_LOG(_L("OwnID"));
       }
    }   
            
    DEBUG_LOG(_L("SKEYID"));

    HBufC8 *prf_data;
    if (iRole==INITIATOR)   //peer id. payload
    {
        prf_data = HBufC8::NewLC(iPeerPublicKey.Length() + iOwnPublicKey_ptr.Length() + iCookie_R.Length()
        + iCookie_I.Length() + iSAPayloadSize + id_size);
        prf_data->Des().Copy(iPeerPublicKey);
        prf_data->Des().Append(iOwnPublicKey_ptr);
        prf_data->Des().Append(iCookie_R);
        prf_data->Des().Append(iCookie_I);
        prf_data->Des().Append(iSAPayload,iSAPayloadSize);  //stored at the begining
        if ( auth_method != IKE_A_CRACK )
           prf_data->Des().Append(iPeerIdentPayload, iPeerIdentPayloadSize);
    }
    else    //RESPONDER
    {
        prf_data = HBufC8::NewLC(iOwnPublicKey_ptr.Length() + iPeerPublicKey.Length() + iCookie_R.Length()
        + iCookie_I.Length() + iSAPayloadSize + id_size);
        prf_data->Des().Copy(iOwnPublicKey_ptr);
        prf_data->Des().Append(iPeerPublicKey);
        prf_data->Des().Append(iCookie_R);
        prf_data->Des().Append(iCookie_I);
        prf_data->Des().Append(iSAPayload,iSAPayloadSize);  //stored at the begining
        if ( auth_method != IKE_A_CRACK )
           prf_data->Des().Append(iOwnIdentPayload, iOwnIdentPayloadSize);
    }

    DEBUG_LOG(_L("PRF"));

    ComputePRFL(aHash, iSKEYID, prf_data->Des());

    CleanupStack::PopAndDestroy();  //prf_data

    DEBUG_LOG(_L("HASH_R"));

}


//Computes the value of iHASH_I
void CIkev1Negotiation::ComputeHash1L(TDes8 &aHash)
{
    TInt    id_size = 0;
    TUint16 auth_method = iChosenProposal_I.iAttrList->iAuthMethod; 

    DEBUG_LOG(_L("Computing HASH_I"));

    if ( auth_method != IKE_A_CRACK ) {         
       if (iRole==INITIATOR)   //Own identification payload
       {
          id_size = iOwnIdentPayloadSize;
          DEBUG_LOG(_L("OwnID"));
       }
       else    //peer id. payload
       {
          id_size = iPeerIdentPayloadSize;
          DEBUG_LOG(_L("PeerID"));
       }
    }   
            
            
    DEBUG_LOG(_L("SKEYID"));

    HBufC8 *prf_data;
    if (iRole==INITIATOR)
    {
        prf_data = HBufC8::NewLC(iOwnPublicKey_ptr.Length() + iPeerPublicKey.Length() + iCookie_I.Length()
        + iCookie_R.Length() + iSAPayloadSize + id_size);
        prf_data->Des().Copy(iOwnPublicKey_ptr);
        prf_data->Des().Append(iPeerPublicKey);
        prf_data->Des().Append(iCookie_I);
        prf_data->Des().Append(iCookie_R);
        prf_data->Des().Append(iSAPayload,iSAPayloadSize);  //stored at the begining
        if ( auth_method != IKE_A_CRACK )
           prf_data->Des().Append(iOwnIdentPayload,iOwnIdentPayloadSize);      
    }
    else    //RESPONDER
    {
        prf_data = HBufC8::NewLC(iPeerPublicKey.Length() + iOwnPublicKey_ptr.Length() + iCookie_I.Length()
                                 + iCookie_R.Length() + iSAPayloadSize + id_size);
        prf_data->Des().Copy(iPeerPublicKey);
        prf_data->Des().Append(iOwnPublicKey_ptr);
        prf_data->Des().Append(iCookie_I);
        prf_data->Des().Append(iCookie_R);
        prf_data->Des().Append(iSAPayload,iSAPayloadSize);  //stored at the begining

        if ( auth_method != IKE_A_CRACK )       
           prf_data->Des().Append(iPeerIdentPayload, iPeerIdentPayloadSize);        
    }   

    DEBUG_LOG(_L("PRF"));

    ComputePRFL(aHash, iSKEYID, prf_data->Des());
    CleanupStack::PopAndDestroy();  //prf_buf
    DEBUG_LOG(_L("HASH_I"));
}


//Checks the encryption alg is valid
TBool CIkev1Negotiation::CheckEncrAlg(TUint16 aValue)
{
    switch (aValue)
    {
    case DES_CBC:
    case DES3_CBC:
    case AES_CBC:
        return ETrue;
    case IDEA_CBC:
    case BLOWFISH_CBC:
    case RC5_R16_B64_CBC:
    case CAST_CBC:
        DEBUG_LOG(_L("Not implemented Encr algorithm"));
        return ETrue; // Unknown attribute value NOT a fatal error !            
    }
    DEBUG_LOG(_L("Bad Encr algorithm"));

    return ETrue; // Unknown attribute value NOT a fatal error !
}



TBool CIkev1Negotiation::CheckHashAlg(TUint16 aValue)
{
    switch (aValue)
    {
    case HASH_MD5:
    case HASH_SHA1:
        return ETrue;
    case HASH_TIGER:
        DEBUG_LOG(_L("Not implemented Hash algorithm"));
        return ETrue; // Unknown attribute value NOT a fatal error !                            
    }
    DEBUG_LOG(_L("Bad Hash algorithm"));
        return ETrue; // Unknown attribute value NOT a fatal error !                        

}


TBool CIkev1Negotiation::CheckAuthMethod(TUint16 aValue)
{
    switch (aValue)
    {   
    case PRE_SHARED:
        if (iHostData->iPresharedKey.iKey.Length()==0)  //No preshared key defined
        {
            DEBUG_LOG(_L("Authentication method error (No Preshared key available"));
            return EFalse;
        }
        return ETrue;
    case RSA_SIG:
    case DSS_SIG:
    case IKE_A_CRACK:       
        return ETrue;
    }
    DEBUG_LOG(_L("Bad Authentication method"));
    return ETrue; // Unknown attribute value NOT a fatal error !

}

TBool CIkev1Negotiation::CheckGroupDesc(TUint16 aValue)
{
    switch (aValue)
    {
    case MODP_768:
    case MODP_1024:
    case MODP_1536:     
    case MODP_2048:
        return ETrue;
    case EC2N_155:
    case EC2N_185:
        break;
    }

    return ETrue; // Unknown attribute value NOT a fatal error !    
}


TBool CIkev1Negotiation::CheckGroupType(TUint16 aValue)
{
    switch(aValue)
    {
    case MODP:
        return ETrue;
    case ECP:
    case EC2N:
        break;      
    }

    return ETrue; // Unknown attribute value NOT a fatal error !        

}

TBool CIkev1Negotiation::CheckGroupPrime(const TUint8* /* aValue */, TUint16 /* length */)
{
    return ETrue;
}

TBool CIkev1Negotiation::CheckGroupGen(const TUint8* /* aValue */, TUint16 /* length */)
{
    return ETrue;
}

TBool CIkev1Negotiation::CheckGroupCurve(const TUint8* /* aValue */, TUint16 /* length */)
{
    return ETrue;
}

//Used for Phase I and II
TBool CIkev1Negotiation::CheckLifeType(TUint16 aValue)
{
    switch(aValue)
    {
    case SECONDS:
    case KBYTES:
        return ETrue;
    }
    return EFalse;
}

TBool CIkev1Negotiation::CheckLifeDuration(const TUint8* /* aValue */, TUint16 /* length */)
{
    return ETrue;
}

TBool CIkev1Negotiation::CheckPRF(TUint16 aValue)
{
    if (aValue!=OAKLEY_PRF_3DES_CBC_MAC)
    {
        DEBUG_LOG(_L("Bad PRF"));    
        return EFalse;
    }
    return ETrue;
}

TBool CIkev1Negotiation::CheckKeyLength(TUint16 /*aValue*/,TUint8 aID,TUint8 aProtocol)
{
    TBool Status = ETrue;
    switch (aProtocol)
    {
    case PROTO_ISAKMP:
        if ( aID != AES_CBC )
		{	
           Status = EFalse; //all other supported algs have fixed size
		   DEBUG_LOG(_L("Key length specified with fixed ISAKMP encryption algorithm"));
		}
        break;
    case PROTO_IPSEC_AH:
        Status = EFalse;      //Supported algorithms have fixed key length
		DEBUG_LOG(_L("Key length specified with fixed AH integrity algorithm"));
        break;
    case PROTO_IPSEC_ESP:
		if ( aID != ESP_AES_CBC )
		{	
		   Status = EFalse;
		   DEBUG_LOG(_L("Key length specified with fixed ESP encryption algorithm"));		   
		}   
        break;
    default:    //Unsupported SA type
		Status = EFalse;      //Supported algorithms have fixed key length
		break;
    }

	return Status;
}

TBool CIkev1Negotiation::CheckFieldSize(TUint16 /* aValue */)
{
    DEBUG_LOG(_L("Field size not supported"));
    return EFalse;
}

TBool CIkev1Negotiation::CheckGroupOrder(const TUint8* /* aValue */, TUint16 /* length */)
{
    DEBUG_LOG(_L("Group Order not supported "));
    return ETrue; // Unknown attribute value NOT a fatal error !                    
}

//Encapsulation mode
TBool CIkev1Negotiation::CheckEncMode(TUint16 aValue)
{
    switch (aValue)
    {
    case DOI_TUNNEL:
    case DOI_TRANSPORT:
        return ETrue;
    case UDP_ENC_TUNNEL:
    case UDP_RFC_ENC_TUNNEL:
//  case UDP_ENC_TRANSPORT:  
        if ( iNAT_D_Flags )
           return ETrue;
        break;
    }

    DEBUG_LOG(_L("Bad Encapsulation mode"));
    return EFalse;
    
}

//defined authentication algorithm types. Other are invalid.
TBool CIkev1Negotiation::CheckAuthAlg(TUint16 aValue)
{
    switch (aValue)
    {
    case DOI_HMAC_MD5:
    case DOI_HMAC_SHA:
        return ETrue;
	case DOI_DES_MAC:		
    case DOI_KPDK:
        DEBUG_LOG(_L("Unimplemented Auhentication Algorithm"));
    }
    DEBUG_LOG(_L("Bad Auhentication Algorithm"));
    return ETrue; // Unknown attribute value NOT a fatal error !                        
}

//By now only X509_CERT_SIG. Tristate error codes:
//KErrNone -> accepted
//KErrNotSupported ignored but no error Notification.
//KErrGeneral: NOT accepted 
TInt CIkev1Negotiation::CheckEncodingL(TUint8 aEncoding)
{
    switch (aEncoding)
    {
    case X509_CERT_SIG://X.509 Certificate - Signature
        break;
    case CRL://Certificate Revocation List (CRL)
        DEBUG_LOG(_L("WARNING: CRL ignored because not supported"));
        return KErrNotSupported;    //No notification, just ignored!
    case PKCS://PKCS #7 wrapped X.509 certificate
    case PGP://PGP Certificate
    case DNS ://DNS Signed Key
    case X509_CERT_KE://X.509 Certificate - Key Exchange
    case KERBEROS://Kerberos Tokens
    case ARL://Authority Revocation List (ARL)
    case SPKI://SPKI Certificate
    case X509_CERT_ATTR://X.509 Certificate - Attribute
        DEBUG_LOG(_L("CERT_TYPE_UNSUPPORTED (not supported CERT type)"));
        SendNotifyL(CERT_TYPE_UNSUPPORTED);
        return KErrNotSupported;    // No notification, just ignored!        

    default://Invalid encoding type
        DEBUG_LOG(_L("INVALID_CERT_ENCODING (not existent CERT type)"));
        SendNotifyL(INVALID_CERT_ENCODING);
        return KErrNotSupported;    // No notification, just ignored!                

    }

    return KErrNone;
}


//Provisional (Uses as a seed time, Address and port)
TCookie CIkev1Negotiation::CreateCookieL() const
{
    TCookie c;
    //Cookie generation is Random no longer uses known data like addr
    //or port (wrong?)
    c.SetLength(ISAKMP_COOKIE_SIZE);
	TRandom::RandomL(c);
    return c;
}

TInt32 CIkev1Negotiation::RandomMessageId()
{
    TTime tmp_time;
    tmp_time.UniversalTime();
    TInt64 seed = tmp_time.Int64();
    TInt32 rand = Math::Rand(seed);
    return rand;
}

TBool CIkev1Negotiation::CheckCookies(const TCookie& aInit, const TCookie& aResp)
    {
    TCookie NULL_COOKIE;
    NULL_COOKIE.FillZ(ISAKMP_COOKIE_SIZE);

    if ( iCookie_I.Compare(NULL_COOKIE) != 0 &&
         iCookie_I.Compare(aInit) != 0 )
        {
        DEBUG_LOG(_L("Initiator COOKIE incorrect"));
        return EFalse;
        }
    if ( iCookie_R.Compare(NULL_COOKIE) != 0 &&
         iCookie_R.Compare(aResp) != 0 )
        {
        DEBUG_LOG(_L("Responder COOKIE incorrect"));
        return EFalse;
        }

    return ETrue;    
    }

//Checks if the payload value is correct
TBool CIkev1Negotiation::CheckPayloadCode(TUint8 aPayload)
{
    switch (aPayload)
    {
    case ISAKMP_PAYLOAD_NONE:
    case ISAKMP_PAYLOAD_SA:
    case ISAKMP_PAYLOAD_P:
    case ISAKMP_PAYLOAD_T:
    case ISAKMP_PAYLOAD_KE:
    case ISAKMP_PAYLOAD_ID:
    case ISAKMP_PAYLOAD_CERT:
    case ISAKMP_PAYLOAD_CR:
    case ISAKMP_PAYLOAD_HASH:
    case ISAKMP_PAYLOAD_SIG:
    case ISAKMP_PAYLOAD_NONCE:
    case ISAKMP_PAYLOAD_NOTIF:
    case ISAKMP_PAYLOAD_D:
    case ISAKMP_PAYLOAD_VID:
    case ISAKMP_PAYLOAD_ATTRIBUTES:                         
    case ISAKMP_PAYLOAD_CHRE:
    case ISAKMP_INT_NETWORK:
    case IETF_NAT_DISCOVERY:
    case IETF_RFC_NAT_DISCOVERY:
    case IETF_NAT_ORIG_ADDR:
    case IETF_RFC_NAT_ORIG_ADDR:
        return ETrue;   //supported payload type
    }
    DEBUG_LOG1(_L("INVALID_PAYLOAD_TYPE (%x)"),aPayload);
    return EFalse;

}


//Checks if the version (major,minor) is supported
TBool CIkev1Negotiation::CheckVersionL(TUint8 aVersion)
{
    if (aVersion >> 4  > MAJOR)
    {
        DEBUG_LOG(_L("INVALID_MAJOR_VERSION"));
        SendNotifyL(INVALID_MAJOR_VERSION);
        return EFalse;
    }

    if (aVersion & (0x0f) > MINOR)
    {
        DEBUG_LOG(_L("INVALID_MINOR_VERSION"));
        SendNotifyL(INVALID_MINOR_VERSION);
        return EFalse;
    }

    return ETrue;   //version correct
}

//Checks if the exchange type is valid and the same as in the negotiation
TBool CIkev1Negotiation::CheckExchangeTypeL(TUint8 aType)
{
    switch (aType)
    {
    case ISAKMP_EXCHANGE_ID:    //Main
    case ISAKMP_EXCHANGE_AGGR:  // Agressive
    case ISAKMP_EXCHANGE_INFO:
    case IKE_QUICK_MODE:
        //invalid Exchange Type Not the same being used and not an error notification
        if (aType != iExchange)
        {
            DEBUG_LOG(_L("INVALID_EXCHANGE_TYPE"));  //send the informational exchange
            SendNotifyL(INVALID_EXCHANGE_TYPE);  //send the informational exchange
            return EFalse;  //invalid Exchange Type
        }
        break;
    case ISAKMP_EXCHANGE_BASE:  // Base
    case ISAKMP_EXCHANGE_NONE:  // Identity Protection (Main mode in IKE)
    case ISAKMP_EXCHANGE_AUTH:  // Authentication Only
    case IKE_NEW_GROUP_MODE:    // New Group Mode
        DEBUG_LOG(_L("INVALID_EXCHANGE_TYPE"));  //send the informational exchange
        SendNotifyL(UNSUPPORTED_EXCHANGE_TYPE);  //send the informational exchange
        return EFalse;
    }

    return ETrue;
}

//Checks the non-relevant bits are 0. Other comprovations are done when needed
TBool CIkev1Negotiation::CheckFlagsL(TUint8 aFlags)
{
    if (aFlags >> 3 != 0)
    {
        DEBUG_LOG(_L("INVALID_FLAGS"));  //send the informational exchange
        SendNotifyL(INVALID_FLAGS);  //send the informational exchange
        return EFalse;
    }

    return ETrue;
}

//Checks the Id has a correct value. 0 in Phase I, the correct one in Phase II
TBool CIkev1Negotiation::CheckMessageIdL(TUint32 aId)
{
    if (aId != iMessageId)    //iMessageId will be 0 during Phase I
    {
        DEBUG_LOG2(_L("INVALID_MESSAGE_ID %u (neg=%u)"),aId, iMessageId);
        SendNotifyL(INVALID_MESSAGE_ID); //send the informational exchange
        return EFalse;
    }
    return ETrue;
}

//Checks the DOI is valid
TBool CIkev1Negotiation::CheckDOI(TUint32 aDOI)
{
    if (aDOI > IPSEC_DOI)    //Not IPSEC nor ISAKMP DOI
        return EFalse;

    return ETrue;
}

//Checks the SIT is valid
TBool CIkev1Negotiation::CheckSituationL(TUint32 aSIT)
{
    //Secrecy and integrity not yet supported
    if ((aSIT & IPSEC_SIT_SECRECY) || (aSIT & IPSEC_SIT_INTEGRITY))
    {
        DEBUG_LOG(_L("SITUATION_NOT_SUPPORTED"));    //send the informational exchange
        SendNotifyL(SITUATION_NOT_SUPPORTED);    //send the informational exchange
        return EFalse;
    }

    return ETrue;
}
//check the generic payload is OK. Correct payload + Reserved==0
TBool CIkev1Negotiation::CheckGenericPayloadL(const TPayloadISAKMP *aPayload)
{
    if (!CheckPayloadCode(aPayload->GetPayload()))
        return EFalse;

    if (aPayload->GetReserved() != 0)   //Must be always 0
    {
        DEBUG_LOG(_L("INVALID RESERVED FIELD"));
        SendNotifyL(PAYLOAD_MALFORMED);
        return EFalse;
    }

    if ((aPayload->GetLength() < MIN_ISAKMP_PAYLOAD_SIZE) || (aPayload->GetLength() > iLengthLeft))
    {
        DEBUG_LOG(_L("BAD PAYLOAD SIZE"));
        SendNotifyL(PAYLOAD_MALFORMED);
        return EFalse;
    }

    iLengthLeft -= aPayload->GetLength();   //Updates the length left in the buffer

    return ETrue;

}

//checks if protocol supported
TBool CIkev1Negotiation::CheckProtocolL(TUint8 aProtocol)
{
    switch (aProtocol)
    {
        //PHASE_I Protocol
        case PROTO_ISAKMP:  //Only when establishing own SA??
            if (iPhase != PHASE_I)
            {
                DEBUG_LOG(_L("INVALID_PROTOCOL_ID (ISAKMP only allowed in Phase I)"));
                SendNotifyL(INVALID_PROTOCOL_ID);
                return EFalse;
            }
            return ETrue;
			
        //PHASE_II Protocols
        case PROTO_IPSEC_AH:
        case PROTO_IPSEC_ESP:
            if (iPhase != PHASE_II)
            {
                DEBUG_LOG1(_L("INVALID_PROTOCOL_ID (Prot (%u) only allowed in Phase II)"),aProtocol);
                SendNotifyL(INVALID_PROTOCOL_ID);
                return EFalse;
            }
            return ETrue;
    }
    DEBUG_LOG1(_L("INVALID_PROTOCOL_ID (Unknown Protocol (%u))"),aProtocol);
    SendNotifyL(INVALID_PROTOCOL_ID);
    return EFalse;
}

TBool CIkev1Negotiation::CheckSPIL(const TProposalISAKMP *aProposal)
{

    TUint size=aProposal->GetSPISize();
    if (iPhase == PHASE_I)
    {
        if (size > MAX_SPI_SIZE)
        {
            DEBUG_LOG(_L("INVALID_SPI (Bad Size)"));
            SendNotifyL(INVALID_SPI);
            return EFalse;
        }
    }
    else    //Phase II
    {
        TUint32 spi = 0;
        if (aProposal->GetSPISize() > sizeof(TUint32))
        {
            DEBUG_LOG(_L("INVALID_SPI (Too big. Max. is 32 bits)"));
            SendNotifyL(INVALID_SPI);
            return EFalse;
        }
        Mem::Copy((TUint8 *)&spi, ((TProposalISAKMP *)aProposal)->SPI(), aProposal->GetSPISize());
        spi = ByteOrder::Swap32(spi);
        if (spi < 256)  //The first 256 are reserved
        {
            DEBUG_LOG(_L("INVALID_SPI (spi's < 256 are RESERVED)"));
            SendNotifyL(INVALID_SPI);
            return EFalse;
        }
    }
    return ETrue;
}

//Checks for transform payloads. MUST NOT abort processing, just discard the payload
TBool CIkev1Negotiation::CheckTransformID(TUint8 aProtocol,TUint8 aID)
{
    switch (aProtocol)
    {
    case PROTO_ISAKMP:
        if (aID != KEY_IKE)
            return EFalse;
        break;
		
    case PROTO_IPSEC_AH:
		if ( (aID != AH_MD5) && (aID != AH_SHA))
		{
			DEBUG_LOG(_L("Unsupported Authentication Algorithm"));					
			return EFalse;
		}
        break;
    case PROTO_IPSEC_ESP:
		switch ( aID )
		{
			case ESP_DES_CBC:
			case ESP_3DES_CBC:
			case ESP_NULL:
			case ESP_AES_CBC:				
				break;
			default:
				DEBUG_LOG(_L("Unsupported Encryption Algorithm"));
				return EFalse;
		}	
        break;
		
    default:
        return EFalse;
    }

    return ETrue;
}


//Diffie-Hellman key exchange
//The info in iNegotiation MUST be correct
TBool CIkev1Negotiation::ComputeDHPublicValueL()
{
    TUint desc;
    if (iPhase == PHASE_I)
    {
        //If aggressive sends the SA and KE at the same time b4 knowing the chosen group.
        //The group in the first proposed transform is chosen. Shouldn't be transforms with 
        //different groups sent. Checked when using the configuration tool?
        if ((iExchange == ISAKMP_EXCHANGE_AGGR) && (iRole == INITIATOR))
            desc = iProposal_I.iAttrList->iGroupDesc;
        else
            desc = iChosenProposal_I.iAttrList->iGroupDesc;
    }
    else
    {
        if (iRole == INITIATOR) //We have to use one of the proposals because we don't have the reply yet
                                //Anyay only one group can be specified for phase II so it's fine
            desc = iProposal_IIList->At(0)->iAttrList->At(0)->iGroupDesc;
        else    //RESPONDER
            desc = iChosenProp_IIList->At(0)->iAttrList->At(0)->iGroupDesc;
    }

    delete iOwnKeys;    //Happens in phase II because we may recalculate the DH value.
	iOwnKeys = NULL;
    delete iOwnPublicKey;   // Happens in phase II because we may recalculate the DH value.
    iOwnPublicKey = NULL;

    iOwnKeys = GeneratePubPrivKeysL(desc);
    if (!iOwnKeys)
    {
        DEBUG_LOG(_L("Error generating DH public and private keys"));
        return EFalse;
    }
    iOwnPublicKey = iOwnKeys->GetPubKey();    //save the public key in a buffer to have easy access
    iOwnPublicKey_ptr.Set(iOwnPublicKey->Des());
    return ETrue;
}


//Initial IV computation
//Pre:Requires correct Public keys alredy stored!!!
TBool CIkev1Negotiation::InitIVL()
{
	HBufC8* prf_data =
    HBufC8::NewLC(((iOwnPublicKey_ptr.Length() + iPeerPublicKey.Length()) | 0x3) + 1);
	
    if (iRole == INITIATOR)
    {
        prf_data->Des().Copy(iOwnPublicKey_ptr);
        prf_data->Des().Append(iPeerPublicKey);

    }
    else    //RESPONDER
    {
        prf_data->Des().Copy(iPeerPublicKey);
        prf_data->Des().Append(iOwnPublicKey_ptr);
    }
    if (iChosenProposal_I.iAttrList->iHashAlg == HASH_MD5)
         MD5HashL(prf_data->Des(), iIV);
    else SHA1HashL(prf_data->Des(), iIV);

    if (iChosenProposal_I.iAttrList->iEncrAlg == AES_CBC )
         iIVSize = 16;
    else iIVSize = 8;
    iIV.SetLength(iIVSize); 
    DEBUG_LOG(_L("Init"));

	CleanupStack::PopAndDestroy();  //prf_data
    
    return ETrue;
}



//subsequent IV computations. Like when send notifications or beginning of Phase II
TBool CIkev1Negotiation::ComputeIVL(TDes8 &aIV, TInt32 aMessageId)
{ 
	HBufC8* prf_data =
    HBufC8::NewLC(((aIV.Length() + sizeof(aMessageId)) | 0x3) + 1);
	
    if ((iChosenProposal_I.iAttrList->iEncrAlg != DES3_CBC) &&
	    (iChosenProposal_I.iAttrList->iEncrAlg != AES_CBC)  &&
	    (iChosenProposal_I.iAttrList->iEncrAlg != DES_CBC))
    {
        return EFalse;
    }
    //former IV
    prf_data->Des().Copy(aIV);
    //Message ID
    TInt32 id = ByteOrder::Swap32(aMessageId);  //Needed to add it

    prf_data->Des().Append((TUint8 *)&id, sizeof(id));
    
    DEBUG_LOG(_L("prf"));
    if (iChosenProposal_I.iAttrList->iHashAlg == HASH_MD5)
		 MD5HashL(prf_data->Des(), aIV);
    else SHA1HashL(prf_data->Des(), aIV);
		
    DEBUG_LOG(_L("Computed IV"));
			
    CleanupStack::PopAndDestroy();  //prf_data			
    return ETrue;
}

//Generates all the keying material SKEYID,SKEYID_d,SKEYID_a,SKEYID_e
TBool CIkev1Negotiation::ComputeKeysL()
{
	TUint desc;			
    //If aggressive sends the SA and KE at the same time b4 knowing the chosen group.
    //The group in the first proposed transform is chosen. Shouldn't be transforms with 
    //different groups sent
    if ((iExchange == ISAKMP_EXCHANGE_AGGR) && (iRole == INITIATOR))
        desc = iProposal_I.iAttrList->iGroupDesc;
    else
        desc = iChosenProposal_I.iAttrList->iGroupDesc;
	//
	//Computes agreed key 
	//
	HBufC8* agreedKey = ComputeAgreedKeyL(desc, iPeerPublicKey, iOwnKeys);  //(gxy)
	if ( !agreedKey ) {
		DEBUG_LOG(_L("DH secret creation failed (ComputeAgreedKeyL)"));
		SetErrorStatus( KKmdIkeNegotFailed );
		SendNotifyL(INVALID_KEY_INFORMATION);				
		return EFalse;
	}
	CleanupStack::PushL(agreedKey);  //agreedKey

	// Use prf and agreed DH-key to generate keying material
	HBufC8* prf_data =
    HBufC8::NewLC(((iNONCE_I.Length() + iNONCE_R.Length() + 2*ISAKMP_COOKIE_SIZE) | 0x3) + 1);
	HBufC8* prf_key  =
    HBufC8::NewLC(((iNONCE_I.Length() + iNONCE_R.Length() ) | 0x3) + 1);

    DEBUG_LOG(_L("Agreed Key."));

    switch(iChosenProposal_I.iAttrList->iAuthMethod)
    {
    case RSA_SIG:
    case DSS_SIG:
    case IKE_A_CRACK:       
        //For signatures:
        //SKEYID = prf(Ni_b|Nr_b, g^xy)
        //key
        prf_key->Des().Copy(iNONCE_I);
        prf_key->Des().Append(iNONCE_R);
        ComputePRFL(iSKEYID, prf_key->Des(), agreedKey->Des());
        break;
    case PRE_SHARED:
        {
        //pre-shared keys:
        //SKEYID=prf(pre_shared key, Ni_b | Nr_b);
        //data
        prf_data->Des().Copy(iNONCE_I.Ptr(),iNONCE_I.Length());
        prf_data->Des().Append(iNONCE_R.Ptr(),iNONCE_R.Length());
        DEBUG_LOG(_L("Pre-shared Key"));

        HBufC8 *preshared_key_buf = GetPskFromPolicyL();
        CleanupStack::PushL(preshared_key_buf);        
        TPtrC8 preshared_key_ptr(preshared_key_buf->Des());
        ComputePRFL(iSKEYID, preshared_key_ptr, prf_data->Des());
        CleanupStack::PopAndDestroy(preshared_key_buf);
        }
        break;
    default://method not implemented
        DEBUG_LOG1(_L("ATTRIBUTES_NOT_SUPPORTED:Auth Method %d not supported "),iChosenProposal_I.iAttrList->iAuthMethod);
        SetErrorStatus( KKmdIkeNegotFailed );
        SendNotifyL(ATTRIBUTES_NOT_SUPPORTED);
        return EFalse;
    }
	
	CleanupStack::PopAndDestroy(2);  //prf_data and prf_key
	prf_data =
	HBufC8::NewLC(((agreedKey->Length() + iSKEYID.Length() + 2*ISAKMP_COOKIE_SIZE + 4) | 0x3) + 1);
	
    prf_data->Des().Copy(agreedKey->Des());
    prf_data->Des().Append(iCookie_I);
    prf_data->Des().Append(iCookie_R);
    prf_data->Des().Append(0);
    ComputePRFL(iSKEYID_d, iSKEYID, prf_data->Des());

    prf_data->Des().Copy(iSKEYID_d);
    prf_data->Des().Append(agreedKey->Des());
    prf_data->Des().Append(iCookie_I);
    prf_data->Des().Append(iCookie_R);
    prf_data->Des().Append(1);
    ComputePRFL(iSKEYID_a, iSKEYID, prf_data->Des());

    prf_data->Des().Copy(iSKEYID_a);
    prf_data->Des().Append(agreedKey->Des());
    prf_data->Des().Append(iCookie_I);
    prf_data->Des().Append(iCookie_R);
    prf_data->Des().Append(2);
    ComputePRFL(iSKEYID_e, iSKEYID, prf_data->Des());

	agreedKey->Des().FillZ();  // Zeroe DH secret g^xy

    //Builds the IV 
    if (!InitIVL())
    {
        DEBUG_LOG(_L("Error Computing IV"));
        return EFalse;
    }
    if (iExchange == ISAKMP_EXCHANGE_AGGR)
    {
        iLastIV.Copy(iIV);      //Saves last IV in Phase 1
        iLastIV.SetLength(iIVSize);   
        DEBUG_LOG(_L("Last IV Saved!"));
    }

    //if key extension required:
    TUint8 *key;
    TInt key_len=0;
    TInt total_key_len = ISAKMPEncrKeyLength((TUint8)iChosenProposal_I.iAttrList->iEncrAlg);
    if (iSKEYID_e.Length() < total_key_len)
    {
        DEBUG_LOG(_L("Extending encrytion key..."));
        key = new (ELeave) TUint8[total_key_len*2];
        CleanupStack::PushL(key);
        key[0] = 0;
        TPtr8 kx0_ptr(key, 1, 1);
        TPtr8 kx1_ptr(key, 0, total_key_len * 2);
        ComputePRFL(kx1_ptr, iSKEYID_e, kx0_ptr); //K1=prf(SKEYID_e,0)
        key_len += kx1_ptr.Length();
        
        while (key_len < total_key_len)
        {
            kx0_ptr.Set(&key[key_len - kx1_ptr.Length()], kx1_ptr.Length(), total_key_len);
            kx1_ptr.Set(&key[key_len], 0, total_key_len);
            ComputePRFL(kx1_ptr, iSKEYID_e, kx0_ptr); //Kx=prf(SKEYID_e,K<x-1>)
            key_len += kx1_ptr.Length();
        }
        iSKEYID_e.Copy(key, total_key_len);
        CleanupStack::PopAndDestroy();  //key
        DEBUG_LOG(_L(" SKEYID_e (EXTENDED)"));
    }
	else
	{
		iSKEYID_e.SetLength(total_key_len);		
		DEBUG_LOG(_L(" SKEYID_e"));
	}
    
    DEBUG_LOG(_L(" Init IV"));

	CleanupStack::PopAndDestroy(2);  //prf_data and agreedKey
	
    return ETrue;
}

//Computes the IPSEC keys needed for each IPSEC SA
//KEYMAT = prf(SKEY_ID, protocol | SPI | Ni_b | Nr_b)
//if PFS:
//KEYMAT = prf(SKEY_ID, g(qm)^xy | protocol | SPI | Ni_b | Nr_b)
void CIkev1Negotiation::ComputeKeys2L(const CProposal_II *aProp, TInt aKeyLen, TSPINode &aInboundSpiNode, TDes8& aOutboundKey_II, TDes8& aInboundKey_II)
{
    DEBUG_LOG(_L("Computing PHASE II keys "));
    
    aOutboundKey_II.SetLength(0);
    aInboundKey_II.SetLength(0);
    DEBUG_LOG(_L("Total Computed Key Length="));
    DEBUG_LOG_NUM(aKeyLen);

	HBufC8* agreedKey = NULL; //g(qm)^xy
	TInt  prf_len = iSKEYID_d.Length() + iNONCE_I.Length() + iNONCE_R.Length() + 8; // 8 for protocol and SPI
	
    if (iPFS)
    {
        agreedKey = ComputeAgreedKeyL(iChosenProp_IIList->At(0)->iAttrList->At(0)->iGroupDesc, iPeerPublicKey, iOwnKeys);
		if ( !agreedKey )
			User::Leave(KErrGeneral);
		CleanupStack::PushL(agreedKey);
		prf_len += agreedKey->Length(); 
        DEBUG_LOG(_L(" Agreed Key"));
    }
#ifdef _DEBUG    
	else DEBUG_LOG(_L("(NO PFS)"));
#endif // _DEBUG	

    DEBUG_LOG(_L("Protocol:"));
    DEBUG_LOG_NUM(aProp->iProtocol);
  
    TUint32 in_spi = aInboundSpiNode.iSPI;  //inbound spi in Network Order
    DEBUG_LOG(_L("InSPI:"));
    DEBUG_LOG_NUM(ByteOrder::Swap32(in_spi));
    TUint32 out_spi;
    Mem::Copy((TUint8 *)&out_spi, aProp->iSPI.Ptr(), aProp->iSPI.Length());
    DEBUG_LOG1(_L("OutSPI: %x"), out_spi);
    //Inbound and outbound calculations only differ in the SPI
    //If the key is not long enough we need to extend it
    TPtr8 key_ptr((TUint8 *)aOutboundKey_II.Ptr() + aOutboundKey_II.Length(), 0, aOutboundKey_II.MaxLength());
	
	HBufC8* prf_data = HBufC8::NewLC((prf_len | 0x3) + 1);
	
    while ((aOutboundKey_II.Length() * 8) < aKeyLen)  //include the key extension algorithm
    {
        prf_data->Des().Copy(key_ptr);
        if (agreedKey)//Only used if PFS
        {
            prf_data->Des().Append(agreedKey->Des());
        }
        prf_data->Des().Append(&aProp->iProtocol, sizeof(aProp->iProtocol));
        prf_data->Des().Append(aProp->iSPI.Ptr(),aProp->iSPI.Length());
        prf_data->Des().Append(iNONCE_I);
        prf_data->Des().Append(iNONCE_R);
        key_ptr.Set((TUint8 *)aOutboundKey_II.Ptr() + aOutboundKey_II.Length(), 0, aOutboundKey_II.MaxLength() - aOutboundKey_II.Length());
        ComputePRFL(key_ptr, iSKEYID_d, prf_data->Des());
        aOutboundKey_II.SetLength(aOutboundKey_II.Length() + key_ptr.Length());
    }

    key_ptr.Set((TUint8 *)aInboundKey_II.Ptr() + aInboundKey_II.Length(), 0, aOutboundKey_II.MaxLength());
    while ((aInboundKey_II.Length() * 8) < aKeyLen)  //include the key extension algorithm
    {
        prf_data->Des().Copy(key_ptr);
        if (agreedKey)//Only used if PFS
        {
            prf_data->Des().Append(agreedKey->Des());
        }
        prf_data->Des().Append(&aProp->iProtocol,sizeof(aProp->iProtocol));
        prf_data->Des().Append((TUint8 *)&in_spi, sizeof(TUint32));
        prf_data->Des().Append(iNONCE_I);
        prf_data->Des().Append(iNONCE_R);
        
        key_ptr.Set((TUint8 *)aInboundKey_II.Ptr() + aInboundKey_II.Length(), 0, aInboundKey_II.MaxLength() - aInboundKey_II.Length());
        ComputePRFL(key_ptr, iSKEYID_d, prf_data->Des());
        aInboundKey_II.SetLength(aInboundKey_II.Length() + key_ptr.Length());
    }

	if ( agreedKey )		
	     CleanupStack::PopAndDestroy(2);  //prf_data and agreedKey
	else CleanupStack::PopAndDestroy();   //prf_data
			
}


/* This function is called by oakley module to do pseudo random computation for
   given data */
//IMPORTANT: If some func added or modified check MAX_PRF_LENGTH is correct in ike.h

void CIkev1Negotiation::ComputePRFL(TDes8 &prf_output, const TDesC8 &prf_key, const TDesC8 &prf_data)
{
// All actions taken are moved into the crypto module IKE_CRYPTO.CPP
    
    if ( iChosenProposal_I.iAttrList->iPRF == OAKLEY_PRF_3DES_CBC_MAC)
    {
       DEBUG_LOG(_L("PRF (3DES_CBC_MAC)"));        
       Hmac3DesCbcL(prf_data, prf_output, prf_key);  
    }
    else
    {
      // Use HMAC version of the negotiated hash function as default PRF
        if ( iChosenProposal_I.iAttrList->iHashAlg == HASH_MD5 ) {
           DEBUG_LOG(_L("PRF (MD5)"));           
           MD5HmacL(prf_data, prf_output, prf_key);
        }   
        else {
           if ( iChosenProposal_I.iAttrList->iHashAlg == HASH_SHA1 ) {
              DEBUG_LOG(_L("PRF (SHA1)"));              
              SHA1HmacL(prf_data, prf_output, prf_key);
           }
        }   
    }

}


//NOTE: Must be called after ProcessCertificate() !!!
TBool CIkev1Negotiation::CertifyRemoteIdentityL(const TIdentISAKMP *aIdPayload)
{
  TBool Status = EFalse;
  TInt id_len  = aIdPayload->IDDataLen();

  if ( id_len && iPkiService && iPeerX509Cert )
  {
    TPtrC8 IdData((TUint8 *)aIdPayload->IDData(), id_len);

    // Leave is trapped and handled here because the event log is easily accessible
    // and in the upper layers it would not be possible to identify the
    // cause of the error based on the generic leave code.
    TRAPD(err, Status = IkePkiUtils::CertifyIdentityL(iPeerX509Cert, IdData, (TInt)aIdPayload->GetIDType()));

    if( KErrNotSupported == err )
    {
      LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
                     R_VPN_MSG_CERT_ERROR,
                     KErrNotSupported,
                     iPluginSession->VpnIapId(),
                     &iRemoteAddr );
    }

#ifdef _DEBUG    
    if (Status)
        DEBUG_LOG(_L("Remote identity has been certified"));
#endif // _DEBUG    
  }
       
  return Status;
}


//Digests aHash with the key in aCert and compares it to the stored value aSig
//Needs to be check after the Signature payload has been processed
TBool CIkev1Negotiation::VerifySignatureL(CX509Certificate *aCert,TUint8 *aHash, TInt aHashLength,TUint8 *aSig, TUint aSigLength)
{

	TBool ret = EFalse;
	if ( iPkiService )
	{
		TPtrC8 Signature(aSig, aSigLength);
		TPtrC8 RefHash(aHash, aHashLength);	   							   			
	    ret = IkePkiUtils::VerifyIkev1SignatureL(Signature, RefHash, *aCert);
	}   
	
    return ret;


}

//NOTE!!! The func sets iFinished to TRUE always so after detecting an error the communication finishes.
//If it doesn't have to iFinished should be set outside the function.
void CIkev1Negotiation::SendNotifyL(TUint16 aError)
{
	SetFinished();  //Ends the negotiation (error condition detected) If CONNECTED also should end
	
#ifdef _DEBUG	
    TBuf<60> buf;
    DEBUG_LOG(_L("SendNotifyL(), Reason: "));
    buf.Append(TextNotifyType(aError));
    DEBUG_LOG(buf);
#endif // _DEBUG    
    TUint8 protocol = PROTO_ISAKMP;
    TUint8 tmp_exchange = iExchange;
    TUint32 tmp_msg_id = iMessageId;
	TIkev1IsakmpStream* msg = SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );			
    //If configured to send notification payloads for errors
    if (iHostData->iNotify)
    {
        iExchange = ISAKMP_EXCHANGE_INFO;
        iMessageId = RandomMessageId();
        msg->IsakmpInit(this);
		
        //HASH Payload only if payload protected with encyption
        if (iFlags & ISAKMP_HDR_EFLAG)
      		msg->IsakmpHashL();		

        if (iPhase == PHASE_I)
            protocol = iChosenProposal_I.iProtocol;
        else
        {
            if (iChosenProp_IIList) //May be the begining of PHASE_II when still not decided
                protocol = iChosenProp_IIList->At(0)->iProtocol;
        }
        msg->IsakmpNotification(aError, protocol);

        if (iFlags & ISAKMP_HDR_EFLAG)
        {
            msg->IsakmpHashContL();
        }

        iExchange  = tmp_exchange;
        iMessageId = tmp_msg_id;

		SendL(*msg);

  		LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
  		               R_VPN_MSG_SENT_ERROR_RESPONSE,
  		               aError,
  		               iPluginSession->VpnIapId(),
  		               &iRemoteAddr );
    }

}


//Hash function key length (in bytes)
TInt CIkev1Negotiation::HashLength()
{
    if (iChosenProposal_I.iAttrList->iPRF == OAKLEY_PRF_3DES_CBC_MAC)
        return 24;
        
    //If no PRF hash alg used instead.
    if (iChosenProposal_I.iAttrList->iPRF==0)
    {
        if (iChosenProposal_I.iAttrList->iHashAlg==HASH_MD5)
            return HMAC_MD5_SIZE/8; //16 bytes
            //return MD5_DIGEST_LENGTH;
        else if (iChosenProposal_I.iAttrList->iHashAlg==HASH_SHA1)
            return HMAC_SHA1_SIZE/8;
            //return SHA_DIGEST_LENGTH;
    }
    
    return 0;
}


//Could be done using directly iEncrAlg from the chosen transform (in bytes)
TUint32 CIkev1Negotiation::ISAKMPEncrKeyLength(TUint8 aAlgId) const
{
    TUint32 KeyLth = 0;
    switch (aAlgId) {
        case DES_CBC:
            KeyLth = 8;
            break;
        case DES3_CBC:
            KeyLth = 24;
            break;
        case AES_CBC:
            if ( iChosenProposal_I.iAttrList->iKeyLength )
                 KeyLth = (TUint32)(iChosenProposal_I.iAttrList->iKeyLength/8);
            else KeyLth = 16;  // default
            break;
        default:
            break;
    }           
        
    return KeyLth;
}

TIkev1IsakmpStream* CIkev1Negotiation::SaveIkeMsgBfr(TIkev1IsakmpStream* aMsg)
{
	delete iSavedIkeMsgBfr;
    iSavedIkeMsgBfr = aMsg;
	return aMsg;
}	

//Returns the key length (in bits) of an algorithm for iAuth (TAttrib_II) field when protocol is AH
TUint32 CIkev1Negotiation::HMAC_KeyLength(TUint8 aId) const
{
    if (aId == SADB_AALG_MD5HMAC)
        return (HMAC_MD5_SIZE);
    else if (aId == SADB_AALG_SHA1HMAC)
        return (HMAC_SHA1_SIZE);

  return (0); // Error 
}

//Exchange Type
TPtrC CIkev1Negotiation::TextNotifyType(TUint16 aNotif)
{
#ifndef _DEBUG
    (void)aNotif;
#endif

#ifdef _DEBUG
    //TBuf<35> err;
    switch(aNotif)
    {
    case INVALID_PAYLOAD_TYPE:
        return _L("INVALID_PAYLOAD_TYPE");
    case DOI_NOT_SUPPORTED:
        return _L("DOI_NOT_SUPPORTED");
    case SITUATION_NOT_SUPPORTED:
        return _L("SITUATION_NOT_SUPPORTED");
    case INVALID_COOKIE:
        return _L("INVALID_COOKIE");
    case INVALID_MAJOR_VERSION:
        return _L("INVALID_MAJOR_VERSION");
    case INVALID_MINOR_VERSION:
        return _L("INVALID_MINOR_VERSION");
    case INVALID_EXCHANGE_TYPE:
        return _L("INVALID_EXCHANGE_TYPE");
    case INVALID_FLAGS:
        return _L("INVALID_FLAGS");
    case INVALID_MESSAGE_ID:
        return _L("INVALID_MESSAGE_ID");
    case INVALID_PROTOCOL_ID:
        return _L("INVALID_PROTOCOL_ID");
    case INVALID_SPI:
        return _L("INVALID_SPI");
    case INVALID_TRANSFORM_ID:
        return _L("INVALID_TRANSFORM_ID");
    case ATTRIBUTES_NOT_SUPPORTED:
        return _L("ATTRIBUTES_NOT_SUPPORTED");
    case NO_PROPOSAL_CHOSEN:
        return _L("NO_PROPOSAL_CHOSEN");
    case BAD_PROPOSAL_SYNTAX:
        return _L("BAD_PROPOSAL_SYNTAX");
    case PAYLOAD_MALFORMED:
        return _L("PAYLOAD_MALFORMED");
    case INVALID_KEY_INFORMATION:
        return _L("INVALID_KEY_INFORMATION");
    case INVALID_ID_INFORMATION:
        return _L("INVALID_ID_INFORMATION");
    case INVALID_CERT_ENCODING:
        return _L("INVALID_CERT_ENCODING");
    case INVALID_CERTIFICATE:
        return _L("INVALID_CERTIFICATE");
    case CERT_TYPE_UNSUPPORTED:
        return _L("CERT_TYPE_UNSUPPORTED");
    case INVALID_CERT_AUTHORITY:
        return _L("INVALID_CERT_AUTHORITY");
    case INVALID_HASH_INFORMATION:
        return _L("INVALID_HASH_INFORMATION");
    case AUTHENTICATION_FAILED:
        return _L("AUTHENTICATION_FAILED");
    case INVALID_SIGNATURE:
        return _L("INVALID_SIGNATURE");
    case ADDRESS_NOTIFICATION:
        return _L("ADDRESS_NOTIFICATION");
    case NOTIFY_SA_LIFETIME:
        return _L("NOTIFY_SA_LIFETIME");
    case CERTIFICATE_UNAVAILABLE:
        return _L("CERTIFICATE_UNAVAILABLE");
    case UNSUPPORTED_EXCHANGE_TYPE:
        return _L("UNSUPPORTED_EXCHANGE_TYPE");
    case UNEQUAL_PAYLOAD_LENGTHS:
        return _L("UNEQUAL_PAYLOAD_LENGTHS");
    case CONNECTED:
        return _L("CONNECTED");
	case DOI_RESPONDER_LIFETIME:
		return _L("RESPONDER_LIFETIME");
	case DOI_REPLAY_STATUS:
		return _L("REPLAY_STATUS");
	case DOI_INITIAL_CONTACT:
		return _L("INITIAL_CONTACT");
    }
    return _L("Unknown ");
#else
    return NULL;
#endif          
}


void CIkev1Negotiation::TextPayload(TDes &aBuf, TUint8 aPayload)
{
#ifndef _DEBUG
    (void)aBuf;
    (void)aPayload;
#endif

#ifdef _DEBUG
    switch(aPayload)
    {
    case ISAKMP_PAYLOAD_NONE:// (Terminator)
        aBuf = _L("ISAKMP_PAYLOAD_NONE");
        break;
    case ISAKMP_PAYLOAD_SA:// Security Association
        aBuf = _L("ISAKMP_PAYLOAD_SA");
        break;
    case ISAKMP_PAYLOAD_P:// Proposal
        aBuf = _L("ISAKMP_PAYLOAD_P");
        break;
    case ISAKMP_PAYLOAD_T:// Transform
        aBuf = _L("ISAKMP_PAYLOAD_T");
        break;
    case ISAKMP_PAYLOAD_KE:// Key Exchange
        aBuf = _L("ISAKMP_PAYLOAD_KE");
        break;
    case ISAKMP_PAYLOAD_ID:// Identification
        aBuf = _L("ISAKMP_PAYLOAD_ID");
        break;
    case ISAKMP_PAYLOAD_CERT:// Certificate
        aBuf = _L("ISAKMP_PAYLOAD_CERT");
        break;
    case ISAKMP_PAYLOAD_CR:// Certificate Request
        aBuf = _L("ISAKMP_PAYLOAD_CR");
        break;
    case ISAKMP_PAYLOAD_HASH:// Hash
        aBuf = _L("ISAKMP_PAYLOAD_HASH");
        break;
    case ISAKMP_PAYLOAD_SIG:// Signature
        aBuf = _L("ISAKMP_PAYLOAD_SIG");
        break;
    case ISAKMP_PAYLOAD_NONCE:// Nonce
        aBuf = _L("ISAKMP_PAYLOAD_NONCE");
        break;
    case ISAKMP_PAYLOAD_NOTIF:// Notification
        aBuf = _L("ISAKMP_PAYLOAD_NOTIF");
        break;
    case ISAKMP_PAYLOAD_D:// Delete
        aBuf = _L("ISAKMP_PAYLOAD_D");
        break;
    case ISAKMP_PAYLOAD_VID:// Vendor ID
        aBuf = _L("ISAKMP_PAYLOAD_VID");
        break;
    case ISAKMP_PAYLOAD_PRIVATE:// Private use (up to 255)
        aBuf = _L("ISAKMP_PAYLOAD_PRIVATE");
        break;
    default:
        aBuf.Format(_L("Unknown (%d) "),aPayload);
    }
#endif                  
}

//Sends the built message through the socket
void CIkev1Negotiation::SendL(TIkev1IsakmpStream &aMsg)
{
    if (aMsg.iError)
    {
        DEBUG_LOG(_L("Error Building message"));
        return;
    }
    TBuf8<IKEV1_MAX_IV_SIZE> tmp_IV;
    
    ThdrISAKMP *hdr = (ThdrISAKMP *)aMsg.iBuf.Ptr();
    hdr->SetLength(aMsg.iBuf.Length());
    DEBUG_LOG(_L("Sending (clear)..."));
    
    TBool FloatedPort = EFalse; 
    if ( iNAT_D_Flags & (REMOTE_END_NAT + LOCAL_END_NAT) )
        FloatedPort = ETrue;
    
#ifdef _DEBUG
    const TPtrC8 ikeMsgPtr( aMsg.iBuf.Ptr(), aMsg.iBuf.Length() );
    TInetAddr localAddr;
    iPluginSession->GetLocalAddress( localAddr );    
    TInt port = ( FloatedPort ? IkeSocket::KIkePort4500 : IkeSocket::KIkePort500 );
    localAddr.SetPort( port );
    TRACE_MSG_IKEV1( ikeMsgPtr, localAddr, iLastRemoteAddr );
#endif // _DEBUG
    
    TPtr8 lastMsg(iLastMsg->Des());   

    if (hdr->GetFlags() & ISAKMP_HDR_EFLAG)
    {
        DEBUG_LOG(_L("Encrypting..."));

        if (hdr->GetExchange()==ISAKMP_EXCHANGE_INFO || hdr->GetExchange()==ISAKMP_EXCHANGE_TRANSACT )
        {
            if ( hdr->GetExchange()==ISAKMP_EXCHANGE_TRANSACT )
            {
               //
               // Get current IV via CTransNegotiation object linked into
               // CIkev1Negotiation  
               //
               if ( !iTransactionNeg ||
                    !iTransactionNeg->GetIV(hdr->GetMessageId(), tmp_IV) )
               {
                   DEBUG_LOG(_L("Send error ! Cannot get Transaction IV !"));
                   return;
               }       
               DEBUG_LOG(_L("Transaction IV"));                     
               EncryptL(aMsg.iBuf, lastMsg, tmp_IV, iSKEYID_e, iChosenProposal_I.iAttrList->iEncrAlg);
               iTransactionNeg->SetIV(hdr->GetMessageId(), tmp_IV);
            }   
            else
            {
               if (iLastIV.Length() != 0)
                    tmp_IV.Copy(iLastIV);
               else    //iLastIV not yet computed so current iIV is used
                    tmp_IV.Copy(iIV);
               ComputeIVL(tmp_IV, hdr->GetMessageId());
               DEBUG_LOG(_L("Notif IV"));
               EncryptL(aMsg.iBuf, lastMsg, tmp_IV, iSKEYID_e, iChosenProposal_I.iAttrList->iEncrAlg);
            }   

        }
        else    //Normal exchange MAIN, AGGR or QUICK
        {
            DEBUG_LOG(_L("IV"));
            EncryptL(aMsg.iBuf, lastMsg, iIV, iSKEYID_e, iChosenProposal_I.iAttrList->iEncrAlg);
            DEBUG_LOG(_L("New IV (dec)"));
            //Saves last iIV in Phase 1
            if (((iStage==6) && (iExchange == ISAKMP_EXCHANGE_ID)) ||
                ((iStage==3) && (iExchange == ISAKMP_EXCHANGE_AGGR)))
            {
                iLastIV.Copy(iIV);  
                DEBUG_LOG(_L("Last IV Saved!"));
            }
        }

        DEBUG_LOG(_L("Sending ..."));
        DEBUG_LOG1(_L("EncrLen = %d"), lastMsg.Length());
    }
    else
        {
        lastMsg.Copy(aMsg.iBuf);
        }

    hdr = (ThdrISAKMP *)lastMsg.Ptr();
    hdr->SetLength(lastMsg.Length());  //Set the total length!!!		

    if (hdr->GetExchange() == ISAKMP_EXCHANGE_INFO)
    {
        SendAndSaveIkeMsgL(lastMsg, iLastRemoteAddr, FloatedPort);   //No timers!
    }
    else    //Normal msg.
    {
        iTimer->Cancel();   //Cancel previous timer because reply received & processed
        DEBUG_LOG(_L("Timer Cancelled!"));
        iRetryNum = 0;
		SendAndSaveIkeMsgL(lastMsg, iLastRemoteAddr, FloatedPort);
        iTimer->IssueRequest(MAX_RETRANS_TIMER * 1000000); // 1000000 = 1 second
    }
    
}


void CIkev1Negotiation::ReSendL()
{
    //Will resend a packet in the interval (MAX_RETRANS_TIMER/2 , MAX_RETRANS_TIMER)
    if ( iRetryNum < MAX_RETRANS_COUNT ) 
    {
        DEBUG_LOG2(_L("---------- Phase %d - Stage %d ----------"),iPhase, iStage - 1);
        DEBUG_LOG1(_L("ReSending(%d)..."), iRetryNum);
		TBool FloatedPort = EFalse;	
		if ( iNAT_D_Flags & (REMOTE_END_NAT + LOCAL_END_NAT) )
			FloatedPort = ETrue;
		TPtr8 lastMsg(iLastMsg->Des());
		iPluginSession->SendIkeMsgL(lastMsg, iLastRemoteAddr, FloatedPort);  
        //next retransmission between MAX_RETRANS_TIMER/2 and MAX_RETRANS_TIMER seconds
        TTime tmp_time;
        TReal secs = 0;
        tmp_time.UniversalTime();
        TInt64 seed = tmp_time.Int64();
        TInt rand = Math::Rand(seed);
        TInt err = Math::Round(secs, rand / (KMaxTInt / MAX_RETRANS_TIMER/2), 0);
        secs = Math::Round(secs, secs + MAX_RETRANS_TIMER/2, 0);
        if ((!secs) || (err != KErrNone))
            secs = MAX_RETRANS_TIMER/2;
        iTimer->IssueRequest((TInt)secs * 1000000);  // 1000000 = 1 second
        iRetryNum++;
    }
    else
    {
		SendDeleteL(PROTO_ISAKMP);
		if ( iPhase == PHASE_I )
		{	
           DEBUG_LOG(_L("Max num retries reached!!!"));
		}		
		else
		{		    
		    DEBUG_LOG(_L("Quick mode failed, Max num retries reached!!!"));
		}
	
		if ( iPhase == PHASE_I &&
		     iPluginSession->FindIkev1SA() == NULL )
		{
            // Set error status in Phase 1, if there are no IKE SAs.
            if ( GetNotifyStatus() )
                 SetErrorStatus(KKmdIkeNoProposalErr);                                           
            else SetErrorStatus(KKmdIkeNoResponseErr);
		}
		
        LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
                       R_VPN_MSG_VPN_GW_NO_RESP,
                       ErrorStatus(),
                       iPluginSession->VpnIapId(),
                       &iRemoteAddr );								
        iPluginSession->DeleteNegotiation( this );
    }
}

void CIkev1Negotiation::SendDeleteL(TUint8 aProtocol, TUint32 aIpsecSPI)
{
	TIkev1IsakmpStream* msg = SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );							
	
    TBuf8<MAX_SPI_SIZE> SPI;

    DEBUG_LOG(_L("Sending Delete payload..."));
    
    //Creates a DUMMY negotiation with the info to send the packet
    TUint8 tmp_exchange = iExchange;
    iExchange=ISAKMP_EXCHANGE_INFO; //Set the exchange type as info

    msg->IsakmpInit(this);

    //HASH Payload only if payload protected with encyption
    if (iFlags & ISAKMP_HDR_EFLAG)
	    msg->IsakmpHashL();	

    if (aProtocol == PROTO_ISAKMP)  //ISAKMP SPI are the cookies
    {
        SPI.Copy(iCookie_I);
        SPI.Append(iCookie_R);
    }
    else    //IPSECSPI
    {
        SPI.Copy((TUint8 *)&aIpsecSPI, sizeof(TUint32));
    }
    msg->IsakmpDelete(SPI, aProtocol);

    if (iFlags & ISAKMP_HDR_EFLAG)
    {
        msg->IsakmpHashContL();
    }
    DEBUG_LOG(_L("Sending Delete Payload..."));
	SendL(*msg);

    iExchange = tmp_exchange;
}

void CIkev1Negotiation::CheckSendResponderLifetime(TIkev1IsakmpStream &aMsg)
{
    TInt count = iChosenProp_IIList->Count();
    CProposal_II *prop;
    TChosenAttrib_II *attr_II;
    TSPINode inboundspi_node;
    TInt j;
    for (j = 0 ; j < count; j++)    //Check all the chosen proposals (Probably one)
    {
        prop = iChosenProp_IIList->At(j);
        inboundspi_node = iInboundSPIList->At(j);
        attr_II = (TChosenAttrib_II *)prop->iAttrList->At(0);   //only 1 transform is chosen no matter how many there are

        if ((attr_II->iReducedLifeSecs.Length() != 0) || (attr_II->iReducedLifeKBytes.Length() != 0))   //Any lifetime to update
            aMsg.IsakmpResponderLifetime(prop->iProtocol,
                                         inboundspi_node.iSPI,
                                         attr_II->iResponderLifetimeSecs,
                                         attr_II->iResponderLifetimeKBytes);

    }
}


/**--------------------------------------------------------------------
 *
 *  The following methods are used to implement the Dead Peer Detection
 *  protocol defined in <draft-ietf-ipsec-dpd-04.txt>
 *  When timeout expires the R-U-THERE notify message is transmitted
 *  if there has not been any activity during the last timeout
 *
 *--------------------------------------------------------------------*/
void CIkev1Negotiation::DpdNotifyMessageReceivedL(TIkev1SAData* aSa, TUint16 aMsgType, TUint32 aSequence)
{

	if ( aMsgType == DPD_R_U_THERE )
	{
	  //
	  // -- Assure that sequence number in notify data is what expected
	  // -- If ok, transmit a R-U-THERE-ACK
	  //
		DEBUG_LOG(_L("DPD R-U-THERE Notify received"));	  
		if ( (aSa->iExpectedDPDSequence == aSequence) || (aSa->iExpectedDPDSequence == 0) )
		{
			aSa->iExpectedDPDSequence = GetNextSequence(aSequence);
			DEBUG_LOG(_L("Sending DPD R-U-THERE_ACK notify"));
			SendDpdNotifyMessageL(DPD_R_U_THERE_ACK, aSequence);
			iPluginSession->UpdateIkev1SAL(aSa->iSAId, EFalse, aSa);			
		}
#ifdef _DEBUG		
		else DEBUG_LOG(_L("Wrong sequence number in DPD notify message"));
#endif // _DEBUG		
	}
	else if ( aMsgType == DPD_R_U_THERE_ACK )
	{
	  //
	  // -- Assure that sequence number in notify data is corresponds
	  //    current pending sequence
	  //
		DEBUG_LOG(_L("DPD R-U-THERE-ACK Notify received"));
		if ( aSa->iPendingDPDSequence == aSequence )
		{
			aSa->iPendingDPDSequence = 0;
			aSa->iDPDRetry           = 0;
			iPluginSession->UpdateIkev1SAL(aSa->iSAId, EFalse, aSa);
		}
#ifdef _DEBUG		
		else DEBUG_LOG(_L("Wrong sequence number in DPD notify ack message"));
#endif // _DEBUG		
	}	
}

void CIkev1Negotiation::SendKeepAliveMsgL(TIkev1SAData* aSa)
{
	if ( aSa->iDPDRetry > KMaxDpdRetryCount )
	{
		//
		//  DPD Retry count exhausted, current IKE SA in interpreted to
		//  be closed
		//
        LOG_KMD_EVENT( MKmdEventLoggerIf::KLogError,
                       R_VPN_MSG_VPN_GW_NO_RESP,
                       KKmdIkeNoResponseErr,
                       iPluginSession->VpnIapId(),
                       &iRemoteAddr );      	
        
		iPluginSession->DeleteIpsecSAs(aSa->iSAId);		
		iPluginSession->UpdateIkev1SAL(aSa->iSAId, ETrue);
	}
	else
	{
	    //
	    //  Send DPD R-U-THERE notify message 
	    //
		if ( aSa->iPendingDPDSequence == 0 )
		{	
			aSa->iPendingDPDSequence = aSa->iDPDSequence;
			aSa->iDPDSequence = GetNextSequence(aSa->iDPDSequence);			
		}	
		else
		{
		    aSa->iDPDRetry ++;
		}
		DEBUG_LOG(_L("Sending DPD R-U-THERE notify"));		
		SendDpdNotifyMessageL(DPD_R_U_THERE, aSa->iPendingDPDSequence);
	}
	SetFinished();
}

MKmdEventLoggerIf& CIkev1Negotiation::EventLogger()
{
    return iPluginSession->EventLogger();
}

void CIkev1Negotiation::IpsecSaSpiRetrieved(TUint32 aSpiRequestId, 
                                            TInt aStatus, 
                                            TUint32 aSpi) 
{
    DEBUG_LOG3(_L("IPsec SA SPI retrieved, seq=%d, SPI=%d, status=%d"),
            aSpiRequestId, aSpi, aStatus);
    if ( aStatus == KErrNone )
        {
        TRAP( aStatus, ReceiveSPIL( aSpi, aSpiRequestId ) );
        }
    else
        {
        iPluginSession->HandleError( aStatus );
        }
}

TBool CIkev1Negotiation::IsRekeyingIkeSa()
{
    return ( iSARekeyInfo != NULL );
}

void CIkev1Negotiation::PreparePhase2L(const TPfkeyMessage &aReq)
{
    DEBUG_LOG(_L("Prepare for Phase II"));
    GetAcquireDataL(aReq);
    iPhaseIIAfterIkeSaRekey = ETrue;
}

TUint32 CIkev1Negotiation::GetNextSequence(TUint32 aSequence)
{
	aSequence ++;
	if ( aSequence == 0 )
		aSequence = 1;
	return aSequence;
}

void CIkev1Negotiation::SendDpdNotifyMessageL(TUint16 aMsgType, TUint32 aSequence)
{
	iExchange  = ISAKMP_EXCHANGE_INFO;
	iMessageId = RandomMessageId();
	TIkev1IsakmpStream* Msg = SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );
	Msg->IsakmpInit(this);
	Msg->IsakmpHashL();
	TUint32 NotifData = ByteOrder::Swap32(aSequence);
	Msg->IsakmpNotification(aMsgType, PROTO_ISAKMP, (TUint8*)&NotifData, sizeof(NotifData));
	Msg->IsakmpHashContL();
	SendL(*Msg);
}

TInt CIkev1Negotiation::ErrorStatus()
{
    TInt ret( KErrNone );
    if ( iPluginSession )
    {
        ret = iPluginSession->ErrorStatus();
    }
    return ret;
}

void CIkev1Negotiation::SetErrorStatus(TInt aStatus)
{
    SetFinished();
    iPluginSession->SetErrorStatus(aStatus);
}

void CIkev1Negotiation::SendAndSaveIkeMsgL( const TDesC8& aIkeMsg,
                                            TInetAddr& aDestAddr,
                                            TBool aUseNatPort )
{
    iPluginSession->SendIkeMsgL( aIkeMsg, aDestAddr, aUseNatPort );
    SaveLastMsgL();
}


TBool CIkev1Negotiation::IsRetransmit(TLastIKEMsg& aRef)
{       
    TBool isRetransmit(EFalse);
    if (iLastIKEMsgInfo.IsUninitialized())
        {
        TIkev1SAData* ikev1SAData = iPluginSession->FindIkev1SAData(iSAId);
        if (ikev1SAData && ikev1SAData->iLastIKEMsgInfo.IsReTransmit(aRef))
            {
            isRetransmit = ETrue;
            }
        }
    else
        {
        isRetransmit = iLastIKEMsgInfo.IsReTransmit(aRef);
        }
    return isRetransmit;
}

void CIkev1Negotiation::SaveRetransmitInfo(TLastIKEMsg& aRef)
{
    aRef.Store(iLastIKEMsgInfo);
    TIkev1SAData* ikev1SAData = iPluginSession->FindIkev1SAData(iSAId);
    if (ikev1SAData != NULL)
        {
        aRef.Store(ikev1SAData->iLastIKEMsgInfo);
        }
}

void CIkev1Negotiation::SaveLastMsgL()
{
    if ( iLastMsg != NULL )
        {
        TIkev1SAData* ikev1SAData = iPluginSession->FindIkev1SAData(iSAId);
        if ( ikev1SAData != NULL )
            {
            delete ikev1SAData->iLastMsg;
            ikev1SAData->iLastMsg = iLastMsg->AllocL(); 
            }                
        }    
}

HBufC8* CIkev1Negotiation::GetPskFromPolicyL()
{
    ASSERT(iHostData);
    //
    // Get Preshared Key from IKE policy and return in to caller in
    // HBufc8.
    //
    HBufC8* psk = NULL;
    if ( iHostData->iPresharedKey.iFormat ==  STRING_KEY )
    {
        psk = HBufC8::NewL(iHostData->iPresharedKey.iKey.Length());
        psk->Des().Copy(iHostData->iPresharedKey.iKey);
    }
    else if ( iHostData->iPresharedKey.iFormat == HEX_KEY ) 
    {
        psk = HBufC8::NewL(iHostData->iPresharedKey.iKey.Length() / 2);        
        
        for(TInt i = 0; i < iHostData->iPresharedKey.iKey.Length(); i += 2)
        {        
            TPtrC hexByte(iHostData->iPresharedKey.iKey.Mid(i, 2));
            TLex lex(hexByte);
            TUint8 value;
            User::LeaveIfError(lex.Val(value, EHex));
            
            psk->Des().Append(&value, 1);
        }
        
    }

    return psk;
}

void CIkev1Negotiation::SetPhase2LifeDurations( const TInt64 aSoftAddTime,
                                                const TInt64 aHardAddTime,
                                                const TInt64 aSoftBytes,
                                                const TInt64 aHardBytes,
                                                TAttrib_II& aAttr_II,
                                                TBool aResponder )    
{
    TInt64 lifetime64 = aSoftAddTime;
    iHardLifetime = aHardAddTime;
    
    if ( lifetime64 == 0 ||
         aResponder )
        {
        lifetime64 = iHardLifetime;
        }
    
    TUint high = 0;
    TUint low = 0;
    if ( lifetime64 != 0 )
        {
        high = ByteOrder::Swap32(I64HIGH(lifetime64));
        if (high > 0)
            {
            aAttr_II.iLifeDurationSecs.Copy((TUint8 *)&high, sizeof(high));
            }
        low = ByteOrder::Swap32(I64LOW(lifetime64));
        aAttr_II.iLifeDurationSecs.Append((TUint8 *)&low, sizeof(low));
        }
    
    if ( aResponder )
        {
        // Set responder lifetime.
        lifetime64 = aSoftAddTime;
        if ( lifetime64 == 0 )
            {
            lifetime64 = iHardLifetime;
            }    
        
        if ( lifetime64 != 0 )
            {
            high = ByteOrder::Swap32(I64HIGH(lifetime64));
            if (high > 0)
                {
                aAttr_II.iResponderLifetimeSecs.Copy((TUint8 *)&high, sizeof(high));            
                }
            low = ByteOrder::Swap32(I64LOW(lifetime64));
            aAttr_II.iResponderLifetimeSecs.Append((TUint8 *)&low, sizeof(low));
            }        
        }

    //Bytes lifetime
    lifetime64 = aSoftBytes;
    
    if ( lifetime64 == 0 ||
         aResponder )
        {
        lifetime64 = aHardBytes;
        }
        
    lifetime64 = (lifetime64/1024); //Bytes to KB
    if ( lifetime64 != 0 )
        {
        high = ByteOrder::Swap32(I64HIGH(lifetime64));      
        if (high > 0)
            aAttr_II.iLifeDurationKBytes.Copy((TUint8 *)&high, sizeof(high));
        low = ByteOrder::Swap32(I64LOW(lifetime64));        
        aAttr_II.iLifeDurationKBytes.Append((TUint8 *)&low, sizeof(low));
        }    
    
    if ( aResponder )
        {
        // Set responder lifetime.
        if ( lifetime64 == 0 )
            {
            lifetime64 = iHardLifetime;
            }    
        lifetime64 = (lifetime64/1024); //Bytes to KB
        
        if ( lifetime64 != 0 )
            {
            high = ByteOrder::Swap32(I64HIGH(lifetime64));
            if (high > 0)
                {
                aAttr_II.iResponderLifetimeKBytes.Copy((TUint8 *)&high, sizeof(high));            
                }
            low = ByteOrder::Swap32(I64LOW(lifetime64));
            aAttr_II.iResponderLifetimeKBytes.Append((TUint8 *)&low, sizeof(low));
            }        
        }    
}

TInt CIkev1Negotiation::ProcessUserResponseL(CAuthDialogInfo *aDialogInfo )
{
    delete iDialog;  /* delete dialog object */
    iDialog = NULL;
    
    iCRACKLAMUserName = aDialogInfo->iUsername->AllocL();
    iCRACKLAMPassword = aDialogInfo->iSecret->AllocL();
    
    delete aDialogInfo;  /* release dialog info object */
    iDialogInfo = NULL;  /* reset dialog info pointer  */
    DEBUG_LOG(_L("Continue negotiation from begining"));
    InitNegotiationL();
    
    return KErrNone;

}
//
// The implementation for class MIkeDialogComplete virtual function
//
TInt CIkev1Negotiation::DialogCompleteL(CIkev1Dialog* /*aDialog*/, TAny* aUserInfo,
                                            HBufC8* aUsername, HBufC8* aSecret, HBufC8* aDomain)
{
/*---------------------------------------------------------------------------
 *  
 *  A response received from client user (through asynchronous dialog)
 *  This method is introduced as a TUserCallback for CGetIKEPassword dialog
 *  object is created. When the dialog is completed this callback function
 *  is called 
 *  
 *-------------------------------------------------------------------------*/
    TUint32 obj_id = 1;
     CAuthDialogInfo* info = (CAuthDialogInfo*)aUserInfo;
     DEBUG_LOG1(_L("CIkev1Negotiation::DialogCompleteL(), aUserInfo =  %x"), aUserInfo);
             
     if ( info )
     {
        obj_id = info->GetObjId();
        DEBUG_LOG1(_L("Preparing to call AuthDialogCompletedL(), ObjId = %x"), obj_id);
        if ( obj_id == DIALOG_INFO_ID )
        {
           info->iUsername = aUsername;
           info->iSecret   = aSecret;
           info->iDomain   = aDomain;
           obj_id = info->PluginSession()->AuthDialogCompletedL(info);
        }   
     }

     return obj_id;
    
}


