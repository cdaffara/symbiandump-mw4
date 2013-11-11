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
* Description:   Frame brush
*
*/



#include "alf/alfframebrush.h"
#include "alf/alfenv.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alflogger.h"
#include "alfskinnableimage.h"

const TInt KNumberOfFrameParts = 9;

// Private data structure
struct CAlfFrameBrush::TFrameBrushPrivateData
    {
    CAlfSkinnableImage* iImage;
    CAlfSkinnableImage* iFramePartImages[KNumberOfFrameParts];
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfFrameBrush::CAlfFrameBrush()
    : CAlfBrush()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfFrameBrush::ConstructL(CAlfEnv& aEnv, const TAknsItemID& aIID, 
    const TAlfXYMetric& aEdgeOffset)
    {
    iFrameBrushData = new (ELeave) TFrameBrushPrivateData;
    iFrameBrushData->iImage = NULL;
    Mem::FillZ(iFrameBrushData->iFramePartImages, (sizeof(TInt)*KNumberOfFrameParts));

    iFrameBrushData->iImage = new (ELeave) CAlfSkinnableImage(&aEnv);       
    
    for (TInt i=0; i<KNumberOfFrameParts;i++)
        {
        iFrameBrushData->iFramePartImages[i] = new (ELeave) CAlfSkinnableImage(&aEnv); 
        }
    
    TAlfFrameBrushMetricParams params;
    params.iInnerRect = TRect(0,0,0,0);
    params.iOuterRect = TRect(0,0,0,0);
    params.iFrameIID = aIID;
    params.iEdgeOffset = aEdgeOffset;
    
    TPckgC<TAlfFrameBrushMetricParams> paramsPckg(params);
        
    CAlfBrush::ConstructL(
        aEnv,
        EAlfFrameBrushCreateMetric,
        0,
        paramsPckg
        );
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfFrameBrush::ConstructL(CAlfEnv& aEnv, const TAknsItemID& aIID, 
    TInt aEdgeOffsetX, TInt aEdgeOffsetY)
    {
    iFrameBrushData = new (ELeave) TFrameBrushPrivateData;
    iFrameBrushData->iImage = NULL;
    Mem::FillZ(iFrameBrushData->iFramePartImages, (sizeof(TInt)*KNumberOfFrameParts));

    iFrameBrushData->iImage = new (ELeave) CAlfSkinnableImage(&aEnv);       

    for (TInt i=0; i<KNumberOfFrameParts;i++)
        {
        iFrameBrushData->iFramePartImages[i] = new (ELeave) CAlfSkinnableImage(&aEnv); 
        }
    
    TAlfFrameBrushParams params;
    params.iInnerRect = TRect(0,0,0,0);
    params.iOuterRect = TRect(0,0,0,0);
    params.iFrameIID = aIID;
    params.iEdgeOffsetX = aEdgeOffsetX;
    params.iEdgeOffsetY = aEdgeOffsetY;
    
    TPckgC<TAlfFrameBrushParams> paramsPckg(params);
        
    CAlfBrush::ConstructL(
        aEnv,
        EAlfFrameBrushCreate,
        0,
        paramsPckg
        );
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfFrameBrush::ConstructL(CAlfEnv& aEnv, const TAlfImage& aImage, const TAlfXYMetric& aEdgeOffset)
    {
    iFrameBrushData = new (ELeave) TFrameBrushPrivateData;
    iFrameBrushData->iImage = NULL;
    Mem::FillZ(iFrameBrushData->iFramePartImages, (sizeof(TInt)*KNumberOfFrameParts));

    iFrameBrushData->iImage = new (ELeave) CAlfSkinnableImage(&aEnv);       
    for (TInt i=0; i<KNumberOfFrameParts;i++)
        {
        iFrameBrushData->iFramePartImages[i] = new (ELeave) CAlfSkinnableImage(&aEnv); 
        }
    
    iFrameBrushData->iImage->SetImage(aImage); // This texturizes skin graphics if needed

    TAlfFrameBrushImageParams params;
    params.iEdgeOffset = aEdgeOffset;
    params.iImage = TAlfImageParams(iFrameBrushData->iImage->Image());
    
    TPckgC<TAlfFrameBrushImageParams> paramsPckg(params);
        
    CAlfBrush::ConstructL(
        aEnv,
        EAlfFrameBrushCreateImage,
        0,
        paramsPckg
        );        
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFrameBrush* CAlfFrameBrush::NewL(CAlfEnv& aEnv,
    const TAknsItemID& aIID, TInt aEdgeOffsetX, TInt aEdgeOffsetY)
    {
    CAlfFrameBrush* self = CAlfFrameBrush::NewLC( aEnv, aIID, aEdgeOffsetX, aEdgeOffsetY );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFrameBrush* CAlfFrameBrush::NewLC(CAlfEnv& aEnv, 
    const TAknsItemID& aIID, TInt aEdgeOffsetX, TInt aEdgeOffsetY)
    {
    CAlfFrameBrush* self = new( ELeave ) CAlfFrameBrush;
    CleanupStack::PushL( self );
    self->ConstructL(aEnv, aIID, aEdgeOffsetX, aEdgeOffsetY);
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFrameBrush* CAlfFrameBrush::NewL(CAlfEnv& aEnv,
    const TAknsItemID& aIID, const TAlfXYMetric& aEdgeOffset)
    {
    CAlfFrameBrush* self = CAlfFrameBrush::NewLC( aEnv, aIID, aEdgeOffset );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFrameBrush* CAlfFrameBrush::NewLC(CAlfEnv& aEnv, 
    const TAknsItemID& aIID, const TAlfXYMetric& aEdgeOffset)
    {
    CAlfFrameBrush* self = new( ELeave ) CAlfFrameBrush;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aIID, aEdgeOffset );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfFrameBrush::SetFrameRectsL(const TRect& aInnerRect, const TRect& aOuterRect)
    {
    TAlfFrameBrushParams params;
    params.iInnerRect = aInnerRect;
    params.iOuterRect = aOuterRect;

    TPckgC<TAlfFrameBrushParams> paramsPckg(params);        

    TInt err = Comms()->DoCmdNoReply(EAlfFrameBrushSetFrameRects, paramsPckg );    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfFrameBrush::SetFrameRectsL ignore error %d", err )
        }
        
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CAlfFrameBrush::~CAlfFrameBrush()
    {
    if ( iFrameBrushData )
        {
        delete iFrameBrushData->iImage;
        iFrameBrushData->iImage = NULL;    	        
        
        for (TInt i=0; i<KNumberOfFrameParts;i++)
            {
            delete iFrameBrushData->iFramePartImages[i]; 
            iFrameBrushData->iFramePartImages[i] = NULL;
            }        
        }
    delete iFrameBrushData;
    iFrameBrushData = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfFrameBrush::SetEdgeOffset(const TAlfXYMetric& aEdgeOffset)
    {
    TPckgC<TAlfXYMetric> paramsPckg(aEdgeOffset);        

    TInt err = Comms()->DoCmdNoReply(EAlfFrameBrushSetEdgeOffsetMetric, paramsPckg);    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfFrameBrush::SetEdgeOffset (TAlfXYMetric) ignore error %d", err )
        }                        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfFrameBrush::SetImage(const TAlfImage& aImage)
    {
    iFrameBrushData->iImage->SetImage(aImage); // This texturizes skin graphics if needed

    TAlfImageParams params = TAlfImageParams(iFrameBrushData->iImage->Image());
    TPckgC<TAlfImageParams> paramsPckg(params);        

    TInt err = Comms()->DoCmdNoReply(EAlfFrameBrushSetImage, paramsPckg );    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfFrameBrush::SetImage ignore error %d", err )
        }        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFrameBrush* CAlfFrameBrush::NewL(CAlfEnv& aEnv, const TAlfImage& aImage, const TAlfXYMetric& aEdgeOffset)
    {
    CAlfFrameBrush* self = CAlfFrameBrush::NewLC( aEnv, aImage, aEdgeOffset );
    CleanupStack::Pop( self );
    return self;        
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFrameBrush* CAlfFrameBrush::NewLC(CAlfEnv& aEnv, const TAlfImage& aImage, const TAlfXYMetric& aEdgeOffset)
    {
    CAlfFrameBrush* self = new( ELeave ) CAlfFrameBrush;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aImage, aEdgeOffset );
    return self;        
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfFrameBrush::SetImage(TFrameParts aFramePart, const TAlfImage& aImage)
    {
    iFrameBrushData->iFramePartImages[aFramePart]->SetImage(aImage); // This texturizes skin graphics if needed

    TAlfImageParams imageparams = TAlfImageParams(iFrameBrushData->iFramePartImages[aFramePart]->Image());
    
    TAlfFrameBrushFramePartImageParams params;
    params.iImage = imageparams;
    params.iFramePart = aFramePart;
    
    TPckgC<TAlfFrameBrushFramePartImageParams> paramsPckg(params);        

    TInt err = Comms()->DoCmdNoReply(EAlfFrameBrushSetFramePartImage, paramsPckg );    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfFrameBrush::SetImage () ignore error %d", err )
        }        
    }
    
