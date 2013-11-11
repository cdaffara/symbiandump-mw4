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
* Description:        Custom plug-in implementations offer their own API documentation
*                where detailed characteristics about how methods declared in this
*                class are implemented.
*
*/








#ifndef SEN_SERVICE_CONNECTION_IMPL_H
#define SEN_SERVICE_CONNECTION_IMPL_H

// INCLUDES
#include <flogger.h>
#include <e32property.h>

#include "sendebug.h"

#include "SenServiceConnection.h"
#ifdef __ENABLE_ALR__
	#include "senserviceconnectioninterfaces.h"
#endif //__ENABLE_ALR__
#include "seninternalserviceconnectioninterfaces.h"

#include "MSenServiceDescription.h"
#include "rsenserviceconnection.h"
#include "senpointermap.h"
#include "SenHttpTransportProperties.h"

#include "msencobrandinginterface.h"

#include "senservicedispatcher.h"
#include "senregistrationtimer.h"

// CONSTANTS
_LIT8(KServiceConnectionElementLocalName, "ServiceConnection");

// FORWARD DECLARATIONS
class CSenConnectionChunk;
class CSenAsyncOperation;
class CSenConnectionStatusObserver;
class CSenFileProgressObserver;
class CSenBinaryContainer;
class RProperty;

// TYPEDEFS
typedef RSenPointerMap<TInt, CSenConnectionChunk> RChunkMap;
typedef RSenPointerMap<TDesC8, CSenBinaryContainer> RBinaryMap;
typedef RArray<TXmlEngDataContainer> RDataContainerArray;
typedef RSenPointerMap<TInt, RDataContainerArray> RDataContainerMap;

// CLASS DECLARATION

/**
* Custom plug-in implementations offer their own API documentation
* where detailed characteristics about how methods declared in this
* class are implemented.
*/
#ifdef __ENABLE_ALR__
	class CSenServiceConnectionImpl : public CSenServiceConnection, MSenInternalServiceConnection, MSenAlrServiceConnection, MSenRegistrationActor
#else
	class CSenServiceConnectionImpl : public CSenServiceConnection, MSenInternalServiceConnection, MSenRegistrationActor
