/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:          SenUtils test module.
*
*/









#ifndef SENUTILS_H
#define SENUTILS_H

// INCLUDES
#include <StifTestModule.h>
#include <StifLogger.h>
#include <SenParser.h>
#include <SenBaseAttribute.h>
#include <SenBaseElement.h>
#include <SenBaseFragment.h>
#include <SenDomFragment.h>
#include <SenIdentityProvider.h>
#include <SenNameSpace.h>
#include <SenSoapEnvelope.h>
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
//#include <S32FILE.H>
//#include <e32std.h>

//  FORWARD DECLARATIONS
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

// Logging path
_LIT( KSenUtilsLogPath, "\\logs\\testframework\\SenUtilsBCTest\\" ); 
// Log file
_LIT( KSenUtilsLogFile, "SenUtilsBCTest.txt" ); 

// Function pointer related internal definitions
// Visual studio 6.0 (__VC32__) needs different type of handling 
#ifdef __VC32__
#define GETPTR
#else
#define GETPTR &
#endif  
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSenUtilsBCTest;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CSenUtilsBCTest::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CSenUtilsBCTest::* TestFunction)(TTestResult&);

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

// CLASS DECLARATION

/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfo
    {
    public:
        TPtrC iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {
        };

    };


// CLASS DECLARATION

/**
*  This a SenUtils class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSenUtilsBCTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CSenUtilsBCTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CSenUtilsBCTest();

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
        * From CTestModuleBase InitL is used to initialize the 
        *       SenUtils. It is called once for every instance of 
        *       TestModuleSenUtils after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of SenUtils.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from SenUtils. 
        * @since ?Series60_version
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @since ?Series60_version
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        * test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& aFailureType, 
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        *
        * User may add implementation for OOM test warning handling. Usually no
        * implementation is required.           
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */); 

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        *
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );

    private:

        /**
        * C++ default constructor.
        */
        CSenUtilsBCTest();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Function returning test case name and pointer to test case function.
        * @since ?Series60_version
        * @param aCaseNumber test case number
        * @return TCaseInfo 
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;
        
        void SetupL();
        void Teardown();

        TPtr16 ConvertToPtr16LC(CSenBaseFragment &fragment);
        TPtr16 ConvertToPtr16LC(MSenElement &element);

        /**
        * Actual Hardcoded test case functions are listed below.
        */
