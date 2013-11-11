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
#include "senwebservicesession.h"

#include <SenServiceConnection.h> // error codes etc

#include "senservicesession.h"
#include "SenServiceConnection.h" // session status constants
#include "SenSoapMessage.h"
#include "senwsdescription.h"
#include "msencoreservicemanager.h"
#include "msenremoteserviceconsumer.h"
#include "SenXmlUtils.h"
#include "SenDateUtils.h"
#include "msentransport.h"
#include "SenCredential.h"
//#include "SenPolicy.h"
#include "SenSoapFault.h"
#include "sendebug.h"
#include "senservicemanagerdefines.h"
#include "senservicepolicy.h"
#include "seninternalcredential.h"
#include "sensaxutils.h"
#include <xml/dom/xmlengnodelist.h>
#include <SenIdentityProvider.h>
#include "senlogger.h"
#include <SenXmlConstants.h>

#include <SenTransportProperties.h>
// CONSTANTS

namespace
    {
    _LIT8(KServiceInterval1,"ServiceInterval");
    _LIT(KInvalideDate,"19000101:");
    /* microseconds before actual notOnOrAfter time
     * when credentials are treated
     * as expired.
     */
    const TInt KClockSlipMicroSeconds = 3*60*1000*1000;

    // constants for parsing
    _LIT8(KSDFramework,"<ServiceDescription framework=\"");
    _LIT8(KContractStart, "<Contract>");
    _LIT8(KContractEnd, "</Contract>");
    _LIT8(KTagWithAttrEnd, "\">");
    _LIT8(KEndPointStart, "<Endpoint>");
    _LIT8(KEndPointStartWithCue, "<Endpoint cue=\"");   
    _LIT8(KTagWithAttrClose, "\"/>"); 
    _LIT8(KEndPointEnd, "</Endpoint>");
    _LIT8(KCredentialsStart, "<Credentials");
    _LIT8(KCredentialsEnd, "</Credentials>");
    _LIT8(KNotOnOrAfterFormat, " notOnOrAfter=\"%S\" >");
    _LIT8(KEmptyTagEnd, " >");
    _LIT8(KSDEnd, "</ServiceDescription>");
    // localnames for element(s)
    _LIT8(KProviderPolicyLocalName, "ProviderPolicy");
    _LIT8(KNewLine, "\n");
    
    const TInt KCredIdBufSize           = 128;
    _LIT8(KCredentialIdLocalName,       "SenCredentialId");
    _LIT8(KCredentialIdStart,           "<SenCredentialId>");
    _LIT8(KCredentialIdEnd,             "</SenCredentialId>");
    _LIT8(KEndpointLocalname,           "Endpoint");
//    _LIT8(KContractLocalname,           "Contract");
    }

EXPORT_C CSenWebServiceSession::CSenWebServiceSession(TDescriptionClassType aType,
                                                      MSIF& aFramework)
:   CSenServiceSession(aType, aFramework)
    {
    }

EXPORT_C void CSenWebServiceSession::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,_L("CSenWebServiceSession::ConstructL()"));
    // Sets the local name to "ServiceDescription"
    // and initiates the inner ipElement
    CSenServiceSession::BaseConstructL();
    // Init member variables
    iClientServerInterval = 0;
    iValidUntil = Time::NullTTime();
    iMaxTime = Time::NullTTime();;
    iFrameworkId = iFramework.Id().AllocL();
    }

EXPORT_C CSenWebServiceSession::~CSenWebServiceSession()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,_L("CSenWebServiceSession::~CSenWebServiceSession()"));
    delete iSecurity;
    delete iContract;
    delete iEndpoint;
    delete iFrameworkId;
    iFrameworkId = NULL;
    delete iTransportCue;
    iTransportCue = NULL;
    iCredentialPtr.RemoveCredentialObserver(*this);
    iCredentialPtr.Close();
    }

EXPORT_C TPtrC8 CSenWebServiceSession::TransportCue()
    {
    if( iTransportCue )
        {
        return *iTransportCue;
        }
    else 
        {
        return CSenServiceSession::TransportCue();
        }
    }

EXPORT_C TInt CSenWebServiceSession::InitializeFromL(MSenServiceDescription& aServiceDescription)
    {
   TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,_L("Zapping filelogged messages (.xml)"));
    
    // Call superclass method to initialize facets, transport (endpoint) cue, etc..
	CSenServiceSession::InitializeFromL(aServiceDescription);

    HBufC8* pContract = aServiceDescription.Contract().AllocL();
    delete iContract;
    iContract = pContract;
    
    HBufC8* pEndpoint = aServiceDescription.Endpoint().AllocL();
    delete iEndpoint;
    iEndpoint = pEndpoint;

    if(aServiceDescription.DescriptionClassType()
                        ==
        MSenServiceDescription::EWSDescription
                        ||
        aServiceDescription.DescriptionClassType()
                        ==
        MSenServiceDescription::EWSPattern
                        ||
        aServiceDescription.DescriptionClassType()
                        ==
        MSenServiceDescription::EIdentityProvider
        )
        {

        CSenWSDescription& xmlDesc =
                            (CSenWSDescription&)aServiceDescription;

        CSenElement* pElement =
                    (xmlDesc.AsElement()).Element(KServiceInterval1);
        if(pElement)
            {
            TLex8 lex(pElement->Content());
            TInt64 val;
            User::LeaveIfError(lex.Val(val));
            iClientServerInterval = val;
			TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase, KNormalLogLevel , _L8("Interval in microseconds (from file): %d"),
                                iClientServerInterval.Int64()));
            }

        RCredentialList& credentials = xmlDesc.Credentials();
        TInt count(credentials.Count());
        for(TInt i=0; i<count; i++)
            {
            CSenCredential* pCredential = credentials[i];
            if(pCredential)
                {
                // note: errors while adding single credential must ignored:
                // - the remaining credentials (array) must be processed..

                //CSenCredential* pCopy = CSenCredential::NewL(*pCredential);
                //CleanupStack::PushL(pCopy);
                AddCredentialL(*pCredential);
                //CleanupStack::Pop(); // pCopy
                }
            }
        // rebuild provider policy definition
        RebuildFrom(xmlDesc);
        // update provider policy information (new feature)

        MSenServicePolicy* servicePolicy = ServicePolicy();
        if(servicePolicy)
            {
            MSenServicePolicy* givenServicePolicy = xmlDesc.ServicePolicy();
            if(givenServicePolicy)
                (CSenServicePolicy*)servicePolicy->RebuildServicePolicyFrom(*givenServicePolicy);
            }            
        }
    SetStatusL();
    return KErrNone; // err value not currently in use
    }

EXPORT_C TInt CSenWebServiceSession::ComputeStatusL()
    {
    TInt retVal = CSenServiceSession::ComputeStatusL();

    if (retVal == KSenConnectionStatusReady)
        {
        TTime now;
        now.UniversalTime();
        now += iClientServerInterval;

#ifdef _SENDEBUG
            if (iValidUntil != Time::NullTTime())
                {
			TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase, KNormalLogLevel , _L8("Client-Server Interval in microseconds: %d"),
                                    iClientServerInterval.Int64()));
                
                TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
                TInt leaveCode(KErrNone);
                TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(ts, now);)
                if (leaveCode == KErrNone)
                    {
                    
					TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("Fixed client time : %S"), &ts));
                
                    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts2;
                    TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(ts2, iValidUntil);)
			     TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("Credential time : %S, clockslip: %d micros"), &ts2, KClockSlipMicroSeconds));
                    }
                leaveCode = 0; // not used
                }
#endif // _SENDEBUG

        if ((iValidUntil != Time::NullTTime() ) &&
            now >
            (iValidUntil-TTimeIntervalMicroSeconds(KClockSlipMicroSeconds)))
            {
			TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,_L("Credential is expired."));
            retVal = KSenConnectionStatusExpired;
            }
        }
    return retVal;
    }

EXPORT_C TBool CSenWebServiceSession::IsExpiredL()
    {
    ComputeStatusL();
    return (CSenServiceSession::StatusL() == KSenConnectionStatusExpired);
    }