#endif //__ENABLE_ALR__
    {
        friend class CSenAsyncOperation;
        friend class CSenUnderTakerWaiter;        
    public:

        // Constructors and destructor:
        static CSenServiceConnectionImpl* NewL(MSenServiceConsumer& aObserver,
                                                const TDesC8& aUri);

        static CSenServiceConnectionImpl* NewLC(MSenServiceConsumer& aObserver,
                                                const TDesC8& aUri);

        static CSenServiceConnectionImpl* NewL(MSenServiceConsumer& aObserver,
                                    MSenServiceDescription& aServiceDescription);

        static CSenServiceConnectionImpl* NewLC(MSenServiceConsumer& aObserver,
                                    MSenServiceDescription& aServiceDescription);

        static CSenServiceConnectionImpl* NewL(MSenServiceConsumer& aObserver,
                                    CSenServicePattern& aServicePattern);

        static CSenServiceConnectionImpl* NewLC(MSenServiceConsumer& aObserver,
                                    CSenServicePattern& aServicePattern);

		static CSenServiceConnectionImpl* NewL(MSenServiceConsumer& aConsumer,
                                               const TDesC8& aContract,
                                               MSenExtendedConsumerInterface& aExtendedConsumer);

        static CSenServiceConnectionImpl* NewLC(MSenServiceConsumer& aConsumer,
                                                const TDesC8& aContract,
                                                MSenExtendedConsumerInterface& aExtendedConsumer);

        static CSenServiceConnectionImpl* NewL(MSenServiceConsumer& aConsumer,
                                               MSenServiceDescription& aServiceDescription,
                                               MSenExtendedConsumerInterface& aExtendedConsumer);

        static CSenServiceConnectionImpl* NewLC(MSenServiceConsumer& aConsumer,
                                                MSenServiceDescription& aServiceDescription,
                                                MSenExtendedConsumerInterface& aExtendedConsumer);

        static CSenServiceConnectionImpl* NewL(MSenServiceConsumer& aConsumer,
                                               CSenServicePattern& aServicePattern,
                                               MSenExtendedConsumerInterface& aExtendedConsumer);

        static CSenServiceConnectionImpl* NewLC(MSenServiceConsumer& aConsumer,
                                                CSenServicePattern& aServicePattern,
                                                MSenExtendedConsumerInterface& aExtendedConsumer);
        // Opens connection to WSF Symbian Server
        void ConnectL();  
        
        void SetConsumer( MSenServiceConsumer& aConsumer, MSenExtendedConsumerInterface* apExtConsumer = NULL );
        void Consumer( MSenServiceConsumer*& aConsumer, MSenExtendedConsumerInterface*& apExtConsumer );

        virtual ~CSenServiceConnectionImpl();

        // New functions:
        virtual TInt SendL(const TDesC8& aRequest);
        virtual TInt SubmitL(const TDesC8& aRequest, HBufC8*& aResponse);
        virtual TInt CompleteServerMessagesOnOff(const TBool& aCompleteOnOff);
        virtual TInt HasFacetL(const TDesC8& aURI, TBool& aHasFacet);
        virtual TInt IsReady(TBool& aReady);
        virtual TInt SendL(RFile& aFile);
        virtual TInt SubmitL(RFile& aFileHandle, HBufC8*& aResponse);
//        virtual TBool IsServChunkSlotAvailable();
        virtual TInt SendL(CSenSoapEnvelope& aMessage);
        virtual TInt SubmitL(CSenSoapEnvelope& aMessage, HBufC8*& aResponseTo);

        virtual TInt ServiceDescriptionL(HBufC8*& aServiceDescription);
        virtual TInt StartTransaction();
        virtual TInt TransactionCompleted();
        virtual TInt CancelAllRequests();

        virtual void WriteStartElementL( const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes);

        virtual void WriteEndElementL(const TDesC8& aNsUri,
                                      const TDesC8& aLocalName,
                                      const TDesC8& aQName);

        virtual const TDesC8& LocalName() const;
        virtual const TDesC8& NsUri() const;
        virtual const TDesC8& NsPrefix() const;
        virtual void WriteAsXMLToL(RWriteStream& aWriteStream);
        virtual HBufC* AsXmlUnicodeL();
        virtual HBufC8* AsXmlL();
        virtual TBool ConsistsOfL(MSenFragment& aCandidate);

        // Functions from base classes

        // from MSenFragment
        /**
        * @return the current object as element.
        *          Retains ownership.
        */
        virtual CSenElement& AsElement();

        /**
        * @return the current object as element. May return NULL.
        *          Caller takes ownership.
        */
        virtual CSenElement* ExtractElement();

        virtual void SetReader(CSenXmlReader& aReader);
        virtual CSenXmlReader* Reader();

        /**
        * SetReader() must be called before this method can be used.
        */
        virtual void ParseL(const TDesC8& aBuf);

        /**
        * Same as ParseL() except that it takes 16-bit input and does silently
        * fail in case of all errors.
        * SetReader() must be called before this method can be used.
        * @return error code, if any.
        */
        virtual TInt BuildFrom(const TDesC8& aBuf);

        /**
        * Let the delegate CSenFragment handle with SAX events.
        * "this" CSenFragment is made the owner of the delegate and
        * the delegate is expected to make this CSenFragment the receiver
        * of SAX events once it has seen the end element for itself.
        */
        virtual void DelegateParsingL(MSenFragment& aDelegate);

        /**
        * Let a new CSenFragment for a new CSenBaseElement with the given
        * namespace and name handle SAX events. The new MSenElement is made
        * a child of the MSenElement of this CSenFragment.
        */
        virtual void DelegateParsingL(  const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        const RAttributeArray& aAttributes);

        virtual void ParseWithL(CSenXmlReader& aReader);
        virtual void SetOwner(MSenFragment& aFragment);
        virtual void ResumeParsingFromL( const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName );

        virtual void SetAttributesL(const RAttributeArray& aAttributes);
        virtual TInt TxnId();
        virtual MSenMessage* Response();
        virtual TInt CancelTransaction(TInt aTransactionID);
        virtual TInt SetTransportPropertiesL(const TDesC8& aProperties);
        virtual TInt TransportPropertiesL(HBufC8*& aProperties);
        virtual TInt SendL(const TDesC8& aRequest, const TDesC8& aProperties);
        virtual TInt SendL(CSenSoapEnvelope& aMessage, const TDesC8& aProperties);
        virtual TInt SubmitL(CSenSoapEnvelope& aMessage, const TDesC8& aProperties, HBufC8*& aResponseTo);
        virtual TInt SubmitL(const TDesC8& aRequest, const TDesC8& aProperties, HBufC8*& aResponse);
        virtual TInt AssignMessageToChunkL(CSenChunk& aChunk, MSenMessage& aMessage);	//codescannerwarnings
        virtual TInt SendL(MSenMessage& aMessage);
        virtual TInt SubmitL(MSenMessage& aMessage, CSenSoapEnvelope2*& aResponseTo);
        virtual TInt IdentityProviderL(CSenIdentityProvider*& apIdentityProvider);
        virtual TInt Identifier();
		
				virtual void DataTrafficDetails( TSenDataTrafficDetails& aDetails,
										 TSenDataTrafficOperations& aOperations );
										 


#ifdef __ENABLE_ALR__
    	virtual void MigrateToPrefferedCarrierL(TBool &aUserChoice) ;
			virtual void NewCarrierAcceptedL(TBool &aUserChoice) ;		
#endif
      virtual TInt PendingTrasanctionsCount();
      
      // From MSenInternalServiceConnection
			/**
        * Sends information about BLOB transfer progres to hostlet.
        *
        * @param aTxnId Transaction ID.
        * @param aIncoming ETrue if it is incoming BLOB, EFalse if outgoing.
        * @param aMessage SOAP message for incoming messages with BLOBs.
        * @param aCid CID of current BLOB.
        * @param aProgress Count of sent/received BLOB bytes.
        */
        virtual TInt SendProgressToHostlet(TInt aTxnId, TBool aIncoming, const TDesC8& aMessage, const TDesC8& aCid, TInt aProgress);
        
        TPtrC8 InitDescription();
        
    protected: // Functions from base classes

        // from CActive
        void RunL();
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CSenServiceConnectionImpl(MSenServiceConsumer& aObserver,
                                  MSenExtendedConsumerInterface* aExtendedConsumerInteface);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(MSenServiceDescription& aPattern);

        // New functions
        TInt SendL(CSenSoapEnvelope2& aMessage);

        TInt SubmitL(CSenSoapEnvelope2& aMessage, CSenSoapEnvelope2*& aResponseTo);


        TInt SendL(CSenConnectionChunk* aClientOp);

        TInt SubmitL(CSenConnectionChunk& aClientOp);

        TInt ResponseFromChunk(CSenChunk& aClientOp, HBufC8*& aResponseTo);

        TInt SubmitL(CSenConnectionChunk& aClientOp, CSenSoapEnvelope2*& aResponseTo);

        TInt CheckConnection();

        void InitializeL( /*MSenServiceDescription& aPattern */ );

        void DeliverResponseL(const TInt aErrorCode, CSenChunk* apOperation = NULL);

        RChunkMap& ChunkMapL();

        TPtrC SessionID();

        TPtrC NextChunkName();

        RPointerArray<CSenAsyncOperation>& AsyncOpsArrayL();	//codescannerwarnings

        void HandleMessageFromChildAOL(TInt aStatus);

        TInt AssignPropertiesToChunkL(CSenChunk& aChunk, CSenSoapEnvelope& aMessage,
                                     const TDesC8& aProperties = KNullDesC8);	////codescannerwarnings

        TInt MoveBinaryContainersToServer(CSenConnectionChunk& aOperation,
                                          RArray<TXmlEngDataContainer>& aList);
		TInt MoveFileChunkHandleToServer(CSenConnectionChunk& aOperation);


        RFileLogger* Log() const;

        void DeliverStatus(TInt aStatus);

        /*
        * Method gets the Http method from the transport properties
        * @param aProperties contains the transport properties in serialized (UTF-8) form.
        * @param aMethod A TSenHttpMethod reference to be filled in with
        *                    the value of the Http method.
        * @return KErrNone if successful or otherwise some system-wide error code.
        * KErrNotFound if Http method is not found
        */
        TInt HttpMethodFromPropertiesL( const TDesC8& aProperties,
                                        CSenHttpTransportProperties::TSenHttpMethod& aMethod );

		TInt SearchIdentityProviderL(CSenIdentityProvider*& aSenChunk, const TDesC8& aProviderId);
        TInt RegisterAuthenticationObserver();
        TBool HasConnectionAgentCallbackInitialized();
        TInt ReauthNeededL(const TDesC8& aProviderId);
		TInt RegisterFilesObserver();
        const TThreadId OwnerThreadId() const;
        TInt RegisterCoBrandingObserver();
        
        TInt RegisterAndSubscribeFileProgressObserversL();
        
        virtual TAny* InterfaceByUid( TUid aUID );
        
        
#ifdef __ENABLE_ALR__
	TInt RegisterMobilityObserverL() ;
#endif

    private: // Data
        RFileLogger                         iLog;
        RSenServiceConnection               iConnection;    // owned
        MSenServiceConsumer*                iObserver;      // not owned
        HBufC8*                             iInBuf;         // owned
        TPtr8                               iInBuffer;
        HBufC8*                             iOutBuf;        // owned
        TPtr8                               iOutBuffer;
        TBool                               iInitialized;
        CSenBaseFragment*                   iFragment;
        TInt                                iResponseLength;
        TInt                                iErrorNumber;
        TPtr8                               iErrorNumberBuffer;
        TInt                                iTxnId;
        TPtr8                               iTxnIdBuffer;

        RChunkMap*                          iChunkMap;

        HBufC*                              ipSessionId;
        TPtr                                iSessionIdBuffer;
        TInt                                iChunkNameNumber;
        HBufC*                              ipChunkName;

        TBool                               iTransactionIdKnown;
        TUint                               iAsyncOpsCount;
        RPointerArray<CSenAsyncOperation>*  iAsyncOpsArray;

        MSenMessage*                        iResponse;

        CSenChunk*                          ipInitChunk; // owned
        TBool*                              ipCompleteServerMsgsOnOff; // owned

        CSenConnectionStatusObserver*       ipConnectionStatusObserver; // owned
        CSenFileProgressObserver*			ipFileProgressObserver;     // owned

        TInt                                iConnectionID;
        MSenFilesObserver*                  iFilesObserver;	// connectionagent observer
        //TInt                                iTlsLogChannel;
        TInt                                iTlsLogStatusChannel;

        HBufC8*                             ipInitPatternAsXml;
        HBufC8*                             ipInitConsumerPolicyAsXml;
        MSenAuthenticationProvider*         ipAuthProvider; // connectionagent observer
        TBool								iConnectionAgentCallbackInitialized;
        TBool								iRegisterFileObserverDone;
        TBool							    iRegisterMobilityObserverDone;
    	TBool								iRegisterAuthObserverDone;
    	TBool								iRegisterCoBrandingObserverDone;
        TBool								iReauthenticationNeeded;
        MSenCoBrandingObserver*             ipCoBrandingObserver;
        MSenHostletConsumer*                ipHostletConsumer;
        MSenExtendedConsumerInterface*      ipExtendedConsumerInterface; // connectionagent observer
        TAny*                               ipInterface;
#ifdef __ENABLE_ALR__
        MMobilityProtocolResp *				iMobiltyObserver;
		TBool                               iMobiltyCallBackReceived;
#endif
        TBool 								iCancelSession;
        TBool 								iIsInternalSC;
    
        TThreadId                           iOwnerThreadId;
        CSenServiceDispatcher*              ipSenServiceDispatcher;
        CSenUnderTakerWaiter*               ipSenUnderTakerWaiter;
        TInt                                iVirtualTransactionCounter;
        TBool 								iDispatcherEnabled;
        CSenRegistrationTimer*              ipRegistrationTimer;
        RFile 								iSharedFileHandle ;		
    };

