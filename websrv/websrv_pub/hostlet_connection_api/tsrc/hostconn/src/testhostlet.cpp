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
#include "testhostlet.h"

//  EXTERNAL INCLUDES
#include <StifTestInterface.h>

#include <SenTransportProperties.h>

//  INTERNAL INCLUDES
#include "senhostletconnectionbctest.h"

namespace
    {
    //_LIT8(KHostletEndpoint, "hostlet://UnitTestHostlet");
    _LIT8(KHostletContract, "urn:nokia.com.serene:unit-test-hostlet");
    }

CTestHostlet* CTestHostlet::NewL(CSenHostletConnectionBCTest* aTestSuite,
                                 TInt (*ap2ServiceL) (CSenHostletConnectionBCTest*, MSenHostletRequest&, MSenHostletResponse&),
                                 TInt (*ap2OnServiceCompleteL) (CSenHostletConnectionBCTest*, const TInt, TInt, const TDesC8&),
                                 void (*ap2DescribeServiceL)(CSenXmlServiceDescription&))
    {
    CTestHostlet* pSelf = CTestHostlet::NewLC(aTestSuite, ap2ServiceL, ap2OnServiceCompleteL, ap2DescribeServiceL);
    CleanupStack::Pop(pSelf);
    return pSelf;
    }

CTestHostlet* CTestHostlet::NewLC(CSenHostletConnectionBCTest* aTestSuite,
                                  TInt (*ap2ServiceL) (CSenHostletConnectionBCTest*, MSenHostletRequest&, MSenHostletResponse&),
                                  TInt (*ap2OnServiceCompleteL) (CSenHostletConnectionBCTest*, const TInt, TInt, const TDesC8&),
                                  void (*ap2DescribeServiceL)(CSenXmlServiceDescription&))
    {
    CTestHostlet* pSelf = new (ELeave) CTestHostlet(aTestSuite, ap2ServiceL, ap2OnServiceCompleteL, ap2DescribeServiceL);
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
	    
    return pSelf;
    }

CTestHostlet::CTestHostlet(CSenHostletConnectionBCTest* aTestSuite,
                           TInt (*ap2ServiceL) (CSenHostletConnectionBCTest*, MSenHostletRequest&, MSenHostletResponse&),
                           TInt (*ap2OnServiceCompleteL) (CSenHostletConnectionBCTest*, const TInt, TInt, const TDesC8&),
                           void (*ap2DescribeServiceL)(CSenXmlServiceDescription&))
    : ipTestSuite(aTestSuite),
      ip2ServiceL(ap2ServiceL),
      ip2OnServiceCompleteL(ap2OnServiceCompleteL),
      ip2DescribeServiceL(ap2DescribeServiceL)
    {
    }

CTestHostlet::~CTestHostlet()
    {
    delete ipHostletDescription;
    iRequests.ResetAndDestroy();
    iResponses.ResetAndDestroy();
    iRequestProps.ResetAndDestroy();
    iRequestPropTypes.Reset();
    iResponseProps.ResetAndDestroy();
    iResponseCodes.Reset();
    iTxnIds.Reset();
    iCompletionCodes.Reset();
    }

void CTestHostlet::ConstructL()
    {
    ipHostletDescription = CSenXmlServiceDescription::NewL(KNullDesC8, KHostletContract);
	//ipHostletDescription = CSenXmlServiceDescription::NewL(KHostletEndpoint, KNullDesC8);    	
    ipHostletDescription->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    }
    

void CTestHostlet::DescribeServiceL(CSenXmlServiceDescription& aSD) 
    { 
    if( ip2DescribeServiceL ) 
    	{
   		(*ip2DescribeServiceL)(aSD);
    	}
        else
            {
            aSD.SetEndPointL(Endpoint());
            aSD.SetContractL(Contract()); 
            aSD.SetFrameworkIdL(FrameworkId());	
            }
    }

TPtrC8 CTestHostlet::Contract() const
    {
    return ipHostletDescription->Contract();
    }

TPtrC8 CTestHostlet::Endpoint() const
    {
    return ipHostletDescription->Endpoint();
    }

TInt CTestHostlet::ServiceL(MSenHostletRequest& aRequest, MSenHostletResponse& aResponse)
    {
    

    iTxnIds.Append(aRequest.RequestId());
    iResponseCodes.Append(KErrNone);
    

    	
    TInt retVal(KErrNone);
    // Setup default response, can be overridden by test case:
    // Dummy response
    TPtrC8 request = aRequest.RequestUtf8();


    iRequests.Append(request.AllocL());
    
    MSenProperties::TSenPropertiesClassType propType;
    TPtrC8 requestProperties = aRequest.Properties(propType);
    iRequestProps.Append(requestProperties.AllocL());
    iRequestPropTypes.Append(propType);
    
//    _LIT8(KResponseFmt, "<Response><ReplyMessage>CSenHostletConnectionBCTest is alive.</ReplyMessage><Request>%S</Request></Response>");
  	_LIT8(KResponseFmt, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header></S:Header><S:Body>testing text hostlet</S:Body></S:Envelope>");  
    HBufC8* pResponse = HBufC8::NewLC(request.Length()+KResponseFmt().Length());
    TInt appendError = iResponses.Append(pResponse);
    if( appendError )
        {
        CleanupStack::PopAndDestroy(pResponse); // delete orphan HBufC8 / pResponse
        }
    else
        {
        CleanupStack::Pop(pResponse); // now owned by iResponses -array..
        }        
    
    TPtr8 response = iResponses[iTxnIds.Count()-1]->Des();
    response.Format(KResponseFmt, &request);
    aResponse.SetResponseUtf8L(response, iResponseCodes[iTxnIds.Count()-1]); // by default, all requests are "accepted"
      
    if ( ip2ServiceL )
        {
        // NOTE(!): if RESPONSE CODE in aResponse is modified in YOUR test case,
        // remember set iResponseCodes for this test hostlet instance in your test
        //  case code (ServiceL callback)
        retVal = (*ip2ServiceL)(ipTestSuite, aRequest, aResponse); 
        }
    TPtrC8 providedResponse = aResponse.ResponseUtf8();
    
    if ( iResponses[iTxnIds.Count()-1]->Compare(providedResponse) != 0 )
        {
        // response was altered by TEST CASE:
        iResponses.Insert(aResponse.ResponseUtf8().AllocL(),iTxnIds.Count()-1);
        }
        
        
    RDebug::Print(_L("CTestHostlet::ServiceL received:"));
    RDebug::RawPrint(request);
    if (ipTestSuite->ipUsedHostletConnection)
    {
    	iRespondRetCode = ipTestSuite->ipUsedHostletConnection->RespondL(aResponse); 
    }
    else
		iRespondRetCode = ipTestSuite->ipHostletConnection->RespondL(aResponse); 
   return retVal;
    }

void CTestHostlet::OnServiceCompleteL(const TInt aTxnId, 
                                      TInt aCompletionCode, 
                                      const TDesC8& aDesc)
    {
  
    iCompletionCodes.Append(aCompletionCode); 
    if( ip2OnServiceCompleteL )
        {
        (*ip2OnServiceCompleteL)( ipTestSuite, aTxnId, aCompletionCode, aDesc );
        }
    ipTestSuite->StopActiveScheduler(); // service provided
    }
    
//  END OF FILE
