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


// INCLUDE FILES
#include <e32math.h>
#include <ecom/ecom.h>
#include <upnpdevice.h>
#include <upnptmserverdeviceinfo.h>
#include <upnpsettings.h>
#include "testtimer.h"
#include "discoverytesttimer.h"
#include "tmservertest.h"


// LOCAL CONSTANTS AND MACROS
_LIT8(KTestSvgMimeType,           "image/svg+xml");
_LIT8(KTmServerDevice,            "urn:schemas-upnp-org:device:TmServerDevice:1" );
_LIT8(KTmServerService,           "urn:schemas-upnp-org:service:TmApplicationServer:1" );
_LIT8(KTmServerServiceType,       "TmApplicationServer:1" );
_LIT8(KTmServerServiceType1,      "TmClientProfile:1" );
_LIT8(KBtMacValue,                "00257df016d5");
_LIT8(KArgTypeAppStatusUpdate,    "AppStatusUpdate");
_LIT8(KGetAppStatus,              "GetApplicationStatus");
_LIT8(KAppStatus,                 "AppStatus");
_LIT8(KGetApplicationList,        "GetApplicationList");
_LIT8(KAppListing,                "AppListing");
_LIT8(KLaunchApp,                 "LaunchApplication");
_LIT8(KTerminateApp,              "TerminateApplication");
_LIT8(KAppLaunchStatus,           "AppURI");
_LIT8(KAppTerminateStatus,        "TerminationResult");
_LIT8(KAppId ,                    "AppID");
_LIT8(KAppFilter,                 "AppListingFilter");
_LIT8(KResultProfile ,            "resultProfile");
_LIT8(KSetClientProfile ,         "SetClientProfile");
_LIT8(KGetClientProfile,          "GetClientProfile");
_LIT8(KGetMaxNumProfiles,         "GetMaxNumProfiles");
_LIT8(KClientProfile ,            "clientProfile");
_LIT8(KNumProfilesAllowed ,       "numProfilesAllowed");
_LIT8(KProfileId,                 "ProfileID");
_LIT8(KProfileId1,                "profileID");
_LIT8(KBackground,                "Background");
_LIT8(KForeground,                "Foreground");
_LIT8(KAppListFilterValue1,       "\"icon@mimetype=\"*svg+xml*\",icon@width=\"*\",icon@height=\"*\",icon@depth=\"*\"\"");
_LIT8(KAppListFilterValue2,       "\"name=\"*nav*\",description=\"*\",appInfo@appCategory=\"*\",appInfo@trustLevel=\"*\",displayInfo@contentCategory=\"*\",displayInfo@contentRules=\"*\",displayInfo@trustLevel=\"*\",allowedProfileIDs=\"*\"\"");
_LIT8(KAppListFilterValue3,       "\"name=\"*bluetooth*\",description=\"*\",icon@mimetype=\"*svg+xml*\",remotingInfo@protocolID=\"*\",remotingInfo@direction=\"*\",appInfo@appCategory=\"*\",audioInfo@audioType=\"*\",resourceStatus=\"free\",signature=\"*\"\"");
_LIT8(KAppListFilterValue4,       "\"name=\"*\",description=\"*Audio*\",icon@mimetype=\"*svg+xml*\",remotingInfo@protocolID=\"*\",remotingInfo@format=\"*\",appInfo@appCategory=\"*\",audioInfo@audioType=\"*\",resourceStatus=\"free\",signature=\"*\"\"");
_LIT8(KWildCard,                  "*");
_LIT(KIconPathMusicUnLaunched,          "c:\\upnptmserver\\icons\\music_unlaunched.svg");
_LIT(KIconPathMusicLaunched,            "c:\\upnptmserver\\icons\\music_launched.svg");
_LIT(KIconPathMaps,                     "c:\\upnptmserver\\icons\\maps.svg");
_LIT(KIconPathBtA2DpUnLaunched,         "c:\\upnptmserver\\icons\\gallery.svg");
_LIT(KIconPathBtA2DpLaunched,           "c:\\upnptmserver\\icons\\homemedia.svg");
_LIT(KIconPathBtHfp,                    "c:\\upnptmserver\\icons\\videos.svg");
_LIT(KIconDevicePath,                   "c:\\upnptmserver\\icons\\s60_device.svg");    
_LIT8(KDeviceInfo,                      "<X_Nokia_deviceInfo><version><major>1</major><minor>0</minor></version><mtp_tc>true</mtp_tc>"
    "<license_model>pre,post</license_model><sound_feedback><enabled>true</enabled></sound_feedback><ipRouting><enabled>false</enabled></ipRouting>"
    "</X_Nokia_deviceInfo>");

_LIT8(KClientProfileInfo, "&lt;clientProfile&gt;&lt;clientID&gt;Cl_1&lt;/clientID&gt;&lt;friendlyName&gt;Client One&lt;/friendlyName&gt;&lt;manufacturer&gt;man_2&lt;/manufacturer&gt;&lt;modelName&gt;CL_Model2&lt;/modelName&gt;&lt;modelNumber&gt;2009&lt;/modelNumber&gt;&lt;iconPreference&gt;&lt;width&gt;240&lt;/width&gt;&lt;height&gt;240&lt;/height&gt;&lt;/iconPreference&gt;&lt;contentRules&gt;&lt;rule&gt;&lt;ruleId&gt;0&lt;/ruleId&gt;&lt;ruleValue&gt;0.1&lt;/ruleValue&gt;&lt;/rule&gt;&lt;rule&gt;&lt;ruleId&gt;1&lt;/ruleId&gt;&lt;/rule&gt;&lt;/contentRules&gt;&lt;/clientProfile&gt;");

_LIT8(KDeviceLockedClientProfile, "&lt;clientProfile&gt;&lt;clientID&gt;Cl_2&lt;/clientID&gt;&lt;/clientProfile&gt;");

_LIT8(KXmlSignature, "&lt;Signature Id= &quot;AppListSignature&quot; xmlns=&quot;http://www.w3.org/2000/09/xmldsig#&quot;&gt; &lt;SignedInfo&gt;&lt;CanonicalizationMethod Algorithm=&quot;http://www.w3.org/2006/12/xml-c14n11&quot;/&gt;&lt;SignatureMethodAlgorithm=&quot;http://www.w3.org/2000/09/xmldsig#dsa-sha1&quot;/&gt; &lt;Reference URI=&quot;&quot;&gt;&lt;/Reference&gt;&lt;/SignedInfo&gt;&lt;/Signature&gt;");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTmServerTest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CTmServerTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    TmServerTest.cpp file and to TmServerTest.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // [test cases entries] - do not remove
        
        ENTRY( "Start TM Server Device", CTmServerTest::StartTmServerDeviceL ),
        ENTRY( "Publish TM Server Service", CTmServerTest::PublishTmServerServiceL ),
        ENTRY( "Stop TM Server Device", CTmServerTest::StopTmServerDeviceL ),
        ENTRY( "ReStart TM Server Device", CTmServerTest::ReStartTmServerDeviceL ),         
        ENTRY( "Handle LaunchApp Command", CTmServerTest::HandleLaunchAppActionL ),
        ENTRY( "Handle Invalid AppId LaunchApp Command", CTmServerTest::HandleInvalidAppIdLaunchAppActionL ),
        ENTRY( "Handle Invalid ProfileId LaunchApp Command", CTmServerTest::HandleInvalidProfileIdLaunchAppActionL ),
        ENTRY( "Handle LaunchApp Command For Non-Existent AppID", CTmServerTest::HandleNonExistentLaunchAppActionL ),
        ENTRY( "Handle LaunchApp Command For UnAuthorized AppID", CTmServerTest::HandleUnauthorizedLaunchAppActionL ),
        ENTRY( "Handle Device Locked Condition For LaunchApp Command", CTmServerTest::HandleDeviceLockedLaunchAppActionL ),
        ENTRY( "Handle TerminateApp Command", CTmServerTest::HandleTerminateAppActionL),
        ENTRY( "Handle TerminateApp Command For Non-Existent AppID", CTmServerTest::HandleNonExistentTerminateAppActionL),
        ENTRY( "Handle Invalid AppId TerminateApp Command", CTmServerTest::HandleInvalidAppIdTerminateAppActionL),
        ENTRY( "Handle Invalid ProfileId TerminateApp Command", CTmServerTest::HandleInvalidProfileIdTerminateAppActionL),
        ENTRY( "Handle TerminateApp Command For UnAuthorized AppID", CTmServerTest::HandleUnauthorizedTerminateAppActionL),
        ENTRY( "Handle Device Locked Condition For TerminateApp Command", CTmServerTest::HandleDeviceLockedTerminateAppActionL ),
        ENTRY( "Handle GetAppStatus Command ", CTmServerTest::HandleGetAppStatusActionL ),
        ENTRY( "Handle GetAppStatus Command For All the Applications", CTmServerTest::HandleWildcardGetAppStatusActionL ),
        ENTRY( "Handle GetAppStatus Command For Invalid AppId", CTmServerTest::HandleInvalidAppIdGetAppStatusActionL ),
        ENTRY( "Handle GetAppStatus Command For Non-Existent AppID", CTmServerTest::HandleNonExistentGetAppStatusActionL ),
        ENTRY( "Handle GetAppList Command with Filter1", CTmServerTest:: HandleGetAppListActionFilter1L ), 
        ENTRY( "Handle GetAppList Command with Filter2", CTmServerTest:: HandleGetAppListActionFilter2L ),
        ENTRY( "Handle GetAppList Command with Filter3", CTmServerTest:: HandleGetAppListActionFilter3L ),  
        ENTRY( "Handle GetAppList Command with Filter4", CTmServerTest:: HandleGetAppListActionFilter4L ), 
        ENTRY( "Handle GetAppList Command with WildCard Filter", CTmServerTest:: HandleGetAppListActionWildCardFilterL ), 
        ENTRY( "Handle GetAppList Command with No Filter", CTmServerTest:: HandleGetAppListActionNoFilterL ), 
        ENTRY( "Handle GetAppList Command with Invalid Arg1", CTmServerTest:: HandleGetAppListActionInvalidArg1L ), 
        ENTRY( "Handle GetAppList Command with Invalid Arg2", CTmServerTest:: HandleGetAppListActionInvalidArg2L ),
        ENTRY( "Handle GetAppList Command with Invalid Arg3", CTmServerTest:: HandleGetAppListActionInvalidArg3L ),
        ENTRY( "Handle GetAppList Command with Invalid Arg4", CTmServerTest:: HandleGetAppListActionInvalidArg4L ),
        ENTRY( "Handle GetAppList Command with Invalid Arg5", CTmServerTest:: HandleGetAppListActionInvalidArg5L ),
        ENTRY( "Handle Invalid ProfileId GetAppList Command", CTmServerTest:: HandleInvalidProfileIdGetAppListActionL),
        ENTRY( "Handle Operation Rejected Condition For GetAppList Command", CTmServerTest:: HandleGetAppListActionOperationRejectedL), 
        ENTRY( "Handle SetClientProfile Command", CTmServerTest:: HandleSetClientProfileActionL ),  
        ENTRY( "Handle Invalid ProfileId SetClientProfile Command", CTmServerTest:: HandleInvalidProfileIdSetClientProfileActionL ), 
        ENTRY( "Handle Invalid Profile SetClientProfile Command", CTmServerTest:: HandleInvalidProfileSetClientProfileActionL ),
        ENTRY( "Handle Device Locked Condition For SetClientProfile Command", CTmServerTest:: HandleDeviceLockedSetClientProfileActionL ),
        ENTRY( "Handle Resource Busy Condition For SetClientProfile Command", CTmServerTest:: HandleResourceBusySetClientProfileActionL ),
        ENTRY( "Handle GetClientProfile Command", CTmServerTest:: HandleGetClientProfileActionL ), 
        ENTRY( "Handle Invalid ProfileId GetClientProfile Command", CTmServerTest:: HandleInvalidProfileIdGetClientProfileActionL ), 
        ENTRY( "Handle GetMaxNumProfiles Command", CTmServerTest:: HandleGetMaxNumProfilesActionL ), 
        ENTRY( "Handle Operation Rejected Condition For GetMaxNumProfiles Command", CTmServerTest:: HandleGetMaxNumProfilesOperationRejectedL ),
        ENTRY( "Register Duplicate Application", CTmServerTest::RegisterDuplicateAppL),  
        ENTRY( "Register Duplicate Applications", CTmServerTest::RegisterDuplicateAppsL),       
        ENTRY( "UnRegister Application", CTmServerTest::UnRegisterAppL),        
        ENTRY( "UnRegister Multiple Applications", CTmServerTest::UnRegisterAppsL),
        ENTRY( "UnRegister UnRegistered Application", CTmServerTest::UnRegisterUnRegisteredAppL),
        ENTRY( "UnRegister Multiple UnRegistered Applications", CTmServerTest::UnRegisterUnRegisteredAppsL),
        ENTRY( "Handle Event For AppStatus Update", CTmServerTest::HandleEventAppStatusUpdateL ),
        ENTRY( "Handle Event For AppList Update", CTmServerTest::HandleEventAppListUpdateL ),
        ENTRY( "Handle Event For Duplicate AppStatus Update", CTmServerTest::HandleEventDuplicateAppStatusUpdateL),
        ENTRY( "Handle Event For ProfileIdList Status Update", CTmServerTest::HandleEventProfileIdListStatusUpdateL),
                
        // OOM Testcases    
        OOM_ENTRY( "[OOM_StartTmServerDevice]", CTmServerTest::StartTmServerDeviceL,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_HandleGetAppListWithFilter]", CTmServerTest:: HandleGetAppListActionFilter1L,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_HandleGetAppListWildCardFilter]", CTmServerTest:: HandleGetAppListActionWildCardFilterL,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_HandleSetClientProfileCommand]", CTmServerTest:: HandleSetClientProfileActionL,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_HandleGetAppStatusCommand]", CTmServerTest:: HandleGetAppStatusActionL,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_HandleGetAppStatusForAllApplications]", CTmServerTest::HandleWildcardGetAppStatusActionL,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_HandleAppStatusUpdateEvent]", CTmServerTest::HandleEventAppStatusUpdateL,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_HandleDuplicateAppStatusUpdateEvent]", CTmServerTest::HandleEventDuplicateAppStatusUpdateL,ETrue,1,5 ),
        OOM_ENTRY( "[OOM_UnRegisterApplications]", CTmServerTest::UnRegisterAppsL,ETrue,1,5 )
        };
    
    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;
    }


