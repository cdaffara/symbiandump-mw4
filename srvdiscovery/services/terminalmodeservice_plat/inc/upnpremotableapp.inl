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

#ifndef __UPNPREMOTABLEAPP_INL__
#define __UPNPREMOTABLEAPP_INL__


// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::IconList
// Method is used to retreive icon list.
// @return Returns reference to the list of CUpnpTerminalModeIcon objects
// ---------------------------------------------------------------------------------
//
inline const RPointerArray<CUpnpTerminalModeIcon>& CUpnpRemotableApp::IconList()const
    {
    return iIconList;
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::TmInfoElementList
// Method is used to retrieve terminal mode info list
// @return Returns reference to an array of CUpnpTmInfoElement objects
// ---------------------------------------------------------------------------------
//
inline const RPointerArray<CUpnpTmInfoElement>& CUpnpRemotableApp::TmInfoElementList()const
    {
    return iTmInfoElementList;
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::AppId
// Method is used to retrieve application ID.
// @return Returns appID.
// ---------------------------------------------------------------------------------
//
inline TUint CUpnpRemotableApp::AppId()const
    {
    return iAppId;
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::AppName
// Method is used to retrieve applicationName.
// @return Returns application name.
// ---------------------------------------------------------------------------------
//
inline const TDesC8& CUpnpRemotableApp::AppName()const
    {
    return iAppName;
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::AppDescription
// Method is used to retrieve brief description of the application.
// @return Returns application description.
// ---------------------------------------------------------------------------------
inline const TDesC8& CUpnpRemotableApp::AppDescription()const
    {
    return iAppDescription;
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::ResourceStatus
// Method is used to retrieve current sttaus of the resource.
// @return Returns the resource status of the application.
// ---------------------------------------------------------------------------------
//
inline const TDesC8& CUpnpRemotableApp::ResourceStatus()const
    {
    return iResourceStatus;
    }

// ---------------------------------------------------------------------------------
// CUpnpRemotableApp::AllowedProfileIdList
// Method is used to retrieve list of allowed profile IDs for the application.
// @return Returns the reference to the array of profile IDs.
// ---------------------------------------------------------------------------------
//
inline const RArray<TUint>& CUpnpRemotableApp::AllowedProfileIdList()const
    {
    return iProfileIdList;
    }

#endif  //__UPNPREMOTABLEAPP_INL__
