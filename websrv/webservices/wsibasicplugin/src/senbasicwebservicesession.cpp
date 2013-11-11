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
#include "senbasicwebservicesession.h"
#include "SenHttpTransportProperties.h" // could be system included (ADO task)
#include "senservicesession.h"
#include "SenSoapMessage.h"
#include "SenSoapConstants.h"           // KSenSoapActionHeaderName
#include "senwsdescription.h"
#include "msencoreservicemanager.h"
#include "msenremoteserviceconsumer.h"
#include "SenXmlUtils.h"
#include "SenDateUtils.h"
#include "msentransport.h"
#include "SenServiceConnection.h"
#include "sendebug.h"
#include "senlogger.h"
#include <SenSoapFault.h>


CSenBasicWebServiceSession* CSenBasicWebServiceSession::NewL(MSIF& aFramework)
    {
    CSenBasicWebServiceSession* pNew =
        CSenBasicWebServiceSession::NewLC(aFramework);
    CleanupStack::Pop();
    return pNew;
    }

CSenBasicWebServiceSession* CSenBasicWebServiceSession::NewLC(MSIF& aFramework)
    {
    CSenBasicWebServiceSession* pNew =
        new (ELeave) CSenBasicWebServiceSession(EBasicWebServiceSession,
                                                aFramework);
    CleanupStack::PushL(pNew);
    // bugfix 2004-09-08,
    // removed faulty call to CSenServiceSession::BaseConstructL
    pNew->ConstructL();
    return pNew;
    }

CSenBasicWebServiceSession::CSenBasicWebServiceSession(
                                        TDescriptionClassType aType,
                                        MSIF& aFramework)
:
CSenWebServiceSession(aType, aFramework)
    {
    }

void CSenBasicWebServiceSession::ConstructL()
    {
    // Sets the local name to "ServiceDescription" and initiates the inner ipElement
    CSenWebServiceSession::ConstructL();
    }

CSenBasicWebServiceSession::~CSenBasicWebServiceSession()
    {
    }

// Override compute status to make BWS sessions to be ALWAYS valid (ready)
TInt CSenBasicWebServiceSession::ComputeStatusL()
    {
    // session is ready, if an endpoint is defined
    return CSenServiceSession::ComputeStatusL();
    }
    
TInt CSenBasicWebServiceSession::InitializeFromL(MSenServiceDescription& aServiceDescription)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenWebServiceSession::InitializeFromL")));
    
    // Call superclass method to initialize facets, transport (endpoint) cue, etc..
	TInt retVal = CSenWebServiceSession::InitializeFromL(aServiceDescription);
    CSenWSDescription& xmlSD = (CSenWSDescription&)aServiceDescription;
    TPtrC8 transportCue = xmlSD.TransportCue();
    if( transportCue.Length() > 0 )
        {
        SetTransportCueL( transportCue );
        }
	return retVal;
    }
    