#ifdef EKA2
NONSHARABLE_CLASS(CSenAsyncOperation) : public CActive
#else
class CSenAsyncOperation : public CActive
#endif
    {
    public: // Constructors and destructor
        static CSenAsyncOperation* NewL(CSenServiceConnectionImpl* aActive);
        static CSenAsyncOperation* NewLC(CSenServiceConnectionImpl* aActive);
        virtual ~CSenAsyncOperation();

    protected: // Functions from base classes
        // from CActive
        void RunL();
        void DoCancel();
       /* TInt RunError(TInt aError); */


    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        CSenAsyncOperation(CSenServiceConnectionImpl* iActive);

    public: // Data
        CSenServiceConnectionImpl* iActive;
        TInt    iErrorNumber;
        TPtr8   iErrorNumberBuffer;
        TInt    iTxnId;
        TPtr8   iTxnIdBuffer;
		TInt	iReauthTxnId; // This is used to find connection chunk in the reauthentication loop
    };

#ifdef EKA2
NONSHARABLE_CLASS(CSenConnectionStatusObserver) : public CActive
#else
class CSenConnectionStatusObserver : public CActive
#endif
    {
    public:
        static CSenConnectionStatusObserver* NewL(MSenServiceConsumer& aObserver,
                                                  TInt aConnectionID);
        static CSenConnectionStatusObserver* NewLC(MSenServiceConsumer& aObserver,
                                                   TInt aConnectionID);
        ~CSenConnectionStatusObserver();

    private:
        CSenConnectionStatusObserver(MSenServiceConsumer& aObserver,
                                     TInt aConnectionID);
        void ConstructL();
        void RunL();
        void DoCancel();

    private:
        RProperty                   iConnectionStatusProperty;
        MSenServiceConsumer*        iObserver;                  // not owned
        TInt                        iConnectionID;
    };
    
    
