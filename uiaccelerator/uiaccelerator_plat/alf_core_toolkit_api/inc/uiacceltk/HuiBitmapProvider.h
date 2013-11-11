/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of MHuiBitmapProvider, an interface that provides 
*                a bitmap and its alpha mask when requested. 
*
*/



#ifndef __HUIBITMAPPROVIDER_H__
#define __HUIBITMAPPROVIDER_H__

class CFbsBitmap;

/** 
 * MHuiBitmapProvider is an interface that provides a bitmap and its alpha 
 * mask when requested.
 * 
 * By storing MHuiBitmapProviders in the CHuiTextureManager we can delete and 
 * regenerate the textures used by the toolkit as needed - enabling improved 
 * use of memory resource, such as when the application goes to background.
 * In this case, when we want to restore all the textures, we may just recall 
 * the providers to reload the textures. 
 *
 * Typically, control classes would implement the ProvideBitmapL method of this
 * class, which would handle all the loading or generation needed to produce the
 * bitmaps stored by the TextureManager.
 * 
 * A provider should exist as long as the associated texture is stored within
 * the texture manager.
 */
class MHuiBitmapProvider 
    {
public:

    IMPORT_C virtual ~MHuiBitmapProvider();
            
    /**
     * Provide a bitmap and mask from the given UID.
     * This method should create a bitmap and its mask, usually by loading it from disk.
     *
     * @param aId      TextureManager id of this bitmap, may be used for extracting
     *                 an icon resource.
     * @param aBitmap  Output parameter for the provider: On return, has to store 
     *                 a pointer to the generated bitmap. Ownership transferred to caller.
	 *                 The support for input CFbsBitmap formats varies between 
	 *                 different renderers. See \ref cfbsbitmapsupport 
	 *                 Table describing renderer bitmap support.      
	 * @param aMaskBitmap  
	 *                 Output parameter for the provider: On return, has to store 
     *                 a pointer to the generated alpha mask or <code>NULL</code> 
     * 				   if there is no mask. Ownership of the bitmap is transferred 
     *                 to the caller.
     * 
     * @see CHuiTextureManager::CreateTextureL()
     */
     virtual void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap) = 0;
    
    };


#endif // __HUIBITMAPPROVIDER_H__