TInt CSenBasicWebServiceSession::SubmitSoapL( const TDesC8& aSoapMessage, 
                                              const TDesC8& aTransportProperties,
                                              MSenRemoteServiceConsumer& aConsumer,
                                              HBufC8*& aResponse )
    {
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWsiPlugin::RegisterServiceDescriptionL");
    CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"CSenBasicWebServiceSession::SubmitSoapL(TDesC8&)");
	TPtrC8 endpoint = Endpoint();
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId() , KMaxLogLevel,"///////////////////////////////////////////////////////");
    CSLOG_FORMAT((aConsumer.ConnectionId()  , KMinLogLevel, _L8("- Endpoint: %S"), &endpoint));
	CSLOG_FORMAT((aConsumer.ConnectionId()  , KMaxLogLevel, _L8("- Message (%d bytes):"), aSoapMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId() , KMaxLogLevel,( aSoapMessage ));
    CSLOG_L(aConsumer.ConnectionId() , KMaxLogLevel,"///////////////////////////////////////////////////////");
#endif

    MSenTransport& transport = aConsumer.TransportL();
    SetFrameworkPropertiesL( transport );
    TInt retVal = transport.SubmitL( endpoint, aSoapMessage, aTransportProperties, aResponse, aConsumer);
    
    if(!aResponse)
        {
        // response was NULL: probably either out
        // of heap or some transport malfunction.
        return retVal;
        }
    else if(aResponse->Length() < KSenSoapEnvelopeName().Length()*2)
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
        CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"(NOK) Response is not a SOAP message");
        }
    else
        {

        if (parseRetCode != KErrNone)
            {
            CSLOG_FORMAT((aConsumer.ConnectionId()  , KMinLogLevel, _L8("(NOK) Parsing SOAP msg failed (%d)"),
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
            CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"WS-I: (OK) Response is a SOAP message");
            CleanupStack::PushL(pResponseSoapMsg);

            TBool completeServerMessages(ETrue);
            HasFacetL(KCompleteMessagesFacet, completeServerMessages);

            // response is OK and in SOAP form.
			if(pResponseSoapMsg)
				{
				if (pResponseSoapMsg->IsFault())
					{
					// response is SOAP fault
					CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"WS-I: Response is a SOAP fault.");
					retVal = KErrSenSoapFault;

                if (!completeServerMessages)
                    {
                    CleanupStack::Pop(pResponseSoapMsg);
                    // try to detach the SOAP fault
                    HBufC8* pDetached = NULL;
                    retVal = HandleSoapFaultL(pResponseSoapMsg, pDetached);
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
                        CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"WS-I: Major; SOAP fault element not found \
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
							CSLOG_L(aConsumer.ConnectionId() , KNormalLogLevel,"- Detached SOAP message body.");
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
        CSLOG_L(aConsumer.ConnectionId() , KNormalLogLevel,"- Detached SOAP message body.");
        CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"CSenBasicWebServiceSession::SubmitSoapL:");
        CSLOG_FORMAT((aConsumer.ConnectionId()  , KNormalLogLevel, _L8("- Response (%d bytes):"), 
            aResponse->Length()));
        CSLOG_ALL(aConsumer.ConnectionId() , KMaxLogLevel,(*aResponse));
        CSLOG_L(aConsumer.ConnectionId() , KMaxLogLevel,"--------------------------------------------------");
        CleanupStack::Pop(); // aResponse
        }
#endif
    CleanupStack::Pop(); // aResponse
    return retVal;
    }

TInt CSenBasicWebServiceSession::SendSoapL( const TDesC8& aSoapMessage, 
                                            const TDesC8& aTransportProperties,
                                            MSenRemoteServiceConsumer& aConsumer,
                                            TInt& aTxnId,
                                            HBufC8*& /*aRevalidationError*/ ) 
    {
	TPtrC8 endpoint = Endpoint();
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId() , KMaxLogLevel,"///////////////////////////////////////////////////////");
    CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"CSenBasicWebServiceSession::SendSoapL:");
    CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"- Endpoint:");
	TLSLOG(aConsumer.ConnectionId()  , KMinLogLevel,( endpoint ));
	CSLOG_FORMAT((aConsumer.ConnectionId()  , KMaxLogLevel, _L8("- Message (%d bytes):"), aSoapMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId() , KMaxLogLevel,(aSoapMessage));
    CSLOG_L(aConsumer.ConnectionId() , KMaxLogLevel,"///////////////////////////////////////////////////////");
#endif
    MSenTransport& transport = aConsumer.TransportL();
    SetFrameworkPropertiesL( transport );
    TInt retVal = transport.SendL( endpoint, 
                                   aSoapMessage, 
                                   aTransportProperties, 
                                   *this, 
                                   aConsumer, 
                                   aTxnId );
    CSLOG_FORMAT((aConsumer.ConnectionId()  , KNormalLogLevel, _L8("- Transaction id: %d"), aTxnId));
    return retVal;
    }
    
TInt CSenBasicWebServiceSession::HttpMethodFromPropertiesL( const TDesC8& aProperties,
                                                            CSenHttpTransportProperties::TSenHttpMethod& aMethod )
    {
    TInt retVal(KErrNotFound);
    if ( aProperties.Length() > 0 && aProperties.Find(KHttpMethodLocalName)!= KErrNotFound && 
        (aProperties.Find(KHttpGet)!= KErrNotFound || aProperties.Find(KHttpDelete)!= KErrNotFound) ) 
        {
        // Properties strings are found, now use CSenHttpProperties instance to validate that HTTP
        // method really is either HTTP GET or HTTP DELETE:
        CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC(); // push tp
        
        // Parse aProperties into HTTP transport properties object:
        TInt leaveCode( KErrNone );
        TRAP( leaveCode, pHttpProperties->ReadFromL( aProperties ); )
        if ( leaveCode == KErrNone )
            {
            CSenHttpTransportProperties::TSenHttpMethod method;
            retVal = pHttpProperties->HttpMethodL( method );
            if ( retVal == KErrNone )
                {
                aMethod = method;
                }
#ifdef _SENDEBUG            
            else 
                {
                // HttpMethodL failed:
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- HttpMethodL call failed. Error code: (%d)"), retVal));
                }
#endif // _SENDEBUG            
            }
#ifdef _SENDEBUG            
        else 
            {
            // ReadFromL leaved:
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Reading (parsing) of HTTP properties failed. Leave code: (%d)"), leaveCode));
            
            }
#endif // _SENDEBUG            
        CleanupStack::PopAndDestroy( pHttpProperties ); // de-alloc tp
        }
    return retVal;
    }
    
    

