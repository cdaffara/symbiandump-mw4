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
* Description:  ?Description
*
*/



#ifndef WSTESTER_H
#define WSTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <s32file.H>
#include <tconvbase64.h>
#include <s32strm.h>

//Sen
#include <SenXmlServiceDescription.h>
#include <SenServiceConnection.h>
#include <SenCredential.h>

#include <SenSoapMessage.h>
#include <SenServiceManager.h>
//#include <SenServiceManagerImpl.h>
#include <SenIdentityProvider.h>
#include <SenXmlUtils.h>
#include <SenDateUtils.h>
#include <SenServicePattern.h>
#include "SAXTester.h"
#include <SenBaseElement.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_MODULE_VERSION_MAJOR 30
#define TEST_MODULE_VERSION_MINOR 9
#define TEST_MODULE_VERSION_BUILD 38
// Logging path
_LIT( KWSTesterLogPath, "\\logs\\testframework\\WSTester\\" ); 
// Log file
_LIT( KWSTesterLogFile, "WSTester.txt" ); 
_LIT8(KFixedHTTPPrefix, "http://");
_LIT8(KFixedHTTPSPrefix, "https://");


// Literals for SAX testing purposes
_LIT8(KLName8, "Level1");
_LIT8(KL2Name8, "Level2");
_LIT8(KDefaultNS, "Default:NS:Test");
_LIT8(KQName, "TestPre:Level1");
_LIT8(KNsPrefix, "TestPre");
_LIT8(KParseLocalMsg, "<Level1><Level2>Level2 content</Level2><Level2_b>Level2_b content</Level2_b></Level1>");
_LIT8(KParseDifferentLocalMsg, "<Dif1><Level2>Level2 content</Level2></Dif1>");
_LIT8(KParseLocalNs, "<Level1 xmlns=\"Default:NS:Test\"><Level2>Level2 content</Level2></Level1>");
_LIT8(KParseLocalNsQ, "<TestPre:Level1 xmlns:TestPre=\"Default:NS:Test\"><Level2>Level2 content</Level2></TestPre:Level1>");
_LIT8(KAttribute, "Atripuutti");
_LIT8(KAttributeValue, "JustValue");
_LIT8(KParseElemMsg, "<BaseFrag><Level2>Level2 content</Level2></BaseFrag>");
_LIT8(KElementXml, "<BaseFrag></BaseFrag>");

/**
* Result and reference are indentical Test case should be passed.
*/
const TInt KResultOK = 0;
/**
* There is no result. Test case should be failed.
*/
const TInt KResultFailedNoResult = 1;
/**
* There is no reference. Test case should be failed.
*/
const TInt KResultFailedNoReference = 2;
/**
* Tesult and  reference are not identical. Test case should be failed.
*/
const TInt KResultFailedNotIdentical = 3;

/**
* A string in reference file was not found on the source.  Test case should be failed.
*/
const TInt KResultFailedFound = 4;

