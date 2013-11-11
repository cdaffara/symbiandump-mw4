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
* Description:   Skinned image
*
*/



#ifndef C_ALFSKINNABLEIMAGE_H
#define C_ALFSKINNABLEIMAGE_H


#include <e32base.h>
#include <alf/alfimage.h>
#include <alf/alfimageloaderutil.h>

class CAlfEnv;


/**
 *  Internal class that provides texturization for a skinned TAlfImage.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfSkinnableImage ): public CBase
	{
public:
    
    /* Constructors and destructors. */
    
    /**
     * Constructor.
     *
     * @param aEnv Environment. Ownership not transferred.
     */
    CAlfSkinnableImage(CAlfEnv* aEnv);
    
    /**
     * Destructor. 
     */
    ~CAlfSkinnableImage();
    
    
    /* Methods. */
    
    /**
     * Sets the image specification and texturizes the image if it does 
     * not yet have a texture.
     *
     * @param aImage The image.
     */
    void SetImage(const TAlfImage& aImage);
    
    /**
     * Gets the image specification.
     *
     * @return The image.
     */
    TAlfImage& Image();

private:

    void CreateTextureL();    
    void DeleteTexture();
    CAlfTextureManager& ImageTextureManager(const TAlfImage& aImage);
    CAlfTextureManager& ImageTextureManager(TUid aManagerUid);

private:

    TAlfImage iImage;
    CAlfEnv* iEnv;
    TInt iSkinnedImageTextureId;
    TUid iSkinnedImageTextureManagerId;
    CAlfAutoSizeImageLoaderUtil* iSkinnedImageUtil;        
    // Skin Logical Name
    const TDesC* iSkinIdName;
	TInt iId;
	};
	
#endif // C_ALFSKINNABLEIMAGE_H
