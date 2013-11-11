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
* Description:           
*
*/












// INCLUDE FILES
#include "senrestservicesession.h"
#include "senservicesession.h"
#include "SenSoapMessage.h"
#include "senwsdescription.h"
#include "msencoreservicemanager.h"
#include "msenremoteserviceconsumer.h"
#include "SenXmlUtils.h"
#include "SenDateUtils.h"
#include "msentransport.h"
#include "SenServiceConnection.h"
#include "sendebug.h"
#include "senlogger.h"
// #include "SenHttpTransportProperties.h"

CSenRestServiceSession* CSenRestServiceSession::NewL(MSIF& aFramework)
    {
    CSenRestServiceSession* pNew =
        CSenRestServiceSession::NewLC(aFramework);
    CleanupStack::Pop();
    return pNew;
    }

CSenRestServiceSession* CSenRestServiceSession::NewLC(MSIF& aFramework)
    {
    CSenRestServiceSession* pNew =
        new (ELeave) CSenRestServiceSession(ERestServiceSession,
                                                aFramework);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenRestServiceSession::CSenRestServiceSession(TDescriptionClassType aType, MSIF& aFramework)
: CSenWebServiceSession(aType, aFramework)
    {
    }

void CSenRestServiceSession::ConstructL()
    {
    // sets the local name to "ServiceDescription" and
    // initiates the inner ipElement
    CSenWebServiceSession::ConstructL();
    }

CSenRestServiceSession::~CSenRestServiceSession()
    {
    }

// Override compute status to make REST sessions to be ALWAYS valid (ready)
TInt CSenRestServiceSession::ComputeStatusL()
    {
    // session is ready, if an endpoint is defined
    return CSenServiceSession::ComputeStatusL();
    }

TInt CSenRestServiceSession::SendSoapL( const TDesC8& aSoapMessage, 
                                        const TDesC8& aTransportProperties,
                                        MSenRemoteServiceConsumer& aConsumer,
                                        TInt& aTxnId,
                                        HBufC8*& /*aRevalidationError*/ ) 
    {
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenRestServiceSession::SendSoapL");
    TPtrC8 endpoint = Endpoint();
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"///////////////////////////////////////////////////////");
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KMinLogLevel, _L8("- Endpoint: %S"), &endpoint));
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KMaxLogLevel, _L8("- Request (%d bytes):"), aSoapMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,(aSoapMessage));
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"///////////////////////////////////////////////////////");
#endif



    TInt retVal( aConsumer.TransportL().SendL( endpoint, 
                                               aSoapMessage,
                                               aTransportProperties, 
                                               *this, 
                                               aConsumer, 
                                               aTxnId) );
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KNormalLogLevel, _L8("- Transaction ID: %d"), aTxnId));
    return retVal;
    }

TInt CSenRestServiceSession::SubmitSoapL(const TDesC8& aSoapMessage, 
                                         const TDesC8& aTransportProperties,
                                         MSenRemoteServiceConsumer& aConsumer,
                                         HBufC8*& aResponse)
    {
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenRestServiceSession::SubmitSoapL");
    TPtrC8 endpoint = Endpoint();
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"///////////////////////////////////////////////////////");
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KMinLogLevel, _L8("- Endpoint: %S"), &endpoint));
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KMaxLogLevel, _L8("- Request (%d bytes):"), aSoapMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,(aSoapMessage));
   	CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"///////////////////////////////////////////////////////");