//===============================Serene status
const TInt KSubmit			= 1;
const TInt KSend			= 2;
const TInt KNewUriL			= 3;
const TInt KNewUriLC		= 4;
const TInt KNewSDescLC		= 5;
const TInt KNewSDescL		= 6;
const TInt KNewFooASURL		= 7;
const TInt KNewLocalhost	= 8;
const TInt KSendFoobar		= 9;
const TInt KSendFW1			= 10;
const TInt KSendFW2			= 11;
const TInt KSendRegistered	= 12;
const TInt KGetListTrue		= 13;
const TInt KGetListFalse	= 14;
const TInt KGetListTrueUE	= 15;
const TInt KSendSOAP		= 16;
const TInt KSubmitSOAP		= 17;
const TInt KRegisterAndSend    = 18;
const TInt KNewSPatternL	= 19;
const TInt KNewSPatternLC	= 20;
const TInt KBasicNewL		= 21;
const TInt KBasicNewLC		= 22;
const TInt KBasicEndpointURINewL		= 23;
const TInt KBasicEndpointURINewLC		= 24;
const TInt KSendEndPContr				= 25;
const TInt KBasicAuthentication			= 26;
const TInt KRegisterAndSendFalseLogin	= 27;
const TInt KSubmitNonAlloc				= 28;
const TInt KSubmitSOAPRequest			= 29;
const TInt KBasicAuthenticationFalse	= 30;
const TInt KSendSOAPRequest				= 31;
const TInt KImplTesting					= 32;
const TInt KSendLong					= 33;
const TInt KEndpointMovedTest			= 34;
const TInt KGetConnectionFooContaract	= 35;
const TInt KSendASAbort					= 36;
const TInt KInvalidLogin				= 37;
const TInt KInvalidHost					= 38;
const TInt KLowercase					= 40;
const TInt KUppercase 					= 41;
const TInt KAllowed 					= 42;
const TInt KAllowedUppercase			= 43;
const TInt KUppercaseAllowed			= 44;
const TInt KAllowUpperTrun 				= 45;
const TInt KInitialOK					= 46;
const TInt KBasicDisco 					= 47;
const TInt KInvalidCredentials 			= 48;
const TInt KUnexistantDS				= 49;
const TInt KTruncate 					= 50;
const TInt KMultippleROs 				= 51;
const TInt KDSFailed 					= 52;
const TInt KDSFailedResult 				= 53;
const TInt KNoCredentials 				= 54;
const TInt KConnectAS					= 55;
const TInt KSendBUCF	 				= 56;
const TInt KSendBUCFBody 				= 57;
const TInt KSendBUCFEnvelope			= 58;
const TInt KInvalidWSP					= 59;
const TInt KFacets						= 60;
const TInt KDSEcrypted					= 61;
const TInt KNoResourceID				= 62;
const TInt KSendInvalidXML				= 63;
const TInt KSendInvalidSOAP				= 64;
const TInt KSendYomi					= 65;
const TInt KSendBUCFEnvelope2			= 66;
const TInt KDelegateParsing				= 67;	
const TInt KDelegateParsing2			= 68;	
const TInt KDetach						= 69;
const TInt KNonSupportedAM				= 70;
const TInt KCredentialsExpires			= 71;
const TInt KServerError500				= 72;
const TInt KServerError404				= 73;
const TInt KHasFacet					= 74;
const TInt KIsReady						= 75;
const TInt KCompleteServerMessagesOnOff_1 = 76;
const TInt KCompleteServerMessagesOnOff_2 = 77;
const TInt KCompleteServerMessagesOnOff_3 = 78;
const TInt KCompleteServerMessagesOnOff_4 = 79;
const TInt KPerformanceSOAPSubmit		= 80;



//NONSHARABLE_CLASS(CWSTester)
class CWSTester: public CScriptBase, 	public MSenServiceConsumer
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWSTester* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CWSTester();

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
        /**
         * Method used to log version of test module
         */
        void SendTestModuleVersion();
                
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
        CWSTester( CTestModuleIf& aTestModuleIf );

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
        


		TInt CheckNonIncludanceL(TPtrC aSrcFile, TPtrC aReferenceFile);
		TInt CheckIncludanceL(TPtrC aSrcFile, TPtrC aReferenceFile);
		TInt TestFacets();
		HBufC8* DeBase64DescL(TDesC8& aMessage);
        /**
        * Test methods are listed below. 
        */

/*################################################
*
*        
*   SERVICE MANAGER DLL
*
*
*################################################*/

/*-----------------------------------------------
            Service Manager
-----------------------------------------------*/

    virtual TInt _S_NewServiceManager( CStifItemParser& aItem );
    virtual TInt _S_RegisterIdentityProvider( CStifItemParser& aItem );
    virtual TInt _S_UnregisterIdentityProvider( CStifItemParser& aItem );
    virtual TInt _S_AssociateService( CStifItemParser& aItem );
    virtual TInt _S_DissociateService( CStifItemParser& aItem );
    virtual TInt _S_RegisterServiceDescription( CStifItemParser& aItem );
    virtual TInt _S_UnregisterServiceDescription( CStifItemParser& aItem );
    virtual TInt _S_ServiceDescriptions( CStifItemParser& aItem );
   	virtual TInt _S_ServiceDescriptionsByUri( CStifItemParser& aItem );
	virtual TInt _S_InstallFramework( CStifItemParser& aItem );


/*################################################
*
*        
*   XML DLL
*
*
*################################################*/

/*-----------------------------------------------
            SAX based parsing
-----------------------------------------------*/
virtual TInt _S_NewSAXFragment( CStifItemParser& aItem );
virtual TInt _S_SAXTest( CStifItemParser& aItem );

