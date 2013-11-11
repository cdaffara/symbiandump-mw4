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
* Description:   Defines CHuiAnimatedTexture base class, which can be used as 
*                a base class for application-specific animated textures.
*
*/



#ifndef __HUIANIMATEDTEXTURE_H__
#define __HUIANIMATEDTEXTURE_H__


#include <uiacceltk/HuiTexture.h>
#include <uiacceltk/HuiTimeObserver.h>


/* Forward declarations. */
class CHuiTextureManager;


/**
 * Animated textures are informed of the progress of time. Before a display
 * refresh occurs, the texture manager will tell all animated textures to
 * update themselves.
 *
 * CHuiAnimatedTexture can be used as a base class for application-specific
 * animated texture classes.
 */
class CHuiAnimatedTexture : public CBase,
                            public MHuiTexture,
                            public MHuiSegmentedTexture,
                            public MHuiTimeObserver
    {
public:

    /**
     * Destructor.
     */
    IMPORT_C ~CHuiAnimatedTexture();


public: // new methods

    /**
     * Returns the texture object associated with this animated texture.
     */
    IMPORT_C CHuiTexture& Texture();

    /**
     * Called to update the texture when time progresses.
     */
    virtual void AdvanceTime(TReal32 aElapsedTime) __SOFTFP = 0;


public: // from MHuiTexture

    IMPORT_C MHuiSegmentedTexture* SegmentedTexture();

    IMPORT_C const MHuiSegmentedTexture* SegmentedTexture() const;
    
    IMPORT_C MHuiShadowedTexture* ShadowedTexture();
    
    IMPORT_C const MHuiShadowedTexture* ShadowedTexture() const;

    IMPORT_C TBool HasContent() const;


public: //  MHuiSegmentedTexture

    IMPORT_C TSize Size() const;
    
    IMPORT_C TInt SegmentCount() const;
    
    IMPORT_C TUint SegmentName(TInt aSegment) const;
    
    IMPORT_C TSize SegmentSize(TInt aSegment) const;
    
    IMPORT_C TSize SegmentTextureSize(TInt aSegment) const;
    
    IMPORT_C TPoint SegmentOffset(TInt aSegment) const;
    
	IMPORT_C TBool TextureChanged() const;

	IMPORT_C void TextureClearChanged() const;    

protected:

    /**
     * Default constructor.
     */
    IMPORT_C CHuiAnimatedTexture(CHuiTextureManager& aManager);

    /**
     * Second-phase constructor.
     */
    IMPORT_C void BaseConstructL();
    
protected: // from MHuiTexture
    
    IMPORT_C void TextureExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

protected:

    /** Texture manager who owns this texture. */
    CHuiTextureManager& iManager;

    /** Contents of the animated texture. */
    CHuiTexture* iTexture;
private:
    TAny* iSpare;
    };


#endif // __HUIANIMATEDTEXTURE_H__
