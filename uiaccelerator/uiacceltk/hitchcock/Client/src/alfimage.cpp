/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Image
*  Version     : %version: 20.1.1 %
*
*/



#include "alf/alfimage.h"
#include <alf/alftexture.h>
#include <alf/alfconstants.h>
#include "alf/alfimageloaderutil.h"
#include "alf/alftexturemanager.h"
#include "alflogger.h"
#include <uiacceltk/HuiUtil.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfImage::TAlfImage()
    : iTexture( NULL ) 
    {
    iTl = TAlfTimedPoint(0.f, 0.f);    
    // Target set because of serverside THuiImage      
    iTl.SetTarget(0.f,0); 

    iBr = TAlfTimedPoint(0.f, 0.f);        
    // Target set because of serverside THuiImage      
    iBr.SetTarget(0.f,0); 

    InitSkinData();
    }

// ---------------------------------------------------------------------------
// Constructor with the texture
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfImage::TAlfImage(const CAlfTexture& aTexture, 
                       TReal32 aTlX, TReal32 aTlY,
                       TReal32 aBrX, TReal32 aBrY) __SOFTFP
    :  iTexture (&aTexture)
    {
    iTl = TAlfTimedPoint(aTlX, aTlY);    
    // Target set because of serverside THuiImage      
    iTl.SetTarget(TAlfRealPoint(aTlX,aTlY)); 
    
    iBr = TAlfTimedPoint(aBrX, aBrY);
    // Target set because of serverside THuiImage                  
    iBr.SetTarget(TAlfRealPoint(aBrX,aBrY)); 

    InitSkinData();
    }



// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfImage::~TAlfImage()
    {
    }

// ---------------------------------------------------------------------------
// Gets the texture
// ---------------------------------------------------------------------------
//
EXPORT_C const CAlfTexture& TAlfImage::Texture() const 
    {
    __ASSERT_ALWAYS( (TInt)iTexture, USER_INVARIANT());
	return *iTexture;
    }

// ---------------------------------------------------------------------------
// Returns TL texture coordinate
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfRealPoint TAlfImage::TopLeft() const
    {
    TAlfRealPoint retVal;
    retVal.iX = iTl.iX.ValueNow();
    retVal.iY = iTl.iY.ValueNow();    
    return retVal;    
    }

// ---------------------------------------------------------------------------
// Returns BR texture coordinate
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfRealPoint TAlfImage::BottomRight() const
    {
    TAlfRealPoint retVal;
    retVal.iX = iBr.iX.ValueNow();
    retVal.iY = iBr.iY.ValueNow();    
    return retVal;            
    } 

// ---------------------------------------------------------------------------
// Returns ETrue if the texture has been set,
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TAlfImage::HasTexture() const
    {
    return (iTexture != NULL);        
    }

// ---------------------------------------------------------------------------
// Sets the texture
// ---------------------------------------------------------------------------
//
EXPORT_C void TAlfImage::SetTexture(CAlfTexture& aTexture)
    {
    InitSkinData();
    iTexture = &aTexture;        
    }

// ---------------------------------------------------------------------------
// Moves the texture
// ---------------------------------------------------------------------------
//
EXPORT_C void TAlfImage::Translate(TReal32 aDx, TReal32 aDy, TInt aTransitionTime) __SOFTFP
    {
    iTl.iX.SetTarget(iTl.iX.Target() + aDx, aTransitionTime);
    iTl.iY.SetTarget(iTl.iY.Target() + aDy, aTransitionTime);
    iBr.iX.SetTarget(iBr.iX.Target() + aDx, aTransitionTime);
    iBr.iY.SetTarget(iBr.iY.Target() + aDy, aTransitionTime);        
    }

// ---------------------------------------------------------------------------
// Scales the texture
// ---------------------------------------------------------------------------
//
EXPORT_C void TAlfImage::Scale(TReal32 aScaleX, TReal32 aScaleY, TInt aTransitionTime) __SOFTFP
    {
    iTl.iX.SetTarget(iTl.iX.Target() * aScaleX, aTransitionTime);
    iTl.iY.SetTarget(iTl.iY.Target() * aScaleY, aTransitionTime);
    iBr.iX.SetTarget(iBr.iX.Target() * aScaleX, aTransitionTime);
    iBr.iY.SetTarget(iBr.iY.Target() * aScaleY, aTransitionTime);        
    }

