/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header declaration
*
*/











#ifndef SEN_CLIENTSESSION_H
#define SEN_CLIENTSESSION_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <flogger.h>

#include "msenremoteserviceconsumer.h"
#include "msenremotehostlet.h"
#include "msenservicemanager.h"
#include "msenremoteservicesession.h"
#include "senpointermap.h"
#include "senchunk.h"

#include "sentransportbase.h"

#include "msenmessagecontext.h" // application domain: Framework\inc
#include "senmessagecontext.h"

#include "sencontext.h"         // private: Framework\inc

#include "RSenDocument.h"
#include "senservicemanagerdefines.h"
#include "senatomentry.h"
#ifdef __ENABLE_ALR__
#include "senmobilityobserver.h"
#endif    //__ENABLE_ALR__

// CONSTANTS
const TInt KErrConsumerIdIsNull = 84;

// FORWARD DECLARATIONS
class CSenIdentifier;
class CSenServiceSession;
class CSenClientMessage;
class CSenSoapEnvelope2;
class TXmlEngElement;
class TXmlEngDataContainer;
class RSenDocument;
class CSenMessageContext;
class CSenServerContext;
class RSenConnectionServerSession;
class TFileTransferProgress;

typedef RSenPointerMap<TInt, CSenClientMessage> RHostletRequestMap;

//typedef RPointerArray<RMessage2> RTransactionArray;
typedef RPointerArray<CSenClientMessage> RTransactionArray;
typedef RPointerArray<TInt> RTransactionIdArray;


NONSHARABLE_CLASS(CSenMessageResourceHandler): public CActive
    {
    friend class RSenConnectionServerSession;
    private:
    CSenMessageResourceHandler(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
            TInt aProgress, RSenConnectionServerSession* aOwner);
    ~CSenMessageResourceHandler();
    void DoCancel();
    void RunL();
    TInt SetBuffer(const TDesC8& aSoapOrCid);
    
    private:
    TPckgBuf<TTransferProgress> iProgressData;
    HBufC8* iSoapOrCid;
    RSenConnectionServerSession* iOwner;
    CSenMessageResourceHandler* iNext;
    CSenMessageResourceHandler* iPrevious;
    };
    
//class RSenConnectionServerSession: public RSessionBase
NONSHARABLE_CLASS(RSenConnectionServerSession): public RSessionBase
    {
    public:
    TInt CreateSession(const RMessage2& aMessage);
    void SendFileProgress(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
            const TDesC8& aSoapOrCid, TInt aProgress);
    void Remove(CSenMessageResourceHandler* aResourceHandler);
    void Close();
    
    TInt InitProgressObserver(const RMessage2& aMessage);
    TInt InitAuthObserver(const RMessage2& aMessage);
    TInt ReauthenticationNeeded(CSenChunk& aSenChunk);
        TInt InitCoBrandingObserver(const RMessage2& aMessage);
        TBool OnGetBrandIdL(  const TDesC8& aBrandIdListAsXmlSnippet, RBuf8& aSelectedBrandId);
#ifdef __ENABLE_ALR__
        TInt InitMobilityObserver(const RMessage2& aMessage) ;
	
        TInt PreferredCarrierAvailable( TAccessPointInfo& aOldAPInfo, TAccessPointInfo& aNewAPInfo, TBool& aIsUpgrade, TBool& aIsSeemLess) ;
    	
        TInt NewCarrierActive (TAccessPointInfo& aNewAPInfo, TBool& aIsSeamless ) ;
        TInt MobilityError( TInt& aError ) ;	
#endif    //__ENABLE_ALR__
    private:
    CSenMessageResourceHandler* iFirst;
    };

NONSHARABLE_CLASS(CSenConnAgentSync): public CActive
    {
    friend class RSenConnectionServerSession;
private:
    CSenConnAgentSync();
    ~CSenConnAgentSync();
    void DoCancel();
    void RunL();
    void Start();
    };

