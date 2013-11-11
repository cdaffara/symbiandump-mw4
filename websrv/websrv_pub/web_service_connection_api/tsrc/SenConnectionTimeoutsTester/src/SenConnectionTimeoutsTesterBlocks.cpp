/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        ?Description
*
*/


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "SenConnectionTimeoutsTester.h"

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
    _LIT8(KWSPContract,         "urn:nokia:test:addrbook:2004-09"); aa
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
    
#ifdef SYSOPENDIGIA_ADDRESSBOOK_SERVICE
    _LIT(KIAPName,                  "Ethernet No Daemon Static IP");
#else
    //_LIT(KIAPName,                  "Ethernet with Daemon Dynamic IP");
    //_LIT(KIAPName,                  "Ethernet No Daemon Static IP");
    _LIT(KIAPName,                  "Ethernet with Daemon Dynamic IP");
#endif
    _LIT8(KLoggerFileNameProperty,  "HttpLoggerFileName");

    _LIT(KFileToRead,               "c:\\Logs\\LoggingHttpFilter\\httptrace.xml");
    _LIT8(KFileName,                "httptrace.xml");

    _LIT8(KHTTPMethodGet,           "GET");
    _LIT8(KHTTPMethodPost,          "POST");
    
//    const TBool MultipleSimultanousRHttpSessionsSupported = EFalse;
    // If IAP changes all previously sent messages will be canceled.
    
    
    _LIT8(KProxyHost,               "172.19.160.50"); // IP address to: bsprx01.americas.nokia.com
    const TInt KProxyPort =         8080;
    
    }


// MACROS
//#define ?macro ?macro_def
//Macro for Porting to STIF
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
// CSenConnectionTimeoutsTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CSenConnectionTimeoutsTester::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CSenConnectionTimeoutsTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSenConnectionTimeoutsTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		ENTRY("SendL_TPAsync1Test_CSenServiceConnection_SendLconnectionisnotinitialized", CSenConnectionTimeoutsTester_TP_Async1),
		ENTRY("SendL_TPAsync2Test_CSenServiceConnection_SendLconnectionisnotinitialized", CSenConnectionTimeoutsTester_TP_Async2),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// CONSTRUCTION
//CSenConnectionTimeoutsTester* CSenConnectionTimeoutsTester::NewL()
//    {
//    CSenConnectionTimeoutsTester* self = CSenConnectionTimeoutsTester::NewLC();
//    CleanupStack::Pop();

//    return self;
//    }

//CSenConnectionTimeoutsTester* CSenConnectionTimeoutsTester::NewLC()
//    {
//    CSenConnectionTimeoutsTester* self = new( ELeave ) CSenConnectionTimeoutsTester();
//    CleanupStack::PushL( self );

//    self->ConstructL();

//    return self;
//    }


// Default constructor
//CSenConnectionTimeoutsTester::CSenConnectionTimeoutsTester():iInitialized(FALSE)
//    {
//    }
      
// Second phase construct
//void CSenConnectionTimeoutsTester::ConstructL()
//    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
//    CEUnitTestSuiteClass::ConstructL();
//    }

//  METHODS

void CSenConnectionTimeoutsTester::HandleMessageL(const TDesC8& aMessage)
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

void CSenConnectionTimeoutsTester::HandleErrorL(const int aErrorCode, const TDesC8& aError)
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

void CSenConnectionTimeoutsTester::ErrorL(TInt aCode)
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

void CSenConnectionTimeoutsTester::SetStatus(const TInt aStatus)
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
        if(!iInitialized)
            {
            TInt leaveCode(KErrNone);
            TRAP(leaveCode, ErrorL(EConnectionNotReady));
            leaveCode = 0; // not used
            }
        iConnectionState = aStatus;
        StopActiveScheduler();
        }
    }


