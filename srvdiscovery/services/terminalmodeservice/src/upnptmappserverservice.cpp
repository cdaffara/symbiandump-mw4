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
* Description: CUpnpTmAppServerService class implementation.
*
*/


// System includes
#include <upnpdevice.h>
#include <upnpservice.h>

//User includes
#include "upnptmappserverservice.h"
#include "upnptmserverdevice.h"
#include "upnptmserverimpl.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmappserverserviceTraces.h"
#endif


//Defines different action names ,their arguments and their related state variables
_LIT8(KGetApplicationList,             "GetApplicationList");
_LIT8(KLaunchApp,                      "LaunchApplication");
_LIT8(KTerminateApp,                   "TerminateApplication");
_LIT8(KGetAppStatus,                   "GetApplicationStatus");
_LIT8(KAppFilter,                      "AppListingFilter");
_LIT8(KProfileId,                      "ProfileID");
_LIT8(KAppListing,                     "AppListing");
_LIT8(KAppId ,                         "AppID");
_LIT8(KAppStatus,                      "AppStatus");
_LIT8(KAppLaunchUri,                   "AppURI");
_LIT8(KAppTerminateResult,             "TerminationResult");
_LIT8(KArgTypeAppList,                 "A_ARG_TYPE_AppList");
_LIT8(KArgTypeAppStatus,               "A_ARG_TYPE_AppStatus");
_LIT8(KArgTypeAppStatusUpdate,         "AppStatusUpdate");
_LIT8(KArgTypeAppListUpdate,           "AppListUpdate");
_LIT8(KStatusTrue,                     "True");
_LIT8(KStatusFalse,                    "False");


// ================= MEMBER FUNCTIONS ==============================================

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::CUpnpTmAppServerService
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmAppServerService::CUpnpTmAppServerService( CUpnpDevice& aUpnpDevice,CUpnpTmServerImpl& aTmServerImpl ):
    CUpnpServiceImplementation(aUpnpDevice),iUpnpDevice(aUpnpDevice), iTmServerImpl(aTmServerImpl)
    {
    
    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::~CUpnpTmAppServerService
// C++ default destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmAppServerService::~CUpnpTmAppServerService()
    {
    OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_CUPNPTMAPPSERVERSERVICE_ENTRY );
    if ( iHttpServerSession )
        {
        iHttpServerSession->Stop();
        }
    delete iHttpServerSession;
    OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_CUPNPTMAPPSERVERSERVICE_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::ConstructL
// Two-phased constructor.
// Initialize variables, set parent service and create XML reader.
// ---------------------------------------------------------------------------------
//
void CUpnpTmAppServerService::ConstructL( const TDesC& aDescriptionPath )
    {
    OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_CONSTRUCTL_ENTRY );
    _LIT8( KServiceType, "TmApplicationServer:1" );
    BaseConstructL( aDescriptionPath, KServiceType);

    SetStateVariableL( KArgTypeAppList, KEmptyString, EFalse );
    SetStateVariableL( KArgTypeAppStatus, KEmptyString, EFalse );
    SetStateVariableL( KArgTypeAppStatusUpdate, KEmptyString, EFalse );
    SetStateVariableL( KArgTypeAppListUpdate, KEmptyString, EFalse );
    ConstructHttpL( );
    OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::ConstructHttpL
// Method is used to start the HTTP Server
// ---------------------------------------------------------------------------------
//
void CUpnpTmAppServerService::ConstructHttpL( )
    {
    OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_CONSTRUCTHTTPL_ENTRY );
    iHttpServerSession = CUpnpHttpServerSession::NewL( CUpnpSettings::GetIapL(),*this );
    iHttpServerSession->DefaultRuntime().SetCreator( *this );
    iHttpServerSession->StartL();
    iTmServerImpl.GenerateUrlComponents(*iHttpServerSession);
    OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_CONSTRUCTHTTPL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpTmAppServerService* CUpnpTmAppServerService::NewL(CUpnpDevice& aUpnpDevice, const TDesC& aDescriptionPath, 
                                                                               CUpnpTmServerImpl& aTmServerImp )
    {
    OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_NEWL_ENTRY );
    CUpnpTmAppServerService* appServerService = new (ELeave) CUpnpTmAppServerService(aUpnpDevice, aTmServerImp);
    CleanupStack::PushL( appServerService );
    appServerService->ConstructL( aDescriptionPath );
    CleanupStack::Pop( appServerService );
    OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_NEWL_EXIT );
    return appServerService;
    }

