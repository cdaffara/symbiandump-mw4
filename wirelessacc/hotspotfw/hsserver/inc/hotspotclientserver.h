/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Data structures for client server communication.
*
*/



#ifndef HOTSPOTCLIENTSERVER_H
#define HOTSPOTCLIENTSERVER_H

#include "hssiapsettings.h"

_LIT(KHotSpotServerExe,         "\\sys\\bin\\HotSpotServer.exe");
_LIT(KHotSpotDataServerName,    "HotSpotServer");

_LIT(KHotSpotServerName,"hotspotserver");
_LIT(KHotSpotServerImg,"HotSpotServer");		

_LIT( KHotSpotPlugin, "NetCfgExtnHotSpot" );
_LIT( KDhcpPlugin, "NetCfgExtnDhcp" );
const TUid KHotspotServerUid3={0x10282ECB};

const TUint KHssShutdown  = 777;
const TUint8 KHssMacAddressLength = 6;

const TUid KInternetConnectivityUid={0x10282EC5};
const TUid K3rdPartyEcomInterfaceUid={0x10282ED1};

_LIT8( KExtensionAPI, " HSFWExt" );
_LIT8( KUidPrefix, "0x" );
const TInt KExtensionAPILength = 32;

/**
 * Number of message slots in the server.
 * Needed message slots can be defined as follows:
 * total slots = n+2, where n is number of independent
 * asynchronous services offered by the server.
 */
const TUint KDefaultMsgSlots        = 6;

//the server version. A version must be specifyed when creating a session with the server
const TUint KHotSpotMajorVersionNumber = 0;
const TUint KHotSpotMinorVersionNumber = 1;
const TUint KHotSpotBuildVersionNumber = 1;

const TInt KIapNameLength = 128;
const TInt KUidLength = 10;

/**
* Commands from client to server.
*/
enum THotSpotCommands
    {
    // Client API 
    EHssGetScanResults,
    EHssRegister,
    EHssUnRegister,
    EHssJoin,
    EHssCancelStart,
    EHssStop,
    EHssLoginComplete,
    EHssLogoutComplete,
    EHssActivateNotifications,
    EHssCancelNotifications,
    EHssSetTimerValues,
    // WLAN Agent API
	EHssStart,
	EHssStartAgain,
	EHssCloseConnection,
	EHssCancel,
    // NIFMAN Config API
    EHssStartLogin,
    EHssCancelLogin,
    // Shutdown API
    EHssServerShutdown,
    // Internal
    EHssGetIAP,
    // UI API
    EHssUiState,
    // ICTS API
    EHssStartBrowser
    };

class TIapName
    {
    public:
        inline void SetIapName( const TDesC& aIapName )
            {
            iIapName = aIapName;
            };
        inline TDesC& IapName()
            {
            return iIapName;
            };
    private:
        TBufC< KIapNameLength > iIapName;
        
    };

class TClientUid
    {
    public:
        inline void SetUid( const TUid aUid )
            {
            iUid = aUid;
            };
        inline TUid ClientUid()
            {
            return iUid;
            };
    private:
        TUid iUid;
        
    };

class TIapSettings
    {
    public:
        inline void SetIapSettings( const THssIapSettings& aIapSettings )
            {
            iIapSettings = aIapSettings;
            };
        inline THssIapSettings& IapSettings()
            {
            return iIapSettings;
            };
    private:
        THssIapSettings iIapSettings;
        
    };
    
/**
 * Data values for security modes.
 */
enum THssSecurity
    {
    EHssSecurityOpen,
    EHssSecurityWep,
    EHssSecurity802d1x,
    EHssSecurityWpa,
    EHssSecurityWpaPsk,
    EHssSecurityWpa2,
    EHssSecurityWpa2Psk
    };

/**
* Nofications from server to client.
*/
enum THssResponses
    {
    EHssNewNetworksDetected,
    EHssOldNetworksLost
    };

/**
* Hotspot Browser UI states.
*/
enum THsBrowserUiStates
    {
    EHsBrowserUiUndefined,
    EHsBrowserUiRunning,
    EHsBrowserUiAuthenticatedOk,
    EHsBrowserUiAuthenticatedNok,
    EHsBrowserUiClosed
    };

#endif // HOTSPOTCLIENTSERVER_H
