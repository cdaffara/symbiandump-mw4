/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef USBUINOTIFAPITEST_H
#define USBUINOTIFAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include <usbuinotif.h>
#include <usbpersonalityids.h>
#include <centralrepository.h>
#include <UsbWatcherInternalCRKeys.h>
#include <usbman.h>
#include <usbwatcher.h>
#include <usb/hostms/srverr.h> 
// CONSTANTS

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KUSBUiNotifApiTestLogPath, "e:\\testing\\stiflogs\\" ); 

// Log file
_LIT( KUSBUiNotifApiTestLogFile, "USBUiNotifApiTest.txt" ); 
_LIT( KUSBUiNotifApiTestLogFileWithTitle, "USBUiNotifApiTest_[%S].txt" );

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CUSBUiNotifApiTest;

// DATA TYPES
enum TTestOption
    {
    EQueryDiscarded = 0,
    EQueryAccepted,
    EQueryCanceled
    };

// CLASS DECLARATION

/**
*  CUSBUiNotifApiTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CUSBUiNotifApiTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUSBUiNotifApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CUSBUiNotifApiTest();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CUSBUiNotifApiTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */
        virtual TInt CableConnectedNotifierTest( CStifItemParser& aItem );
        virtual TInt FinishCableConnectedQuery( CStifItemParser& aItem );
        virtual TInt UsbQueriesNotifierTest( CStifItemParser& aItem );
        virtual TInt UsbOTGErrorNotifierTests( CStifItemParser& aItem );
        virtual TInt UsbOTGWarningNotifierTests( CStifItemParser& aItem );
        virtual TInt FinishQuery( CStifItemParser& aItem );
        virtual TInt LoadNotifiersL( CStifItemParser& aItem );
        virtual TInt UnLoadNotifiers ( CStifItemParser& aItem );
        virtual TInt UsbMSMMNotifierTests ( CStifItemParser& aItem );
        virtual TInt CancelMsmmNotifier( CStifItemParser& aItem );
        virtual TInt CancelQueryNotifier( CStifItemParser& aItem );
        virtual TInt CancelOtgErrorNotifier( CStifItemParser& aItem );
        virtual TInt CancelOtgWarningNotifier( CStifItemParser& aItem );
        virtual TInt CancelCableConnectedNotifier( CStifItemParser& aItem );
        virtual TInt WaitForRequest( CStifItemParser& aItem );
        virtual TInt SynchStart( CStifItemParser& aItem );
        virtual TInt Update( CStifItemParser& aItem );
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
        TInt GetTestOption( TPtrC aOptionString, TTestOption& aOption );
        TInt GetPersonalityIdFromString( TPtrC aOptionString, TInt& aPersonalityId );
        TInt GetQueryType( TPtrC aTypeString, TUSBUIQueries& aQueryType );        
        TInt GetOTGErrorType( TPtrC aTypeString, TUsbUiNotifOtgError& aQueryType );
        TInt GetOTGWarningType( TPtrC aTypeString, TUsbUiNotifOtgWarning& aQueryType );
        TInt GetMSMMrrorType( TPtrC aTypeString, THostMsErrCode& aQueryType );
        TInt FindAndKillProcess(const TDesC& aProcessName);

    public:     // Data

    protected:  // Data

    private:    // Data
        
        RNotifier *iNotifier;
        TBool iNotifierConnected;
        RUsbWatcher* iUsbWatcher;
 
        CRepository* iRepository;
        
        TUSBConnectionNotifierParamsPckg iConnectionNotifierResponseParamsPckg;
        TRequestStatus iReqStatus;
        TInt iPersonalityIdBackup;
        TBool iCompleteQuery;        
        TPckg<TInt> iRes;
        CArrayPtr<MEikSrvNotifierBase2>* iNotifierArray;
        TUid iInstanceUid;
        THostMsErrData iErrData;
        THostMsErrorDataPckg iErrPckg ;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // USBUINOTIFAPITEST_H

// End of File
