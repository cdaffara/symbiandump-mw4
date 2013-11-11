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
* Description:   Graphics context
*
*/



#include <alf/alfgc.h>
#include <alf/alfenv.h>
#include <alf/alftexture.h>
#include <alf/alftimedvalue.h>
#include <alf/alftextstyle.h>
#include <alf/alfrealline.h>
#include "alflogger.h"

//#include <uiacceltk/huicanvasconstants.h>


enum TAlfCanvasCommands
    {
    EAlfCanvasDrawImage,
    EAlfCanvasDrawText,
    EAlfCanvasDrawLines,
	EAlfCanvasDrawEllipse,
	EAlfCanvasDrawPoints,
	EAlfCanvasDrawPolygon,
	EAlfCanvasDrawRects,

	// Setters
    EAlfCanvasSetTextStyle,
	EAlfCanvasSetPenColor,
	EAlfCanvasSetPenWidth,
	EAlfCanvasSetOpacity,
	EAlfCanvasSetPolygonDrawMode,
	EAlfCanvasSetTextAlign,

    // Transformations
	EAlfCanvasLoadIdentity,
	EAlfCanvasTranslate,
	EAlfCanvasScale,
	EAlfCanvasRotate,
	
	EAlfCanvasPrepareDrawText
    };

NONSHARABLE_STRUCT( TAlfCanvasEndDrawParams )
    {
    TInt    iDummy1;
    TInt    iDummy2;
    TInt    iDummy3;
    TInt    iDummy4;
    };
    
NONSHARABLE_STRUCT( TAlfCanvasDrawImageParams )
    {
    TInt    iTexture;
    TReal32 iDestinationRectTopLeftX;
    TReal32 iDestinationRectTopLeftY;
    TReal32 iDestinationRectBottomRightX;
    TReal32 iDestinationRectBottomRightY;
    };

NONSHARABLE_STRUCT( TAlfCanvasDrawTextParams )
    {
    TReal32 iDestinationRectTopLeftX;
    TReal32 iDestinationRectTopLeftY;
    TReal32 iDestinationRectBottomRightX;
    TReal32 iDestinationRectBottomRightY;
    };
    
NONSHARABLE_STRUCT( TAlfCanvasDrawLinesParams )
    {
    TInt iLineCount;
	RArray<TReal32> iPointCords;
	};    

NONSHARABLE_STRUCT( TAlfCanvasDrawEllipseParams )
	{
    TReal32 iDestinationRectTopLeftX;
    TReal32 iDestinationRectTopLeftY;
    TReal32 iDestinationRectBottomRightX;
    TReal32 iDestinationRectBottomRightY;
    };
    	
	
	// Setters
NONSHARABLE_STRUCT( TAlfCanvasSetTextStyleParams )
    {
    TInt  iTextstyleId;
    };

NONSHARABLE_STRUCT( TAlfCanvasSetPenColorParams )
	{
    TRgb  iPenColor;
    };
NONSHARABLE_STRUCT( TAlfCanvasSetPenWidthParams )
	{
    TReal32  iPenWidth;
    };
NONSHARABLE_STRUCT( TAlfCanvasSetOpacityParams )
	{
    TReal32  iOpacity;
    };
NONSHARABLE_STRUCT( TAlfCanvasSetPolygonDrawModeParams )
	{
    TInt  iPolygonDrawMode;
    };
NONSHARABLE_STRUCT( TAlfCanvasSetTextAlignParams )
	{
    TInt  iTextAlignHorizontal;
    TInt  iTextAlignVertical;
    };


   // Transformations
NONSHARABLE_STRUCT( TAlfCanvasLoadIdentityParams )
	{
	TInt iDummy;
	};
	
NONSHARABLE_STRUCT( TAlfCanvasTranslateParams )
	{
	TReal32 iX; 
	TReal32 iY; 
	TReal32 iZ;
	};
	
NONSHARABLE_STRUCT( TAlfCanvasScaleParams )
	{
	TReal32 iX; 
	TReal32 iY; 
	TReal32 iZ;
	};
	
NONSHARABLE_STRUCT( TAlfCanvasRotateParams )
	{
	TReal32 iAngle;
	TReal32 iX; 
	TReal32 iY; 
	TReal32 iZ;
	};

    
template <class T>
HBufC8* AlfGcExternalizeL( const RArray<T>& aArray )
    {
    const TInt itemCount = aArray.Count();
    if ( !itemCount)
        {
        return HBufC8::NewL(0);
        }
    
    const T* firstItem = &aArray[0];    
    TPtrC8 arrayPtr( (TUint8*)firstItem, itemCount*sizeof(T) );            
    HBufC8* buffer = HBufC8::NewL( sizeof(TInt) + arrayPtr.Length() );
    TPtr8 ptr = buffer->Des();     
    
    ptr.Append( (const TUint8*)&itemCount, sizeof(TInt) );
    ptr.Append( arrayPtr );    
    return buffer;
    }


