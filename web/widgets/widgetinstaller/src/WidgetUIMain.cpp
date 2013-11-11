/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This file contains code to match the widget installer ui implementation to 
*                KSWInstWidgetImplementationUid.
*
*                This class implements the ECom SWInstUIPluginAPI interface
*                for midlet installation.
*
*/

#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "WidgetInstallerUi.h"
#include "SWInstWidgetUid.h"

using namespace SwiUI;

// CONSTANTS

const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KSWInstWidgetImplementationUid,
                                CWidgetInstallerUI::NewL )
    };

// ========================== OTHER EXPORTED FUNCTIONS =========================

// ============================================================================
// ImplementationGroupProxy
// Returns the filters implemented in this DLL
// Returns: The filters implemented in this DLL
//
// @since 3.1
// ============================================================================
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( KImplementationTable ) / sizeof( TImplementationProxy );
    return KImplementationTable;
    }

//  End of File