// ----------------------------------------------------------------------------------------------
// CUpnpTmAppServerService::ActionReceivedLD
// @param aAction action received from the client
// ----------------------------------------------------------------------------------------------
//
void CUpnpTmAppServerService::ActionReceivedLD( CUpnpAction* aAction )
    {
    OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_ACTIONRECEIVEDLD_ENTRY );
    if ( !aAction )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PushL( aAction );
    TUpnpErrorCode eCode = EHttpOk;
    
    // Get the name, saves calls in the if-else statements
    TPtrC8 name( aAction->Name() );
    // Verify the action and do the operation(s)
    if ( name.Compare( KGetApplicationList ) == 0 )
        {
        eCode = GetAppListActionL( aAction );
        }
    else if ( name.Compare( KLaunchApp ) == 0 )
        {
        eCode = LaunchAppActionL( aAction );
        }
    else if ( name.Compare( KTerminateApp ) == 0 )
        {
        eCode = TerminateAppActionL ( aAction );
        }
    else if ( name.Compare( KGetAppStatus) == 0 )
        {
        eCode = GetAppStatusActionL( aAction );
        }
    else
        {
        eCode = EInvalidAction;
        }
    SendL( aAction, eCode );
    CleanupStack::PopAndDestroy( aAction );
    OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_ACTIONRECEIVEDLD_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::GetAppListActionL
// @param aAction pointer to upnp action object
// @return Returns upnp error code
// ---------------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpTmAppServerService::GetAppListActionL( CUpnpAction* aAction )
   {
   OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_GETAPPLISTACTIONL_ENTRY );
   // Fetch the value for app listing filter argument
   const TDesC8& filter = aAction->ArgumentValue(KAppFilter);
   // Validate the AppListing filter string
   // AppListingFilter input argument can contain one of these: <1> an empty string ("")  
   // <2> wildcard(*)  <3> a well formatted filter string 
   if ( ( filter.Compare(KNullDesC8()) != KErrNone ) && ( filter.Compare(KWildCard ) != KErrNone ) 
        && ( filter.Find(Keq) == KErrNotFound ) )
       {
       // AppListingFilter argument is not having empty string ( "" ) , or wildcard(*) , 
       // or a valid filter string( =" is missing )
       OstTrace0( TRACE_ERROR, DUP2_CUPNPTMAPPSERVERSERVICE_GETAPPLISTACTIONL, "CUpnpTmAppServerService::GetAppListActionL" );   
       return EInvalidArgs;  // invalid AppListingFilter argument
       }
   
   // Fetch the value for profile ID argument
   TUint profileIdInt;
   ConvertDescriptorToInt( aAction->ArgumentValue(KProfileId), profileIdInt );
   OstTrace1( TRACE_NORMAL, CUPNPTMAPPSERVERSERVICE_GETAPPLISTACTIONL, "CUpnpTmAppServerService::GetAppListActionL;profileIdInt=%u", profileIdInt );
   
   TTerminalModeErrorCode ret;
   const TDesC8& appList = iTmServerImpl.GetApplicationListL( filter,profileIdInt,ret );
   if ( ret != ETerminalModeSuccess )
       {
       OstTrace1( TRACE_ERROR, DUP1_CUPNPTMAPPSERVERSERVICE_GETAPPLISTACTIONL, "CUpnpTmAppServerService::GetAppListActionL;ret=%d", ret );
       return TUpnpErrorCode( ret );       
       }
   aAction->SetArgumentL( KAppListing, appList ); 
   OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_GETAPPLISTACTIONL_EXIT );
   return EHttpOk;
   }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::LaunchAppActionL
