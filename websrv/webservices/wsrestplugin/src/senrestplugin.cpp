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








#include "senrestplugin.h"
#include "MSenServiceDescription.h"
#include "SenServiceConnection.h" // framework IDs, error codes..
#include "msencoreservicemanager.h"
#include "senrestservicesession.h"
#include "sendebug.h"
#include "SenFacet.h" 
#include "senlogger.h" 

#include <SenHttpTransportProperties.h>


namespace
    {
    _LIT8(KFrameworkLocalname,   "Framework");
#ifdef _SENDEBUG
        const TInt KFLATBUFSIZE = 64;
#endif // _SENDEBUG
    _LIT8(KTextWwwFormContentType,  "application/x-www-form-urlencoded; charset=UTF-8");
    }

// Create instance of concrete ECOM interface implementation
CSenRestPlugin* CSenRestPlugin::NewL(TAny* aManager)
    {
    MSenCoreServiceManager* manager =
        reinterpret_cast<MSenCoreServiceManager*>(aManager);
    
    CSenRestPlugin* self = new (ELeave) CSenRestPlugin(*manager);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Constructor
CSenRestPlugin::CSenRestPlugin(MSenCoreServiceManager& aManager)
    : iManager(aManager)
    {
    }

CSenRestPlugin::~CSenRestPlugin()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenRestPlugin::~CSenRestPlugin()")));
    }

// Second phase construction.
void CSenRestPlugin::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenRestPlugin::ConstructL")));
    
    BaseConstructL(KRestFrameworkXMLNS, KFrameworkLocalname);
    }

/*RFileLogger* CSenRestPlugin::Log() const
    {
    return iManager.Log();
    }
*/


TInt CSenRestPlugin::RegisterServiceDescriptionL(MSenServiceDescription& aServiceDescription)
    {
    TInt retVal(KErrNone);

#ifdef _SENDEBUG
    TPtrC8 contract = aServiceDescription.Contract();
    TPtrC8 endpoint = aServiceDescription.Endpoint();
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenRestPlugin::RegisterServiceDescriptionL");
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - creating session from description.");
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" - Endpoint: '%S'"), &endpoint));
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" - Contract: '%S'"), &contract));
#endif

    if( aServiceDescription.Endpoint().Length() == 0 )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - Endpoint is zero-lenght(!), cannot register such SD.");
        return KErrSenNoEndpoint;
        }


    // REST sessions are matched by consumer id.
    // This way we can avoid session sharing/multiple consumers
     
    CSenRestServiceSession* pSession = CSenRestServiceSession::NewLC(*this);
    retVal = pSession->InitializeFromL(aServiceDescription);


    if(retVal == KErrNone)
        {
        // Takes the ownership of this session
        retVal = Manager().AddServiceDescriptionL(pSession);
        CleanupStack::Pop(pSession);
        }
    else
        {
        CleanupStack::PopAndDestroy(pSession);
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenRestPlugin::RegisterServiceDescriptionL:");
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Session->InitializeFromL failed: %d"), retVal));
        }
    return retVal;
    }

TInt CSenRestPlugin::UnregisterServiceDescriptionL(MSenServiceDescription& aServiceDescription )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenRestPlugin::UnregisterServiceDescriptionL");
    TInt retVal(KErrNone);
    CSenRestServiceSession *pSession = NULL;
        
    if(aServiceDescription.DescriptionClassType() == MSenServiceDescription::ERestServiceSession)
        {
        pSession = (CSenRestServiceSession*) &aServiceDescription;
        retVal = Manager().RemoveServiceDescriptionL(*pSession);
        }
     else
     {
#ifdef _SENDEBUG
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"Creating session from description...");
            TPtrC8 con = aServiceDescription.Contract();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("-contract: '%S'"), &con));
#endif // _SENDEBUG

            pSession = CSenRestServiceSession::NewLC(*this);
            retVal = pSession->InitializeFromL(aServiceDescription);
            if (retVal == KErrNone)
                {
                retVal = Manager().RemoveServiceDescriptionL(*pSession);
                }
            CleanupStack::PopAndDestroy(pSession);     	
     }
    return retVal;
    }
    


const TDesC8& CSenRestPlugin::Id()
    {   
    return KDefaultRestServicesFrameworkID(); // "REST"
    }

