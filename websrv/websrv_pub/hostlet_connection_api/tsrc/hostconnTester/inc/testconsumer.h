/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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






#ifndef __TESTCONSUMER_H__
#define __TESTCONSUMER_H__

//  EXTERNAL INCLUDES
#include <SenServiceConnection.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
class ChostconnTester;

class CTestConsumer : public CBase, public MSenServiceConsumer
    {
    public:

        /**
         * Two phase construction
         */
        static CTestConsumer* NewL(ChostconnTester* aTestSuite,
                                   void (*ap2HandleMessageL) (ChostconnTester*, const TDesC8&) = NULL,
                                   void (*ap2HandleErrorL) (ChostconnTester*, const TInt, const TDesC8&) = NULL,
                                   void (*ap2SetStatus) (ChostconnTester*, const TInt) = NULL);
        static CTestConsumer* NewLC(ChostconnTester* aTestSuite,
                                   void (*ap2HandleMessageL) (ChostconnTester*, const TDesC8&) = NULL,
                                   void (*ap2HandleErrorL) (ChostconnTester*, const TInt, const TDesC8&) = NULL,
                                   void (*ap2SetStatus) (ChostconnTester*, const TInt) = NULL);
        /**
         * Destructor
         */
        ~CTestConsumer();

        virtual void HandleMessageL(const TDesC8& aMessage);

        virtual void HandleErrorL(const TInt aErrorCode,
                                  const TDesC8& aMessage);

        virtual void SetStatus(const TInt aStatus);

    public: // Public data => OK in testers, no benefit gained from getters => in real apps, these should be private:
        
        RPointerArray<HBufC8>		iResponses; // ok/error => check iErrorCode to determine which
        RPointerArray<CSenTransportProperties> iResponseProperties;
        RArray<TInt> 				iResponseCodes;// either error code (if handle error) OR KErrNone (if handle message) 
        TInt 						iConnectionStatus;
        TInt 						iTxnId;
        TInt                        iErrorCode;


    private:    // Constructor
        CTestConsumer(ChostconnTester* aTestSuite,
                      void (*ap2HandleMessageL) (ChostconnTester*, const TDesC8&) = NULL,
                      void (*ap2HandleErrorL) (ChostconnTester*, const TInt, const TDesC8&) = NULL,
                      void (*ap2SetStatus) (ChostconnTester*, const TInt) = NULL);

    private:    // Data
        ChostconnTester*       ipTestSuite;
        
    private:    // Function pointers
        void (*ip2HandleMessageL) (ChostconnTester*, const TDesC8&);
        void (*ip2HandleErrorL) (ChostconnTester*, const TInt, const TDesC8&);
        void (*ip2SetStatus) (ChostconnTester*, const TInt);
    };

#endif      //  __TESTCONSUMER_H__

// End of file
