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






#ifndef __TESTHOSTLET_H__
#define __TESTHOSTLET_H__

//  EXTERNAL INCLUDES
#include "hostconnTester.h"
#include <MSenProperties.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
class CHostletConnectionBCTest;

class CTestHostlet : public CBase, public MSenHostlet
    {
    public:

        /**
         * Two phase construction
         */
        static CTestHostlet* NewL(ChostconnTester* aTestSuite,
                                  TInt (*ap2ServiceL) (ChostconnTester*, MSenHostletRequest&, MSenHostletResponse&) = NULL,
                                  TInt (*ap2OnServiceCompleteL) (ChostconnTester*, const TInt, TInt, const TDesC8&) = NULL,
                                  void (*ap2DescribeServiceL) (CSenXmlServiceDescription&) = NULL);

        static CTestHostlet* NewLC(ChostconnTester* aTestSuite,
                                   TInt (*ap2ServiceL) (ChostconnTester*, MSenHostletRequest&, MSenHostletResponse&) = NULL,
                                   TInt (*ap2OnServiceCompleteL) (ChostconnTester*, const TInt, TInt, const TDesC8&) = NULL,
                                   void (*ap2DescribeServiceL) (CSenXmlServiceDescription&) = NULL);

        /**
         * Destructor
         */
        ~CTestHostlet();
        virtual void DescribeServiceL(CSenXmlServiceDescription& aSD);

        virtual TPtrC8 Contract() const;

        virtual TPtrC8 Endpoint() const;

        virtual TInt ServiceL(MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
        virtual void OnServiceCompleteL(const TInt aTxnId, TInt aCompletionCode, const TDesC8& aDesc);

    public: // Public data => OK in testers, no benefit gained from getters => in real apps, these should be private:
        CSenXmlServiceDescription*  ipHostletDescription;
        RPointerArray<HBufC8>		iRequests;
        RPointerArray<HBufC8>		iResponses;
        RPointerArray<HBufC8>       iRequestProps;
        RArray<MSenProperties::TSenPropertiesClassType> iRequestPropTypes;
        RPointerArray<CSenTransportProperties> iResponseProps;
        RArray<TInt>				iResponseCodes;
        TInt                        iRespondRetCode; // direct retCode from RespondL (async)
        RArray<TInt>                iCompletionCodes; // from OnServiceCompleteL => indicates how Respond operation was delivered
        RArray<TInt>				iTxnIds;
    //MSenHostlet
    
    		inline virtual TPtrC8 FrameworkId() const 
    		{ 
    			return KDefaultRestServicesFrameworkID(); 
    		}
    //MSenHostletRequest
            inline virtual TPtrC8 RequestUtf8() const 
            {
            	return _L8("");
            }
            inline virtual TPtrC ThreadId() const 
            {
            	return _L("");
            }
            inline virtual TPtrC8 ConsumerId() const 
            {
            	return _L8("");
            }
			inline virtual TInt RequestId() const 
			{
				return KErrNone;
			}
			inline virtual TPtrC8 Properties(MSenProperties::TSenPropertiesClassType& aType) const 
			{
				return _L8("");
			}
    private:    // Constructor
        CTestHostlet(ChostconnTester* aTestSuite,
                     TInt (*ap2ServiceL) (ChostconnTester*, MSenHostletRequest&, MSenHostletResponse&),
                     TInt (*ap2OnServiceCompleteL) (ChostconnTester*, const TInt, TInt, const TDesC8&),
                     void (*ap2DescribeServiceL) (CSenXmlServiceDescription&));
        void ConstructL();

    private:    // Data
        ChostconnTester*       ipTestSuite;
        
    private:    // Function pointers
        TInt (*ip2ServiceL) (ChostconnTester*, MSenHostletRequest&, MSenHostletResponse&);
        TInt (*ip2OnServiceCompleteL) (ChostconnTester*, const TInt, TInt, const TDesC8&);
        void (*ip2DescribeServiceL)(CSenXmlServiceDescription&);
    };

#endif      //  __TESTHOSTLET_H__

// End of file
