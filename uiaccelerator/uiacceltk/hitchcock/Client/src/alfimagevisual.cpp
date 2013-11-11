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
* Description:   Image visual
*
*/



#include "alf/alfimagevisual.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfimage.h"
#include "alf/alfgencomponent.h"
#include "alf/alftexture.h" 
#include "alf/alfconstants.h"
#include "alflogger.h"
#include "alf/alfimageloaderutil.h"
#include "alfskinnableimage.h"
#include "alf/alfdropshadow.h"

#include <uiacceltk/HuiUtil.h>

// ======== LOCAL FUNCTIONS ========
#define FLOAT_COMPARE(a, b) (Abs(a-b)<0.0001f)

// Private data.
struct CAlfImageVisual::TImageVisualPrivateData
    {
    CAlfSkinnableImage* iImage;
    CAlfSkinnableImage* iSecondaryImage;    
    CAlfImageVisual::TScaleMode iScaleMode;
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfImageVisual::CAlfImageVisual()
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::ConstructL(CAlfControl& aOwner)
    {
    CAlfVisual::ConstructL(aOwner);    
    iImageVisualData = new (ELeave) TImageVisualPrivateData;
    iImageVisualData->iImage = NULL;
    iImageVisualData->iSecondaryImage = NULL;
    
    iImageVisualData->iImage = new (ELeave) CAlfSkinnableImage(&aOwner.Env());
    iImageVisualData->iSecondaryImage = new (ELeave) CAlfSkinnableImage(&aOwner.Env());    
    iImageVisualData->iScaleMode = EScaleFit;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfImageVisual::~CAlfImageVisual()
    {
    if (iImageVisualData)
        {
        delete iImageVisualData->iImage;
        iImageVisualData->iImage = NULL,
        
        delete iImageVisualData->iSecondaryImage;
        iImageVisualData->iSecondaryImage = NULL;        
        }
    
    delete iImageVisualData;
    iImageVisualData = NULL;
    }


// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfImageVisual* CAlfImageVisual::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout)
    {
    CAlfImageVisual* image = STATIC_CAST(CAlfImageVisual*,
        aOwnerControl.AppendVisualL(EAlfVisualTypeImage, aParentLayout));
    return image;
    }

// ---------------------------------------------------------------------------
// Sets scale mode.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetScaleMode(CAlfImageVisual::TScaleMode aScaleMode)
    {
    TPckgC<CAlfImageVisual::TScaleMode> buf(aScaleMode);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetScaleMode, buf);    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetScaleMode ignore error %d", err )
        }
    else
        {
        iImageVisualData->iScaleMode = aScaleMode;
        }
    }
    
// ---------------------------------------------------------------------------
// Gets scale mode.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfImageVisual::TScaleMode CAlfImageVisual::ScaleMode()
    {
    CAlfImageVisual::TScaleMode ret = EScaleFit;
    if( iImageVisualData )
        {
        ret = iImageVisualData->iScaleMode;
        }            
    else
        {
        __ALFLOGSTRING( "CAlfImageVisual::ScaleMode Ignore ScaleMode iImageVisualData is NULL" )
        }
    return ret;
    }
// ---------------------------------------------------------------------------
// Sets image.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetImage(const TAlfImage& aImage)
    {    
    iImageVisualData->iImage->SetImage(aImage); // This texturizes skin graphics if needed

    TAlfImageParams params(iImageVisualData->iImage->Image());

    TPckgC<TAlfImageParams> buf(params);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetImage, buf);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetImage ignore error %d", err )
        }
        
    }

// ---------------------------------------------------------------------------
// Sets secondary image.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetSecondaryImage(const TAlfImage& aImage)
    {        
    iImageVisualData->iSecondaryImage->SetImage(aImage); // This texturizes skin graphics if needed

    TAlfImageParams params(iImageVisualData->iSecondaryImage->Image());

    TPckgC<TAlfImageParams> buf(params);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetSecondaryImage, buf);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetSecondaryImage ignore error %d", err )
        }
        
    }

// ---------------------------------------------------------------------------
// Sets secondary image alpha value.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetSecondaryAlpha(const TAlfTimedValue& aAlpha)
    {
    TPckgC<TAlfTimedValue> alphaPckg(aAlpha);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetSecondaryAlpha, alphaPckg );    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetSecondaryAlpha ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// Gets image.
// ---------------------------------------------------------------------------
//
EXPORT_C const TAlfImage& CAlfImageVisual::Image() const
    {
    return iImageVisualData->iImage->Image();    
    }

// ---------------------------------------------------------------------------
// Gets secondary image.
// ---------------------------------------------------------------------------
//
EXPORT_C const TAlfImage& CAlfImageVisual::SecondaryImage() const
    {
    return iImageVisualData->iSecondaryImage->Image();            
    }

// ---------------------------------------------------------------------------
// Sets color.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetColor(const TRgb& aColor)
    {
    TPckgC<TRgb> buf(aColor);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetColor, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetColor ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// Sets colormode.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetColorMode(CAlfImageVisual::TColorMode aColorMode, 
    TReal32 aParam) __SOFTFP
    {
    TIntTReal params(aColorMode, aParam);
    TPckg<TIntTReal> buf(params);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetColorMode, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetColorMode ignore error %d", err )
        }
    }    
    