EXPORT_C TInt CSenWebServiceSession::AddCredentialL(
                                                CSenCredential& aCredential)
    {
    if ( aCredential.AsElement().LocalName() == KCredentialIdLocalName )
        {
        delete iSecurity;
        iSecurity = NULL;
        iSecurity = aCredential.AsXmlL();
        
        TInt credentialId;
        TLex8 lex;
        lex.Assign(aCredential.AsElement().Content());
        lex.Val(credentialId);                        

        TInt error(KErrNone);
        RSenCredentialPtr credentialPtr;
        credentialPtr = ((MSenServiceManager&)iFramework.Manager()).CredentialL(credentialId,
                                                                                error);
        if ( error == KErrNone )
            {
            iCredentialPtr = credentialPtr.Clone();
            iCredentialPtr.AddCredentialObserverL(*this);		//codescannerwarnings
            iValidUntil = iCredentialPtr.Credential()->ValidUntil();
            if ( iClientServerInterval == 0 )
                {
                TPtrC8 value;
                TInt ret = iCredentialPtr.Credential()->PropertiesL().PropertyL( KServiceInterval1,
                                                                                 value );
                if ( ret == KErrNone )
                    {
                    TLex8 lex(value);
                    TInt64 val;
                    ret = lex.Val(val);
                    if ( ret == KErrNone )
                        {
                        iClientServerInterval = val;
                        }
                    }
                }
            }
        else
            {
            // invalidate the session, because we have no credential 
            iValidUntil.Set(KInvalideDate); // way back in history: January 1st 1900

            iCredentialPtr.RemoveCredentialObserver(*this);
            iCredentialPtr.Close();

            delete iSecurity;
            iSecurity = NULL;

            TBuf8<KCredIdBufSize> buffer;
            buffer.Num(KErrNotFound);

            iSecurity = HBufC8::NewL(buffer.Length()+KCredentialIdStart().Length()+KCredentialIdEnd().Length());
            TPtr8 sec = iSecurity->Des();
            sec.Append(KCredentialIdStart);
            sec.Append(buffer);
            sec.Append(KCredentialIdEnd);
            }
        }
    else
        {
        TInt retVal(KErrNone);

        delete iSecurity;
        iSecurity = NULL;
        HBufC8* pSecurityToken = 0;
        retVal = ConstructSecurityTokenL(aCredential, pSecurityToken);
        if (KErrNone != retVal)
            {
            if ( pSecurityToken != NULL )
                {
                delete pSecurityToken;
                pSecurityToken = NULL;
                iCredentialPtr.RemoveCredentialObserver(*this);
                iCredentialPtr.Close();
                }
            return retVal;
            }
            
        iValidUntil = aCredential.ValidUntil(); // SetSecurityL needs up-to-date iValidUntil(!)

        CleanupStack::PushL(pSecurityToken);
        SetSecurityL(*pSecurityToken);
        CleanupStack::PopAndDestroy(pSecurityToken);
        }
        
    SetStatusL();
    
    return KErrNone;
    }

EXPORT_C TInt CSenWebServiceSession::ConstructSecurityTokenL( CSenCredential& aCredential,
                                                              HBufC8*& aToken )
    {
    aToken = aCredential.AsXmlL();
    return KErrNone;
    }

EXPORT_C TInt CSenWebServiceSession::NewMessageL( CSenSoapMessage*& aMessage )
    {
    aMessage = CSenSoapMessage::NewL();
    return KErrNone;
    }

EXPORT_C TInt CSenWebServiceSession::MessageForSendingL( const TDesC8& aBody,
                                                         const TDesC8& /* aSenderID */,
                                                         CSenSoapMessage*& aMessage )
    {
    TInt retVal(KErrNone);
    aMessage = CSenSoapMessage::NewL();
    CleanupStack::PushL(aMessage);
    SetFrameworkHeadersL(*aMessage); 
    aMessage->SetBodyL(aBody);
    CleanupStack::Pop(); // aMessage
    return retVal;
    }

EXPORT_C TInt CSenWebServiceSession::ParseResponseL( const TDesC8& aResponse,
                                                     HBufC8*& aParsed )
    {
		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::ParseResponseL(const TDesC8&, HBufC8*&)");
    // this called only from the synchronous submit(..) methods
    TInt retVal(KErrNone);
    CSenSoapMessage* pSoapMessage = NULL;
    retVal = ParseResponseL(aResponse, pSoapMessage);
    if(retVal!=KErrNone)
        {
        delete pSoapMessage;
        return retVal;
        }

    CleanupStack::PushL(pSoapMessage);

    // Check for completeMessages
    TBool completeServerMessages;
    HasFacetL(KCompleteMessagesFacet,completeServerMessages);
    if (completeServerMessages)
        {
        aParsed = pSoapMessage->AsXmlL();
        }
    else
        {
        // If the message contained SOAP fault it will be in the body.
        // It's up to the client to check for a Fault.
        aParsed = pSoapMessage->BodyAsStringL();
        if (!aParsed)
            {
            retVal = KErrSenNoSoapBody; 
						TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::ParseResponseL - NULL from BodyAsStringL - return KErrSenNoSoapBody");
            }
        }

    CleanupStack::PopAndDestroy(); // pSoapMessage
    return retVal;
    }


EXPORT_C TInt CSenWebServiceSession::SendSoapMessageToConsumerL( CSenSoapMessage* apMessage,
                                                                 const TInt aTxnId,
                                                                 MSenRemoteServiceConsumer& aConsumer,
                                                                 MSenProperties* aResponseTransportProperties )
    {
    if ( !apMessage )
        {
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendSoapMessageToConsumerL");
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"Fatal()!: NULL argument passed instead of valid pointer to CSenSoapMessage");
        return HandleErrorL( KErrSenInternal, NULL, aTxnId, aConsumer, aResponseTransportProperties );
        }

    CleanupStack::PushL(apMessage);
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendSoapMessageToConsumerL");
    TInt retVal(KErrNone);
    // Check if consumer wishes to receive complete server messages:
    TBool completeServerMessages;
    HasFacetL(KCompleteMessagesFacet, completeServerMessages);
    if(completeServerMessages)
        {
        HBufC8* pMessageAsXML = apMessage->AsXmlL();
        CleanupStack::PopAndDestroy(apMessage);
        if (pMessageAsXML)
            {
            retVal = aConsumer.HandleMessageL( pMessageAsXML, aTxnId, aResponseTransportProperties );
            }
        }
    else
        {
        //HBufC8* pBody = apMessage->BodyL().Content().AllocL(); 
        HBufC8* pBody = apMessage->BodyAsStringL();
        CleanupStack::PopAndDestroy(apMessage);
        retVal = aConsumer.HandleMessageL( pBody, aTxnId, aResponseTransportProperties );
        }    
    return retVal;
    }

// SYNC, takes ownership of apSoapMessage
EXPORT_C TInt CSenWebServiceSession::HandleSoapFaultL( CSenSoapMessage* apSoapMessage,
                                                       HBufC8*& aResponse /*,
                                                       CSenTransportProperties*& aResponseTransportProperties */) 
    {
    if( apSoapMessage )
        {
        CleanupStack::PushL(apSoapMessage);
        CSenSoapFault* pDetached = apSoapMessage->DetachFaultL();
        CleanupStack::PopAndDestroy( apSoapMessage );
        TBool completeServerMessages(ETrue);
        HasFacetL(KCompleteMessagesFacet, completeServerMessages);
        if(pDetached)
            {
            CleanupStack::PushL(pDetached);
            // Response contains only soapfault in case no completeServerMessages facet 
            // otherwise it should be left empty
            // aResponse is updated only when detached soap fault is required.
            if(completeServerMessages == EFalse)
            	{
            	aResponse = pDetached->AsXmlL();	
            	}
						TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::HandleErrorL:");
						TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- successfully detached SOAP fault:");
            CleanupStack::PopAndDestroy(pDetached); // pDetached
            // From *this* method, KErrSenSoapFault is *OK return code*, 
            // just like KErrNone generally is..
            return KErrSenSoapFault; 
            }
        }
    return KErrNotFound; // SOAP fault not found
    }

// ASYNC, takes ownership of apSoapMessage
EXPORT_C TInt CSenWebServiceSession::HandleSoapFaultL( CSenSoapMessage* apSoapMessage,
                                                       const TInt aErrorCode,
                                                       const TInt aTxnId,
                                                       MSenRemoteServiceConsumer& aConsumer,
                                                       MSenProperties* aResponseTransportProperties )
    {
    TInt retVal( KErrSenInternal );
    if ( apSoapMessage ) 
        {
        CleanupStack::PushL(apSoapMessage); // ownership is here
    
        CSenSoapFault* pDetached = apSoapMessage->DetachFaultL();
        if(pDetached)
            {
            CleanupStack::PopAndDestroy( apSoapMessage ); // de-alloc msg, detach OK
            
            CleanupStack::PushL(pDetached);
            HBufC8* pAsXml = pDetached->AsXmlL();
            CleanupStack::PopAndDestroy(pDetached);
            
            retVal = aConsumer.HandleErrorL(pAsXml, aErrorCode, aTxnId, aResponseTransportProperties);
            }
        else
            {
            // Could not detach Soap fault (this method was called, but aSoapMessage did not include fault!)
            HBufC8* pAsXml = apSoapMessage->AsXmlL();
            CleanupStack::PopAndDestroy( apSoapMessage ); // de-alloc, serialization ok
            retVal = aConsumer.HandleErrorL(pAsXml, aErrorCode, aTxnId, aResponseTransportProperties);
            }
        }
    else
        {
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"Fatal(!): CSenWebServiceSession::HandleSoapFaultL - apSoapMessage arg is NULL. ");
            retVal = aConsumer.HandleErrorL(NULL, aErrorCode, aTxnId, aResponseTransportProperties);
        }        
    return retVal;
    }