/*
* @param aMessage contains request which is parsed into a SoapEnvelope object using
*                 CSenWebServiceSession::MessageForSendingL and then SendSoapL is called  
*/
TInt CSenBasicWebServiceSession::SendL( const TDesC8& aMessage, 
                                        const TDesC8& aTransportProperties,
                                        MSenRemoteServiceConsumer& aConsumer,
                                        TInt& aTxnId,
                                        HBufC8*& aRevalidationError )
    {
    CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"CSenBasicWebServiceSession::SendL(TDesC8&, ...)");
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId() , KMaxLogLevel,"///////////////////////////////////////////////////////");
	TPtrC8 endpoint = Endpoint();
	CSLOG_FORMAT((aConsumer.ConnectionId()  , KMinLogLevel, _L8("- Endpoint: %S"), &endpoint));
	CSLOG_FORMAT((aConsumer.ConnectionId()  , KMaxLogLevel, _L8("- Message (%d bytes):"), aMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId() , KMaxLogLevel,( aMessage ));
    CSLOG_L(aConsumer.ConnectionId() , KMaxLogLevel,"///////////////////////////////////////////////////////");
#endif
    TInt retVal(KErrNone);

    // First check if HTTP GET or DELETE has been spesified via transport properties:
    // -- in such case, the raw descriptor should be sent "as it is", without wrapping
    // it inside SOAP Body. This way WS-I can support HTTP GET and DELETE and still
    // validate that response(s) are SOAP (unlike REST, which does no response validation).

    CSenHttpTransportProperties::TSenHttpMethod httpMethod;
    TInt readHttpMethodRetVal = HttpMethodFromPropertiesL( aTransportProperties, httpMethod );

    if ( readHttpMethodRetVal == KErrNone && 
         httpMethod == CSenHttpTransportProperties::ESenHttpGet || 
         httpMethod == CSenHttpTransportProperties::ESenHttpDelete )
        {
        // Perform direct call to service session (and not to web service session!)
        retVal = CSenServiceSession::SendL( aMessage, aTransportProperties, aConsumer, aTxnId, aRevalidationError);
        }
    else
        {
        // The request data (UTF-8 descriptor) is expected to be serialized content
        // for SOAP Body and HTTP GET/DELETE is not taking place. 
        // Proceed in standard way and parse the request with SOAP-ENV:
    	CSenSoapMessage* pMsg = NULL;
        retVal = CSenWebServiceSession::MessageForSendingL( aMessage, aConsumer.Id(), pMsg );
        CleanupStack::PushL( pMsg );
        if ( retVal!=KErrNone )
            {
            CSLOG_FORMAT((aConsumer.ConnectionId()  , KMinLogLevel, _L8("- MessageForSendingL returned an error: %d"), retVal));
            CleanupStack::PopAndDestroy( pMsg );
            }
        else // no error
            {
            HBufC8* pSoapMsg = pMsg->AsXmlL();
            CleanupStack::PopAndDestroy( pMsg ); 
            CleanupStack::PushL( pSoapMsg ); 
            retVal = SendSoapL( *pSoapMsg, aTransportProperties, aConsumer, aTxnId, aRevalidationError);
            CleanupStack::PopAndDestroy( pSoapMsg );
            }
        }
    return retVal;
    }

/*
* aMessage contains request which is parsed into a SoapEnvelope object using
* CSenWebServiceSession::MessageForSendingL and then SubmitSoapL is called  
*/
TInt CSenBasicWebServiceSession::SubmitL( const TDesC8&  aMessage, 
                                          const TDesC8&  aTransportProperties ,
                                          MSenRemoteServiceConsumer&  aConsumer ,
                                          HBufC8*& aResponse )
    {
    CSLOG_L(aConsumer.ConnectionId() , KMinLogLevel,"CSenBasicWebServiceSession::SubmitL(TDesC8&, ...)");
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId() , KMaxLogLevel,"///////////////////////////////////////////////////////");
	TPtrC8 endpoint = Endpoint();
	CSLOG_FORMAT((aConsumer.ConnectionId()  , KMinLogLevel, _L8("- Endpoint: %S"), &endpoint));
	CSLOG_FORMAT((aConsumer.ConnectionId()  , KMaxLogLevel, _L8("- Message (%d bytes) about to submit:"), aMessage.Length()));
	CSLOG_ALL(aConsumer.ConnectionId() , KMaxLogLevel,( aMessage ));
	CSLOG_L(aConsumer.ConnectionId() , KMaxLogLevel,"///////////////////////////////////////////////////////");