TInt CSenRestPlugin::AddServiceDescriptionL( MSenServiceDescription& aPattern,
                                             MSenRemoteServiceConsumer& /* aRemoteConsumer */,
                                             HBufC8*& /* aErrorMsg */ )
    {

    // Currently, accept only SDs, which explicitly declare REST framework ID.
    if(aPattern.FrameworkId() != KDefaultRestServicesFrameworkID)
        {
        return 0;
        }

    TPtrC8 endpoint = aPattern.Endpoint();
    if( endpoint.Length() > 0 )
        {
        CSenRestServiceSession* pNewSession =
            (CSenRestServiceSession*) NewServiceDescriptionL();
        CleanupStack::PushL(pNewSession);

        // InitializeFromL call copies the contract and endpoint, 
        // and possibly any facets set by Service Management API:
        TInt retVal = pNewSession->InitializeFromL( aPattern );
        if( retVal != KErrNone )
            {
            CleanupStack::PopAndDestroy();
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenRestPlugin::AddServiceDescriptionL:");
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- New session->InitializeFromL failed: %d"), retVal));
            return 0;
            }
        else // init ok
            {
            // Core / XML DAO takes ownership of new session:
            // - if duplicate (equal primary keys) exists, it is deleted
            Manager().AddServiceDescriptionL(pNewSession); 
            CleanupStack::Pop(pNewSession);
            return 1; // one new session added
            }
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - No endpoint; new session could not be added.");
        return 0; // zero added
        }



    // alternative version:
    /*
    CSenServiceSession* pNewSession = NULL;
    HBufC8* pErrMsg = NULL;
    TInt retVal = CreateServiceSessionL(aPattern, aPattern, 
                      pNewSession, aRemoteConsumer, pErrMsg);
    delete pErrMsg; 

    if(retVal==KErrNone)
        {
        return 1; // one added;
        }
    else
        {
        return 0; // zero added;  // no endpoint for REST service session
        }
    */
    }

TInt CSenRestPlugin::CreateServiceSessionL( MSenServiceDescription& aServiceDescription,
                                            MSenServiceDescription& /* aPattern */,
                                            CSenServiceSession*& aNewSession,
                                            MSenRemoteServiceConsumer& /* aRemoteConsumer */,
                                            HBufC8*& /* aErrorMsg */ )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenRestPlugin::CreateServiceSessionL");
    CSenRestServiceSession* pNewSession = NULL;

    TInt retVal(KErrNone);
    if(aServiceDescription.DescriptionClassType() == MSenServiceDescription::ERestServiceSession)
        {
        pNewSession = (CSenRestServiceSession*) &aServiceDescription;
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Service description is already a REST service session");
        //pNewSession->CopyFacetsFromL(aPattern); // not performed with REST
        retVal = KErrNone;
        }
    else
        {
        TPtrC8 endpoint = aServiceDescription.Endpoint();
        if( endpoint.Length() > 0 )
            {
            pNewSession = (CSenRestServiceSession*) NewServiceDescriptionL();
            CleanupStack::PushL(pNewSession);

            // InitializeFromL call copies the contract and endpoint:
            retVal = pNewSession->InitializeFromL(aServiceDescription);

#ifdef _SENDEBUG
            CBufFlat* pBuf = CBufFlat::NewL( KFLATBUFSIZE );
            CleanupStack::PushL(pBuf);
            RBufWriteStream writeStream;
            writeStream.Open(*pBuf);
            CleanupClosePushL(writeStream);
            pNewSession->WriteAsXMLToL(writeStream);
            TPtr8 ptr = pBuf->Ptr(0);
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- New REST service session as XML:");
            TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,( ptr ));
            CleanupStack::PopAndDestroy(2); // writeStream.Close(), pBuf
#endif // _SENDEBUG        
            
            if( retVal == KErrNone ) // initialize from pattern OK
                {
                // NOTE: Core ALWAYS takes ownership of new session:
                // - if some session with equal primary keys exists, it is deleted
                retVal = Manager().AddServiceDescriptionL(pNewSession); // 2006-07-06: now retVal is taken in use in here(!)
                CleanupStack::Pop(pNewSession);
#ifdef _SENDEBUG
                if(retVal!=KErrNone)
                    {
                    delete pNewSession;
                    pNewSession = NULL;
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenRestPlugin::CreateServiceSessionL:");
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8(" - CoreServiceMngr::AddServiceDescriptionL returned: %d"), retVal));
                    }
#endif
                }
            else // initialize from pattern failed!
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenRestPlugin::CreateServiceSessionL:");
                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- New session->InitializeFromL failed: %d"), retVal));
                CleanupStack::PopAndDestroy(pNewSession);
                }
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," - No endpoint - ; returning KErrSenNoEndpoint");
            retVal = KErrSenNoEndpoint; // no endpoint for REST web service session
            }
        }
    aNewSession = pNewSession; 
    return retVal;
    }

TInt CSenRestPlugin::OnEvent(const TInt /* aEvent */,
                            TAny* /* apArgument */)
    {
    //LOG_WRITE_L("CSenRestPlugin::OnEvent");
    //LOG_WRITEFORMAT((_L8("-event code: %d"), aEvent));
    TInt retVal(KErrNone);

    return retVal;
    }

TAny* CSenRestPlugin::ExtendedInterface()
    {
    // There is no extended interface in REST framework at the moment
    return NULL;
    }

