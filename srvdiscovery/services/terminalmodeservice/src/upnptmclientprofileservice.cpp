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
* Description: CUpnpTmClientProfileService class implementation.
*
*/

// System includes
#include <upnpdevice.h>
#include <upnpservice.h>

//User includes
#include "upnptmclientprofileservice.h"
#include "upnptmserverdevice.h"
#include "upnptmserverimpl.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmclientprofileserviceTraces.h"
#endif


//Defines different action names ,their arguments and their related state variables
_LIT8(KGetMaxNumProfiles,              "GetMaxNumProfiles");
_LIT8(KSetClientProfile,               "SetClientProfile");
_LIT8(KGetClientProfile,               "GetClientProfile");
_LIT8(KProfileId,                      "profileID");
_LIT8(KClientProfile ,                 "clientProfile");
_LIT8(KResultProfile ,                 "resultProfile");
_LIT8(KNumProfilesAllowed ,            "numProfilesAllowed");
_LIT8(KArgTypeUnusedProfileIds,        "UnusedProfileIDs");
_LIT8(KArgTypeClientProfile,           "A_ARG_TYPE_ClientProfile");
_LIT8(KArgTypeMaxProfiles,             "MaxNumProfiles");
_LIT8(KArgTypeProfileId,               "A_ARG_TYPE_ProfileID");


// ================= MEMBER FUNCTIONS ==============================================