// ---------------------------------------------------------------------------
// Sets the texture coordinates
// ---------------------------------------------------------------------------
//
EXPORT_C void TAlfImage::SetTexCoords(TReal32 aTlX, TReal32 aTlY,
                                  TReal32 aBrX, TReal32 aBrY,
                                  TInt aTransitionTime) __SOFTFP
    {
    iTl.iX.SetTarget(aTlX, aTransitionTime);
    iTl.iY.SetTarget(aTlY, aTransitionTime);
    iBr.iX.SetTarget(aBrX, aTransitionTime);
    iBr.iY.SetTarget(aBrY, aTransitionTime);                
    }    
    
// ---------------------------------------------------------------------------
// Returns TL texture coordinates
// ---------------------------------------------------------------------------
//
TAlfTimedPoint TAlfImage::TimedTopLeft() const
    {
    return iTl;    
    }

// ---------------------------------------------------------------------------
// Returns BR texture coordinates
// ---------------------------------------------------------------------------
//
TAlfTimedPoint TAlfImage::TimedBottomRight() const
    {
    return iBr;            
    }
    
  
// ---------------------------------------------------------------------------
// Inits skin data
// ---------------------------------------------------------------------------
//
void TAlfImage::InitSkinData()
    {
    TAknsItemID id;
    id.Set(0,0);
    iSkinID = id;
    iAppIconUid = TUid::Uid(0);    
    iAppIconType = EAknsAppIconTypeList;
    iPreferredSize = TSize(0,0);    
    iPreferredScaleMode = EAspectRatioPreserved;
    iFallbackFileName = NULL;
 	iFallbackBitmapId = -1;
    iFallbackMaskId = -1;
    iPreferredTextureId = KAlfAutoGeneratedTextureId;
    iPreferredTextureManager = NULL;
    iFlags = EAlfTextureFlagSkinContent;
    iSkinnableImage = EFalse;
    }
    
// ---------------------------------------------------------------------------
// Constructor with skin data
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfImage::TAlfImage(TAknsItemID aID,
                              TSize aPreferredSize,
                              TScaleMode aPreferredScaleMode, 
                              const TDesC* aFallbackFileName, 
                              TInt aFallbackBitmapId, 
                              TInt aFallbackMaskId,
                              TReal32 aTlX, TReal32 aTlY,
                              TReal32 aBrX, TReal32 aBrY,
                              TAlfTextureFlags aFlags,
                              TInt aPreferredTextureId,
                              CAlfTextureManager* aPreferredTextureManager) __SOFTFP
    {
    InitSkinData();
    iSkinnableImage = ETrue;
    
    iSkinID = aID;
    iPreferredSize = aPreferredSize;
    iPreferredScaleMode = aPreferredScaleMode;
    
    iFallbackFileName = aFallbackFileName;
    iFallbackBitmapId = aFallbackBitmapId;
    iFallbackMaskId = aFallbackMaskId;
    
    iTl = TAlfTimedPoint(aTlX, aTlY);    
    // Target set because of serverside THuiImage      
    iTl.SetTarget(TAlfRealPoint(aTlX,aTlY)); 
    
    iBr = TAlfTimedPoint(aBrX, aBrY);
    // Target set because of serverside THuiImage                  
    iBr.SetTarget(TAlfRealPoint(aBrX,aBrY)); 

    iFlags = aFlags;
    iPreferredTextureId = aPreferredTextureId;
    iPreferredTextureManager = aPreferredTextureManager;
    
    iTexture = NULL;
    }

// ---------------------------------------------------------------------------
// Constructor with app icon
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfImage::TAlfImage(TUid aAppIconUid, 
                              TAknsAppIconType aAppIconType,
                              TSize aPreferredSize,
                              TScaleMode aPreferredScaleMode, 
                              TReal32 aTlX, TReal32 aTlY,
                              TReal32 aBrX, TReal32 aBrY,
                              TAlfTextureFlags aFlags,
                              TInt aPreferredTextureId,
                              CAlfTextureManager* aPreferredTextureManager) __SOFTFP
    {
    InitSkinData();
    iSkinnableImage = ETrue;
    iAppIconUid = aAppIconUid;
    iAppIconType = aAppIconType;
    
    
    iPreferredSize = aPreferredSize;
    iPreferredScaleMode = aPreferredScaleMode;

    iTl = TAlfTimedPoint(aTlX, aTlY);    
    // Target set because of serverside THuiImage      
    iTl.SetTarget(TAlfRealPoint(aTlX,aTlY)); 
    
    iBr = TAlfTimedPoint(aBrX, aBrY);
    // Target set because of serverside THuiImage                  
    iBr.SetTarget(TAlfRealPoint(aBrX,aBrY)); 

    iFlags = aFlags;
    iPreferredTextureId = aPreferredTextureId;
    iPreferredTextureManager = aPreferredTextureManager;
    
    iTexture = NULL;
    }

