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
* Description: Header declaration
*
*/








#ifndef R_SEN_SERVICE_CONNECTION_H
#define R_SEN_SERVICE_CONNECTION_H

// INCLUDES
#include <e32base.h>
#include <flogger.h>
#include "senchunk.h"
#include "senservicemanagerdefines.h" // internal Core\inc  - IPC enumerations
#include "SenServiceConnection.h" 

// FORWARD DECLARE
class CSenSoapEnvelope;
class CSenChunk;
class RFile;
class RSenServiceConnection;
#ifdef __ENABLE_ALR__
class MMobilityProtocolResp;
#endif

class TFourInts
    {
    public:
    	TInt iInt1;
    	TInt iInt2;
    	TInt iInt3;
    	TInt iInt4;
	};

NONSHARABLE_CLASS(CSenProgressResourceHandler): private CActive
    {
    friend class RSenServiceConnection;
    private:
    CSenProgressResourceHandler(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
            TInt aProgress, RSenServiceConnection* aOwner);
    ~CSenProgressResourceHandler();
    void DoCancel();
    void RunL();
    TInt SetBuffer(const TDesC8& aSoapOrCid);
    
    private:
    TPckgBuf<TTransferProgress> iProgressData;
    HBufC8* iSoapOrCid;
    RSenServiceConnection* iOwner;
    CSenProgressResourceHandler* iNext;
    CSenProgressResourceHandler* iPrevious;
    };

NONSHARABLE_CLASS(CSenConnAgentSync): public CActive
    {
    friend class RSenServiceConnection;
private:
    CSenConnAgentSync();
    ~CSenConnAgentSync();
    void DoCancel();
    void RunL();
    void Start();
    };

// CLASS DECLARATION
class RSenServiceConnection : public RSessionBase
    {
public:
        /**
        * C++ default constructor.
        */
        RSenServiceConnection(RFileLogger* aLog);
        
        // New functions
        TInt Connect();
        TVersion Version() const;
        void Initialize( TRequestStatus& aStatus, 
        				 TDes8& aErrPtr, 
        				 TDes& aSessionId, 
        				 CSenChunk& aSenChunk,
                         TInt aSdLength,
                         TInt aCpLength,
                         TBool aHasAuthenticationCallback );


        /**
        * Transaction method is asynchronous
        * 
        * @param aStatus is the AO's status
        * @param aErrPtr is where error will be written by server
        * @param aTxnPtr is where transaction ID received from 
        * transport (in the server-side) will be written by server
        * @param aChunk is reference to new chunk, which will be opened
        * RChunk (S60 3rd Edition) or RFile (S60 2nd Edition)
        * @param aChunkName defines the global name for new RChunk (S60 2nd edition)
        * @param aRequest is the message copied into RChunk
        * @return KErrNone or system-wide error code. Note that if
        * an error is returned, then SendReceive has not been called
        * and caller (SC implementation class) does not need to call 
        * SetActive()
        */
        void Transaction(TRequestStatus& aStatus, 
                         TDes8& aErrPtr, 
                         TDes8& aTxnPtr, 
                         CSenChunk& aClientOp);

        TInt SendMsgAndReceiveTxnId(CSenChunk& aClientOp);

        TInt IsReady(TBool& aReady);
        TInt HasFacet(TDesC8& aUri, TBool& aHasFacet);
        TInt CompleteServerMessagesOnOff( TBool& aOnOff );
        void SetLog(RFileLogger* aLog);
        TInt RequestServiceDescription(TDes8& aSdLength);
        TInt ReceiveServiceDescription(TDes8& aServiceDescription);
        TInt StartTransaction();
        TInt TransactionCompleted();
        void CancelSession(TRequestStatus& aStatus);
        void CancelRequests(TRequestStatus& aStatus);
        void CancelRequest(TInt aTransactionID);
   	    void DataTrafficDetails(TSenDataTrafficDetails& aDetails,TSenDataTrafficOperations& aOperation);  
        TInt SetTransportPropertiesL(CSenChunk& aSenChunk);
        TInt TransportPropertiesL(CSenChunk& aSenChunk);        

        TInt MoveFile(CSenChunk& aSenChunk, 
                      const TDesC8& aCid, 
                      RFile& aFile);

        TInt SendFileHandle(CSenChunk& aSenChunk, 
                      		RFile& aFile);
        TInt MoveChunk(CSenChunk& aSenChunk, 
                       const TDesC8& aCid, 
                       RChunk& aChunk,
                       TInt aDataOffset, 
                       TInt aDataSize);
                       
        TInt ConnectionID();
        
        TInt IdentityProviderL(CSenChunk& aSenChunk);
        TInt RegisterIdentityProvider(TDesC8& aMessage);
        void SetChannel(TInt aChannel);
        
        TInt RegisterTransferObserver(const TDesC* aServerName, TAny* aConnection);
        // @return KErrNoMemory if out of heap
        TInt SendFileProgress( TInt aTxnId, TBool aIncoming,TBool aIsSoap,
                               const TDesC8& aSoapOrCid, TInt aProgress );
        void Remove(CSenProgressResourceHandler* aResourceHandler);
        void Close();
        
        TInt SearchIdentityProviderL(CSenChunk& aSenChunk, const TDesC8& aProviderId);
        TInt RegisterAuthenticationObserver(const TDesC* aServerName, TAny* aConnection);
        TInt RegisterCoBrandingObserver(const TDesC* aServerName, TAny* aConnection);
#ifdef __ENABLE_ALR__
	    TInt RegisterMobilityObserverL(const TDesC* aServerName, TAny* aConnection) ;
	    TInt MigrateToPrefferedCarrierL(TBool &aUserChoice) ;
	    TInt NewCarrierAcceptedL(TBool &aUserChoice) ;
#endif

    private: // Helper functions
        RFileLogger* Log() const;

    private: // Data
        TInt                iMessageContextId; // locally (in SC scope) unique message context ID counter
        RFileLogger*        iLog;
        TPckgBuf<TFourInts> iFourInts;
        CSenProgressResourceHandler* iFirst;
        TInt                iTLSLogChannel;
    };
    
#endif //R_SEN_SERVICE_CONNECTION_H