/*-----------------------------------------------
            DOM based parsing
-----------------------------------------------*/
		virtual TInt _S_LocalName( CStifItemParser& aItem );
		virtual TInt _S_NamespaceURI( CStifItemParser& aItem );
		virtual TInt _S_SetNamespace( CStifItemParser& aItem );
		virtual TInt _S_NsPrefix( CStifItemParser& aItem );
		virtual TInt _S_SetPrefix( CStifItemParser& aItem );
		virtual TInt _S_HasContent( CStifItemParser& aItem );
		virtual TInt _S_Content( CStifItemParser& aItem );
		virtual TInt _S_ContentUnicode( CStifItemParser& aItem );
		virtual TInt _S_SetContent( CStifItemParser& aItem );
		virtual TInt _S_AddNamespace( CStifItemParser& aItem );
		virtual TInt _S_AttrValue( CStifItemParser& aItem );
		virtual TInt _S_AddAttr( CStifItemParser& aItem );
		virtual TInt _C_Namespace( CStifItemParser& aItem );
		virtual TInt _C_Elements( CStifItemParser& aItem );
		virtual TInt _C_Attributes( CStifItemParser& aItem );
		virtual TInt _C_Namespaces( CStifItemParser& aItem );
		virtual TInt _C_Parent( CStifItemParser& aItem );
		virtual TInt _C_Root( CStifItemParser& aItem );
		virtual TInt _C_SetParent( CStifItemParser& aItem );
		virtual TInt _C_Element( CStifItemParser& aItem );
		virtual TInt _C_CreateElement( CStifItemParser& aItem );
		virtual TInt _C_InsertElement( CStifItemParser& aItem );
		virtual TInt _S_AddElement( CStifItemParser& aItem );
		virtual TInt _S_RemoveElement( CStifItemParser& aItem );
		virtual TInt _C_ReplaceElement( CStifItemParser& aItem );
		virtual TInt _S_AsXml( CStifItemParser& aItem );
		virtual TInt _S_AsXmlUnicode( CStifItemParser& aItem );
		virtual TInt _C_AsElement( CStifItemParser& aItem );
		virtual TInt _C_CopyFrom( CStifItemParser& aItem );
		virtual TInt _C_Detach( CStifItemParser& aItem );
		virtual TInt _C_Child( CStifItemParser& aItem );
		virtual TInt _C_AllocAttrValue( CStifItemParser& aItem );
		virtual TInt _S_Set( CStifItemParser& aItem );
		virtual TInt _S_WriteAsXMLTo( CStifItemParser& aItem );
		virtual TInt _S_WriteNamespacesTo( CStifItemParser& aItem );
		virtual TInt _S_ContentWriteStream( CStifItemParser& aItem );
		virtual TInt _S_SetAttributes( CStifItemParser& aItem );

/*################################################
*
*        
*   SERVICE DESCRIPTION DLL
*
*
*################################################*/
/*-----------------------------------------------
            Service Connection
-----------------------------------------------*/
        virtual TInt _S_NewServiceConnection( CStifItemParser& aItem );
   		virtual TInt _S_CompleteServerMessagesOnOff( CStifItemParser& aItem );
		virtual TInt _S_TransactionCompleted( CStifItemParser& aItem );
		virtual TInt _S_StartTransaction( CStifItemParser& aItem );

/*-----------------------------------------------
            XML Service Description
-----------------------------------------------*/
        
        virtual TInt _S_NewXMLServiceDescription( CStifItemParser& aItem );
        virtual TInt _S_EndPoint( CStifItemParser& aItem );
        virtual TInt _S_Contract( CStifItemParser& aItem );
        virtual TInt _S_FrameworkId( CStifItemParser& aItem );
        virtual TInt _C_ProviderIDToService( CStifItemParser& aItem );
        virtual TInt _S_ConsumerIapId( CStifItemParser& aItem );
/*-----------------------------------------------
            Service Pattern
-----------------------------------------------*/
        virtual TInt _S_NewServicePattern( CStifItemParser& aItem );
/*-----------------------------------------------
            IdentityProvider
-----------------------------------------------*/
        virtual TInt _S_NewIdentityProvider( CStifItemParser& aItem );
        virtual TInt _S_ProviderID( CStifItemParser& aItem );
        virtual TInt _S_UserInfo( CStifItemParser& aItem );
/*################################################
*
*        
*   UTILS
*
*
*################################################*/

/*-----------------------------------------------
            SoapEnvelope
-----------------------------------------------*/
        virtual TInt _C_Body( CStifItemParser& aItem );
/*-----------------------------------------------
            SoapDateUtils
-----------------------------------------------*/
        virtual TInt _C_FromXmlDateTime( CStifItemParser& aItem );
        virtual TInt _C_ToXmlDateTimeUtf( CStifItemParser& aItem );