CSenWSDescription* CSenRestPlugin::NewServiceDescriptionL()
    {
    CSenRestServiceSession* pSD =
        CSenRestServiceSession::NewLC(*this);
    pSD->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    CSenFacet* pCompleteOn = CSenFacet::NewL();
    CleanupStack::PushL(pCompleteOn);
    pCompleteOn->SetNameL(KCompleteMessagesFacet);
    pSD->SetFacetL(*pCompleteOn);
    CleanupStack::PopAndDestroy(pCompleteOn);
    CleanupStack::Pop(pSD);
    return pSD;
    }

/**
* Return the ServiceManager for which this instance is working.
* This is used by the ServiceSession instances created by this framework.
*
* Note, in Sen, MSenCoreServiceManager so all Frameworks
* return their "owner" - i.e. - the only service manager in Sen Framework
*
* @return (MSenCoreServiceManager)
*/

MSenCoreServiceManager& CSenRestPlugin::Manager()
    {
    return iManager;  
    }

/**
* Set the ServiceManager to which this instance reports.
* The (singleton) ServiceManager calls this method when
* it instantiates a framework.
*
* @param aServiceManager The singleton ServiceManager in the system.
*/
void CSenRestPlugin::SetManager(MSenCoreServiceManager& aServiceManager)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenRestPlugin::SetManager(.)")));
    iManager = aServiceManager;
    }

/**
* Return an object that can do SAX based parsing of a XML fragment
* that contains framework specific configuration information.
* The ServiceManager asks for this BaseFragment upon startup
* in order to give this framework a chance to read its configuration
* information from the central ServiceManager configuration document.
*
* @return (CSenBaseFragment)
*/
CSenBaseFragment& CSenRestPlugin::ConfigParser()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CSenRestPlugin::ConfigParser()")));
    return *this;
    }

/**
* Write the ServiceInvocationFramework configuration as XML to a HBufC8.
*
* This method is invoked by the ServiceManager when it saves
* its state and/or configuration.
*
* Upon startup the chunk of XML that is written will be passed back to
* the configuation parser of the framework.
*
* The "schema" that shall be adhered to is:
* <pre>
* &lt;element name="Framework"&gt;
*       &lt;any/&gt;&lt;!-- this is where implementations write additional
*       information --&gt;
*       &lt;attribute name="class" type="String"&gt;
*    &lt;!-- In Symbian the class attribute is used as cue for
*    the ECOM Resolver --&gt;
* &lt;/element&gt;
* </pre>
*
* @param aTo - a HBufC8 to which the framework should write its configuration
*/


void CSenRestPlugin::EndElementL(const TDesC8& aNsUri,
                                const TDesC8& aLocalName,
                                const TDesC8& aQName)
    {
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("REST plugin @ end of element: %S (xmlns='%S', localname='%S')"),
        &aQName, &aNsUri, &aLocalName));
    CSIF::EndElementL(aNsUri,aLocalName,aQName);
    }

// over-ride CSenBaseFragment
HBufC* CSenRestPlugin::AsXmlUnicodeL()
    {
    HBufC8* pUtf8 = AsXmlL();
    CleanupStack::PushL(pUtf8);
    HBufC* pAsXmlInUnicode = SenXmlUtils::ToUnicodeLC(*pUtf8);
    CleanupStack::Pop(pAsXmlInUnicode);
    CleanupStack::PopAndDestroy(pUtf8);
    return pAsXmlInUnicode;
    }

HBufC8* CSenRestPlugin::AsXmlL()
    {
    _LIT8(KLessFramework,"<Framework ");//CodeScannerWarning
    _LIT8(KXmlns,"xmlns=\"");
    _LIT8(KQuot,"\" ");
    _LIT8(KClassQuot,"class=\"");
    _LIT8(KQuotNewLine,"\"/>\n");
    HBufC8* pAsXml = HBufC8::NewLC(256);
    TPtr8 ptr = pAsXml->Des();
    ptr.Append(KLessFramework);
    ptr.Append(KXmlns);
    ptr.Append(KRestFrameworkXMLNS);
    ptr.Append(KQuot);
    
    ptr.Append(KClassQuot);
    ptr.Append(KRestFrameworkCue);
    ptr.Append(KQuotNewLine);
    CleanupStack::Pop(pAsXml);
    return pAsXml;
    }

TInt CSenRestPlugin::SetTransportPropertiesL(MSenTransport& aTransport)
    {
    // Set default REST transport properties
    CSenHttpTransportProperties* pProps = CSenHttpTransportProperties::NewLC();
    pProps->SetContentTypeL(KTextWwwFormContentType);
    HBufC8* pPropsAsXml = pProps->AsUtf8L();
    CleanupStack::PopAndDestroy(pProps);

    CleanupStack::PushL( pPropsAsXml );
    TInt retVal = aTransport.SetPropertiesL( *pPropsAsXml, MSenLayeredProperties::ESenFrameworkLayer, NULL );
    CleanupStack::PopAndDestroy( pPropsAsXml );
    return retVal;    
    }
    
// END OF FILE