NONSHARABLE_CLASS(CSenBinaryData): public CBase
    {
    public:
        enum TSenBinaryDataType
            {
            EFile   = 1,
            EChunk  = 2
            };

        static CSenBinaryData* NewL(RChunk& aChunk, TDesC8& aCid, TInt aOffset, TInt aSize);
        static CSenBinaryData* NewLC(RChunk& aChunk, TDesC8& aCid, TInt aOffset, TInt aSize);
        static CSenBinaryData* NewL(RFs& aFs, RFile& aFile, TDesC8& aCid, TInt aOffset = 0, TInt aSize = 0);
        static CSenBinaryData* NewLC(RFs& aFs, RFile& aFile, TDesC8& aCid, TInt aOffset = 0, TInt aSize = 0);
            
        ~CSenBinaryData();
        
        RChunk& Chunk();
        RFile& File();
        TDesC8& Cid();
        TInt Offset();
        TInt Size();
        TSenBinaryDataType Type();

    protected: 
        CSenBinaryData(TInt aOffset, TInt aSize);
        void BaseConstructL(RChunk& aChunk, TDesC8& aCid);
        void BaseConstructL(RFs& aFs, RFile& aFile, TDesC8& aCid);
    
    private:
        RChunk              iChunk;
        
        RFs                 iFs;
        RFile               iFile;
        
        TSenBinaryDataType  iType;
        HBufC8*             ipCid;
        TInt                iOffset;
        TInt                iSize;
    };

// CLASS DECLARATION
#ifdef __ENABLE_ALR__
class CSenClientSession : public CSession2, public MSenRemoteServiceConsumer, public MSenRemoteHostlet, public MMobilityProtocolResp
#else
class CSenClientSession : public CSession2, public MSenRemoteServiceConsumer, public MSenRemoteHostlet
#endif
    {
    public: // Constructors and destructor
        
        static CSenClientSession* NewL( MSenServiceManager& aServer, CSenServerContext& aCtx );
        static CSenClientSession* NewLC( MSenServiceManager& aServer, CSenServerContext& aCtx );

        ~CSenClientSession();

        // Function from base classes
        
        // from CSession2
        void ServiceL(const RMessage2& aMessage);

        // from MSenRemoteServiceConsumer
        virtual const TDesC8& Id() const;
        CSenIdentifier& Identifier() const;

        TInt SetSessionL(MSenRemoteServiceSession& aServiceSession);
        TInt HandleMessageL(HBufC8* apMessage,
                            const TInt aTxnId,
                            MSenProperties* aResponseTransportProperties);
                            
        TInt HandleErrorL(HBufC8* apError,
                          const TInt aErrorCode,
                          const TInt aTxnId,
                          MSenProperties* aResponseTransportProperties);

        void SetStatusL(const TInt status);
        void FileProgress(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
                const TDesC8& aSoapOrCid, TInt aProgress);
        void TransferProgressForHostlet( TInt aTxnId, TBool aIsIncoming, TBool aIsSoap, const TDesC8& aSoapOrCid, TInt aProgress);
        void SetDataTrafficDetails( TSenDataTrafficDetails& aDetails); 

        MSenTransport& TransportL();

        TInt ProcessRequestFromConsumerL(const TDesC8& aMessage,
                                         const TInt aTxnId,
                                         MSenRemoteServiceConsumer& aConsumer); 

        TInt ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk);
        MSenMessageContext* MessageContextByTxnIdL(TInt aTxnId, TInt& aError);

        TInt OnServiceCompleteL(const TInt aTxnId, TInt aCompletionCode, const TDesC8& aDesc = KNullDesC8); 
        
        void StateChanged(TInt aState);
        
        TBool HasAuthenticationCallback();
        TInt ReauthenticationNeededL(CSenIdentityProvider*& aIdp);
        TBool OnGetBrandIdL(  const TDesC8& aBrandIdListAsXmlSnippet, RBuf8& aSelectedBrandId);
        TBool HasCoBrandingCallback();		
#ifdef __ENABLE_ALR__
		void ObserveMobiltyService(const RMessage2& aMessage) ;
		void MigrateToPrefferedCarrierL(const RMessage2& aMessage) ;	
		void PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
	                                        TAccessPointInfo aNewAPInfo,
	                                        TBool aIsUpgrade,
	                                        TBool aIsSeamless ) ;
	    void Error( TInt aError );
	    void NewCarrierActive( TAccessPointInfo aNewAPInfo, TBool aIsSeamless ) ;
		void NewCarrierAcceptedL(const RMessage2& aMessage) ;		
