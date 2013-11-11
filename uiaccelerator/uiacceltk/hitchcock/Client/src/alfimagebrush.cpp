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
* Description:   Image brush
*
*/



#include "alf/alfimagebrush.h"
#include "alf/alfimage.h"
#include "alf/alftexture.h"
#include "alf/alfenv.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"
#include "alfskinnableimage.h"
#include "alf/alfconstants.h"

#include <uiacceltk/HuiUtil.h>

// Private data structure
struct CAlfImageBrush::TImageBrushPrivateData
    {
    CAlfSkinnableImage* iImage;   // owned
    TInt iLeftBorderWidth;
    TInt iRightBorderWidth;
    TInt iTopBorderWidth;
    TInt iBottomBorderWidth;     
    TAlfTimedPoint iOffset;
    TAlfTimedValue iScale;
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfImageBrush::CAlfImageBrush()
    : CAlfBrush()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfImageBrush::ConstructL(CAlfEnv& aEnv, const TAlfImage & aImage)
    {
    iImageBrushData = new (ELeave) TImageBrushPrivateData;
    iImageBrushData->iImage = NULL;
    iImageBrushData->iImage = new (ELeave) CAlfSkinnableImage(&aEnv);
    iImageBrushData->iImage->SetImage(aImage); // This texturizes skin graphics if needed
    iImageBrushData->iLeftBorderWidth = 0;
    iImageBrushData->iRightBorderWidth = 0;
    iImageBrushData->iTopBorderWidth = 0;
    iImageBrushData->iBottomBorderWidth = 0;    
    
    TAlfImageBrushParams params;
    params.iTextureHandle = iImageBrushData->iImage->Image().Texture().ServerHandle(); 
    params.iTl = aImage.TimedTopLeft();
    params.iBr = aImage.TimedBottomRight();
    params.iLeftBorderWidth = iImageBrushData->iLeftBorderWidth;
    params.iRightBorderWidth = iImageBrushData->iRightBorderWidth;
    params.iTopBorderWidth = iImageBrushData->iTopBorderWidth;
    params.iBottomBorderWidth = iImageBrushData->iBottomBorderWidth;
    
    TPckgC<TAlfImageBrushParams> paramsPckg(params);
        
    CAlfBrush::ConstructL(
        aEnv,
        EAlfImageBrushCreate,
        0,
        paramsPckg
        );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfImageBrush* CAlfImageBrush::NewL(CAlfEnv& aEnv, 
    const TAlfImage & aImage)
    {
    CAlfImageBrush* self = CAlfImageBrush::NewLC( aEnv, aImage );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfImageBrush* CAlfImageBrush::NewLC(CAlfEnv& aEnv, 
    const TAlfImage & aImage)
    {
    CAlfImageBrush* self = new( ELeave ) CAlfImageBrush;
    CleanupStack::PushL( self );
    self->ConstructL(aEnv, aImage);
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CAlfImageBrush::~CAlfImageBrush()
    {
    if ( iImageBrushData )
        {
        delete iImageBrushData->iImage;
        iImageBrushData->iImage = NULL;
        }
    delete iImageBrushData;
    iImageBrushData = NULL;
    }


// ---------------------------------------------------------------------------
// Set image
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageBrush::SetImage(const TAlfImage& aImage)
    {
    iImageBrushData->iImage->SetImage(aImage); // This texturizes skin graphics if needed
    
    TAlfImageBrushParams params;
    params.iTextureHandle = aImage.Texture().ServerHandle(); 
    params.iTl = aImage.TimedTopLeft();
    params.iBr = aImage.TimedBottomRight();
    params.iLeftBorderWidth = iImageBrushData->iLeftBorderWidth;
    params.iRightBorderWidth = iImageBrushData->iRightBorderWidth;
    params.iTopBorderWidth = iImageBrushData->iTopBorderWidth;
    params.iBottomBorderWidth = iImageBrushData->iBottomBorderWidth;    
    TPckgC<TAlfImageBrushParams> paramsPckg(params);        

    TInt err = Comms()->DoCmdNoReply(EAlfImageBrushSetImage, paramsPckg);    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageBrush::SetImage ignore error %d", err )
        }
    }
 
// ---------------------------------------------------------------------------
// Get image
// ---------------------------------------------------------------------------
//   
EXPORT_C const TAlfImage& CAlfImageBrush::Image() const
    {
    return iImageBrushData->iImage->Image();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageBrush::SetBorders(TInt aLeftBorderWidth,
                             TInt aRightBorderWidth,
                             TInt aTopBorderHeight,
                             TInt aBottomBorderHeight)
    {
    iImageBrushData->iLeftBorderWidth = aLeftBorderWidth;
    iImageBrushData->iRightBorderWidth = aRightBorderWidth;
    iImageBrushData->iTopBorderWidth = aTopBorderHeight;
    iImageBrushData->iBottomBorderWidth = aBottomBorderHeight;    

    TAlfImageBrushParams params;
    params.iLeftBorderWidth = iImageBrushData->iLeftBorderWidth;
    params.iRightBorderWidth = iImageBrushData->iRightBorderWidth;
    params.iTopBorderWidth = iImageBrushData->iTopBorderWidth;
    params.iBottomBorderWidth = iImageBrushData->iBottomBorderWidth;    
    TPckgC<TAlfImageBrushParams> paramsPckg(params);        

    TInt err = Comms()->DoCmdNoReply(EAlfImageBrushSetBorders, paramsPckg);    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageBrush::SetBorders ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfImageBrush::BorderDrawingEnabled() const
    {
	return ((iImageBrushData->iLeftBorderWidth!=0) || (iImageBrushData->iRightBorderWidth!=0)		
		    || (iImageBrushData->iTopBorderWidth!=0) || (iImageBrushData->iBottomBorderWidth!=0));
        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfImageBrush::CenterDrawingEnabled() const
    {
	if(!iImageBrushData->iImage->Image().HasTexture()) 
		{
		return EFalse;
		}				
	// determine if there are center pixels available inside borders
	// if not, we can't draw the center..
	TSize imageSize = iImageBrushData->iImage->Image().Texture().Size();	
	if((imageSize.iWidth - iImageBrushData->iLeftBorderWidth - iImageBrushData->iRightBorderWidth <= 0)
	   || (imageSize.iHeight - iImageBrushData->iTopBorderWidth - iImageBrushData->iBottomBorderWidth <= 0))
		{
		return EFalse;
		}	  
		
	return ETrue;        
    }

// ---------------------------------------------------------------------------
// Set scale mode for the image brush
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageBrush::SetScaleMode(CAlfImageVisual::TScaleMode aScaleMode)
    {
    TPckgC<TInt> paramsPckg(aScaleMode);        

    TInt err = Comms()->DoCmdNoReply(EAlfImageBrushSetScaleMode, paramsPckg );    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageBrush::SetScaleMode panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Sets offfset for the image
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfImageBrush::SetOffset(const TAlfTimedPoint& aOffset)
    {
    iImageBrushData->iOffset.iX = aOffset.iX;
    iImageBrushData->iOffset.iY = aOffset.iY;


    TPckgC<TAlfTimedPoint> offsetPckg(iImageBrushData->iOffset);
    
    TInt err = Comms()->DoCmdNoReply(EAlfImageBrushSetOffset, offsetPckg);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageBrush::SetOffset panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Returns the offset
// ---------------------------------------------------------------------------
//    
EXPORT_C const TAlfTimedPoint& CAlfImageBrush::Offset() const
    {
    TPckg<TAlfTimedPoint> offsetPckg(iImageBrushData->iOffset);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfImageBrushOffset, KNullDesC8, offsetPckg);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfImageBrush::Offset ignore error %d", err )
        }

    return iImageBrushData->iOffset;
    }

// ---------------------------------------------------------------------------
// Sets scale factor
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfImageBrush::SetScale(const TAlfTimedValue& aScale)
    {
    TPckgC<TAlfTimedValue> buf(aScale);

    TInt err = Comms()->DoCmdNoReply( EAlfImageBrushSetScale, buf );

    if ( err == KErrNone )
        {
        iImageBrushData->iScale = aScale;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfImageBrush::SetScale panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Returns scale factor
// ---------------------------------------------------------------------------
//    
EXPORT_C const TAlfTimedValue& CAlfImageBrush::Scale() const
    {
    TPckg<TAlfTimedValue> buf(iImageBrushData->iScale);
    TInt err = Comms()->DoSynchronousCmd( EAlfImageBrushScale, KNullDesC8, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfImageBrush::Scale panic error %d", err )
        USER_INVARIANT();
        }
    
    return iImageBrushData->iScale;
    }