EXPORT_C TInt CSenWebServiceSession::SendToConsumerL( HBufC8* apMessage,
                                                      const TInt aTxnId,
                                                      MSenRemoteServiceConsumer& aConsumer,
                                                      MSenProperties* aResponseTransportProperties )
    {
    CleanupStack::PushL( apMessage );
    CSLOG_L( aConsumer.ConnectionId(), KMinLogLevel,"CSenWebServiceSession::SendToConsumerL - begin" );
    TInt retVal(KErrNone);

    // Check whether this is a response to one-way request; in such case, 
    // there is no body to validate (but only few response properties):
    if ( aResponseTransportProperties )
        {
        TBool isOnewayMsgRsp(EFalse);
        aResponseTransportProperties->BoolPropertyL( KSenOnewayMessageOnOff, isOnewayMsgRsp );
        if ( isOnewayMsgRsp )
            {
            // There is no response body to process, halt the execution of this method in here,
            // and invoke the consumer immediately:
            CSLOG_L( aConsumer.ConnectionId(), KMinLogLevel,"-- About to invoke MSenRemoteServiceConsumer::HandleMessageL" );
            CleanupStack::Pop( apMessage );
            retVal = aConsumer.HandleMessageL( apMessage, aTxnId, aResponseTransportProperties );
            CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("CSenWebServiceSession::SendToConsumerL - end: HandleMessageL returned: %d"), retVal));
            return retVal; // skip the SOAP validation, response carries no body
            }
        }

    
    // SOAP orientated implementation:
    CSenSoapMessage* pSoapMessage = NULL;
    TInt leaveCode(KErrNone); 
    
    if( apMessage && apMessage->Length() > 0 )
        {
        TRAP( leaveCode, retVal = ParseResponseL( *apMessage, pSoapMessage ); )
        }
    else // apMessage == NULL or descriptor is of zero-length
        {
        if ( !apMessage )
            {
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Note: apMessage argument is NULL => response is treated through KErrSenNoSoapBody");
            }    
        retVal = KErrSenNoSoapBody; // clearly, WSS assumes that all messages are SOAP!
        }
        
    if( leaveCode != KErrNone )
        {
        // Parsing WAS attempted, but it failed(!)  
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendToConsumerL");
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- Error: response could not be parsed into SOAP msg, leave code from parse: %d"), leaveCode));
        pSoapMessage = NULL;
        if( retVal==KErrNone )
            {
            // indicate with return value, that response is
            // invalid - even though submit was ok, the
            // response could NOT be parsed!
            retVal = leaveCode;
            }
        }
        
    if(retVal == KErrNone)
        {
        CleanupStack::PopAndDestroy(apMessage);
        CleanupStack::PushL( pSoapMessage );
        // Response was parsed OK, now check whether it is a SOAP fault:
        if (pSoapMessage->IsFault())
            {
            CleanupStack::Pop(pSoapMessage); // next method takes the ownership:
            retVal = HandleSoapFaultL(pSoapMessage, KErrSenSoapFault, aTxnId, aConsumer, aResponseTransportProperties);
            if( retVal == KErrSenSoapFault )
                {
                // SOAP fault response is treated as "OK"
                retVal = KErrNone;
                }
            }
        else
            {
            // Response is a valid SOAP envelope
            CSLOG_L(aConsumer.ConnectionId()  ,  KMinLogLevel ,"CSenWebServiceSession::SendToConsumerL:");
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel ,"- Sending SOAP message to consumer.");
            CleanupStack::Pop(pSoapMessage); // next method takes the ownership:
            retVal = SendSoapMessageToConsumerL( pSoapMessage, aTxnId, aConsumer, aResponseTransportProperties );
#ifdef _SENDEBUG
            if( retVal!=KErrNone )
                {
                CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendToConsumerL:");
                CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- SendSoapMessageToConsumerL failed: %d"), retVal ));
                }
#endif
            }
        }
    else // error recognized
        {
        delete pSoapMessage;
        pSoapMessage = NULL;
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendToConsumerL");
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Error: response is not a SOAP message!");
        
        CleanupStack::Pop(apMessage);
        retVal = HandleErrorL(retVal, apMessage, aTxnId, aConsumer, aResponseTransportProperties);
        }
    CSLOG_L( aConsumer.ConnectionId(), KMinLogLevel,"CSenWebServiceSession::SendToConsumerL - end" );        
    return retVal;
    }

// THIS IS WHAT TRANSPORT SEES:
EXPORT_C TInt CSenWebServiceSession::SendErrorToConsumerL( const TInt aErrorCode,
                                                           HBufC8* apError,
                                                           const TInt aTxnId,
                                                           MSenRemoteServiceConsumer& aConsumer,
                                                           MSenProperties* aResponseTransportProperties )
    {
    CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("CSenWebServiceSession::SendErrorToConsumerL - aErrorCode [%d]"), aErrorCode));
    CleanupStack::PushL(apError);
    TInt retVal(KErrNone);

    // Check whether this is a response to one-way request; in such case, 
    // there is no body to validate (but only few response properties):
    if ( aResponseTransportProperties )
        {
        TBool isOnewayMsgRsp(EFalse);
        aResponseTransportProperties->BoolPropertyL( KSenOnewayMessageOnOff, isOnewayMsgRsp );
        if ( isOnewayMsgRsp )
            {
            // There is no response body to process, halt the execution of this method in here,
            // and invoke the consumer immediately:
            CSLOG_L( aConsumer.ConnectionId(), KMinLogLevel,"-- About to invoke MSenRemoteServiceConsumer::HandleMessageL" );
            CleanupStack::Pop( apError );
            retVal = aConsumer.HandleErrorL( apError, aErrorCode, aTxnId, aResponseTransportProperties );
            CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("CSenWebServiceSession::SendToConsumerL - end: HandleMessageL returned: %d"), retVal));
            return retVal; // skip the SOAP validation, response carries no body
            }
        }
    
    // SOAP orientated implementation:
    CSenSoapMessage* pSoapMessage = NULL;
    
        TInt leaveCode(KErrNone); 
    
    if( apError && apError->Length() > 0 )
        {
        TRAP( leaveCode, retVal = ParseResponseL( *apError, pSoapMessage ); )
        }
    else // apMessage == NULL or descriptor is of zero-length
        {
        if ( !apError )
            {
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Note: apError argument is NULL => response is treated through KErrSenNoSoapBody");
            }    
        retVal = KErrSenNoSoapBody; // clearly, WSS assumes that all messages are SOAP!
        }
    
    if( leaveCode != KErrNone )
        {
        // Parsing WAS attempted, but it failed(!)
        pSoapMessage = NULL;
        
        // THE RESPONSE IS NOT SOAP
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendErrorToConsumerL");
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Error: response could not be parsed into SOAP msg!");
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- ParseResponseL leaved: %d"), leaveCode ));
        if( aErrorCode == KErrNone )
            {
            // In this rare case, indicate that SOAP envelope was broken!
            // So, return KErrSenBrokenSoapEnvelope in here.
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- aErrorCode==KErrNone, but SOAP env is broken!");
            
            CleanupStack::Pop(apError);
            retVal = HandleErrorL(KErrSenBrokenSoapEnvelope, apError, aTxnId, aConsumer, aResponseTransportProperties);
            }
        else
            {
            CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- Returning error code received from transport: %d"), 
                aErrorCode));

            CleanupStack::Pop(apError);
            retVal = HandleErrorL(aErrorCode, apError, aTxnId, aConsumer, aResponseTransportProperties);
            }
        }
    else // ParseResponseL did not leave 
        {
        if (retVal == KErrNone )
            {
            CleanupStack::PopAndDestroy(apError);
            // ParseResponseL OK
            // Call for functionality which checks certain, "recoverable" SOAP errors:
            // Some frameworks might be able to handle certain SOAP faults themselves
            retVal = HandleSoapFaultL(pSoapMessage, aErrorCode, aTxnId, aConsumer, aResponseTransportProperties);
            }
        else // parsing failed or apError == NULL
            {
            delete pSoapMessage;
            // ParseResponseL NOT ok!
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendErrorToConsumerL");
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Error: response is not a SOAP message!");
            if ( aErrorCode == KErrNone )
                {
                // Rare case: for some odd reason transport did not return error code:
                // - return the error code from ParseResponseL
                CSLOG_L(aConsumer.ConnectionId()  , KNormalLogLevel,"- Transport did not return error code.");
                CleanupStack::Pop(apError);
                retVal = HandleErrorL(retVal, apError, aTxnId, aConsumer, aResponseTransportProperties);
                }
            else
                {
                // Just pass forward the error code received from transport:
                CSLOG_L(aConsumer.ConnectionId()  , KNormalLogLevel,"- Just pass forward error code from transport.");
                CleanupStack::Pop(apError);
                retVal = HandleErrorL(aErrorCode, apError, aTxnId, aConsumer, aResponseTransportProperties);
                }
            }
        }
    return retVal;
    }

EXPORT_C TInt CSenWebServiceSession::HandleErrorL( const TInt aErrorCode,
                                                   HBufC8* apError,
                                                   const TInt aTxnId,
                                                   MSenRemoteServiceConsumer& aConsumer,
                                                   MSenProperties* aResponseTransportProperties )
    {
    // Current implementation does not handle non-SOAP errors 
    // that occured at transport layer. Instead, the error is
    // simply passed to consumer:
    return aConsumer.HandleErrorL(apError, aErrorCode, aTxnId, aResponseTransportProperties); 
    // Frameworks should override this method in order to recover
    // from specific errors/faults
    }


EXPORT_C CSenXmlReader* CSenWebServiceSession::XmlReader()
    {
    return iFramework.Manager().XMLReader();
    }