const TInt KAlfGcBufferGranularity = 1024;

struct CAlfGc::TAlfGcData
    {
    TAlfGcData() : iBuffer(NULL) {}
    HBufC8* iBuffer;
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAlfGc* CAlfGc::NewL()
    {
    CAlfGc* self = new (ELeave) CAlfGc;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfGc::CAlfGc()
    {
    }
    
void CAlfGc::ConstructL()
    {
    iData = new (ELeave) TAlfGcData;
    }
    
CAlfGc::~CAlfGc()
    {
    if ( iData )
        {
        delete iData->iBuffer;
        iData->iBuffer = NULL;
        }
    delete iData;
    iData = NULL;
    }
    

void CAlfGc::ActivateL()
    {    
    Deactivate(); // Just in case...
    iData->iBuffer = HBufC8::NewL( KAlfGcBufferGranularity );
    }

HBufC8* CAlfGc::CommandBuffer()
    {
    return iData->iBuffer;      
    }
    
void CAlfGc::Deactivate()
    {        
    delete iData->iBuffer;
    iData->iBuffer = NULL;
    }
    
void CAlfGc::AppendCommand( TInt aCommand, const TDesC8& aParams )
    {
    TRAP_IGNORE(AppendCommandL(aCommand,aParams));
    }

void CAlfGc::AppendCommandL( TInt aCommand, const TDesC8& aParams )
    {
    if (!iData->iBuffer)
        {
        return;    
        }
    
    __ALFLOGSTRING1( "CAlfGc::AppendCommand - start, buffer length: %d", iData->iBuffer->Length() )
    
    // create a bigger buffer if needed
    const TInt newSpaceNeeded = 4 + 4 + aParams.Length() + 4;
    __ALFLOGSTRING1( "CAlfGc::AppendCommand - new space needed: %d", newSpaceNeeded)
    if ( iData->iBuffer->Des().MaxLength() - iData->iBuffer->Length() < newSpaceNeeded )
        {
        __ALFLOGSTRING1( "CAlfGc::AppendCommand - allocating new buffer length: %d", iData->iBuffer->Length() + newSpaceNeeded * 2  )
        iData->iBuffer = iData->iBuffer->ReAllocL( iData->iBuffer->Length() + newSpaceNeeded * 2 );
        } 
    
    TPtr8 ptr = iData->iBuffer->Des();
    TInt length = aParams.Length();
    TInt command = aCommand;
    
    ptr.Append( (const TUint8*)&command, sizeof(TInt) );
    ptr.Append( (const TUint8*)&length, sizeof(TInt) );
    ptr.Append( aParams );
    
    TInt padding = 0;
    if (length % 4)
        {
        padding = 4 - length % 4;    
        }
    
    for ( TInt i = 0 ; i < padding ; i++ )
        {
        ptr.Append( 0xFF );        
        }        
    
    for ( TInt i = 0 ; i < aParams.Length() ; i++ )
        {
        __ALFLOGSTRING2( "[%d]: %d", i, aParams[i] )
        }
    
    __ALFLOGSTRING1( "CAlfGc::AppendCommand - end, buffer length: %d", iData->iBuffer->Length() )
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfGc::DrawImage(const CAlfTexture& aTexture, const TAlfRealRect& aDestinationRect)   
    {
    TInt serversideTexture = aTexture.ServerHandle();
    
    const TAlfCanvasDrawImageParams params = {serversideTexture, 
        aDestinationRect.iTl.iX,
        aDestinationRect.iTl.iY,
        aDestinationRect.iBr.iX,
        aDestinationRect.iBr.iY
     };
    TPckgC<TAlfCanvasDrawImageParams> paramsPckg( params );
    AppendCommand( EAlfCanvasDrawImage, paramsPckg );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfGc::DrawText(const TDesC& aText, const TAlfRealRect& aDestinationRect)
    {
        TPtrC8 ptr(reinterpret_cast<const TUint8 *>(aText.Ptr()), aText.Length()*2);                
        AppendCommand( EAlfCanvasPrepareDrawText, ptr );
        
        const TAlfCanvasDrawTextParams params = { 
            aDestinationRect.iTl.iX,
            aDestinationRect.iTl.iY,
            aDestinationRect.iBr.iX,
            aDestinationRect.iBr.iY
            };
        TPckgC<TAlfCanvasDrawTextParams> paramsPckg( params );                    
        AppendCommand( EAlfCanvasDrawText, paramsPckg );        
    }

void CAlfGc::DrawLines(const RArray<TReal32> & aLinesPointCords)
	{
	HBufC8* buffer = NULL;
    TRAP_IGNORE(buffer = AlfGcExternalizeL(aLinesPointCords))
    if (buffer)
        {        
    	AppendCommand( EAlfCanvasDrawLines, *buffer );
    	delete buffer;
    	buffer = NULL;
        }
	}
	

void CAlfGc::DrawEllipse(const TAlfRealRect& aDestinationRect)
	{
	const TAlfCanvasDrawEllipseParams params = {
        aDestinationRect.iTl.iX,
        aDestinationRect.iTl.iY,
        aDestinationRect.iBr.iX,
        aDestinationRect.iBr.iY
     };
    TPckgC<TAlfCanvasDrawEllipseParams> paramsPckg( params );
    AppendCommand( EAlfCanvasDrawEllipse, paramsPckg );
	}

void CAlfGc::DrawPoints(const RArray<TReal32> & aPointsCords)
	{
	HBufC8* buffer = NULL;
    TRAP_IGNORE(buffer = AlfGcExternalizeL(aPointsCords))
    if (buffer)
        {        
    	AppendCommand( EAlfCanvasDrawPoints, *buffer );
    	delete buffer;
    	buffer = NULL;
        }
	}
	
void CAlfGc::DrawPolygon(const RArray<TReal32> & aPolygonPointCords)
	{
	HBufC8* buffer = NULL;
    TRAP_IGNORE(buffer = AlfGcExternalizeL(aPolygonPointCords))
    if (buffer)
        {        
    	AppendCommand( EAlfCanvasDrawPolygon, *buffer );
    	delete buffer;
    	buffer = NULL;
        }
	}

void CAlfGc::DrawRects(const RArray<TReal32> & aRectPointCords)
	{
	HBufC8* buffer = NULL;
    TRAP_IGNORE(buffer = AlfGcExternalizeL(aRectPointCords))
    if (buffer)
        {        
    	AppendCommand( EAlfCanvasDrawRects, *buffer );
    	delete buffer;
    	buffer = NULL;
        }
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CAlfGc::SetTextStyle(const CAlfTextStyle& aTextStyle)
    {
    TInt serverSideTextStyleId = aTextStyle.ServerHandle();    
    TAlfCanvasSetTextStyleParams params = {serverSideTextStyleId};
    TPckgC<TAlfCanvasSetTextStyleParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasSetTextStyle, paramsPckg );        
    }
    

void CAlfGc::SetPenColor(const TRgb& aColor)
	{
	TAlfCanvasSetPenColorParams params = {aColor};
    TPckgC<TAlfCanvasSetPenColorParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasSetPenColor, paramsPckg );        
	}
	
void CAlfGc::SetPenWidth(const TReal32& aWidth)
	{
	TAlfCanvasSetPenWidthParams params = {aWidth};
    TPckgC<TAlfCanvasSetPenWidthParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasSetPenWidth, paramsPckg );        
	}
	
void CAlfGc::SetOpacity(const TReal32& aOpacity)
	{
	TAlfCanvasSetOpacityParams params = {aOpacity};
    TPckgC<TAlfCanvasSetOpacityParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasSetOpacity, paramsPckg );        
	}
	

void CAlfGc::SetPolygonDrawMode(TAlfPolygonDrawMode aPolygonDrawMode)
	{
	TAlfCanvasSetPolygonDrawModeParams params = {aPolygonDrawMode};
    TPckgC<TAlfCanvasSetPolygonDrawModeParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasSetPolygonDrawMode, paramsPckg );	
	}
