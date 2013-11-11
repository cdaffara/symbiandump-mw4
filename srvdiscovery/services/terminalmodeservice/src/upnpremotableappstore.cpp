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
#include "upnpremotableappstore.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnpremotableappstoreTraces.h"
#endif


// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpRemotableAppStore::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpRemotableAppStore* CUpnpRemotableAppStore::NewL()
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPPSTORE_NEWL_ENTRY );
    return ( new (ELeave) CUpnpRemotableAppStore()); 
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableAppStore::CUpnpRemotableAppStore
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpRemotableAppStore::CUpnpRemotableAppStore( )
    {
    
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableAppStore::~CUpnpRemotableAppStore
// Destructor 
// ---------------------------------------------------------------------------------
//
CUpnpRemotableAppStore::~CUpnpRemotableAppStore()
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPPSTORE_CUPNPREMOTABLEAPPSTORE_ENTRY );
    iRemotableAppArray.ResetAndDestroy();
    iRemotableAppArray.Close();
    iAppIdArray.Close();
    OstTraceFunctionExit0( CUPNPREMOTABLEAPPSTORE_CUPNPREMOTABLEAPPSTORE_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableAppStore::AppendRemotableAppL
// Method is used to create a list of all the registered apps
// @param aApp Pointer to CUpnpRemotableApp object
// ---------------------------------------------------------------------------------
//
void CUpnpRemotableAppStore::AddRemotableAppL( CUpnpRemotableApp* aApp )
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPPSTORE_ADDREMOTABLEAPPL_ENTRY );
    TInt appId = aApp->AppId();
    if ( iAppIdArray.Find( appId ) == KErrNotFound )
        {
        // Register all distinct apps
        OstTrace1( TRACE_FLOW, CUPNPREMOTABLEAPPSTORE_ADDREMOTABLEAPPL, "CUpnpRemotableAppStore::AddRemotableAppL;appId=%d", appId );    
        iRemotableAppArray.AppendL(aApp);
        iAppIdArray.AppendL( appId );
        }
    else
        {
        // Delete the duplicate remotable app object
        delete aApp;
        aApp = NULL;
        }
    OstTraceFunctionExit0( CUPNPREMOTABLEAPPSTORE_ADDREMOTABLEAPPL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableAppStore::RemoveRemotableApp
// Method is used to remove the app frpm the list of already registered apps
// @param aAppId App to be removed
// ---------------------------------------------------------------------------------
//
TInt CUpnpRemotableAppStore::RemoveRemotableApp( TUint aAppId )
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPPSTORE_REMOVEREMOTABLEAPP_ENTRY );
    TInt ret(KErrNotFound);
    TInt appIndex = iAppIdArray.Find( aAppId );
    if ( appIndex != KErrNotFound )
        {
        // If appID is matched then delete the Remotable App object and remove 
        // the object pointer from the array
        iAppIdArray.Remove(appIndex);
        delete iRemotableAppArray[appIndex];
        iRemotableAppArray[appIndex] = NULL;
        iRemotableAppArray.Remove(appIndex);
        ret = KErrNone;
        }
    OstTrace1( TRACE_NORMAL, CUPNPREMOTABLEAPPSTORE_REMOVEREMOTABLEAPP, "CUpnpRemotableAppStore::RemoveRemotableApp;ret=%d", ret );  
    OstTraceFunctionExit0( CUPNPREMOTABLEAPPSTORE_REMOVEREMOTABLEAPP_EXIT );
    return ret;
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableAppStore::FetchRemotableApp
// Method is used to fetch the list of registered apps
// @param aAppId App ID of the application whose access is needed
// @return Returns reference to CUpnpRemotableApp object
// ---------------------------------------------------------------------------------
//
CUpnpRemotableApp& CUpnpRemotableAppStore::FetchRemotableApp( TInt aAppIndex )const
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPPSTORE_FETCHREMOTABLEAPP_ENTRY );
    ASSERT( aAppIndex >=0 );
    OstTraceFunctionExit0( CUPNPREMOTABLEAPPSTORE_FETCHREMOTABLEAPP_EXIT );
    return *iRemotableAppArray[aAppIndex];
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableAppStore::AppIdArray
// Method is used to fetch the list of appIDs of registered apps
// @return Returns reference to an array of TInt objects
// ---------------------------------------------------------------------------------
//
const RArray<TUint>& CUpnpRemotableAppStore::AppIdArray()const 
    {
    OstTraceFunctionEntry0( CUPNPREMOTABLEAPPSTORE_APPIDARRAY_ENTRY );
    OstTraceFunctionExit0( CUPNPREMOTABLEAPPSTORE_APPIDARRAY_EXIT );
    return iAppIdArray;
    }

// End of File