EXPORT_C TInt CSenWebServiceSession::ParseResponseL(const TDesC8& aResponse,
                                                    CSenSoapMessage*& aMessage)
    {
		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::ParseResponseL(TDesC8&, CSenSoapMessage*&");

    TInt retVal(KErrNone);
    retVal = NewMessageL(aMessage);

    CleanupStack::PushL(aMessage);

    if (retVal != KErrNone)
        {
        return retVal;
        }

    //////////////////////////////////////////////////////////////////////////
		TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(_L("******************************************************")));
		TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMaxLogLevel  , _L("Response to be parsed (%d bytes):"), aResponse.Length()));
        {
				TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,aResponse);
        //FILELOGALL(_L("WsLog"), _L("last_rsp.xml"), aResponse);
        }
		TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(_L("******************************************************")));
    //////////////////////////////////////////////////////////////////////////

    aMessage->SetReader(*XmlReader());
    TInt leaveCode(KErrNone);
		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- Parsing response into SOAP message object:");
    TBool normalParsing(ETrue);
    if ( aResponse.Size() > KSenMaxLengthSaxParsingSoapMsg )
        {
        TInt ret(KErrNotFound);
        TRAP(leaveCode, ret = HandleBodyWithoutParsingL(*aMessage, aResponse);)
        if (leaveCode == KErrNone && ret == KErrNone)
            {
            normalParsing = EFalse;
            }
        }
    
    if ( normalParsing )
        {
        TRAP(leaveCode, aMessage->ParseL(aResponse));
        }
    
    if(leaveCode==KErrNone)
        {
#ifdef _SENDEBUG
        ///////////////////////////////////////////////////////////////////////
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(_L("---------------------------------------------------")));
        HBufC8* pAsXml = aMessage->AsXmlL();
        if(pAsXml)
            {
            CleanupStack::PushL(pAsXml);
						TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::ParseResponseL: OK!");
						TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMaxLogLevel  , _L(" SOAP message (%d bytes)"),
                                pAsXml->Length()));
						TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(*pAsXml));
            CleanupStack::PopAndDestroy(); //pAsXml
            }
				TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(_L("---------------------------------------------------")));
        ///////////////////////////////////////////////////////////////////////
#endif // _SENDEBUG
        retVal = ParseMessageL(*aMessage);
        }
    else
        {
				TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- Response is not SOAP message!");
        retVal = leaveCode;
        }
    CleanupStack::Pop(); // aMessage
    return retVal;
    }

EXPORT_C TInt CSenWebServiceSession::ParseMessageL(CSenSoapMessage& aSoapMessage)
    {
    // We can verify, that SOAP message has Body:
    if(!aSoapMessage.HasBody())
        {
        return KErrSenNoSoapBody;
        }
    else
        {
        return KErrNone;
        }
    /**
     * We could verify wsse:Security headers
     * but we don't have a need for that right now.
     */
    }

EXPORT_C TInt CSenWebServiceSession::AddConsumerL(MSenRemoteServiceConsumer& aConsumer)
    {
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::AddConsumerL");
    iConsumerList.Reset(); 
    TInt retVal = iConsumerList.Append(&aConsumer);
    return retVal;
    }

// 2005: refactored GetConsumer() to Consumer()
// Note: Same as client() in Java Ref. Impl.
EXPORT_C MSenRemoteServiceConsumer* CSenWebServiceSession::Consumer()
    {
    TInt count(iConsumerList.Count());
    if(count>0)
        {
        return iConsumerList[0];
        }
    else
        {
        return NULL;
        }
    }

EXPORT_C TBool CSenWebServiceSession::IsReadyL()
    {
    return (CSenServiceSession::StatusL() == KSenConnectionStatusReady);
    }

EXPORT_C TInt CSenWebServiceSession::SubmitL(const TDesC8& aMessage,
                                             const TDesC8& aTransportProperties,
                                             MSenRemoteServiceConsumer& aConsumer,
                                             HBufC8*& aResponse /*,
                                             CSenTransportProperities*& apResponseTransportProperties */)
    {
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL");

    CSenSoapMessage* pMsg = NULL;
    TInt retVal( MessageForSendingL(aMessage, aConsumer.Id(), pMsg) ); 
    CleanupStack::PushL(pMsg);

    if(retVal!=KErrNone)
        {
         CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- MessageForSendingL returned an error: %d"), retVal));
        CleanupStack::PopAndDestroy(); // pMsg
        return retVal;
        }

    HBufC8* pHttpBody = pMsg->AsXmlL();
    if ( pHttpBody )
        {
        TPtrC8 endpoint = Endpoint();
        CleanupStack::PushL(pHttpBody);
        TPtr8 httpBody = pHttpBody->Des();
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"////////////////////////////////////////////////////////");
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Endpoint: %S"), &endpoint));
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Message (%d) about to submit:"),
                            httpBody.Length()));
        //CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,(httpBody));
//wslog        FILELOGALL(_L("WsLog"), _L("last_req.xml"), httpBody);
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"////////////////////////////////////////////////////////");


        MSenTransport& transport = aConsumer.TransportL();
        retVal = transport.SubmitL(endpoint, httpBody, aTransportProperties, aResponse, aConsumer);

        CleanupStack::PopAndDestroy(); // pHttpBody
        }

    CleanupStack::PopAndDestroy(); //  pMsg

    if ( !aResponse )
        {
        // Response was NULL: probably either out
        // of heap or some transport malfunction.
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL:");
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Received NULL from transport.");
        return retVal;
        }

    else if( aResponse->Length() < KSenSoapEnvelopeName().Length()*2 )
        {
        // No use parsing, Envelope -root element not there

        // deliver non-soap body to consumer
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL:");
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- response is not a SOAP envelope.");
        return retVal;
        }

    CSenSoapMessage* pResponseSoapMsg = NULL;
    TInt leaveCode(KErrNone);
    TInt parseRetVal(KErrNone);
    TRAP( leaveCode, parseRetVal = ParseResponseL(*aResponse, pResponseSoapMsg));

    if( leaveCode!=KErrNone ) // parsing leaved!
        {

        pResponseSoapMsg = NULL; 

        // Return directly the response, which was received from
        // transport. Note that pResponseSoapMsg has already
        // been deleted because of leave
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel , "CSenWebServiceSession::SubmitL:");
       CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel , "- Error: response could not be parsed into SOAP msg!");
        
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- ParseResponseL leaved: %d"), leaveCode ));

        retVal = leaveCode;
        // else return the error code received from transport (retVal)
        }
    else // parsing did not leave..
        {
        if ( parseRetVal != KErrNone ) // .. but parsing failed
            {
            // Not mandatory, ParseResponseL should take 
            // care of gc, if it returns an error:
            delete pResponseSoapMsg; 

            // NOTE - 2005 change: now the body of such
            // response will be returned to the caller
            // (with the error code).

            // Response could not be parsed into SOAP message(!)
            CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- Response could not be parsed, error: (%d)"),
                parseRetVal));

            if(retVal==KErrNone)
                {
                CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Returning error code from ParseResponseL");
                // transport did not return error, return error from ParseResponseL
                retVal = parseRetVal;
                }
            // else: return the error code received from transport
            }
        else // .. and parsing went ok
            {
            CleanupStack::PushL( pResponseSoapMsg );
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Response is a SOAP message");

            // Check whether "complete server messages" is on or off
            TBool completeServerMessages(EFalse);
            HasFacetL(KCompleteMessagesFacet, completeServerMessages);

            // Response is OK and SOAP message. 
            if( pResponseSoapMsg->IsFault() )
                {
                // Response is a SOAP fault
                retVal = KErrSenSoapFault; // might change, if framework handles this fault
                CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"Response is a SOAP fault. Calling HandleErrorL.");

                // Check if SOAP fault could be handled by the framework:
                HBufC8* pResponse = NULL;

                // Note that HandleSoapFaultL -method normally detaches any SOAP fault:
                CleanupStack::Pop(pResponseSoapMsg);
                retVal = HandleSoapFaultL(pResponseSoapMsg, pResponse);
                if(retVal == KErrSenSoapFault)
                    {
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL:");
                    if( completeServerMessages )
                        {
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Returning SOAP envelope with fault");
                        // The following lines are important when there is a resending
                        // the new response is copied to aResponse.
                        // When there is no resend, then pResponse = NULL no copy happens                     
                        if(pResponse)
                        	{
                        	delete aResponse;
                        	aResponse = pResponse;
                        	}
                        // aResponse will be returned (original response from transport)
                        }
                    else // we can pass the detached fault
                        {
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Returning detached a SOAP fault.");
                        // Destroy the original response buffer received from transport,
                        // since now the parsed SOAP envelope can be returned
                        delete aResponse;
                        aResponse = pResponse;
                        }
                    }
                else if ( retVal == KErrNone )
                    {
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL:");
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Framework handled this SOAP fault.");
                    // KErrNone means that fault was handled
                    // properly (by framework spesific code)
                    // and consumer may receive the response 
                    // it requested.

                    // Currently there are no SOAP faults handled
                    // in WebServiceSession class, so KErrNone is
                    // never returned. This if -section is here 
                    // only for *frameworks to extend*.

                    // Destroy the original response buffer received from transport,
                    // since now the parsed SOAP envelope can be returned
                    delete aResponse;
                    aResponse = pResponse;
                    }
                else if ( retVal == KErrNotFound )
                    {
                    // SOAP fault element  could not be found
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL - MAJOR:");
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"-IsFault()==true BUT fault element NOT found!");
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"-returning KErrSenBrokenSoapFault");
                    retVal = KErrSenBrokenSoapFault;
                    // Note: 2005 change: now the broken (original) response
                    // from the transport is returned, instead of NULL
                    }