//========================= TEST CASES =======================================================

TInt CTmServerTest::StartTmServerDeviceL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Start Terminalmode Server Device" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();     
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo,*this );
    
    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer); 
    
    // Start the tmserver device and its services
    iTmServer->StartL();

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if ( iDeviceVerified)
        {
        _LIT( KDescription , "Terminalmode Server Device started successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "Start of Terminalmode Server Device Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::PublishTmServerServiceL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Publish Terminalmode server Service" );
    iLog->Log( KLogInfo );
    
    iTmServerServiceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo,*this );
    
    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);   
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if ( iServiceVerified)
        {
        _LIT( KDescription , "Terminalmode server Service published successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "Publishing of Terminalmode server Service Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        } 
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::StopTmServerDeviceL  ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Stop Terminalmode server Device" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo,*this );
    
    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);   
    
    //Start the tmserver device and its services
    iTmServer->StartL();  
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    if ( iDeviceVerified  )
        {
         //Stop the tmserver device and its services
         iTmServer->StopL(); 
        }    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();         
    if ( iDeviceLostVerified )
        {
        _LIT( KDescription , "Terminalmode server Device stopped successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "Stopping of Terminalmode server Device Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::ReStartTmServerDeviceL ( TTestResult& aResult )
    {
   _LIT( KLogInfo, "ReStart Terminalmode server Device" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo,*this );

    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);    
    //Start the tmserver device and its services
    iTmServer->StartL();
    //Stop the tmserver device and its services
    iTmServer->StopL();
    //Start the tmserver device and its services
    iTmServer->StartL();
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();    
    if ( iDeviceVerified)
        {
        _LIT( KDescription , "Terminalmode server Device re-started successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "Re-starting of Terminalmode server Device Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }    
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    } 

TInt CTmServerTest::RegisterDuplicateAppL(  TTestResult& aResult  )
    {   
    _LIT( KLogInfo, "Register Duplicate Application" );
    iLog->Log( KLogInfo );

    //Sets the information associated with the terminal mode server device    
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
    
    _LIT8(KAppDesc, "Bluetooth A2DP Audio Server");
    iRemotableApp = CUpnpRemotableApp::NewL( KAppId2Value ,_L8("Bluetooth A2DP"));
    iRemotableApp->SetAppDescriptionL(KAppDesc());
       
    CUpnpTmInfoElement& tmInfoElement = iRemotableApp->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement.AddTmInfoElementL(_L8("protocolID"),_L8("BTA2DP"));
    tmInfoElement.AddTmInfoElementL(_L8("direction"),_L8("out"));
       
    CUpnpTmInfoElement& tmInfoElement1 = iRemotableApp->CreateTmInfoElementL(CUpnpTmInfoElement::EAppInfo);
    tmInfoElement1.AddTmInfoElementL(_L8("appCategory"),_L8("0x0000003"));
    tmInfoElement1.AddTmInfoElementL(_L8("trustLevel"),_L8("0x00000001"));
       
    iRemotableApp->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathBtA2DpUnLaunched, KIconWidth2, KIconHeight2, KIconDepth1,KTestSvgMimeType)); 
    iRemotableApp->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathBtA2DpLaunched,KIconWidth3, KIconHeight3, KIconDepth1,KTestSvgMimeType));
    iRemotableApp->SetResourceStatusL(_L8("free"));

    // Register the application 
    iTmServer->RegisterAppL(iRemotableApp);
    
    iRemotableApp1 = CUpnpRemotableApp::NewL( KAppId2Value ,_L8("Bluetooth A2DP"));
    iRemotableApp1->SetAppDescriptionL(KAppDesc());
          
    CUpnpTmInfoElement& tmInfoElement2 = iRemotableApp1->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement2.AddTmInfoElementL(_L8("protocolID"),_L8("BTA2DP"));
    tmInfoElement2.AddTmInfoElementL(_L8("direction"),_L8("out"));
          
    CUpnpTmInfoElement& tmInfoElement3 = iRemotableApp1->CreateTmInfoElementL(CUpnpTmInfoElement::EAppInfo);
    tmInfoElement3.AddTmInfoElementL(_L8("appCategory"),_L8("0x0000003"));
    tmInfoElement3.AddTmInfoElementL(_L8("trustLevel"),_L8("0x00000001"));
          
    iRemotableApp1->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathBtA2DpUnLaunched, KIconWidth2, KIconHeight2, KIconDepth1,KTestSvgMimeType)); 
    iRemotableApp1->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathBtA2DpLaunched,KIconWidth3, KIconHeight3, KIconDepth1,KTestSvgMimeType));
    iRemotableApp1->SetResourceStatusL(_L8("free"));

    // Register the application which is already registered
    iTmServer->RegisterAppL(iRemotableApp1);     
    
    _LIT( KDescription , "Registering of duplicate app is handled successfully");
    aResult.SetResult( KErrNone, KDescription );
    iLog->Log( KDescription );
    
    delete iTmServer;
    iTmServer = NULL;
    
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::RegisterDuplicateAppsL(  TTestResult& aResult  )
    {   
    _LIT( KLogInfo, "Register Duplicate Applications" );
    iLog->Log( KLogInfo );

    //Sets the information associated with the terminal mode server device    
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
       
    _LIT8(KAppDesc, "Mobile Navigation Application");
    iRemotableApp = CUpnpRemotableApp::NewL( KAppIdValue ,_L8("Navigation"));  
    iRemotableApp->SetAppDescriptionL(KAppDesc());
    CUpnpTmInfoElement& tmInfoElement = iRemotableApp->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement.AddTmInfoElementL(_L8("protocolID"),_L8("VNC"));
    
    CUpnpTmInfoElement& tmInfoElement1 = iRemotableApp->CreateTmInfoElementL(CUpnpTmInfoElement::EAppInfo);
    tmInfoElement1.AddTmInfoElementL(_L8("appCategory"),_L8("0x00010001"));
    tmInfoElement1.AddTmInfoElementL(_L8("trustLevel"),_L8("0x80"));
    
    CUpnpTmInfoElement& tmInfoElement2 = iRemotableApp->CreateTmInfoElementL(CUpnpTmInfoElement::EDisplayInfo);
    tmInfoElement2.AddTmInfoElementL(_L8("contentCategory"),_L8("0x0000001"));
    tmInfoElement2.AddTmInfoElementL(_L8("contentRules"),_L8("0x00000FE"));
    tmInfoElement2.AddTmInfoElementL(_L8("trustLevel"),_L8("0x00000001"));
    iRemotableApp->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathMaps, KIconWidth1, KIconHeight1, KIconDepth,KTestSvgMimeType));
      
    // Register the application 
    iTmServer->RegisterAppL(iRemotableApp);
    
    _LIT8(KAppDesc1, "RTP Audio Server");
    iRemotableApp1 = CUpnpRemotableApp::NewL( KAppId1Value ,_L8("RTP Server"));
    iRemotableApp1->SetAppDescriptionL(KAppDesc1());
     
    CUpnpTmInfoElement& tmInfoElement3 = iRemotableApp1->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement3.AddTmInfoElementL(_L8("protocolID"),_L8("RTP"));
    tmInfoElement3.AddTmInfoElementL(_L8("format"),_L8("99"));
    tmInfoElement3.AddTmInfoElementL(_L8("direction"),_L8("out"));
       
    CUpnpTmInfoElement& tmInfoElement4 = iRemotableApp1->CreateTmInfoElementL(CUpnpTmInfoElement::EAppInfo);
    tmInfoElement4.AddTmInfoElementL(_L8("appCategory"),_L8("0x0000002"));
    tmInfoElement4.AddTmInfoElementL(_L8("trustLevel"),_L8("0x00000001"));
       
    CUpnpTmInfoElement& tmInfoElement5 = iRemotableApp1->CreateTmInfoElementL(CUpnpTmInfoElement::EAudioInfo);
    tmInfoElement5.AddTmInfoElementL(_L8("audioType"),_L8("all"));    
    iRemotableApp1->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathMusicUnLaunched,KIconWidth, KIconHeight, KIconDepth,KTestSvgMimeType));
    iRemotableApp1->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathMusicLaunched,KIconWidth, KIconHeight, KIconDepth,KTestSvgMimeType));
    iRemotableApp1->SetResourceStatusL(_L8("free"));
     
    iRemotableAppArray.AppendL(iRemotableApp1);     
   
    iRemotableApp2 = CUpnpRemotableApp::NewL( KAppId1Value ,_L8("RTP Server"));
    iRemotableApp2->SetAppDescriptionL(KAppDesc1());
        
    CUpnpTmInfoElement& tmInfoElement6 = iRemotableApp2->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement6.AddTmInfoElementL(_L8("protocolID"),_L8("RTP"));
    tmInfoElement6.AddTmInfoElementL(_L8("format"),_L8("99"));
    tmInfoElement6.AddTmInfoElementL(_L8("direction"),_L8("out"));
          
    CUpnpTmInfoElement& tmInfoElement7 = iRemotableApp2->CreateTmInfoElementL(CUpnpTmInfoElement::EAppInfo);
    tmInfoElement7.AddTmInfoElementL(_L8("appCategory"),_L8("0x0000002"));
    tmInfoElement7.AddTmInfoElementL(_L8("trustLevel"),_L8("0x00000001"));
          
    CUpnpTmInfoElement& tmInfoElement8 = iRemotableApp2->CreateTmInfoElementL(CUpnpTmInfoElement::EAudioInfo);
    tmInfoElement8.AddTmInfoElementL(_L8("audioType"),_L8("all"));    
    iRemotableApp2->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathMusicUnLaunched,KIconWidth, KIconHeight, KIconDepth,KTestSvgMimeType));
    iRemotableApp2->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathMusicLaunched,KIconWidth, KIconHeight, KIconDepth,KTestSvgMimeType));
    iRemotableApp2->SetResourceStatusL(_L8("free"));
    iRemotableAppArray.AppendL(iRemotableApp2);     

    // Register the dulplicate applications
    iTmServer->RegisterAppsL( iRemotableAppArray);     
    
    _LIT( KDescription , "Registering of duplicate applications is handled successfully");
    aResult.SetResult( KErrNone, KDescription );
    iLog->Log( KDescription );
    iRemotableAppArray.Close();
    delete iTmServer;
    iTmServer = NULL;
    
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::UnRegisterAppL(  TTestResult& aResult  )
    {
    _LIT( KLogInfo, "UnRegister Application" );
    iLog->Log( KLogInfo );

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);   
    
    //Unregister the application by specifying AppId 
    iTmServer->UnRegisterApp( KAppIdValue );
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();
    
    _LIT( KDescription , "Application is un-registered successfully");
    aResult.SetResult( KErrNone, KDescription );
    iLog->Log( KDescription );
    
    delete iTestTimer;
    iTestTimer = NULL;
    
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::UnRegisterAppsL(  TTestResult& aResult  )
    {
    _LIT( KLogInfo, "UnRegister Multiple Applications" );
    iLog->Log( KLogInfo );
    
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
   
    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);
    
    RArray<TUint> appIdArray;
    appIdArray.AppendL( KAppId1Value );
    appIdArray.AppendL( KAppId2Value );

    //Unregister multiple applications by specifying array of AppIds  
    iTmServer->UnRegisterApps( appIdArray); 

    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();
    
    _LIT( KDescription , "Multiple Applications are un-registered successfully");
    aResult.SetResult( KErrNone, KDescription );
    iLog->Log( KDescription );
    appIdArray.Close();
    
    delete iTestTimer;
    iTestTimer = NULL;     
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::UnRegisterUnRegisteredAppL(  TTestResult& aResult  )
    {
    _LIT( KLogInfo, "UnRegister UnRegistered Application" );
    iLog->Log( KLogInfo );
    
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);
    
    //Unregister the application which is already unregistered
    iTmServer->UnRegisterApp( KNonExistentAppIdValue );
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();
    
    _LIT( KDescription , "UnRegistering of UnRegistered Application is handled successfully");
    aResult.SetResult( KErrNone, KDescription );
    iLog->Log( KDescription );
    
    delete iTestTimer;
    iTestTimer = NULL;
    
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::UnRegisterUnRegisteredAppsL(  TTestResult& aResult  )
    {
    _LIT( KLogInfo, "UnRegister Multiple UnRegistered Applications" );
    iLog->Log( KLogInfo );
    
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);
    
    RArray<TUint> appIdArray;
    appIdArray.AppendL( KNonExistentAppIdValue );
    appIdArray.AppendL( KNonExistentAppId1Value );

    //Unregister multiple applications which are already unregistered
    iTmServer->UnRegisterApps( appIdArray);
        
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();
    
    _LIT( KDescription , "UnRegistering of UnRegistered Applications is handled successfully");
    aResult.SetResult( KErrNone, KDescription );
    iLog->Log( KDescription );
    
    appIdArray.Close();
    delete iTestTimer;
    iTestTimer = NULL;
    
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleEventDuplicateAppStatusUpdateL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Event For Duplicate AppStatus Update" );
    iLog->Log( KLogInfo ); 
          
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);
       
    //Start the tmserver device and its services              
    iTmServer ->StartL();
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);          
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start();   
    	         
    if ( iDeviceVerified  )
         { 
             //Subscribe to Services
           iSearchDevice->SubscribeToServiceL();         
          }    
    RArray<TUint> updatedAppIdList;
    updatedAppIdList.Append(KAppId1Value);
    updatedAppIdList.Append(KAppId1Value);
    //Update the status of applications which have undergone some change
    iTmServer->UpdateAppStatusL(updatedAppIdList); 
       
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();
       
    if ( iStatusNotified )
         {
          _LIT( KDescription , "Handle Event For Duplicate AppStatus Update handled successfully");
          aResult.SetResult( KErrNone, KDescription );
          iLog->Log( KDescription );
         }
    else
         {
          aResult.iResult = KErrGeneral;
          _LIT( KDescription , "Handle Event For Duplicate AppStatus Update handling Failed");
          aResult.SetResult( KErrNone, KDescription );
          iLog->Log( KDescription );
         }
    updatedAppIdList.Close();  
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
  }

