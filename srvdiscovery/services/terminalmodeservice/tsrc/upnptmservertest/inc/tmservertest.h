/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TMSERVERTEST_H
#define TMSERVERTEST_H

// INCLUDES
#include "StifTestModule.h"
#include "testtimer.h"
#include "discoverytesttimer.h"

#include <StifLogger.h>
#include <NormalHardcodedAssert.h>
#include <upnptmserver.h>
#include <upnptmserverdeviceinfo.h>
#include <upnpcontrolpoint.h>
#include <upnpremotableapp.h>
#include <upnpterminalmodeicon.h>
#include <terminalmodeconsts.h>
#include <upnptmserverobserver.h>

// MACROS
#define TEST_MODULE_VERSION_MAJOR 0
#define TEST_MODULE_VERSION_MINOR 0
#define TEST_MODULE_VERSION_BUILD 0

// Logging path
_LIT( KTmServerTestLogPath,         "\\logs\\testframework\\upnptmservertest\\" ); 
// Log file
_LIT(KTmServerTestLogFile,          "upnptmservertest.txt" ); 
_LIT(KTmServerTestLogFileWithTitle, "upnptmservertest_[%S].txt" );

const TInt KIconWidth               = 21;
const TInt KIconHeight              = 21;
const TInt KIconDepth               = 24;
const TInt KIconWidth1              = 88;
const TInt KIconHeight1             = 88;
const TInt KIconWidth2              = 36;
const TInt KIconHeight2             = 44;
const TInt KIconWidth3              = 60;
const TInt KIconHeight3             = 60;
const TInt KIconDepth1              = 12;
const TUint KProfileIdValue         =  0;
const TUint KAppIdValue             =  1;
const TUint KAppId1Value            =  2;
const TUint KAppId2Value            =  3;
const TUint KAppId3Value            =  4;
const TUint KAppId4Value            =  5;
const TUint KAppId5Value            =  6;
const TUint KNonExistentAppIdValue  =  7;
const TUint KNonExistentAppId1Value =  8;
const TUint KInvalidProfileIdValue  =  9;
const TInt  KInvalidAppIdValue      = -1;


#define GETPTR &
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FORWARD DECLARATIONS
class CTmServerTest;

// DATA TYPES
// A typedef for function that does the actual testing,
// function is a type 
typedef TInt (CTmServerTest::* TestFunction)(TTestResult&);

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


class CDiscoverDevice : public CUpnpControlPoint
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CDiscoverDevice* NewL(CTmServerTest& aTmServerTest);

    /**
     * Destructor.
     */
    virtual ~CDiscoverDevice();

protected:
    // Functions from base classes

    /**
     * From CUpnpControlPoint State update handler.
     */
    void StateUpdatedL( CUpnpService* aService );

    /**
     * From CUpnpControlPoint Device discovery handler.
     */
    void DeviceDiscoveredL( CUpnpDevice* aDevice );

    /**
     * From CUpnpControlPoint Device dissappear handler.
     */
    void DeviceDisappearedL( CUpnpDevice* aDevice );

    /**
     * From CUpnpControlPoint Action response handler function.
     */
    void ActionResponseReceivedL( CUpnpAction* aAction );

    /**
     * From CUpnpControlPoint HTTP message handler function.
     */
    void HttpResponseReceivedL( CUpnpHttpMessage* aMessage );

            
    TInt TmServerStateUpdated( CUpnpDevice& aDevice,   CUpnpService*& aService );
                                                         
                                                         
public:

    /**
     * C++ default constructor.
     */
    CDiscoverDevice(CTmServerTest& aTmServerTest );

    void ConstructL(); 
    TInt LaunchAppActionL( TInt aAppId, TInt aProfileId  );
    TInt TerminateAppActionL( TInt aAppId, TInt aProfileId );
    TInt GetAppStatusActionL( TInt aAppId );
    TInt GetAppListActionL(const TDesC8& aAppListFilter ,TInt aProfileId );
    TInt SetClientProfileActionL(TInt aProfileId, const TDesC8& aClientInfoValue );
    TInt GetClientProfileActionL(TInt aProfileId );
    TInt GetMaxNumProfilesActionL();
    void SubscribeToServiceL();
        
public:
    CTmServerTest&   iTmServerTest;  
    };

// CLASS DECLARATION

/**
*  This a TmServerTest class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CTmServerTest) : public CTestModuleBase,
                                   public MUpnpTmServerObserver
    {
public:  // Constructors and destructor


    /**
    * Two-phased constructor.
    */
    static CTmServerTest* NewL();

    /**
    * Destructor.
    */
    virtual ~CTmServerTest();

