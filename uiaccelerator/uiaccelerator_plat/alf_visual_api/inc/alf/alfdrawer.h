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
* Description:   
*
*/
#ifndef __ALFDRAWER_H__
#define __ALFDRAWER_H__

#include <e32base.h>

class CWsScreenDevice;
class CFbsBitmap;

/**
 * This provides additional alf drawing synchronization.
 * 
 * This class requires application environment (CCoeEnv) to exist.
 */
NONSHARABLE_CLASS( CAlfDrawer ) : public CBase
    {
public:
    /**
     * Creates new CAlfDrawer instance.
     * @return newly created instance.
     */
    IMPORT_C static CAlfDrawer* NewL();
    
    /**
     * Destructor.
     */
    IMPORT_C ~CAlfDrawer();

public:
    /**
     * Finishes drawing. 
     * This will perform similar operation as RWsSession::Finish
     * and then wait for all pending drawing to be finished.
     * @return error code, KErrNone upon success.
     */
    IMPORT_C TInt Finish();

    /**
     * Fallback copy screen to bitmap. This copies pixels from UI surface.
     * This is similar to @c CWsScreenDevice::CopyScreenToBitmap. That method
     * may use GPU memory and so cannot be guaranteed to succeed always.
     * 
     * @param aDevice screendevice.
     * @param aBitmap bitmap to which pixels ought to be copied.
     * @param aRect rect to copy.
     * @return error code.
     */
    IMPORT_C static TInt FallbackCopyScreenToBitmap(
        CWsScreenDevice& aDevice,
        CFbsBitmap* aBitmap, 
        const TRect& aRect);

private:
    CAlfDrawer();
    void ConstructL();

private:
    struct TAlfDrawerData;
    TAlfDrawerData* iData;
    };

#endif // __ALFDRAWER_H__
