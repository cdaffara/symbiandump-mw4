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
* Description: Header declaration
*
*/









#ifndef SENUTILSTESTCASES_H
#define SENUTILSTESTCASES_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <SenParser.h>
#include <SenBaseAttribute.h>
#include <SenBaseElement.h>
#include <SenBaseFragment.h>
#include <SenDomFragment.h>
#include <SenIdentityProvider.h>
#include <SenNameSpace.h>
//#include <SenSoapEnvelope.h>
#include <SenSoapFault.h>
#include <SenSoapMessage.h>
#include <SenWsSecurityHeader.h>
#include <SenXmlReader.h>
#include <SenXmlServiceDescription.h>
#include <SenDateUtils.h>
#include <SenXmlUtils.h>
#include <SenXmlConstants.h>
#include <SenTransportProperties.h>
#include <SenHttpTransportProperties.h>
//#include <SenVtcpTransportProperties.h>
#include <f32file.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KsenutilsTestCasesLogPath, "\\logs\\testframework\\senutilsTestCases\\" ); 
// Log file
_LIT( KsenutilsTestCasesLogFile, "senutilsTestCases.txt" ); 
_LIT( KsenutilsTestCasesLogFileWithTitle, "senutilsTestCases_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CsenutilsTestCases;
class CSenBaseAttribute;
class CSenBaseFragment;
class CSenSoapMessage;
class CSenIdentityProvider;
class CSenGuidGen;
class CSenDomFragment;
class CSenSoapFault;
class CSenXmlServiceDescription;
class CSenSoapEnvelope;
class CSenWsSecurityHeader;
class SenXmlUtils;
class CSenBaseElement;
class MSenElement;
class CSenXmlReader;
class SenDateUtils;
class CSenNamespace;

namespace{
    _LIT8(KText,"text");
    _LIT8(KText2,"text2");
    _LIT8(KXmlSpecific, "\"&<>");
    _LIT8(KFaultMessage, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Body><S:Fault><faultcode>VersionMismatch</faultcode><faultstring>some string</faultstring><faultactor>some actor</faultactor><detail>bla bla</detail></S:Fault></S:Body></S:Envelope>");
    _LIT8(KSOAPMessageNS, "<tns:Envelope xmlns:tns=\"http://schemas.xmlsoap.org/soap/envelope/\"><tns:Header/><tns:Body><ab:QueryResponse xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:Status code=\"OK\"/><ab:Data>Hard Coded response, alway the same</ab:Data></ab:QueryResponse></tns:Body></tns:Envelope>");
    _LIT8(KSOAPMessageNSInside, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header/><S:Body><ab:QueryResponse xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:Status code=\"OK\"/><ab:Data>Hard Coded response, alway the same</ab:Data></ab:QueryResponse></S:Body></S:Envelope>");
    _LIT8(KSOAPMessage,  "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header/><S:Body><QueryResponse><Data>Hard Coded response, alway the same</Data></QueryResponse></S:Body></S:Envelope>");
}
typedef TInt (CsenutilsTestCases::* TestFunction)( CStifItemParser&);
// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CsenutilsTestCases test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CsenutilsTestCases) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CsenutilsTestCases* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CsenutilsTestCases();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CsenutilsTestCases( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        
        void SetupL();
        void Teardown();

        TPtr16 ConvertToPtr16LC(CSenBaseFragment &fragment);
        TPtr16 ConvertToPtr16LC(MSenElement &element);
         TInt CSenSoapEnvelope_NewLL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_SetBodyLL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_BodyLL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_HeaderLL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_AddHeaderLL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_BodyAsStringLL(CStifItemParser& aItem);
         
         TInt CSenSoapEnvelope_DetachFaultLL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_FaultLL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_SetSoapActionLL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_SoapActionL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_SoapAction2L(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_HasHeaderL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_SoapVersionL(CStifItemParser& aItem);
         TInt CSenSoapEnvelope_HasBodyL(CStifItemParser& aItem);
		 TInt CSenSoapEnvelope_IsFaultL(CStifItemParser& aItem);
		 TInt CSenSoapEnvelope_ParseLL(CStifItemParser& aItem);
        
         TInt CSenSoapFault_NewLL(CStifItemParser& aItem);
         TInt CSenSoapFault_NewL_1L(CStifItemParser& aItem);
         TInt CSenSoapFault_FaultCodeL(CStifItemParser& aItem);
         TInt CSenSoapFault_FaultSubcodeL(CStifItemParser& aItem);
         TInt CSenSoapFault_FaultStringL(CStifItemParser& aItem);
         TInt CSenSoapFault_FaultActorL(CStifItemParser& aItem);
         TInt CSenSoapFault_DetailL(CStifItemParser& aItem);

         TInt CSenSoapMessage_NewLL(CStifItemParser& aItem);
         TInt CSenSoapMessage_NewL_1L(CStifItemParser& aItem);
         TInt CSenSoapMessage_NewL_2L(CStifItemParser& aItem);
         TInt CSenSoapMessage_SetSecurityHeaderLL(CStifItemParser& aItem);
         TInt CSenSoapMessage_AddSecurityTokenLL(CStifItemParser& aItem);
		 TInt CSenSoapMessage_ParseLL(CStifItemParser& aItem);
         

         TInt CSenWsSecurityHeader_NewLL(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_NewLCL(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_NewL_1L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_NewLC_1L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_NewL_2L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_NewLC_2L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_BinarySecurityTokenL_L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_BinarySecurityTokenL_1L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_TimestampL_L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_TimestampL_1L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_UsernameTokenLL(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_UsernameTokenL_1L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_UsernameTokenL_2L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_UsernameTokenL_3L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_UsernameTokenL_4L(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_XmlNsL(CStifItemParser& aItem);
         TInt CSenWsSecurityHeader_XmlNsPrefixL(CStifItemParser& aItem);
        
         TInt SenDateUtils_FromXmlDateTimeLL(CStifItemParser& aItem);
         TInt SenDateUtils_ToXmlDateTimeUtf8LL(CStifItemParser& aItem);
         TInt SenDateUtils_ToXmlDateTimeUtf82LL(CStifItemParser& aItem);
        
         TInt SenTransportProperties_FileAttachmentLL(CStifItemParser& aItem);
         //TInt SenTransportProperties_SetFileAttachmentsLL(CStifItemParser& aItem);
         
         TInt SenXmlProperties_NewLL(CStifItemParser& aItem);
         TInt SenXmlProperties_NewLCL(CStifItemParser& aItem);
		 TInt SenXmlProperties_NewL_1L(CStifItemParser& aItem);
		 TInt SenXmlProperties_NewLC_1L(CStifItemParser& aItem);
         TInt SenXmlProperties_NewL_2L(CStifItemParser& aItem);
         TInt SenXmlProperties_NewLC_2L(CStifItemParser& aItem);
         
        TInt SenTransportProperties_NewLL(CStifItemParser& aItem);
        TInt SenTransportProperties_NewLCL(CStifItemParser& aItem);
        TInt SenTransportProperties_NewL_1L(CStifItemParser& aItem);
        TInt SenTransportProperties_NewLC_1L(CStifItemParser& aItem);
        TInt SenTransportProperties_NewL_2L(CStifItemParser& aItem);
        TInt SenTransportProperties_NewLC_2L(CStifItemParser& aItem);
		TInt SenTransportProperties_SetReaderL(CStifItemParser& aItem);
        TInt SenTransportProperties_PropertiesClassTypeL(CStifItemParser& aItem);
        TInt SenTransportProperties_WriteToLL(CStifItemParser& aItem);
        TInt SenTransportProperties_ReadFromLL(CStifItemParser& aItem);
        TInt SenTransportProperties_AsUtf8LL(CStifItemParser& aItem);
        TInt SenTransportProperties_AsUtf8LCL(CStifItemParser& aItem);
        TInt SenTransportProperties_SetPropertyLL(CStifItemParser& aItem);
        TInt SenTransportProperties_PropertyLL(CStifItemParser& aItem);
        TInt SenTransportProperties_SetIntPropertyLL(CStifItemParser& aItem);
        TInt SenTransportProperties_PropertyL1L(CStifItemParser& aItem);
        TInt SenTransportProperties_IntPropertyLL(CStifItemParser& aItem);
        TInt SenTransportProperties_SetBoolPropertyLL(CStifItemParser& aItem);        
        TInt SenTransportProperties_BoolPropertyLL(CStifItemParser& aItem);
        TInt SenTransportProperties_SetOmittedLL(CStifItemParser& aItem);
        TInt SenTransportProperties_RemovePropertyLL(CStifItemParser& aItem);
        TInt SenTransportProperties_IsSafeToCastL(CStifItemParser& aItem);
		TInt SenTransportProperties_CloneLL(CStifItemParser& aItem);
        TInt SenTransportProperties_CloneL(CStifItemParser& aItem);
        TInt SenTransportProperties_ApplyBindingLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_HeartbeatLL(CStifItemParser& aItem);

        TInt SenTransportProperties_SetHeartbeatLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_IapIdLL(CStifItemParser& aItem);

        TInt SenTransportProperties_SetIapIdLL(CStifItemParser& aItem);

        TInt SenTransportProperties_ProxyPortLL(CStifItemParser& aItem);

        TInt SenTransportProperties_SetProxyPortLL(CStifItemParser& aItem);

        TInt SenTransportProperties_ProxyHostLL(CStifItemParser& aItem);

        TInt SenTransportProperties_SetProxyHostLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_ProxyUsageLL(CStifItemParser& aItem);

        TInt SenTransportProperties_SetProxyUsageLL(CStifItemParser& aItem);

				TInt SenTransportProperties_SecureDialogLL(CStifItemParser& aItem);

        TInt SenTransportProperties_SetSecureDialogLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_UserAgentLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_SetUserAgentLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_DeviceIDLL(CStifItemParser& aItem);

        TInt SenTransportProperties_SetDeviceIDLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_SoapActionLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_SetSoapActionLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_DownloadFolderLL(CStifItemParser& aItem);
        
        TInt SenTransportProperties_SetDownloadFolderLL(CStifItemParser& aItem );
               
        TInt SenTransportProperties_SetFileAttachmentLL( CStifItemParser& aItem);

        TInt SenTransportProperties_MwsNamespaceLL(CStifItemParser& aItem );
        
        TInt SenTransportProperties_SetMwsNamespaceLL(CStifItemParser& aItem );

        TInt SenTransportProperties_MessageIdLL(CStifItemParser& aItem );

        TInt SenTransportProperties_SetMessageIdLL( CStifItemParser& aItem);

        TInt SenTransportProperties_OnewayMessageOnOffLL( CStifItemParser& aItem);

        TInt SenTransportProperties_SetOnewayMessageOnOffLL(CStifItemParser& aItem );
        
        TInt SenTransportProperties_SetMaxTimeToLiveLL(CStifItemParser& aItem);
        
		TInt SenTransportProperties_MaxTimeToLiveLL(CStifItemParser& aItem);
		
		TInt SenTransportProperties_SetMinTimeToLiveLL(CStifItemParser& aItem);
		
		TInt SenTransportProperties_MinTimeToLiveLL(CStifItemParser& aItem);

		TInt SenHttpTransportProperties_NewLL(CStifItemParser& aItem);
		
		TInt SenHttpTransportProperties_NewLCL(CStifItemParser& aItem);

		TInt SenHttpTransportProperties_NewL_1L(CStifItemParser& aItem);
		
		TInt SenHttpTransportProperties_NewLC_1L(CStifItemParser& aItem);

		TInt SenHttpTransportProperties_NewL_2L(CStifItemParser& aItem);
		
		TInt SenHttpTransportProperties_NewLC_2L(CStifItemParser& aItem);
		
		TInt SenVtcpTransportProperties_NewLL(CStifItemParser& aItem);
		
		TInt SenVtcpTransportProperties_NewLCL(CStifItemParser& aItem);

		TInt SenVtcpTransportProperties_NewL_1L(CStifItemParser& aItem);
		
		TInt SenVtcpTransportProperties_NewLC_1L(CStifItemParser& aItem);

		TInt SenVtcpTransportProperties_NewL_2L(CStifItemParser& aItem);
		
		TInt SenVtcpTransportProperties_NewLC_2L(CStifItemParser& aItem);

        TInt SenVtcpTransportProperties_OnewayMessageOnOffLL( CStifItemParser& aItem);

        TInt SenVtcpTransportProperties_SetOnewayMessageOnOffLL(CStifItemParser& aItem );
        
        TInt SenVtcpTransportProperties_SetMaxTimeToLiveLL(CStifItemParser& aItem);
        
		TInt SenVtcpTransportProperties_MaxTimeToLiveLL(CStifItemParser& aItem);
		
		TInt SenVtcpTransportProperties_SetMinTimeToLiveLL(CStifItemParser& aItem);
		
		TInt SenVtcpTransportProperties_MinTimeToLiveLL(CStifItemParser& aItem);
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
    TestFunction iMethod;

        // Pointer to logger
        CStifLogger * iLog; 

		CSenXmlReader* iXmlReader;


    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // SENUTILSTESTCASES_H

// End of File