#ifdef EKA2
NONSHARABLE_CLASS(CSenFileProgressObserver) : public CActive
#else
class CSenFileProgressObserver : public CActive
#endif
    {
    public:
        static CSenFileProgressObserver* NewL( MSenFilesObserver& aObserver,
                                               TInt aConnectionID,
                                               TBool aIsDispatcherEnabled,
                                               CSenServiceDispatcher* apSenServiceDispatcher, 
                                               RChunkMap& aChunkMap );

        static CSenFileProgressObserver* NewLC( MSenFilesObserver& aObserver,
                                                TInt aConnectionID,
                                                TBool aIsDispatcherEnabled,
                                                CSenServiceDispatcher* apSenServiceDispatcher, 
                                                RChunkMap& aChunkMap );
        ~CSenFileProgressObserver();

    private:
        CSenFileProgressObserver( MSenFilesObserver& aObserver,
                                  TInt aConnectionID,
                                  TBool aIsDispatcherEnabled,
                                  CSenServiceDispatcher* apSenServiceDispatcher, 
                                  RChunkMap& aChunkMap );
        void ConstructL();
         /*
        * Method update the consumer about the file progress transfer info
        */
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    private:
        RProperty                   iFileProgressProperty;
        MSenFilesObserver*        	iFileProgressObserver;        // not owned
        TInt                        iConnectionID;
        TBool                       iIsDispatcherEnabled;
        CSenServiceDispatcher*      ipSenServiceDispatcher;
        RChunkMap&                  iChunkMap;
    };    