#endif //__ENABLE_ALR__
    protected: 
        CSenClientSession(MSenServiceManager& aServer, CSenServerContext& aCtx);
        void ConstructL();

        // New functions
        
        void PanicClient(const RMessagePtr2& aMessage,TInt aPanic) const;
        void InitializeL(const RMessage2& aMessage);
        void TransactionL(const RMessage2& aMessage);
        TInt SearchMsgChunk( const RMessage2& aMessage, CSenChunk*& aHit );
        void SendMsg( const RMessage2& aMessage ); // async
        void SendMsgL( const RMessage2& aMessage, CSenChunk& aSenChunk ); // async
        RFileLogger* Log() const;
        void InstallFrameworkL(const RMessage2& aMessage);
        void DissociateServiceL(const RMessage2& aMessage);
        void AssociateServiceL(const RMessage2& aMessage);
        void RegisterIdentityProviderL(const RMessage2& aMessage);
        void UnregisterIdentityProviderL(const RMessage2& aMessage);
        void RegisterServiceDescriptionL(const RMessage2& aMessage);
        void UnregisterServiceDescriptionL(const RMessage2& aMessage);

        /**
        * Searches and returns all service descriptions, which contain
        * matching fields (elements) with spesified service pattern.
        * Typical fields used in service pattern are Endpoint and 
        * Contract.
        */
        void ServiceDescriptionsByPatternL(const RMessage2& aMessage);
        /**
        * Searches and returns all service descriptions, which match
        * with specified Contract (typically some URI).
        */
        void ServiceDescriptionsByUriL(const RMessage2& aMessage);

        void IsReadyL(const RMessage2& aMessage);
        void HasFacetL(const RMessage2& aMessage);
        void CompleteServerMessagesOnOffL(const RMessage2& aMessage);
        void ReceiveServiceDescriptionL(const RMessage2& aMessage);
        void RequestServiceDescriptionL(const RMessage2& aMessage);
        void StartTransactionL(const RMessage2& aMessage);
        void TransactionCompletedL(const RMessage2& aMessage);
        void CancelSessionL(const RMessage2& aMessage);
        void TransportPropertiesL(const RMessage2& aMessage);
        void SetTransportPropertiesL(const RMessage2& aMessage);
    
        void EstablishHostletConnectionL(const RMessage2& aMessage);
        void AwaitHostletRequestL(const RMessage2& aMessage);
        void ProvideHostletResponseL(const RMessage2& aMessage);
