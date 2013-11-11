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
* Description:   Gradient brush
*
*/



#include "alf/alfgradientbrush.h"
#include "alf/alfconstants.h"
#include "alf/alfgencomponent.h"
#include "alf/alfimage.h"
#include "alf/alftexture.h"
#include "alflogger.h"
#include "alf/alfenv.h"
#include "alfskinnableimage.h"

#include <uiacceltk/HuiUtil.h>

struct CAlfGradientBrush::TGradientBrushPrivateData
    {
    CAlfSkinnableImage* iImage;
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfGradientBrush* CAlfGradientBrush::NewL( CAlfEnv& aEnv )
    {
    CAlfGradientBrush* self = CAlfGradientBrush::NewLC( aEnv );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfGradientBrush* CAlfGradientBrush::NewLC( CAlfEnv& aEnv )
    {
    CAlfGradientBrush* self = new( ELeave ) CAlfGradientBrush;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfGradientBrush::CAlfGradientBrush()
    {
    }
    
// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CAlfGradientBrush::ConstructL( CAlfEnv& aEnv )
    {
    CAlfBrush::ConstructL(
        aEnv,
        EAlfGradientBrushCreate,
        0,
        KNullDesC8()
        );
        
    iGradientBrushData = new (ELeave) TGradientBrushPrivateData;
    iGradientBrushData->iImage = NULL;
    iGradientBrushData->iImage = new (ELeave) CAlfSkinnableImage(&aEnv);       
    
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfGradientBrush::~CAlfGradientBrush()
    {
    if ( iGradientBrushData )
        {
        delete iGradientBrushData->iImage;
        iGradientBrushData->iImage = NULL;
        }
    delete iGradientBrushData;
    iGradientBrushData = NULL;
    }

// ---------------------------------------------------------------------------
// Set gradient direction
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfGradientBrush::SetDirection(TDirection aDirection)
    {
    TPckgC<TDirection> buf(aDirection);

    TInt err = Comms()->DoCmdNoReply( EAlfGradientBrushSetDirection, buf );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGradientBrush::SetDirection panic error %d", err )
        USER_INVARIANT();
        }
    }
   
// ---------------------------------------------------------------------------
// Set gradient base color
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfGradientBrush::SetColor(const TRgb& aColor, TReal32 aOpacity ) __SOFTFP
    {
    TAlfGradientBrushSetColorParams params;
    params.iColor = aColor;
    params.iOpacity = aOpacity;
    
    TPckgC<TAlfGradientBrushSetColorParams> buf(params);

    TInt err = Comms()->DoCmdNoReply( EAlfGradientBrushSetColor, buf );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGradientBrush::SetColor panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Set gradient image
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfGradientBrush::SetImage(const TAlfImage& aImage)
    {      
    iGradientBrushData->iImage->SetImage(aImage); // This texturizes skin graphics if needed

    TAlfImageParams params(iGradientBrushData->iImage->Image());
    
    TPckgC<TAlfImageParams> buf(params);

    TInt err = Comms()->DoCmdNoReply(EAlfGradientBrushSetImage, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGradientBrush::SetImage ignore error %d", err )
        }
    }
  
// ---------------------------------------------------------------------------
// Return color at index
// ---------------------------------------------------------------------------
//  
EXPORT_C TRgb CAlfGradientBrush::Color(TInt aIndex) const
    {
    TPckgC<TInt> inBuf(aIndex);
    TRgb result;
    TPckg<TRgb> outBuf(result);
    TInt err = Comms()->DoSynchronousCmd( EAlfGradientBrushColor, inBuf, outBuf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGradientBrush::Color panic error %d", err )
        USER_INVARIANT();
        }
        
    return result;
    }
  
// ---------------------------------------------------------------------------
// Return opacity at index
// ---------------------------------------------------------------------------
//  
EXPORT_C TReal32 CAlfGradientBrush::Opacity(TInt aIndex) const
    {
    TPckgC<TInt> inBuf(aIndex);
    TReal32 result = 1.f;
    TPckg<TReal32> outBuf(result);
    TInt err = Comms()->DoSynchronousCmd( EAlfGradientBrushOpacity, inBuf, outBuf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGradientBrush::Opacity panic error %d", err )
        USER_INVARIANT();
        }
        
    return result;
    }
 
// ---------------------------------------------------------------------------
// Return image
// ---------------------------------------------------------------------------
//   
EXPORT_C const TAlfImage& CAlfGradientBrush::Image() const
    {
    return iGradientBrushData->iImage->Image();
    }

// ---------------------------------------------------------------------------
// Add color bar to gradient
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfGradientBrush::AppendColorL(
        TReal32 aPosition, 
        const TRgb& aColor,
        TReal32 aOpacity ) __SOFTFP
    {
    TAlfGradientBrushAppendColorParams params;
    params.iPosition = aPosition;
    params.iColor = aColor;
    params.iOpacity = aOpacity;
    
    TPckgC<TAlfGradientBrushAppendColorParams> buf(params);
    TBuf8<1> dum;
    TInt err = Comms()->DoSynchronousCmd( EAlfGradientBrushAppendColor, buf, dum);

    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGradientBrush::AppendColorL leave error %d", err )
        User::Leave( err );
        }
    }

