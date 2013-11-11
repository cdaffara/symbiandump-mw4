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
* Description:  Handle dialogs needed for browser operation
*
*/



//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "BrCtlDialogsProvider.h"


// ----------------------------------------------------------------------------
// CBrCtlObjectInfo class
// Used by and defined in support of CBrCtlDialogProvider
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// CBrCtlObjectInfo default construtor
// ----------------------------------------------------------------------------
CBrCtlObjectInfo::CBrCtlObjectInfo()
    {
    // Set to reflect an unsupported MIME type, user will modify as
    // needed to reflect info from object
    iAppSupported = EFalse;
    iPluginSupported = EFalse;
	iSize.Set(NULL, 0);
	iAppName.Set(NULL, 0);
	iFileType.Set(NULL, 0);
    }

// ----------------------------------------------------------------------------
// CBrCtlObjectInfo construtor
// ----------------------------------------------------------------------------
CBrCtlObjectInfo::CBrCtlObjectInfo(TBool aAppSupported, TBool aPluginSupported,
                                   const TDesC& aSize, const TDesC& aAppName,
                                   const TDesC& aFileType)
    {
    iAppSupported = aAppSupported;
    iPluginSupported = aPluginSupported;
	iSize.Set(aSize);
	iAppName.Set(aAppName);
	iFileType.Set(aFileType);
    }

// End of File