//        void AcquireHostletRequestHandleL(const RMessage2& aMessage);
        void MoveChunkL(const RMessage2& aMessage);
        void MoveFileL(const RMessage2& aMessage);
        void SendFileHandleL(const RMessage2& aMessage);
        TInt SendProgressToHostlet(const RMessage2& aMessage);
        CSenClientMessage* ClientMessageByPendingTxnIdL( TInt aTxnId);

    private:
    
        // Defines, whether this client session is a service connection (SC) or a hostlet connection (HC)
        enum TSessionType
            {
            ENotInitialized = 0,
            ERemoteConsumer_SC,
            ERemoteHostlet_HC
            };

        // User permission check states
        enum TestCase
            {
            ENotChecked = 0,
            EAccepted,
            ENotAccepted
            };

        // New functions

        /**
        * SessionValidity() method checks session validity 
        * @return KErrNone if session is ready and 
        *         its credentials (if any) are valid
        *
        *         KErrSenNotInitialized if session is not yet set 
        *         (iSession is NULL)
        *
        *         KErrConnectionInitializing if session's state  is new 
        *         (iStatus is KSenConnectionStatusNew)
        *
        *         KSenConnectionStatusExpired if session's state is initializing 
        *         (iStatus is KSenConnectionStatusNew)
        *
        *         KSenConnectionStatusExpired if session's state is expired 
        *         (iStatus is KSenConnectionStatusExpired)
        */
        TInt SessionValidity();

        TBool CheckValidity(const RMessage2 &aMessage);
        TBool CheckAllowRegisterL(TInt& aError, const RMessage2& aMessage, const TDesC8& aFrameworkId);
        TBool CheckAllowUnRegisterL(TInt& aError, const RMessage2& aMessage, const TDesC8& aFrameworkId);
        TBool CheckAllowUnRegisterL(TInt& aError, const RMessage2& aMessage);
        /**
        * ReleaseServiceSession is called in the destructor of this class to
        * make sure that framework spesific garbage collection for consumed 
        * service session will be executed. It is up to framework implementations
        * whether they need to cleanup (destroy) this service session which is
        * being here released.
        */
        TInt ReleaseServiceSession();

        /**
        * Method is called from destructor, to ensure that
        * all pending async requests get completed.
        *
        * Messages will be completed using ESenServRequestCancelled
        * operation code from TWsfServRqstOpCode enumeration defined
        * in SenServiceManagerDefines.h
        *
        * Internally, this method calls and TRAPS CleanupPendingMessagesL()
        * @return KErrNone or leave code, if CompletePendingMessagesL() fails.
        */
        TInt CompletePendingMessages(); 
        /**
        * Leaving variant of CleanupPendingMessages(), which includes
        * actual cleanup code.
        *
        * This version may be called if *cancel operation* is received
        * from client to cancel all pending -async- operations.
        */
        void CompletePendingMessagesL();

		TInt TransactionIndex(TInt aTxnId);
        TInt ContextIndex(TInt aCtxId);

        RTransactionArray& Transactions();
        RTransactionIdArray& TxnIds();
        RHostletRequestMap& HostletRequestMap();
        
        TInt CompleteTransaction(TInt aErrorCode, CSenChunk& aChunk);
        TInt CompleteTransactionByCtxId(TInt aErrorCode, TInt aCtxId);
        TInt CompleteTransactionByIndex(TInt aErrorCode, TInt aTransactionIndex);

        TInt ReadChunkNameLC( const RMessage2& aMessage, TInt aIpcArgIndex, HBufC*& apChunkName );

        /**
        * Helper called by SC::ReceiveInitialize and HC:EstablishConnection
        * Instantiates a new XML service description object, which then will
        * parse the provided XML (serialized XML service description)
        * @param aSdAsXml is the input XML descriptor
        * @param apInitializer provides ref-to-ptr where new initializer 
        * (a XML service description) will be assigned, when method succeeds.
        * @param apConsumerPolicy is optional argument, which can be utilized
        * to provide consumer policy descriptor, when such is available.
        * @return KErrNone if session was initialized or 
        * otherwise some system-wide errorcode.
        */
        TInt ParseSessionInitializerL(CSenWSDescription*& apInitializer,
                                      const TDesC8& aSdAsXml,
                                      const TDesC8* apConsumerPolicy = NULL);

        /**
        * Helper called by SC::ReceiveInitialize and HC:EstablishConnection
        * Initializes the service session or returns an error.
        * @param aInitializer is the SD to be used in session initialization
        * @param aErrorMsg will contain error description, typically SOAP fault,
        * in case that (IDP / authentication) service spesific error occured
        * @return KErrNone if session was initialized or 
        * otherwise some system-wide errorcode.
        */
        TInt InitializeServiceSessionL(CSenWSDescription& aInitializer, HBufC8*& aErrorMsg);

        /**
        * Helper.
        * Reads a Service Description (buffer) from certain 
        * RMessage2 IPC ARGS indexes
        * @return KErrNone if buffer was successfully read
        * or otherwise some system-wide errorcode.
        */
        TInt ReadBufferFromRMessageL(const RMessage2& aMessage, 
                                     TInt aSdLengthIdx, 
                                     TInt aSdBufIdx,
                                     HBufC8*& aSdBuf);

        TInt ListBinaryElementsL(RArray<TXmlEngElement>& aElementArray,
                                 TXmlEngElement& aElement);
                                 
        TInt ParseMessageL(TInt aTransactionId,
                           const TDesC8& aRequest,
                           CSenSoapEnvelope2& aSoapEnvelope);
        TInt ParseMessageL(TInt aTransactionId,
                           const TDesC8& aRequest,
                           CSenAtomEntry& aAtomEntry);
        void AddCredentialL(const RMessage2& aMessage);
        void CredentialsL(const RMessage2& aMessage);
        void RemoveCredentialsL(const RMessage2& aMessage);
        
        void CancelRequestL(const RMessage2& aMessage);
        
        void DataTrafficDetails(const RMessage2& aMessage); 
        void ConnectionID(const RMessage2& aMessage);
        
        void ConnectionIdentityProviderL(const RMessage2& aMessage);
//        CSenIdentityProvider* ConnectionIdentityProviderL();
        // Constantantness on the TInt return type has no meaning so prototype is modified 
        // Overrides TInt MSenRemoteServiceConsumer::ConnectionId()
     	TInt ConnectionId();
     	
     	void SearchIdentityProviderL(const RMessage2& aMessage);
     	void IdentityProviders(const RMessage2& aMessage);
     	void ObserveTransfer(const RMessage2& aMessage);
     	void ObserveAuthCallback(const RMessage2& aMessage);
     	TInt CompleteReauthLoop(CSenChunk& aSenChunk);
        TBool IsTrustedClient(const RMessage2& aMessage);
    	TBool CleanPassword(const RMessage2& aMessage);
    	virtual TSecureId SecureId();
        virtual TVendorId VendorId();

     	void ObserveCoBranding(const RMessage2& aMessage);

        void DeliverProgressToClient( TInt aTxnId, TBool aIsIncoming, TBool aIsSoap, const TDesC8& aSoapOrCid, TInt aProgress );


#ifdef __ENABLE_ALR__
    	void MigrateToPrefferedCarrier(TBool aUserChoice) ;
    	void NewCarrierAcceptedL(TBool aUserChoice) ;
