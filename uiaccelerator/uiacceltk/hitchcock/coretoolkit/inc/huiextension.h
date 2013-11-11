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
* Description:   Internal extensions
*
*/



#ifndef __HUIEXTENSION_H__
#define __HUIEXTENSION_H__


#include <e32std.h>

class CFbsBitmap;

// Uid for Partial Bitmap Upload extension
// THuiTexturePartialBitmapUploadParams used as parameter.
const TUid KHuiTexturePartialBitmapUploadUid = { 0x2002C359 };

// Parameter structure for Partial Bitmap Upload extension
struct THuiTexturePartialBitmapUploadParams
    {
    // Input parameters
    
    TRect iRect;
    CFbsBitmap* iBitmap;
    
    // Output parameters
    
    TInt iErrorCode;
    };

// Uid for Query visual extension
const TUid KHuiVisualQueryUid = { 0x2002C36B };

// Parameter structure for Query visual extension
struct THuiVisualQueryParams
    {
    enum TQueryType
        {
        // iResult contains canvas flags
        EQueryCanvasFlags,
        // iResult contains positive value if external content drawing enabled, 0 otherwise.
        EQueryExternalContentDrawingEnabled,
        // iResult contains positive value if this has something to draw, 0 otherwise.
        EQueryHasDrawableContent
        };

    // Query type (in)
    TQueryType iQueryType;
        
    // Result of query (out)
    TInt iValue;
    
    // Error code (out). This should be filled with KErrNotSupported by caller and visual should modify to KErrNone.
    TInt iResult;
    };

// Uid for Copy Screen To Bitmap extension
const TUid KHuiGcCopyScreenToBitmap = { 0x2002E67B };

// Parameter structure for Copy Screen To Bitmap extension
struct THuiGcCopyScreenToBitmapParams
    {
    // Input parameters
    
    CFbsBitmap* iBitmap;
    
    // Output parameters
    
    TInt iErrorCode;
    };

// Uid for Release Cached Textures extension for CHuiSkin.
const TUid KHuiSkinReleaseCachedTextures = { 0x2002E6D4 };

#endif // __HUIEXTENSION_H__