#endif
    TInt retVal(KErrNone);
    CSenHttpTransportProperties::TSenHttpMethod httpMethod;
    TInt readHttpMethodRetVal = HttpMethodFromPropertiesL( aTransportProperties, httpMethod );

    if ( readHttpMethodRetVal == KErrNone && 
         httpMethod == CSenHttpTransportProperties::ESenHttpGet || 
         httpMethod == CSenHttpTransportProperties::ESenHttpDelete )
        {
        // Perform direct call to service session (and not to web service session!)
        retVal = CSenServiceSession::SubmitL( aMessage, aTransportProperties, aConsumer, aResponse);
        }
    else
        {
        CSenSoapMessage* pMsg = NULL;
        retVal = CSenWebServiceSession::MessageForSendingL( aMessage, aConsumer.Id(), pMsg );
        CleanupStack::PushL(pMsg);
        if ( retVal!=KErrNone )
            {
            CSLOG_FORMAT((aConsumer.ConnectionId()  , KMinLogLevel, _L8("- MessageForSendingL returned an error: %d"), retVal));
            CleanupStack::PopAndDestroy( pMsg );
            }
        else // no error
            {
            HBufC8* pSoapMsg = pMsg->AsXmlL();
            CleanupStack::PopAndDestroy( pMsg ); // de-alloc SOAP object after de-serialization
            CleanupStack::PushL( pSoapMsg );
            retVal = SubmitSoapL( *pSoapMsg, aTransportProperties, aConsumer, aResponse );
            CleanupStack::PopAndDestroy( pSoapMsg );
            }
        }
	return retVal; 
   }


TBool CSenBasicWebServiceSession::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::EWebServiceSession ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        { 
        // Otherwise, ask from direct superclass (may invoke chain of recursive calls)
        return CSenWebServiceSession::HasSuperClass( aType ); 
        }
    }


// TEMPORARY: remove when "message context"- and "transport factory taking 
// properties from all layers"- tasks are ready
TInt CSenBasicWebServiceSession::SetFrameworkPropertiesL(MSenTransport& aTransport)
    {
    return iFramework.SetTransportPropertiesL(aTransport);
    }
TInt CSenBasicWebServiceSession::HandleSoapFaultL(CSenSoapMessage* apSOAPMessage,
                                            const TInt aErrorCode,
                                            const TInt aTxnId,
                                            MSenRemoteServiceConsumer& aConsumer,
                                            MSenProperties* aResponseTransportProperties)
	{
    LOG_WRITE_L("CSenBasicWebServiceSession::HandleSoapFaultL ");

    TInt retVal(KErrNone);
    
    if ( apSOAPMessage )
        {
        	CleanupStack::PushL( apSOAPMessage ); 
	        CSenSoapFault* pDetached = apSOAPMessage->DetachFaultL();
	        if(pDetached)
	            {
	            LOG_WRITE_L("Detached a SOAP fault");
	            CleanupStack::PushL(pDetached);
	            HBufC8* pAsXml = pDetached->AsXmlL();
	            if(pAsXml)
	                {
	                aConsumer.HandleErrorL(pAsXml, KErrSenSoapFault, aTxnId, aResponseTransportProperties);
	                }
	            CleanupStack::PopAndDestroy();
	            retVal = KErrNone; 
	            }
	        else
	            {
	            LOG_WRITE_L("No SOAP fault was received.");
	            aConsumer.HandleErrorL(NULL, aErrorCode, aTxnId, aResponseTransportProperties);
	            retVal = KErrSenInternal;
	            }
	   		CleanupStack::PopAndDestroy( apSOAPMessage ); 
        }
		
		
                
    return retVal;
    }
TInt CSenBasicWebServiceSession::HandleSoapFaultL(CSenSoapMessage* apSOAPMessage,
                                            HBufC8*& aResponse)
{
	return(CSenWebServiceSession::HandleSoapFaultL(apSOAPMessage,aResponse));
}

//  End of File
