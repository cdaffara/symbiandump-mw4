/*
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
* Description:   Internal interface for CAlfDrawer
*
*/



#ifndef ALFDRAWERINTERNAL_H
#define ALFDRAWERINTERNAL_H

#include <e32std.h>

/**
 * Interface to get screen & parameters
 */
class MAlfDrawerScreenInterface
    {
public:
    virtual TInt GetSizeAndRotation(TSize& aSize, TInt& aRotation) = 0;
    virtual TInt ReadPixels(CFbsBitmap* aBitmap) = 0;
    };

/**
 * Internal drawer interface.
 */
NONSHARABLE_CLASS( AlfDrawerInternal )
    {
public:
    IMPORT_C static TInt CopyScreenToBitmap(
        MAlfDrawerScreenInterface* aInterface, 
        CFbsBitmap* aBitmap, 
        const TRect& aRect );
    };
    
#endif // ALFDRAWERINTERNAL_H
