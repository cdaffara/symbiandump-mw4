/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "senConnectionTester.h"

//  EXTERNAL INCLUDES
#include <http.h>
#include <httperr.h>

#include <SenBaseFragment.h>
#include <SenBaseElement.h>
#include <SenDomFragment.h>
#include <SenServiceConnection.h>
#include <SenServicePattern.h>
#include <SenSoapEnvelope.h>
#include <SenSoapEnvelope2.h>
#include <SenXmlReader.h>
#include <SenXmlServiceDescription.h>
#include <SenXmlUtils.h>
#include <SenServiceManager.h>
#include <SenIdentityProvider.h>
#include <SenHttpTransportProperties.h>
#include <SenSoapMessage.h>
#include <SenSoapMessage2.h>
#include <SenDateUtils.h>
#include <SenTransportProperties.h>

#include <senvtcptransportproperties.h>

#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengserializer.h>

#include <e32base.h> // for CActive
#include <aknnotewrappers.h> 

#include <commdb.h>

#include <s32file.h>    // filestream
#include <e32svr.h>     // fileman
// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
namespace 
    {
        
#ifdef AOL_RADIO_SERVICE
    // AOL redio.etenee.net:9080
    _LIT8(KWSPContract,         "urn:aol-com:services:radio");
    _LIT8(KAuthServEndpoint,    "http://radio.etenee.net:9080/tfs/IDPSSO_IDWSF");
    _LIT8(KASProviderID,        "http://radio.etenee.net:9080/tfs/");
    _LIT8(KTestAuthzID,         "012345678901234");
    _LIT8(KTestPassword,        "012345678901234");
#elif VALIMO_ADDRESSBOOK_SERVICE
    // Addressbook service hosted inside Nokia intra by ASP / testing team
    _LIT8(KWSPContract,         "urn:nokia:test:addrbook:2004-09");
    _LIT8(KAuthServEndpoint,    "http://10.21.32.110/tfs/IDPSSO_IDWSF10");
    _LIT8(KASProviderID,        "provider01");
    _LIT8(KTestAuthzID,         "john");
    _LIT8(KTestPassword,        "password");
#elif SYSOPENDIGIA_ADDRESSBOOK_SERVICE
    
    _LIT8(KWSPContract,         "urn:nokia:test:addrbook:2004-09");
    _LIT8(KAuthServEndpoint,    "http://ys01liw022.partner.yomi.com:9080/tfs/IDPSSO_IDWSF");
    _LIT8(KASProviderID,        "http://ys01liw022.partner.yomi.com:9080/tfs/");
    _LIT8(KTestAuthzID,         "012345678901234");
    _LIT8(KTestPassword,        "012345678901234");
#else // == default (no MACROs defined in .mmp)
    // Addressbook service hosted by Forum Nokia (accessible from external network)
    _LIT8(KWSPContract,         "urn:nokia:test:addrbook:2004-09"); 
    _LIT8(KAuthServEndpoint,    "http://selma.ndhub.net:9080/tfs/IDPSSO_IDWSF");
    _LIT8(KASProviderID,        "http://selma.ndhub.net:9080/tfs/");
    _LIT8(KTestAuthzID,         "testuser1");
    _LIT8(KTestPassword,        "testuser1");
#endif // FN_ADDRESSBOOK_SERVICE


    _LIT8(KASContract,              "urn:liberty:as:2004-04");
    _LIT8(KHttpsEndPoint,           "https://10.21.32.110/sereneHardCoded/WS_WSI_R9980_001" );    
    
    _LIT8(KWSIFrameworkID,          "WS-I");
    _LIT8(KIDWSFFrameworkID,        "ID-WSF");
    _LIT8(KRESTFrameworkID,         "REST");       
//    _LIT8(KWsStarFrameworkID,       "WS-STAR");       
#ifdef SYSOPENDIGIA_ADDRESSBOOK_SERVICE
    _LIT8(KAddressBookServiceId,    "http://radio.aol.com/xsd/2.0/ns/");
    _LIT8(KAddressBookContract,     "http://radio.aol.com/xsd/2.0/ns/");
#else
    _LIT8(KAddressBookServiceId,    "urn:nokia:test:addrbook:2004-09");
    _LIT8(KAddressBookContract,     "urn:nokia:test:addrbook:2004-09");
#endif
    
#ifdef __WINS__    
  #ifdef SYSOPENDIGIA_ADDRESSBOOK_SERVICE
    _LIT(KIAPName,                  "Ethernet No Daemon Static IP");
  #else
    //_LIT(KIAPName,                  "Ethernet No Daemon Static IP");
    _LIT(KIAPName,                  "Ethernet with Daemon Dynamic IP");
  #endif
#else
// ARMv5
    _LIT(KIAPName,                  "Internet");
#endif  
    _LIT8(KLoggerFileNameProperty,  "HttpLoggerFileName");

    _LIT(KFileToRead,               "e:\\Logs\\LoggingHttpFilter\\httptrace.xml");
    _LIT8(KFileName,                "httptrace.xml");

    _LIT8(KHTTPMethodGet,           "GET");
    _LIT8(KHTTPMethodPost,          "POST");
    
//    const TBool MultipleSimultanousRHttpSessionsSupported = EFalse;
    // If IAP changes all previously sent messages will be canceled.
    
    
    //_LIT8(KProxyHost,             "esprx00.nokia.com"); 
    _LIT8(KProxyHost,               "172.19.160.50"); // IP address to: bsprx01.americas.nokia.com
    const TInt KProxyPort =         8080;

    //MSN Constants :
    //---------------
    _LIT8(KPassportUser,        "m_minkkis@hotmail.com");
    _LIT8(KPassportPass,        "M1nkk1s");

    _LIT8(KStsEndpoint,         "https://login.live.com/rst2.srf");
    _LIT8(KMSNProxy,            "tcp.mws.beta.mobile.live.com");
    //_LIT8(KMetadataEndpoint,    "http://10.132.11.31/WSStar/secpolicy/secpol.xml");
    _LIT8(KMetadataEndpoint, "https://http.mws.mobile.live.com/2006/10/MWP2007_02/SecurityPolicy/Default.aspx");   

    _LIT8(KPolicyAdr,           "Addressing");
    _LIT8(KPolicyMT,            "MetadataEndpoint");
    _LIT8(KPolicyTLS,           "UsernameTokenOverTLS");
    _LIT8(KPolicySOAP12,        "SOAP12");
    _LIT8(KPolicyPassExt,       "PassportExtensions");
    _LIT8(KProviderId,          "provider01");

#ifdef OLD_MSN_NAMESPACE_2005_08
    _LIT8(KMessagingEndpoint,   "tcp://schemas.live.com/mws/2005/08/messaging");
    _LIT8(KMessagingContract,   "http://schemas.live.com/mws/2005/08/messaging");
    _LIT8(KMessagingNameSpace,  "http://schemas.live.com/mws/2005/08/core");
#else
    _LIT8(KMessagingEndpoint,   "tcp://schemas.live.com/mws/2006/10/messaging");
    _LIT8(KMessagingContract,   "http://schemas.live.com/mws/2006/10/messaging");
    _LIT8(KMessagingNameSpace,  "http://schemas.live.com/mws/2006/10/core");
#endif
    }

// MACROS
//#define ?macro ?macro_def
#define STIFF_ASSERT_NOT_EQUALS_DESC(expression1, expression2, desc) \
	{TL((expression1) != (expression2)){RDebug::Print(_L(desc)) ;}}