/*################################################
            MService Consumer implementation
################################################*/
		virtual void HandleMessageL(const TDesC8& aMessage);
		//virtual void HandleErrorL(const TDesC8& aError);
        virtual void HandleErrorL(const int aErrorCode, const TDesC8& aError);
		virtual void SetStatus(const TInt aStatus);

/*################################################
            Common
################################################*/
        virtual TInt _X_WaitForResponse( CStifItemParser& aItem );
        virtual TInt _X_Reference( CStifItemParser& aItem );
        virtual TInt _X_ReferenceFragment( CStifItemParser& aItem );
        virtual TInt _X_CheckResultFL( CStifItemParser& aItem );
        virtual TInt _X_LogResult( CStifItemParser& aItem );
        virtual TInt _X_Init( CStifItemParser& aItem );
        virtual TInt _X_DeleteFile( CStifItemParser& aItem );
		virtual TInt _X_TimeOut(CStifItemParser& aItem );
        virtual TInt _X_Flag(CStifItemParser& aItem );
		virtual TInt _X_NonIncludance( CStifItemParser& aItem );
		virtual TInt _X_Includance( CStifItemParser& aItem );
		virtual TInt _X_DeBase64( CStifItemParser& aItem );
		virtual TInt _X_CheckDir( CStifItemParser& aItem );
		virtual TInt _X_CheckFile( CStifItemParser& aItem );
		virtual TInt _X_SOAPRequest( CStifItemParser& aItem );		
		virtual TInt _X_NonExistence( CStifItemParser& aItem );
		virtual TInt _X_Iteration( CStifItemParser& aItem );

/*################################################
            IAP Selection
################################################*/


		virtual TInt _X_IAPList( CStifItemParser& aItem );
//		virtual TInt _C_SetIAP( CStifItemParser& aItem );
		

/*################################################
            Complex
################################################*/
        virtual TInt _C_ParseDomFragment( CStifItemParser& aItem );
        virtual TInt _C_ParseXMLDocument( CStifItemParser& aItem );
        virtual TInt _C_Dump(CStifItemParser& aItem );
   		virtual TInt _C_ParseToElement(CStifItemParser& aItem );


		void SendBUCFTest();
		void SendYomiTest();
		void SendInvalidTest();
		void SendInvalidSOAPTest();
		void SendBUCFBodyTest();
		void SendBUCFEnvelopeTest();
		void SendBUCFEnvelopeTest2();
        void SendSOAPTest();
        void SubmitSOAPTest();
   		void SendSOAPRequest(TInt aDelay);
		void SubmitSOAPRequest();        
		void CheckConnection();
		void PerformanceSOAPSubmit();

        TInt CheckResultL(TPtrC aSrcFile, TPtrC aReferenceFile);
        TInt CheckResultFragmentL(TPtrC aSrcFile, TPtrC aReferenceFile);
        void LogResultL(const TDesC16& aDesc16);
        void LogResultL(const TDesC8& aDesc8);
        TPtrC8 GetParC(CStifItemParser& aItem);
        void PopAndDestroyArgc();
   		CSenElement* ParseToElementL( HBufC8* pTestXml );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
		RStringPool iStringPool;
        //logging
        CStifLogger* iLog;
        //For decrypting data....
				TBase64 iBase64Codec;
        RFs	iFsSession;
        TInt iRunCase;
        TInt iStarted;
        //for implemented void methos, if f.ex. handle message is wrongwe have to signalize error
        TInt iErrorsOccured;
        //counter for arguments in script, arguments have to destroyed after parsing
        TInt iArgc;
   		//RXMLReader iXmlReader;
        CSenXmlReader* iXmlReader;
   		CSenElement* iElement;


        RFile iResultFile;
        RFileWriteStream iWriter;
        TInt result;
        TPtrC iResFile;
        TInt iTestIteracion;

        // simple query
		TBuf8<255>	iSOAPRequest;

		TBool iCasePassed;

        
        CSenSoapMessage* iSOAPMessage;
//Service Connection
        CSenServiceConnection* iSenServiceConnection;
//Service Description
        CSenXmlServiceDescription* iSenXmlServiceDescription;
//Service manager
        CSenServiceManager*     iServiceManager;
//IdentityProvider
        CSenIdentityProvider*   iProvider;
//Credential
        CSenCredential*   iSenCredential;

		CSAXTester*		iSaxTester;
		CSenBaseFragment* iFrg;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };
#endif      // WSTESTER_H
            
// End of File
