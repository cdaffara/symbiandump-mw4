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
* Description: CUpnpRemotableApp class implementation.
*
*/

#include "upnpremotableapp.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnpremotableappTraces.h"
#endif


// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::NewL
// Two-phased constructor.
// @param aAppId App ID of the remotable app
// @param aAppName Name of the remotable app
// ---------------------------------------------------------------------------------
//
EXPORT_C CUpnpRemotableApp* CUpnpRemotableApp::NewL( TUint aAppId ,const TDesC8& aAppName )
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPP_NEWL_ENTRY );
    CUpnpRemotableApp* self = new (ELeave) CUpnpRemotableApp(aAppId);    
    CleanupStack::PushL(self);
    self->ConstructL( aAppName );  
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUPNPREMOTABLEAPP_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::CUpnpRemotableApp
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------------
//
CUpnpRemotableApp::CUpnpRemotableApp( TUint aAppId ):iAppId(aAppId)
    {
    
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpRemotableApp::ConstructL( const TDesC8& aAppname )
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPP_CONSTRUCTL_ENTRY );
    iAppName.CreateL(aAppname);
    OstTraceFunctionExit0( CUPNPREMOTABLEAPP_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::~CUpnpRemotableApp
// Destructor
// ---------------------------------------------------------------------------------
//
CUpnpRemotableApp::~CUpnpRemotableApp()
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPP_CUPNPREMOTABLEAPP_ENTRY );
    iTmInfoElementList.ResetAndDestroy();
    iTmInfoElementList.Close();
    iProfileIdList.Close();
    iIconList.ResetAndDestroy();
    iIconList.Close();
    iAppName.Close();
    iAppDescription.Close();
    iResourceStatus.Close();
    OstTraceFunctionExit0( CUPNPREMOTABLEAPP_CUPNPREMOTABLEAPP_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::AddIconL
// Method is used to add an icon element to the application.
// @param aIcon Pointer to TerminalMode icon object
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpRemotableApp::AddIconL(CUpnpTerminalModeIcon* aIcon)
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPP_ADDICONL_ENTRY );
    iIconList.AppendL(aIcon);
    OstTraceFunctionExit0( CUPNPREMOTABLEAPP_ADDICONL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::SetAppDescriptionL
// Method is used to add the description of the application
// @param aDescription Description of the aapplication
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpRemotableApp::SetAppDescriptionL( const TDesC8& aDescription )
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPP_SETAPPDESCRIPTIONL_ENTRY );
    iAppDescription.Close();
    iAppDescription.CreateL(aDescription);
    OstTraceFunctionExit0( CUPNPREMOTABLEAPP_SETAPPDESCRIPTIONL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::SetAllowedProfileIdListL
// Method is used to set the list of allowed profile IDs for the application.
// @param aProfileIdList Reference to the list of allowed profile IDs.
//                       A copy of the list is maintained.
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpRemotableApp::SetAllowedProfileIdListL( const RArray<TUint>& aProfileIdList )
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPP_SETALLOWEDPROFILEIDLISTL_ENTRY );
    iProfileIdList.Close();
    /**
     * Create a copy of the profile ID list.
     * Only distinct and unique profile IDs are copied into the array.
     */
    for ( TInt i(0); i < aProfileIdList.Count(); i++ )
        {
        OstTrace1( TRACE_NORMAL, CUPNPREMOTABLEAPP_SETALLOWEDPROFILEIDLISTL, "CUpnpRemotableApp::SetAllowedProfileIdListL;aProfileIdList[i]=%d", aProfileIdList[i] );
        if ( iProfileIdList.Find(aProfileIdList[i]) == KErrNotFound )
            {
            iProfileIdList.AppendL(aProfileIdList[i]);
            }
        }
    OstTraceFunctionExit0( CUPNPREMOTABLEAPP_SETALLOWEDPROFILEIDLISTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::CreateTmInfoElementL
// Method is used to create a terminal mode info for the application and 
// it gets appended to the terminal mode list.
// @param aTerminalModeInfoType Terminal Mode Info Type
// @return Returns the reference to the CUpnpTerminalModeInfo object
// ---------------------------------------------------------------------------------
//
EXPORT_C CUpnpTmInfoElement& CUpnpRemotableApp::CreateTmInfoElementL(
                       CUpnpTmInfoElement::TTerminalModeInfoType aTerminalModeInfoType )
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPP_CREATETERMINALMODEINFOL_ENTRY );
    CUpnpTmInfoElement* tmInfoElement = CUpnpTmInfoElement::NewL(aTerminalModeInfoType);
    CleanupStack::PushL(tmInfoElement);
    iTmInfoElementList.AppendL(tmInfoElement);
    CleanupStack::Pop(tmInfoElement);
    OstTraceFunctionExit0( CUPNPREMOTABLEAPP_CREATETERMINALMODEINFOL_EXIT );
    return *tmInfoElement;
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::SetResourceStatusL
// Method is used to define the current status of the resource.
// @param aResourceStatus Status of the resource
//   “free” – Resource is free. Can be used by the Terminal Mode client. (default)
//   “busy” – Resource already used. Resource assignment can be overridden by
//            a client’s invocation LaunchApplication action.
//   “NA” – Resource already used. Resource assignment cannot be overridden by a 
//          LaunchApplication action invoked by a client.
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpRemotableApp::SetResourceStatusL( const TDesC8& aResourceStatus )
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPP_SETRESOURCESTATUSL_ENTRY );
    iResourceStatus.Close();
    iResourceStatus.CreateL(aResourceStatus); 
    OstTraceFunctionExit0( CUPNPREMOTABLEAPP_SETRESOURCESTATUSL_EXIT );
    }