#ifdef _SENDEBUG
                else 
                    {
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL:");
                    CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- HandleErrorL failed: %d"), retVal));
                    }
#endif // _SENDEBUG

                }
            else // This SOAP envelope is NOT a fault
                {
                if ( completeServerMessages )
                    {
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL:");
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Returning complete SOAP envelope.");
                    // Destroy the original response buffer received from
                    // transport, since now the parsed SOAP envelope can
                    // be returned

                    delete aResponse;
                    aResponse = NULL;
                    aResponse = pResponseSoapMsg->AsXmlL(); 
                    }
                else
                    {
                    HBufC8* pBody = pResponseSoapMsg->BodyAsStringL();
                    if(pBody)
                        {
                        // Destroy the original response buffer received from 
                        // transport, since now the SOAP body must be returned
                        delete aResponse;
                        aResponse = pBody;
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL:");
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Detached SOAP message body.");
                        }
                    else
                        {
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitL:");
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Could not detach SOAP body(!) - return KErrSenNoSoapBody");
                        retVal = KErrSenNoSoapBody;
                        }
                    }
                CleanupStack::PopAndDestroy(pResponseSoapMsg);
                }
            }
        }

#ifdef _SENDEBUG
    if(aResponse)
        {
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"-------------------------------------------------------------------");
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("Submit response (%d bytes):"),
                            aResponse->Length()));
        //CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,( *aResponse ));
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"-------------------------------------------------------------------");
        }
#endif

    return retVal;
    }

EXPORT_C TInt CSenWebServiceSession::SendL( const TDesC8& aMessage, 
                                            const TDesC8& aTransportProperties,
                                            MSenRemoteServiceConsumer& aConsumer,
                                            TInt& aTxnId,
                                            HBufC8*& /*aRevalidationError*/ )
    {
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendL");

    CSenSoapMessage* pMsg = NULL;
    TInt retVal( MessageForSendingL(aMessage, aConsumer.Id(), pMsg) ); 
    CleanupStack::PushL(pMsg);

    if(retVal!=KErrNone)
        {
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- MessageForSendingL returned an error: %d"), retVal));
        CleanupStack::PopAndDestroy(); // pMsg
        return retVal;
        }

    HBufC8* pHttpBody = pMsg->AsXmlL();
    if(pHttpBody)
        {
        TPtrC8 endpoint  = Endpoint();
        CleanupStack::PushL(pHttpBody);
        TPtr8 httpBody = pHttpBody->Des();
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"////////////////////////////////////////////////////////");
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Endpoint: %S"), &endpoint));
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Message (%d bytes) about to send:"), httpBody.Length()));
        CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,(httpBody));
//wslog        FILELOGALL(_L("WsLog"), _L("last_req.xml"), httpBody);
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"////////////////////////////////////////////////////////");

        MSenTransport& t = aConsumer.TransportL();

        retVal = t.SendL( endpoint, httpBody, aTransportProperties, *this, aConsumer, aTxnId );

        CleanupStack::PopAndDestroy(); // pHttpBody
        }

    CleanupStack::PopAndDestroy(); //  pMsg
    return retVal;
    }



EXPORT_C void CSenWebServiceSession::SetFrameworkHeadersL(CSenSoapMessage& aMsg)
    {
    if ( iCredentialPtr.Credential() )
        {
        CSenInternalCredential* pCred = iCredentialPtr.Credential();
        HBufC8* pAsXml = pCred->AsXmlL();
        CleanupStack::PushL(pAsXml);
        aMsg.SetSecurityHeaderL(*pAsXml);
        CleanupStack::PopAndDestroy(pAsXml);
        }
    else
        {
        // There is no credential available
        aMsg.SetSecurityHeaderL(KNullDesC8); 
        }
    }

EXPORT_C TInt CSenWebServiceSession::SetUserNameL(TDesC8& aUsername)
    {
    TInt retVal(KErrNone);
    HBufC8* pToken8 = NULL;

    retVal = CSenWsSecurityHeader::UsernameTokenL(aUsername, pToken8);
    CleanupStack::PushL(pToken8);

    if(retVal != KErrNone)
        {
        CleanupStack::PopAndDestroy(1); // token
        return retVal;
        }
    else if(pToken8)
        {
        if(iSecurity == NULL)
            {
            iSecurity = pToken8->Des().AllocL();
            }
        else
            {
            TPtr8 ptr = iSecurity->Des();

            if (ptr.MaxLength() < pToken8->Length() + ptr.Length())
                {
                HBufC8* newSecurity = HBufC8::NewLC(pToken8->Length()
                                                    +ptr.Length());
                TPtr8 newPtr = newSecurity->Des();
                newPtr.Append(*iSecurity);
                newPtr.Append(*pToken8);
                delete iSecurity;
                iSecurity = newSecurity;
                CleanupStack::Pop();    // newSecurity
                }
            else
                {
                ptr.Append(*pToken8);
                }
            }
        }
    CleanupStack::PopAndDestroy(); // pToken8

    return retVal;
    }

EXPORT_C TPtrC8 CSenWebServiceSession::Endpoint()
    {
    if(iEndpoint)
        return *iEndpoint;
    else
        return KNullDesC8();
    }

EXPORT_C TPtrC8 CSenWebServiceSession::Contract()
    {
    if(iContract)
        return *iContract;
    else
        return KNullDesC8();
    }

EXPORT_C TPtrC8 CSenWebServiceSession::FrameworkId()
    {
    if(iFrameworkId)
        return *iFrameworkId;
    else
        return KNullDesC8();
    }

EXPORT_C TPtrC8 CSenWebServiceSession::FrameworkVersion()
    {
    return KNullDesC8();
    }

EXPORT_C void CSenWebServiceSession::SetContractL(const TDesC8& aContract)
    {
    HBufC8* pNew = NULL;
    if(aContract!=KNullDesC8)
        {
        pNew = aContract.AllocL();
        }

    delete iContract;
    iContract = pNew;
    }

EXPORT_C void CSenWebServiceSession::SetEndPointL(const TDesC8& aEndpoint)
    {
    HBufC8* pNew =NULL;
    if(aEndpoint!=KNullDesC8)
        {
        pNew = aEndpoint.AllocL();
        }

    delete iEndpoint;
    iEndpoint = pNew;

    SetStatusL();
    }

EXPORT_C void CSenWebServiceSession::SetSecurityL(const TDesC8& aSecurity)
    {
		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::SetSecurityL");
    TInt retVal(KErrNone);
    
    if ( aSecurity.Length() < 1 )
        {
        delete iSecurity;
        iSecurity = NULL;
        iCredentialPtr.RemoveCredentialObserver(*this);
        iCredentialPtr.Close();

        // invalidate the session, because we have no credential 
        //iValidUntil.Set(_L("19000101:")); // way back in history: January 1st 1900
        //acording to ID-WSF no need of reset the credential here
        }
     else
        {
        if ( HasSecurity() )
            {
            iCredentialPtr.RemoveCredentialObserver(*this);
            ((MSenServiceManager&)iFramework.Manager()).UpdateCredentialL(
                                iCredentialPtr.Credential()->IdentifierL().IdL(),
                                aSecurity,
                                retVal);
            if ( retVal == KErrNone )
                {
                iCredentialPtr.AddCredentialObserverL(*this);
                CSenInternalCredential* pCred = iCredentialPtr.Credential();
                
                if ( iClientServerInterval.Int64() != 0 )
                    {
                    TBuf8<64> buf;
                    buf.AppendNum(iClientServerInterval.Int64());
                    pCred->PropertiesL().SetPropertyL(KServiceInterval1, buf);
                    }
                FillCredentialIdentifierL(pCred->IdentifierL());

                TTime validUntil = iValidUntil;
                pCred->PropertiesL().SetValidUntilL(validUntil);
                }
            else
                {
                delete iSecurity;
                iSecurity = NULL;
                iCredentialPtr.Close();
                
                // invalidate the session, because we have no credential 
        		iValidUntil.Set(KInvalideDate); // way back in history: January 1st 1900  //CodeScannerWarnings
                }
            }
        else
            {
            RSenCredentialPtr credentialPtr =
                    ((MSenServiceManager&)iFramework.Manager()).AddCredentialL(aSecurity, retVal);
        
            if ( retVal == KErrNone )
                {        

                iCredentialPtr.RemoveCredentialObserver(*this);
                iCredentialPtr.Close();
                iCredentialPtr = credentialPtr.Clone();
                iCredentialPtr.AddCredentialObserverL(*this);	//codescannerwarnings

                CSenInternalCredential* pCred = iCredentialPtr.Credential();
                
                if ( iClientServerInterval.Int64() != 0 )
                    {
                    TBuf8<64> buf;
                    buf.AppendNum(iClientServerInterval.Int64());
                    pCred->PropertiesL().SetPropertyL(KServiceInterval1, buf);
                    }
                FillCredentialIdentifierL(pCred->IdentifierL());	//codescannerwarnings
                
                TTime validUntil = iValidUntil;
                pCred->PropertiesL().SetValidUntilL(validUntil);
                
                SetCredentialIdL(pCred->IdentifierL().IdL());
                }
            else
                {
                delete iSecurity;
                iSecurity = NULL;
                iCredentialPtr.RemoveCredentialObserver(*this);
                iCredentialPtr.Close();
                
                // invalidate the session, because we have no credential 
                iValidUntil.Set(KInvalideDate); // way back in history: January 1st 1900
                }
            }
        }
    }
    
