/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Drop shadow handler
*
*/



#include "uiacceltk/huidropshadow.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiTextureProcessor.h"
#include "uiacceltk/HuiTextureManager.h"
#include "uiacceltk/HuiEnv.h"
#include <AknsUtils.h>
#include <e32math.h>

// internal flags
enum
    {
    EHuiDropShadowChanged           = 0x01,
    EHuiDropShadowColorValid        = 0x02
    };

// ======== LOCAL FUNCTIONS ========

NONSHARABLE_STRUCT( CHuiDropShadow::THuiDropShadowData )
    {
    THuiDropShadowData():
        iColor(KRgbBlack), 
        iColorSkinId(), 
        iColorIndex(KErrNotFound), 
        //iColorValid(EFalse),
        //iChanged( EFalse )
        iFlags(0)
            {
            }
    
    mutable TRgb iColor;
    TAknsItemID iColorSkinId;
    TInt iColorIndex;
    //mutable TBool iColorValid;
    //TBool iChanged;
    TUint8 iFlags;
    };

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CHuiDropShadow::CHuiDropShadow() : 
    iOpacity(0.6f), 
    iOffset( 2.f, 2.f ),
    iOffsetUnit( EHuiUnitPixel ), // magnitude is in iTimedOffset
    iRadius( 1.5f ),
    iScale( 1.f  )
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CHuiDropShadow::ConstructL()
    {
    iData = new (ELeave) THuiDropShadowData;
    }

// ---------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CHuiDropShadow::~CHuiDropShadow()
    {
    if ( iData )
        {
        }
    delete iData;
    }
   
// ---------------------------------------------------------------------------
// Sets the shadow offset in polar coorsinates
// ---------------------------------------------------------------------------
//
EXPORT_C void CHuiDropShadow::SetOffset( 
    TReal32 aAngle, 
    THuiMetric aDistance, 
    TInt aTransitionTime ) __SOFTFP
    {
    if ( iOffsetUnit != aDistance.iUnit && aTransitionTime )
        {
        // @todo: convert current timed value into new unit
        }
    
    // it is not that efficient to use floating point calculations here
    // but this is not called often.
    
    TReal x;
    Math::Cos( x, 2.0*KPi*aAngle/360.0 );
    x*=aDistance.iMagnitude;
    
    
    TReal y;
    Math::Sin( y, 2.0*KPi*aAngle/360.0  );
    y*=-1.0*aDistance.iMagnitude;
    
    iOffset.Set( THuiRealPoint(x,y), aTransitionTime );
    iOffsetUnit = aDistance.iUnit;
    }

// ---------------------------------------------------------------------------
// Sets color in RGB
// ---------------------------------------------------------------------------
//        
EXPORT_C void CHuiDropShadow::SetColor(
    const TRgb& aColor, 
    TInt /*aTransitionTime*/)
    {
    iData->iColor = aColor;
    iData->iColorIndex = KErrNotFound;
    iData->iFlags |= EHuiDropShadowChanged;
    }

// ---------------------------------------------------------------------------
// Sets color in S60 skin
// ---------------------------------------------------------------------------
//
EXPORT_C void CHuiDropShadow::SetColor(
    const TAknsItemID& aID,
    const TInt aIndex, 
    TInt /*aTransitionTime*/)
    {
    iData->iColorSkinId = aID;
    iData->iColorIndex = aIndex;
    iData->iFlags &= ~EHuiDropShadowColorValid;
    iData->iFlags |= EHuiDropShadowChanged;
    }

// ---------------------------------------------------------------------------
// Returns the current color RGB value
// ---------------------------------------------------------------------------
// 
TRgb CHuiDropShadow::Color() const
    {
    if ( iData->iColorIndex != KErrNotFound && !(iData->iFlags&EHuiDropShadowColorValid) )
        {
        CHuiStatic::GetCachedColor(
                iData->iColor, 
                iData->iColorSkinId, 
                iData->iColorIndex);
    
        iData->iFlags |= EHuiDropShadowColorValid; // What about skins change..
        }
    return iData->iColor;
    }

// ---------------------------------------------------------------------------
// Returns ETrue if the drops shadow parateters have changed.
// ---------------------------------------------------------------------------
//
TBool CHuiDropShadow::Changed() const
    {
    if ( iData->iFlags&EHuiDropShadowChanged )
        {
        return ETrue;
        }
    return iOpacity.Changed() || iOffset.Changed() || iRadius.Changed() || iScale.Changed();
    }

// ---------------------------------------------------------------------------
// Clears all changed flags
// ---------------------------------------------------------------------------
//
void CHuiDropShadow::ClearChanged()
    {
    iData->iFlags &= ~EHuiDropShadowChanged;    
    iOpacity.ClearChanged();
    iOffset.ClearChanged();
    iRadius.ClearChanged();
    iScale.ClearChanged();
    }

