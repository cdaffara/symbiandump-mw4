/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ECom table handling for all plugins in this dll.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <bitdev.h>
#include <implementationproxy.h>
#include <alf/AlfTransEffectPlugin.h>

#include "alftfxplugincommon.hrh"
#include "alfsystemeffects.h"
#include "alfgfxeffects.h"


// ============================ ECOM FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ImplementationTable
// Defines the plug-in implementation table for ECom.
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY( KAlfGfxImplUidValue, CAlfGfxEffects::NewFxPluginL ),
	IMPLEMENTATION_PROXY_ENTRY( KAlfTfxSystemEffectsImpUidValue, CAlfSystemEffects::NewExtensionL )
    };


// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns the plug-in implementation table to ECom.
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy ( TInt& aTableCount )
	{
	aTableCount = sizeof (ImplementationTable) / sizeof (TImplementationProxy);
	return ImplementationTable;
	}

//  End of File  
