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
* Description:   Drop shadow parameter handler
*
*/



#include "alf/alfdropshadow.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"

// ======== LOCAL FUNCTIONS ========

NONSHARABLE_STRUCT( CAlfDropShadow::TAlfDropShadowData )
    {
    TAlfDropShadowData(): iComms(NULL) {}
    
    CAlfGenComponent* iComms; // not own
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 2-phased constructor
// ---------------------------------------------------------------------------
//
CAlfDropShadow* CAlfDropShadow::NewL(CAlfGenComponent& aComms)
    {
    CAlfDropShadow* self = new (ELeave) CAlfDropShadow;
    CleanupStack::PushL( self );
    self->ConstructL( aComms );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// First phase constructor
// ---------------------------------------------------------------------------
//
CAlfDropShadow::CAlfDropShadow()
    {
    }


// ---------------------------------------------------------------------------
// Sencond phase constructor
// ---------------------------------------------------------------------------
//
void CAlfDropShadow::ConstructL(CAlfGenComponent& aComms)
    {
    iData = new (ELeave) TAlfDropShadowData;

    iData->iComms = &aComms;
    }

// ---------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CAlfDropShadow::~CAlfDropShadow()
    {
    delete iData;
    }
   
// ---------------------------------------------------------------------------
// Sets offset in polar coordinates.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfDropShadow::SetOffset( 
    TReal32 aAngle, 
    TAlfMetric aDistance, 
    TInt aTransitionTime ) __SOFTFP
    {
    TAlfDropShadowOffsetPolarParams params;
    params.iAngle = aAngle;
    params.iDistance = aDistance;
    params.iTransitionTime = aTransitionTime;
    
    TPckgC<TAlfDropShadowOffsetPolarParams> inBuf( params );
    
    iData->iComms->DoCmdNoReply(EAlfDropShadowSetOffsetInPolar, inBuf );
    }

// ---------------------------------------------------------------------------
// Sets RGB color
// ---------------------------------------------------------------------------
//        
EXPORT_C void CAlfDropShadow::SetColor(
    const TRgb& aColor, 
    TInt aTransitionTime)
    {
    TAlfDropShadowColorRgbParams params;
    params.iColor = aColor;
    params.iTransitionTime = aTransitionTime;
    
    TPckgC<TAlfDropShadowColorRgbParams> inBuf( params );
    
    iData->iComms->DoCmdNoReply(EAlfDropShadowSetColorRgb, inBuf );
    }

// ---------------------------------------------------------------------------
// Sets skinned color
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfDropShadow::SetColor(
    const TAknsItemID& aID,
    const TInt aIndex, 
    TInt aTransitionTime )
    {
    TAlfDropShadowColorSkinParams params;
    params.iID = aID;
    params.iIndex = aIndex;
    params.iTransitionTime = aTransitionTime;
    
    TPckgC<TAlfDropShadowColorSkinParams> inBuf( params );
    
    iData->iComms->DoCmdNoReply(EAlfDropShadowSetColorSkin, inBuf );
    }

// ---------------------------------------------------------------------------
// Sets opacity
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfDropShadow::SetOpacity( const TAlfTimedValue& aOpacity )
    {
    TPckgC<TAlfTimedValue> inBuf(aOpacity);
    iData->iComms->DoCmdNoReply(EAlfDropShadowSetOpacity, inBuf );
    }

// ---------------------------------------------------------------------------
// Sets offset in cartesian coordinates
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfDropShadow::SetOffset( const TAlfTimedPoint& aOffset, TInt aOffsetUnit )
    {
    TAlfDropShadowOffsetXYParams params;
    params.iOffset = aOffset;
    params.iOffsetUnit = aOffsetUnit;

    TPckgC<TAlfDropShadowOffsetXYParams> inBuf( params );
    
    iData->iComms->DoCmdNoReply(EAlfDropShadowSetOffsetInXY, inBuf );
    }

// ---------------------------------------------------------------------------
// Sets blur radius
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfDropShadow::SetRadius( const TAlfTimedValue& aRadius )
    {
    TPckgC<TAlfTimedValue> inBuf(aRadius);
    iData->iComms->DoCmdNoReply(EAlfDropShadowSetBlurRadius, inBuf );
    }
    

// ---------------------------------------------------------------------------
// Sets scale factor
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfDropShadow::SetScale( const TAlfTimedValue& aScale )
    {
    TPckgC<TAlfTimedValue> inBuf(aScale);
    iData->iComms->DoCmdNoReply(EAlfDropShadowSetScale, inBuf );
    }