#endif //__ENABLE_ALR__

    private: // Data
        MSenServiceManager &iManager;
        MSenRemoteServiceSession* iServiceSession;  // not owned
        //CSenServiceSession *iServiceSession;      // not owned
        TInt iStatus;
    
        // can be used for routing of messages :
        // could use the latest UUID for this too..
        HBufC8* iConsumerID;    
        HBufC8* iPolicyBuf;     
    
        // data to be written to client
        CBufFlat* iSendBuf; 

        CSenWSDescription* iInitializer;
        TInt iAllowWSDataReg;
        TInt iAllowWSDataUnReg;

        CSenTransportBase* ipTransport;
        CSenIdentifier* ipIdentifier;
        CSenClientMessage* ipHostletAwaitOp;

        RTransactionArray   iPendingTransactions;   // owned pointers to heap
        RTransactionIdArray iPendingTxnIds;       // owned pointers to heap
        RHostletRequestMap  iHostletRequests;      // keys and values are owned

        //RTransactionArray*      ipPendingTransactions;   // owned pointers to heap
        //RTransactionIdArray*    ipPendingTxnIds;       // owned pointers to heap
        //RHostletRequestMap*     ipHostletRequests;      // keys and values are owned

        TSessionType iSessionType;

        TInt iAllowShowDialogCount;
        
        TInt iConnectionID;
        
        TInt iAuthenticationRetries;
        TBool iConnectionHasAuthenticationCallback;
        
        CSenServerContext& iServerContext;
        TSecureId iSecureId; 
        TVendorId iVendorId;
        //session to client side server
        RSenConnectionServerSession iConnectionSession;
        TSenDataTrafficDetails iDetails;
		
		TBool iFwNotified;
		
		// Cache the client secure id to update clients about connection status
        TSecureId iClientSecureID;
        TBool iConnectionHasCoBrandingCallback;
        TBool iReauthResendNeeded;
#ifdef __ENABLE_ALR__        
       	CALRObserver* iALRObserver;	// mobilty observer		
		MMobilityProtocolResp* iMobiltyObserver ;
		TBool				iMobilityCallBackForwarded ;
		TBool				iMobilityChoice ;
#endif //__ENABLE_ALR__
        };

// CLASS DECLARATION
NONSHARABLE_CLASS(CSenClientMessage): public CSenChunk
    {
    public: 
        // Constructors and destructor
        static CSenClientMessage* NewL(const RMessage2& aMessage, MSenRemoteServiceConsumer* apConsumer = NULL);
        static CSenClientMessage* NewLC(const RMessage2& aMessage, MSenRemoteServiceConsumer* apConsumer = NULL);
        
        static CSenClientMessage* NewL(const RMessage2& aMessage, MSenRemoteServiceConsumer* apConsumer, MSenTransport& aTransport);
        static CSenClientMessage* NewLC(const RMessage2& aMessage, MSenRemoteServiceConsumer* apConsumer, MSenTransport& aTransport);
        
        ~CSenClientMessage();

        // New methods:
        RMessage2& RMessage();
        MSenRemoteServiceConsumer* Consumer();
        
        TBool operator==(const CSenClientMessage& a);
        
        MSenMessageContext& MessageContext();
        
        RPointerArray<CSenBinaryData>& BinaryDataArray();
        
        TInt CtxId();

    private:
        void ConstructL(const RMessage2& aMessage, 
                        MSenRemoteServiceConsumer* apConsumer = NULL);
        
        void ConstructL(const RMessage2& aMessage, 
                        MSenRemoteServiceConsumer* apConsumer,
                        MSenTransport& aTransport);
        CSenClientMessage();

        
    private: // Data
        CSenMessageContext*             ipMessageContext;
        RMessage2                       iMessage;
        MSenRemoteServiceConsumer*      ipConsumer; // not owned
        RPointerArray<CSenBinaryData>   iBinaryDataArray;
    };


NONSHARABLE_CLASS(TFileOutgoingTransferProgress): public TFileOutgoingTransferProgressBase
    {
    public: 
        //~TFileOutgoingTransferProgress();    
        TFileOutgoingTransferProgress( TInt aTxnId, TBool aIsIncoming, TInt aProgress, TBool aHasCidPostfix, TInt aCid, TInt aCid2 = -1 );
    };


NONSHARABLE_CLASS( TFourInts )
    {
    public:
    	TInt iInt1;
    	TInt iInt2;
    	TInt iInt3;
    	TInt iInt4;
	};

#endif // SEN_CLIENTSESSION_H

// End of File
