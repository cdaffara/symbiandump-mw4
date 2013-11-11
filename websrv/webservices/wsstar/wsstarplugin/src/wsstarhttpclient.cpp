/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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











// INCLUDE FILES
#include "wsstarhttpclient.h"
#include "SenHttpTransportProperties.h"
#include "SenServiceConnection.h" // public error codes
#include "sendebug.h"
#include "senlogger.h"

//---------------------------------------------------------------------------
// Creates and returns a new instace of this class.
//---------------------------------------------------------------------------
//
CWSStarHttpClient* CWSStarHttpClient::NewL(
                                            CSIF& aSIF,
                                            CSenWSDescription* aInitializer,
                                            RFileLogger& aLogger)
    {
    CWSStarHttpClient* pNew = NewLC(aSIF, aInitializer, aLogger);
    CleanupStack::Pop();
    return pNew;
    }

//---------------------------------------------------------------------------
// Creates and returns a new instace of this class and leaves the object on the cleanup stack
//---------------------------------------------------------------------------
//
CWSStarHttpClient* CWSStarHttpClient::NewLC(
                                                        CSIF& aSIF,
                                                        CSenWSDescription* aInitializer,
                                                        RFileLogger& aLogger)
    {
    CWSStarHttpClient* pNew =
        new (ELeave) CWSStarHttpClient(
                    aSIF,
                    MSenServiceDescription::EWSStarPolicyClient,
                    aInitializer,
                    aLogger);

    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//
CWSStarHttpClient::CWSStarHttpClient( CSIF& aSIF,
                                      TDescriptionClassType aType,
                                      CSenWSDescription* aInitializer,
                                      RFileLogger& aLogger )
    : CSenInternalServiceConsumer(aType, aSIF),
    iInitializer(aInitializer),
    iLog(aLogger)
    {
    }

void CWSStarHttpClient::BaseConstructL()
    {
    CSenInternalServiceConsumer::ConstructL();
    iPolicySession = CWSStarServiceSession::NewL(Framework());
    iPolicySession->AddConsumerL(*this);
    iPolicySession->SetEndPointL(iInitializer->Endpoint());
    iPolicySession->InitializeFromL(*iInitializer);
    ipTransport = Framework().Manager().CreateL(*iPolicySession, NULL);        
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarHttpClient::ConstructL");
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSStarHttpClient::~CWSStarHttpClient()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarHttpClient::~CWSStarHttpClient");
    if( iPolicySession )
	    {
        TRAP_IGNORE( iPolicySession->RemoveConsumerL(*this); )
    	}
    delete ipTransport;
    delete iPolicySession;
    }


//---------------------------------------------------------------------------
// The most siginificant method of this class. 
//---------------------------------------------------------------------------
//
TInt CWSStarHttpClient::GetHttpL( const TDesC8& aRequest,
                                  HBufC8*& apResponse,
                                  CSenWSDescription* apSD )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarHttpClient::GetHttpL");
    TInt retVal(KErrNone);             // for returning error codes
    if( !iInitializer )
        {
        return KErrSenInternal;
        }
    TPtrC8 endpoint(iInitializer->Endpoint());    

    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();
    pHttpProperties->SetHttpMethodL(CSenHttpTransportProperties::ESenHttpGet);
    _LIT8(KAcceptedHeader, "text/xml,text/plain");    
    pHttpProperties->SetAcceptL(KAcceptedHeader);
    TUint32 desiredIapId(0);
   
    if( apSD )
        {
        TInt getIapRetVal = apSD->IapId( desiredIapId );
        if ( getIapRetVal == KErrNone )
            {
            pHttpProperties->SetIapIdL( desiredIapId );
            }
        }
     //limit http timeout, 
	pHttpProperties->SetMaxTimeToLiveL(WSStarSession::KMaxHttpTimeOut);
	TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarHttpClient::GetHttpL - with IAP ID (%d):"), desiredIapId));    
    HBufC8* pSerializedProperties  = pHttpProperties->AsUtf8L();
    CleanupStack::PopAndDestroy(pHttpProperties);

    CleanupStack::PushL(pSerializedProperties);
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,( pSerializedProperties->Des() ));
    retVal = ipTransport->SubmitL(endpoint, aRequest, *pSerializedProperties, apResponse, *this);	
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarHttpClient::GetHttpL - SubmitL returned: %d"), retVal));    

    CleanupStack::PopAndDestroy( pSerializedProperties );

#ifdef _SENDEBUG
    if( apResponse )
        {
        TPtrC8 response = apResponse->Des();
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"* * * * * * * * * * * * *");
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("CWSStarHttpClient::GetHttpL - response (%d bytes):"), response.Length()));    
        TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,( response ));
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenCoreServiceManager::CreateL:");
        }
#endif // _SENDEBUG        

    if( retVal != KErrNone )
        {
        delete apResponse; 
        apResponse = NULL;
        }
    return retVal;
    }

//---------------------------------------------------------------------------
// SetStatus
//---------------------------------------------------------------------------
//

void CWSStarHttpClient::SetStatus(TInt /*aStatus */)
    {
    }

//---------------------------------------------------------------------------
// ServiceSession
//---------------------------------------------------------------------------
//
CSenServiceSession* CWSStarHttpClient::ServiceSession()
    {
    return iPolicySession;
    }

//---------------------------------------------------------------------------
// Obtain Transport using a factory from Core Manager
//---------------------------------------------------------------------------
//
MSenTransport& CWSStarHttpClient::TransportL()
    {
    if(!ipTransport)
        {
        // Core is capable of working as transport factory, too.
        // The returned transport instancies may or may not be
        // ECOM plugins:
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarHttpClient::Instantiating new transport");
        ipTransport = Framework().Manager().CreateL(*iPolicySession, iPolicySession); 
        }
    return *ipTransport;
    }
void CWSStarHttpClient::SetClientSessionL(
                                                CWSStarServiceSession* aSession)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarHttpClient::SetServiceSessionL");
    // free memory
    //delete iPolicySession;
    iPolicySession = aSession;
    if(iPolicySession)
        {
        iPolicySession->AddConsumerL(*this);
        }
    }

//---------------------------------------------------------------------------
// Getter for endpoint
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarHttpClient::Endpoint()
    {
    return iPolicySession->Endpoint();
    }
//---------------------------------------------------------------------------
// Getter for contract
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarHttpClient::Contract()
    {
    return iPolicySession->Contract();
    }

//---------------------------------------------------------------------------
// serialie to xml
//---------------------------------------------------------------------------
//
void CWSStarHttpClient::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    iPolicySession->WriteAsXMLToL(aWriteStream);
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//
void CWSStarHttpClient::SetVersion(const TDesC8* aVersion)
    {
    iVersion = aVersion;
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//    
TBool CWSStarHttpClient::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::ESenInternalServiceConsumer ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CSenInternalServiceConsumer::HasSuperClass( aType ); 
        }
    }
void CWSStarHttpClient::SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) 		
	{
		iHttpDetails = aDetails;
	}
void CWSStarHttpClient::DataTrafficDetails( TSenDataTrafficDetails& aDetails) 			
	{
		aDetails = iHttpDetails;
	}

// End of file


