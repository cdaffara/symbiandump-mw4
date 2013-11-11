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









#ifndef SENCONNECTIONTIMEOUTSTESTER_H
#define SENCONNECTIONTIMEOUTSTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <MSenServiceConsumer.h>
#include <SenBaseFragment.h>
#include <SenBaseElement.h>
#include <SenXmlReader.h>

#include <f32file.h>
#include <e32base.h> // for CActive
#include <e32std.h>
#include <unistd.h>
#include <aknnotewrappers.h> 

// CONSTANTS
//const ?type ?constant_var = ?constant;

//  INTERNAL INCLUDES
namespace
    {
    _LIT16(KSessionsFile,"c:\\private\\101f96f4\\SenSessions.xml");
    _LIT16(KIdentitiesFile,"c:\\private\\101f96f4\\senidentities.xml");
    }

enum TUT_CSenConnectionTimeoutsTester
    {
    ENotInitialized = 1,
    EConnectionNotReady,
    };
//  FORWARD DECLARATIONS
class CSenBaseFragment;
class CSenBaseElement;
class CSenSoapEnvelope;
class SenXmlUtils;
class CSenServiceConnection;
class CSenXmlReader;
class CSenDomFragment;
class CSenXmlServiceDescription;
class CSenServicePattern;
class CSenServiceManager;
class CSenSoapMessage;
class CSenSoapMessage2;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif
// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KSenConnectionTimeoutsTesterLogPath, "\\logs\\testframework\\SenConnectionTimeoutsTester\\" ); 
// Log file
_LIT( KSenConnectionTimeoutsTesterLogFile, "SenConnectionTimeoutsTester.txt" ); 
_LIT( KSenConnectionTimeoutsTesterLogFileWithTitle, "SenConnectionTimeoutsTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSenConnectionTimeoutsTester;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
typedef TInt (CSenConnectionTimeoutsTester::* TestFunction)( CStifItemParser&);

// CLASS DECLARATION

/**
*  CSenConnectionTimeoutsTester test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSenConnectionTimeoutsTester) : public CScriptBase, public MSenServiceConsumer 
    {
    public:
        enum TCallback
            {
            ECallbackNone = 0,
            ECallbackHandleMessage,
            ECallbackHandleError
            };
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSenConnectionTimeoutsTester* NewL( CTestModuleIf& aTestModuleIf );
        static CSenConnectionTimeoutsTester* NewL();
        static CSenConnectionTimeoutsTester* NewLC();

        /**
        * Destructor.
        */
        virtual ~CSenConnectionTimeoutsTester();

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
        CSenConnectionTimeoutsTester( CTestModuleIf& aTestModuleIf );
        CSenConnectionTimeoutsTester();

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
        * Test methods are listed below. 
        */

        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;
    private:    // New methods
    
            // MService Consumer implementation
        void HandleMessageL(const TDesC8& aMessage);        
        void HandleErrorL(const int aErrorCode, const TDesC8& aError);
        void SetStatus(const TInt aStatus);

        void SetupL();

        void Teardown();

        void ErrorL(TInt aCode);


        TInt CSenConnectionTimeoutsTester_TP_Async1(CStifItemParser& aItem);
        
        TInt CSenConnectionTimeoutsTester_TP_Async2(CStifItemParser& aItem);     
        
        
        
        TInt CSenConnectionTimeoutsTester_TP_Send(const TDesC8& aLogFileName,
                                              const TDesC8& aCustomHttpHeader,
                                              const TDesC8& aCustomHttpHeaderValue,
                                              CSenServiceConnection* iServiceConnection);
                                              
        TInt CSenConnectionTimeoutsTester_TP_CheckLogs(const TDesC& aLogFilePath,
                                                   const TDesC8& aCustomHttpHeader,
                                                   const TDesC8& aCustomHttpHeaderValue);                                    
                                                   
        TUint32 SearchIAPIdByName(const TDesC& aIAPName);
        
        CSenSoapMessage* CreateAuthRequestLC();
        
        CSenSoapMessage2* CreateNewAuthRequestLC();
        
        HBufC8* ReadFileL(const TDesC& aFilename);
        
        HBufC8* HttpMethodValueFromXmlLC(const TDesC8& aXml);
        
        HBufC8* HttpPropertyValueFromXmlLC(const TDesC8& aXml,
                                           const TDesC8& aPropertyName);
                                           
        void StartActiveScheduler(TInt aNumberOfSentMessages);
        void StopActiveScheduler();

    private:    // Data
    TestFunction iMethod;
        // Pointer to logger
        CStifLogger * iLog; 
        
        CSenXmlServiceDescription*  iSenXmlServiceDescription;
        CSenXmlServiceDescription*  iSession;
        CSenServiceConnection*      iServiceConnection;
        CSenServiceManager*         iManager;
        CSenXmlReader*              iXmlReader;
        
        TInt                        iTransactionId;
        TInt                        iConnectionState;
        TBool                       iInitialized;
        TBool                       iReady;
        
        TCallback                   iCallback;
        HBufC8*                     ipMessage;
        
        TInt                        iNumberOfPendingOps;
        HBufC8*                     ipFrameworkID;
        
        TInt                        iErrorCode;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // SENCONNECTIONTIMEOUTSTESTER_H

// End of File