#endif

    MSenTransport& transport = aConsumer.TransportL();
    SetFrameworkPropertiesL(transport);
    TInt retVal = transport.SubmitL( endpoint,
                                     aSoapMessage, 
                                     aTransportProperties, 
                                     aResponse,
									 aConsumer);
    if ( !aResponse )
        {
        // response was NULL: probably either out
        // of heap or some transport malfunction.
        return retVal;
        }
    else if ( aResponse->Length() < KSenSoapEnvelopeName().Length()*2 )
        {
        // no use parsing, Envelope -root element not there
        // deliver non-soap body to consumer
        return retVal;
        }

    CleanupStack::PushL(aResponse);

    CSenSoapMessage* pResponseSoapMsg = NULL;
    TInt leaveCode(KErrNone);

    // attempt to parse the response
    // here. SOAP faults are to be
    // searched after,

    TInt parseRetCode(KErrNone);
    TRAP( leaveCode, (parseRetCode =
        ParseResponseL(*aResponse, pResponseSoapMsg)) );

    if(leaveCode!=KErrNone)
        {
        // THE RESPONSE could not be parsed
        delete pResponseSoapMsg;

        // THE RESPONSE IS NOT SOAP
        if(retVal==KErrNone)
            {
            // indicate with return value, that response is
            // invalid - even though submit was ok, the
            // response could NOT be parsed!
            retVal = leaveCode;
            }
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"(NOK) Response is not a SOAP message");
        }
    else
        {

        if (parseRetCode != KErrNone)
            {
            CSLOG_FORMAT((aConsumer.ConnectionId()   , KMinLogLevel, _L8("(NOK) Parsing SOAP msg failed (%d)"),
                    parseRetCode));

            if(retVal==KErrNone) // submit was ok
                {
                // indicate with return value, that response is
                // invalid - even though submit was ok, the
                // response could NOT be parsed!
                retVal = parseRetCode;
                }

            // THE RESPONSE could not be parsed
            delete pResponseSoapMsg;
            }
        else
            {
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"REST: (OK) Response is a SOAP message");
            CleanupStack::PushL(pResponseSoapMsg);

            TBool completeServerMessages(ETrue);
            HasFacetL(KCompleteMessagesFacet, completeServerMessages);

            // response is OK and in SOAP form.
			if(pResponseSoapMsg)
			   {
			   if (pResponseSoapMsg->IsFault())
                   {
                	// response is SOAP fault
                	CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"REST: Response is a SOAP fault.");
                	retVal = KErrSenSoapFault;

                if (!completeServerMessages)
                    {
                    // try to detach the SOAP fault
                    HBufC8* pDetached = NULL;
                    CleanupStack::Pop(pResponseSoapMsg);
                    retVal = CSenWebServiceSession::HandleSoapFaultL(pResponseSoapMsg, pDetached);
                    if(retVal == KErrSenSoapFault || retVal == KErrNone)
                        {
                        // KErrNone means that original fault was handled
                        // properly and we now have the wanted result 
                        // and consumer may receive it (message body)

                        // Note: currently we don't handle any SOAP faults
                        // in WebServiceSession class, so KErrNone should
                        // not be returned. The OR clause (||) here is 
                        // only for future extensions.
                        delete aResponse;
                        aResponse = NULL;
                        aResponse = pDetached;
                        pDetached = NULL;
                        }
                    else if(retVal == KErrNotFound)
                        {
                        // SOAP fault element  could not be found
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"REST: Major; SOAP fault element not found \
                                    even though assumed SOAP fault.");
                        retVal = KErrSenInternal;
                        }
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(pResponseSoapMsg);
                    }
                }
            else
                {
                // Check complete server messages on/off
                if (!completeServerMessages)
                    {
                    HBufC8* pBody = pResponseSoapMsg->BodyAsStringL();
                    if(pBody)
                        {
                        delete aResponse;
                        aResponse = NULL;
                        aResponse = pBody;
                        pBody = NULL;
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Detached SOAP message body.");
                        }
                    }
                CleanupStack::PopAndDestroy(pResponseSoapMsg);
				}
              }
            }
        }

#ifdef _SENDEBUG
    if(aResponse)
        {
        CleanupStack::PushL(aResponse);
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"--------------------------------------------------");
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"CSenRestServiceSession::SubmitSoapL:");
        CSLOG_FORMAT((aConsumer.ConnectionId()   , KMaxLogLevel, _L8("- Response (%d bytes):"), 
            aResponse->Length()));
        CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,(*aResponse));
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"--------------------------------------------------");
        CleanupStack::Pop(); // aResponse
        }
#endif
    CleanupStack::Pop(); // aResponse
    return retVal;
    }

TInt CSenRestServiceSession::CompleteServerMessagesOnOffL(const TBool& aCompleteOnOff)
    {
    // Now REST SIF supports CompleteServerMessagesOnOff
    return CSenServiceSession::CompleteServerMessagesOnOffL(aCompleteOnOff);
    }