TInt CTmServerTest::HandleEventProfileIdListStatusUpdateL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Event For ProfileIdList Status Update" );
    iLog->Log( KLogInfo ); 
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);
    
    //Start the tmserver device and its services
    iTmServer ->StartL();
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start();  
    
    if ( iDeviceVerified  )
        {
        //Subscribes to a service 
        iSearchDevice->SubscribeToServiceL();
        }  

    RArray<TUint> UnusedProfileIdList;
    UnusedProfileIdList.Append(5);
    UnusedProfileIdList.Append(6);
    //Update the list of profile IDs for profiles which are not used
    iTmServer->UpdateUnusedProfileIdsL(UnusedProfileIdList);     
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();

    if ( iStatusNotified )
        {
        _LIT( KDescription , "Handle Event For ProfileIdList Status Update handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "Handle Event For ProfileIdList Status Update handling failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    
       UnusedProfileIdList.Close();
       delete iDiscoveryTestTimer;
       iDiscoveryTestTimer = NULL;
       delete iTestTimer;
       iTestTimer = NULL;
       REComSession::FinalClose();
       return KErrNone;
   }

TInt CTmServerTest::HandleLaunchAppActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo , "Handle LaunchApp Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
   
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);

    //Start the tmserver device and its services
    iTmServer ->StartL();
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    	
    if ( iDeviceVerified  )
        {
         //Launch the application remotely by specifying appId and profileId 
         iSearchDevice->LaunchAppActionL(KAppId1Value,KProfileIdValue);
        }   
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  

    if (iStartApp )
        {
        _LIT( KDescription , "LaunchApp Command handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "LaunchApp Command handled  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleTerminateAppActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle TerminateApp Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);

    //Start the tmserver device and its services
    iTmServer->StartL();

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    
    if ( iDeviceVerified )
        {
        //Terminate the application which is already running
        iSearchDevice->TerminateAppActionL( KAppId1Value,KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if ( iStopApp)
        {
        _LIT( KDescription , "TerminateApp Command handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "TerminateApp Command handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleInvalidAppIdLaunchAppActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Invalid AppId LaunchApp Command" );
    iLog->Log( KLogInfo );    
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer); 
    
    //Start the tmserver device and its services
    iTmServer->StartL();
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    
    if ( iDeviceVerified  )
        {
        //Launch the application by specifying invalid appId
        iSearchDevice->LaunchAppActionL( KInvalidAppIdValue,KProfileIdValue);
        }   
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  

    if (iStartApp )
        {
        _LIT( KDescription , "LaunchApp Command with invalid AppId handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "LaunchApp Command with invalid AppId handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleInvalidProfileIdLaunchAppActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Invalid ProfileId LaunchApp Command" );
    iLog->Log( KLogInfo );    
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);   
    //Start the tmserver device and its services
    iTmServer->StartL();

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    
    if ( iDeviceVerified  )
        {
        //Launch the application by specifying invalid profileId
        iSearchDevice->LaunchAppActionL( KAppId1Value,KInvalidProfileIdValue);
        }   
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    if (iStartApp )
        {
        _LIT( KDescription , "LaunchApp Command with invalid ProfileId handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "LaunchApp Command with invalid ProfileId handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleNonExistentLaunchAppActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle LaunchApp Command For Non-Existent AppID" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);   

    //Start the tmserver device and its services
    iTmServer->StartL();  
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
        //Launch the application by specifying non-existent appId
        iSearchDevice->LaunchAppActionL( KNonExistentAppIdValue, KProfileIdValue );
        }   
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  

    if (iStartApp )
        {
        _LIT( KDescription , "LaunchApp Command For Non-Existent AppID handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "LaunchApp Command For Non-Existent AppID handled  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleUnauthorizedLaunchAppActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle LaunchApp Command For UnAuthorized AppID" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);
    
    //Start the tmserver device and its services
    iTmServer ->StartL();

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Launch the application by specifying unauthorized appId
         iSearchDevice->LaunchAppActionL(KAppId5Value,KProfileIdValue);
        }   
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    if (iStartApp )
        {
       _LIT( KDescription , "LaunchApp Command For UnAuthorized AppID handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "LaunchApp Command For UnAuthorized AppID handled  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleDeviceLockedLaunchAppActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Device Locked Condition For LaunchApp Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);
    
    //Start the tmserver device and its services
    iTmServer ->StartL();

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Launch the application
         iSearchDevice->LaunchAppActionL(KAppId4Value,KProfileIdValue);
        }   
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    if (iStartApp )
        {
       _LIT( KDescription , "LaunchApp Command For Device Locked Condition handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "LaunchApp Command For Device Locked Condition handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleNonExistentTerminateAppActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle TerminateApp Command For Non-Existent AppID" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);   
    
    //Start the tmserver device and its services
    iTmServer->StartL();    

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    
    if ( iDeviceVerified )
        {
         //Terminate the application by specifying unauthorized appId
         iSearchDevice->TerminateAppActionL( KNonExistentAppIdValue,KProfileIdValue );
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if ( iStopApp)
        {
        _LIT( KDescription , "TerminateApp Command For Non-Existent App ID handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "TerminateApp  Command handling for Non-Existent App Id Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleInvalidAppIdTerminateAppActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Invalid AppId TerminateApp Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);  
    
    //Start the tmserver device and its services
    iTmServer->StartL();    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    
    if ( iDeviceVerified )
        {
         //Terminate the application by specifying invalid appId
         iSearchDevice->TerminateAppActionL(  KInvalidAppIdValue,KProfileIdValue );
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if ( iStopApp)
        {
        _LIT( KDescription , "TerminateApp Command with invalid AppId handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "TerminateApp Command with invalid AppId handling  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleInvalidProfileIdTerminateAppActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Invalid ProfileId TerminateApp Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);   
    
    //Start the tmserver device and its services
    iTmServer->StartL();    

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    
    if ( iDeviceVerified )
        {
         //Terminate the application by specifying invalid profileId
         iSearchDevice->TerminateAppActionL(  KAppId1Value,KInvalidProfileIdValue );
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if ( iStopApp)
        {
        _LIT( KDescription , "TerminateApp Command with invalid ProfileId handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "TerminateApp Command with invalid ProfileId handling  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleUnauthorizedTerminateAppActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle TerminateApp Command UnAuthorized AppID" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);    
    
    //Start the tmserver device and its services
    iTmServer->StartL();    

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    
    if ( iDeviceVerified )
        {
         //Terminate the application by specifying unauthorized appId
         iSearchDevice->TerminateAppActionL( KAppId5Value,KProfileIdValue );
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();     
    if ( iStopApp)
        {
        _LIT( KDescription , "TerminateApp Command For Un-Authorized App ID handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "TerminateApp  Command handling For Un-Authorized App ID Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleDeviceLockedTerminateAppActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Device Locked Condition For TerminateApp Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);
    
    //Start the tmserver device and its services
    iTmServer ->StartL();

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Terminate the application
         iSearchDevice->TerminateAppActionL(KAppId4Value,KProfileIdValue);
        }   
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    if (iStopApp )
        {
       _LIT( KDescription , "TerminateApp Command For Device Locked Condition handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "TerminateApp Command For Device Locked Condition handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppStatusActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppStatus Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);     

    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Retrieve the status of application
         iSearchDevice->GetAppStatusActionL( KAppId1Value );
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  

    if (iGetAppStatus)
        {
        _LIT( KDescription , "GetAppStatus Command  handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppStatus Command handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleWildcardGetAppStatusActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppStatus Command For All the Applications" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
    
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer); 
    
    //Start the tmserver device and its services
    iTmServer->StartL();   

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Retrieve the status of all the application
         iSearchDevice->GetAppStatusActionL( KAppId5Value );
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  

    if (iGetAppStatus)
        {
        _LIT( KDescription , "GetAppStatus Command  handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppStatus Command handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }


TInt CTmServerTest::HandleInvalidAppIdGetAppStatusActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppStatus Command For Invalid AppId" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
    
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);    
    
    //Start the tmserver device and its services
    iTmServer->StartL();
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        { 
         //Retrieve the status of the application by specifying invalid appId
         iSearchDevice->GetAppStatusActionL( KInvalidAppIdValue );
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  

    if (iGetAppStatus)
        {
        _LIT( KDescription , "GetAppStatus Command with invalid AppId  handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppStatus Command with invalid AppId handling  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleNonExistentGetAppStatusActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppStatus Command For Non-Existent AppID" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
    
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);  
    
    //Start the tmserver device and its services
    iTmServer->StartL();  
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Retrieve the status of the application by specifying non existent appId
         iSearchDevice->GetAppStatusActionL( KNonExistentAppIdValue );
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  

    if (iGetAppStatus)
        {
        _LIT( KDescription , "GetAppStatus Command for Non-Existent App ID handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppStatus Command handling for Non-Existent App ID Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleEventAppStatusUpdateL( TTestResult& aResult )
 {
    _LIT( KLogInfo, "Handle Event For AppStatus Update" );
    iLog->Log( KLogInfo ); 
       
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);
    
    //Start the tmserver device and its services              
    iTmServer ->StartL();

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    	       
    if ( iDeviceVerified  )
         {
          //Subscribes to a service 
          iSearchDevice->SubscribeToServiceL();
         }
    RArray<TUint> updatedAppIdList;
    updatedAppIdList.Append(KAppId1Value);
    updatedAppIdList.Append(KAppId2Value);
    
    //Update the status of applications which have undergone some change
    iTmServer->UpdateAppStatusL(updatedAppIdList); 
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();
    
    if ( iStatusNotified )
         {
          _LIT( KDescription , "Handle Event For AppStatus Update handled successfully");
          aResult.SetResult( KErrNone, KDescription );
          iLog->Log( KDescription );
         }
    else
         {
          aResult.iResult = KErrGeneral;
          _LIT( KDescription , "Handle Event For AppStatus Update handling Failed");
          aResult.SetResult( KErrNone, KDescription );
          iLog->Log( KDescription );
         }
    updatedAppIdList.Close();
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
  }

TInt CTmServerTest::HandleEventAppListUpdateL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Event For AppList Update" );
    iLog->Log( KLogInfo ); 
       
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications 
    RegisterApplicationsL(*iTmServer);
    
    //Start the tmserver device and its services              
    iTmServer ->StartL();

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);          
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start();  
          
    if ( iDeviceVerified  )
         {
          //Subscribes to a service 
          iSearchDevice->SubscribeToServiceL();        
         }  
      
    TInt errorCode;
    CUpnpRemotableApp& aRemotableApp = iTmServer->GetRemotableApp(KAppIdValue ,errorCode );
    if (errorCode == KErrNone)
        { 
         _LIT8(KAppDesc, "Mobile Device Display");
         aRemotableApp.SetAppDescriptionL(KAppDesc());
        }
    RArray<TUint> updatedAppIdList;
    updatedAppIdList.Append(KAppIdValue);
    updatedAppIdList.Append(KAppId2Value);
   
    //Update the list of applications which have been modified to/from the list
    iTmServer->UpdateAppListL(updatedAppIdList);      
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();
    
    if ( iStatusNotified )
         {
          _LIT( KDescription , "Handle Event For AppList Update handled successfully");
          aResult.SetResult( KErrNone, KDescription );
          iLog->Log( KDescription );
         }
    else
         {
          aResult.iResult = KErrGeneral;
          _LIT( KDescription , "Handle Event For AppList Update handling Failed");
          aResult.SetResult( KErrNone, KDescription );
          iLog->Log( KDescription );
         }
    updatedAppIdList.Close();
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
  }

TInt CTmServerTest::HandleGetAppListActionFilter1L ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList Command with Filter1" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
    
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);  
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
          //Retrieve the list of applications by specifying filter value
          iSearchDevice->GetAppListActionL(KAppListFilterValue1(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with Filter1 handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with Filter1 handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionFilter2L ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList with Filter2 Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer); 
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Retrieve the list of applications by specifying filter value
         iSearchDevice->GetAppListActionL(KAppListFilterValue2(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with Filter2 handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with Filter2 handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionFilter3L ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList with Filter3 Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);  
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Retrieve the list of applications by specifying filter value 
         iSearchDevice->GetAppListActionL(KAppListFilterValue3(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with Filter3 handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with Filter3 handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionFilter4L ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList with Filter4 Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);  
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Retrieve the list of applications by specifying filter value 
         iSearchDevice->GetAppListActionL(KAppListFilterValue4(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with Filter4 handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with Filter4 handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionInvalidArg1L ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList Command with Invalid Arg1" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);    
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
          _LIT8(KInvalidFilter1,"\"name=\"*Audio*\"" );
         //Retrieve the list of applications by specifying invalid filter value
         iSearchDevice->GetAppListActionL(KInvalidFilter1(),KProfileIdValue);  
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with Invalid Arg1 handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with Invalid Arg1 handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }


TInt CTmServerTest::HandleGetAppListActionInvalidArg2L ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList Command with Invalid Arg2" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);  
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         _LIT8(KInvalidFilter2, "\"name=\"*nav*\",appInfo=\"*\"\""); 
         //Retrieve the list of applications by specifying invalid filter value
         iSearchDevice->GetAppListActionL(KInvalidFilter2(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with Invalid Arg2 handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with Invalid Arg2 handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionInvalidArg3L ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList Command with Invalid Arg3" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);   
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         _LIT8(KInvalidFilter3, "\"name=\"*Audio*\",@audioType=\"*\"\"");
         //Retrieve the list of applications by specifying invalid filter value
         iSearchDevice->GetAppListActionL(KInvalidFilter3(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with Invalid Arg3 handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with Invalid Arg3 handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionInvalidArg4L ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList Command with Invalid Arg4" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);   
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         _LIT8(KInvalidFilter4, "\"icon@mimetype\"*svg+xml*\"\"");
         //Retrieve the list of applications by specifying invalid filter value
         iSearchDevice->GetAppListActionL(KInvalidFilter4(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with Invalid Arg4 handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with Invalid Arg4 handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionInvalidArg5L ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList Command with Invalid Arg5" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;
    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);   
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         _LIT8(KInvalidFilter5, "\"name=\"*nav*\"appInfo@appCategory=\"*\"\"");
         //Retrieve the list of applications by specifying invalid filter value
         iSearchDevice->GetAppListActionL(KInvalidFilter5(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with Invalid Arg5 handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with Invalid Arg5 handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionWildCardFilterL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList Command With WildCard Filter" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);    
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Retrieve the list of applications
         iSearchDevice->GetAppListActionL( KWildCard(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command With WildCard Filter handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command With WildCard Filter handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionNoFilterL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetAppList Command With No Filter" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);    
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         _LIT8(KNoFilter, " ");
         //Retrieve the list of applications
         iSearchDevice->GetAppListActionL( KNoFilter(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command With No Filter handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command With No Filter handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }   

TInt CTmServerTest::HandleInvalidProfileIdGetAppListActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Invalid ProfileId GetAppList Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this );
    
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer); 
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    
    if ( iDeviceVerified  )
        {
         //Retrieve the list of applications by specifying invalid profileId
         iSearchDevice->GetAppListActionL(KWildCard(),KInvalidProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "GetAppList Command with invalid ProfileId handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetAppList Command with invalid ProfileId  handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetAppListActionOperationRejectedL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Operation Rejected Condition For GetAppList Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this );
    
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer); 
    
    //Start the tmserver device and its services
    iTmServer->StartL();   
    iOperationRejected = ETrue; 
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 
    
    if ( iDeviceVerified  )
        {
         //Retrieve the list of applications by specifying profileId
         iSearchDevice->GetAppListActionL(KAppListFilterValue1(),KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();      
    if (iGetAppList)
        {
        _LIT( KDescription , "Handle Operation Rejected Condition For GetAppList Command handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "Handle Operation Rejected Condition For GetAppList Command handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleSetClientProfileActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle SetClientProfile Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer); 
    
    //Start the tmserver device and its services
    iTmServer->StartL(); 
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         // Register the client profile 
         iSearchDevice->SetClientProfileActionL(KProfileIdValue, KClientProfileInfo);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if (iSetClientProfile)
        {
        _LIT( KDescription , "SetClientProfile Command handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "SetClientProfile Command handling  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    iClientIconPrefArray.ResetAndDestroy();
    iClientIconPrefArray.Close();
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleInvalidProfileIdSetClientProfileActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Invalid ProfileId SetClientProfile Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);    
    
    //Start the tmserver device and its services
    iTmServer->StartL();   

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Register the client profile with invalid profileId
         iSearchDevice->SetClientProfileActionL(KInvalidProfileIdValue, KClientProfileInfo );
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if (iSetClientProfile)
        {
        _LIT( KDescription , "SetClientProfile Command with invalid ProfileId handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "SetClientProfile Command with invalid ProfileId handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleInvalidProfileSetClientProfileActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Invalid Profile SetClientProfile Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
    
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer); 
    
    //Start the tmserver device and its services
    iTmServer->StartL();
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
        _LIT8(KInvalidProfile,"&lt;clientProfile");
        //Register the client profile with invalid profile
        iSearchDevice->SetClientProfileActionL(KProfileIdValue, KInvalidProfile());
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if (iSetClientProfile)
        {
        _LIT( KDescription , "SetClientProfile Command with invalid Profile handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "SetClientProfile Command with invalid Profile handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleDeviceLockedSetClientProfileActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Device Locked Condition For SetClientProfile Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
    
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer); 
    
    //Start the tmserver device and its services
    iTmServer->StartL();
    
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
        //Register the client profile 
        iSearchDevice->SetClientProfileActionL(KProfileIdValue, KDeviceLockedClientProfile());
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if (iSetClientProfile)
        {
        _LIT( KDescription , "Device Locked Condition for SetClientProfile Command handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "Device Locked Condition for SetClientProfile Command handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleResourceBusySetClientProfileActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Resource Busy Condition For SetClientProfile Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 
    
    //Register all the desired applications
    RegisterApplicationsL(*iTmServer); 
    
    //Start the tmserver device and its services
    iTmServer->StartL();
    iResourceBusy = ETrue;
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
        //Register the client profile 
        iSearchDevice->SetClientProfileActionL(KProfileIdValue, KClientProfileInfo());
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if (iSetClientProfile)
        {
        _LIT( KDescription , "Resource Busy Condition for SetClientProfile Command handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "Resource Busy Condition for SetClientProfile Command handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetClientProfileActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetClientProfile Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);     

    //Start the tmserver device and its services
    iTmServer->StartL();   

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Retrieve the contents of a client profile stored in the terminal mode device
         iSearchDevice->GetClientProfileActionL(KProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if (iGetClientProfile)
        {
        _LIT( KDescription , "GetClientProfile Command handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetClientProfile Command handling  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleInvalidProfileIdGetClientProfileActionL ( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Invalid ProfileId GetClientProfile Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);  
    
    //Start the control point and search for device
    iTmServer->StartL();   

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Retrieve the contents of a client profile by specifying invalid profileId
         iSearchDevice->GetClientProfileActionL(KInvalidProfileIdValue);
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if (iGetClientProfile)
        {
        _LIT( KDescription , "GetClientProfile Command with invalid ProfileId handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetClientProfile Command with invalid ProfileId handling Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }


TInt CTmServerTest::HandleGetMaxNumProfilesActionL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle GetMaxNumProfiles Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);  
    
    //Start the control point and search for device
    iTmServer->StartL();   

    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Get the maximum number of client profiles supported
         iSearchDevice->GetMaxNumProfilesActionL();
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if (iGetMaxNumProfiles)
        {
        _LIT( KDescription , "GetMaxNumProfiles Command handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "GetMaxNumProfiles Command handling  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

TInt CTmServerTest::HandleGetMaxNumProfilesOperationRejectedL( TTestResult& aResult )
    {
    _LIT( KLogInfo, "Handle Operation Rejected Condition For GetMaxNumProfiles Command" );
    iLog->Log( KLogInfo );
    
    iTmServerDeviceType = ETrue;

    //Sets the information associated with the terminal mode server device
    SetTmServerSettingsL();
    iTmServer = CUpnpTmServer::NewL( *iTmServerDeviceInfo, *this ); 

    //Register all the desired applications
    RegisterApplicationsL(*iTmServer);  
    
    //Start the control point and search for device
    iTmServer->StartL();   
    iOperationRejected = ETrue; 
    //Start the control point and search for device
    iSearchDevice = CDiscoverDevice::NewL(*this);    
    iDiscoveryTestTimer = CDiscoveryTestTimer::NewL(*this);
    iDiscoveryTestTimer->AfterDiscovery(15);
    CActiveScheduler::Start(); 

    if ( iDeviceVerified  )
        {
         //Get the maximum number of client profiles supported
         iSearchDevice->GetMaxNumProfilesActionL();
        }
    
    iTestTimer = CTestTimer::NewL(*this);
    iTestTimer->After(15);
    CActiveScheduler::Start();  
    
    if (iGetMaxNumProfiles)
        {
        _LIT( KDescription , "Operation Rejected Condition For GetMaxNumProfiles Command handled successfully");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    else
        {
        aResult.iResult = KErrGeneral;
        _LIT( KDescription , "Operation Rejected Condition For GetMaxNumProfiles Command handling  Failed");
        aResult.SetResult( KErrNone, KDescription );
        iLog->Log( KDescription );
        }
    delete iDiscoveryTestTimer;
    iDiscoveryTestTimer = NULL;
    delete iTestTimer;
    iTestTimer = NULL;
    REComSession::FinalClose();
    return KErrNone;
    }

// ========================================================================================================

void CTmServerTest::SetTmServerSettingsL()
    { 
    iTmServerDeviceInfo = CUpnpTmServerDeviceInfo::NewL(CUpnpSettings::GetIapL());  
    iTmServerDeviceInfo->SetDeviceInfoL( KDeviceInfo );
    iTmServerDeviceInfo->SetBtAddressL( KBtMacValue );
    iTmServerDeviceInfo->SetStartConn(ETrue);
    iTmServerDeviceInfo->AddDeviceIconL(CUpnpTerminalModeIcon::NewL(KIconDevicePath, KIconWidth, KIconHeight, KIconDepth,KTestSvgMimeType));
    }

void CTmServerTest::RegisterApplicationsL(CUpnpTmServer& aTmServer )
    {  
    RArray<TUint> allowedProfileIdList;
    allowedProfileIdList.Append(0);
    allowedProfileIdList.Append(1);
    allowedProfileIdList.Append(2);
    allowedProfileIdList.Append(2);
    allowedProfileIdList.Append(3);
    allowedProfileIdList.Append(4);
    allowedProfileIdList.Append(5);
    allowedProfileIdList.Append(6);

    _LIT8(KAppDesc, "Mobile Navigation Application");
    iRemotableApp = CUpnpRemotableApp::NewL( KAppIdValue ,_L8("Navigation"));  
    iRemotableApp->SetAppDescriptionL(KAppDesc());
    iRemotableApp->SetAllowedProfileIdListL(allowedProfileIdList);   
    CUpnpTmInfoElement& tmInfoElement = iRemotableApp->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement.AddTmInfoElementL(_L8("protocolID"),_L8("VNC"));
    
    CUpnpTmInfoElement& tmInfoElement1 = iRemotableApp->CreateTmInfoElementL(CUpnpTmInfoElement::EAppInfo);
    tmInfoElement1.AddTmInfoElementL(_L8("appCategory"),_L8("0x00010001"));
    tmInfoElement1.AddTmInfoElementL(_L8("trustLevel"),_L8("0x80"));
    
    CUpnpTmInfoElement& tmInfoElement2 = iRemotableApp->CreateTmInfoElementL(CUpnpTmInfoElement::EDisplayInfo);
    tmInfoElement2.AddTmInfoElementL(_L8("contentCategory"),_L8("0x0000001"));
    tmInfoElement2.AddTmInfoElementL(_L8("contentRules"),_L8("0x00000FE"));
    tmInfoElement2.AddTmInfoElementL(_L8("trustLevel"),_L8("0x00000001"));
    iRemotableApp->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathMaps, KIconWidth1, KIconHeight1, KIconDepth,KTestSvgMimeType));
    aTmServer.RegisterAppL(iRemotableApp);   
    allowedProfileIdList.Close();
   
    _LIT8(KAppDesc1, "RTP Audio Server");
    iRemotableApp1 = CUpnpRemotableApp::NewL( KAppId1Value ,_L8("RTP Server"));
    iRemotableApp1->SetAppDescriptionL(KAppDesc1());
    
    CUpnpTmInfoElement& tmInfoElement3 = iRemotableApp1->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement3.AddTmInfoElementL(_L8("protocolID"),_L8("RTP"));
    tmInfoElement3.AddTmInfoElementL(_L8("format"),_L8("99"));
    tmInfoElement3.AddTmInfoElementL(_L8("direction"),_L8("out"));
    
    CUpnpTmInfoElement& tmInfoElement4 = iRemotableApp1->CreateTmInfoElementL(CUpnpTmInfoElement::EAppInfo);
    tmInfoElement4.AddTmInfoElementL(_L8("appCategory"),_L8("0x0000002"));
    tmInfoElement4.AddTmInfoElementL(_L8("trustLevel"),_L8("0x00000001"));
    
    CUpnpTmInfoElement& tmInfoElement5 = iRemotableApp1->CreateTmInfoElementL(CUpnpTmInfoElement::EAudioInfo);
    tmInfoElement5.AddTmInfoElementL(_L8("audioType"),_L8("all"));    
    iRemotableApp1->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathMusicUnLaunched,KIconWidth, KIconHeight, KIconDepth,KTestSvgMimeType));
    iRemotableApp1->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathMusicLaunched,KIconWidth, KIconHeight, KIconDepth,KTestSvgMimeType));
    iRemotableApp1->SetResourceStatusL(_L8("free"));
   
    iRemotableAppArray.AppendL(iRemotableApp1);      

    _LIT8(KAppDesc2, "Bluetooth A2DP Audio Server");
    iRemotableApp2 = CUpnpRemotableApp::NewL( KAppId2Value ,_L8("Bluetooth A2DP"));
    iRemotableApp2->SetAppDescriptionL(KAppDesc2());
    
    CUpnpTmInfoElement& tmInfoElement6 = iRemotableApp2->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement6.AddTmInfoElementL(_L8("protocolID"),_L8("BTA2DP"));
    tmInfoElement6.AddTmInfoElementL(_L8("direction"),_L8("out"));
    
    CUpnpTmInfoElement& tmInfoElement7 = iRemotableApp2->CreateTmInfoElementL(CUpnpTmInfoElement::EAppInfo);
    tmInfoElement7.AddTmInfoElementL(_L8("appCategory"),_L8("0x0000003"));
    tmInfoElement7.AddTmInfoElementL(_L8("trustLevel"),_L8("0x00000001"));
    
    iRemotableApp2->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathBtA2DpUnLaunched, KIconWidth2, KIconHeight2, KIconDepth1,KTestSvgMimeType)); 
    iRemotableApp2->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathBtA2DpLaunched,KIconWidth3, KIconHeight3, KIconDepth1,KTestSvgMimeType));
    iRemotableApp2->SetResourceStatusL(_L8("free"));
    iRemotableAppArray.AppendL(iRemotableApp2);      
    
     _LIT8(KAppDesc3, "Bluetooth HFP Audio");
    RArray<TUint> allowedProfileIdList1;
    allowedProfileIdList1.Append(0);
    iRemotableApp3 = CUpnpRemotableApp::NewL( KAppId3Value,_L8("Bluetooth HFP"));
    iRemotableApp3->SetAppDescriptionL(KAppDesc3());
    iRemotableApp3->SetAllowedProfileIdListL(allowedProfileIdList1);
    
    CUpnpTmInfoElement& tmInfoElement8 = iRemotableApp3->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement8.AddTmInfoElementL(_L8("protocolID"),_L8("BTHFP"));
    tmInfoElement8.AddTmInfoElementL(_L8("direction"),_L8("bi"));
    
    CUpnpTmInfoElement& tmInfoElement9 = iRemotableApp3->CreateTmInfoElementL(CUpnpTmInfoElement::EAppInfo);
    tmInfoElement9.AddTmInfoElementL(_L8("appCategory"),_L8("0x0000004"));
    tmInfoElement9.AddTmInfoElementL(_L8("trustLevel"),_L8("0x00000001")); 
   
    iRemotableApp3->AddIconL(CUpnpTerminalModeIcon::NewL(KIconPathBtHfp, KIconWidth, KIconHeight, KIconDepth,KTestSvgMimeType)); 
    iRemotableApp3->SetResourceStatusL(_L8("busy"));    
    iRemotableAppArray.AppendL(iRemotableApp3);     
    allowedProfileIdList1.Close();
    
    _LIT8(KAppDesc4, "Device Attestation Protocol");
    iRemotableApp4 = CUpnpRemotableApp::NewL( KAppId4Value ,_L8("Device Attestation"));
    iRemotableApp4->SetAppDescriptionL(KAppDesc4());
       
    CUpnpTmInfoElement& tmInfoElement11 = iRemotableApp4->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement11.AddTmInfoElementL(_L8("protocolID"),_L8("DAP")); 
    iRemotableAppArray.AppendL(iRemotableApp4); 

    iRemotableApp5 = CUpnpRemotableApp::NewL( KAppId5Value ,_L8("Device Attestation"));
    iRemotableApp5->SetAppDescriptionL(KAppDesc4());

    RArray<TUint> allowedProfileIdList2;
    allowedProfileIdList2.Append(1);    
    allowedProfileIdList2.Append(1);
    iRemotableApp5->SetAllowedProfileIdListL(allowedProfileIdList2);
       
    CUpnpTmInfoElement& tmInfoElement12 = iRemotableApp5->CreateTmInfoElementL(CUpnpTmInfoElement::ERemotingInfo);
    tmInfoElement12.AddTmInfoElementL(_L8("protocolID"),_L8("DAP")); 
    iRemotableAppArray.AppendL(iRemotableApp5);   
    allowedProfileIdList2.Close();
    
    aTmServer.SetXmlSignatureL(KXmlSignature);    
    aTmServer.RegisterAppsL(iRemotableAppArray); 
    iRemotableAppArray.Close();
    }

void CTmServerTest::TestTimerTimedOutL()
    {
    iTmServer->StopL();
    delete iSearchDevice;
    iSearchDevice = NULL;
    delete iTmServer;
    iTmServer = NULL;
    delete iTmServerDeviceInfo;
    iTmServerDeviceInfo = NULL;   
    CActiveScheduler::Stop();
    }

void CTmServerTest::TestTimerDiscoveryTimedOut()
    {
    CActiveScheduler::Stop();
    }

void CTmServerTest::DeviceVerified()
    {
    iDeviceVerified = ETrue;
    }

void CTmServerTest::DeviceLostVerified()
    {
    iDeviceLostVerified = ETrue;
    }

void CTmServerTest::ServiceVerified()
    {
    iServiceVerified = ETrue;
    }

CDiscoverDevice* CDiscoverDevice::NewL(CTmServerTest& aTmServerTest)
    {
    CDiscoverDevice* self = new (ELeave) CDiscoverDevice(aTmServerTest);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CDiscoverDevice::CDiscoverDevice(CTmServerTest& aTmServerTest):
    iTmServerTest(aTmServerTest)
    {
    
    }

void CDiscoverDevice::ConstructL()
    {
    CDesC8ArrayFlat* targetDeviceTypes = new (ELeave) CDesC8ArrayFlat( 1 );
    CleanupStack::PushL( targetDeviceTypes ); 
    targetDeviceTypes->AppendL( KTmServerDevice() );    
    CUpnpControlPoint::ConstructL( *targetDeviceTypes );
    CleanupStack::Pop( targetDeviceTypes );
    targetDeviceTypes->Reset();
    delete targetDeviceTypes;
    targetDeviceTypes = NULL;

    //SSDP search for Terminalmode server devices
    if (iTmServerTest.iTmServerDeviceType)
         {
          SearchL( KTmServerDevice() );
          }
    else if (iTmServerTest.iTmServerServiceType)
          {
          SearchL( KTmServerService() );
          }   
     }

CDiscoverDevice::~CDiscoverDevice()
    {   

    }


void CDiscoverDevice::StateUpdatedL( CUpnpService* aService )
    {
    CUpnpDevice& device = aService->Device();
    if (aService->ServiceType().Match( KTmServerServiceType ) != KErrNotFound )
        {
        TInt err =  TmServerStateUpdated( device, aService );
        }
    else
        {
        User::Leave(KErrGeneral);
        }
    }

TInt CDiscoverDevice::TmServerStateUpdated( CUpnpDevice& aDevice,
                                                        CUpnpService*& aService )
    {
    CUpnpStateVariable* appStatusUpdate = aService->StateVariable( KArgTypeAppStatusUpdate );
    if( !appStatusUpdate )
        {
        iTmServerTest.iStatusNotified = EFalse;
        return KErrArgument;
        }
    else
        {
        const TDesC8& deviceUuid = aDevice.Uuid();
        const TDesC8& statusBuf = appStatusUpdate->Value(); 

        iTmServerTest.iStatusNotified = ETrue;
        }
    return KErrNone;
    }

void CDiscoverDevice::DeviceDiscoveredL( CUpnpDevice* aDevice )
    {
    if( iTmServerTest.iTmServerDeviceType)
        {
        if(aDevice->DeviceType().Compare(KTmServerDevice) == KErrNone )
            {
            iTmServerTest.DeviceVerified();   
            }
        }
    else if( iTmServerTest.iTmServerServiceType)
        {
        if(aDevice->ServiceDescriptionReceiveState()== CUpnpDevice::EAllServicesAdded)
            {
            iTmServerTest.ServiceVerified();
            }
        }
    }

void CDiscoverDevice::DeviceDisappearedL( CUpnpDevice* /*aDevice */)
    {
     iTmServerTest.DeviceLostVerified();
    }

void CDiscoverDevice::ActionResponseReceivedL( CUpnpAction* aAction )
    {
    CUpnpService& service = aAction->Service();
    if( service.ServiceType().Length() <= 0 )
        {
        User::Leave(KErrArgument);
        }
    if (aAction->Name().Compare(KGetApplicationList) == KErrNone)
        {
        aAction->ArgumentValue( KAppListing );
        iTmServerTest.iGetAppList = ETrue;
        }
    else if (aAction->Name().Compare(KLaunchApp) == KErrNone)
        {
        aAction->ArgumentValue( KAppLaunchStatus );
        iTmServerTest.iStartApp = ETrue;
        }
    else if (aAction->Name().Compare(KTerminateApp) == KErrNone)
        {
        aAction->ArgumentValue( KAppTerminateStatus );
        iTmServerTest.iStopApp = ETrue;
        }
    else if (aAction->Name().Compare(KGetAppStatus) == KErrNone)
        {
        aAction->ArgumentValue( KAppStatus );
        iTmServerTest.iGetAppStatus = ETrue;
        }
    else if (aAction->Name().Compare(KSetClientProfile) == KErrNone)
        {
        aAction->ArgumentValue( KResultProfile );
        iTmServerTest.iSetClientProfile = ETrue;
        }
    else if (aAction->Name().Compare(KGetClientProfile) == KErrNone)
        {
        aAction->ArgumentValue( KClientProfile );
        iTmServerTest.iGetClientProfile = ETrue;
        }
    else if (aAction->Name().Compare(KGetMaxNumProfiles) == KErrNone)
        {
        aAction->ArgumentValue( KNumProfilesAllowed );
        iTmServerTest.iGetMaxNumProfiles = ETrue;
        }
    }

 void CDiscoverDevice::HttpResponseReceivedL( CUpnpHttpMessage* /*aMessage*/ )
    {
    
    }


TInt CDiscoverDevice::GetAppListActionL( const TDesC8& aAppListFilter ,TInt aProfileId )
    {
    const RPointerArray<CUpnpDevice>& deviceList = DeviceList();
    if( deviceList.Count() == KErrNone ) 
        {
        return KErrBadHandle;
        }
    TInt i(0);
    while ( deviceList[i]->DeviceType().Compare(KTmServerDevice()) != KErrNone )
        {
        i++;
        }
    if( i == deviceList.Count()) 
        {
        return KErrNotFound;
        }
    RPointerArray<CUpnpService>& services = const_cast<CUpnpDevice*>(deviceList[i])->ServiceList();
    
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( KTmServerServiceType ) != KErrNotFound )
            {
            CUpnpAction* action = services[i]->CreateActionLC( KGetApplicationList );
               if( action )
                  {
                  TBuf8<8> buf;
                  buf.Num(aProfileId); 
                  action->SetArgumentL(KAppFilter, aAppListFilter );
                  action->SetArgumentL(KProfileId, buf );
                  SendL( action );                 
                  CleanupStack::Pop( action );
                  return action->SessionId();
                  }
               else
                  {
                  return KErrGeneral;
                  }
            }
        }
    return KErrNotFound;
    }



TInt CDiscoverDevice::LaunchAppActionL( TInt aAppId, TInt aProfileId )
    {
    const RPointerArray<CUpnpDevice>& deviceList = DeviceList();
    if( deviceList.Count() == KErrNone ) 
        {
        return KErrBadHandle;
        }
    TInt i(0);
    while ( deviceList[i]->DeviceType().Compare(KTmServerDevice()) != KErrNone )
        {
        i++;
        }
    if( i == deviceList.Count()) 
        {
        return KErrNotFound;
        }
    RPointerArray<CUpnpService>& services = const_cast<CUpnpDevice*>(deviceList[i])->ServiceList();
    
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( KTmServerServiceType ) != KErrNotFound )
            {
            CUpnpAction* action =  services[i]->CreateActionLC( KLaunchApp );
            if( action )
              {
                  TBuf8<8> buf;
                  buf.Num(aAppId);  
                  action->SetArgumentL( KAppId, buf );                                  
                  buf.Zero();
                  buf.Num(aProfileId);  
                  action->SetArgumentL( KProfileId, buf );                  
               }            
              SendL( action );
              CleanupStack::Pop( action );
              return action->SessionId();
              }
            else
              {
              return KErrGeneral;
              }
            }
        return KErrNotFound;
    }

TInt CDiscoverDevice::TerminateAppActionL( TInt aAppId, TInt aProfileId )
    {
    const RPointerArray<CUpnpDevice>& deviceList = DeviceList();
    if( deviceList.Count() == KErrNone ) 
        {
        return KErrBadHandle;
        }
    TInt i(0);
    while ( deviceList[i]->DeviceType().Compare(KTmServerDevice()) != KErrNone )
        {
        i++;
        }
    if( i == deviceList.Count()) 
        {
        return KErrNotFound;
        }
    RPointerArray<CUpnpService>& services = const_cast<CUpnpDevice*>(deviceList[i])->ServiceList();
    
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( KTmServerServiceType ) != KErrNotFound )
            {
            CUpnpAction* action =  services[i]->CreateActionLC( KTerminateApp );
            if( action )
              {
                  TBuf8<8> buf;
                  buf.Num(aAppId);             
                  action->SetArgumentL( KAppId, buf );
                  buf.Zero();
                  buf.Num(aProfileId);             
                  action->SetArgumentL( KProfileId, buf );
               }            

              SendL( action );
              CleanupStack::Pop( action );
              return action->SessionId();
              }
            else
              {
              return KErrGeneral;
              }
            }       
    return KErrNotFound;
    }

TInt CDiscoverDevice::GetAppStatusActionL( TInt aAppId )
    {
    const RPointerArray<CUpnpDevice>& deviceList = DeviceList();
    if( deviceList.Count() == KErrNone ) 
        {
        return KErrBadHandle;
        }
    TInt i(0);
    while ( deviceList[i]->DeviceType().Compare(KTmServerDevice()) != KErrNone )
        {
        i++;
        }
    if( i == deviceList.Count()) 
        {
        return KErrNotFound;
        }
    RPointerArray<CUpnpService>& services = const_cast<CUpnpDevice*>(deviceList[i])->ServiceList();
    
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( KTmServerServiceType ) != KErrNotFound )
            {
            CUpnpAction* action = services[i]->CreateActionLC( KGetAppStatus );
            if( action )
               {
               TBuf8<8> buf;
               buf.Num(aAppId); 
               if ( aAppId == KAppId1Value || aAppId == KNonExistentAppIdValue )
                    {   
                    action->SetArgumentL( KAppId, buf );                 
                    }              
               else if( aAppId == KAppId5Value )
                   {
                    _LIT8(KWildCard, "*");
                    action->SetArgumentL( KAppId, KWildCard());
                   }   
               else
                   {
                   action->SetArgumentL( KAppId, KNullDesC8());
                   }
               SendL( action );
               CleanupStack::Pop( action );
               return action->SessionId();
               }
            else
               {
               return KErrGeneral;
               }
             }
         }
     return KErrNotFound;
     }

TInt CDiscoverDevice::SetClientProfileActionL(TInt aProfileId, const TDesC8& aClientInfoValue )
    {
    const RPointerArray<CUpnpDevice>& deviceList = DeviceList();
    if( deviceList.Count() == KErrNone ) 
        {
        return KErrBadHandle;
        }
    TInt i(0);
    while ( deviceList[i]->DeviceType().Compare(KTmServerDevice()) != KErrNone )
        {
        i++;
        }
    if( i == deviceList.Count()) 
        {
        return KErrNotFound;
        }
    RPointerArray<CUpnpService>& services = const_cast<CUpnpDevice*>(deviceList[i])->ServiceList();
    
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( KTmServerServiceType1 ) != KErrNotFound )
            {
            CUpnpAction* action = services[i]->CreateActionLC( KSetClientProfile);
            if( action )
               {   
               TBuf8<8> buf;
               buf.Num(aProfileId); 
               action->SetArgumentL(KProfileId1, buf );
               action->SetArgumentL(KClientProfile, aClientInfoValue );
               SendL( action );
               CleanupStack::Pop( action );
               return action->SessionId();
               }
            else
               {
               return KErrGeneral;
               }
             }
         }
     return KErrNotFound;
     }

TInt CDiscoverDevice::GetClientProfileActionL(TInt aProfileId )
    {
    const RPointerArray<CUpnpDevice>& deviceList = DeviceList();
    if( deviceList.Count() == KErrNone ) 
        {
        return KErrBadHandle;
        }
    TInt i(0);
    while ( deviceList[i]->DeviceType().Compare(KTmServerDevice()) != KErrNone )
        {
        i++;
        }
    if( i == deviceList.Count()) 
        {
        return KErrNotFound;
        }
    RPointerArray<CUpnpService>& services = const_cast<CUpnpDevice*>(deviceList[i])->ServiceList();
    
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( KTmServerServiceType1 ) != KErrNotFound )
            {
            CUpnpAction* action = services[i]->CreateActionLC( KGetClientProfile);
            if( action )
               {   
               TBuf8<8> buf;
               buf.Num(aProfileId); 
               action->SetArgumentL(KProfileId1, buf );
               SendL( action );
               CleanupStack::Pop( action );
               return action->SessionId();
               }
            else
               {
               return KErrGeneral;
               }
             }
         }
     return KErrNotFound;
     }

TInt CDiscoverDevice::GetMaxNumProfilesActionL( )
    {
    const RPointerArray<CUpnpDevice>& deviceList = DeviceList();
    if( deviceList.Count() == KErrNone ) 
        {
        return KErrBadHandle;
        }
    TInt i(0);
    while ( deviceList[i]->DeviceType().Compare(KTmServerDevice()) != KErrNone )
        {
        i++;
        }
    if( i == deviceList.Count()) 
        {
        return KErrNotFound;
        }
    RPointerArray<CUpnpService>& services = const_cast<CUpnpDevice*>(deviceList[i])->ServiceList();
    
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( KTmServerServiceType1 ) != KErrNotFound )
            {
            CUpnpAction* action = services[i]->CreateActionLC( KGetMaxNumProfiles);
            if( action )
               {   
               SendL( action );
               CleanupStack::Pop( action );
               return action->SessionId();
               }
            else
               {
               return KErrGeneral;
               }
             }
         }
     return KErrNotFound;
     }

void CDiscoverDevice::SubscribeToServiceL()
    {
    const RPointerArray<CUpnpDevice>& deviceList = DeviceList();
    if( deviceList.Count() == KErrNone ) 
        {
        User::Leave( KErrBadHandle );
        }
    TInt i(0);
    while ( deviceList[i]->DeviceType().Compare(KTmServerDevice()) != KErrNone )
        {
        i++;
        }
    if( i == deviceList.Count()) 
        {
        User::Leave ( KErrNotFound );
        }
        RPointerArray<CUpnpService>& services = const_cast<CUpnpDevice*>(deviceList[i])->ServiceList();
        
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( KTmServerServiceType ) != KErrNotFound )
            {
            SubscribeL( services[i] ); 
            }
        }
    }

TTerminalModeErrorCode CTmServerTest::OnGetApplicationList(TUint aProfileId ) 
    {
    if(iOperationRejected )
        {
        return ETerminalModeOperationRejected;
        }
    else if ( ( aProfileId == 0 ) || ( aProfileId == 1 ) || ( aProfileId == 2) || ( aProfileId == 3 ) 
            || ( aProfileId == 4 ) || ( aProfileId == 5 ) || ( aProfileId == 6 ) ) 
        { 
        return ETerminalModeSuccess;   
        } 
    return ETerminalModeInvalidProfileId; 
    }

TTerminalModeErrorCode CTmServerTest::OnLaunchApp( TUint aAppId, RBuf8& aUrl, TUint aProfileId )                                                                       
    {
    if ( ( aProfileId != 0 ) && ( aProfileId != 1 ) && ( aProfileId != 2) && ( aProfileId != 3 ) 
             && ( aProfileId != 4 ) && ( aProfileId != 5 ) && ( aProfileId != 6 ) ) 
        { 
        return ETerminalModeInvalidProfileId;   
        }
    aUrl.Close();
    if ( aAppId == KAppIdValue )
        {
        _LIT8(KUrl, "VNC://192.168.4.1:5900");
        aUrl.Create(KUrl());
        }
    else if ( aAppId == KAppId1Value )
        {
        _LIT8(KUrl, "RTP://192.168.2.1:5900");
        aUrl.Create(KUrl());
        }
    else if ( aAppId == KAppId2Value )
        {
        _LIT8(KUrl, "BTA2DP://192.168.2.1:5900");
        aUrl.Create(KUrl());
        }
    else if  ( aAppId == KAppId3Value)
        {
        _LIT8(KUrl, "BTHFP://192.168.2.1:5900");
        aUrl.Create(KUrl());
        }
    else if  ( aAppId == KAppId4Value)
        {
        return ETerminalModeDeviceLocked;
        }
    else 
        {
        return ETerminalModeUnauthorizedApp;
        }   
    return ETerminalModeSuccess;
    }
    
TTerminalModeErrorCode CTmServerTest::OnTerminateApp( TUint aAppId, TUint aProfileId )
    {
    if ( ( aProfileId != 0 ) && ( aProfileId != 1 ) && ( aProfileId != 2) && ( aProfileId != 3 ) 
             && ( aProfileId != 4 ) && ( aProfileId != 5 ) && ( aProfileId != 6 ) ) 
         { 
         return ETerminalModeInvalidProfileId;   
         }
   if ( aAppId == KAppIdValue|| aAppId == KAppId1Value || aAppId == KAppId2Value || aAppId == KAppId3Value)
       {
        return ETerminalModeSuccess;
       }
   else if  ( aAppId == KAppId4Value)
       {
        return ETerminalModeDeviceLocked;
       }
   else
       {
        return ETerminalModeUnauthorizedApp;
       }
    }

TTerminalModeErrorCode CTmServerTest::OnGetAppStatus(  TUint aAppId, TUint& aProfileId ,RBuf8& aStatusType )
    {
    aProfileId = KErrNone;
    aStatusType.Close();
    if ( aAppId == KAppIdValue )
        {
        aStatusType.Create(KBackground());
        }
    else if ( aAppId == KAppId1Value )
        {
         aStatusType.Create(KForeground());
        }
    else if ( aAppId == KAppId2Value )
        {
        aStatusType.Create(KBackground());
        }
    else if ( aAppId == KAppId3Value )
        {
        aStatusType.Create(KForeground());
        }  
    else
        {
        aStatusType.Create(KBackground());
        } 
    return ETerminalModeSuccess;
    }

TTerminalModeErrorCode CTmServerTest::OnGetMaxNumProfiles( TUint& aProfileIdCount )
    {
    if (iOperationRejected )
        {
        return ETerminalModeOperationRejected;
        }
    else
        {
        aProfileIdCount = 9;
        return ETerminalModeSuccess;  
        }
    }

TTerminalModeErrorCode CTmServerTest::OnSetClientProfile(TUint aProfileId, const TDesC8& aClientProfile,RBuf8& aResultProfile  )
    {
    _LIT8(KClientInfo,"<clientProfile><clientID>Cl_2</clientID></clientProfile>");
    if(iResourceBusy)
        {
        return ETerminalModeResourceBusy;
        }
    else if(aClientProfile.Compare(KClientInfo()) == KErrNone )
         {
         return ETerminalModeDeviceLocked;   
         }  
    else if ( ( aClientProfile.Find(_L8("<clientProfile><")) == KErrNotFound ) &&
        ( aClientProfile.Find(_L8("&lt;clientProfile&gt;&lt;")) == KErrNotFound ))
       {
       return ETerminalModeInvalidProfile;   
       }
    CUpnpTmClientIconPref* clientIconPref = NULL;
    if ( aProfileId == 0 )
       {
       // different mime type but same dimension
       clientIconPref = CUpnpTmClientIconPref::NewL(0);
       clientIconPref->SetWidth(88);
       clientIconPref->SetHeight(88);   
       }
    else if ( aProfileId == 1 )
       {
       // different mime type and different dimension
       clientIconPref = CUpnpTmClientIconPref::NewL(1);
       clientIconPref->SetMimeTypeL(_L8("image/bmp"));
       clientIconPref->SetWidth(70);
       clientIconPref->SetHeight(80);   
       }
    else if ( aProfileId == 2 )
       {
       // different mime type and no dimension specified by the client
       clientIconPref = CUpnpTmClientIconPref::NewL(2); 
       clientIconPref->SetMimeTypeL(_L8("image/bmp"));
       }
    else if ( aProfileId == 3 )
       {
       // same mime type but different dimension
       clientIconPref = CUpnpTmClientIconPref::NewL(3);
       clientIconPref->SetMimeTypeL(_L8("image/svg+xml"));
       clientIconPref->SetWidth(100);
       clientIconPref->SetHeight(100);       
       }
    else if ( aProfileId == 4 )
       {
       // same mime type but no dimension specified by the client
       clientIconPref = CUpnpTmClientIconPref::NewL(4);
       clientIconPref->SetMimeTypeL(_L8("image/svg+xml"));  
       }
    else if ( aProfileId == 5 )
       {
       // same mime type and same dimension
       clientIconPref = CUpnpTmClientIconPref::NewL(5);
       clientIconPref->SetMimeTypeL(_L8("image/svg+xml")); 
       clientIconPref->SetWidth(88);
       clientIconPref->SetHeight(88); 
       }
    else if ( aProfileId == 6 )
        {
        // different mime type(jpeg) and different dimensions
        clientIconPref = CUpnpTmClientIconPref::NewL(6);
        clientIconPref->SetMimeTypeL(_L8("image/jpeg")); 
        clientIconPref->SetWidth(36);
        clientIconPref->SetHeight(44); 
        }
    else
       {
       return ETerminalModeInvalidProfileId;     
       }
    iClientIconPrefArray.AppendL(clientIconPref);
    RBuf8 clientProfile;
    clientProfile.Create(aClientProfile);
    aResultProfile.Close();
    aResultProfile.Create(clientProfile);
    clientProfile.Close();
    return ETerminalModeSuccess;
    }

TTerminalModeErrorCode CTmServerTest::OnGetClientProfile( TUint aProfileId, RBuf8& aClientProfile)
    {
    if ( ( aProfileId == 0 ) || ( aProfileId == 1 ) || ( aProfileId == 2) || ( aProfileId == 3 ) 
            || ( aProfileId == 4 ) || ( aProfileId == 5 ) || ( aProfileId == 6 ) ) 
         { 
         aClientProfile.Close();
         aClientProfile.Create(KClientProfile());
         return ETerminalModeSuccess;   
         } 
    return ETerminalModeInvalidProfileId;  
    }

const CUpnpTmClientIconPref& CTmServerTest::GetClientIconPrefsL( TUint aProfileId )
    {
    CUpnpTmClientIconPref* clientIconPref = NULL;
    for ( TInt i(0); i < iClientIconPrefArray.Count(); i++ )
        {
        if ( iClientIconPrefArray[i]->ClientProfileId() == aProfileId )     
            {
            clientIconPref = iClientIconPrefArray[i];  
            break;
            }
        }
    return *clientIconPref;
    }

