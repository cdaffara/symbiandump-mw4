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
* Description:      
*
*/






//  CLASS HEADER
#include "TestConsumer.h"

//  INTERNAL INCLUDES
#include "hostconnTester.h"

#include "SenTransportProperties.h"

//namespace
//    {
//    _LIT8(KHostletEndpoint, "hostlet://UnitTestHostlet");
//    _LIT8(KHostletContract, "urn:nokia.com.serene:unit-test-hostlet");
//    }


CTestConsumer* CTestConsumer::NewL(ChostconnTester* aTestSuite,
                                   void (*p2HandleMessageL) (ChostconnTester*, const TDesC8&),
                                   void (*p2HandleErrorL) (ChostconnTester*, const TInt, const TDesC8&),
                                   void (*p2SetStatus) (ChostconnTester*, const TInt))
    {
    CTestConsumer* pSelf = CTestConsumer::NewLC(aTestSuite, p2HandleMessageL,
                                                p2HandleErrorL, p2SetStatus);
    CleanupStack::Pop(pSelf);

    return pSelf;
    }

CTestConsumer* CTestConsumer::NewLC(ChostconnTester* aTestSuite,
                                    void (*p2HandleMessageL) (ChostconnTester*, const TDesC8&),
                                    void (*p2HandleErrorL) (ChostconnTester*, const TInt, const TDesC8&),
                                    void (*p2SetStatus) (ChostconnTester*, const TInt))
    {
    CTestConsumer* pSelf = new (ELeave) CTestConsumer(aTestSuite, p2HandleMessageL,
                                                      p2HandleErrorL, p2SetStatus);
    CleanupStack::PushL(pSelf);

    return pSelf;
    }

CTestConsumer::CTestConsumer(ChostconnTester* aTestSuite,
                             void (*p2HandleMessageL) (ChostconnTester*, const TDesC8&),
                             void (*p2HandleErrorL) (ChostconnTester*, const TInt, const TDesC8&),
                             void (*p2SetStatus) (ChostconnTester*, const TInt))
    : ipTestSuite(aTestSuite),
      ip2HandleMessageL(p2HandleMessageL),
      ip2HandleErrorL(p2HandleErrorL),
      ip2SetStatus(p2SetStatus)
    {

    }

CTestConsumer::~CTestConsumer()
    {
    iResponses.ResetAndDestroy();
    iResponseProperties.ResetAndDestroy();
    iResponseCodes.Reset();
    }

void CTestConsumer::HandleMessageL(const TDesC8& aMessage)
    {
    iErrorCode = KErrNone;
    iTxnId = ipTestSuite->ipConnection->TxnId();

    MSenMessage* pResponse = ipTestSuite->ipConnection->Response();
    if( pResponse )
        {
        MSenProperties* pProperties = pResponse->Properties();
        if( pProperties )
            {
            CSenTransportProperties* pTransportProperties = NULL;
            if( pProperties->IsSafeToCast(MSenProperties::ESenTransportProperties))
                {
                pTransportProperties = (CSenTransportProperties*)pProperties;
                
                CSenTransportProperties* pClone = (CSenTransportProperties*)pTransportProperties->CloneL();
                if( pClone )
                    {
                    CleanupStack::PushL(pClone);
                    TInt appendError = iResponseProperties.Append(pClone);
                    if( appendError )
                        {
                        CleanupStack::PopAndDestroy(pClone);
                        }
                    else
                        {
                        // OK, pClone is now owned by ipResponseProperties
                        CleanupStack::Pop(pClone);
                        }
                    }
                }
            }
        }


    iResponseCodes.Append(KErrNone);
    iResponses.Append(aMessage.AllocL());
    if ( ip2HandleMessageL )
        {
        (*ip2HandleMessageL)(ipTestSuite, aMessage);        
        }
    ipTestSuite->StopActiveScheduler();
    }

void CTestConsumer::HandleErrorL(const TInt aErrorCode, const TDesC8& aMessage)
    {
    iErrorCode = aErrorCode;
    iTxnId = ipTestSuite->ipConnection->TxnId();

    MSenMessage* pResponse = ipTestSuite->ipConnection->Response();
    if( pResponse )
        {
        MSenProperties* pProperties = pResponse->Properties();
        if( pProperties )
            {
            CSenTransportProperties* pTransportProperties = NULL;
            if( pProperties->IsSafeToCast(MSenProperties::ESenTransportProperties))
                {
                pTransportProperties = (CSenTransportProperties*)pProperties;
                
                CSenTransportProperties* pClone = (CSenTransportProperties*)pTransportProperties->CloneL();
                if( pClone )
                    {
                    CleanupStack::PushL(pClone);
                    TInt appendError = iResponseProperties.Append(pClone);
                    if( appendError )
                        {
                        CleanupStack::PopAndDestroy(pClone);
                        }
                    else
                        {
                        // OK, pClone is now owned by ipResponseProperties
                        CleanupStack::Pop(pClone);
                        }
                    }
                }
            }
        }

    iResponseCodes.Append(aErrorCode);
    HBufC8* pMessage = aMessage.AllocL();
    CleanupStack::PushL(pMessage);
    TInt appendError2 =  iResponses.Append(pMessage);
    if( appendError2 )
        {
        CleanupStack::PopAndDestroy(pMessage);
        }
    else
        {
        CleanupStack::Pop(pMessage);
        }        

    if ( ip2HandleErrorL )
        {
        (*ip2HandleErrorL)(ipTestSuite, aErrorCode, aMessage);
        }
    ipTestSuite->StopActiveScheduler();
    }

void CTestConsumer::SetStatus(const TInt aStatus)
    {
    iConnectionStatus = aStatus;

    
    if ( ip2SetStatus )
        {
        (*ip2SetStatus)(ipTestSuite, aStatus); // func call via ptr
        }
    else
        {
        // Active Scheduler was NOT stopped by static function (func call via ptr above)
        switch( aStatus )
            {
            case KSenConnectionStatusReady:
                {
                ipTestSuite->StopActiveScheduler(); // otherwise failed SC construction is reported via HandleErrorL(!)
                }
                break;
            case KSenConnectionStatusNew:
            case KSenConnectionStatusExpired:
            default:            
                break;
            }
        
        }
    }

//  END OF FILE