// ---------------------------------------------------------------------------------
// CUpnpTmClientProfileService::CUpnpTmClientProfileService
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmClientProfileService::CUpnpTmClientProfileService( CUpnpDevice& aUpnpDevice,CUpnpTmServerImpl& aTmServerImpl )
    :CUpnpServiceImplementation(aUpnpDevice),iUpnpDevice(aUpnpDevice), iTmServerImpl(aTmServerImpl)
    {
    OstTraceFunctionEntry0( CUPNPTMCLIENTPROFILESERVICE_CUPNPTMCLIENTPROFILESERVICE_ENTRY );
    
    OstTraceFunctionExit0( CUPNPTMCLIENTPROFILESERVICE_CUPNPTMCLIENTPROFILESERVICE_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientProfileService::~CUpnpTmClientProfileService
// C++ default destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmClientProfileService::~CUpnpTmClientProfileService()
    {

    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientProfileService::ConstructL
// Two-phased constructor.
// Initialize variables, set parent service and create XML reader.
// ---------------------------------------------------------------------------------
//
void CUpnpTmClientProfileService::ConstructL(const TDesC& aDescriptionPath )
    {
    OstTraceFunctionEntry0( CUPNPTMCLIENTPROFILESERVICE_CONSTRUCTL_ENTRY );
    _LIT8( KServiceType, "TmClientProfile:1" );
    BaseConstructL( aDescriptionPath, KServiceType);

    SetStateVariableL( KArgTypeUnusedProfileIds, KEmptyString, EFalse );
    SetStateVariableL( KArgTypeClientProfile, KEmptyString, EFalse );
    SetStateVariableL( KArgTypeMaxProfiles, KEmptyString, EFalse );
    SetStateVariableL( KArgTypeProfileId, KEmptyString, EFalse );
    OstTraceFunctionExit0( CUPNPTMCLIENTPROFILESERVICE_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientProfileService::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpTmClientProfileService* CUpnpTmClientProfileService::NewL(CUpnpDevice& aUpnpDevice, const TDesC& aDescriptionPath, CUpnpTmServerImpl& aTmServerImp )
    {
    OstTraceFunctionEntry0( CUPNPTMCLIENTPROFILESERVICE_NEWL_ENTRY );
    CUpnpTmClientProfileService* clientProfileService = new (ELeave) CUpnpTmClientProfileService(aUpnpDevice, aTmServerImp);
    CleanupStack::PushL( clientProfileService );
    clientProfileService->ConstructL( aDescriptionPath );
    CleanupStack::Pop( clientProfileService );
    OstTraceFunctionExit0( CUPNPTMCLIENTPROFILESERVICE_NEWL_EXIT );
    return clientProfileService;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientProfileService::ActionReceivedLD
// @param aAction action received from the client
// ---------------------------------------------------------------------------------
//
void CUpnpTmClientProfileService::ActionReceivedLD( CUpnpAction* aAction )
    {
    OstTraceFunctionEntry0( CUPNPTMCLIENTPROFILESERVICE_ACTIONRECEIVEDLD_ENTRY );
    if ( !aAction )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PushL( aAction );
    TUpnpErrorCode eCode = EHttpOk;
    
    // Get the name, saves calls in the if-else statements
    TPtrC8 name( aAction->Name() );
    // Verify the action and do the operation(s)
    if ( name.Compare( KGetMaxNumProfiles ) == 0 )
        {
        eCode = GetMaxNumProfilesActionL( aAction );
        }
    else if ( name.Compare( KSetClientProfile ) == 0 )
        {
        eCode = SetClientProfileActionL( aAction );
        }
    else if ( name.Compare( KGetClientProfile ) == 0 )
        {
        eCode = GetClientProfileActionL ( aAction );
        }
    else
        {
        eCode = EInvalidAction;
        }
    SendL( aAction, eCode );
    CleanupStack::PopAndDestroy( aAction );
    OstTraceFunctionExit0( CUPNPTMCLIENTPROFILESERVICE_ACTIONRECEIVEDLD_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientProfileService::GetMaxNumProfilesActionL
// @param aAction pointer to action object
// @return Returns upnp error code
// ---------------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpTmClientProfileService::GetMaxNumProfilesActionL( CUpnpAction* aAction )
   {
   OstTraceFunctionEntry0( CUPNPTMCLIENTPROFILESERVICE_GETMAXNUMPROFILESACTIONL_ENTRY );
   TUint profileCount;
   TTerminalModeErrorCode ret = iTmServerImpl.GetMaxNumProfiles( profileCount );
   if ( ret != ETerminalModeSuccess ) 
       {
       OstTrace0( TRACE_ERROR, DUP1_CUPNPTMCLIENTPROFILESERVICE_GETMAXNUMPROFILESACTIONL, "CUpnpTmClientProfileService::GetMaxNumProfilesActionL" );
       return TUpnpErrorCode( ret );       
       } 
   OstTrace1( TRACE_NORMAL, CUPNPTMCLIENTPROFILESERVICE_GETMAXNUMPROFILESACTIONL, "CUpnpTmClientProfileService::GetMaxNumProfilesActionL;profileCount=%d", profileCount );      
   TBuf8<UpnpString::KMaxTUintLength> countBuf;
   countBuf.Num(profileCount);
   
   aAction->SetArgumentL( KNumProfilesAllowed, countBuf ); 
   OstTraceFunctionExit0( CUPNPTMCLIENTPROFILESERVICE_GETMAXNUMPROFILESACTIONL_EXIT );
   return EHttpOk;
   }

// -------------------------------------------------------------------------------------
// CUpnpTmClientProfileService::SetClientProfileActionL
// @param aAction  Pointer to UPnP action object
// @return Returns upnp error code
// ---------------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpTmClientProfileService::SetClientProfileActionL( CUpnpAction* aAction )
    {
    OstTraceFunctionEntry0( CUPNPTMCLIENTPROFILESERVICE_SETCLIENTPROFILEACTIONL_ENTRY );
    TUint profileIdInt;
    // Fetch the value for profile ID argument
    TLex8 lex( aAction->ArgumentValue(KProfileId) );
    lex.Val(profileIdInt);
    OstTrace1( TRACE_ERROR, CUPNPTMCLIENTPROFILESERVICE_SETCLIENTPROFILEACTIONL, "CUpnpTmClientProfileService::SetClientProfileActionL;profileIdInt=%d", profileIdInt );      
    
    // Fetch the value for client profile argument
    const TDesC8& clientProfile = aAction->ArgumentValue(KClientProfile);
    
    RBuf8 resultProfileBuf;
    TTerminalModeErrorCode ret = iTmServerImpl.SetClientProfile( profileIdInt,clientProfile,resultProfileBuf );
    if ( ret != ETerminalModeSuccess )
       {
       resultProfileBuf.Close();
       OstTrace0( TRACE_ERROR, DUP1_CUPNPTMCLIENTPROFILESERVICE_SETCLIENTPROFILEACTIONL, "CUpnpTmClientProfileService::SetClientProfileActionL" );
       return TUpnpErrorCode( ret );
       }
    CleanupClosePushL(resultProfileBuf);
    aAction->SetArgumentL( KResultProfile , resultProfileBuf );
    CleanupStack::PopAndDestroy(&resultProfileBuf);
    OstTraceFunctionExit0( CUPNPTMCLIENTPROFILESERVICE_SETCLIENTPROFILEACTIONL_EXIT );
    return EHttpOk;  
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientProfileService::GetClientProfileActionL
// @param aAction pointer to action object
// @return Returns upnp error code
// ---------------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpTmClientProfileService::GetClientProfileActionL( CUpnpAction* aAction )
    { 
    OstTraceFunctionEntry0( CUPNPTMCLIENTPROFILESERVICE_GETCLIENTPROFILEACTIONL_ENTRY );
    TUint profileIdInt;
    // Fetch the value for profile ID argument
    TLex8 lex( aAction->ArgumentValue(KProfileId) );
    lex.Val(profileIdInt);
    OstTrace1( TRACE_ERROR, CUPNPTMCLIENTPROFILESERVICE_GETCLIENTPROFILEACTIONL, "CUpnpTmClientProfileService::GetClientProfileActionL;profileIdInt=%d", profileIdInt );        

    RBuf8 resultProfileBuf;
    TTerminalModeErrorCode ret = iTmServerImpl.GetClientProfile(profileIdInt,resultProfileBuf );
    
    if ( ret != ETerminalModeSuccess )
       {
       resultProfileBuf.Close();
       OstTrace0( TRACE_ERROR, DUP1_CUPNPTMCLIENTPROFILESERVICE_GETCLIENTPROFILEACTIONL, "CUpnpTmClientProfileService::GetClientProfileActionL" ); 
       return TUpnpErrorCode( ret );
       }
    CleanupClosePushL(resultProfileBuf);
    aAction->SetArgumentL( KClientProfile,resultProfileBuf ); 
    CleanupStack::PopAndDestroy(&resultProfileBuf);
    OstTraceFunctionExit0( CUPNPTMCLIENTPROFILESERVICE_GETCLIENTPROFILEACTIONL_EXIT );
    return EHttpOk;   
    }

// ---------------------------------------------------------------------------------
// CUpnpTmAppServerService::UnusedProfileIdEventL
// Notifies the client about the status updates.
// @param aUnusedProfileIdBuffer Buffer holding the unused profile IDs information.
// ---------------------------------------------------------------------------------
//
void CUpnpTmClientProfileService::UnUsedProfileIdEventL(const TDesC8& aUnusedProfileIdBuffer)
    {
    OstTraceFunctionEntry0( CUPNPTMCLIENTPROFILESERVICE_UNUSEDPROFILEIDEVENTL_ENTRY );
    SetStateVariableL( KArgTypeUnusedProfileIds, aUnusedProfileIdBuffer, EFalse);
    OstTraceFunctionExit0( CUPNPTMCLIENTPROFILESERVICE_UNUSEDPROFILEIDEVENTL_EXIT );
    }

//End of File