/*    
EXPORT_C TInt CSenWebServiceSession::RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer)    
    {
    LOG_WRITE_L("CSenWebServiceSession::RemoveConsumerL");
    CSenServiceSession::RemoveConsumerL(aConsumer);
    
    // If this session has no consumers, we can hold our grip to the credential
    // (note: this does NOT mean that credential is removed, not at all(!), but
    //  that new search for the credential has to be performed).
    
    // NOTE: this cannot be done at ws service session layer as long as ID-WSF does not
    // use credential sharing / lookup mechanism in cases where session is found invalid
    // (this is done in WS-* framework plug-in)
    TInt count(iConsumerList.Count());
    if( count == 0 )
        {
        LOG_WRITE_L("- Credential count == 0");
        LOG_WRITE_L("-> Closing the handle to the credential owned by Credential Manager.");
        SetSecurityL(KNullDesC8);    
        }
    }
*/    
void CSenWebServiceSession::SetCredentialIdL(TInt aCredentialId)
    {
    TBuf8<KCredIdBufSize> buffer;
    buffer.Num(aCredentialId);

    delete iSecurity;
    iSecurity = NULL;
    TInt leaveCode( KErrNone );
    TRAP( leaveCode, iSecurity = HBufC8::NewL( buffer.Length()+KCredentialIdStart().Length()+KCredentialIdEnd().Length() ); )
    if( !leaveCode && iSecurity )
        {
        TPtr8 sec = iSecurity->Des();
        sec.Append( KCredentialIdStart );
        sec.Append( buffer);
        sec.Append( KCredentialIdEnd );
        }
#ifdef _SENDEBUG
    else
        {
				TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::SetCredentialId(): iSecurity = HBufC8::NewL leaved (OOM)!");
        }
#endif // _SENDEBUG        
    }

EXPORT_C TInt CSenWebServiceSession::GetCredentialIdL()
    {
    TInt retVal(0);
    if (HasSecurity())
        {
        retVal = iCredentialPtr.Credential()->IdentifierL().IdL();	//codescannerwarnings
        }
    return retVal;
    }

    
EXPORT_C void CSenWebServiceSession::FillCredentialIdentifierL(CSenCredentialIdentifier& aIdentifier)
    {
		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::FillCredentialIdentifierL");
    if ( iEndpoint )
        {
				TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,  _L8("- Setting endpoint to credential as property: %S"), iEndpoint ));
        aIdentifier.SetPropertyL(KEndpointLocalname, *iEndpoint);
        }
    }
    
EXPORT_C TInt CSenWebServiceSession::TryToSearchValidCredentialL()
    {
		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::TryToSearchValidCredentialL()");
    
    // Get CredentialIdentifier which should include ProviderID
    CSenCredentialIdentifier* pCredIdentifier = CSenCredentialIdentifier::NewLC();
    FillCredentialIdentifierL(*pCredIdentifier);
    
    CSenWSDescription* pPattern = CSenWSDescription::NewLC();
    CSenElement& patternElement = ((CSenWSDescription*)pPattern)->AsElement();
    
    TPtrC8 providerId(KNullDesC8);
    
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    TXmlEngElement element = pCredIdentifier->AsElementL();
    element.GetChildElements(list);
    while ( list.HasNext() )
        {
        TXmlEngElement element = list.Next();
        if ( element.Name() != KSenIdpProviderIdLocalname )
            {
            CSenElement& addedElement = patternElement.AddElementL(element.Name());
            addedElement.SetContentL(element.Text());
            }
        else
            {
            providerId.Set(element.Text());
            }
        }
    
    CSenIdentityProvider* pIdentityProvider = NULL;
    if ( providerId != KNullDesC8 )
        {
				TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::TryToSearchValidCredentialL():");
				TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  , _L8("- Trying to search IdentityProvider using ProviderId : %S"), &providerId));
        CSenWSDescription* pIdpPattern = CSenWSDescription::NewLC();
        pIdpPattern->SetEndPointL(providerId);
        pIdentityProvider = iFramework.Manager().IdentityProviderL(*pIdpPattern);
        CleanupStack::PopAndDestroy(pIdpPattern);
        }

    RSenCredentialPtrArray credentialPtrsArray;
    CleanupClosePushL(credentialPtrsArray);

    TInt retVal(KErrNone);
    if ( pIdentityProvider )
        {
				TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::TryToSearchValidCredentialL() - IdentityProvider was found. Trying to search related Credentials.");
        retVal = ((MSenServiceManager&)iFramework.Manager()).CredentialsL(*pPattern, 
                                                                          *pIdentityProvider,
                                                                          credentialPtrsArray);
        }
    else
        {
				TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::TryToSearchValidCredentialL() - FATAL: IdentityProvider was NOT found:");
				TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- Trying to search Credentials without IdentityProvider information.");
                retVal = ((MSenServiceManager&)iFramework.Manager()).CredentialsL(*pPattern,
                                                                          credentialPtrsArray);
        }
        
    if ( (retVal == KErrNone) && (credentialPtrsArray.Count() > 0) )
        {
				TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("CSenWebServiceSession::TryToSearchValidCredentialL(): %d Credentials found."), credentialPtrsArray.Count()));
        // Take the first valid Credential from found Credentials array.
        TBool credentialOK(EFalse);
        TInt count = credentialPtrsArray.Count();
        for (TInt i=0; i < count; i++)
            {
            SetCredentialPtrL(credentialPtrsArray[0]);//codescannerwarnings
            iValidUntil = iCredentialPtr.Credential()->PropertiesL().ValidUntilL();	//codescannerwarnings
            
            if ( ComputeStatusL() == KSenConnectionStatusReady)
                {
                credentialOK = ETrue;
                break;
                }
            }
            
        if ( credentialOK )
            {
						TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  , _L8("CSenWebServiceSession::TryToSearchValidCredentialL(): Valid Credential were found. CredentialId : %d"),
                            iCredentialPtr.Credential()->IdentifierL().IdL()));	//codescannerwarnings
            retVal = KErrNone;
            }
        else
            {
						TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::TryToSearchValidCredentialL(): Valid Credentials were not found.");
            SetSecurityL(KNullDesC8);
            retVal = KErrNotFound;
            }
        }
    else
        {
				TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenWebServiceSession::TryToSearchValidCredentialL(): Credentials were not found.");
        retVal = KErrNotFound;
        }
        
    CleanupStack::PopAndDestroy(&credentialPtrsArray);
        
    CleanupStack::PopAndDestroy(&list); // Close()
    CleanupStack::PopAndDestroy(pPattern);
    CleanupStack::PopAndDestroy(pCredIdentifier);
    
    return retVal;
    }

EXPORT_C const TTime& CSenWebServiceSession::ValidUntilL()
    {
    if (iValidUntil != Time::NullTTime())
        {
        return iValidUntil;
        }
    else
        {
        //const TTime& MAX_TIME = Time::MaxTTime();
        //return MAX_TIME; // if no expiration was set, the session is
                         // valid forever(!)
         iMaxTime = Time::MaxTTime();
         return iMaxTime;             
                         
        }
    }