void CSenConnectionTimeoutsTester::SetupL(  )
    {
    __UHEAP_MARK;

    RFile file;
    RFs iFsSession;
    _LIT(KFileName,"c:\\testing\\data\\rest.on");
    
   
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

void CSenConnectionTimeoutsTester::Teardown(  )
    {
    delete ipFrameworkID;
    ipFrameworkID = NULL;
    delete ipMessage;
    ipMessage = NULL;
    __UHEAP_MARKEND;
    }



    

//Befor executing this TC remember to set big enought TimeOut value (10 min.)

TInt CSenConnectionTimeoutsTester::CSenConnectionTimeoutsTester_TP_Async1(CStifItemParser& aItem)
{

    
    
    _LIT8(KAsyncEndPoint,           "http://10.21.32.110/sereneHardCoded/WS_Async_002");
    _LIT8(KCustomHttpHeader,        "CustomHeader");
    
    _LIT8(KLogFile,                "httptrace");
    _LIT(KFileExtension,          ".xml");
    _LIT8(KCustomHttpHeaderValue,  "AsyncTest_");
    _LIT(KLogFileToRead,           "c:\\Logs\\LoggingHttpFilter\\httptrace");
    
    SetupL();
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KAsyncEndPoint, KNullDesC8());
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
     
     
    TBuf8<255> tempLogFile;     
    TBuf8<255> tempCustomHttpHeaderValue;
    TBuf<255> tempLogFileToRead;
    
    for (TInt i = 1; i <= 8; i++)
    {
        tempLogFile.Copy(KLogFile);
        tempLogFile.AppendNum(i);
        tempLogFile.Append(KFileExtension);
    
        tempCustomHttpHeaderValue.Copy(KCustomHttpHeaderValue);
        tempCustomHttpHeaderValue.AppendNum(i);
        
        tempLogFileToRead.Copy(KLogFileToRead);
        tempLogFileToRead.AppendNum(i);
        tempLogFileToRead.Append(KFileExtension);
        
        TL(CSenConnectionTimeoutsTester_TP_Send(tempLogFile,KCustomHttpHeader,tempCustomHttpHeaderValue,iServiceConnection) == KErrNone) ;    
        TL(CSenConnectionTimeoutsTester_TP_CheckLogs( tempLogFileToRead,  KCustomHttpHeader, tempCustomHttpHeaderValue) == KErrNone);     
        
    }//for i
                                                  
    
        delete iServiceConnection;
        iServiceConnection = NULL;
        Teardown();
		return KErrNone;    
    }    



//Before executing this TC remember to set big enought TimeOut value (10 min.)
TInt CSenConnectionTimeoutsTester::CSenConnectionTimeoutsTester_TP_Async2(CStifItemParser& aItem)
    {
    _LIT8(KAsyncEndPoint,           "http://10.21.32.110/sereneHardCoded/WS_Async_002");
    _LIT8(KCustomHttpHeader,        "CustomHeader");
    
    _LIT8(KLogFile,                "httptrace");
    _LIT(KFileExtension,          ".xml");
    _LIT8(KCustomHttpHeaderValue,  "AsyncTest_");
    _LIT(KLogFileToRead,           "c:\\Logs\\LoggingHttpFilter\\httptrace");
    
	SetupL();
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KAsyncEndPoint, KNullDesC8());
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

    TBuf8<255> tempLogFile;     
    TBuf8<255> tempCustomHttpHeader;
    TBuf8<255> tempCustomHttpHeaderValue;
    TBuf<255> tempLogFileToRead;
    
    for (TInt i = 1; i <= 8; i++)
    {
        tempLogFile.Copy(KLogFile);
        tempLogFile.AppendNum(i);
        tempLogFile.Append(KFileExtension);
        
        tempCustomHttpHeader.Copy(KCustomHttpHeader);
        tempCustomHttpHeader.AppendNum(i);
    
        tempCustomHttpHeaderValue.Copy(KCustomHttpHeaderValue);
        tempCustomHttpHeaderValue.AppendNum(i);
        
        tempLogFileToRead.Copy(KLogFileToRead);
        tempLogFileToRead.AppendNum(i);
        tempLogFileToRead.Append(KFileExtension);
        
        TL(CSenConnectionTimeoutsTester_TP_Send(tempLogFile,tempCustomHttpHeader,tempCustomHttpHeaderValue,iServiceConnection) == KErrNone) ; 
        TL(CSenConnectionTimeoutsTester_TP_CheckLogs( tempLogFileToRead,  tempCustomHttpHeader, tempCustomHttpHeaderValue) == KErrNone);      
        
    }//for i
                                                  
    
    delete iServiceConnection;
    iServiceConnection = NULL;
    Teardown();
		return KErrNone;     
     
    }    