// ---------------------------------------------------------------------------
// Sets stretch.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetStretch(TInt aTop, TInt aBottom)
    {
    TInt2 params(aTop, aBottom);
    TPckg<TInt2> buf(params);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetStretch, buf );    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetStretch ignore error %d", err )
        }
    }
    
// ---------------------------------------------------------------------------
// Sets stretchmode.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetStretchMode(CAlfGc::TStretchMode aStretchMode)
    {
    TPckg<CAlfGc::TStretchMode> buf(aStretchMode);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetStretchMode, buf );    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetStretchMode ignore error %d", err )
        }
    }
    
// ---------------------------------------------------------------------------
// Sets drop shadow. (deprecated)
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetDropShadow(TInt aShadowDistance)
    {
    if ( aShadowDistance == 0 )
        {
        TRAP_IGNORE( EnableDropShadowL(EFalse) );
        return;
        }
        
    TRAPD( err, EnableDropShadowL() );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetDropShadow ignore error %d", err )
        return;
        }
        
    DropShadowHandler()->SetOffset( TAlfTimedPoint( aShadowDistance, aShadowDistance ) );
    }

// ---------------------------------------------------------------------------
// Sets drop shadow.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetDropShadow(const TAlfMetric& aShadowDistance)
    {
    SetDropShadow(TAlfXYMetric(aShadowDistance));
    }

// ---------------------------------------------------------------------------
// Sets drop shadow.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetDropShadow(const TAlfXYMetric& aShadowDistance)
    {
    if ( FLOAT_COMPARE(aShadowDistance.iX.iMagnitude, 0.f ) && 
         FLOAT_COMPARE(aShadowDistance.iY.iMagnitude, 0.f ) )
        {
        TRAP_IGNORE( EnableDropShadowL(EFalse) );
        return;
        }
    
    TRAPD( err, EnableDropShadowL() );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetDropShadow ignore error %d", err )
        return;
        }
        
    DropShadowHandler()->SetOffset( 
        TAlfTimedPoint( aShadowDistance.iX.iMagnitude,aShadowDistance.iY.iMagnitude ),
        aShadowDistance.iX.iUnit ); // assume that the unit for x and y are the same
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetTurnAngle(const TAlfTimedValue& aAngle)
    {
    TPckgC<TAlfTimedValue> pckg(aAngle);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetTurnAngle, pckg );            
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetTurnAngle ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetScale(const TAlfTimedValue& aScale)
    {
    TPckgC<TAlfTimedValue> pckg(aScale);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetScale, pckg );                    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetScale ignore error %d", err )
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageVisual::SetOffset(const TAlfTimedPoint& aOffset)
    {
    TPckgC<TAlfTimedPoint> pckg(aOffset);

    TInt err = Comms()->DoCmdNoReply(EAlfImageVisualSetOffset, pckg );                    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SetOffset ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C TAlfTimedValue CAlfImageVisual::SecondaryAlpha()
    {
    TAlfTimedValue value;
    TPckg<TAlfTimedValue> buf(value);
    TInt err = Comms()->DoSynchronousCmd( EAlfImageVisualSecondaryAlpha, 
        KNullDesC8, 
        buf);    
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfImageVisual::SecondaryAlpha panic error %d", err )
        USER_INVARIANT();
        }
    
    return value;            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfTimedValue CAlfImageVisual::TurnAngle()
    {
    TAlfTimedValue value;
    TPckg<TAlfTimedValue> buf(value);
    TInt err = Comms()->DoSynchronousCmd( EAlfImageVisualTurnAngle, 
        KNullDesC8, 
        buf);    
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfImageVisual::TurnAngle panic error %d", err )
        USER_INVARIANT();
        }
    
    return value;                    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfTimedValue CAlfImageVisual::Scale()
    {
    TAlfTimedValue value;
    TPckg<TAlfTimedValue> buf(value);
    TInt err = Comms()->DoSynchronousCmd( EAlfImageVisualScale, 
        KNullDesC8, 
        buf);    
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfImageVisual::Scale panic error %d", err )
        USER_INVARIANT();
        }
    
    return value;                            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfTimedPoint CAlfImageVisual::Offset()
    {
    TAlfTimedPoint value;
    TPckg<TAlfTimedPoint> buf(value);
    TInt err = Comms()->DoSynchronousCmd( EAlfImageVisualOffset, 
        KNullDesC8, 
        buf);    
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfImageVisual::Offset panic error %d", err )
        USER_INVARIANT();
        }
    
    return value;                                    
    }
    
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfImageVisual::RemoveAndDestroyAllD()
    {
    CAlfVisual::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfImageVisual::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfVisual::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfImageVisual::FindTag(const TDesC8& aTag)
    {
    return CAlfVisual::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfImageVisual::DoRemoveAndDestroyAllD()
    {
    CAlfVisual::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfImageVisual::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfVisual::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    
