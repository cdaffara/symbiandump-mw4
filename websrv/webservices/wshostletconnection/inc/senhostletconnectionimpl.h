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
* Description:           Custom plug-in implementations offer their own API documentation
*                where detailed characteristics about how methods declared in this
*                class are implemented.
*
*/











#ifndef SEN_HOSTLET_CONNECTION_IMPL_H
#define SEN_HOSTLET_CONNECTION_IMPL_H

// INCLUDES
#include <e32property.h>
#include <flogger.h>

#include "SenHostletConnection.h"   // public 

#include "rsenhostletconnection.h"
#include "senpointermap.h"
#include "senlogger.h"
#include "sendebug.h"

#include "senregistrationtimer.h"

// FORWARD DECLARATIONS
class MSenHostlet;
class MSenHostletRequest;
class MSenHostletResponse;
//class MSenServiceDescription;
class CSenChunk;
class CSenAsyncOperation;
class CSenHostletTransaction;
class CSenIdentifier;
class CSenHostletRequest;
class CSenHostletResponse;
class CSenFileProgressObserver;

// TYPEDEFS
typedef RSenPointerMap<TInt, CSenHostletTransaction> RHostletTransactionMap;


// CLASS DECLARATION

/**
* Custom plug-in implementations offer their own API documentation
* where detailed characteristics about how methods declared in this
* class are implemented.
*/
class CSenHostletConnectionImpl : public CSenHostletConnection, MSenRegistrationActor
    {
        friend class CSenAsyncOperation;
    public: 

        // Constructors and destructor:
        static CSenHostletConnectionImpl* NewL(MSenHostlet& aProvider);

        static CSenHostletConnectionImpl* NewLC(MSenHostlet& aProvider);

        virtual ~CSenHostletConnectionImpl();

        // New functions:
        virtual TInt RespondL(MSenHostletResponse& aResponse);
        
        virtual TInt Identifier();

        
    protected: // Functions from base classes 
        
        // from CActive
        void RunL();
        void DoCancel();

    private:
        
        /**
        * C++ default constructor.
        */
        CSenHostletConnectionImpl(MSenHostlet& aProvider);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // New functions
        TInt EstablishConnectionL(MSenHostlet& aProvider);
        TInt RegisterAndSubscribeFileProgressObserversL();

        // Utility functions:
        RHostletTransactionMap& HostletTransactionMapL();
        TPtrC SessionID();
        RPointerArray<CSenAsyncOperation>& AsyncOpsArrayL();
        void HandleErrorFromChildAOL(TInt aError, TInt aTxnId, const TDesC8& aDesc, CSenAsyncOperation& aChild);
        void HandleMessageFromChildAOL(TInt aStatus, CSenAsyncOperation& aChild);

        TInt RegisterFilesObserver();
    private: // Data
        RSenHostletConnection iConnection;  // owned
        MSenHostlet& iProvider;             // not owned

        TInt    iResponseLength;
        TInt    iErrorNumber;
        TPtr8   iErrorNumberBuffer;
        TInt    iTxnId;
        TPtr8   iTxnIdBuffer;

        RHostletTransactionMap* iHostletTransactionMap;

        HBufC* ipSessionId;
        TPtr iSessionIdBuffer;
        TInt iChunkNameNumber;
        HBufC* ipChunkName;
        TBool iAlive;
        
        TBool iTransactionIdKnown;
        TUint iAsyncOpsCount;
        RPointerArray<CSenAsyncOperation>* iAsyncOpsArray;
        TInt iConnectionID;
        MSenFilesObserver*                  iFilesObserver;
        TInt iTlsLogChannel;
        TAny*                               ipInterface;
        TBool 								iCancelSession;
        CSenFileProgressObserver*			ipFileProgressObserver; // owned
        TBool iRegisterFileObserverDone;
        CSenRegistrationTimer*              ipRegistrationTimer;
    };

#ifdef EKA2
NONSHARABLE_CLASS(CSenAsyncOperation) : public CActive
#else
class CSenAsyncOperation : public CActive 
#endif
    {
    public: // Constructors and destructor
        static CSenAsyncOperation* NewL(CSenHostletConnectionImpl* aActive);
        static CSenAsyncOperation* NewLC(CSenHostletConnectionImpl* aActive);
        virtual ~CSenAsyncOperation();

    protected: // Functions from base classes 
        // from CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
        
    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        CSenAsyncOperation(CSenHostletConnectionImpl* iActive);

    public: // Data
        CSenHostletConnectionImpl* iActive;
        TInt    iErrorNumber;
        TPtr8   iErrorNumberBuffer;
        TInt    iTxnId;
        TPtr8   iTxnIdBuffer;
    };    
    
#ifdef EKA2
NONSHARABLE_CLASS(CSenHostletTransaction) : public CBase
#else
class CSenHostletTransaction : public CBase 
#endif
    {
    public: // Constructors and destructor
        static CSenHostletTransaction* NewL(CSenChunk* aSenChunk);
        static CSenHostletTransaction* NewLC(CSenChunk* aSenChunk);
        virtual ~CSenHostletTransaction();
        
        void SetChunk(CSenChunk* aChunk);
        CSenChunk* Chunk();

        void SetIdentifier(CSenIdentifier* aIdentifier);
        CSenIdentifier* Identifier();

        void SetRequest(CSenHostletRequest* aRequest);
        CSenHostletRequest* Request();

        void SetResponse(CSenHostletResponse* aResponse);
        CSenHostletResponse* Response();

    private:
        CSenHostletTransaction(CSenChunk* aSenChunk);

    private: // Data
        CSenChunk*              iSenChunk;
        CSenIdentifier*         iIdentifier;
        CSenHostletRequest*     iRequest;
        CSenHostletResponse*    iResponse;
    };    
    
#ifdef EKA2
NONSHARABLE_CLASS(CSenFileProgressObserver) : public CActive
#else
class CSenFileProgressObserver : public CActive
#endif
    {
    public:
        static CSenFileProgressObserver* NewL( MSenFilesObserver& aObserver,
                                               TInt aConnectionID );

        static CSenFileProgressObserver* NewLC( MSenFilesObserver& aObserver,
                                                TInt aConnectionID );
        ~CSenFileProgressObserver();

    private:
        CSenFileProgressObserver( MSenFilesObserver& aObserver,
                                  TInt aConnectionID );
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
    };    

     
    

#endif //SEN_HOSTLET_CONNECTION_IMPL_H

// End of File