#ifdef EKA2
NONSHARABLE_CLASS(CSenBinaryContainer) : public CBase
#else
class CSenBinaryContainer : public CBase
#endif
    {
    public:
    
    
        enum TDataType
            {
            EFile     = 0,
            EChunk      = 1,
            ENumberOfTypes = 2
            };

        // Constructors and destructor
        static CSenBinaryContainer* NewL( const TDesC8& aCid, RFile aFile );
        static CSenBinaryContainer* NewLC( const TDesC8& aCid, RFile aFile );

        static CSenBinaryContainer* NewL( const TDesC8& aCid, RChunk aChunk, TInt aChunkSize, TInt aChunkOffset );
        static CSenBinaryContainer* NewLC( const TDesC8& aCid, RChunk aChunk, TInt aChunkSize, TInt aChunkOffset  );

        TBool IsFile() const;  // checks iType (TDataType)
        TBool IsChunk() const; // checks iType (TDataType)
        TDataType Type() const;
        
        TPtrC8 Cid() const;
        const HBufC8* CidPtr() const;
        RFile File() const;
        RChunk Chunk() const;
        TInt ChunkSize() const;
        TInt ChunkOffset() const;

        ~CSenBinaryContainer();

    private:
        void ConstructL( const TDesC8& aCid, RFile aFile );
        void ConstructL( const TDesC8& aCid, RChunk aChunk, TInt aChunkSize, TInt aChunkOffset );

        CSenBinaryContainer( TDataType aType );
        

    private: // Data
        // "a temporary fix" for issue 21  => in reality, A MAP IS NEEDED (cid, <handle>)  in here (or ipDataContainerArray needs to hold a copy!)
        TDataType                     iType;
        HBufC8* 					  ipCid;
        RFile				   		  iFile;
        RChunk				   		  iChunk;
        TInt                          iChunkSize;
        TInt                          iChunkOffset;
    };