TInt CSenConnectionTimeoutsTester::CSenConnectionTimeoutsTester_TP_Send(const TDesC8& aLogFileName, const TDesC8& aCustomHttpHeader,
                                                                const TDesC8& aCustomHttpHeaderValue, CSenServiceConnection* iServiceConnection)
    {
    TInt result = KErrNone;
    CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();

    // SendL(MSG) // with custom header
    CSenHttpTransportProperties* pHttpProperties = CSenHttpTransportProperties::NewLC();    
    pHttpProperties->SetHttpHeaderL(KLoggerFileNameProperty, aLogFileName);
    pHttpProperties->SetHttpHeaderL(aCustomHttpHeader, aCustomHttpHeaderValue);
    TUint32 iapId = SearchIAPIdByName(KIAPName);
    pHttpProperties->SetIapIdL(iapId);
    HBufC8* pSerializedProperties = pHttpProperties->AsUtf8L();
    CleanupStack::PushL(pSerializedProperties);
    iServiceConnection->SendL(*pSOAPMessage, *pSerializedProperties);
    StartActiveScheduler(1);
    //RDebug::Print(_L(iCallback == ECallbackHandleMessage);
    if(iCallback != ECallbackHandleMessage) return KErrGeneral;
   
    CleanupStack::PopAndDestroy(pSerializedProperties);
    CleanupStack::PopAndDestroy(pHttpProperties);
    CleanupStack::PopAndDestroy(pSOAPMessage);
    return result;
    }  
    
    
TInt CSenConnectionTimeoutsTester::CSenConnectionTimeoutsTester_TP_CheckLogs(const TDesC& aLogFilePath,  const TDesC8& aCustomHttpHeader, const TDesC8& aCustomHttpHeaderValue)
    {
    TInt result = KErrNone;
    HBufC8* pFileContent = ReadFileL(aLogFilePath);
    //RDebug::Print(_L(pFileContent != NULL);
    if (pFileContent == NULL) return KErrNotFound;
    
    CleanupStack::PushL(pFileContent);
    HBufC8* pValue = HttpPropertyValueFromXmlLC(*pFileContent, aCustomHttpHeader);
    //RDebug::Print(_L(*pValue == aCustomHttpHeaderValue);
    if (*pValue != aCustomHttpHeaderValue) return KErrNotFound;
    
    CleanupStack::PopAndDestroy(pValue);
    CleanupStack::PopAndDestroy(pFileContent);    

    return result;
    }


HBufC8* CSenConnectionTimeoutsTester::HttpMethodValueFromXmlLC(const TDesC8& aXml)
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

HBufC8* CSenConnectionTimeoutsTester::HttpPropertyValueFromXmlLC(const TDesC8& aXml,
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
                }
            }
        }

    CleanupStack::PopAndDestroy(pDom);
    CleanupStack::PopAndDestroy(pReader);
    CleanupStack::PushL(pRetBuf);
    return pRetBuf;
    }
    
HBufC8* CSenConnectionTimeoutsTester::ReadFileL(const TDesC& aFilename)
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
    
TUint32 CSenConnectionTimeoutsTester::SearchIAPIdByName(const TDesC& aIAPName)
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
    
CSenSoapMessage* CSenConnectionTimeoutsTester::CreateAuthRequestLC()
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
     
CSenSoapMessage2* CSenConnectionTimeoutsTester::CreateNewAuthRequestLC()
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
    
void CSenConnectionTimeoutsTester::StartActiveScheduler(TInt aNumberOfPendingOps)
    {
    iNumberOfPendingOps = aNumberOfPendingOps;
    CActiveScheduler::Start();
    }
       
void CSenConnectionTimeoutsTester::StopActiveScheduler()
    {
    iNumberOfPendingOps--;
    if ( iNumberOfPendingOps == 0 )
        {
        CActiveScheduler::Stop();
        }
    }

// -----------------------------------------------------------------------------
// CSenConnectionTimeoutsTester::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CSenConnectionTimeoutsTester::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