#define STIFF_ASSERT_DESC(expression, desc)	{if(!(expression)){RDebug::Print(_L(desc)) ;return KErrArgument;}}
#define LOCAL_ASSERT(expression)	{if(!(expression)){return KErrArgument;}}

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CsenConnectionTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CsenConnectionTester::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CsenConnectionTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CsenConnectionTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		ENTRY("RegisterIdentityProviderL_CSenServiceConnection_RegisterIdentityProviderL",CsenConnectionTester::UT_CSenServiceConnection_RegisterIdentityProviderLL),
		ENTRY("NewL-contracttest_CSenServiceConnection_NewLcontract",CsenConnectionTester::UT_CSenServiceConnection_NewL_ContractL),
		ENTRY("SendL,Pattern,IAP1Test_CSenServiceConnection_SendL,Pattern,IAP1Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_Pattern_IAP1L),
		ENTRY("SendL,Pattern,IAP2Test_CSenServiceConnection_SendL,Pattern,IAP2Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_Pattern_IAP2L),
		ENTRY("SendL,Pattern,IAP3Test_CSenServiceConnection_SendL,Pattern,IAP3Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_Pattern_IAP3L),
		ENTRY("NewLC-contracttest_CSenServiceConnection_NewLCcontract",CsenConnectionTester::UT_CSenServiceConnection_NewLC_ContractL),
		ENTRY("NewL-descriptiontestforWS-I_CSenServiceConnection_NewLdescriptionforWS-I",CsenConnectionTester::UT_CSenServiceConnection_NewL_WS_I_DescriptionL),
		ENTRY("NewLC-descriptiontestforWS-I_CSenServiceConnection_NewLCdescriptionforWS-I",CsenConnectionTester::UT_CSenServiceConnection_NewLC_WS_I_DescriptionL),
		ENTRY("NewL-descriptiontestforID-WSF_CSenServiceConnection_NewLdescriptionforID-WSF",CsenConnectionTester::UT_CSenServiceConnection_NewL_ID_WSF_DescriptionL),
		ENTRY("NewLC-descriptiontestforID-WSF_CSenServiceConnection_NewLCdescriptionforID-WSF",CsenConnectionTester::UT_CSenServiceConnection_NewLC_ID_WSF_DescriptionL),
		ENTRY("NewL-patterntestforWS-I_CSenServiceConnection_NewLpatternforWS-I",CsenConnectionTester::UT_CSenServiceConnection_NewL_WS_I_PatternL),
		ENTRY("NewLC-patterntestforWS-I_CSenServiceConnection_NewLCpatternforWS-I",CsenConnectionTester::UT_CSenServiceConnection_NewLC_WS_I_PatternL),
		ENTRY("NewL-patterntestforID-WSF_CSenServiceConnection_NewLpatternforID-WSF",CsenConnectionTester::UT_CSenServiceConnection_NewL_ID_WSF_PatternL),
		ENTRY("NewLC-patterntestforID-WSF_CSenServiceConnection_NewLCpatternforID-WSF",CsenConnectionTester::UT_CSenServiceConnection_NewLC_ID_WSF_PatternL),
		ENTRY("SendL-noerrortest_CSenServiceConnection_SendLnoerror",CsenConnectionTester::UT_CSenServiceConnection_SendL_NoErrorL),
		ENTRY("SendL-canceltest_CSenServiceConnection_SendLnoerror",CsenConnectionTester::UT_CSenServiceConnection_SendL_CancelL),
		ENTRY("SendL-connectionisnotinitializedtest_CSenServiceConnection_SendLconnectionisnotinitialized",CsenConnectionTester::UT_CSenServiceConnection_SendL_ConNotInitialL),
		ENTRY("SendL-MultipleMessages_CSenServiceConnection_SendLMultipleMessages",CsenConnectionTester::UT_CSenServiceConnection_SendMultipleMessagesL),
		ENTRY("SendL,Get,Post1Test_CSenServiceConnection_SendL,Get,Post1Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_Get_Post1L),
		ENTRY("SendL,Get,Post2Test_CSenServiceConnection_SendL,Get,Post2Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_Get_Post2L),
		ENTRY("SendL,Post,Proxy1Test_CSenServiceConnection_SendL,Post,Proxy1Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_Proxy1L),
		ENTRY("SendL,Post,Proxy2Test_CSenServiceConnection_SendLconnectionisnotinitialized",CsenConnectionTester::UT_CSenServiceConnection_SendL_Proxy2L),
		ENTRY("SendL,ContentType1Test_CSenServiceConnection_SendL,ContentType1Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_ContentType1L),
		ENTRY("SendL,ContentType2Test_CSenServiceConnection_SendL,ContentType2Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_ContentType2L),
		ENTRY("SendL,UserAgent1Test_CSenServiceConnection_SendL,UserAgent1Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_UserAgent1L),
		ENTRY("SendL,UserAgent2Test_CSenServiceConnection_SendL,UserAgent2Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_UserAgent2L),
		ENTRY("SendL,MessageSoapActionTest_CSenServiceConnection_SendL,MessageSoapActionTest",CsenConnectionTester::UT_CSenServiceConnection_SendL_MessageSoapActionL),
		ENTRY("SendL,SoapAction1Test_CSenServiceConnection_SendL,SoapAction1Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_SoapAction1L),
		ENTRY("SendL,SoapAction2Test_CSenServiceConnection_SendL,SoapAction2Test",CsenConnectionTester::UT_CSenServiceConnection_SendL_SoapAction2L),
		ENTRY("SendL,CustomHttpHeaderTest_CSenServiceConnection_SendL,CustomHttpHeaderTest",CsenConnectionTester::UT_CSenServiceConnection_SendL_CustomHttpHeaderL),
		ENTRY("SendL,TPboundariesTest_CSenServiceConnection_SendL,TPboundariesTest",CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries1),
		ENTRY("SendL,TPboundariesTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries2),
		ENTRY("SendL,TPboundariesTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries3),
		ENTRY("SendL,TPboundariesTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries4),
		ENTRY("SendL,TPboundariesTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries5),
		ENTRY("SendL,TPboundariesTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries6),
		ENTRY("SendL,TPboundariesTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries7),
		ENTRY("SendL,TPboundariesTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries8),
		ENTRY("SendL,TPboundariesTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries9),
		ENTRY("SendL,TPSecDialogTest_CSenServiceConnection_SendLconnectionisnotinitialized",CsenConnectionTester::UT_CSenServiceConnection_TP_SecDialog),
		ENTRY("SendL,TPOmittedTrueTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Omitted_True),
		ENTRY("SendL,TPOmittedFalseTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Omitted_False),
		ENTRY("SendL,TPRemovedTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Removed),
		ENTRY("SendL,TPOmit/RemTest_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TP_Omit_Rem),
		ENTRY("SubmitL,LibXml2_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_L),
		ENTRY("SubmitL,LibXml2+props_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_with_Properties_L),
		ENTRY("SubmitL,BLOB20KB_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_20KBL),
		ENTRY("SubmitL,BLOB200KB_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_200KBL),
		ENTRY("SubmitL,BLOBless1MB_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_Less1MBL),
		ENTRY("SubmitL,BLOBmore1MB_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_More1MBL),
		ENTRY("MSN-Basiccase_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TestMessengerL),
		ENTRY("MSN-ReUseCredential_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TestMessenger2L),
		ENTRY("MSN-AuthCallbacksuccess_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TestMessenger3L),
		ENTRY("MSN-AuthCallbackfail_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TestMessenger4L),
		ENTRY("MSN-UserInfoPrompt_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TestMessenger5L),
		ENTRY("MSN-showprompt_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TestMessenger6L),
		ENTRY("MSN-Invalidpassword_CSenServiceConnection_",CsenConnectionTester::UT_CSenServiceConnection_TestMessenger7L),
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
//  METHODS
// -----------------------------------------------------------------------------
void CsenConnectionTester::HandleMessageL(const TDesC8& aMessage)
    {
    iCallback = ECallbackHandleMessage;
    if ( ipMessage )
        {
        delete ipMessage;
        ipMessage = NULL;
        }
    ipMessage = aMessage.AllocL();
    iTransactionId = iServiceConnection->TxnId();

    StopActiveScheduler();
    }

void CsenConnectionTester::HandleErrorL(const int aErrorCode, const TDesC8& aError)
    {
    iCallback = ECallbackHandleError;
    iErrorCode = aErrorCode;
    
    // debugging only - START
    if( HTTPStatus::IsInformational( this->iErrorCode ) )
        {
        // if((aStatus >= HTTPStatus::EContinue) && (aStatus < HTTPStatus::EOk))
        RDebug::Print(_L("- HTTP error code: IS INFORMATION."));
        }
    if( HTTPStatus::IsSuccessful( this->iErrorCode ) )
        {
        // if((aStatus >= HTTPStatus::EOk) && (aStatus < HTTPStatus::EMultipleChoices))
        RDebug::Print(_L("- HTTP error code: IS SUCCESSFUL."));
        }
    if( HTTPStatus::IsRedirection( this->iErrorCode ) )
        {
        // if((aStatus >= HTTPStatus::EMultipleChoices) && (aStatus < HTTPStatus::EBadRequest))
        RDebug::Print(_L("- HTTP error code: IS REDIRECTION."));
        }
    if( HTTPStatus::IsClientError( this->iErrorCode ) )
        {
        // if((aStatus >= HTTPStatus::EBadRequest) && (aStatus < HTTPStatus::EInternalServerError))
        RDebug::Print(_L("- HTTP error code: IS CLIENT ERROR."));
        }
    if( HTTPStatus::IsServerError( this->iErrorCode ) )
        {
        // if(aStatus >= HTTPStatus::EInternalServerError)
        RDebug::Print(_L("- HTTP error code: IS CLIENT ERROR."));
        }    
    // debuggin only - END
    
    if ( ipMessage )
        {
        delete ipMessage;
        ipMessage = NULL;
        }
    ipMessage = aError.AllocL();
    iTransactionId = iServiceConnection->TxnId();
    
    TBuf<70> errorMessage;
    _LIT(KErrNotInitialized, "Connection isn't initialized.");
    
    switch(aErrorCode) 
        {
        case KErrSubmitting:
            errorMessage = KErrNotInitialized;
            break;
        default:
            break;
        }   

    StopActiveScheduler();
    }

void CsenConnectionTester::ErrorL(TInt aCode)
    {
    _LIT(KNotReady, "Connection is not ready.");
    
    TBuf<70> errorMessage;
    switch(aCode) 
        {
        case EConnectionNotReady:
            errorMessage = KNotReady;
            break;
        default:
            break;
        }
    }

void CsenConnectionTester::SetStatus(const TInt aStatus)
    {
    switch(aStatus)
        {
        case KSenConnectionStatusNew:
            break;
        case KSenConnectionStatusReady:
            iInitialized = ETrue;
            break;
        case KSenConnectionStatusExpired:
            break;
        default:
            TBuf<32> num;
            num.AppendNum(aStatus);
            break;
        }
    if ( aStatus >= KSenConnectionStatusCreateFailed && aStatus <= KSenConnectionStatusMetadataExchange )
        {
        iConnectionState = aStatus;
        if(!iInitialized)
            {
            TInt leaveCode(KErrNone);
            TRAP(leaveCode, ErrorL(EConnectionNotReady));
            leaveCode = 0; // not used
            }
        StopActiveScheduler();
        }
    }

void CsenConnectionTester::SetupL(  )
    {
    __UHEAP_MARK;

    RFile file;
    RFs iFsSession;
    _LIT(KFileName,"e:\\testing\\data\\rest.on");
    
   
    User::LeaveIfError(iFsSession.Connect());
        
    TInt fileOP = file.Open(iFsSession,KFileName,EFileRead);
    if (fileOP == KErrNone) 
        {
        RDebug::Print(_L( "Found rest.on file => using RESTful framework" ) ); 
        file.Close();
        ipFrameworkID = HBufC8::NewL(KRESTFrameworkID().Length());
        ipFrameworkID->Des().Append(KRESTFrameworkID);
        }
    else
        {
        RDebug::Print(_L( "File rest.on not found => defaulting to WS-I" ) ); 
        ipFrameworkID = HBufC8::NewL(KWSIFrameworkID().Length());
        ipFrameworkID->Des().Append(KWSIFrameworkID);
        }
    iFsSession.Close();
    
    iCallback = ECallbackNone;
    }

void CsenConnectionTester::Teardown(  )
    {
    delete ipFrameworkID;
    ipFrameworkID = NULL;
    delete ipMessage;
    ipMessage = NULL;
    delete ipProviderForAuthProvider;
    ipProviderForAuthProvider = NULL;
    __UHEAP_MARKEND;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_RegisterIdentityProviderLL()
    {
    iManager = CSenServiceManager::NewL();
    STIFF_ASSERT_NOT_EQUALS_DESC(iManager, (CSenServiceManager*)NULL, "The test is failed.");

    CSenIdentityProvider* idp = CSenIdentityProvider::NewLC(
        KAuthServEndpoint, KASContract);

    // Set the Provider ID
    idp->SetProviderID(KASProviderID);

    // Associate Forum Nokia's Addressbook service ID (contract)
    // to this Identity Provider
    idp->SetServiceID(KWSPContract);

    // Set Liberty ID-WSF framework ID
    idp->SetFrameworkIdL(KDefaultIdWsfFrameworkID);

    // ------------------------------------------------------------------------
    // The following username/password properties will be used for
    // authentication. Please note, that use advisory authentication
    // id "IMEI" would result device ID to be directly fetched from phone.
    // ------------------------------------------------------------------------
    //
    
#if SYSOPENDIGIA_ADDRESSBOOK_SERVICE    
    idp->SetUserInfoL(KNullDesC8, KTestAuthzID, KTestPassword);
#else
    idp->SetUserInfoL(KTestAuthzID, KNullDesC8, KTestPassword);
#endif    
    
    TL((iManager->RegisterIdentityProviderL(*idp)) == KErrNone);
    TL((iManager->RegisterServiceDescriptionL(*idp)) == KErrNone);

    CleanupStack::PopAndDestroy(idp); 

    __ASSERT_ALWAYS_NO_LEAVE(delete iManager);
    iManager = NULL;
    return KErrNone;
    }
    
TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_ContractL(  )
    {
        iServiceConnection = CSenServiceConnection::NewL(*this, KAddressBookContract);
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    TL(iServiceConnection != (CSenServiceConnection*)NULL);
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_ContractL(  )
    {
    iServiceConnection = CSenServiceConnection::NewLC(*this, KAddressBookContract);
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    TL(iServiceConnection != (CSenServiceConnection*)NULL);
    
    CleanupStack::Pop(iServiceConnection);
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    return KErrNone;
    }
    
TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_WS_I_DescriptionL(  )
    {
    // Only Endpoint and FrameworkId needs to be defined in WS-I case.
    // There is no need to define Contract. => KNullDesC8 is passed as a Contract.
    iSenXmlServiceDescription = CSenXmlServiceDescription::NewL(KAuthServEndpoint, KNullDesC8());
    iSenXmlServiceDescription->SetFrameworkIdL(*ipFrameworkID);    
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *iSenXmlServiceDescription);
    StartActiveScheduler(1);
    
    STIFF_ASSERT_DESC(iConnectionState == KSenConnectionStatusReady,
                      "Could not initialize ServiceConnection");
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iSenXmlServiceDescription);
    iSenXmlServiceDescription = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_WS_I_DescriptionL(  )
    {
    // Only Endpoint and FrameworkId needs to be defined in WS-I case.
    // There is no need to define Contract. => KNullDesC8 is passed as a Contract.
    iSenXmlServiceDescription = CSenXmlServiceDescription::NewL(KAuthServEndpoint, KNullDesC8());
    iSenXmlServiceDescription->SetFrameworkIdL(*ipFrameworkID);    
    
    iServiceConnection = CSenServiceConnection::NewLC(*this, *iSenXmlServiceDescription);
    StartActiveScheduler(1);
    
    STIFF_ASSERT_DESC(iConnectionState == KSenConnectionStatusReady,
                      "Could not initialize ServiceConnection");
    
    CleanupStack::Pop(iServiceConnection);
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iSenXmlServiceDescription);
    iSenXmlServiceDescription = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_ID_WSF_DescriptionL(  )
    {
    iSenXmlServiceDescription = CSenXmlServiceDescription::NewL();
    iSenXmlServiceDescription->SetFrameworkIdL(KIDWSFFrameworkID);    
    iSenXmlServiceDescription->SetContractL(KAddressBookServiceId);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    iSenXmlServiceDescription->SetIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *iSenXmlServiceDescription);
    StartActiveScheduler(1);
    
    STIFF_ASSERT_DESC(iConnectionState == KSenConnectionStatusReady,
                      "Could not initialize ServiceConnection");
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iSenXmlServiceDescription);
    iSenXmlServiceDescription = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_ID_WSF_DescriptionL(  )
    {
    iSenXmlServiceDescription = CSenXmlServiceDescription::NewL();
    iSenXmlServiceDescription->SetFrameworkIdL(KIDWSFFrameworkID);    
    iSenXmlServiceDescription->SetContractL(KAddressBookServiceId);

    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    iSenXmlServiceDescription->SetIapIdL(iapId);

    iServiceConnection = CSenServiceConnection::NewLC(*this, *iSenXmlServiceDescription);
    StartActiveScheduler(1);
    
    STIFF_ASSERT_DESC(iConnectionState == KSenConnectionStatusReady,
                      "Could not initialize ServiceConnection");
    
    CleanupStack::Pop(iServiceConnection);
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iSenXmlServiceDescription);
    iSenXmlServiceDescription = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_WS_I_PatternL(  )
    {
    CSenServicePattern* pSenServicePattern = CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pSenServicePattern->SetFrameworkIdL(*ipFrameworkID);    
    //pSenServicePattern->SetContractL(KAuthServEndpoint);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pSenServicePattern);
    StartActiveScheduler(1);
    
    STIFF_ASSERT_DESC(iConnectionState == KSenConnectionStatusReady,
                      "Could not initialize ServiceConnection");
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    
    CleanupStack::Pop(pSenServicePattern);
    __ASSERT_ALWAYS_NO_LEAVE(delete pSenServicePattern);
    pSenServicePattern = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_WS_I_PatternL(  )
    {
    CSenServicePattern* aSenServicePattern = CSenServicePattern::NewL(KAuthServEndpoint, KNullDesC8());
    aSenServicePattern->SetFrameworkIdL(*ipFrameworkID);    
    //aSenServicePattern->SetContractL(KAuthServEndpoint);

    iServiceConnection = CSenServiceConnection::NewLC(*this, *aSenServicePattern);
    StartActiveScheduler(1);
    
    STIFF_ASSERT_DESC(iConnectionState == KSenConnectionStatusReady,
                      "Could not initialize ServiceConnection");

    CleanupStack::Pop(); //iServiceConnection
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    
    delete aSenServicePattern;
    aSenServicePattern = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_ID_WSF_PatternL(  )
    {
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KAddressBookServiceId, KNullDesC8());
    pPattern->SetFrameworkIdL(KIDWSFFrameworkID);    
    pPattern->SetContractL(KAddressBookServiceId);

    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    StartActiveScheduler(1);
    
    STIFF_ASSERT_NOT_EQUALS_DESC(iServiceConnection,(CSenServiceConnection*)NULL, "The test is failed.");
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    
    CleanupStack::Pop(pPattern);
    __ASSERT_ALWAYS_NO_LEAVE(delete pPattern);
    pPattern = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_ID_WSF_PatternL(  )
    {
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KAddressBookServiceId, KNullDesC8());
    pPattern->SetFrameworkIdL(KIDWSFFrameworkID);    
    pPattern->SetContractL(KAddressBookServiceId);

    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);

    iServiceConnection = CSenServiceConnection::NewLC(*this, *pPattern);
    StartActiveScheduler(1);
    
    TL (iServiceConnection != (CSenServiceConnection*)NULL);
    CleanupStack::Pop(); //iServiceConnection
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    
    CleanupStack::Pop(pPattern);
    __ASSERT_ALWAYS_NO_LEAVE(delete pPattern);
    pPattern = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_NoErrorL(  )
    {
    iSenXmlServiceDescription = CSenXmlServiceDescription::NewL();
    iSenXmlServiceDescription->SetFrameworkIdL(KIDWSFFrameworkID);    
    iSenXmlServiceDescription->SetContractL(KAddressBookServiceId);
        
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    iSenXmlServiceDescription->SetIapIdL(iapId);

    iServiceConnection = CSenServiceConnection::NewL(*this, *iSenXmlServiceDescription);
    StartActiveScheduler(1);
    
    STIFF_ASSERT_NOT_EQUALS_DESC(iServiceConnection,(CSenServiceConnection*)NULL, "The test is failed.");
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = iServiceConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    StartActiveScheduler(1);

    TL (iTransactionId == transactionIdOrError);
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iSenXmlServiceDescription);
    iSenXmlServiceDescription = NULL;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_CancelL(  )
    {
    iSenXmlServiceDescription = CSenXmlServiceDescription::NewL();
    iSenXmlServiceDescription->SetFrameworkIdL(KIDWSFFrameworkID);    
    iSenXmlServiceDescription->SetContractL(KAddressBookServiceId);
        
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    iSenXmlServiceDescription->SetIapIdL(iapId);

    iServiceConnection = CSenServiceConnection::NewL(*this, *iSenXmlServiceDescription);
    StartActiveScheduler(1);
    
    STIFF_ASSERT_NOT_EQUALS_DESC(iServiceConnection,(CSenServiceConnection*)NULL, "The test is failed.");
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = iServiceConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    iServiceConnection->CancelTransaction(transactionIdOrError);
    
    StartActiveScheduler(1);

    TL (iTransactionId == transactionIdOrError);
    LOCAL_ASSERT(iErrorCode == KErrSenCancelled);
    
    User::After(2000000);
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iSenXmlServiceDescription);
    iSenXmlServiceDescription = NULL;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ConNotInitialL(  )
    {
    iSenXmlServiceDescription = CSenXmlServiceDescription::NewL();
    iSenXmlServiceDescription->SetFrameworkIdL(KIDWSFFrameworkID);    
    iSenXmlServiceDescription->SetContractL(KAddressBookServiceId);
        
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    iSenXmlServiceDescription->SetIapIdL(iapId);

    iServiceConnection = CSenServiceConnection::NewL(*this, *iSenXmlServiceDescription);
    
    STIFF_ASSERT_NOT_EQUALS_DESC(iServiceConnection,(CSenServiceConnection*)NULL, "The test is failed.");
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TL((iServiceConnection->SendL(KRequest)) == KErrSenNotInitialized);
    StartActiveScheduler(1);
    
    delete iSenXmlServiceDescription;
    iSenXmlServiceDescription = NULL;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete iServiceConnection);
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendMultipleMessagesL(  )
    {
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // Http Get (using SetTransportPropertiesL)
    // ****************************************
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    
    CleanupStack::PushL(pSerializedProperties);

    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    //CleanupStack::PopAndDestroy(pSerializedProperties);
    
    //pHttpProperties->SetOmittedL(KIapIdLocalName, ETrue);
    //pSerializedProperties = pHttpProperties->AsUtf8L();

    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(10);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }


TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Get_Post1L(  )
    {
    // 1. SendL(MSG [SC::POST by default])  
    // 2. SendL(MSG, TP::GET),
    // 3. SendL(MSG, TP::POST)
            
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    // Check from HTTP Transaction logger log that POST method were used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodPost);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);

    // Http Get
    pHttpProperties->SetHttpMethodL(CSenHttpTransportProperties::ESenHttpGet);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    

    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleError);
    
    // Check from HTTP Transaction logger log that GET method were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodGet);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);
    
    // Http Post
    pHttpProperties->SetHttpMethodL(CSenHttpTransportProperties::ESenHttpPost);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    

    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    // Check from HTTP Transaction logger log that POST method were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodPost);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    
    CleanupStack::PopAndDestroy(pHttpProperties);
    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Get_Post2L(  )
    {
    // 1. SetTransportPropertiesL(TP::GET)
    // 2. SendL(MSG) - Send(MSG, TP::POST)
    // 3. SendL (MSG [SC::GET])
    // 4. SetTransportPropertiesL(TP:POST)
    // 5. SendL(MSG [SC:POST]
            
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // Http Get (using SetTransportPropertiesL)
    // ****************************************
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpMethodL(CSenHttpTransportProperties::ESenHttpGet);
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SetTransportPropertiesL(*pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleError);

    // Check from HTTP Transaction logger log that GET method were used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodGet);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);

    // Http Post (using TransportProperties in SendL parameters)
    // *********************************************************
    pHttpProperties->SetHttpMethodL(CSenHttpTransportProperties::ESenHttpPost);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    

    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    
    // Check from HTTP Transaction logger log that Post method were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodPost);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);

    // Http Get (using TransportProperties in SendL parameters)
    // ********************************************************
    pHttpProperties->SetHttpMethodL(CSenHttpTransportProperties::ESenHttpGet);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    

    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleError);
    
    // Check from HTTP Transaction logger log that GET method were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodGet);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);
    
    // Http Post (using SetTransportPropertiesL)
    // *****************************************
    pHttpProperties->SetHttpMethodL(CSenHttpTransportProperties::ESenHttpPost);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    

    iServiceConnection->SetTransportPropertiesL(*pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    
    // Check from HTTP Transaction logger log that Post method were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodPost);        
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);

    // Http Post (using TransportProperties in SendL parameters)
    // *********************************************************
    pHttpProperties->SetHttpMethodL(CSenHttpTransportProperties::ESenHttpPost);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    

    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    // Check from HTTP Transaction logger log that POST method were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodPost);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    
    CleanupStack::PopAndDestroy(pHttpProperties);
    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }
    
TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Proxy1L(  )
    {
    // SetProxy via Set(TP)
    // ********************
    // 1. SetTransportProperties(TP::PROXY)
    // 2. SendL(MSG)    _LIT8(KSoapAction,              "SOAPAction");
    
    _LIT8(KSoapAction,              "SOAPAction");
    _LIT8(KSoapActionValue,         "SoapEnvelopeAction");
    _LIT8(KSoapActionReturnValue,   "\"SoapEnvelopeAction\"");
    
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);


    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    
    // Now the code execution has returned in here (either HandleMessageL or HandleErrorL was invoked)
    
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();
    pSOAPMessage->SetSoapActionL(KSoapActionValue);    
    
    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetProxyHostL(KProxyHost);
    pHttpProperties->SetProxyPortL(KProxyPort);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SetTransportPropertiesL(*pSerializedProperties);

    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    
    // Read HTTP Transaction from file
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    
    LOCAL_ASSERT(pFileContent != NULL);

    CleanupStack::PushL(pFileContent);
    
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    
    CleanupStack::PopAndDestroy(pFileContent);

    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Proxy2L(  )
    {
    // SetProxy via SendL(MSG, TP)
    // ***************************
    // 1. SendL(MSG, TP::PROXY)
    
    _LIT8(KSoapAction,              "SOAPAction");
    _LIT8(KSoapActionValue,         "SoapEnvelopeAction");
    _LIT8(KSoapActionReturnValue,   "\"SoapEnvelopeAction\"");
    
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();
    pSOAPMessage->SetSoapActionL(KSoapActionValue);    
    
    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetProxyHostL(KProxyHost);
    pHttpProperties->SetProxyPortL(KProxyPort);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);

    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    
    // Read HTTP Transaction from file
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    
    LOCAL_ASSERT(pFileContent != NULL);

    CleanupStack::PushL(pFileContent);
    
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    
    CleanupStack::PopAndDestroy(pFileContent);

    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }
    
TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Pattern_IAP1L(  )
    {
    // ServicePattern::SetConsumerIapIdL(IAP_ID)
    // *****************************************
    // 1. Create new Service Pattern (SPatt)
    // 2. SPatt::SetConsumerIapIdL(IAP_ID)
    // 3. Instantiate new SC, wait for SetStatus(READY)
    // 4. SendL(MSG)  using selected IAP
            
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }    

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Pattern_IAP2L(  )
    {
    // SendL(MSG, TP:IAP_ID)         NOTE: this test case requires user interaction!!!
    // *******************************************************************************
    // 1. Instantiate new SC, wait for SetStatus(READY) [IAP is prompted from end user!]
    // 2. TP::SetIapIdL(IAP_ID)
    // 3. SendL(MSG, TP::IAP_ID)  using selected IAP
    // 4. SendL(MSG)  -- should prompt the IAP from the end-user
            
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }    

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Pattern_IAP3L(  )
    {
    // SetTransportPropertiesL(TP::IAP_ID)
    // 1. SetTransportPropertiesL(TP::IAP_ID)
    // 2. SendL(MSG)  using selected IAP
    // 3. TP:SetOmittedL(IAP_ID)
    // 4. SendL(MSG, TP::IAP_ID-Omitted) -- should prompt the IAP from the end-user    
           
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // 1. SetTransportPropertiesL(TP::IAP_ID)
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SetTransportPropertiesL(*pSerializedProperties);
    // 2. SendL(MSG)  using selected IAP
    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    // 3. TP:SetOmittedL(IAP_ID)
    //pHttpProperties->SetOmittedL(KIapIdLocalName, ETrue);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    // 4. SendL(MSG, TP::IAP_ID-Omitted) -- should prompt the IAP from the end-user    
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ContentType1L(  )
    {
    // SendL(MSG, TP::Content-Type)
    // 1. Get TP via SC:TransportPropertiesL() // store currently effective content-type
    // 2. TP::SetContentTypeIdL(Content-Type)
    // 3. SendL(MSG, TP::Content-Type)  // check that selected Content-Type is used
    // 4. SendL(MSG) // original content type should be used (from step 1)

    _LIT8(KContentTypeValue, "text");
           
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    HBufC8* pOriginalContentType = NULL; 
    
    // 1. Get TP via SC:TransportPropertiesL() // store currently effective content-type
    HBufC8* pTransportPropertiesAsXml = NULL;
    iServiceConnection->TransportPropertiesL(pTransportPropertiesAsXml);
    if ( pTransportPropertiesAsXml )
        {
        CleanupStack::PushL(pTransportPropertiesAsXml);
        CSenXmlReader* pParser = CSenXmlReader::NewLC();
        CSenHttpTransportProperties* pReadHttpProperties = 
                    CSenHttpTransportProperties::NewLC(*pTransportPropertiesAsXml,
                                                       *pParser);
        TPtrC8 contentType;
        TInt retVal = pReadHttpProperties->ContentTypeL(contentType);
        if ( retVal == KErrNone )
            {
            pOriginalContentType = contentType.AllocL();
            }
        CleanupStack::PopAndDestroy(pReadHttpProperties);
        CleanupStack::PopAndDestroy(pParser);
        CleanupStack::PopAndDestroy(pTransportPropertiesAsXml);
        }
        
    LOCAL_ASSERT(pOriginalContentType != NULL); // Content-type is a MUST for every transport
        
    if ( pOriginalContentType )
        {
        CleanupStack::PushL(pOriginalContentType);
        }

    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // 2. TP::SetContentTypeIdL(Content-Type)
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    pHttpProperties->SetContentTypeL(KContentTypeValue);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);

    // 3. SendL(MSG, TP::Content-Type)  // check that selected Content-Type is used
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    // Check from HTTP Transaction logger log that correct ContentType were used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, _L8("Content-Type"));
       
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KContentTypeValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    
    
    // 4. SendL(MSG) // original content type should be used (from step 1)
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // Check from HTTP Transaction logger log that correct ContentType were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, _L8("Content-Type"));
    if( pValue && pOriginalContentType )
        {
        LOCAL_ASSERT(*pValue == *pOriginalContentType);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    CleanupStack::PopAndDestroy(pSOAPMessage);

    if ( pOriginalContentType )
        {
        CleanupStack::PopAndDestroy(pOriginalContentType);
        }
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }
    
TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ContentType2L(  )
    {
    // SetTransportPropertiesL(TP::Content-Type)
    // 1. Get TP via SC:TransportPropertiesL()
    // 2. SendL(MSG), Content-Type got from step 1 should be effective
    // 3. SetTransportPropertiesL(TP::Content-Type) // change content-type
    // 4. SendL(MSG)  // check new content-type
    // 5. SendL(MSG, TP:Content-Type)  // check *third* kind of content-type

    _LIT8(KContentTypeValue1, "text");
    _LIT8(KContentTypeValue2, "xml");
           
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    // 1. Get TP via SC:TransportPropertiesL()
    TPtrC8 contentType;
    HBufC8* pTransportPropertiesAsXml = NULL;
    iServiceConnection->TransportPropertiesL(pTransportPropertiesAsXml);
    if ( pTransportPropertiesAsXml )
        {
        CleanupStack::PushL(pTransportPropertiesAsXml);
        CSenXmlReader* pParser = CSenXmlReader::NewLC();
        CSenHttpTransportProperties* pReadHttpProperties = 
                    CSenHttpTransportProperties::NewLC(*pTransportPropertiesAsXml,
                                                       *pParser);
        pReadHttpProperties->ContentTypeL(contentType);
        CleanupStack::PopAndDestroy(pReadHttpProperties);
        CleanupStack::PopAndDestroy(pParser);
        CleanupStack::PopAndDestroy(pTransportPropertiesAsXml);
        }

    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // 2. SendL(MSG), Content-Type got from step 1 should be effective
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SetTransportPropertiesL(*pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    // 3. SetTransportPropertiesL(TP::Content-Type) // change content-type
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetContentTypeL(KContentTypeValue1);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SetTransportPropertiesL(*pSerializedProperties);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // 4. SendL(MSG)  // check new content-type
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // Check from HTTP Transaction logger log that correct ContentType were used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, _L8("Content-Type"));
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KContentTypeValue1);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 5. SendL(MSG, TP:Content-Type)  // check *third* kind of content-type
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetContentTypeL(KContentTypeValue2);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // Check from HTTP Transaction logger log that correct ContentType were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, _L8("Content-Type"));
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KContentTypeValue2);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_UserAgent1L(  )
    {
    // SendL(MSG, TP::User-Agent)
    // 1. Get TP via SC:TransportPropertiesL() // store currently effective User-Agent
    // 2. TP::SetUserAgentL(User-Agent)
    // 3. SendL(MSG, TP::User-Agent)  // check that selected User-Agentis used
    // 4. SendL(MSG) // original User-Agent should be used (from step 1)

    _LIT8(KUserAgentValue, "TestAgent");
           
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    // 1. Get TP via SC:TransportPropertiesL() // store currently effective User-Agent
    HBufC8* pOriginalUserAgent = NULL;
    HBufC8* pTransportPropertiesAsXml = NULL;
    iServiceConnection->TransportPropertiesL(pTransportPropertiesAsXml);
    if ( pTransportPropertiesAsXml )
        {
        TPtrC8 userAgent;
        CleanupStack::PushL(pTransportPropertiesAsXml);
        CSenXmlReader* pParser = CSenXmlReader::NewLC();
        CSenHttpTransportProperties* pReadHttpProperties = 
                    CSenHttpTransportProperties::NewLC(*pTransportPropertiesAsXml,
                                                       *pParser);
        pReadHttpProperties->UserAgentL(userAgent);
        pOriginalUserAgent = userAgent.AllocL();
        CleanupStack::PopAndDestroy(pReadHttpProperties);
        CleanupStack::PopAndDestroy(pParser);
        CleanupStack::PopAndDestroy(pTransportPropertiesAsXml);
        CleanupStack::PushL(pOriginalUserAgent);
        }

    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // 2. TP::SetUserAgentL(User-Agent)
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetUserAgentL(KUserAgentValue);

    // 3. SendL(MSG, TP::User-Agent)  // check that selected User-Agent is used
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    // Check from HTTP Transaction logger log that selected User-Agent were used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, _L8("User-Agent"));
    LOCAL_ASSERT(*pValue == KUserAgentValue);
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 4. SendL(MSG) // original User-Agent should be used (from step 1)
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // Check from HTTP Transaction logger log that original User-Agent were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, _L8("User-Agent"));
    if ( pValue && pOriginalUserAgent )
        {
        LOCAL_ASSERT(*pValue == *pOriginalUserAgent);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    if ( pOriginalUserAgent )
        {
        CleanupStack::PopAndDestroy(pOriginalUserAgent);
        }
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_UserAgent2L(  )
    {
    // SetTransportPropertiesL(TP:User-Agent)
    // 1. Get TP via SC:TransportPropertiesL() // store original user-agent
    // 2. SendL(MSG); // check User-Agent from step 1 is sent/used
    // 3. SetTransportPropertiesL(TP::User-Agent) // change user-agent
    // 4. SendL(MSG, TP::User-Agent) // change user agent, check that it is used
    // 5. SendL(MSG); // check that set User-Agent is used, from step 3

    _LIT8(KUserAgentValue1, "TestAgent1");
    _LIT8(KUserAgentValue2, "TestAgent2");
           
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    // 1. Get TP via SC:TransportPropertiesL() // store original user-agent
    HBufC8* pOriginalUserAgent = NULL;
    HBufC8* pTransportPropertiesAsXml = NULL;
    iServiceConnection->TransportPropertiesL(pTransportPropertiesAsXml);
    if ( pTransportPropertiesAsXml )
        {
        TPtrC8 userAgent;
        CleanupStack::PushL(pTransportPropertiesAsXml);
        CSenXmlReader* pParser = CSenXmlReader::NewLC();
        CSenHttpTransportProperties* pReadHttpProperties = 
                    CSenHttpTransportProperties::NewLC(*pTransportPropertiesAsXml,
                                                       *pParser);
        pReadHttpProperties->UserAgentL(userAgent);
        pOriginalUserAgent = userAgent.AllocL();
        CleanupStack::PopAndDestroy(pReadHttpProperties);
        CleanupStack::PopAndDestroy(pParser);
        CleanupStack::PopAndDestroy(pTransportPropertiesAsXml);
        CleanupStack::PushL(pOriginalUserAgent);
        }

    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // 2. SendL(MSG); // check User-Agent from step 1 is sent/used
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    // Check from HTTP Transaction logger log that original User-Agent were used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, _L8("User-Agent"));

    if ( pValue && pOriginalUserAgent )
        {
        LOCAL_ASSERT(*pValue == *pOriginalUserAgent);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 3. SetTransportPropertiesL(TP::User-Agent) // change user-agent
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetUserAgentL(KUserAgentValue1);
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SetTransportPropertiesL(*pSerializedProperties);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // 4. SendL(MSG, TP::User-Agent) // change user agent, check that it is used
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetUserAgentL(KUserAgentValue2);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // Check from HTTP Transaction logger log that defined User-Agent were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, _L8("User-Agent"));
    LOCAL_ASSERT(*pValue == KUserAgentValue2);
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 5. SendL(MSG); // check that set User-Agent is used, from step 3
    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    // Check from HTTP Transaction logger log that defined (in step 3) User-Agent were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, _L8("User-Agent"));
    LOCAL_ASSERT(*pValue == KUserAgentValue1);
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    if ( pOriginalUserAgent )
        {
        CleanupStack::PopAndDestroy(pOriginalUserAgent);
        }
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_MessageSoapActionL(  )
    {
    _LIT8(KSoapAction,              "SOAPAction");
    _LIT8(KSoapActionValue,         "MessageAction");
    _LIT8(KSoapActionReturnValue,   "\"MessageAction\"");
    
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();
    pSOAPMessage->SetSoapActionL(KSoapActionValue);    
    
    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    

    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    
    // Read HTTP Transaction from file
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    
    LOCAL_ASSERT(pFileContent != NULL);

    CleanupStack::PushL(pFileContent);
    
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    LOCAL_ASSERT(*pValue == KSoapActionReturnValue);
    CleanupStack::PopAndDestroy(pValue);
    
    CleanupStack::PopAndDestroy(pFileContent);

    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }
    
TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_SoapAction1L(  )
    {
    // SendL(MSG, TP:SOAPAction)
    // 1. Get TP via SC:TransportPropertiesL() // store currently effective SOAPAction
    // 2. TP::SetSoapActionL(SOAPAction)
    // 3. SendL(MSG, TP::SOAPAction)  // check that selected SOAPAction is used
    // 4. SendL(MSG) // original SOAPAction should be used (from step 1)    
    
    _LIT8(KSoapAction,              "SOAPAction");
    _LIT8(KSoapActionValue,         "MessagePropertyAction");
    _LIT8(KSoapActionReturnValue,   "\"MessagePropertyAction\"");
    
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.

    // 1. Get TP via SC:TransportPropertiesL() // store currently effective SOAPAction
    HBufC8* pOriginalSoapAction = NULL;
    HBufC8* pTransportPropertiesAsXml = NULL;
    iServiceConnection->TransportPropertiesL(pTransportPropertiesAsXml);
    if ( pTransportPropertiesAsXml )
        {
        CleanupStack::PushL(pTransportPropertiesAsXml);
        CSenXmlReader* pParser = CSenXmlReader::NewLC();
        CSenHttpTransportProperties* pReadHttpProperties = 
                    CSenHttpTransportProperties::NewLC(*pTransportPropertiesAsXml,
                                                       *pParser);
        TPtrC8 soapAction(KNullDesC8);
        TInt retVal = pReadHttpProperties->SoapActionL(soapAction);
        if ( retVal == KErrNone )
            {
            pOriginalSoapAction = soapAction.AllocL();
            }
        CleanupStack::PopAndDestroy(pReadHttpProperties);
        CleanupStack::PopAndDestroy(pParser);
        CleanupStack::PopAndDestroy(pTransportPropertiesAsXml);
        }
        
    if ( pOriginalSoapAction )
        {
        CleanupStack::PushL(pOriginalSoapAction);
        }

    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();
    
    // 2. TP::SetSoapActionL(SOAPAction)
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetSoapActionL(KSoapActionValue);
    
    // 3. SendL(MSG, TP::SOAPAction)  // check that selected SOAPAction is used
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    // Check from HTTP Transaction logger log that defined SoapAction were used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);

    // 4. SendL(MSG) // original SOAPAction should be used (from step 1)    
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // Check from HTTP Transaction logger log that original SoapAction is used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    
    if( pValue && pOriginalSoapAction )
        {
        LOCAL_ASSERT(*pValue == *pOriginalSoapAction);
        }
//    else if ( pValue )
//        {
        // This is temporary: the default "" SOAPAction value has been removed from HTTPC:
        // also WS-I, REST and ID-WSF define this "" -value only on ::SendL (in message layer!)
        // This means, that "original soap action" from SC does not CURRENTLY yet provide the
        // framework default(!)
//        LOCAL_ASSERT( *pValue ==  KSenSoapActionHeaderValueEmpty ) // default from framework.
//        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    if ( pOriginalSoapAction )
        {
        CleanupStack::PopAndDestroy(pOriginalSoapAction);
        }    

    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }
    
TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_SoapAction2L(  )
    {
    // SendL(SOAP-ENV::SOAPAction)
    // 1. Get TP via SC:TransportPropertiesL() // store original soap-action
    // 2. SendL(SOAP-ENV::SOAPAction); // check SOAPAction from SoapEnvelope is sent/used
    // 3. SendL(MSG); // check that original SOAPACtion is used, from step 1
    // 4. SetTransportProperties(TP::SOAPAction2);
    // 5. SendL(MSG); // check that previous SC::SOAPAction is used, from step 4
    // 6. SendL(MSG, TP:SOAPAction); // check that TP:SOAPAction is used
    // 7. SendL(SOAP-ENV::SOAPAction); // check that SOAP-ENV:SOAPAction is used
    // 8. SendL(SOAP-ENV); // check that previous SC::SOAPAction is used, from step 4
    // 9. SendL(SOAP-ENV, TP::SOAPAction); // check that new TP::SOAPAction is used
    // 10. SendL(SOAP-ENV:SOAPAction, TP::SOAPAction ); // both are set: check that SOAP-ENV:SOAPAction is used, over-riding TP
    // 11. SendL(SOAP-ENV:SOAPAction ); SC::SOAPAction // only SOAP-ENV::SOAPAction is set: check that it is effective, instead of SC::SOAPAction
    // 12. SendL(MSG, TP:SOAPAction ); SC::SOAPAction // only TP::SOAPAction is set: check that it is effective, instead of SC::SOAPAction
    
    _LIT8(KSoapAction,                      "SOAPAction");
    _LIT8(KEnvelopeSoapActionValue,         "EnvelopeAction");
    _LIT8(KEnvelopeSoapActionReturnValue,   "\"EnvelopeAction\"");
    _LIT8(KSessionSoapActionValue,          "SessionAction");
    _LIT8(KSessionSoapActionReturnValue,    "\"SessionAction\"");
    _LIT8(KMessageSoapActionValue,          "MessageAction");
    _LIT8(KMessageSoapActionReturnValue,    "\"MessageAction\"");
    
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.

    // 1. Get TP via SC:TransportPropertiesL() // store original soap-action
    HBufC8* pOriginalSoapAction = NULL;
    HBufC8* pTransportPropertiesAsXml = NULL;
    iServiceConnection->TransportPropertiesL(pTransportPropertiesAsXml);
    if ( pTransportPropertiesAsXml )
        {
        CleanupStack::PushL(pTransportPropertiesAsXml);
        CSenXmlReader* pParser = CSenXmlReader::NewLC();
        CSenHttpTransportProperties* pReadHttpProperties = 
                    CSenHttpTransportProperties::NewLC(*pTransportPropertiesAsXml,
                                                       *pParser);
        TPtrC8 soapAction(KNullDesC8);
        TInt retVal = pReadHttpProperties->SoapActionL(soapAction);
        if ( retVal == KErrNone )
            {
            pOriginalSoapAction = soapAction.AllocL();
            }
        CleanupStack::PopAndDestroy(pReadHttpProperties);
        CleanupStack::PopAndDestroy(pParser);
        CleanupStack::PopAndDestroy(pTransportPropertiesAsXml);
        }

    if ( pOriginalSoapAction )
        {
        CleanupStack::PushL(pOriginalSoapAction);
        }

    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();
    
    // 2. SendL(SOAP-ENV::SOAPAction); // check SOAPAction from SoapEnvelope is sent/used
    pSOAPMessage->SetSoapActionL(KEnvelopeSoapActionValue);    
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);    
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    
    // Check from HTTP Transaction logger log that defined SoapAction were used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KEnvelopeSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);
    
    CleanupStack::PopAndDestroy(pSOAPMessage);
    // Create a new Soap message because old one contains defined SoapAction
    pSOAPMessage = CreateAuthRequestLC();

    // 3. SendL(MSG); // check that original SOAPACtion is used, from step 1
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // Check from HTTP Transaction logger log that original SoapAction were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if ( pValue && pOriginalSoapAction )
        {
        LOCAL_ASSERT(*pValue == *pOriginalSoapAction);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 4. SetTransportProperties(TP::SOAPAction2);
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetSoapActionL(KSessionSoapActionValue);
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SetTransportPropertiesL(*pSerializedProperties);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    // 5. SendL(MSG); // check that previous SC::SOAPAction is used, from step 4
    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);    
    
    // Check from HTTP Transaction logger log that defined (in step 4.) SoapAction were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KSessionSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    
    
    // 6. SendL(MSG, TP:SOAPAction); // check that TP:SOAPAction is used
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetSoapActionL(KMessageSoapActionValue);
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);    

    // Check from HTTP Transaction logger log that defined SoapAction were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KMessageSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 7. SendL(SOAP-ENV::SOAPAction); // check that SOAP-ENV:SOAPAction is used
    pSOAPMessage->SetSoapActionL(KEnvelopeSoapActionValue);    
    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);    

    // Check from HTTP Transaction logger log that defined SoapAction were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KEnvelopeSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);
    
    CleanupStack::PopAndDestroy(pSOAPMessage);
    // Create a new Soap message because old one contains defined SoapAction
    pSOAPMessage = CreateAuthRequestLC();
    
    // 8. SendL(SOAP-ENV); // check that previous SC::SOAPAction is used, from step 4
    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);    

    // Check from HTTP Transaction logger log that defined (in step 4.) SoapAction were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KSessionSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 9. SendL(SOAP-ENV, TP::SOAPAction); // check that new TP::SOAPAction is used
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetSoapActionL(KMessageSoapActionValue);
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);    

    // Check from HTTP Transaction logger log that defined SoapAction were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KMessageSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 10. SendL(SOAP-ENV:SOAPAction, TP::SOAPAction); // both are set: check that SOAP-ENV:SOAPAction is used, over-riding TP
    pSOAPMessage->SetSoapActionL(KEnvelopeSoapActionValue);    
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetSoapActionL(KMessageSoapActionValue);
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);    

    // Check from HTTP Transaction logger log that defined SoapAction were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KEnvelopeSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    CleanupStack::PopAndDestroy(pSOAPMessage);
    // Create a new Soap message because old one contains defined SoapAction
    pSOAPMessage = CreateAuthRequestLC();

    // 11. SendL(SOAP-ENV:SOAPAction ); SC::SOAPAction // only SOAP-ENV::SOAPAction is set: check that it is effective, instead of SC::SOAPAction
    // Note: SOAPAction for ServiceConnection was defined in step 4.
    pSOAPMessage->SetSoapActionL(KEnvelopeSoapActionValue);    
    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);    

    // Check from HTTP Transaction logger log that defined SoapAction were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KEnvelopeSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    CleanupStack::PopAndDestroy(pSOAPMessage);
    // Create a new Soap message because old one contains defined SoapAction
    pSOAPMessage = CreateAuthRequestLC();
    
    // 12. SendL(MSG, TP:SOAPAction ); SC::SOAPAction // only TP::SOAPAction is set: check that it is effective, instead of SC::SOAPAction
    // Note: SOAPAction for ServiceConnection was defined in step 4.
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetSoapActionL(KMessageSoapActionValue);
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);    

    // Check from HTTP Transaction logger log that defined SoapAction were used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KSoapAction);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KMessageSoapActionReturnValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    if ( pOriginalSoapAction )
        {
        CleanupStack::PopAndDestroy(pOriginalSoapAction);
        }    

    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }
    
TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_CustomHttpHeaderL(  )
    {
    // Set custom HTTP header
    // 1. Declare CUSTOM_HTTP_HEADER_NAME and CUSTOM_HTTP_HEADER_VALUE  (value should include Scandic characters, like Ä and Ö)
    // 2. Get TP using SC:TransportPropertiesL()
    // 3. SendL(MSG) // see that CUSTOM_HTTP_HEADER -field is not present
    // 4. SendL(MSG, TP::CUSTOM_HTTP_HEADER) // see that http filter logs CUSTOM_HTTP_HEADER
    // 5. Define TP::CUSTOM_HTTP_HEADER_VALUE2 (for already existing CUSTOM_HTTP_HEADER_NAME)
    // 6. Call SC::SetTransportPropertiesL(TP::CUSTOM_HTTP_HEADER_VALUE2) // VALUE2 should containt basic XML entities -- escapes like &, <, >, '   etc..
    // 7. SendL(MSG) // see that TP::CUSTOM_HTTP_HEADER_VALUE2 is effective

    // 1. Declare CUSTOM_HTTP_HEADER_NAME and CUSTOM_HTTP_HEADER_VALUE  (value should include Scandic characters, like Ä and Ö)
    _LIT8(KCustomHttpHeader,        "CustomHeader");
    _LIT8(KCustomHttpHeaderValue1,  "CustomHeaderValue1Ã…Ã„Ã–Ã¥Ã¤Ã¶"); // UTF8 String which contains: "CustomHeaderValue1ÅÄÖåäö"
    _LIT8(KCustomHttpHeaderValue2,  "CustomHeaderValue2<>&");
    HBufC8* pEncoded = SenXmlUtils::EncodeHttpCharactersLC(KCustomHttpHeaderValue2);
           
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    // 2. Get TP using SC:TransportPropertiesL()
    HBufC8* pTransportPropertiesAsXml = NULL;
    iServiceConnection->TransportPropertiesL(pTransportPropertiesAsXml);
    if ( pTransportPropertiesAsXml )
        {
        delete pTransportPropertiesAsXml;
        }

    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // 3. SendL(MSG) // see that CUSTOM_HTTP_HEADER -field is not present
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    // Check from HTTP Transaction logger log that CUSTOM_HTTP_HEADER was not used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, KCustomHttpHeader);
    if ( pValue )
        {
        LOCAL_ASSERT(pValue == NULL);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 4. SendL(MSG, TP::CUSTOM_HTTP_HEADER) // see that http filter logs CUSTOM_HTTP_HEADER
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pHttpProperties->SetHttpHeaderL(KCustomHttpHeader, KCustomHttpHeaderValue1);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    // Check from HTTP Transaction logger log that CUSTOM_HTTP_HEADER was used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KCustomHttpHeader);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KCustomHttpHeaderValue1);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    // 5. Define TP::CUSTOM_HTTP_HEADER_VALUE2 (for already existing CUSTOM_HTTP_HEADER_NAME)
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetIapIdL(iapId);
    pHttpProperties->SetHttpHeaderL(KCustomHttpHeader, *pEncoded);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    // 6. Call SC::SetTransportPropertiesL(TP::CUSTOM_HTTP_HEADER_VALUE2) // VALUE2 should containt basic XML entities -- escapes like &, <, >, '   etc..
    iServiceConnection->SetTransportPropertiesL(*pSerializedProperties);
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    // 7. SendL(MSG) // see that TP::CUSTOM_HTTP_HEADER_VALUE2 is effective
    iServiceConnection->SendL(*pSOAPMessage);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    // Check from HTTP Transaction logger log that CUSTOM_HTTP_HEADER was used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KCustomHttpHeader);
    RDebug::RawPrint( *pValue );
    if ( pValue )
        {
        RDebug::Print(_L("ENCODED:") );
        RDebug::RawPrint( *pEncoded );
        RDebug::Print(_L("OUTPUT:") );
        RDebug::RawPrint( *pValue );
        LOCAL_ASSERT(*pValue == *pEncoded);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    CleanupStack::PopAndDestroy(pSOAPMessage);
    CleanupStack::PopAndDestroy(pEncoded);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries1(  )
    {
    TInt result;
    _LIT8(KCustomHttpHeader,"CustomHeader");
    //_LIT8(KCustomHttpHeaderValue, ); // NULL cannot be used as descriptor literal value in Symbian
    //result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader,KCustomHttpHeaderValue);
    result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader, KNullDesC8);
    TL(result == KErrNone);
    return KErrNone;
    }


TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries2(  )
    {
    TInt result;
    // _LIT8(KCustomHttpHeader,); // NULL cannot be used as descriptor literal value in Symbian => it is more important to test KNullDesC8 (zero-length inputs etc)
    _LIT8(KCustomHttpHeader, "X"); // property name's length = 1
    _LIT8(KCustomHttpHeaderValue,  "CustomHeaderValue"); 
    
    // Note: XML specs do not allow <Element> (tag name) to have zero-length.

    // It is not a bad idea to create such a test case, where GOAL is actually test the http property class 
    // itself (public DLL which is run by client app process); Utils DLL should maybe leave after it has been made to 
    // regocnize illegal inputs. All the might check special chars, BUT --NOTE-- it would make the setters quite much 
    // heavier (in terms of performance) if the key names (xml tag names) are first validated.
    //result = UT_CSenServiceConnection_TP_Custom_Header(KNullDesC8, KCustomHttpHeaderValue);
    result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader, KCustomHttpHeaderValue);
    TL(result == KErrNone);
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries3(  )
    {
    TInt result;
    _LIT8(KCustomHttpHeader, "CustomHeader");
    result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader, KNullDesC8);
    //NOTE: --> we could create test case for length==1 case: _LIT8(KCustomHttpHeaderValue, "Y"); // property value's length = 1
    // _LIT8(KCustomHttpHeaderValue,  ""); // == KNullDesC8 
    //result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader, KCustomHttpHeaderValue);
    TL(result == KErrNone);
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries4(  )
    {
    TInt result;
    //_LIT8(KCustomHttpHeader,""); // == KNullDesC8
    _LIT8(KCustomHttpHeader, "X"); // property name's length = 1
    _LIT8(KCustomHttpHeaderValue, "CustomHeaderValue"); 
    //result = UT_CSenServiceConnection_TP_Custom_Header(KNullDesC8, KCustomHttpHeaderValue);
    result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader, KCustomHttpHeaderValue);
    
    TL(result == KErrNone);
    return KErrNone;
    }   

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries5(  )
    {
    TInt result;
    _LIT8(KCustomHttpHeader,"CustomHeader");
    _LIT8(KCustomHttpHeaderValue,  "256HeaderValuexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"); 
    result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader,KCustomHttpHeaderValue);
    TL (result == KErrNone);
    return KErrNone;
    }


TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries6(  )
    {
    TInt result;
    // Note: property name used to start with number here => changed, since it is against XML specs (property names are XML tags, parser does not accept tags which start by a number)

    // It is not a bad idea to create such a test case, where GOAL is actually test the http property class 
    // itself (public DLL which is run by client app process); Utils DLL should maybe leave after it has been made to 
    // regocnize illegal inputs. All the might check special chars, BUT --NOTE-- it would make the setters quite much 
    // heavier (in terms of performance) if the key names (xml tag names) are first validated.
    _LIT8(KCustomHttpHeader,"Header256xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    _LIT8(KCustomHttpHeaderValue,  "CustomHeaderValue"); 
    result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader,KCustomHttpHeaderValue);
    TL (result == KErrNone);
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries7(  )
    {
    TInt result;
    _LIT8(KCustomHttpHeader,"Header256xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    _LIT8(KCustomHttpHeaderValue,  "256HeaderValuexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"); 
    result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader,KCustomHttpHeaderValue);
    TL (result == KErrNone);
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries8(  )
    {
    TInt result;
    _LIT8(KCustomHttpHeader,"CustomHeader");
    _LIT8( KCRLFSP,       "\r\n " );
    //_LIT8(KCustomHttpHeaderValue,  "CustomHeaderValue55"); 
    result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader,KCRLFSP);
    TL (result == KErrNone);
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries9(  )
    {
    TInt result;
    _LIT8(KCustomHttpHeader,"CustomHeader");
    _LIT8(KCustomHttpHeaderValue,  "CustomHeaderValue1, CustomHeaderValue2"); 
    result = UT_CSenServiceConnection_TP_Custom_Header(KCustomHttpHeader,KCustomHttpHeaderValue);
    TL (result == KErrNone);
    return KErrNone;
    }

//Befor executing this TC remember to set big enought TimeOut value (10 min.)


TInt CsenConnectionTester::UT_CSenServiceConnection_TP_SecDialog()
    {
    TBool secDial;
               
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KHttpsEndPoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
   
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    

    pHttpProperties->SetSecureDialogL(ETrue);
    pHttpProperties->SecureDialogL(secDial);
    TL (secDial != EFalse);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetSecureDialogL(EFalse);
    pHttpProperties->SecureDialogL(secDial);
    TL (secDial == EFalse);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Omitted_True()
    {
    TBool secDial;
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    

    pHttpProperties->SetSecureDialogL( ETrue );
    pHttpProperties->SecureDialogL(secDial);
    TL (secDial != EFalse);
    pHttpProperties->SetOmittedL(KSecureDialogLocalName,ETrue);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);   
    
    TInt cmpVal = pSerializedProperties->Find(_L8("<SecureDialog Omitted=\"true\">true</SecureDialog>"));
    TL (cmpVal != KErrNotFound)
    
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Omitted_False()
    {
    TBool secDial;
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    

    pHttpProperties->SetSecureDialogL( ETrue );
    pHttpProperties->SecureDialogL(secDial);
    TL (secDial != EFalse);
    pHttpProperties->SetOmittedL(KSecureDialogLocalName, EFalse);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);   
    
    TInt cmpVal = pSerializedProperties->Find(_L8("<SecureDialog Omitted=\"false\">true</SecureDialog>"));
    TL (cmpVal != KErrNotFound)
    
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    return KErrNone;
    }


TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Removed()
    {
    TBool secDial;
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    

    pHttpProperties->SetSecureDialogL(ETrue);
    pHttpProperties->SecureDialogL(secDial);
    TL (secDial != EFalse);
    pHttpProperties->RemovePropertyL(KSecureDialogLocalName);
    
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);   
    
    TInt cmpVal = pSerializedProperties->Find(_L8("<SecureDialog>"));
    TL(cmpVal == KErrNotFound)
    
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Omit_Rem()
    {
    _LIT8(KUserAgentValue,          "TestAgent");
    _LIT8(KUserAgentDefaultValue,   "Serene");
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.
    
    // 1. Get TP using SC:TransportPropertiesL()
    HBufC8* pTransportPropertiesAsXml = NULL;
    iServiceConnection->TransportPropertiesL(pTransportPropertiesAsXml);
    if ( pTransportPropertiesAsXml )
        {
        delete pTransportPropertiesAsXml;
        }

    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // 2. SendL(MSG) // with set user-agent
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetUserAgentL(KUserAgentValue);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    //3. Check from HTTP Transaction logger log that user-agent was used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, KUserAgentLocalName);
    if( pValue )
        {
        LOCAL_ASSERT(*pValue == KUserAgentValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    
    
    
    // 4. SendL(MSG) // with removed user-agent
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetUserAgentL(KUserAgentValue);
    pHttpProperties->RemovePropertyL(KUserAgentLocalName);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    //5. Check from HTTP Transaction logger log that user-agent was used.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KUserAgentLocalName);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue != KUserAgentValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);        
    
    
    // 6. SendL(MSG) // with omitted set to EFalse for user-agent
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetUserAgentL(KUserAgentValue);
    pHttpProperties->SetOmittedL(KUserAgentLocalName, EFalse);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    //7. Check from HTTP Transaction logger log that given user-agent
    //   was used (when EFalse was given as Omitted value).
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KUserAgentLocalName);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KUserAgentValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);            
    
    // 8. SendL(MSG) // with omitted set to ETrue for user-agent
    pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetUserAgentL(KUserAgentValue);
    pHttpProperties->SetOmittedL(KUserAgentLocalName, ETrue);
    pHttpProperties->SetIapIdL(iapId);
    pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    //9. Check from HTTP Transaction logger log that 
    //   default user-agent for HTTP Transport plugin was
    //   used instead of user-agent which was set together
    //   with omitted value ETrue.
    pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    pValue = HttpPropertyValueFromXmlLC(*pFileContent, KUserAgentLocalName);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KUserAgentDefaultValue);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);                


    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }


TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_L(  )
    {
    // Create ServiceConnection
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.

    // 1. Chunk declaration
    _LIT8(  cid,            "123456789@123456789" );
    _LIT(   chunkName,      "test chunk" );
    _LIT8(  chunkContent,   "Testing chunk1" );
    TInt chunkSize =        1000;
    TInt chunkMaxSize =     5000;
    TInt chunkOffset =      100;
    TInt chunkDataSize =    14;
    RChunk binaryDataChunk;
    binaryDataChunk.CreateGlobal(chunkName(), chunkSize, chunkMaxSize);
    CleanupClosePushL(binaryDataChunk);
    TUint8* heapPtr = binaryDataChunk.Base();
    heapPtr = heapPtr + chunkOffset;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr-sizeof(TPtr8)) TPtr8(heapPtr, 0, chunkDataSize);
    // Set the descriptor
    TPtr8* pDes = reinterpret_cast<TPtr8*>(heapPtr-sizeof(TPtr8));
    pDes->Zero();
    pDes->Append(chunkContent);

    // 2. Chunk declaration
    _LIT8(  cid2,           "xyz" );
    _LIT(   chunkName2,     "test chunk2" );
    _LIT8(  chunkContent2,  "Testing chunk2" );
    TInt chunkSize2 =       3000;
    TInt chunkMaxSize2 =    10000;
    TInt chunkOffset2 =     112;
    TInt chunkDataSize2 =   14;
    
    RChunk binaryDataChunk2;
    binaryDataChunk2.CreateGlobal(chunkName2(), chunkSize2, chunkMaxSize2);
    CleanupClosePushL(binaryDataChunk2);
    TUint8* heapPtr2 = binaryDataChunk2.Base();
    heapPtr2 = heapPtr2 + chunkOffset2;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr2-sizeof(TPtr8)) TPtr8(heapPtr2, 0, chunkDataSize2);
    // Set the descriptor
    TPtr8* pDes2 = reinterpret_cast<TPtr8*>(heapPtr2-sizeof(TPtr8));
    pDes2->Zero();
    pDes2->Append(chunkContent2);
    
    // 1. File declaration
    _LIT8(  cid3,           "999999999@123456789" );
    _LIT(   KTestFileName,  "e:\\testing\\data\\fileContainer.txt" );
    _LIT8(  KFileContent,  "Test FileContainer1" );
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);
	TInt retVal = rfs.ShareProtected();
    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Replace(rfs, KTestFileName, EFileWrite));
    User::LeaveIfError(file.Write(KFileContent));
    file.Flush();
    file.Close();
    User::LeaveIfError(file.Open(rfs, KTestFileName, EFileRead));

    // 2. File declaration
    _LIT8(  cid4,           "1z2" );
    _LIT(   KTestFileName2, "e:\\testing\\data\\fileContainer2.txt" );
    _LIT8(  KFileContent2,  "Test FileContainer2" );
	RFs rfs2;
	User::LeaveIfError(rfs2.Connect());
	CleanupClosePushL(rfs2);
	retVal = rfs2.ShareProtected();
    RFile file2;
    CleanupClosePushL(file2);
    User::LeaveIfError(file2.Replace(rfs2, KTestFileName2, EFileWrite));
    User::LeaveIfError(file2.Write(KFileContent2));
    file2.Flush();
    file2.Close();
    User::LeaveIfError(file2.Open(rfs2, KTestFileName2, EFileRead));

    // Create SOAPMessage
    CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL();
    CleanupStack::PushL(pSOAPMessage);
    
    RSenDocument document = pSOAPMessage->AsDocumentL();
    
    // Create BinaryContainers
	TXmlEngChunkContainer chunkContainer = document.CreateChunkContainerL(
	                                                               cid,
	                                                               binaryDataChunk,
	                                                               chunkOffset,
	                                                               chunkDataSize);
	TXmlEngChunkContainer chunkContainer2 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
	TXmlEngChunkContainer chunkContainer3 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
    TXmlEngFileContainer fileContainer = document.CreateFileContainerL(cid3, file);
    TXmlEngFileContainer fileContainer2 = document.CreateFileContainerL(cid4, file2);
    
    // Get Envelope element
    TXmlEngElement envelopeElement = pSOAPMessage->AsElementL();
    
    // Create mime namespace declaration into Envelope element
    // xmlns:xmlmime="http://www.w3.org/2004/11/xmlmime"


    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2004/11/xmlmime"),
                                                                               _L8("xmlmime"));

//    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2004/11/xmlmime"),
//                                                                               _L8("mime")); // IE does not render XML where attribute name startes with "xml" (xmlmime)


    // Create service specific namespace declaration into Body element                                                                               
    // xmlns:m="http://example.org/stuff"
    TXmlEngNamespace serviceNamespace = pSOAPMessage->BodyL().AddNamespaceDeclarationL(
                                                                               _L8("http://example.org/stuff"),
                                                                               _L8("m"));
    // Add data element into Body
    // <S:Body xmlns:m="http://example.org/stuff">
    //     <m:data>
    //         ...
    //     </m:data>
    // </S:Body>
    TXmlEngElement dataElement = pSOAPMessage->BodyL().AddNewElementL(_L8("data"),
                                                                      serviceNamespace);
    
    // Add 1. attachment element into data element
    // <m:data>
    //     <m:textAttachment1 xmlmime:contentType="text">
    //         <xop:Include xmlns:xop="http://www.w3.org/2004/08/xop/include" href="cid:123456789@123456789"/>
    //     </m:textAttachment1>
    //     ...
    // </m:data>
    TXmlEngElement attachmentElement1 = dataElement.AddNewElementL(_L8("textAttachment1"),
                                                                   serviceNamespace);
    attachmentElement1.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement1.AppendChildL(chunkContainer);
	
    // Add 2. attachment element into data element
    TXmlEngElement attachmentElement2 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement2.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement2.AppendChildL(fileContainer);
	
    // Add 3. attachment element into data element
    TXmlEngElement attachmentElement3 = dataElement.AddNewElementL(_L8("textAttachment3"),
                                                                   serviceNamespace);
    attachmentElement3.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement3.AppendChildL(chunkContainer2);

    // Add 4. attachment element into data element
    TXmlEngElement attachmentElement4 = dataElement.AddNewElementL(_L8("textAttachment4"),
                                                                   serviceNamespace);
    attachmentElement4.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement4.AppendChildL(fileContainer2);

    // Add 5. attachment element inside data element
    TXmlEngElement attachmentElement5 = dataElement.AddNewElementL(_L8("textAttachment5"),
                                                                   serviceNamespace);
    attachmentElement5.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement5.AppendChildL(chunkContainer3);
    
    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    HBufC8* pPropsAsXml = pHttpProperties->AsUtf8LC();
    iServiceConnection->SetTransportPropertiesL(*pPropsAsXml);
    CleanupStack::PopAndDestroy(pPropsAsXml);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    CSenSoapEnvelope2* pRetMessage = NULL;
    iServiceConnection->SubmitL(*pSOAPMessage, pRetMessage);
    delete pRetMessage;

    // Check from HTTP Transaction logger log that POST method was used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodPost);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    CleanupStack::PopAndDestroy(&file2);
    CleanupStack::PopAndDestroy(&rfs2);
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&rfs);
    CleanupStack::PopAndDestroy(&binaryDataChunk2);
    CleanupStack::PopAndDestroy(&binaryDataChunk);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }


TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_with_Properties_L(  )
    {
    // Create ServiceConnection
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.

    // 1. Chunk declaration
    _LIT8(  cid,            "123456789@123456789" );
    _LIT(   chunkName,      "test chunk" );
    _LIT8(  chunkContent,   "Testing chunk1" );
    TInt chunkSize =        1000;
    TInt chunkMaxSize =     5000;
    TInt chunkOffset =      100;
    TInt chunkDataSize =    14;
    RChunk binaryDataChunk;
    binaryDataChunk.CreateGlobal(chunkName(), chunkSize, chunkMaxSize);
    CleanupClosePushL(binaryDataChunk);
    TUint8* heapPtr = binaryDataChunk.Base();
    heapPtr = heapPtr + chunkOffset;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr-sizeof(TPtr8)) TPtr8(heapPtr, 0, chunkDataSize);
    // Set the descriptor
    TPtr8* pDes = reinterpret_cast<TPtr8*>(heapPtr-sizeof(TPtr8));
    pDes->Zero();
    pDes->Append(chunkContent);

    // 2. Chunk declaration
    _LIT8(  cid2,           "xyz" );
    _LIT(   chunkName2,     "test chunk2" );
    _LIT8(  chunkContent2,  "Testing chunk2" );
    TInt chunkSize2 =       3000;
    TInt chunkMaxSize2 =    10000;
    TInt chunkOffset2 =     112;
    TInt chunkDataSize2 =   14;
    
    RChunk binaryDataChunk2;
    binaryDataChunk2.CreateGlobal(chunkName2(), chunkSize2, chunkMaxSize2);
    CleanupClosePushL(binaryDataChunk2);
    TUint8* heapPtr2 = binaryDataChunk2.Base();
    heapPtr2 = heapPtr2 + chunkOffset2;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr2-sizeof(TPtr8)) TPtr8(heapPtr2, 0, chunkDataSize2);
    // Set the descriptor
    TPtr8* pDes2 = reinterpret_cast<TPtr8*>(heapPtr2-sizeof(TPtr8));
    pDes2->Zero();
    pDes2->Append(chunkContent2);
    
    // 1. File declaration
    _LIT8(  cid3,           "999999999@123456789" );
    _LIT(   KTestFileName,  "e:\\testing\\data\\fileContainer.txt" );
    _LIT8(  KFileContent,  "Test FileContainer1" );
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);
	TInt retVal = rfs.ShareProtected();
    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Replace(rfs, KTestFileName, EFileWrite));
    User::LeaveIfError(file.Write(KFileContent));
    file.Flush();
    file.Close();
    User::LeaveIfError(file.Open(rfs, KTestFileName, EFileRead));

    // 2. File declaration
    _LIT8(  cid4,           "1z2" );
    _LIT(   KTestFileName2, "e:\\testing\\data\\fileContainer2.txt" );
    _LIT8(  KFileContent2,  "Test FileContainer2" );
	RFs rfs2;
	User::LeaveIfError(rfs2.Connect());
	CleanupClosePushL(rfs2);
	retVal = rfs2.ShareProtected();
    RFile file2;
    CleanupClosePushL(file2);
    User::LeaveIfError(file2.Replace(rfs2, KTestFileName2, EFileWrite));
    User::LeaveIfError(file2.Write(KFileContent2));
    file2.Flush();
    file2.Close();
    User::LeaveIfError(file2.Open(rfs2, KTestFileName2, EFileRead));

    // Create SOAPMessage
    CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL();
    CleanupStack::PushL(pSOAPMessage);
    
    RSenDocument document = pSOAPMessage->AsDocumentL();
    
    // Create BinaryContainers
	TXmlEngChunkContainer chunkContainer = document.CreateChunkContainerL(
	                                                               cid,
	                                                               binaryDataChunk,
	                                                               chunkOffset,
	                                                               chunkDataSize);
	TXmlEngChunkContainer chunkContainer2 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
	TXmlEngChunkContainer chunkContainer3 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
    TXmlEngFileContainer fileContainer = document.CreateFileContainerL(cid3, file);
    TXmlEngFileContainer fileContainer2 = document.CreateFileContainerL(cid4, file2);
    
    // Get Envelope element
    TXmlEngElement envelopeElement = pSOAPMessage->AsElementL();
    
    // Create mime namespace declaration into Envelope element
    // xmlns:xmlmime="http://www.w3.org/2004/11/xmlmime"
    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2004/11/xmlmime"),
                                                                               _L8("xmlmime"));
    // Create service specific namespace declaration into Body element                                                                               
    // xmlns:m="http://example.org/stuff"
    TXmlEngNamespace serviceNamespace = pSOAPMessage->BodyL().AddNamespaceDeclarationL(
                                                                               _L8("http://example.org/stuff"),
                                                                               _L8("m"));
    // Add data element into Body
    // <S:Body xmlns:m="http://example.org/stuff">
    //     <m:data>
    //         ...
    //     </m:data>
    // </S:Body>
    TXmlEngElement dataElement = pSOAPMessage->BodyL().AddNewElementL(_L8("data"),
                                                                      serviceNamespace);
    
    // Add 1. attachment element into data element
    // <m:data>
    //     <m:textAttachment1 xmlmime:contentType="text">
    //         <xop:Include xmlns:xop="http://www.w3.org/2004/08/xop/include" href="cid:123456789@123456789"/>
    //     </m:textAttachment1>
    //     ...
    // </m:data>
    TXmlEngElement attachmentElement1 = dataElement.AddNewElementL(_L8("textAttachment1"),
                                                                   serviceNamespace);
    attachmentElement1.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement1.AppendChildL(chunkContainer);
	
    // Add 2. attachment element into data element
    TXmlEngElement attachmentElement2 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement2.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement2.AppendChildL(fileContainer);
	
    // Add 3. attachment element into data element
    TXmlEngElement attachmentElement3 = dataElement.AddNewElementL(_L8("textAttachment3"),
                                                                   serviceNamespace);
    attachmentElement3.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement3.AppendChildL(chunkContainer2);

    // Add 4. attachment element into data element
    TXmlEngElement attachmentElement4 = dataElement.AddNewElementL(_L8("textAttachment4"),
                                                                   serviceNamespace);
    attachmentElement4.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement4.AppendChildL(fileContainer2);

    // Add 5. attachment element inside data element
    TXmlEngElement attachmentElement5 = dataElement.AddNewElementL(_L8("textAttachment5"),
                                                                   serviceNamespace);
    attachmentElement5.AddNewAttributeL(_L8("contentType"), _L8("text"), mimeNamespace);
	attachmentElement5.AppendChildL(chunkContainer3);
    
    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pSOAPMessage->SetProperties(pHttpProperties);
    CleanupStack::Pop(pHttpProperties); // ownership was transferred to message
        
    CSenSoapEnvelope2* pRetMessage = NULL;
    iServiceConnection->SubmitL(*pSOAPMessage, pRetMessage);
    delete pRetMessage;

    // Check from HTTP Transaction logger log that POST method was used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    LOCAL_ASSERT(pFileContent != NULL);
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpMethodValueFromXmlLC(*pFileContent);
    if ( pValue )
        {
        LOCAL_ASSERT(*pValue == KHTTPMethodPost);
        }
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);

    CleanupStack::PopAndDestroy(pSOAPMessage);
    
    CleanupStack::PopAndDestroy(&file2);
    CleanupStack::PopAndDestroy(&rfs2);
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&rfs);
    CleanupStack::PopAndDestroy(&binaryDataChunk2);
    CleanupStack::PopAndDestroy(&binaryDataChunk);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_20KBL(  )
    {
    // Create ServiceConnection

//    _LIT8(KSHCEndPoint, "http://172.21.10.144:8080/mtom/servlet/MtomServlet");
    _LIT8(KSHCEndPoint, "http://4FID02268.NOE.Nokia.com:8080/mtom/servlet/MtomServlet");
    
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KSHCEndPoint, KNullDesC8());
//  pPattern->SetFrameworkIdL(*ipFrameworkID);
    pPattern->SetFrameworkIdL(KRESTFrameworkID);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.

    // 1. Chunk declaration
    _LIT8(  cid,            "123456789@123456789" );
    _LIT(   chunkName,      "test chunk" );
    _LIT8(  chunkContent,   "Testing chunk1" );
    TInt chunkSize =        1000;
    TInt chunkMaxSize =     5000;
    TInt chunkOffset =      100;
    TInt chunkDataSize =    14;
    RChunk binaryDataChunk;
    binaryDataChunk.CreateGlobal(chunkName(), chunkSize, chunkMaxSize);
    CleanupClosePushL(binaryDataChunk);
    TUint8* heapPtr = binaryDataChunk.Base();
    heapPtr = heapPtr + chunkOffset;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr-sizeof(TPtr8)) TPtr8(heapPtr, 0, chunkDataSize);
    // Set the descriptor
    TPtr8* pDes = reinterpret_cast<TPtr8*>(heapPtr-sizeof(TPtr8));
    pDes->Zero();
    pDes->Append(chunkContent);

    // 2. Chunk declaration
    _LIT8(  cid2,           "xyz" );
    _LIT(   chunkName2,     "test chunk2" );
    _LIT8(  chunkContent2,  "Testing chunk2" );
    TInt chunkSize2 =       3000;
    TInt chunkMaxSize2 =    10000;
    TInt chunkOffset2 =     112;
    TInt chunkDataSize2 =   14;
    
    RChunk binaryDataChunk2;
    binaryDataChunk2.CreateGlobal(chunkName2(), chunkSize2, chunkMaxSize2);
    CleanupClosePushL(binaryDataChunk2);
    TUint8* heapPtr2 = binaryDataChunk2.Base();
    heapPtr2 = heapPtr2 + chunkOffset2;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr2-sizeof(TPtr8)) TPtr8(heapPtr2, 0, chunkDataSize2);
    // Set the descriptor
    TPtr8* pDes2 = reinterpret_cast<TPtr8*>(heapPtr2-sizeof(TPtr8));
    pDes2->Zero();
    pDes2->Append(chunkContent2);
    
    // 1. File declaration
    _LIT8(  cid3,           "999999999@123456789" );
    _LIT(   KTestFileName,  "e:\\testing\\data\\Sunset2.jpg" );
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);
	TInt retVal = rfs.ShareProtected();
    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Open(rfs, KTestFileName, EFileRead));

    // 2. File declaration
    _LIT8(  cid4,           "1z2" );
    _LIT(   KTestFileName2, "e:\\testing\\data\\fileContainer2.txt" );
    _LIT8(  KFileContent2,  "Test FileContainer2" );
	RFs rfs2;
	User::LeaveIfError(rfs2.Connect());
	CleanupClosePushL(rfs2);
	retVal = rfs2.ShareProtected();
    RFile file2;
    CleanupClosePushL(file2);
    User::LeaveIfError(file2.Replace(rfs2, KTestFileName2, EFileWrite));
    User::LeaveIfError(file2.Write(KFileContent2));
    file2.Flush();
    file2.Close();
    User::LeaveIfError(file2.Open(rfs2, KTestFileName2, EFileRead));

    // 3. File declaration
    _LIT8(  cid5,           "999999999@12789" );
    _LIT(   KTestFileName3,  "e:\\testing\\data\\Winter2.jpg" );
	RFs rfs3;
	User::LeaveIfError(rfs3.Connect());
	CleanupClosePushL(rfs3);
	retVal = rfs3.ShareProtected();
    RFile file3;
    CleanupClosePushL(file3);
    User::LeaveIfError(file3.Open(rfs3, KTestFileName3, EFileRead));

 
    // Create SOAPMessage
    CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL(ESOAP12);
    _LIT8(   KSoapActionValue,  "http://schemas.live.com/mws/2005/08/contacts/Sync" );
    pSOAPMessage->SetSoapActionL(KSoapActionValue);
//  CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL(); // ESOAP11
  	CleanupStack::PushL(pSOAPMessage);
    
    RSenDocument document = pSOAPMessage->AsDocumentL();
    
    // Create BinaryContainers
	TXmlEngChunkContainer chunkContainer = document.CreateChunkContainerL(
	                                                               cid,
	                                                               binaryDataChunk,
	                                                               chunkOffset,
	                                                               chunkDataSize);
	TXmlEngChunkContainer chunkContainer2 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
	TXmlEngChunkContainer chunkContainer3 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
    TXmlEngFileContainer fileContainer = document.CreateFileContainerL(cid3, file);
    TXmlEngFileContainer fileContainer2 = document.CreateFileContainerL(cid4, file2);
    TXmlEngFileContainer fileContainer3 = document.CreateFileContainerL(cid5, file3);
   
    
    // Get Envelope element
    TXmlEngElement envelopeElement = pSOAPMessage->AsElementL();
    
    // Create mime namespace declaration into Envelope element
    // xmlns:xmlmime="http://www.w3.org/2004/11/xmlmime"


    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2005/05/xmlmime"),
                                                                               _L8("xmlmime"));

//    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2004/11/xmlmime"),
//                                                                               _L8("mime")); // IE does not render XML where attribute name startes with "xml" (xmlmime)


    // Create service specific namespace declaration into Body element                                                                               
    // xmlns:m="http://example.org/stuff"
    TXmlEngNamespace serviceNamespace = pSOAPMessage->BodyL().AddNamespaceDeclarationL(
                                                                               _L8("http://example.org/stuff"),
                                                                               _L8("m"));
    // Add data element into Body
    // <S:Body xmlns:m="http://example.org/stuff">
    //     <m:data>
    //         ...
    //     </m:data>
    // </S:Body>
    TXmlEngElement dataElement = pSOAPMessage->BodyL().AddNewElementL(_L8("data"),
                                                                      serviceNamespace);
    
    // Add 1. attachment element into data element
    // <m:data>
    //     <m:textAttachment1 xmlmime:contentType="text">
    //         <xop:Include xmlns:xop="http://www.w3.org/2004/08/xop/include" href="cid:123456789@123456789"/>
    //     </m:textAttachment1>
    //     ...
    // </m:data>
    TXmlEngElement attachmentElement1 = dataElement.AddNewElementL(_L8("textAttachment1"),
                                                                   serviceNamespace);
    attachmentElement1.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement1.AppendChildL(chunkContainer);
	
    // Add 2. attachment element into data element
    TXmlEngElement attachmentElement2 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement2.AddNewAttributeL(_L8("contentType"), _L8("image/png"), mimeNamespace);
	attachmentElement2.AppendChildL(fileContainer);
	
    // Add 3. attachment element into data element
    TXmlEngElement attachmentElement3 = dataElement.AddNewElementL(_L8("textAttachment3"),
                                                                   serviceNamespace);
    attachmentElement3.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement3.AppendChildL(chunkContainer2);

    // Add 4. attachment element into data element
    TXmlEngElement attachmentElement4 = dataElement.AddNewElementL(_L8("textAttachment4"),
                                                                   serviceNamespace);
    attachmentElement4.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement4.AppendChildL(fileContainer2);

    // Add 5. attachment element inside data element
    TXmlEngElement attachmentElement5 = dataElement.AddNewElementL(_L8("textAttachment5"),
                                                                   serviceNamespace);
    attachmentElement5.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement5.AppendChildL(chunkContainer3);

    // Add 6. attachment element into data element
    TXmlEngElement attachmentElement6 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement6.AddNewAttributeL(_L8("contentType"), _L8("image/png"), mimeNamespace);
	attachmentElement6.AppendChildL(fileContainer3);
    
    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    HBufC8* pPropsAsXml = pHttpProperties->AsUtf8LC();
    iServiceConnection->SetTransportPropertiesL(*pPropsAsXml);
    CleanupStack::PopAndDestroy(pPropsAsXml);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    CSenSoapEnvelope2* pRetMessage = NULL;
    iServiceConnection->SubmitL(*pSOAPMessage, pRetMessage);
    delete pRetMessage;

    CleanupStack::PopAndDestroy(pSOAPMessage);
    CleanupStack::PopAndDestroy(&file3);
    CleanupStack::PopAndDestroy(&rfs3);
    CleanupStack::PopAndDestroy(&file2);
    CleanupStack::PopAndDestroy(&rfs2);
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&rfs);
    CleanupStack::PopAndDestroy(&binaryDataChunk2);
    CleanupStack::PopAndDestroy(&binaryDataChunk);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_200KBL(  )
    {
    // Create ServiceConnection

//    _LIT8(KSHCEndPoint, "http://172.21.10.144:8080/mtom/servlet/MtomServlet");
    _LIT8(KSHCEndPoint, "http://4FID02268.NOE.Nokia.com:8080/mtom/servlet/MtomServlet");
    
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KSHCEndPoint, KNullDesC8());
//  pPattern->SetFrameworkIdL(*ipFrameworkID);
    pPattern->SetFrameworkIdL(KRESTFrameworkID);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.

    // 1. Chunk declaration
    _LIT8(  cid,            "123456789@123456789" );
    _LIT(   chunkName,      "test chunk" );
    _LIT8(  chunkContent,   "Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1." );
    TInt chunkSize =        1000;
    TInt chunkMaxSize =     5000;
    TInt chunkOffset =      100;
    TInt chunkDataSize =    150;
    RChunk binaryDataChunk;
    binaryDataChunk.CreateGlobal(chunkName(), chunkSize, chunkMaxSize);
    CleanupClosePushL(binaryDataChunk);
    TUint8* heapPtr = binaryDataChunk.Base();
    heapPtr = heapPtr + chunkOffset;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr-sizeof(TPtr8)) TPtr8(heapPtr, 0, chunkDataSize);
    // Set the descriptor
    TPtr8* pDes = reinterpret_cast<TPtr8*>(heapPtr-sizeof(TPtr8));
    pDes->Zero();
    pDes->Append(chunkContent);

    // 2. Chunk declaration
    _LIT8(  cid2,           "xyz" );
    _LIT(   chunkName2,     "test chunk2" );
    _LIT8(  chunkContent2,  "Testing chunk2" );
    TInt chunkSize2 =       3000;
    TInt chunkMaxSize2 =    10000;
    TInt chunkOffset2 =     112;
    TInt chunkDataSize2 =   14;
    
    RChunk binaryDataChunk2;
    binaryDataChunk2.CreateGlobal(chunkName2(), chunkSize2, chunkMaxSize2);
    CleanupClosePushL(binaryDataChunk2);
    TUint8* heapPtr2 = binaryDataChunk2.Base();
    heapPtr2 = heapPtr2 + chunkOffset2;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr2-sizeof(TPtr8)) TPtr8(heapPtr2, 0, chunkDataSize2);
    // Set the descriptor
    TPtr8* pDes2 = reinterpret_cast<TPtr8*>(heapPtr2-sizeof(TPtr8));
    pDes2->Zero();
    pDes2->Append(chunkContent2);
    
    // 1. File declaration
    _LIT8(  cid3,           "999999999@123456789" );
    _LIT(   KTestFileName,  "e:\\testing\\data\\Sunset.jpg" );
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);
	TInt retVal = rfs.ShareProtected();
    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Open(rfs, KTestFileName, EFileRead));

    // 2. File declaration
    _LIT8(  cid4,           "1z2" );
    _LIT(   KTestFileName2, "e:\\testing\\data\\fileContainer2.txt" );
    _LIT8(  KFileContent2,  "Test FileContainer2" );
	RFs rfs2;
	User::LeaveIfError(rfs2.Connect());
	CleanupClosePushL(rfs2);
	retVal = rfs2.ShareProtected();
    RFile file2;
    CleanupClosePushL(file2);
    User::LeaveIfError(file2.Replace(rfs2, KTestFileName2, EFileWrite));
    User::LeaveIfError(file2.Write(KFileContent2));
    file2.Flush();
    file2.Close();
    User::LeaveIfError(file2.Open(rfs2, KTestFileName2, EFileRead));

    // 3. File declaration
    _LIT8(  cid5,           "999999999@12789" );
    _LIT(   KTestFileName3,  "e:\\testing\\data\\Winter.jpg" );
	RFs rfs3;
	User::LeaveIfError(rfs3.Connect());
	CleanupClosePushL(rfs3);
	retVal = rfs3.ShareProtected();
    RFile file3;
    CleanupClosePushL(file3);
    User::LeaveIfError(file3.Open(rfs3, KTestFileName3, EFileRead));

 
    // Create SOAPMessage
    CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL(ESOAP12);
    _LIT8(   KSoapActionValue,  "http://schemas.live.com/mws/2005/08/contacts/Sync" );
    pSOAPMessage->SetSoapActionL(KSoapActionValue);
//  CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL(); // ESOAP11
  	CleanupStack::PushL(pSOAPMessage);
    
    RSenDocument document = pSOAPMessage->AsDocumentL();
    
    // Create BinaryContainers
	TXmlEngChunkContainer chunkContainer = document.CreateChunkContainerL(
	                                                               cid,
	                                                               binaryDataChunk,
	                                                               chunkOffset,
	                                                               chunkDataSize);
	TXmlEngChunkContainer chunkContainer2 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
	TXmlEngChunkContainer chunkContainer3 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
    TXmlEngFileContainer fileContainer = document.CreateFileContainerL(cid3, file);
    TXmlEngFileContainer fileContainer2 = document.CreateFileContainerL(cid4, file2);
    TXmlEngFileContainer fileContainer3 = document.CreateFileContainerL(cid5, file3);
   
    
    // Get Envelope element
    TXmlEngElement envelopeElement = pSOAPMessage->AsElementL();
    
    // Create mime namespace declaration into Envelope element
    // xmlns:xmlmime="http://www.w3.org/2004/11/xmlmime"


    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2005/05/xmlmime"),
                                                                               _L8("xmlmime"));

//    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2004/11/xmlmime"),
//                                                                               _L8("mime")); // IE does not render XML where attribute name startes with "xml" (xmlmime)


    // Create service specific namespace declaration into Body element                                                                               
    // xmlns:m="http://example.org/stuff"
    TXmlEngNamespace serviceNamespace = pSOAPMessage->BodyL().AddNamespaceDeclarationL(
                                                                               _L8("http://example.org/stuff"),
                                                                               _L8("m"));
    // Add data element into Body
    // <S:Body xmlns:m="http://example.org/stuff">
    //     <m:data>
    //         ...
    //     </m:data>
    // </S:Body>
    TXmlEngElement dataElement = pSOAPMessage->BodyL().AddNewElementL(_L8("data"),
                                                                      serviceNamespace);
    
    // Add 1. attachment element into data element
    // <m:data>
    //     <m:textAttachment1 xmlmime:contentType="text">
    //         <xop:Include xmlns:xop="http://www.w3.org/2004/08/xop/include" href="cid:123456789@123456789"/>
    //     </m:textAttachment1>
    //     ...
    // </m:data>
    TXmlEngElement attachmentElement1 = dataElement.AddNewElementL(_L8("textAttachment1"),
                                                                   serviceNamespace);
    attachmentElement1.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement1.AppendChildL(chunkContainer);
	
    // Add 2. attachment element into data element
    TXmlEngElement attachmentElement2 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement2.AddNewAttributeL(_L8("contentType"), _L8("image/png"), mimeNamespace);
	attachmentElement2.AppendChildL(fileContainer);
	
    // Add 3. attachment element into data element
    TXmlEngElement attachmentElement3 = dataElement.AddNewElementL(_L8("textAttachment3"),
                                                                   serviceNamespace);
    attachmentElement3.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement3.AppendChildL(chunkContainer2);

    // Add 4. attachment element into data element
    TXmlEngElement attachmentElement4 = dataElement.AddNewElementL(_L8("textAttachment4"),
                                                                   serviceNamespace);
    attachmentElement4.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement4.AppendChildL(fileContainer2);

    // Add 5. attachment element inside data element
    TXmlEngElement attachmentElement5 = dataElement.AddNewElementL(_L8("textAttachment5"),
                                                                   serviceNamespace);
    attachmentElement5.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement5.AppendChildL(chunkContainer3);

    // Add 6. attachment element into data element
    TXmlEngElement attachmentElement6 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement6.AddNewAttributeL(_L8("contentType"), _L8("image/png"), mimeNamespace);
	attachmentElement6.AppendChildL(fileContainer3);
    
    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    HBufC8* pPropsAsXml = pHttpProperties->AsUtf8LC();
    iServiceConnection->SetTransportPropertiesL(*pPropsAsXml);
    CleanupStack::PopAndDestroy(pPropsAsXml);
    CleanupStack::PopAndDestroy(pHttpProperties);

    CSenSoapEnvelope2* pRetMessage = NULL;
    iServiceConnection->SubmitL(*pSOAPMessage, pRetMessage);
    delete pRetMessage;

    CleanupStack::PopAndDestroy(pSOAPMessage);
    CleanupStack::PopAndDestroy(&file3);
    CleanupStack::PopAndDestroy(&rfs3);
    CleanupStack::PopAndDestroy(&file2);
    CleanupStack::PopAndDestroy(&rfs2);
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&rfs);
    CleanupStack::PopAndDestroy(&binaryDataChunk2);
    CleanupStack::PopAndDestroy(&binaryDataChunk);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_Less1MBL(  )
    {
    // Create ServiceConnection

//    _LIT8(KSHCEndPoint, "http://172.21.10.144:8080/mtom/servlet/MtomServlet");    
    _LIT8(KSHCEndPoint, "http://4FID02268.NOE.Nokia.com:8080/mtom/servlet/MtomServlet");    
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KSHCEndPoint, KNullDesC8());
//  pPattern->SetFrameworkIdL(*ipFrameworkID);
    pPattern->SetFrameworkIdL(KRESTFrameworkID);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.

    // 1. Chunk declaration
    _LIT8(  cid,            "123456789@123456789" );
    _LIT(   chunkName,      "test chunk" );
    _LIT8(  chunkContent,   "Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1." );
    TInt chunkSize =        1000;
    TInt chunkMaxSize =     5000;
    TInt chunkOffset =      100;
    TInt chunkDataSize =    150;
    RChunk binaryDataChunk;
    binaryDataChunk.CreateGlobal(chunkName(), chunkSize, chunkMaxSize);
    CleanupClosePushL(binaryDataChunk);
    TUint8* heapPtr = binaryDataChunk.Base();
    heapPtr = heapPtr + chunkOffset;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr-sizeof(TPtr8)) TPtr8(heapPtr, 0, chunkDataSize);
    // Set the descriptor
    TPtr8* pDes = reinterpret_cast<TPtr8*>(heapPtr-sizeof(TPtr8));
    pDes->Zero();
    pDes->Append(chunkContent);

    // 2. Chunk declaration
    _LIT8(  cid2,           "xyz" );
    _LIT(   chunkName2,     "test chunk2" );
    _LIT8(  chunkContent2,  "Testing chunk2" );
    TInt chunkSize2 =       3000;
    TInt chunkMaxSize2 =    10000;
    TInt chunkOffset2 =     112;
    TInt chunkDataSize2 =   14;
    
    RChunk binaryDataChunk2;
    binaryDataChunk2.CreateGlobal(chunkName2(), chunkSize2, chunkMaxSize2);
    CleanupClosePushL(binaryDataChunk2);
    TUint8* heapPtr2 = binaryDataChunk2.Base();
    heapPtr2 = heapPtr2 + chunkOffset2;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr2-sizeof(TPtr8)) TPtr8(heapPtr2, 0, chunkDataSize2);
    // Set the descriptor
    TPtr8* pDes2 = reinterpret_cast<TPtr8*>(heapPtr2-sizeof(TPtr8));
    pDes2->Zero();
    pDes2->Append(chunkContent2);
    
    // 1. File declaration
    _LIT8(  cid3,           "999999999@123456789" );
    _LIT(   KTestFileName,  "e:\\testing\\data\\Sunset.jpg" );
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);
	TInt retVal = rfs.ShareProtected();
    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Open(rfs, KTestFileName, EFileRead));

    // 2. File declaration
    _LIT8(  cid4,           "1z2" );
    _LIT(   KTestFileName2, "e:\\testing\\data\\fileContainer2.txt" );
    _LIT8(  KFileContent2,  "Test FileContainer2" );
	RFs rfs2;
	User::LeaveIfError(rfs2.Connect());
	CleanupClosePushL(rfs2);
	retVal = rfs2.ShareProtected();
    RFile file2;
    CleanupClosePushL(file2);
    User::LeaveIfError(file2.Replace(rfs2, KTestFileName2, EFileWrite));
    User::LeaveIfError(file2.Write(KFileContent2));
    file2.Flush();
    file2.Close();
    User::LeaveIfError(file2.Open(rfs2, KTestFileName2, EFileRead));

    // 3. File declaration
    _LIT8(  cid5,           "999999999@12789" );
    _LIT(   KTestFileName3,  "e:\\testing\\data\\testPic3.jpg");
	RFs rfs3;
	User::LeaveIfError(rfs3.Connect());
	CleanupClosePushL(rfs3);
	retVal = rfs3.ShareProtected();
    RFile file3;
    CleanupClosePushL(file3);
    User::LeaveIfError(file3.Open(rfs3, KTestFileName3, EFileRead));

 
    // Create SOAPMessage
    CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL(ESOAP12);
    _LIT8(   KSoapActionValue,  "http://schemas.live.com/mws/2005/08/contacts/Sync" );
    pSOAPMessage->SetSoapActionL(KSoapActionValue);
//  CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL(); // ESOAP11
  	CleanupStack::PushL(pSOAPMessage);
    
    RSenDocument document = pSOAPMessage->AsDocumentL();
    
    // Create BinaryContainers
	TXmlEngChunkContainer chunkContainer = document.CreateChunkContainerL(
	                                                               cid,
	                                                               binaryDataChunk,
	                                                               chunkOffset,
	                                                               chunkDataSize);
	TXmlEngChunkContainer chunkContainer2 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
	TXmlEngChunkContainer chunkContainer3 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
    TXmlEngFileContainer fileContainer = document.CreateFileContainerL(cid3, file);
    TXmlEngFileContainer fileContainer2 = document.CreateFileContainerL(cid4, file2);
    TXmlEngFileContainer fileContainer3 = document.CreateFileContainerL(cid5, file3);
   
    
    // Get Envelope element
    TXmlEngElement envelopeElement = pSOAPMessage->AsElementL();
    
    // Create mime namespace declaration into Envelope element
    // xmlns:xmlmime="http://www.w3.org/2004/11/xmlmime"


    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2005/05/xmlmime"),
                                                                               _L8("xmlmime"));

//    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2004/11/xmlmime"),
//                                                                               _L8("mime")); // IE does not render XML where attribute name startes with "xml" (xmlmime)


    // Create service specific namespace declaration into Body element                                                                               
    // xmlns:m="http://example.org/stuff"
    TXmlEngNamespace serviceNamespace = pSOAPMessage->BodyL().AddNamespaceDeclarationL(
                                                                               _L8("http://example.org/stuff"),
                                                                               _L8("m"));
    // Add data element into Body
    // <S:Body xmlns:m="http://example.org/stuff">
    //     <m:data>
    //         ...
    //     </m:data>
    // </S:Body>
    TXmlEngElement dataElement = pSOAPMessage->BodyL().AddNewElementL(_L8("data"),
                                                                      serviceNamespace);
    
    // Add 1. attachment element into data element
    // <m:data>
    //     <m:textAttachment1 xmlmime:contentType="text">
    //         <xop:Include xmlns:xop="http://www.w3.org/2004/08/xop/include" href="cid:123456789@123456789"/>
    //     </m:textAttachment1>
    //     ...
    // </m:data>
    TXmlEngElement attachmentElement1 = dataElement.AddNewElementL(_L8("textAttachment1"),
                                                                   serviceNamespace);
    attachmentElement1.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement1.AppendChildL(chunkContainer);
	
    // Add 2. attachment element into data element
    TXmlEngElement attachmentElement2 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement2.AddNewAttributeL(_L8("contentType"), _L8("image/png"), mimeNamespace);
	attachmentElement2.AppendChildL(fileContainer);
	
    // Add 3. attachment element into data element
    TXmlEngElement attachmentElement3 = dataElement.AddNewElementL(_L8("textAttachment3"),
                                                                   serviceNamespace);
    attachmentElement3.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement3.AppendChildL(chunkContainer2);

    // Add 4. attachment element into data element
    TXmlEngElement attachmentElement4 = dataElement.AddNewElementL(_L8("textAttachment4"),
                                                                   serviceNamespace);
    attachmentElement4.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement4.AppendChildL(fileContainer2);

    // Add 5. attachment element inside data element
    TXmlEngElement attachmentElement5 = dataElement.AddNewElementL(_L8("textAttachment5"),
                                                                   serviceNamespace);
    attachmentElement5.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement5.AppendChildL(chunkContainer3);

    // Add 6. attachment element into data element
    TXmlEngElement attachmentElement6 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement6.AddNewAttributeL(_L8("contentType"), _L8("image/png"), mimeNamespace);
	attachmentElement6.AppendChildL(fileContainer3);
    
    
    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    HBufC8* pPropsAsXml = pHttpProperties->AsUtf8LC();
    iServiceConnection->SetTransportPropertiesL(*pPropsAsXml);
    CleanupStack::PopAndDestroy(pPropsAsXml);
    CleanupStack::PopAndDestroy(pHttpProperties);

   
    CSenSoapEnvelope2* pRetMessage = NULL;
    iServiceConnection->SubmitL(*pSOAPMessage, pRetMessage);
    delete pRetMessage;

    CleanupStack::PopAndDestroy(pSOAPMessage);
    CleanupStack::PopAndDestroy(&file3);
    CleanupStack::PopAndDestroy(&rfs3);
    CleanupStack::PopAndDestroy(&file2);
    CleanupStack::PopAndDestroy(&rfs2);
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&rfs);
    CleanupStack::PopAndDestroy(&binaryDataChunk2);
    CleanupStack::PopAndDestroy(&binaryDataChunk);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_More1MBL(  )
    {
    // Create ServiceConnection

//    _LIT8(KSHCEndPoint, "http://172.21.10.144:8080/mtom/servlet/MtomServlet");    
    _LIT8(KSHCEndPoint, "http://4FID02268.NOE.Nokia.com:8080/mtom/servlet/MtomServlet");    
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KSHCEndPoint, KNullDesC8());
//  pPattern->SetFrameworkIdL(*ipFrameworkID);
    pPattern->SetFrameworkIdL(KRESTFrameworkID);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pPattern->SetConsumerIapIdL(iapId);
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    // Connection is ready to be used.

    // 1. Chunk declaration
    _LIT8(  cid,            "123456789@123456789" );
    _LIT(   chunkName,      "test chunk" );
    _LIT8(  chunkContent,   "Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1.Testing chunk1." );
    TInt chunkSize =        1000;
    TInt chunkMaxSize =     5000;
    TInt chunkOffset =      100;
    TInt chunkDataSize =    150;
    RChunk binaryDataChunk;
    binaryDataChunk.CreateGlobal(chunkName(), chunkSize, chunkMaxSize);
    CleanupClosePushL(binaryDataChunk);
    TUint8* heapPtr = binaryDataChunk.Base();
    heapPtr = heapPtr + chunkOffset;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr-sizeof(TPtr8)) TPtr8(heapPtr, 0, chunkDataSize);
    // Set the descriptor
    TPtr8* pDes = reinterpret_cast<TPtr8*>(heapPtr-sizeof(TPtr8));
    pDes->Zero();
    pDes->Append(chunkContent);

    // 2. Chunk declaration
    _LIT8(  cid2,           "xyz" );
    _LIT(   chunkName2,     "test chunk2" );
    _LIT8(  chunkContent2,  "Testing chunk2" );
    TInt chunkSize2 =       3000;
    TInt chunkMaxSize2 =    10000;
    TInt chunkOffset2 =     112;
    TInt chunkDataSize2 =   14;
    
    RChunk binaryDataChunk2;
    binaryDataChunk2.CreateGlobal(chunkName2(), chunkSize2, chunkMaxSize2);
    CleanupClosePushL(binaryDataChunk2);
    TUint8* heapPtr2 = binaryDataChunk2.Base();
    heapPtr2 = heapPtr2 + chunkOffset2;
    // Place a new descriptor in the chunk to initialize it
    new (heapPtr2-sizeof(TPtr8)) TPtr8(heapPtr2, 0, chunkDataSize2);
    // Set the descriptor
    TPtr8* pDes2 = reinterpret_cast<TPtr8*>(heapPtr2-sizeof(TPtr8));
    pDes2->Zero();
    pDes2->Append(chunkContent2);
    
    // 1. File declaration
    _LIT8(  cid3,           "999999999@123456789" );
    _LIT(   KTestFileName,  "e:\\testing\\data\\Sunset.jpg" );
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);
	TInt retVal = rfs.ShareProtected();
    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Open(rfs, KTestFileName, EFileRead));

    // 2. File declaration
    _LIT8(  cid4,           "1z2" );
    _LIT(   KTestFileName2, "e:\\testing\\data\\fileContainer2.txt" );
    _LIT8(  KFileContent2,  "Test FileContainer2" );
	RFs rfs2;
	User::LeaveIfError(rfs2.Connect());
	CleanupClosePushL(rfs2);
	retVal = rfs2.ShareProtected();
    RFile file2;
    CleanupClosePushL(file2);
    User::LeaveIfError(file2.Replace(rfs2, KTestFileName2, EFileWrite));
    User::LeaveIfError(file2.Write(KFileContent2));
    file2.Flush();
    file2.Close();
    User::LeaveIfError(file2.Open(rfs2, KTestFileName2, EFileRead));

    // 3. File declaration
    _LIT8(  cid5,           "999999999@12789" );
    _LIT(   KTestFileName3,  "e:\\testing\\data\\testPic4.jpg");
	RFs rfs3;
	User::LeaveIfError(rfs3.Connect());
	CleanupClosePushL(rfs3);
	retVal = rfs3.ShareProtected();
    RFile file3;
    CleanupClosePushL(file3);
    User::LeaveIfError(file3.Open(rfs3, KTestFileName3, EFileRead));

 
    // Create SOAPMessage
    CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL(ESOAP12);
    _LIT8(   KSoapActionValue,  "http://schemas.live.com/mws/2005/08/contacts/Sync" );
    pSOAPMessage->SetSoapActionL(KSoapActionValue);
