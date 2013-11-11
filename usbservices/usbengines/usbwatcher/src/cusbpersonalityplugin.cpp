/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Personality plugins must be inherited from this class.
*
*/


#include <cusbpersonalityplugin.h>
#include <ecom/ecom.h>

// ----------------------------------------------------------------------------
// C++ constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CUsbPersonalityPlugin::CUsbPersonalityPlugin(
        TUsbPersonalityParams& aPersonalityParams )
    : CUsbPersonality( aPersonalityParams )
    {
    }

// ----------------------------------------------------------------------------
// Factory function
// ----------------------------------------------------------------------------
//
EXPORT_C CUsbPersonalityPlugin* CUsbPersonalityPlugin::NewL( 
        TUsbPersonalityParams& aPersonalityParams, TUid aImplementationId ) 
    {
    return ( reinterpret_cast<CUsbPersonalityPlugin*>(
            REComSession::CreateImplementationL( aImplementationId, 
                    _FOFF(CUsbPersonalityPlugin, iPrivateEComUID),
                    ( TAny* ) &aPersonalityParams ) ) );
    }

// ----------------------------------------------------------------------------
// Destructor will destroy the implementation
// ----------------------------------------------------------------------------
//
EXPORT_C CUsbPersonalityPlugin::~CUsbPersonalityPlugin()
    {
    REComSession::DestroyedImplementation(iPrivateEComUID);
    }

// End of file