/*
 		TInt CSenSoapEnvelope_NewLL( TTestResult& aResult );
 		TInt CSenSoapEnvelope_AddHeaderLL( TTestResult& aResult );
 		TInt CSenSoapEnvelope_BodyLL( TTestResult& aResult );
 		TInt CSenSoapEnvelope_BodyAsStringLL( TTestResult& aResult );
 		TInt CSenSoapEnvelope_DetachFaultLL( TTestResult& aResult );
 		TInt CSenSoapEnvelope_HeaderLL( TTestResult& aResult );
		TInt CSenSoapEnvelope_SetBodyLL( TTestResult& aResult );
		TInt CSenSoapEnvelope_SoapActionL( TTestResult& aResult );
		TInt CSenSoapEnvelope_FaultLL( TTestResult& aResult );
		TInt CSenSoapFault_NewLL( TTestResult& aResult );
		TInt CSenSoapFault_NewL_1L( TTestResult& aResult );
		TInt CSenSoapFault_FaultCodeL( TTestResult& aResult );
		TInt CSenSoapFault_FaultStringL( TTestResult& aResult );
		TInt CSenSoapFault_FaultActorL( TTestResult& aResult );
		TInt CSenSoapFault_DetailL( TTestResult& aResult );
		TInt CSenSoapMessage_NewLL( TTestResult& aResult );
		TInt CSenWsSecurityHeader_NewLL( TTestResult& aResult );
		TInt CSenWsSecurityHeader_NewL_1L( TTestResult& aResult );
		TInt CSenWsSecurityHeader_UsernameTokenLL( TTestResult& aResult );
		TInt CSenWsSecurityHeader_UsernameTokenL_1L( TTestResult& aResult );
		TInt CSenWsSecurityHeader_XmlNsL( TTestResult& aResult );
		TInt CSenWsSecurityHeader_XmlNsPrefixL( TTestResult& aResult );
		TInt SenDateUtils_FromXmlDateTimeLL( TTestResult& aResult );
		TInt SenDateUtils_ToXmlDateTimeUtf8LL( TTestResult& aResult );
		TInt CSenSoapMessage_SetSecurityHeaderLL( TTestResult& aResult );
		TInt CSenSoapMessage_AddSecurityTokenLL( TTestResult& aResult );
*/

         TInt CSenSoapEnvelope_NewLL(TTestResult& aResult);
         TInt CSenSoapEnvelope_SetBodyLL(TTestResult& aResult);
         TInt CSenSoapEnvelope_BodyLL(TTestResult& aResult);
         TInt CSenSoapEnvelope_HeaderLL(TTestResult& aResult);
         TInt CSenSoapEnvelope_AddHeaderLL(TTestResult& aResult);
         TInt CSenSoapEnvelope_BodyAsStringLL(TTestResult& aResult);
         
         TInt CSenSoapEnvelope_DetachFaultLL(TTestResult& aResult);
         TInt CSenSoapEnvelope_FaultLL(TTestResult& aResult);
         TInt CSenSoapEnvelope_SetSoapActionLL(TTestResult& aResult);
         TInt CSenSoapEnvelope_SoapActionL(TTestResult& aResult);
         TInt CSenSoapEnvelope_SoapAction2L(TTestResult& aResult);
         TInt CSenSoapEnvelope_HasHeaderL(TTestResult& aResult);
         TInt CSenSoapEnvelope_SoapVersionL(TTestResult& aResult);
         TInt CSenSoapEnvelope_HasBodyL(TTestResult& aResult);
		 TInt CSenSoapEnvelope_IsFaultL(TTestResult& aResult);
		 TInt CSenSoapEnvelope_ParseLL(TTestResult& aResult);
        
         TInt CSenSoapFault_NewLL(TTestResult& aResult);
         TInt CSenSoapFault_NewL_1L(TTestResult& aResult);
         TInt CSenSoapFault_FaultCodeL(TTestResult& aResult);
         TInt CSenSoapFault_FaultSubcodeL(TTestResult& aResult);
         TInt CSenSoapFault_FaultStringL(TTestResult& aResult);
         TInt CSenSoapFault_FaultActorL(TTestResult& aResult);
         TInt CSenSoapFault_DetailL(TTestResult& aResult);

         TInt CSenSoapMessage_NewLL(TTestResult& aResult);
         TInt CSenSoapMessage_NewL_1L(TTestResult& aResult);
         TInt CSenSoapMessage_NewL_2L(TTestResult& aResult);
         TInt CSenSoapMessage_SetSecurityHeaderLL(TTestResult& aResult);
         TInt CSenSoapMessage_AddSecurityTokenLL(TTestResult& aResult);
		 TInt CSenSoapMessage_ParseLL(TTestResult& aResult);
         

         TInt CSenWsSecurityHeader_NewLL(TTestResult& aResult);
         TInt CSenWsSecurityHeader_NewLCL(TTestResult& aResult);
         TInt CSenWsSecurityHeader_NewL_1L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_NewLC_1L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_NewL_2L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_NewLC_2L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_BinarySecurityTokenL_L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_BinarySecurityTokenL_1L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_TimestampL_L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_TimestampL_1L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_UsernameTokenLL(TTestResult& aResult);
         TInt CSenWsSecurityHeader_UsernameTokenL_1L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_UsernameTokenL_2L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_UsernameTokenL_3L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_UsernameTokenL_4L(TTestResult& aResult);
         TInt CSenWsSecurityHeader_XmlNsL(TTestResult& aResult);
         TInt CSenWsSecurityHeader_XmlNsPrefixL(TTestResult& aResult);
        
         TInt SenDateUtils_FromXmlDateTimeLL(TTestResult& aResult);
         TInt SenDateUtils_ToXmlDateTimeUtf8LL(TTestResult& aResult);
         TInt SenDateUtils_ToXmlDateTimeUtf82LL(TTestResult& aResult);
        
         TInt SenTransportProperties_FileAttachmentLL(TTestResult& aResult);
         //TInt SenTransportProperties_SetFileAttachmentsLL(TTestResult& aResult);
         
         TInt SenXmlProperties_NewLL(TTestResult& aResult);
         TInt SenXmlProperties_NewLCL(TTestResult& aResult);
		 TInt SenXmlProperties_NewL_1L(TTestResult& aResult);
		 TInt SenXmlProperties_NewLC_1L(TTestResult& aResult);
         TInt SenXmlProperties_NewL_2L(TTestResult& aResult);
         TInt SenXmlProperties_NewLC_2L(TTestResult& aResult);
         
        TInt SenTransportProperties_NewLL(TTestResult& aResult);
        TInt SenTransportProperties_NewLCL(TTestResult& aResult);
        TInt SenTransportProperties_NewL_1L(TTestResult& aResult);
        TInt SenTransportProperties_NewLC_1L(TTestResult& aResult);
        TInt SenTransportProperties_NewL_2L(TTestResult& aResult);
        TInt SenTransportProperties_NewLC_2L(TTestResult& aResult);
		TInt SenTransportProperties_SetReaderL(TTestResult& aResult);
        TInt SenTransportProperties_PropertiesClassTypeL(TTestResult& aResult);
        TInt SenTransportProperties_WriteToLL(TTestResult& aResult);
        TInt SenTransportProperties_ReadFromLL(TTestResult& aResult);
        TInt SenTransportProperties_AsUtf8LL(TTestResult& aResult);
        TInt SenTransportProperties_AsUtf8LCL(TTestResult& aResult);
        TInt SenTransportProperties_SetPropertyLL(TTestResult& aResult);
        TInt SenTransportProperties_PropertyLL(TTestResult& aResult);
        TInt SenTransportProperties_SetIntPropertyLL(TTestResult& aResult);
        TInt SenTransportProperties_IntPropertyLL(TTestResult& aResult);
        TInt SenTransportProperties_SetBoolPropertyLL(TTestResult& aResult);        
        TInt SenTransportProperties_BoolPropertyLL(TTestResult& aResult);
        TInt SenTransportProperties_SetOmittedLL(TTestResult& aResult);
        TInt SenTransportProperties_RemovePropertyLL(TTestResult& aResult);
        TInt SenTransportProperties_IsSafeToCastL(TTestResult& aResult);
		TInt SenTransportProperties_CloneLL(TTestResult& aResult);
        TInt SenTransportProperties_CloneL(TTestResult& aResult);
        TInt SenTransportProperties_ApplyBindingLL(TTestResult& aResult);
        
        TInt SenTransportProperties_HeartbeatLL(TTestResult& aResult);

        TInt SenTransportProperties_SetHeartbeatLL(TTestResult& aResult);
        
        TInt SenTransportProperties_IapIdLL(TTestResult& aResult);

        TInt SenTransportProperties_SetIapIdLL(TTestResult& aResult);

        TInt SenTransportProperties_ProxyPortLL(TTestResult& aResult);

        TInt SenTransportProperties_SetProxyPortLL(TTestResult& aResult);

        TInt SenTransportProperties_ProxyHostLL(TTestResult& aResult);

        TInt SenTransportProperties_SetProxyHostLL(TTestResult& aResult);
        
        TInt SenTransportProperties_ProxyUsageLL(TTestResult& aResult);

        TInt SenTransportProperties_SetProxyUsageLL(TTestResult& aResult);

				TInt SenTransportProperties_SecureDialogLL(TTestResult& aResult);

        TInt SenTransportProperties_SetSecureDialogLL(TTestResult& aResult);
        
        TInt SenTransportProperties_UserAgentLL(TTestResult& aResult);
        
        TInt SenTransportProperties_SetUserAgentLL(TTestResult& aResult);
        
        TInt SenTransportProperties_DeviceIDLL(TTestResult& aResult);

        TInt SenTransportProperties_SetDeviceIDLL(TTestResult& aResult);
        
        TInt SenTransportProperties_SoapActionLL(TTestResult& aResult);
        
        TInt SenTransportProperties_SetSoapActionLL(TTestResult& aResult);
        
        TInt SenTransportProperties_DownloadFolderLL(TTestResult& aResult);
        
        TInt SenTransportProperties_SetDownloadFolderLL(TTestResult& aResult );
               
        TInt SenTransportProperties_SetFileAttachmentLL( TTestResult& aResult);

        TInt SenTransportProperties_MwsNamespaceLL(TTestResult& aResult );
        
        TInt SenTransportProperties_SetMwsNamespaceLL(TTestResult& aResult );

        TInt SenTransportProperties_MessageIdLL(TTestResult& aResult );

        TInt SenTransportProperties_SetMessageIdLL( TTestResult& aResult);

        TInt SenTransportProperties_OnewayMessageOnOffLL( TTestResult& aResult);

        TInt SenTransportProperties_SetOnewayMessageOnOffLL(TTestResult& aResult );
        
        TInt SenTransportProperties_SetMaxTimeToLiveLL(TTestResult& aResult);
        
		TInt SenTransportProperties_MaxTimeToLiveLL(TTestResult& aResult);
		
		TInt SenTransportProperties_SetMinTimeToLiveLL(TTestResult& aResult);
		
		TInt SenTransportProperties_MinTimeToLiveLL(TTestResult& aResult);

		TInt SenHttpTransportProperties_NewLL(TTestResult& aResult);
		
		TInt SenHttpTransportProperties_NewLCL(TTestResult& aResult);

		TInt SenHttpTransportProperties_NewL_1L(TTestResult& aResult);
		
		TInt SenHttpTransportProperties_NewLC_1L(TTestResult& aResult);

		TInt SenHttpTransportProperties_NewL_2L(TTestResult& aResult);
		
		TInt SenHttpTransportProperties_NewLC_2L(TTestResult& aResult);
		
		TInt SenVtcpTransportProperties_NewLL(TTestResult& aResult);
		
		TInt SenVtcpTransportProperties_NewLCL(TTestResult& aResult);

		TInt SenVtcpTransportProperties_NewL_1L(TTestResult& aResult);
		
		TInt SenVtcpTransportProperties_NewLC_1L(TTestResult& aResult);

		TInt SenVtcpTransportProperties_NewL_2L(TTestResult& aResult);
		
		TInt SenVtcpTransportProperties_NewLC_2L(TTestResult& aResult);

        TInt SenVtcpTransportProperties_OnewayMessageOnOffLL( TTestResult& aResult);

        TInt SenVtcpTransportProperties_SetOnewayMessageOnOffLL(TTestResult& aResult );
        
        TInt SenVtcpTransportProperties_SetMaxTimeToLiveLL(TTestResult& aResult);
        
		TInt SenVtcpTransportProperties_MaxTimeToLiveLL(TTestResult& aResult);
		
		TInt SenVtcpTransportProperties_SetMinTimeToLiveLL(TTestResult& aResult);
		
		TInt SenVtcpTransportProperties_MinTimeToLiveLL(TTestResult& aResult);

     public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        // Pointer to logger
        CStifLogger * iLog; 

		CSenXmlReader* iXmlReader;
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // SENUTILS_H

// End of File