//  CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL(); // ESOAP11
  	CleanupStack::PushL(pSOAPMessage);
    
    RSenDocument document = pSOAPMessage->AsDocumentL();
    
    // Create BinaryContainers
	TXmlEngChunkContainer chunkContainer = document.CreateChunkContainerL(
	                                                               cid,
	                                                               binaryDataChunk,
	                                                               chunkOffset,
	                                                               chunkDataSize);
	TXmlEngChunkContainer chunkContainer2 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
	TXmlEngChunkContainer chunkContainer3 = document.CreateChunkContainerL(
	                                                               cid2,
	                                                               binaryDataChunk2,
	                                                               chunkOffset2,
	                                                               chunkDataSize2);
    TXmlEngFileContainer fileContainer = document.CreateFileContainerL(cid3, file);
    TXmlEngFileContainer fileContainer2 = document.CreateFileContainerL(cid4, file2);
    TXmlEngFileContainer fileContainer3 = document.CreateFileContainerL(cid5, file3);
   
    
    // Get Envelope element
    TXmlEngElement envelopeElement = pSOAPMessage->AsElementL();
    
    // Create mime namespace declaration into Envelope element
    // xmlns:xmlmime="http://www.w3.org/2004/11/xmlmime"


    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2005/05/xmlmime"),
                                                                               _L8("xmlmime"));