// ---------------------------------------------------------------------------
// Constructor with logical name
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfImage::TAlfImage(const TDesC* /*aLogicalName*/,
                              TSize /*aPreferredSize*/,
                              TScaleMode /*aPreferredScaleMode*/, 
                              const TDesC* /*aFallbackFileName*/, 
                              TInt /*aFallbackBitmapId*/, 
                              TInt /*aFallbackMaskId*/,
                              TReal32 /*aTlX*/, TReal32 /*aTlY*/,
                              TReal32 /*aBrX*/, TReal32 /*aBrY*/,
                              TAlfTextureFlags /*aFlags*/,
                              TInt /*aPreferredTextureId*/,
                              CAlfTextureManager* /*aPreferredTextureManager*/) __SOFTFP
    {
    __ALFLOGSTRING( "TAlfImage::TAlfImage FUNCTIONALITY REMOVED. REFER TO ANTRIKSH PROJECT" );
    }

// ---------------------------------------------------------------------------
// Returns skin ID.
// ---------------------------------------------------------------------------
//
TAknsItemID TAlfImage::SkinID() const
    {
    return iSkinID;    
    }
    
// ---------------------------------------------------------------------------
// Returns preferred size.
// ---------------------------------------------------------------------------
//
TSize TAlfImage::PreferredSize() const
    {
    return iPreferredSize;  
    }
    
// ---------------------------------------------------------------------------
// Returns preferred scalemode.
// ---------------------------------------------------------------------------
//
TScaleMode TAlfImage::PreferredScaleMode() const
    {
    return iPreferredScaleMode;  
    }
    
// ---------------------------------------------------------------------------
// Returns fallback filename
// ---------------------------------------------------------------------------
//
const TDesC* TAlfImage::FallbackFileName() const
    {
    if (!iFallbackFileName)
        {
        return &KNullDesC;    
        }
    return iFallbackFileName; 
    }
    
// ---------------------------------------------------------------------------
// Returns fallback index
// ---------------------------------------------------------------------------
//
TInt TAlfImage::FallbackBitmapId() const
    {
    return iFallbackBitmapId;  
    }
    
// ---------------------------------------------------------------------------
// Returns fallback index for the mask.
// ---------------------------------------------------------------------------
//
TInt TAlfImage::FallbackMaskId() const
    {
    return iFallbackMaskId;  
    }

// ---------------------------------------------------------------------------
// Returns preferred texture manager
// ---------------------------------------------------------------------------
//
CAlfTextureManager* TAlfImage::PreferredTextureManager() const
    {
    return iPreferredTextureManager;    
    }

// ---------------------------------------------------------------------------
// Is image from a skin?
// ---------------------------------------------------------------------------
//
TBool TAlfImage::SkinnableImage() const
    {
    return iSkinnableImage;            
    }
    
// ---------------------------------------------------------------------------
// Returns texture flags
// ---------------------------------------------------------------------------
//
TAlfTextureFlags TAlfImage::TextureFlags() const
    {
    return iFlags;        
    }
    
// ---------------------------------------------------------------------------
// returns preferred texture id
// ---------------------------------------------------------------------------
//
TInt TAlfImage::PreferredTextureId() const
    {
    return iPreferredTextureId;    
    }
    
// ---------------------------------------------------------------------------
// Returns app icon UID
// ---------------------------------------------------------------------------
//
TUid TAlfImage::AppIconUid() const
    {
    return iAppIconUid;    
    }

// ---------------------------------------------------------------------------
// Returns app icon type
// ---------------------------------------------------------------------------
//
TAknsAppIconType TAlfImage::AppIconType() const
    {
    return iAppIconType;            
    }