// ---------------------------------------------------------------------------
// Calculates the drawging size of the shadow.
// ---------------------------------------------------------------------------
//    
THuiRealSize CHuiDropShadow::ShadowDrawingSize( 
    const TSize& aImageDrawingSize, 
    const TSize& aShadowTextureSize ) const
    {
    // Shadow texture is usually smaller than the image texture in which it is based on.
    // Also, the content of the shadow texture is shrank by the blur dimension
    // from all the sides.
    
    // round the blur dimension already because the blur filter does it as well.
    const TInt blurDimension = HUI_ROUND_FLOAT_TO_INT( 2*iRadius.Now() );
    
    // the content area of the texture is smaller than the the texture area
    THuiRealSize shadowContentSize = aShadowTextureSize;
    shadowContentSize.iWidth -= 2.f*blurDimension;
    shadowContentSize.iHeight -= 2.f*blurDimension;
    
    // check the relation between the shadow content area and the actial drawed image
    // check possible divide-by-xero case
    // The default value (1) is arbitrary. It only is there to allow prevention of divide_by_zero
    TReal32 xMultiplier = 1;
    if ( shadowContentSize.iWidth != 0 )
        {
        xMultiplier = ((TReal32)(aImageDrawingSize.iWidth)) / ((TReal32)(shadowContentSize.iWidth));
        }
     
    TReal32 yMultiplier = 1;
    if ( shadowContentSize.iHeight != 0 )
        {
        yMultiplier = ((TReal32)(aImageDrawingSize.iHeight)) / ((TReal32)(shadowContentSize.iHeight));
        }
    
    // apply the multiplier to the shadow texture
    THuiRealSize shadowDrawingSize = aShadowTextureSize;
    shadowDrawingSize.iWidth *= xMultiplier;
    shadowDrawingSize.iHeight *= yMultiplier;
   
    // apply the scale
    shadowDrawingSize.iWidth *= iScale.Now();
    shadowDrawingSize.iHeight *= iScale.Now();
    
    return shadowDrawingSize;
    }

// ---------------------------------------------------------------------------
// Returns ETrue if the shadow is visible
// ---------------------------------------------------------------------------
// 
TBool CHuiDropShadow::IsShadowVisible() const
    {
    return !HuiUtil::RealCompare( iScale.Now(), 0.f ) &&
           !HuiUtil::RealCompare( iOpacity.Now(), 0.f );
    }

// ---------------------------------------------------------------------------
// Gets the CHuiTexture from the MHuiTexture is applicable
// ---------------------------------------------------------------------------
//     
CHuiTexture* CHuiDropShadow::ShadowedTextureInstance( const MHuiTexture& aTextureIf )
    {
    CHuiTexture* resultTexture = NULL;
    
    const MHuiShadowedTexture* constShadowedTexture = aTextureIf.ShadowedTexture();
    if ( constShadowedTexture )
        {
        MHuiShadowedTexture* nonConstshadowedTexture = const_cast<MHuiShadowedTexture*>(constShadowedTexture);
        resultTexture = static_cast<CHuiTexture*>( nonConstshadowedTexture );
        }
    return resultTexture;
    }

// ---------------------------------------------------------------------------
// Returns shadow distance in pixels
// ---------------------------------------------------------------------------
//     
THuiRealPoint CHuiDropShadow::DistanceInPixels( const CHuiVisual& aReferenceVisual ) const
    {
    return aReferenceVisual.MetricToPixels( 
        THuiXYMetric( 
            THuiMetric( 
                iOffset.iX.Now(), 
                iOffsetUnit ), 
            THuiMetric( 
                iOffset.iY.Now(), 
                iOffsetUnit ) 
            ) 
        );
    }

// ---------------------------------------------------------------------------
// Returns shadow area
// ---------------------------------------------------------------------------
// 
THuiRealRect CHuiDropShadow::ShadowDrawingRealRect( 
        const TPoint& aImageDrawingPosition,
        const TSize& aImageDrawingSize, 
        const TSize& aShadowTextureSize,
        const CHuiVisual& aReferenceVisual  ) const
    {
    const THuiRealSize shadowDrawingSize = ShadowDrawingSize( aImageDrawingSize, aShadowTextureSize );
    THuiRealPoint shadowDrawingPos( aImageDrawingPosition );
                        
    const THuiRealPoint distanceInPixels = DistanceInPixels( aReferenceVisual );
                
    shadowDrawingPos.iX += distanceInPixels.iX;
    shadowDrawingPos.iY += distanceInPixels.iY;
                
    shadowDrawingPos.iX -= (shadowDrawingSize.iWidth-aImageDrawingSize.iWidth )/2.f;
    shadowDrawingPos.iY -= (shadowDrawingSize.iHeight-aImageDrawingSize.iHeight )/2.f;
            
    return THuiRealRect( shadowDrawingPos, shadowDrawingSize );
    }

// ---------------------------------------------------------------------------
// Returns shadow area
// ---------------------------------------------------------------------------
//
TRect CHuiDropShadow::ShadowDrawingTRect( 
        const TPoint& aImageDrawingPosition,
        const TSize& aImageDrawingSize, 
        const TSize& aShadowTextureSize,
        const CHuiVisual& aReferenceVisual  ) const
    {
    const THuiRealRect realRect = ShadowDrawingRealRect( 
        aImageDrawingPosition, 
        aImageDrawingSize, 
        aShadowTextureSize, 
        aReferenceVisual );
        
    return TRect( HUI_ROUND_FLOAT_TO_INT( realRect.iTl.iX ),
                  HUI_ROUND_FLOAT_TO_INT( realRect.iTl.iY ),
                  HUI_ROUND_FLOAT_TO_INT( realRect.iBr.iX ),
                  HUI_ROUND_FLOAT_TO_INT( realRect.iBr.iY ) );
    }