//    TXmlEngNamespace mimeNamespace = envelopeElement. AddNamespaceDeclarationL(_L8("http://www.w3.org/2004/11/xmlmime"),
//                                                                               _L8("mime")); // IE does not render XML where attribute name startes with "xml" (xmlmime)


    // Create service specific namespace declaration into Body element                                                                               
    // xmlns:m="http://example.org/stuff"
    TXmlEngNamespace serviceNamespace = pSOAPMessage->BodyL().AddNamespaceDeclarationL(
                                                                               _L8("http://example.org/stuff"),
                                                                               _L8("m"));
    // Add data element into Body
    // <S:Body xmlns:m="http://example.org/stuff">
    //     <m:data>
    //         ...
    //     </m:data>
    // </S:Body>
    TXmlEngElement dataElement = pSOAPMessage->BodyL().AddNewElementL(_L8("data"),
                                                                      serviceNamespace);
    
    // Add 1. attachment element into data element
    // <m:data>
    //     <m:textAttachment1 xmlmime:contentType="text">
    //         <xop:Include xmlns:xop="http://www.w3.org/2004/08/xop/include" href="cid:123456789@123456789"/>
    //     </m:textAttachment1>
    //     ...
    // </m:data>
    TXmlEngElement attachmentElement1 = dataElement.AddNewElementL(_L8("textAttachment1"),
                                                                   serviceNamespace);
    attachmentElement1.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement1.AppendChildL(chunkContainer);
	
    // Add 2. attachment element into data element
    TXmlEngElement attachmentElement2 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement2.AddNewAttributeL(_L8("contentType"), _L8("image/png"), mimeNamespace);
	attachmentElement2.AppendChildL(fileContainer);
	
    // Add 3. attachment element into data element
    TXmlEngElement attachmentElement3 = dataElement.AddNewElementL(_L8("textAttachment3"),
                                                                   serviceNamespace);
    attachmentElement3.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement3.AppendChildL(chunkContainer2);

    // Add 4. attachment element into data element
    TXmlEngElement attachmentElement4 = dataElement.AddNewElementL(_L8("textAttachment4"),
                                                                   serviceNamespace);
    attachmentElement4.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement4.AppendChildL(fileContainer2);

    // Add 5. attachment element inside data element
    TXmlEngElement attachmentElement5 = dataElement.AddNewElementL(_L8("textAttachment5"),
                                                                   serviceNamespace);
    attachmentElement5.AddNewAttributeL(_L8("contentType"), _L8("text/plain"), mimeNamespace);
	attachmentElement5.AppendChildL(chunkContainer3);

    // Add 6. attachment element into data element
    TXmlEngElement attachmentElement6 = dataElement.AddNewElementL(_L8("textAttachment2"),
                                                                   serviceNamespace);
    attachmentElement6.AddNewAttributeL(_L8("contentType"), _L8("image/png"), mimeNamespace);
	attachmentElement6.AppendChildL(fileContainer3);
    
    
    // Http Post
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    HBufC8* pPropsAsXml = pHttpProperties->AsUtf8LC();
    iServiceConnection->SetTransportPropertiesL(*pPropsAsXml);
    CleanupStack::PopAndDestroy(pPropsAsXml);
    CleanupStack::PopAndDestroy(pHttpProperties);

   
    CSenSoapEnvelope2* pRetMessage = NULL;
    iServiceConnection->SubmitL(*pSOAPMessage, pRetMessage);
    delete pRetMessage;

    CleanupStack::PopAndDestroy(pSOAPMessage);
    CleanupStack::PopAndDestroy(&file3);
    CleanupStack::PopAndDestroy(&rfs3);
    CleanupStack::PopAndDestroy(&file2);
    CleanupStack::PopAndDestroy(&rfs2);
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&rfs);
    CleanupStack::PopAndDestroy(&binaryDataChunk2);
    CleanupStack::PopAndDestroy(&binaryDataChunk);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }
    
TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessengerL(  )
    {
    RegisterSTSL();
    
    CreateConnectionToMessengerServiceL();
    
    StartActiveScheduler(1);

    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    TL (iServiceConnection != (CSenServiceConnection*)NULL);
    
    CSenVtcpTransportProperties* pProperties = CreateMSTransportPropertiesL(EMSNMessengerRegister);
    CleanupStack::PushL(pProperties);
    iServiceConnection->SetTransportPropertiesL(*pProperties->AsUtf8LC());
    CleanupStack::PopAndDestroy(); //pVtcpProperties->AsUtf8LC()
    CleanupStack::PopAndDestroy(pProperties);

    TInt sendRetCode(KErrNone);
    
    // Send Register message to Messenger
    CSenSoapMessage2* pSoapMessage = CreateMSSoapMessage(EMSNMessengerRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);

    // Send UnRegister message to Messenger
    pSoapMessage = CreateMSSoapMessage(EMSNMessengerUnRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger2L(  )
    {
    UnregisterSTSL();
    RegisterSTSL();
    
    CreateConnectionToMessengerServiceL();
    
    StartActiveScheduler(1);

    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    TL (iServiceConnection != (CSenServiceConnection*)NULL);
    
    CSenVtcpTransportProperties* pProperties = CreateMSTransportPropertiesL(EMSNMessengerRegister);
    CleanupStack::PushL(pProperties);
    iServiceConnection->SetTransportPropertiesL(*pProperties->AsUtf8LC());
    CleanupStack::PopAndDestroy(); //pVtcpProperties->AsUtf8LC()
    CleanupStack::PopAndDestroy(pProperties);

    TInt sendRetCode(KErrNone);
    
    // Send Register message to Messenger
    CSenSoapMessage2* pSoapMessage = CreateMSSoapMessage(EMSNMessengerRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);

    // Send UnRegister message to Messenger
    pSoapMessage = CreateMSSoapMessage(EMSNMessengerUnRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);

    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);

    CSenServiceManager* pManager = CSenServiceManager::NewLC();

    // Get used IdentityProvider from ServiceConnection
    CSenIdentityProvider* pIdp = NULL;
    iServiceConnection->IdentityProviderL(pIdp);
    CleanupStack::PushL(pIdp);
        
    // Get used Credential from Credentials DB
    RCredentialArray credentialArray;
    CleanupClosePushL(credentialArray);
    RCredentialPropertiesArray credentialPropertiesArray;
    CleanupClosePushL(credentialPropertiesArray);
    LOCAL_ASSERT((pManager->CredentialsL(KMessagingEndpoint, *pIdp,
                                         credentialArray,
                                         credentialPropertiesArray)) == KErrNone);
                                         
    // Check that atleast one Credential were found.
    LOCAL_ASSERT(credentialArray.Count() > 0);
    
    CleanupStack::Pop(&credentialPropertiesArray);
    CleanupStack::Pop(&credentialArray);
    CleanupStack::Pop(pIdp);
    CleanupStack::PopAndDestroy(pManager);   

    delete iServiceConnection;
    iServiceConnection = NULL;

    CleanupStack::PushL(pIdp);
    CleanupClosePushL(credentialArray);
    CleanupClosePushL(credentialPropertiesArray);

    // Clean DBs STARTS
    //      1) Unregister STS ServiceDescription and
    //                 STS IdentityProvider
    //         => Unregistering IdentityProvider removes
    //            related Credentials from Credentials DB
    //
    UnregisterSTSL();
    
    //      2) Unregister actual Service (= Messaging) ServiceDescription
    pManager = CSenServiceManager::NewLC();
    CSenXmlServiceDescription* pPattern = CSenXmlServiceDescription::NewLC();
    pPattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pPattern->SetEndPointL(KMessagingEndpoint);
    pPattern->SetContractL(KMessagingContract);
    TInt error = pManager->UnregisterServiceDescriptionL( *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    CleanupStack::PopAndDestroy(pManager);   
    // Clean DBs ENDS
    
    RegisterSTSL();
    
    pManager = CSenServiceManager::NewLC();

    // Add Credential to Credentials DB
    // => When Connection is initialized, working Credential
    //    should be found from Credentials DB
    // => Working Credential should be used and
    //    request to get new SecurityToken should NOT be sent.
    LOCAL_ASSERT((pManager->AddCredentialL(KMessagingEndpoint,
                                           *pIdp,
                                           *credentialArray[0],
                                           *credentialPropertiesArray[0])) == KErrNone);

    CleanupStack::PopAndDestroy(pManager);   
    credentialPropertiesArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy(&credentialPropertiesArray);
    credentialArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy(&credentialArray);
    CleanupStack::PopAndDestroy(pIdp);
    
    CreateConnectionToMessengerServiceL();
    
    StartActiveScheduler(1);

    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    TL (iServiceConnection != (CSenServiceConnection*)NULL);
    
    pProperties = CreateMSTransportPropertiesL(EMSNMessengerRegister);
    CleanupStack::PushL(pProperties);
    iServiceConnection->SetTransportPropertiesL(*pProperties->AsUtf8LC());
    CleanupStack::PopAndDestroy(); //pVtcpProperties->AsUtf8LC()
    CleanupStack::PopAndDestroy(pProperties);

    // Send Register message to Messenger
    pSoapMessage = CreateMSSoapMessage(EMSNMessengerRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);

    // Send UnRegister message to Messenger
    pSoapMessage = CreateMSSoapMessage(EMSNMessengerUnRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger3L(  )
    {
    UnregisterSTSL();
    
    CSenServiceManager* manager = CSenServiceManager::NewLC();
    _LIT8(KWsStarContract,      "urn:wstar:sts");
    _LIT8(KAddressing,          "http://schemas.xmlsoap.org/ws/2006/10/addressing");
    _LIT8(KInvalidPassportPass, "Invalid");

    iAuthInfoAskCounter = 0;
    //Create Identity provider for STS 
    ipProviderForAuthProvider = CSenIdentityProvider::NewL(KStsEndpoint);
    ipProviderForAuthProvider->SetContractL(KWsStarContract);//(TDesC8)
    ipProviderForAuthProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    ipProviderForAuthProvider->SetProviderID(KProviderId);
    ipProviderForAuthProvider->SetUserInfoL( KPassportUser, KPassportUser, KPassportPass);
       
    //register Identity provider for STS 
    CSenIdentityProvider* pInvalidStsProvider = CSenIdentityProvider::NewLC(KStsEndpoint);
    pInvalidStsProvider->SetContractL(KWsStarContract);//(TDesC8)
    pInvalidStsProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pInvalidStsProvider->SetProviderID(KProviderId);
    pInvalidStsProvider->SetUserInfoL( KPassportUser, KPassportUser, KInvalidPassportPass);
    //pInvalidStsProvider->AsElement().AddAttrL(_L8("promptUserInfo"),_L8("false"));
    manager->RegisterIdentityProviderL( *pInvalidStsProvider );
    CleanupStack::PopAndDestroy(pInvalidStsProvider);

    //register SD for STS 
    CSenXmlServiceDescription* pattern = CSenXmlServiceDescription::NewLC();//KNullDesC8(), KWsStarContract); 
    pattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pattern->SetContractL(KWsStarContract);
    pattern->SetEndPointL(KStsEndpoint);
    pattern->AsElement().AddElementL(_L8("ProviderID")).SetContentL(KProviderId);
    SetPolicyL(pattern, KPolicyAdr,     KAddressing); //this RST2
    SetPolicyL(pattern, KPolicySOAP12,  KNullDesC8);   //this RST2

    SetPolicyL(pattern, KPolicyMT,      KMetadataEndpoint);
    SetPolicyL(pattern, KPolicyPassExt, KNullDesC8);
    SetPolicyL(pattern, KPolicyTLS,     KNullDesC8);
    TInt error = manager->RegisterServiceDescriptionL( *pattern);

    CleanupStack::PopAndDestroy(pattern);
    CleanupStack::PopAndDestroy(manager);
    
    CreateConnectionToMessengerServiceL();
    
    StartActiveScheduler(1);
    
    // Check that authentication info were asked exactly 3 times.
    LOCAL_ASSERT(iAuthInfoAskCounter == 1);

    // 
    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady );
    TL (iServiceConnection !=  (CSenServiceConnection*)NULL);
    
    CSenVtcpTransportProperties* pProperties = CreateMSTransportPropertiesL(EMSNMessengerRegister);
    CleanupStack::PushL(pProperties);
    iServiceConnection->SetTransportPropertiesL(*pProperties->AsUtf8LC());
    CleanupStack::PopAndDestroy(); //pVtcpProperties->AsUtf8LC()
    CleanupStack::PopAndDestroy(pProperties);

    TInt sendRetCode(KErrNone);
    
    // Send Register message to Messenger
    CSenSoapMessage2* pSoapMessage = CreateMSSoapMessage(EMSNMessengerRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);

    // Send UnRegister message to Messenger
    pSoapMessage = CreateMSSoapMessage(EMSNMessengerUnRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    
    delete ipProviderForAuthProvider;
    ipProviderForAuthProvider = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger4L(  )
    {
    UnregisterSTSL();
    
    CSenServiceManager* manager = CSenServiceManager::NewLC();
    _LIT8(KWsStarContract,      "urn:wstar:sts");
    _LIT8(KAddressing,          "http://schemas.xmlsoap.org/ws/2006/10/addressing");
    _LIT8(KInvalidPassportPass, "Invalid");

    iAuthInfoAskCounter = 0;
    //Create Identity provider for STS 
    ipProviderForAuthProvider = CSenIdentityProvider::NewL(KStsEndpoint);
    ipProviderForAuthProvider->SetContractL(KWsStarContract);//(TDesC8)
    ipProviderForAuthProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    ipProviderForAuthProvider->SetProviderID(KProviderId);
    ipProviderForAuthProvider->SetUserInfoL( KPassportUser, KPassportUser, KInvalidPassportPass);
       
    //register Identity provider for STS 
    CSenIdentityProvider* pInvalidStsProvider = CSenIdentityProvider::NewLC(KStsEndpoint);
    pInvalidStsProvider->SetContractL(KWsStarContract);//(TDesC8)
    pInvalidStsProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pInvalidStsProvider->SetProviderID(KProviderId);
    pInvalidStsProvider->SetUserInfoL( KPassportUser, KPassportUser, KInvalidPassportPass);
    manager->RegisterIdentityProviderL( *pInvalidStsProvider );
    CleanupStack::PopAndDestroy(pInvalidStsProvider);

    //register SD for STS 
    CSenXmlServiceDescription* pattern = CSenXmlServiceDescription::NewLC();//KNullDesC8(), KWsStarContract); 
    pattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pattern->SetContractL(KWsStarContract);
    pattern->SetEndPointL(KStsEndpoint);
    pattern->AsElement().AddElementL(_L8("ProviderID")).SetContentL(KProviderId);
    SetPolicyL(pattern, KPolicyAdr,     KAddressing); //this RST2
    SetPolicyL(pattern, KPolicySOAP12,  KNullDesC8);   //this RST2

    SetPolicyL(pattern, KPolicyMT,      KMetadataEndpoint);
    SetPolicyL(pattern, KPolicyPassExt, KNullDesC8);
    SetPolicyL(pattern, KPolicyTLS,     KNullDesC8);
    TInt error = manager->RegisterServiceDescriptionL( *pattern);

    CleanupStack::PopAndDestroy(pattern);
    CleanupStack::PopAndDestroy(manager);
    
    CreateConnectionToMessengerServiceL();
    
    StartActiveScheduler(1);
    
    // Check that authentication info were asked exactly 3 times.
    LOCAL_ASSERT(iAuthInfoAskCounter == 3);

    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusCreateFailed );
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    
    delete ipProviderForAuthProvider;
    ipProviderForAuthProvider = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger5L(  )
    {
    UnregisterSTSL();
    CSenServiceManager* manager = CSenServiceManager::NewLC();
    _LIT8(KWsStarContract,      "urn:wstar:sts");
    _LIT8(KAddressing,          "http://schemas.xmlsoap.org/ws/2006/10/addressing");
    _LIT8(KInvalidPassportPass, "Invalid");

    iAuthInfoAskCounter = 0;
    //Create Identity provider for STS 
    ipProviderForAuthProvider = CSenIdentityProvider::NewL(KStsEndpoint);
    ipProviderForAuthProvider->SetContractL(KWsStarContract);//(TDesC8)
    ipProviderForAuthProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    ipProviderForAuthProvider->SetProviderID(KProviderId);
    ipProviderForAuthProvider->SetUserInfoL( KPassportUser, KPassportUser, KInvalidPassportPass);
       
    //register Identity provider for STS 
    CSenIdentityProvider* pInvalidStsProvider = CSenIdentityProvider::NewLC(KStsEndpoint);
    pInvalidStsProvider->SetContractL(KWsStarContract);//(TDesC8)
    pInvalidStsProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pInvalidStsProvider->SetProviderID(KProviderId);
    pInvalidStsProvider->SetUserInfoL( KPassportUser, KPassportUser, KInvalidPassportPass);
    pInvalidStsProvider->AsElement().AddAttrL(_L8("promptUserInfo"),_L8("false"));
    manager->RegisterIdentityProviderL( *pInvalidStsProvider );
    CleanupStack::PopAndDestroy(pInvalidStsProvider);

    //register SD for STS 
    CSenXmlServiceDescription* pattern = CSenXmlServiceDescription::NewLC();//KNullDesC8(), KWsStarContract); 
    pattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pattern->SetContractL(KWsStarContract);
    pattern->SetEndPointL(KStsEndpoint);
    pattern->AsElement().AddElementL(_L8("ProviderID")).SetContentL(KProviderId);
    SetPolicyL(pattern, KPolicyAdr,     KAddressing); //this RST2
    SetPolicyL(pattern, KPolicySOAP12,  KNullDesC8);   //this RST2

    SetPolicyL(pattern, KPolicyMT,      KMetadataEndpoint);
    SetPolicyL(pattern, KPolicyPassExt, KNullDesC8);
    SetPolicyL(pattern, KPolicyTLS,     KNullDesC8);
    TInt error = manager->RegisterServiceDescriptionL( *pattern);

    CleanupStack::PopAndDestroy(pattern);
    CleanupStack::PopAndDestroy(manager);
    
    CreateConnectionToMessengerServiceL();
    
    StartActiveScheduler(1);
    
    // Check that authentication info were asked exactly 3 times.
    LOCAL_ASSERT( iAuthInfoAskCounter == 0 );

    LOCAL_ASSERT( iConnectionState == KSenConnectionStatusCreateFailed );
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    
    delete ipProviderForAuthProvider;
    ipProviderForAuthProvider = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger6L(  )
    {
    CSenServiceManager* manager = CSenServiceManager::NewLC();
    _LIT8(KWsStarContract,      "urn:wstar:sts");
    _LIT8(KAddressing,          "http://schemas.xmlsoap.org/ws/2006/10/addressing");
    _LIT8(KInvalidPassportPass, "Invalid");

    //register Identity provider for STS 
    CSenIdentityProvider* pInvalidStsProvider = CSenIdentityProvider::NewLC(KStsEndpoint);
    pInvalidStsProvider->SetContractL(KWsStarContract);//(TDesC8)
    pInvalidStsProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pInvalidStsProvider->SetProviderID(KProviderId);
    pInvalidStsProvider->SetUserInfoL( KPassportUser, KPassportUser, KInvalidPassportPass);
    manager->RegisterIdentityProviderL( *pInvalidStsProvider );
    CleanupStack::PopAndDestroy(pInvalidStsProvider);

    //register SD for STS 
    CSenXmlServiceDescription* pattern = CSenXmlServiceDescription::NewLC();//KNullDesC8(), KWsStarContract); 
    pattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pattern->SetContractL(KWsStarContract);
    pattern->SetEndPointL(KStsEndpoint);
    pattern->AsElement().AddElementL(_L8("ProviderID")).SetContentL(KProviderId);
    SetPolicyL(pattern, KPolicyAdr,     KAddressing); //this RST2
    SetPolicyL(pattern, KPolicySOAP12,  KNullDesC8);   //this RST2

    SetPolicyL(pattern, KPolicyMT,      KMetadataEndpoint);
    SetPolicyL(pattern, KPolicyPassExt, KNullDesC8);
    SetPolicyL(pattern, KPolicyTLS,     KNullDesC8);
    TInt error = manager->RegisterServiceDescriptionL( *pattern);

    CleanupStack::PopAndDestroy(pattern);
    CleanupStack::PopAndDestroy(manager);
    
    CreateConnectionToMessengerServiceWithoutCallbackL();
    
    StartActiveScheduler(1);

    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    TL (iServiceConnection != (CSenServiceConnection*)NULL);
    
    CSenVtcpTransportProperties* pProperties = CreateMSTransportPropertiesL(EMSNMessengerRegister);
    CleanupStack::PushL(pProperties);
    iServiceConnection->SetTransportPropertiesL(*pProperties->AsUtf8LC());
    CleanupStack::PopAndDestroy(); //pVtcpProperties->AsUtf8LC()
    CleanupStack::PopAndDestroy(pProperties);

    TInt sendRetCode(KErrNone);
    
    // Send Register message to Messenger
    CSenSoapMessage2* pSoapMessage = CreateMSSoapMessage(EMSNMessengerRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);

    // Send UnRegister message to Messenger
    pSoapMessage = CreateMSSoapMessage(EMSNMessengerUnRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    return KErrNone;
    }

TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger7L(  )
    {
    // Create one "fake" ServiceManager instance which keeps
    // Serene server up and running through whole test case.
    CSenServiceManager* manager = CSenServiceManager::NewLC();
    
    UnregisterSTSL();
    RegisterSTSL();
    
    CreateConnectionToMessengerServiceL();
    
    StartActiveScheduler(1);

    LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    TL (iServiceConnection != (CSenServiceConnection*)NULL);
    
    CSenVtcpTransportProperties* pProperties = CreateMSTransportPropertiesL(EMSNMessengerRegister);
    CleanupStack::PushL(pProperties);
    iServiceConnection->SetTransportPropertiesL(*pProperties->AsUtf8LC());
    CleanupStack::PopAndDestroy(); //pVtcpProperties->AsUtf8LC()
    CleanupStack::PopAndDestroy(pProperties);

    TInt sendRetCode(KErrNone);
    
    // Send Register message to Messenger
    CSenSoapMessage2* pSoapMessage = CreateMSSoapMessage(EMSNMessengerRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);
    
    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);

    // Send UnRegister message to Messenger
    pSoapMessage = CreateMSSoapMessage(EMSNMessengerUnRegister);
    CleanupStack::PushL(pSoapMessage);
    
    sendRetCode = iServiceConnection->SendL(*pSoapMessage);
    LOCAL_ASSERT(sendRetCode > KErrNone);

    StartActiveScheduler(1);
    
    LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    CleanupStack::PopAndDestroy(pSoapMessage);

    delete iServiceConnection;
    iServiceConnection = NULL;

    // Clean DBs STARTS
    //      1) Unregister STS IdentityProvider
    //         => Unregistering IdentityProvider removes
    //            related Credentials from Credentials DB
    //
    UnregisterSTSL();
    // Clean DBs ENDS
    
    RegisterSTSInvalidPasswordL();
    
    CreateConnectionToMessengerServiceL();
    
    StartActiveScheduler(1);

    LOCAL_ASSERT( iConnectionState == KSenConnectionStatusCreateFailed );
    LOCAL_ASSERT( iCallback        == ECallbackHandleError );
    LOCAL_ASSERT( iErrorCode       == -30320 ); // -30320 = KErrSenFailedAuthentication
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    
    CleanupStack::PopAndDestroy(manager);
    return KErrNone;
    }
    
int CsenConnectionTester::UT_CSenServiceConnection_TP_Custom_Header(const TDesC8& KCustomHttpHeader,
                                    const TDesC8& KCustomHttpHeaderValue)
    {
    // Set custom HTTP header
    // 1. Declare CUSTOM_HTTP_HEADER_NAME and CUSTOM_HTTP_HEADER_VALUE
    // 2. Get TP using SC:TransportPropertiesL()
    // 3. SendL(MSG, TP::CUSTOM_HTTP_HEADER) // see that CUSTOM_HTTP_HEADER -field is present
    // 4. Check from HTTP Transaction logger log that CUSTOM_HTTP_HEADER was used.
               
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KAuthServEndpoint, KNullDesC8());
    pPattern->SetFrameworkIdL(*ipFrameworkID);
    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pPattern);
    
    CleanupStack::PopAndDestroy(pPattern);
    
    StartActiveScheduler(1);
    //LOCAL_ASSERT(iConnectionState == KSenConnectionStatusReady);
    if(iConnectionState != KSenConnectionStatusReady) return KErrCouldNotConnect;
    // Connection is ready to be used.
    
    // 2. Get TP using SC:TransportPropertiesL()
    HBufC8* pTransportPropertiesAsXml = NULL;
    iServiceConnection->TransportPropertiesL(pTransportPropertiesAsXml);
    if ( pTransportPropertiesAsXml )
        {
        delete pTransportPropertiesAsXml;
        }

    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // 3. SendL(MSG) // with custom header
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, KFileName);
    pHttpProperties->SetHttpHeaderL(KCustomHttpHeader, KCustomHttpHeaderValue);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    //LOCAL_ASSERT(iCallback == ECallbackHandleMessage);

    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    
    //4. Check from HTTP Transaction logger log that CUSTOM_HTTP_HEADER was used.
    HBufC8* pFileContent = ReadFileL(KFileToRead);
    //LOCAL_ASSERT(pFileContent != NULL);
    if (pFileContent == NULL) return KErrNotFound;
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, KCustomHttpHeader);
    //LOCAL_ASSERT(pValue == NULL);
    if(pValue == NULL) return KErrNotFound;
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    
    

    CleanupStack::PopAndDestroy(pSOAPMessage);

    
    delete iServiceConnection;
    iServiceConnection = NULL;
    
    return KErrNone;    
    }    
    
TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Send(const TDesC8& aLogFileName, const TDesC8& aCustomHttpHeader,
                                                                const TDesC8& aCustomHttpHeaderValue, CSenServiceConnection* iServiceConnection)
    {
    TInt result = KErrNone;
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // SendL(MSG) // with custom header
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, aLogFileName);
    pHttpProperties->SetHttpHeaderL(aCustomHttpHeader, aCustomHttpHeaderValue);
    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    //LOCAL_ASSERT(iCallback == ECallbackHandleMessage);
    if(iCallback != ECallbackHandleMessage) return KErrGeneral;
   
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    CleanupStack::PopAndDestroy(pSOAPMessage);
    return result;
    }
    
    
TInt CsenConnectionTester::UT_CSenServiceConnection_TP_CheckLogs(const TDesC& aLogFilePath,  const TDesC8& aCustomHttpHeader, const TDesC8& aCustomHttpHeaderValue)
    {
    TInt result = KErrNone;
    HBufC8* pFileContent = ReadFileL(aLogFilePath);
    //LOCAL_ASSERT(pFileContent != NULL);
    if (pFileContent == NULL) return KErrNotFound;
    
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, aCustomHttpHeader);
    //LOCAL_ASSERT(*pValue == aCustomHttpHeaderValue);
    if (*pValue != aCustomHttpHeaderValue) return KErrNotFound;
    
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    return result;
    }


