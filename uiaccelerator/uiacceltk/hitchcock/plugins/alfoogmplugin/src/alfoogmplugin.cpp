/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  OOM plugin for ALFserver.
*
*/

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <goommonitorplugin.h>
#include <w32std.h>

#include <alf/alfdirectclient.h>

class CAlfOogmMonitorPlugin : public CGOomMonitorPlugin
    {
public:
    CAlfOogmMonitorPlugin();
    ~CAlfOogmMonitorPlugin();
    void ConstructL();

public:
    void FreeRam(TInt aBytesToFree, TInt  aFlags);
    void MemoryGood(TInt  aFlags);

private:
    RAlfDirectClient iAlfDirectClient;
    };

// -----------------------------------------------------------------------------
// C++ constructor.
// -----------------------------------------------------------------------------
//
CAlfOogmMonitorPlugin::CAlfOogmMonitorPlugin()
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CAlfOogmMonitorPlugin::~CAlfOogmMonitorPlugin()
    {
    iAlfDirectClient.Disconnect();
    }

// -----------------------------------------------------------------------------
// Symbian two-phased constructor (2nd phase).
// -----------------------------------------------------------------------------
//
void CAlfOogmMonitorPlugin::ConstructL()
    {
    CGOomMonitorPluginBase::ConstructL();
    }


// -----------------------------------------------------------------------------
// Tries to free RAM.
// -----------------------------------------------------------------------------
//
void CAlfOogmMonitorPlugin::FreeRam(TInt aBytesToFree, TInt  aFlags)
    {
	TBool useSwRend = aFlags & KGOomUseSwRendering;
 	iAlfDirectClient.EnableLowMemoryState(aBytesToFree, useSwRend);
    }

// -----------------------------------------------------------------------------
// CAlfOogmMonitorPlugin::MemoryGood
// -----------------------------------------------------------------------------
//
void CAlfOogmMonitorPlugin::MemoryGood(TInt  aFlags)
    {
	TBool useSwRend = aFlags & KGOomUseSwRendering;
    iAlfDirectClient.EnableLowMemoryState(EFalse, useSwRend);	
    }

// -----------------------------------------------------------------------------
// Creates plugin.
// -----------------------------------------------------------------------------
//
TAny* CreatePlugin()
    {
    CAlfOogmMonitorPlugin* plugin = new CAlfOogmMonitorPlugin;
    if ( plugin )
        {
        TRAPD( err, plugin->ConstructL() );
        if ( err != KErrNone )
            {
            delete plugin;
            plugin = NULL;
            }
        }
    return plugin;
    }

// -----------------------------------------------------------------------------
// Implementation table for plugin.
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    {{0xE4B6C988}, ::CreatePlugin}
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
    return ImplementationTable;
    }

// End of file