// @param aAction pointer to upnp action object
// @return Returns upnp error code
// ---------------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpTmAppServerService::LaunchAppActionL( CUpnpAction* aAction )
   {
   OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_LAUNCHAPPACTIONL_ENTRY );
   TUint appIdInt;  
   TUint profileIdInt;
   // Validates the input parameters
   if ( ( ConvertDescriptorToInt( aAction->ArgumentValue(KAppId), appIdInt ) != KErrNone )
       || ( ConvertDescriptorToInt( aAction->ArgumentValue(KProfileId), profileIdInt ) != KErrNone ))
        {
        OstTraceExt2( TRACE_ERROR, CUPNPTMAPPSERVERSERVICE_LAUNCHAPPACTIONL, "CUpnpTmAppServerService::LaunchAppActionL;appIdInt=%u;profileIdInt=%u", appIdInt, profileIdInt );
        return EInvalidArgs;   // either invalid app ID or invalid profile ID  
        }

   RBuf8 launchUrl;
   TTerminalModeErrorCode ret = iTmServerImpl.LaunchApp( appIdInt,profileIdInt,launchUrl );
   
   if ( ret != ETerminalModeSuccess )
       {
       // Error is returned
       launchUrl.Close();
       OstTrace0( TRACE_ERROR, DUP1_CUPNPTMAPPSERVERSERVICE_LAUNCHAPPACTIONL, "CUpnpTmAppServerService::LaunchAppActionL" );   
       return TUpnpErrorCode( ret );
       }
   CleanupClosePushL(launchUrl);
   aAction->SetArgumentL( KAppLaunchUri , launchUrl );
   
   CleanupStack::PopAndDestroy(&launchUrl);
   OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_LAUNCHAPPACTIONL_EXIT );
   return EHttpOk;  
   }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::TerminateAppActionL
// @param aAction pointer to upnp action object
// @return Returns upnp error code
// ---------------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpTmAppServerService::TerminateAppActionL( CUpnpAction* aAction )
   { 
   OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_TERMINATEAPPACTIONL_ENTRY );
   // Validates the input parameters
   TUint appIdInt;   
   TUint profileIdInt;
   if ( ( ConvertDescriptorToInt( aAction->ArgumentValue(KAppId), appIdInt ) != KErrNone )
       || ( ConvertDescriptorToInt( aAction->ArgumentValue(KProfileId), profileIdInt ) != KErrNone ))
        {
        OstTraceExt2( TRACE_ERROR, CUPNPTMAPPSERVERSERVICE_TERMINATEAPPACTIONL, "CUpnpTmAppServerService::TerminateAppActionL;appIdInt=%u;profileIdInt=%u", appIdInt, profileIdInt );
        return EInvalidArgs;   // either invalid app ID or invalid profile ID  
        }
   
   TTerminalModeErrorCode ret = iTmServerImpl.TerminateApp( appIdInt,profileIdInt );
   if ( ret != ETerminalModeSuccess )
       {
       // Error is returned
       aAction->SetArgumentL( KAppTerminateResult,KStatusFalse );
       OstTrace0( TRACE_ERROR, DUP1_CUPNPTMAPPSERVERSERVICE_TERMINATEAPPACTIONL, "CUpnpTmAppServerService::TerminateAppActionL" );    
       return TUpnpErrorCode( ret );
       }
   aAction->SetArgumentL( KAppTerminateResult,KStatusTrue); 
   OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_TERMINATEAPPACTIONL_EXIT );
   return EHttpOk;   
   }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::GetAppStatusActionL
// @param aAction Pointer to upnp action object
// @return Returns upnp error code
// ---------------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpTmAppServerService::GetAppStatusActionL( CUpnpAction* aAction )
   {
   OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_GETAPPSTATUSACTIONL_ENTRY );
   const TDesC8& appId= aAction->ArgumentValue(KAppId);
   // Check if the appID argument has either wildcard character(*)
   // or a valid integer as its value
   TUint appIdInt;
   if ( (( appId.Compare( KWildCard ) != KErrNone ) &&
          ( ConvertDescriptorToInt( appId, appIdInt ) != KErrNone )) )
        {
        OstTrace1( TRACE_ERROR, CUPNPTMAPPSERVERSERVICE_GETAPPSTATUSACTIONL, "CUpnpTmAppServerService::GetAppStatusActionL;appIdInt=%u", appIdInt );
        return EInvalidArgs; // invalid input argument
        }
   RBuf8 appStatusBuf;
   CleanupClosePushL(appStatusBuf);
   TTerminalModeErrorCode ret;
   iTmServerImpl.GetAppStatusL( appId, appStatusBuf, ret);
   
   if ( ret != ETerminalModeSuccess )
       {
       CleanupStack::PopAndDestroy(&appStatusBuf);
       OstTrace0( TRACE_ERROR, DUP1_CUPNPTMAPPSERVERSERVICE_GETAPPSTATUSACTIONL, "CUpnpTmAppServerService::GetAppStatusActionL" );      
       return TUpnpErrorCode( ret );
       }
   aAction->SetArgumentL( KAppStatus, appStatusBuf );
   
   CleanupStack::PopAndDestroy(&appStatusBuf);
   OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_GETAPPSTATUSACTIONL_EXIT );
   return EHttpOk;   
   }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::NewTransactionL