HBufC8* CsenConnectionTester::HttpMethodValueFromXmlLC(const TDesC8& aXml)
    {
    CSenXmlReader* pReader = CSenXmlReader::NewL();
    CleanupStack::PushL(pReader);
    CSenDomFragment* pDom = CSenDomFragment::NewL();
    CleanupStack::PushL(pDom);
    pReader->SetContentHandler(*pDom);
    pDom->SetReader(*pReader);
    pReader->ParseL(aXml);
    
    CSenElement& element = pDom->AsElement();
    CSenElement* pRequestElement = element.Element(_L8("Request"));
    CSenElement* pTPElement = pRequestElement->Element(_L8("Method"));
    
    HBufC8* pRetBuf = pTPElement->Content().AllocL();

    CleanupStack::PopAndDestroy(pDom);
    CleanupStack::PopAndDestroy(pReader);
    
    CleanupStack::PushL(pRetBuf);
    
    return pRetBuf;
    }
    
TInt CsenConnectionTester::Find(const TDesC8& aDestination,
                                    const TDesC8& aDescToFind,
                                    TInt aStartingPos )
    {
    TInt descLength = aDescToFind.Length();
    TInt length = aDestination.Length() - descLength + 1;
    for (TInt i = aStartingPos; i < length ; i++)
        {
        if (  aDestination.Mid(i,descLength) == aDescToFind )
            {
            return i;
            }
        }
        
    return KErrNotFound;
    }
    