public: // New functions

    /**
    * ?member_description.
    * @since ?Series60_version
    * @param ?arg1 ?description
    * @return ?description
    */
    //?type ?member_function( ?type ?arg1 );
    TInt StartTmServerDeviceL ( TTestResult& aResult );
    TInt PublishTmServerServiceL ( TTestResult& aResult );
    TInt StopTmServerDeviceL ( TTestResult& aResult );
    TInt ReStartTmServerDeviceL( TTestResult& aResult );
    TInt UnRegisterAppL(  TTestResult& aResult  );
    TInt UnRegisterAppsL(  TTestResult& aResult  );
    TInt RegisterDuplicateAppL(  TTestResult& aResult  );
    TInt RegisterDuplicateAppsL(  TTestResult& aResult  );
    TInt UnRegisterUnRegisteredAppL ( TTestResult& aResult  );
    TInt UnRegisterUnRegisteredAppsL ( TTestResult& aResult  );   
    TInt HandleLaunchAppActionL ( TTestResult& aResult );
    TInt HandleInvalidAppIdLaunchAppActionL(TTestResult& aResult );     
    TInt HandleInvalidProfileIdLaunchAppActionL(TTestResult& aResult );   
    TInt HandleNonExistentLaunchAppActionL (TTestResult& aResult);
    TInt HandleUnauthorizedLaunchAppActionL (TTestResult& aResult);  
    TInt HandleDeviceLockedLaunchAppActionL (TTestResult& aResult); 
    TInt HandleTerminateAppActionL ( TTestResult& aResult );
    TInt HandleNonExistentTerminateAppActionL( TTestResult& aResult );
    TInt HandleInvalidAppIdTerminateAppActionL( TTestResult& aResult );
    TInt HandleInvalidProfileIdTerminateAppActionL( TTestResult& aResult );
    TInt HandleUnauthorizedTerminateAppActionL ( TTestResult& aResult );   
    TInt HandleDeviceLockedTerminateAppActionL ( TTestResult& aResult );
    TInt HandleGetAppListActionFilter1L(TTestResult& aResult );
    TInt HandleGetAppListActionFilter2L(TTestResult& aResult );
    TInt HandleGetAppListActionFilter3L(TTestResult& aResult );
    TInt HandleGetAppListActionFilter4L(TTestResult& aResult );
    TInt HandleGetAppListActionInvalidArg1L(TTestResult& aResult );
    TInt HandleGetAppListActionInvalidArg2L(TTestResult& aResult );
    TInt HandleGetAppListActionInvalidArg3L(TTestResult& aResult );  
    TInt HandleGetAppListActionInvalidArg4L(TTestResult& aResult );
    TInt HandleGetAppListActionInvalidArg5L(TTestResult& aResult );
    TInt HandleInvalidProfileIdGetAppListActionL ( TTestResult& aResult );
    TInt HandleGetAppListActionOperationRejectedL ( TTestResult& aResult );
    TInt HandleGetAppListActionWildCardFilterL ( TTestResult& aResult );
    TInt HandleGetAppListActionNoFilterL ( TTestResult& aResult );
    TInt HandleGetAppStatusActionL( TTestResult& aResult );
    TInt HandleWildcardGetAppStatusActionL( TTestResult& aResult );
    TInt HandleInvalidAppIdGetAppStatusActionL ( TTestResult& aResult );
    TInt HandleNonExistentGetAppStatusActionL ( TTestResult& aResult );   
    TInt HandleSetClientProfileActionL( TTestResult& aResult );
    TInt HandleInvalidProfileIdSetClientProfileActionL (TTestResult& aResult ); 
    TInt HandleInvalidProfileSetClientProfileActionL (TTestResult& aResult );   
    TInt HandleDeviceLockedSetClientProfileActionL (TTestResult& aResult );  
    TInt HandleResourceBusySetClientProfileActionL (TTestResult& aResult ); 
    TInt HandleGetClientProfileActionL( TTestResult& aResult );
    TInt HandleInvalidProfileIdGetClientProfileActionL (TTestResult& aResult );
    TInt HandleGetMaxNumProfilesActionL( TTestResult& aResult );
    TInt HandleGetMaxNumProfilesOperationRejectedL ( TTestResult& aResult );
    TInt HandleEventAppStatusUpdateL ( TTestResult& aResult );
    TInt HandleEventAppListUpdateL ( TTestResult& aResult );
    TInt HandleEventDuplicateAppStatusUpdateL(  TTestResult& aResult  );  
    TInt HandleEventProfileIdListStatusUpdateL(  TTestResult& aResult  );
    
    void RegisterApplicationsL(CUpnpTmServer& aTmServer );  
    void SetTmServerSettingsL();
    void TestTimerTimedOutL(); 
    void TestTimerDiscoveryTimedOut(); 
    void DeviceVerified();
    void DeviceLostVerified();
    void ServiceVerified();

