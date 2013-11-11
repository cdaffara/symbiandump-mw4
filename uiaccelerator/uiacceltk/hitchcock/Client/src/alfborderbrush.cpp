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
* Description:   Border brush
*
*/



#include "alf/alfborderbrush.h"
#include "alf/alfenv.h"
#include "alf/alfgencomponent.h"
#include "alf/alfimage.h"
#include "alf/alftexture.h"
#include "alflogger.h"
#include "alfskinnableimage.h"
#include "alf/alfconstants.h"

#include <uiacceltk/HuiUtil.h>

struct CAlfBorderBrush::TBorderBrushPrivateData
    {
    TAlfTimedPoint iThickness;
    TAlfTimedPoint iEdgeOffset;
    TAlfTimedPoint iImageOffset;
    CAlfSkinnableImage* iImage;
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfBorderBrush::CAlfBorderBrush()
    : CAlfBrush()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfBorderBrush::ConstructL(
    CAlfEnv& aEnv,
    const TAlfXYMetric& aThickness,
    const TAlfXYMetric& aEdgeOffset)
    {
    
    TAlfCreateBorderBrushMetricParams params;
    params.iThickness = aThickness; 
    params.iEdgeOffset = aEdgeOffset;
    TPckgC<TAlfCreateBorderBrushMetricParams> paramsPckg(params);
    
    
    CAlfBrush::ConstructL(
        aEnv,
        EAlfBorderBrushCreateMetric,
        0,
        paramsPckg
        );
                
    iBorderBrushData = new (ELeave) TBorderBrushPrivateData;
    iBorderBrushData->iImage = NULL;
    iBorderBrushData->iImage = new (ELeave) CAlfSkinnableImage(&aEnv);       
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfBorderBrush::ConstructL(
    CAlfEnv& aEnv,
    TInt aThicknessWidth, 
    TInt aThicknessHeight,
    TInt aEdgeOffsetX, 
    TInt aEdgeOffsetY )
    {
    
    TAlfCreateBorderBrushParams params;
    params.iThicknessWidth = aThicknessWidth; 
    params.iThicknessHeight = aThicknessHeight;
    params.iEdgeOffsetX = aEdgeOffsetX;
    params.iEdgeOffsetY = aEdgeOffsetY;
    TPckgC<TAlfCreateBorderBrushParams> paramsPckg(params);
    
    
    CAlfBrush::ConstructL(
        aEnv,
        EAlfBorderBrushCreate,
        0,
        paramsPckg
        );
                
    iBorderBrushData = new (ELeave) TBorderBrushPrivateData;
    iBorderBrushData->iImage = NULL;
    iBorderBrushData->iImage = new (ELeave) CAlfSkinnableImage(&aEnv);       
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBorderBrush* CAlfBorderBrush::NewL(
    CAlfEnv& aEnv,
    const TAlfXYMetric& aThickness,
    const TAlfXYMetric& aEdgeOffset)
    {
    CAlfBorderBrush* self = CAlfBorderBrush::NewLC(
        aEnv,
        aThickness,
        aEdgeOffset);
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBorderBrush* CAlfBorderBrush::NewLC(
    CAlfEnv& aEnv,
    const TAlfXYMetric& aThickness,
    const TAlfXYMetric& aEdgeOffset)
    {
    CAlfBorderBrush* self = new( ELeave ) CAlfBorderBrush;
    CleanupStack::PushL( self );
    self->ConstructL(
        aEnv,
        aThickness,
        aEdgeOffset);
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBorderBrush* CAlfBorderBrush::NewL(
    CAlfEnv& aEnv,
    TInt aThicknessWidth, 
    TInt aThicknessHeight,
    TInt aEdgeOffsetX, 
    TInt aEdgeOffsetY )
    {
    CAlfBorderBrush* self = CAlfBorderBrush::NewLC(
        aEnv,
        aThicknessWidth,
        aThicknessHeight,
        aEdgeOffsetX,
        aEdgeOffsetY );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBorderBrush* CAlfBorderBrush::NewLC(
    CAlfEnv& aEnv,
    TInt aThicknessWidth, 
    TInt aThicknessHeight,
    TInt aEdgeOffsetX, 
    TInt aEdgeOffsetY)
    {
    CAlfBorderBrush* self = new( ELeave ) CAlfBorderBrush;
    CleanupStack::PushL( self );
    self->ConstructL(
        aEnv,
        aThicknessWidth,
        aThicknessHeight,
        aEdgeOffsetX,
        aEdgeOffsetY );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CAlfBorderBrush::~CAlfBorderBrush()
    {
    if (iBorderBrushData)
        {
        delete iBorderBrushData->iImage;
        iBorderBrushData->iImage = NULL;    	
        }

    delete iBorderBrushData;
    }

// ---------------------------------------------------------------------------
// Set image
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBorderBrush::SetImage(const TAlfImage& aImage)
    {    
    iBorderBrushData->iImage->SetImage(aImage); // This texturizes skin graphics if needed
    
    TAlfImageParams params(iBorderBrushData->iImage->Image());
    
    TPckgC<TAlfImageParams> buf(params);
    TInt err = Comms()->DoCmdNoReply(EAlfBorderBrushSetImage, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::SetImage ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// Get image
// ---------------------------------------------------------------------------
//    
EXPORT_C const TAlfImage& CAlfBorderBrush::Image() const
    {
    return iBorderBrushData->iImage->Image();   
    }

// ---------------------------------------------------------------------------
// Set color
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBorderBrush::SetColor(const TRgb& aColor)
    {
    // Just send to server
    TPckgC<TRgb> inBuf(aColor);
    TInt err = Comms()->DoCmdNoReply( EAlfBorderBrushSetColor, inBuf );
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::SetColor ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// Thickness
// ---------------------------------------------------------------------------
//
EXPORT_C const TAlfTimedPoint& CAlfBorderBrush::Thickness() const
    {
    TPckg<TAlfTimedPoint> buf(iBorderBrushData->iThickness);
    TInt err = Comms()->DoSynchronousCmd( EAlfBorderBrushGetThickness, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::Thickness panic error %d", err )
        USER_INVARIANT();
        }
    
    return iBorderBrushData->iThickness;
    }
  
// ---------------------------------------------------------------------------
// Sets thickness
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfBorderBrush::SetThickness(const TAlfTimedPoint& aThickness)
    {
    TPckgC<TAlfTimedPoint> buf(aThickness);
    TInt err = Comms()->DoCmdNoReply( EAlfBorderBrushSetThickness, buf);

    if ( err == KErrNone )
        {
        iBorderBrushData->iThickness = aThickness;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::SetThickness panic error %d", err )
        USER_INVARIANT();
        }
    }
   
// ---------------------------------------------------------------------------
// Edge offset
// ---------------------------------------------------------------------------
// 
EXPORT_C const TAlfTimedPoint& CAlfBorderBrush::EdgeOffset() const
    {
    TPckg<TAlfTimedPoint> buf(iBorderBrushData->iEdgeOffset);
    TInt err = Comms()->DoSynchronousCmd( EAlfBorderBrushGetEdgeOffset, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::EdgeOffset panic error %d", err )
        USER_INVARIANT();
        }
    
    return iBorderBrushData->iEdgeOffset;
    }
    
// ---------------------------------------------------------------------------
// Set edge offset
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBorderBrush::SetEdgeOffset(const TAlfTimedPoint& aEdgeOffset)
    {
    TPckgC<TAlfTimedPoint> buf(aEdgeOffset);
    TInt err = Comms()->DoCmdNoReply( EAlfBorderBrushSetEdgeOffset, buf );

    if ( err == KErrNone )
        {
        iBorderBrushData->iEdgeOffset = aEdgeOffset;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::SetEdgeOffset panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Image offset
// ---------------------------------------------------------------------------
//      
EXPORT_C const TAlfTimedPoint& CAlfBorderBrush::ImageOffset() const
    {
    TPckg<TAlfTimedPoint> buf(iBorderBrushData->iImageOffset);
    TInt err = Comms()->DoSynchronousCmd( EAlfBorderBrushGetImageOffset, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::ImageOffset panic error %d", err )
        USER_INVARIANT();
        }
    
    return iBorderBrushData->iImageOffset;
    }

// ---------------------------------------------------------------------------
// Sets image offset
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfBorderBrush::SetImageOffset(const TAlfTimedPoint& aImageOffset)
    {
    TPckgC<TAlfTimedPoint> buf(aImageOffset);

    TInt err = Comms()->DoCmdNoReply( EAlfBorderBrushSetImageOffset, buf );

    if ( err == KErrNone )
        {
        iBorderBrushData->iImageOffset = aImageOffset;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::SetImageOffset panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Sets image offset
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfBorderBrush::SetImageOffset(const TAlfXYMetric& aImageOffset)
    {
    TPckgC<TAlfXYMetric> buf(aImageOffset);

    TInt err = Comms()->DoCmdNoReply( EAlfBorderBrushSetImageOffsetMetric, buf );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::SetImageOffset panic error %d", err )
        USER_INVARIANT();
        }        
    }
// ---------------------------------------------------------------------------
// Sets edge offset
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfBorderBrush::SetEdgeOffset(const TAlfXYMetric& aEdgeOffset)
    {
    TPckgC<TAlfXYMetric> buf(aEdgeOffset);
    TInt err = Comms()->DoCmdNoReply( EAlfBorderBrushSetEdgeOffsetMetric, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::SetEdgeOffset panic error %d", err )
        USER_INVARIANT();
        }        
    }

// ---------------------------------------------------------------------------
// Sets thickness
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfBorderBrush::SetThickness(const TAlfXYMetric& aThickness)
    {
    TPckgC<TAlfXYMetric> buf(aThickness);
    TInt err = Comms()->DoCmdNoReply( EAlfBorderBrushSetThicknessMetric, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfBorderBrush::SetThickness panic error %d", err )
        USER_INVARIANT();
        }        
    }