TBool CsenConnectionTester::ReplaceAll(TPtr8 aDestination,
                                           const TDesC8& aFrom,
                                           const TDesC8& aTo)
    {
    TInt retVal(EFalse);
    
    TInt pos = Find(aDestination, aFrom, 0);
    if (pos != KErrNotFound) retVal = ETrue;
    
    while (pos != KErrNotFound)
        {
        aDestination.Replace(pos,aFrom.Length(),aTo);
        pos = Find(aDestination, aFrom, pos+1);
        }

    return retVal;
    }    
    
TBool CsenConnectionTester::DecodeHttpCharactersL(const TDesC8& aOriginal,
                                                      HBufC8*& aDecoded)
    {
    // Five basic entities as descriptors
    _LIT8(KEscapedApos,     "&apos;");
    _LIT8(KEscapedDblQuot,  "&quot;");
    _LIT8(KEscapedGt,       "&gt;");
    _LIT8(KEscapedLt,       "&lt;");
    _LIT8(KEscapedAmp,      "&amp;");
    
    // XML-escaping chars as descriptors
    _LIT8(KAmpersandDesC8,  "&");
    _LIT8(KAposDesC8,       "\'");
    _LIT8(KDblQuotDesC8,    "\"");
    _LIT8(KGtDesC8,         ">");
    _LIT8(KLtDesC8,         "<");
    
    delete aDecoded;
    aDecoded = NULL;

    // make a copy of the original
    aDecoded = aOriginal.AllocL();

    TBool retVal(EFalse);

    // replace all five basic entities with XML escaping counterpart,
    // if found.
    if (ReplaceAll(aDecoded->Des(), KEscapedApos(), KAposDesC8()))
        {
        retVal = ETrue;
        }
    if (ReplaceAll(aDecoded->Des(), KEscapedDblQuot(), KDblQuotDesC8()))
        {
        retVal = ETrue;
        }
    if (ReplaceAll(aDecoded->Des(), KEscapedGt(), KGtDesC8()))
        {
        retVal = ETrue;
        }
    if (ReplaceAll(aDecoded->Des(), KEscapedLt(), KLtDesC8()))
        {
        retVal = ETrue;
        }
    if (ReplaceAll(aDecoded->Des(), KEscapedAmp(), KAmpersandDesC8()))
        {
        retVal = ETrue;
        }

    return retVal;
    }

HBufC8* CsenConnectionTester::HttpPropertyValueFromXmlLC(const TDesC8& aXml,
                                                             const TDesC8& aPropertyName)
    {
    CSenXmlReader* pReader = CSenXmlReader::NewL();
    CleanupStack::PushL(pReader);
    CSenDomFragment* pDom = CSenDomFragment::NewL();
    CleanupStack::PushL(pDom);
    pReader->SetContentHandler(*pDom);
    pDom->SetReader(*pReader);
    pReader->ParseL(aXml);
    
    HBufC8* pRetBuf = NULL;
    
    CSenElement& element = pDom->AsElement();
    CSenElement* pRequestElement = element.Element(_L8("Request"));
    if ( pRequestElement )
        {
        CSenElement* pTPElement = pRequestElement->Element(_L8("TranportProperties"));
        if ( pTPElement )
            {
            CSenElement* pElement = pTPElement->Element(aPropertyName);
            if ( pElement )
                {
                pRetBuf = pElement->Content().AllocL();
                CleanupStack::PushL(pRetBuf);
                HBufC8* pNew = NULL;
                DecodeHttpCharactersL(*pRetBuf, pNew);
                CleanupStack::PopAndDestroy(pRetBuf);
                pRetBuf = pNew;
                }
            }
        }

    CleanupStack::PopAndDestroy(pDom);
    CleanupStack::PopAndDestroy(pReader);
    CleanupStack::PushL(pRetBuf);
    return pRetBuf;
    }
    