public: // methods from MUpnpTmServerObserver
    TTerminalModeErrorCode OnGetApplicationList( TUint aProfileId );
    TTerminalModeErrorCode OnLaunchApp( TUint aAppId, RBuf8& aUrl, TUint aProfileId );
    TTerminalModeErrorCode OnTerminateApp( TUint aAppId, TUint aProfileId );
    TTerminalModeErrorCode OnGetAppStatus( TUint aAppId, TUint& aProfileId ,RBuf8& aStatusType );
    TTerminalModeErrorCode OnGetMaxNumProfiles( TUint& aProfileIdCount );
    TTerminalModeErrorCode OnSetClientProfile( TUint aProfileId, const TDesC8& aClientProfile,RBuf8& aResultProfile ); 
    TTerminalModeErrorCode OnGetClientProfile( TUint aProfileId, RBuf8& aClientProfile );            
    const CUpnpTmClientIconPref& GetClientIconPrefsL( TUint aProfileId );

public: // Functions from base classes

    /**
    * From CTestModuleBase InitL is used to initialize the 
    *       TmServerTest. It is called once for every instance of 
    *       TestModuleTmServerTest after its creation.
    * @since ?Series60_version
    * @param aIniFile Initialization file for the test module (optional)
    * @param aFirstTime Flag is true when InitL is executed for first 
    *               created instance of TmServerTest.
    * @return Symbian OS error code
    */
    TInt InitL( TFileName& aIniFile, TBool aFirstTime );

    /**
    * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
    *   from TmServerTest. 
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

private:

    /**
    * C++ default constructor.
    */
    CTmServerTest();

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

    /**
    * [test case declarations] - do not remove
    */
    //ADD NEW METHOD DEC HERE"

    /**
    * Printing test case.
    * @since ?Series60_version
    * @param aResult Test case result (PASS/FAIL)
    * @return Symbian OS error code (test case execution error 
    *   that is not returned as test case result in aResult)
    */
    TInt PrintTest( TTestResult& aResult );

    /**
    * Printing loop test case.
    * @since ?Series60_version
    * @param aResult Test case result (PASS/FAIL)
    * @return Symbian OS error code (test case execution error 
    *   that is not returned as test case result in aResult)
    */
    TInt LoopTest( TTestResult& aResult );
        
       

public:     // Data
    // ?one_line_short_description_of_data
    //?data_declaration;
    TBool                              iTmServerDeviceType;
    TBool                              iTmServerServiceType;
           
protected:  // Data
    // ?one_line_short_description_of_data
    //?data_declaration;
        
public:
    TBool                              iGetAppList;
    TBool                              iStartApp;
    TBool                              iStopApp;
    TBool                              iSetClientProfile;
    TBool                              iGetClientProfile;
    TBool                              iGetMaxNumProfiles;
    TBool                              iGetAppStatus;
    TBool                              iStatusNotified;
    TBool                              iOperationRejected;
    TBool                              iResourceBusy;
       
        
private:    // Data
    // Pointer to test (function) to be executed
    TestFunction iMethod;

    // Pointer to logger
    CStifLogger * iLog; 

    // Normal logger
    CStifLogger* iStdLog;

    // Test case logger
    CStifLogger* iTCLog;

    // Flag saying if test case title should be added to log file name
    TBool iAddTestCaseTitleToLogName;

    // Flag saying if version of test module was already sent
    TBool iVersionLogged;

private:    
    CUpnpTmServer*                       iTmServer;
    CDiscoverDevice*                     iSearchDevice;
    CTestTimer*                          iTestTimer;
    CDiscoveryTestTimer*                 iDiscoveryTestTimer;
    CUpnpRemotableApp*                   iRemotableApp;
    CUpnpTerminalModeIcon*               iTerminalModeIcon;
    CUpnpRemotableApp*                   iRemotableApp1;
    CUpnpRemotableApp*                   iRemotableApp2;
    CUpnpRemotableApp*                   iRemotableApp3;
    CUpnpRemotableApp*                   iRemotableApp4;  
    CUpnpRemotableApp*                   iRemotableApp5;           
    CUpnpTmServerDeviceInfo*             iTmServerDeviceInfo;
    RPointerArray<CUpnpRemotableApp>     iRemotableAppArray;  
    TBool                                iDeviceVerified;
    TBool                                iDeviceLostVerified;
    TBool                                iServiceVerified;
    RPointerArray<CUpnpTmClientIconPref> iClientIconPrefArray;
    };

#endif      // TMSERVERTEST_H

// End of File