EXPORT_C void CSenWebServiceSession::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    _LIT8(KTouch, "touch");
    aWriteStream.WriteL(KSDFramework);
    aWriteStream.WriteL(FrameworkId());
    CSenElement* elem = &AsElement();
    
    const TDesC8* attrVal = elem->AttrValue(KTouch);
    	if(attrVal != NULL)
    	{
    	aWriteStream.WriteL(KSenDblQuot);
    	aWriteStream.WriteL(KSenSpace);
    	aWriteStream.WriteL(KTouch);
    	aWriteStream.WriteL(KSenEqualsDblQuot);
    	aWriteStream.WriteL(*attrVal);	
    	}
    aWriteStream.WriteL(KTagWithAttrEnd);

    // write contract
    if(iContract && iContract->Length()>0)
        {
        aWriteStream.WriteL(KContractStart);
        aWriteStream.WriteL(*iContract);
        aWriteStream.WriteL(KContractEnd);
        }

    // write endpoint
    TPtrC8 cue = TransportCue();
    if(iEndpoint && iEndpoint->Length()>0)
        {
        if( iTransportCue && iTransportCue->Length()>0)
            {
            // both cue and endpoint
            aWriteStream.WriteL(KEndPointStartWithCue);
            aWriteStream.WriteL(*iTransportCue);
            aWriteStream.WriteL(KTagWithAttrEnd);
            aWriteStream.WriteL(*iEndpoint);
            aWriteStream.WriteL(KEndPointEnd);
            
            }
        else
            {
            // just endpoint
            aWriteStream.WriteL(KEndPointStart);
            aWriteStream.WriteL(*iEndpoint);
            aWriteStream.WriteL(KEndPointEnd);
            }
        }
    else if ( iTransportCue && iTransportCue->Length() > 0 )
        {
        // only cue, but no endpoint
        aWriteStream.WriteL(KEndPointStartWithCue);
        aWriteStream.WriteL(*iTransportCue);
        aWriteStream.WriteL(KTagWithAttrClose);
        }

    // write security credentials
    if(iSecurity)
        {
        aWriteStream.WriteL(KCredentialsStart);
        if (iValidUntil != Time::NullTTime())
            {
            HBufC8* dateDes = HBufC8::NewLC(256);
            TPtr8 datePtr = dateDes->Des();
            SenDateUtils::ToXmlDateTimeUtf82L(datePtr, iValidUntil);
            HBufC8* tempBuf = HBufC8::NewLC(KNotOnOrAfterFormat().Length() +
                                            datePtr.Length());

            TPtr8 ptr = tempBuf->Des();
            ptr.Format(KNotOnOrAfterFormat, dateDes);

            aWriteStream.WriteL(ptr);
            CleanupStack::PopAndDestroy(2); // tempBuf, dateDes
            tempBuf = NULL;
            }
        else
            {
            aWriteStream.WriteL(KEmptyTagEnd);
            }

        aWriteStream.WriteL(*iSecurity);
        aWriteStream.WriteL(KCredentialsEnd);
        }

    CSenElement* pProviderPolicyElement =
                            AsElement().Element(KProviderPolicyLocalName);

    if(pProviderPolicyElement)
        {
        HBufC8* pProviderPolicyAsXmlUtf8 = pProviderPolicyElement->AsXmlL();
        if(pProviderPolicyAsXmlUtf8)
            {
            CleanupStack::PushL(pProviderPolicyAsXmlUtf8);
            aWriteStream.WriteL(*pProviderPolicyAsXmlUtf8);
            CleanupStack::PopAndDestroy(); // pProviderPolicyAsXmlUtf8
            }
        }

    WriteExtensionsAsXMLToL(aWriteStream);
    
  //Writing the ServicePolicy to XML  
    MSenServicePolicy* servicePolicy = ServicePolicy();
    if(servicePolicy)
        {
        CSenServicePolicy* ele = (CSenServicePolicy*)servicePolicy;
        CSenElement* pServicePolicyElement = &ele->AsElement();          
        if(pServicePolicyElement)
            {
            HBufC8* pServicePolicyAsXmlUtf8 = pServicePolicyElement->AsXmlL();
            if(pServicePolicyAsXmlUtf8)
                {
                CleanupStack::PushL(pServicePolicyAsXmlUtf8);            
                aWriteStream.WriteL(*pServicePolicyAsXmlUtf8);
                CleanupStack::PopAndDestroy(); // pProviderPolicyAsXmlUtf8
                }
            }
        }            

    aWriteStream.WriteL(KSDEnd);
    aWriteStream.WriteL(KNewLine);



    }



EXPORT_C TInt CSenWebServiceSession::SubmitSoapL(const TDesC8& aSoapMessage, 
                                                 const TDesC8& aTransportProperties,
                                                 MSenRemoteServiceConsumer& aConsumer,
                                                 HBufC8*& aResponse)
    {
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
    TPtrC8 endpoint  = Endpoint();
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"////////////////////////////////////////////////////////");
    CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Endpoint: %S"), &endpoint));
    CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Message (%d bytes) about to submit:"), aSoapMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,( aSoapMessage ));
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"////////////////////////////////////////////////////////");
#endif

    MSenTransport& t = aConsumer.TransportL();

    TInt retVal = t.SubmitL(endpoint, aSoapMessage, aTransportProperties, aResponse, aConsumer);

    if(!aResponse)
        {
        // Response was NULL: probably either out
        // of heap or some transport malfunction.
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Received NULL from transport.");
        return retVal;
        }
    else if(aResponse->Length() < KSenSoapEnvelopeName().Length()*2)
        {
        // No use parsing, Envelope -root element not there.
        // Just deliver this "non-soap" response to consumer
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Response is not a SOAP envelope.");
        return retVal;
        }

    // Attempt to parse the response here. SOAP faults 
    // are to be searched after,
    CSenSoapMessage* pResponseSoapMsg = NULL;
    TInt leaveCode(KErrNone);
    TInt parseRetCode(KErrNone);
    TRAP( leaveCode, (parseRetCode =
        ParseResponseL(*aResponse, pResponseSoapMsg)) );

    if(leaveCode!=KErrNone)
        {
        // Return directly the response, which was received from
        // transport. Note that pResponseSoapMsg has already
        // been deleted because of leave
        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8(" - ParseResponseL leaved: %d"), leaveCode ));
        
        if(retVal==KErrNone) // retVal still SubmitL() return value
            {
            // Indicate with return value, that response is
            // invalid - even though submit was ok, the
            // response could NOT be parsed(!).
            retVal = leaveCode;
            }
        }
    else // ParseResponseL did not leave..
        {
        if(parseRetCode != KErrNone) // .. but returned an error
            {
            // Not mandatory, ParseResponseL should take 
            // care of gc, if it returns an error:
             delete pResponseSoapMsg; 
             CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");

            CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- Parsing failed, error: %d"),
                    parseRetCode));

            if(retVal==KErrNone) // submit was ok
                {
                // Indicate with return value, that response is
                // invalid - even though submit was ok, the
                // response could NOT be parsed!
                retVal = parseRetCode;
                }
            }
        else // .. and parsing was successful
            {
            CleanupStack::PushL(pResponseSoapMsg);
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Response is a SOAP message");

            TBool completeServerMessages(ETrue);
            HasFacetL(KCompleteMessagesFacet, completeServerMessages);

            // response is OK and in SOAP form.
            if(pResponseSoapMsg->IsFault())
                {
                // Response is a SOAP fault
                retVal = KErrSenSoapFault; // might change, if framework handles this fault
                CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");

                CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Response is a SOAP fault. Calling HandleErrorL.");

                // Check if this SOAP fault could be handled by the framework
                HBufC8* pResponse = NULL;
                CleanupStack::Pop(pResponseSoapMsg);
                retVal = HandleSoapFaultL(pResponseSoapMsg, pResponse/*, aResponseTransportProperties */);

                if(retVal == KErrSenSoapFault)
                    {
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
                    if(completeServerMessages)
                        {
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Returning SOAP envelope with fault");
						// The following lines are important when there is a resending
                        // the new response is copied to aResponse.
                        // When there is no resend, then pResponse = NULL no copy happens                     
                        if(pResponse)
                        	{
                        	delete aResponse;
                        	aResponse = pResponse;
                        	}                        // aResponse will be returned (original response from transport)
                        }
                    else // Pass the detached fault
                        {
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Returning detached a SOAP fault.");
                        // Destroy the original response buffer received from transport,
                        // since now the parsed SOAP envelope can be returned
                        delete aResponse;
                        aResponse = pResponse;
                        }
                    }
                else if(retVal == KErrNone)
                    {
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Framework handled this SOAP fault.");
                    // KErrNone means that fault was handled
                    // properly (by framework spesific code)
                    // and consumer may receive the response 
                    // it requested.

                    // Currently there are no SOAP faults handled
                    // in WebServiceSession class, so KErrNone is
                    // never returned. This if -section is here 
                    // only for *frameworks to extend*.

                    // Destroy the original response buffer received from transport,
                    // since now the parsed SOAP envelope can be returned
                    delete aResponse;
                    aResponse = pResponse;
                    }
                else if(retVal==KErrNotFound)
                    {
                    // SOAP fault element  could not be found
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL - MAJOR:");
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"-IsFault()==true BUT fault element NOT found!");
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"-returning KErrSenBrokenSoapFault");
                    retVal = KErrSenBrokenSoapFault;
                    // Note: 2005 change: now the broken (original) response
                    // from the transport is returned, instead of NULL
                    }
#ifdef _SENDEBUG
                else 
                    {
                    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
                    CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- HandleErrorL failed: %d"), retVal));
                    }
#endif // _SENDEBUG
                } 
            else // this SOAP envelope is NOT a fault
                {
                CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
                CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Response is a SOAP envelope.");

                // Check complete server messages on/off
                if(!completeServerMessages)
                    {
                    HBufC8* pBody = pResponseSoapMsg->BodyAsStringL();
                    if(pBody)
                        {
                        // Destroy the original response buffer received from transport,
                        // since now the parsed SOAP envelope can be returned
                        delete aResponse;
                        aResponse = pBody;
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Detached SOAP message body.");
                        }
                    else
                        {
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SubmitSoapL:");
                        CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- Could not detach SOAP body(!) - return KErrSenNoSoapBody");
                        // Note: 2005 change: now the broken (original) response
                        // from the transport is returned, instead of NULL
                        retVal = KErrSenNoSoapBody;
                        }
                    }
                
                CleanupStack::PopAndDestroy(pResponseSoapMsg);
                }
                // else { // return complete server message }
                
            }
        }

#ifdef _SENDEBUG
    if(aResponse)
        {
        
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"------------------------------------------------------------");
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"CSenWebServiceSession::SubmitSoapL:");
        CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Response (%d bytes):"), aResponse->Length()));
        CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,( *aResponse ));
        CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"------------------------------------------------------------");
        }
#endif
    return retVal;
    }