HBufC8* CsenConnectionTester::ReadFileL(const TDesC& aFilename)
    {
    RFs fsSession;
    CleanupClosePushL(fsSession);
    User::LeaveIfError(fsSession.Connect() == NULL);
    TEntry entry;
    User::LeaveIfError(fsSession.Entry(aFilename,entry));
    HBufC8* pFileContent = NULL;
    RFileReadStream fileInStream;
    CleanupClosePushL(fileInStream);
    if (fileInStream.Open(fsSession,aFilename,EFileRead) == KErrNone)
        {
        HBufC8* pTempFileContent = HBufC8::NewLC(entry.iSize+10);
        TRAPD(error,
            fileInStream.ReadL((TUint8*)pTempFileContent->Ptr(),entry.iSize);
            );
            
        if (!error)
            {
            TPtrC8 fileContent((TUint8*)pTempFileContent->Ptr(),entry.iSize);
            pFileContent = fileContent.AllocL();
            }
        CleanupStack::PopAndDestroy(pTempFileContent);
        }
    CleanupStack::PopAndDestroy(&fileInStream);
    CleanupStack::PopAndDestroy(&fsSession);
    return pFileContent;
    }
    
TUint32 CsenConnectionTester::SearchIAPIdByNameL( const TDesC& aIAPName )
    {
    TBool found = EFalse;
    CCommsDatabase* pDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(pDB);
    
    //open internet accesspoint table
    CCommsDbTableView* pDbTView = pDB->OpenTableLC(TPtrC(IAP));
    TInt retVal = pDbTView->GotoFirstRecord();
    TBuf<128> name; 
    TUint32 id = 0;
    //Find the correct record
    while ( retVal == KErrNone && found == EFalse )
        {
        pDbTView->ReadTextL(TPtrC(COMMDB_NAME), name);
        pDbTView->ReadUintL(TPtrC(COMMDB_ID), id);
        found = ( name == aIAPName ); 
        if ( !found )
            {
            retVal = pDbTView->GotoNextRecord();
            }
        }
    CleanupStack::PopAndDestroy(pDbTView);
    CleanupStack::PopAndDestroy(pDB);
    return id;
    }
    
CSenSoapMessage* CsenConnectionTester::CreateAuthRequestLC()
    {
    _LIT8(KRequest, "<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"PLAIN ANONYMOUS\" advisoryAuthnID=\"012345678901234\"/>");
    CSenSoapMessage* pSOAPMessage = CSenSoapMessage::NewL();
    CleanupStack::PushL(pSOAPMessage);

    //xxxxxxxx*************** CorrelationHeader starts ***************************
    CSenBaseElement* pElement = CSenBaseElement::NewL(_L8("urn:liberty:sb:2003-08"), _L8("Correlation"), _L8("sb:Correlation"));
    CleanupStack::PushL(pElement);

    pElement->AddAttrL(_L8("messageID"),_L8(""));
    pElement->AddAttrL(_L8("refToMessageID"),_L8(""));

    TTime t;
    t.HomeTime();
    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
    SenDateUtils::ToXmlDateTimeUtf8L(ts, t);
    pElement->AddAttrL(_L8("timestamp"), ts);

    pSOAPMessage->AddHeaderL(*pElement); // pElement will be owned by pSOAPMessage
    CleanupStack::Pop(pElement);
    //************************ CorrelationHeader ends ****************************

    pSOAPMessage->SetBodyL(KRequest);
    return pSOAPMessage;    
    }
    
CSenSoapMessage2* CsenConnectionTester::CreateNewAuthRequestLC()
    {
    _LIT8(KRequest, "<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"PLAIN ANONYMOUS\" advisoryAuthnID=\"012345678901234\"/>");
    CSenSoapMessage2* pSOAPMessage = CSenSoapMessage2::NewL();
    CleanupStack::PushL(pSOAPMessage);

    //xxxxxxxx*************** CorrelationHeader starts ***************************
    /*CSenBaseElement* pElement = CSenBaseElement::NewL(_L8("urn:liberty:sb:2003-08"), _L8("Correlation"), _L8("sb:Correlation"));
    CleanupStack::PushL(pElement);

    pElement->AddAttrL(_L8("messageID"),_L8(""));
    pElement->AddAttrL(_L8("refToMessageID"),_L8(""));

    TTime t;
    t.HomeTime();
    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
    SenDateUtils::ToXmlDateTimeUtf8L(ts, t);
    pElement->AddAttrL(_L8("timestamp"), ts);

    //pSOAPMessage->AddHeaderL(*pElement); // pElement will be owned by pSOAPMessage
    CleanupStack::Pop(pElement);*/
    //************************ CorrelationHeader ends ****************************

    pSOAPMessage->SetBodyL(KRequest);
    return pSOAPMessage;    
    }
    
CSenVtcpTransportProperties* CsenConnectionTester::CreateMSTransportPropertiesL(TMSCommands aCommand)
    {
#ifdef OLD_MSN_NAMESPACE_2005_08
    _LIT8(KRegisterAct, "http://schemas.live.com/mws/2005/08/messaging/Register");
    _LIT8(KInviteReqAct,"http://schemas.live.com/mws/2005/08/messaging/Invite");
    _LIT8(KSendReqAct,"http://schemas.live.com/mws/2005/08/messaging/Message");
#else
    _LIT8(KRegisterAct, "http://schemas.live.com/mws/2006/10/messaging/Register");    
    _LIT8(KInviteReqAct,"http://schemas.live.com/mws/2006/10/messaging/Invite");
    _LIT8(KSendReqAct,"http://schemas.live.com/mws/2006/10/messaging/Message");          
    
#endif

    CSenVtcpTransportProperties* pVtcpProperties = CSenVtcpTransportProperties::NewLC();
    pVtcpProperties->SetProxyHostL(KMSNProxy);
    //pVtcpProperties->SetOnewayMessageOnOffL(TRUE);
    
    pVtcpProperties->SetMwsNamespaceL(KMessagingNameSpace);
        
    switch (aCommand)
        {
        case EMSNMessengerRegister:
            {    
            pVtcpProperties->SetSoapActionL(KRegisterAct);
            break;
            }
        case EMSNMessengerUnRegister:
            {    
            pVtcpProperties->SetSoapActionL(KRegisterAct);
            break;
            }
        case EMSNMessengerInvite:
            {    
            pVtcpProperties->SetSoapActionL(KInviteReqAct);
            break;
            }
        case EMSNMessengerSend:
            {    
            pVtcpProperties->SetSoapActionL(KSendReqAct);
            break;
            }
        default:
            break;
        }
    
    CleanupStack::Pop(pVtcpProperties);
    
    return pVtcpProperties;
    }
    
CSenSoapMessage2* CsenConnectionTester::CreateMSSoapMessage(TMSCommands aCommand)
    {
#ifdef OLD_MSN_NAMESPACE_2005_08    
    _LIT8(KReristerReq, "<sip:RegisterRequest xmlns:sip=\"http://schemas.live.com/mws/2005/08/messaging\">\
    <sip:Expires>240</sip:Expires>\
    <sip:AutoOffline>0</sip:AutoOffline>\
    <sip:Capabilities>1074003969</sip:Capabilities>\
    <sip:SubscribedNotifications>31</sip:SubscribedNotifications>\
    <sip:MessengerArgot sip:Status=\"Online\" sip:PresenceUpdates=\"All\"/>\
    </sip:RegisterRequest>");
    
    _LIT8(KUnReristerReq, "<sip:RegisterRequest xmlns:sip=\"http://schemas.live.com/mws/2005/08/messaging\">\
    <sip:Expires>0</sip:Expires>\
    <sip:MessengerArgot sip:Status=\"Online\" sip:PresenceUpdates=\"All\"/>\
    </sip:RegisterRequest>");
    
    _LIT8(KInviteReq,
        "<sip:InviteRequest xmlns:sip=\"http://schemas.live.com/mws/2005/08/messaging\">\
            <sip:Call>\
                <mc:To xmlns:mc=\"http://schemas.live.com/mws/2005/08/contacts\">7553</mc:To>\
            </sip:Call>\
        </sip:InviteRequest>");
    
    _LIT8(KSendReq,          
    "<sip:MessageRequest xmlns:sip=\"http://schemas.live.com/mws/2005/08/messaging\">\
            <sip:Call>\
                <sip:CallID>1</sip:CallID>\
            </sip:Call>\
            <sip:MessageObject sip:ContentType=\"text/plain\">\
                <sip:MessageBody>lalamido</sip:MessageBody>\
            </sip:MessageObject>\
        </sip:MessageRequest>");
#else
    _LIT8(KReristerReq, "<sip:RegisterRequest xmlns:sip=\"http://schemas.live.com/mws/2006/10/messaging\">\
    <sip:Expires>240</sip:Expires>\
    <sip:AutoOffline>0</sip:AutoOffline>\
    <sip:Capabilities>1074003969</sip:Capabilities>\
    <sip:SubscribedNotifications>31</sip:SubscribedNotifications>\
    <sip:MessengerArgot sip:Status=\"Online\" sip:PresenceUpdates=\"All\"/>\
    </sip:RegisterRequest>");
     
    _LIT8(KUnReristerReq, "<sip:RegisterRequest xmlns:sip=\"http://schemas.live.com/mws/2006/10/messaging\">\
    <sip:Expires>0</sip:Expires>\
    <sip:MessengerArgot sip:Status=\"Online\" sip:PresenceUpdates=\"All\"/>\
    </sip:RegisterRequest>");
    
    _LIT8(KInviteReq,  
        "<sip:InviteRequest xmlns:sip=\"http://schemas.live.com/mws/2006/10/messaging\">\
            <sip:Call>\
                <mc:To xmlns:mc=\"http://schemas.live.com/mws/2006/10/contacts\">4878</mc:To>\
            </sip:Call>\
        </sip:InviteRequest>");
    
    _LIT8(KSendReq,          
    "<sip:MessageRequest xmlns:sip=\"http://schemas.live.com/mws/2006/10/messaging\">\
            <sip:Call>\
                <sip:CallID>1</sip:CallID>\
            </sip:Call>\
            <sip:MessageObject sip:ContentType=\"text/plain\">\
                <sip:MessageBody>lalamido</sip:MessageBody>\
            </sip:MessageObject>\
        </sip:MessageRequest>");
#endif    

    CSenSoapMessage2* pMessage = CSenSoapMessage2::NewLC(ESOAP12);

    AddDeviceOptionsHeader2L(*pMessage);
    
    AddApplicationHeader2L(*pMessage);
    
    switch (aCommand)
        {
        case EMSNMessengerRegister:
            {    
            pMessage->SetBodyL(KReristerReq);
            break;
            }
        case EMSNMessengerUnRegister:
            {    
            pMessage->SetBodyL(KUnReristerReq);
            break;
            }
        case EMSNMessengerInvite:
            {    
            pMessage->SetBodyL(KInviteReq);
            break;
            }
        case EMSNMessengerSend:
            {    
            pMessage->SetBodyL(KSendReq);
            break;
            }
        default:
            break;
        }
    
    CleanupStack::Pop(pMessage);
        
    return pMessage;
    }

void CsenConnectionTester::RegisterSTSL() 
    {
    CSenServiceManager* manager = CSenServiceManager::NewLC();
    _LIT8(KWsStarContract, "urn:wstar:sts");
    _LIT8(KAddressing, "http://schemas.xmlsoap.org/ws/2006/10/addressing");

    //register Identity provider for STS 
    CSenIdentityProvider* stsProvider = CSenIdentityProvider::NewLC(KStsEndpoint);
    stsProvider->SetContractL(KWsStarContract);//(TDesC8)
    stsProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    stsProvider->SetProviderID(KProviderId);
    stsProvider->SetUserInfoL( KPassportUser, KPassportUser, KPassportPass);                                           
    manager->RegisterIdentityProviderL( *stsProvider);
    CleanupStack::PopAndDestroy(stsProvider);
       
    //register SD for STS 
    CSenXmlServiceDescription* pattern = CSenXmlServiceDescription::NewLC();//KNullDesC8(), KWsStarContract); 
    pattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pattern->SetContractL(KWsStarContract);
    pattern->SetEndPointL(KStsEndpoint);
    pattern->AsElement().AddElementL(_L8("ProviderID")).SetContentL(KProviderId);
    SetPolicyL(pattern, KPolicyAdr,     KAddressing); //this RST2
    SetPolicyL(pattern, KPolicySOAP12,  KNullDesC8);   //this RST2

    SetPolicyL(pattern, KPolicyMT,      KMetadataEndpoint);
    SetPolicyL(pattern, KPolicyPassExt, KNullDesC8);
    SetPolicyL(pattern, KPolicyTLS,     KNullDesC8);
    TInt error = manager->RegisterServiceDescriptionL( *pattern);

    CleanupStack::PopAndDestroy(pattern);
    CleanupStack::PopAndDestroy(manager);
    }

void CsenConnectionTester::RegisterSTSInvalidPasswordL() 
    {
    CSenServiceManager* manager = CSenServiceManager::NewLC();
    _LIT8(KWsStarContract, "urn:wstar:sts");
    _LIT8(KAddressing, "http://schemas.xmlsoap.org/ws/2006/10/addressing");

    //register Identity provider for STS 
    CSenIdentityProvider* stsProvider = CSenIdentityProvider::NewLC(KStsEndpoint);
    stsProvider->SetContractL(KWsStarContract);//(TDesC8)
    stsProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    stsProvider->SetProviderID(KProviderId);
    stsProvider->SetUserInfoL( KPassportUser, KPassportUser, _L8("Invalid"));                                           
    manager->RegisterIdentityProviderL( *stsProvider);
    CleanupStack::PopAndDestroy(stsProvider);
       
    //register SD for STS 
    CSenXmlServiceDescription* pattern = CSenXmlServiceDescription::NewLC();//KNullDesC8(), KWsStarContract); 
    pattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pattern->SetContractL(KWsStarContract);
    pattern->SetEndPointL(KStsEndpoint);
    pattern->AsElement().AddElementL(_L8("ProviderID")).SetContentL(KProviderId);
    SetPolicyL(pattern, KPolicyAdr,     KAddressing); //this RST2
    SetPolicyL(pattern, KPolicySOAP12,  KNullDesC8);   //this RST2

    SetPolicyL(pattern, KPolicyMT,      KMetadataEndpoint);
    SetPolicyL(pattern, KPolicyPassExt, KNullDesC8);
    SetPolicyL(pattern, KPolicyTLS,     KNullDesC8);
    TInt error = manager->RegisterServiceDescriptionL( *pattern);

    CleanupStack::PopAndDestroy(pattern);
    CleanupStack::PopAndDestroy(manager);
    }

void CsenConnectionTester::UnregisterSTSL() 
    {
    CSenServiceManager* manager = CSenServiceManager::NewLC();
    _LIT8(KWsStarContract, "urn:wstar:sts");
    _LIT8(KAddressing, "http://schemas.xmlsoap.org/ws/2006/10/addressing");

    //Unregister Identity provider for STS 
    CSenIdentityProvider* stsProvider = CSenIdentityProvider::NewLC(KStsEndpoint);
    stsProvider->SetContractL(KWsStarContract);//(TDesC8)
    stsProvider->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    stsProvider->SetProviderID(KProviderId);
    stsProvider->SetUserInfoL( KPassportUser, KPassportUser, KPassportPass);                                           
    manager->UnregisterIdentityProviderL( *stsProvider);
    CleanupStack::PopAndDestroy(stsProvider);
       
    //Unregister SD for STS 
    CSenXmlServiceDescription* pattern = CSenXmlServiceDescription::NewLC();//KNullDesC8(), KWsStarContract); 
    pattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pattern->SetContractL(KWsStarContract);
    pattern->SetEndPointL(KStsEndpoint);
    pattern->AsElement().AddElementL(_L8("ProviderID")).SetContentL(KProviderId);
    SetPolicyL(pattern, KPolicyAdr,     KAddressing); //this RST2
    SetPolicyL(pattern, KPolicySOAP12,  KNullDesC8);   //this RST2

    SetPolicyL(pattern, KPolicyMT,      KMetadataEndpoint);
    SetPolicyL(pattern, KPolicyPassExt, KNullDesC8);
    SetPolicyL(pattern, KPolicyTLS,     KNullDesC8);
    TInt error = manager->UnregisterServiceDescriptionL( *pattern);

    CleanupStack::PopAndDestroy(pattern);
    CleanupStack::PopAndDestroy(manager);
    }
    
void CsenConnectionTester::SetPolicyL( CSenXmlServiceDescription* pServDesc,
                                           const TDesC8& pName,
                                           const TDesC8& pValue)
    {
    CSenElement* servicePolicy = pServDesc->AsElement().Element(_L8("ServicePolicy")); 
    CSenElement* clientPolicy = servicePolicy->Element(_L8("ClientPolicy"));     
    if(!clientPolicy)
        servicePolicy->AddElementL(_L8("ClientPolicy"));
    clientPolicy = servicePolicy->Element(_L8("ClientPolicy"));

    if(pValue==KNullDesC8())
        {
        clientPolicy->AddElementL(pName);
        }
        else
        {
        clientPolicy->AddElementL(pName).SetContentL(pValue);
        }
    }
    
void CsenConnectionTester::CreateConnectionToMessengerServiceL()
    {
    _LIT8(KAddressing, "http://schemas.xmlsoap.org/ws/2004/08/addressing");
    CSenXmlServiceDescription* pServiceDesc = CSenXmlServiceDescription::NewLC(KMessagingEndpoint, KMessagingContract);

    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pServiceDesc->SetIapIdL(iapId);
    
    pServiceDesc->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pServiceDesc->AsElement().AddElementL(_L8("ProviderID")).SetContentL(KProviderId);
    SetPolicyL(pServiceDesc, KPolicySOAP12, KNullDesC8);
    SetPolicyL(pServiceDesc, KPolicyAdr,    KAddressing);
    
    // create connection    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pServiceDesc, *this);
    CleanupStack::PopAndDestroy(pServiceDesc);
    }

void CsenConnectionTester::CreateConnectionToMessengerServiceWithoutCallbackL()
    {
    _LIT8(KAddressing, "http://schemas.xmlsoap.org/ws/2004/08/addressing");
    CSenXmlServiceDescription* pServiceDesc = CSenXmlServiceDescription::NewLC(KMessagingEndpoint, KMessagingContract);

    TUint32 iapId = SearchIAPIdByNameL(KIAPName);
    pServiceDesc->SetIapIdL(iapId);


    pServiceDesc->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    pServiceDesc->AsElement().AddElementL(_L8("ProviderID")).SetContentL(KProviderId);
    SetPolicyL(pServiceDesc, KPolicySOAP12, KNullDesC8);
    SetPolicyL(pServiceDesc, KPolicyAdr,    KAddressing);
    
    // create connection    
    iServiceConnection = CSenServiceConnection::NewL(*this, *pServiceDesc);
    CleanupStack::PopAndDestroy(pServiceDesc);
    }

void CsenConnectionTester::AddDeviceOptionsHeader2L(CSenSoapMessage2& aMessage)
    {
    TXmlEngElement header = aMessage.HeaderL();

    TXmlEngElement deviceOp = header.AddNewElementL(_L8("DeviceOptions"),
                                                    KMessagingNameSpace,
                                                    _L8("mws"));

    TXmlEngElement child = deviceOp.AddNewElementSameNsL(_L8("Locale"));
    child.AddTextL(_L8("1033"));

    TXmlEngElement child1 = deviceOp.AddNewElementSameNsL(_L8("UserAgent"));

    TXmlEngElement child1_1 = child1.AddNewElementSameNsL(_L8("Manufacturer"));
    child1_1.AddTextL(_L8("NO_UserAgent"));

    TXmlEngElement child1_2 = child1.AddNewElementSameNsL(_L8("Model"));
    child1_2.AddTextL(_L8("NO_Model"));

    TXmlEngElement child1_3 = child1.AddNewElementSameNsL(_L8("OS"));

    TXmlEngElement child1_3_1 = child1_3.AddNewElementSameNsL(_L8("Name"));
    child1_3_1.AddTextL(_L8("NO_OS"));

    TXmlEngElement child1_3_2 = child1_3.AddNewElementSameNsL(_L8("Version"));
    child1_3_2.AddTextL(_L8("NO_Version"));
    }

void CsenConnectionTester::AddApplicationHeader2L(CSenSoapMessage2& aMessage)
    {
    TXmlEngElement header = aMessage.HeaderL();

    TXmlEngElement application = header.AddNewElementL(_L8("Application"),KMessagingNameSpace,_L8("mws"));

    TXmlEngElement child = application.AddNewElementSameNsL(_L8("Name"));
    child.AddTextL(_L8("NO_CLIENT"));

    TXmlEngElement child1 = application.AddNewElementSameNsL(_L8("Vendor"));
    child1.AddTextL(_L8("NO_VENDOR"));

    TXmlEngElement child2 = application.AddNewElementSameNsL(_L8("Version"));
    child2.AddTextL(_L8("NO_VERSION"));
    }
    
void CsenConnectionTester::StartActiveScheduler(TInt aNumberOfPendingOps)
    {
    iNumberOfPendingOps = aNumberOfPendingOps;
    CActiveScheduler::Start();
    }
       
void CsenConnectionTester::StopActiveScheduler()
    {
    iNumberOfPendingOps--;
    if ( iNumberOfPendingOps == 0 )
        {
        CActiveScheduler::Stop();
        }
    }

const CSenIdentityProvider* CsenConnectionTester::IdentityProviderL() const
    {
    //((UT_CSenServiceConnection*)this)->iAuthInfoAskCounter++;
    ((CsenConnectionTester*)this)->iAuthInfoAskCounter++;
    
    return ipProviderForAuthProvider;
    }
TAny* CsenConnectionTester::GetInterfaceByUid( TUid aUID )
	{
	return;
	}

// -----------------------------------------------------------------------------
// CsenConnectionTester::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CsenConnectionTester::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