// This method is invoked whenever http request related to icon file transfer
// arrives to http server from remote client
// @param aMethod Http message method ( GET, HEAD or POST )
// @param aUri Http message request uri
// @param aSender IP address of the Http message sender
// @param[out] aTrans Returns newly created transaction object 
// ---------------------------------------------------------------------------------
//
void CUpnpTmAppServerService::NewTransactionL( const TDesC8& aMethod, const TDesC8& aUri, 
                          const TInetAddr& /*aSender*/, CUpnpHttpServerTransaction*& aResultTrans )
    {
    OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_NEWTRANSACTIONL_ENTRY );
    if ( aMethod != KHttpPost() )
        {
        iTmServerImpl.GetIconInfoL(aUri, aResultTrans);
        }
    OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_NEWTRANSACTIONL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::HttpEventLD
// Provides file transfer completion status
// Notifies about the HTTP events, errors.
// Handling of http events is not provided in Terminal Mode Service
// @param aMessage the HTTP message
// ---------------------------------------------------------------------------------
//
void CUpnpTmAppServerService::HttpEventLD( CUpnpHttpMessage* /*aMessage*/ )
    {

    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::AppStatusUpdateEventL
// Notifies the client about the status updates.
// @param aStatusUpdateBuffer Buffer holding the update status information.
// ---------------------------------------------------------------------------------
//
void CUpnpTmAppServerService::AppStatusUpdateEventL( const TDesC8& aStatusUpdateBuffer )
    {
    OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_APPSTATUSUPDATEEVENTL_ENTRY );
    SetStateVariableL( KArgTypeAppStatusUpdate, aStatusUpdateBuffer, EFalse );
    OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_APPSTATUSUPDATEEVENTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::AppListUpdateEventL
// Notifies the client about the app list updates.
// @param aListUpdateBuffer Buffer holding the update app list information.
// ---------------------------------------------------------------------------------
//
void CUpnpTmAppServerService::AppListUpdateEventL( const TDesC8& aListUpdateBuffer )
    {
    OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_APPLISTUPDATEEVENTL_ENTRY );
    SetStateVariableL( KArgTypeAppListUpdate, aListUpdateBuffer, EFalse );   
    OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_APPLISTUPDATEEVENTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::ConvertDescriptorToInt
// Utility method to convert descriptor's data to integer
// @param aDes   Descriptor whose integer equivalent have to be calculated
// @param aIntVal[out] Integer value
// @return Returns error code
// ---------------------------------------------------------------------------------
//
TInt CUpnpTmAppServerService::ConvertDescriptorToInt( const TDesC8& aDes, TUint& aIntVal )
    {
    OstTraceFunctionEntry0( CUPNPTMAPPSERVERSERVICE_CONVERTDESCRIPTORTOINT_ENTRY );
    aIntVal = KErrNone;
    TLex8 lex( aDes );
    OstTrace1( TRACE_NORMAL, CUPNPTMAPPSERVERSERVICE_CONVERTDESCRIPTORTOINT, "CUpnpTmAppServerService::ConvertDescriptorToInt;aIntVal=%u", aIntVal );
    OstTraceFunctionExit0( CUPNPTMAPPSERVERSERVICE_CONVERTDESCRIPTORTOINT_EXIT );
    return lex.Val(aIntVal);
    }

//End of File