EXPORT_C TInt CSenWebServiceSession::SendSoapL( const TDesC8& aSoapMessage, 
                                                const TDesC8& aTransportProperties,
                                                MSenRemoteServiceConsumer& aConsumer,
                                                TInt& aTxnId,
                                                HBufC8*& /* aRevalidationError*/ ) // SIFs should utilize aRevalidationError
    {
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendSoapL:");
    TPtrC8 endpoint  = Endpoint();
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"////////////////////////////////////////////////////////");
    CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Endpoint: %S:"), &endpoint));
    CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Message (%d bytes) about to send:"), 
                    aSoapMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,( aSoapMessage ));
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"////////////////////////////////////////////////////////");
#endif

    MSenTransport& t = aConsumer.TransportL();
    TInt retVal( t.SendL(endpoint, aSoapMessage, aTransportProperties, *this, aConsumer, aTxnId) );
    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenWebServiceSession::SendSoapL:");

    CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel, _L8("- Received transaction ID: %d"), aTxnId));
    return retVal;
    }



// NOTE: the ownership of the consumer-pointer(s), which are copied (appended)
// into aConsumers array is NOT transfered to the caller!

// 2005: refactored GetConsumers() to Consumers()
EXPORT_C TInt CSenWebServiceSession::Consumers(RServiceConsumerArray& aConsumers) const
    {
    TInt count(iConsumerList.Count());
        {
        for(TInt i=0; i<count; i++)
            {
            // creates a copy of pointers, but ownership is NOT transferred!
            TInt error = aConsumers.Append(iConsumerList[i]);
            if(error != KErrNone)
                return error;
            }
        }
        return KErrNone;
    }

EXPORT_C TTimeIntervalMicroSeconds
                                CSenWebServiceSession::ClientServerInterval()
    {
    return iClientServerInterval;
    }

EXPORT_C void CSenWebServiceSession::SetClientServerIntervalL(
                                        TTimeIntervalMicroSeconds aInterval)	//codescannerwarnings
    {
    iClientServerInterval = aInterval;
    if ( iCredentialPtr.Credential() )
        {
        TBuf8<64> buf;
        buf.AppendNum(iClientServerInterval.Int64());
        iCredentialPtr.Credential()->PropertiesL().SetPropertyL(KServiceInterval1, buf);	//CodeScannerWarnings
        }
    }

EXPORT_C TBool CSenWebServiceSession::HasConsumer() const
    {
    if (iConsumerList.Count() > 0) 
        {
        return ETrue;
        }
    else 
        {
        return EFalse;
        }
    }

EXPORT_C void CSenWebServiceSession::StartTransaction()
    {
    // Nothing in framework base class level
    }

EXPORT_C void CSenWebServiceSession::TransactionCompleted()
    {
    // Nothing in framework base class level
    }

EXPORT_C TBool CSenWebServiceSession::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::EServiceSession ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        { 
        // Otherwise, ask from direct superclass (may invoke chain of recursive calls)
        return CSenServiceSession::HasSuperClass( aType ); 
        }
    }

EXPORT_C TInt CSenWebServiceSession::SetTransportCueL(const TDesC8& aCue)
    {
    HBufC8* pNew =NULL;
    if(aCue!=KNullDesC8)
        {
        pNew = aCue.AllocL();
        }
    delete iTransportCue;
    iTransportCue = pNew;
    
    return KErrNone;
    }

EXPORT_C HBufC8* CSenWebServiceSession::SecurityL()
    {
    if ( HasSecurity() )
        {
        return iCredentialPtr.Credential()->AsXmlL();
        }
    else
        {
        return NULL;
        }
    }
    
EXPORT_C TBool CSenWebServiceSession::HasSecurity()
    {
    if ( iCredentialPtr.Credential() )
        {
        return ETrue;
        }
    
    return EFalse;
    }
    
EXPORT_C void CSenWebServiceSession::SetCredentialPtrL(RSenCredentialPtr aCredentialPtr)	//codescannerwarnings
    {
    // Stop observing the removal of credential as this instance is itself doing the removal
    iCredentialPtr.RemoveCredentialObserver(*this);
    //temp pointer eliminate self deleting when counter = 1 and aCredentail points same data as iCredentialPtr
    // in other words there is a chance that closing iCredentialPtr make dirty aCredentialPtr. Temp ptr keeps balance so 
    //  that counter has proper value.
    RSenCredentialPtr tempPtr = iCredentialPtr.Clone();
    iCredentialPtr.Close();
    iCredentialPtr = aCredentialPtr.Clone();
    tempPtr.Close();    
    // Start observing the new credential
    iCredentialPtr.AddCredentialObserverL(*this);	//codescannerwarnings
    SetCredentialIdL(iCredentialPtr.Credential()->IdentifierL().IdL());	//codescannerwarnings
    }

EXPORT_C TInt CSenWebServiceSession::SendToHostletL(MSenRemoteHostlet& aReceiver,
                                                 const TDesC8& aMessage,
                                                 const TInt aTxnId,
                                                 MSenRemoteServiceConsumer& aFrom,
                                                 MSenProperties* aProperties)
    {
    // default impelementation routes the request directly to the hostlet
    // Handler aware framework could load the required handlers in here
    // to enable addressing / message correlation etc.
    return CSenServiceSession::SendToHostletL(aReceiver, aMessage, aTxnId, aFrom, aProperties); 
    }

EXPORT_C TInt CSenWebServiceSession::ProvideHostletResponseL( MSenRemoteHostlet& aProvider,
                                                              const TInt aTxnId,
                                                              const TInt aServiceCode,
                                                              const TDesC8& aRecipientsConsumerId,
                                                              CSenChunk& aMessageChunk)
    {
    return CSenServiceSession::ProvideHostletResponseL( aProvider, 
                                                        aTxnId, 
                                                        aServiceCode, 
                                                        aRecipientsConsumerId, 
                                                        aMessageChunk ); 
    }
    
TInt CSenWebServiceSession::HandleBodyWithoutParsingL(CSenSoapMessage& aMessage,
                                                      const TDesC8& aResponse)
    {
    TInt ret;
    TInt endTagStart;
    TInt endTagEnd;
    TInt startTagStart;
    TInt startTagEnd;
    TPtrC8 prefix;

    ret = SenSaxUtils::SearchEndTagL(aResponse,KSenSoapEnvelopeBodyName,
                                     endTagStart, endTagEnd, prefix);
    if ( ret != KErrNotFound )
        {
        ret = SenSaxUtils::SearchStartTagL(aResponse,prefix,KSenSoapEnvelopeBodyName,
                                           startTagStart, startTagEnd);
        if ( ret != KErrNotFound )
            {
            TPtrC8 startPart(aResponse.Ptr(),startTagEnd+1);
            TPtrC8 endPart(aResponse.Ptr()+endTagStart,aResponse.Size()-endTagStart);
            HBufC8* pXmlWithoutBody = HBufC8::NewLC(startPart.Length()+endPart.Length());
            pXmlWithoutBody->Des().Append(startPart);
            pXmlWithoutBody->Des().Append(endPart);
            
            aMessage.ParseL(*pXmlWithoutBody);
            CleanupStack::PopAndDestroy(pXmlWithoutBody);
            
            TPtrC8 bodyContent(aResponse.Ptr()+startTagEnd+1, endTagStart-startTagEnd-1);
            aMessage.SetBodyL(bodyContent);
            }
        }
    
    return ret;
    }

EXPORT_C void CSenWebServiceSession::CredentialChanged(TSenCredentialChange aChange,
		                                               TAny* /* aPointer */)
    {
    if ( aChange == MSenCredentialObserver::EDestroyed )
        {
        iValidUntil.Set(KInvalideDate); // way back in history: January 1st 1900  \\CodeScannerWarnings
        TRAP_IGNORE( SetStatusL(); ) 
        }
    }
    
EXPORT_C  TInt CSenWebServiceSession::AddCredentialObserverL(CSenInternalCredential& aCredential)
{
	TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,_L("CSenWebServiceSession::AddCredentialObserverL()"));
	TInt error(KErrNone);
	RSenCredentialPtr credentialPtr = 
	((MSenServiceManager&)iFramework.Manager()).CredentialL(
	    aCredential.IdentifierL().IdL(), error);
	if ( error == KErrNone )
	{
		iCredentialPtr.RemoveCredentialObserver(*this);
		iCredentialPtr.Close();
		iCredentialPtr = credentialPtr.Clone();
		iCredentialPtr.AddCredentialObserverL(*this);
		iValidUntil = credentialPtr.Credential()->PropertiesL().ValidUntilL();
		if ( iClientServerInterval == 0 )
		{
			TPtrC8 value;
			TInt ret = iCredentialPtr.Credential()->PropertiesL().PropertyL(_L8("ServiceInterval"),
			                                                               value);
			if ( ret == KErrNone )
			{
				TLex8 lex(value);
				TInt64 val;
				ret = lex.Val(val);
				if ( ret == KErrNone )
				{
					iClientServerInterval = val;
				}
			}
		}
                
	}
    CredentialChanged(MSenCredentialObserver::EAdded, NULL);      
	SetStatusL();
	TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,_L("CSenWebServiceSession::AddCredentialObserverL() Completed"));
	return KErrNone;
}

//  End of File