void CAlfGc::SetTextAlign(TAlfAlignHorizontal aAlignHorizontal, TAlfAlignVertical aAlignVertical)
	{
	TAlfCanvasSetTextAlignParams params = {aAlignHorizontal, aAlignVertical};
    TPckgC<TAlfCanvasSetTextAlignParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasSetTextAlign, paramsPckg );	
	}

    // Transformations
void CAlfGc::LoadIdentity()
	{
	TAlfCanvasLoadIdentityParams params = {0};
    TPckgC<TAlfCanvasLoadIdentityParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasLoadIdentity, paramsPckg );
	}
	
void CAlfGc::Translate(const TReal32& aX, const TReal32& aY, const TReal32& aZ)
	{
	TAlfCanvasTranslateParams params = {aX, aY, aZ};
    TPckgC<TAlfCanvasTranslateParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasTranslate, paramsPckg );        
	}
	
void CAlfGc::Scale(const TReal32& aX, const TReal32& aY, const TReal32& aZ)
	{
	TAlfCanvasScaleParams params = {aX, aY, aZ};
    TPckgC<TAlfCanvasScaleParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasScale, paramsPckg );        
	}
	
void CAlfGc::Rotate(const TReal32& aAngle, const TReal32& aX, const TReal32& aY, const TReal32& aZ)
	{
	TAlfCanvasRotateParams params = {aAngle, aX, aY, aZ};
    TPckgC<TAlfCanvasRotateParams> paramsPckg( params );                    
    AppendCommand( EAlfCanvasRotate, paramsPckg );        
	}
