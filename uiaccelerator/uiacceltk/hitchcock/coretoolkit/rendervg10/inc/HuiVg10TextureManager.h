/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#ifndef __HUIVG10TEXTUREMANAGER_H__
#define __HUIVG10TEXTUREMANAGER_H__


#include "uiacceltk/HuiTextureManager.h"

enum TBitmapCopyMode
    {
    /** Bitmap handle can be duplicated, no data copying takes place */
    EAllowDuplication,
    /** Always create a new bitmap copy */
    EAlwaysCopy,
    };


/**
 * Texture manager specialized for OpenGL ES.
 */
NONSHARABLE_CLASS(CHuiVg10TextureManager) : public CHuiTextureManager
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructs a new texture manager instance.
     */
    static CHuiVg10TextureManager* NewL(CHuiEnv& aEnv);
    
    /**
     * Constructs a new texture manager instance and leaves it on 
     * the cleanup stack.
     */
    static CHuiVg10TextureManager* NewLC(CHuiEnv& aEnv);

    /**
     * Destructor.
     */
    virtual ~CHuiVg10TextureManager();

    /**
     *  Bitmap color mode conversion.
     *
     *  @param aBitmap      Source bitmap to convert.
     *  @param aDisplayMode Target display mode.
     *  @param aCopyMode    Desired copy behavior.
     *  @return a reference to a bitmap with the desired display mode. DOES NOT transfer the ownership!
     */
    CFbsBitmap& ConvertBitmapL(const CFbsBitmap& aBitmap, TDisplayMode aDisplayMode, TBitmapCopyMode aCopyMode = EAllowDuplication, TSize aNewSize = TSize(0,0));
    
protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    CHuiVg10TextureManager(CHuiEnv& aEnv);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

private:

    // Member variables needed for bitmap color mode conversion
    CFbsBitmap*         iBitmap16MA;
    CFbsBitmap*         iBitmap16MU;
    CFbsBitmap*         iBitmap64K;
    CFbsBitmap*         iBitmap256Gray;
    
    CFbsBitmapDevice*   iBitmapDevice16MA;
    CFbsBitmapDevice*   iBitmapDevice16MU;
    CFbsBitmapDevice*   iBitmapDevice64K;
    CFbsBitmapDevice*   iBitmapDevice256Gray;
    
    CFbsBitGc*          iBitGc16MA;
    CFbsBitGc*          iBitGc16MU;
    CFbsBitGc*          iBitGc64K;
    CFbsBitGc*          iBitGc256Gray;
    };

#endif  // __HUIVG10TEXTUREMANAGER_H__