TInt CSenRestServiceSession::SubmitL( const TDesC8& aMessage, 
                                      const TDesC8& aTransportProperties,
                                      MSenRemoteServiceConsumer& aConsumer,
                                      HBufC8*& aResponse )
    {
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenRestServiceSession::SubmitL(TDesC8&)");
    TPtrC8 endpoint = Endpoint();
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"///////////////////////////////////////////////////////");
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KMaxLogLevel, _L8("- Endpoint: %S"), &endpoint));
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KMaxLogLevel, _L8("- Request (%d bytes):"), aMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,(aMessage));
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"///////////////////////////////////////////////////////");
#endif

    MSenTransport& transport = aConsumer.TransportL();
    SetFrameworkPropertiesL( transport );
    TInt retVal( transport.SubmitL( endpoint, 
                                    aMessage, 
                                    aTransportProperties, 
                                    aResponse,
									aConsumer) );

#ifdef _SENDEBUG
    if(aResponse)
        {
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"---------------------------------------------------");
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenRestServiceSession::SubmitL:");
        CSLOG_FORMAT((aConsumer.ConnectionId()   , KMaxLogLevel, _L8("- Response (%d bytes):"), aResponse->Length()));
        CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,( *aResponse ));
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"---------------------------------------------------");
        }
#endif

    return retVal;
    }


TInt CSenRestServiceSession::SendL( const TDesC8& aMessage, 
                                    const TDesC8& aTransportProperties,
                                    MSenRemoteServiceConsumer& aConsumer,
                                    TInt& aTxnId,
                                    HBufC8*& /* aRevalidationError*/ )
    {
    // One could call the base class implementation in CSenServiceSession as well...
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenRestServiceSession::SendL");
    TPtrC8 endpoint = Endpoint();
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"///////////////////////////////////////////////////////");
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KMaxLogLevel, _L8("- Endpoint: %S"), &endpoint));
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KMaxLogLevel, _L8("- Request (%d bytes):"), aMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,( aMessage ));
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"///////////////////////////////////////////////////////");
#endif
    
    MSenTransport& transport = aConsumer.TransportL();
    SetFrameworkPropertiesL(transport);

    TInt retVal ( transport.SendL( endpoint, 
                                   aMessage,
                                   aTransportProperties,
                                   *this, 
                                   aConsumer, 
                                   aTxnId) );
    CSLOG_FORMAT((aConsumer.ConnectionId()   , KNormalLogLevel, _L8("- Transaction id: %d"), aTxnId));
    return retVal;
    }




TInt CSenRestServiceSession::SendToConsumerL(HBufC8* apMessage,
                                             const TInt aTxnId,
                                             MSenRemoteServiceConsumer& aConsumer,
                                             MSenProperties* aResponseTransportProperties)
    {
    TInt retVal(KErrNone);

    aConsumer.HandleMessageL(apMessage, aTxnId, aResponseTransportProperties);

    return retVal;
    }

// THIS IS WHAT TRANSPORT SEES:
TInt CSenRestServiceSession::SendErrorToConsumerL(const TInt aErrorCode,
                                                  HBufC8* apError,
                                                  const TInt aTxnId,
                                                  MSenRemoteServiceConsumer& aConsumer,
                                                  MSenProperties* aResponseTransportProperties)
    {
    TInt retVal(KErrNone);

    retVal = HandleErrorL(aErrorCode, apError, aTxnId, aConsumer, aResponseTransportProperties);
    return retVal;
    }

TInt CSenRestServiceSession::HandleErrorL(const TInt aErrorCode,
                                          HBufC8* apError,
                                          const TInt aTxnId,
                                          MSenRemoteServiceConsumer& aConsumer,
                                          MSenProperties* aResponseTransportProperties)
    {
    TInt retVal(KErrNone);
    // Implementation does not handle non-SOAP errors.
    retVal = aConsumer.HandleErrorL(apError, aErrorCode, aTxnId, aResponseTransportProperties); 
    return retVal;
    }

TBool CSenRestServiceSession::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::EWebServiceSession ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CSenWebServiceSession::HasSuperClass( aType ); 
        }
    }

// TEMPORARY: remove when "message context"- and "transport factory taking 
// properties from all layers"- tasks are ready
TInt CSenRestServiceSession::SetFrameworkPropertiesL(MSenTransport& aTransport)
    {
    return iFramework.SetTransportPropertiesL(aTransport);
    }
//  End of File