#ifdef EKA2
NONSHARABLE_CLASS(CSenConnectionChunk) : public CSenChunk
#else
class CSenConnectionChunk : public CSenChunk
#endif
    {
    friend class CSenServiceConnectionImpl;
    
    public:
        // Constructors and destructor
         static CSenConnectionChunk* NewL(const TDesC& aChunkName, TBool aInternal = EFalse );
        static CSenConnectionChunk* NewLC(const TDesC& aChunkName, TBool aInternal = EFalse );

        ~CSenConnectionChunk();

        // New methods:
        RArray<TXmlEngDataContainer>& BinaryDataArrayL();	//codescannerwarnings
        TBool HasBinaryDataArray();
         TBool HasSharedFileHandle();
        void HasSharedFileHandle(TBool aHasSharedFileHandle);
        void RemoveBinaryDataArrayIfEmpty();
        TInt AddFileToContainerMap( const TDesC8& aCid, RFile aFile );
        TInt AddChunkToContainerMap( const TDesC8& aCid, RChunk aChunk, TInt aChunkSize, TInt aChunkOffset );
        RBinaryMap& ContainerMap();
        TInt ContainerCount();

#ifndef RD_SEN_ENABLE_USE_OF_WSD_IN_SC_FOR_HW_TARGET
        // in WINSCW environment, we don't need to use WSD, since there is no limitation for how many chunk handles a single process can have open
        TInt ServOpenChunkCount();
#else
		// use WSD only in hardware builds, since in emulator environment, only a single process can use WSD(!)
		static TInt ServOpenChunkCount();
#endif       
    private:
        void ConstructL(const TDesC& aChunkName);

        CSenConnectionChunk();

    private: // Data
        RArray<TXmlEngDataContainer>* ipDataContainerArray; // owned
        TBool iHasSharedFileHandle ;		
        // Proper solution uses the map(!):
        RBinaryMap                    iMap;
#ifndef RD_SEN_ENABLE_USE_OF_WSD_IN_SC_FOR_HW_TARGET
    	TInt iServOpenChunkCount; // WINS env: no need to have static member. To monitor Server side Open chunk count
#else
    	static TInt iServOpenChunkCount; // ARMv5 env: we need stati member. To monitor Server side Open chunk count
#endif        
    };

#endif //SEN_SERVICE_CONNECTION_IMPL_H

// End of File
