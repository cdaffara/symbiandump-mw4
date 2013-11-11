/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/



#include "uiacceltk/HuiCanvasCmdBufferReader.h"
#include "uiacceltk/HuiUtil.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CHuiCanvasCmdBufferReader::CHuiCanvasCmdBufferReader()
    {	    
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHuiCanvasCmdBufferReader::~CHuiCanvasCmdBufferReader()
    {
    iStream.Close();	
    
    delete iCachedText;
    }
    
// ---------------------------------------------------------------------------
// InitL
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::Init( TPtr8& aDes,TInt aLength)
    {
    iStream.Close();
    if (aDes.Ptr())
        {
        iStream.Open( aDes.Ptr(), aLength );
		delete iCachedText;
        iCachedText = NULL;
        iBasePtr = &aDes;
#ifdef _DEBUG
        iBufferIndex = 0;
        iBufferLength = aLength;
#endif
        }
    }

// ---------------------------------------------------------------------------
// ReadInt32L
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::ReadInt32L( TInt& aValue )
        {
        aValue = iStream.ReadInt32L();
#ifdef _DEBUG
    iBufferIndex += sizeof(TInt32);
#endif
        }

// ---------------------------------------------------------------------------
// ReadPointL
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::ReadPointL( TPoint& aPoint )
    {
    iStream.ReadL( (TUint8*)&(aPoint.iX), sizeof( TInt32 ) * 2 );
    }
// ---------------------------------------------------------------------------
// ReadRectL
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::ReadRectL( TRect& aRect )
    {
    iStream.ReadL( (TUint8*)&(aRect.iTl.iX), sizeof( TInt32) * 4 );
    }

// ---------------------------------------------------------------------------
// ReadBitmapLC
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::ReadBitmapLC( CFbsBitmap*& aBitmap )
    {
    TInt uid;
    ReadInt32L( uid );
    aBitmap = new(ELeave)CFbsBitmap;
    CleanupStack::PushL( aBitmap );
    
    //TODO:Bitmap cache, check if bitmap is already in cache. Avoid call to Duplicate
    //TODO: Cache needs to remove stuff, if not used.
    
    aBitmap->Duplicate( uid );
    }

// ---------------------------------------------------------------------------
// ReadBitmapHandleL
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::ReadBitmapHandleL( TInt& aBitmapHandle  )
    {
    aBitmapHandle = iStream.ReadInt32L();        
    }
     
// ---------------------------------------------------------------------------
// ReadTextL
// ---------------------------------------------------------------------------
//
TBool CHuiCanvasCmdBufferReader::ReadTextL( TPtr& aText , THuiCanvasTextParameters& aTextParameters )
    {
    TInt length = 0;
    TBool useCachedText = iStream.ReadInt8L();
    if ( !useCachedText )
        {
        length = iStream.ReadInt32L() / 2 ;
        TInt startPadding = iStream.ReadInt8L();
        TInt endPadding = iStream.ReadInt8L();

        TInt offset = iStream.Source()->TellL( MStreamBuf::ERead ).Offset() + startPadding;
        TPtr16 ptr = TPtr16( (TUint16*)( (TInt)iBasePtr->Ptr() + offset ), length, length );
        aText.Set( ptr );

        iStream.Source()->SeekL( MStreamBuf::ERead, TStreamPos( offset + length * 2 + endPadding ));
        if ( !iCachedText )
            {
            iCachedText = new(ELeave)TPtr16( ptr );
            }
        else
            {
            iCachedText->Set( ptr );
            }
        }
    else 
        {
        aText.Set( *iCachedText );
        }
    TInt8 textParameters = iStream.ReadInt8L();
    switch ( textParameters )
        {
        case 1:
            {
            // Is there text parameters
            aTextParameters.iStart = iStream.ReadInt32L();
            aTextParameters.iEnd = iStream.ReadInt32L();
            aTextParameters.iFlags = iStream.ReadUint16L();
            if ( aTextParameters.iStart >= aTextParameters.iEnd )
                {
                return EFalse;
                }
            break;
            }
        case 2:
            {
            aTextParameters.iStart = 0;
            aTextParameters.iEnd = length / 2;
            aTextParameters.iFlags = 0;
            break;
            }
        default: // 0
            {
            aTextParameters.iStart = 0;
            aTextParameters.iEnd = KMaxTInt;
            aTextParameters.iFlags = 0;
            break;
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// ReadPointArrayL, NOT TESTED
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::ReadPointArrayL( CArrayFix<TPoint>*& aPointArray )
    {
    TInt count = iStream.ReadInt32L();
    TInt index = 0;
    aPointArray = new(ELeave)CArrayFixFlat<TPoint>(count);
    CleanupStack::PushL( aPointArray );
    while( index < count )
        {
        TPoint point = TPoint(0,0);
        ReadPointL( point );
        aPointArray->AppendL( point );
        index++;
        }
    CleanupStack::Pop( aPointArray );
    }

// ---------------------------------------------------------------------------
// ReadPointArrayL, NOT TESTED
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::ReadPointArrayL( TPoint*& aPointArray, TInt& aNumPoints )
    {
    ReadInt32L( aNumPoints );
    TInt index = 0;
    aPointArray = new(ELeave)TPoint[aNumPoints];
    CleanupStack::PushL( aPointArray );
        
    while( index < aNumPoints )
        {
        TPoint* point = new(ELeave)TPoint();
        CleanupStack::PushL( point );
        ReadPointL( *point );
        aPointArray[index]= *point;
        CleanupStack::Pop( point );
        index++;
        }
    CleanupStack::Pop( aPointArray );
    }

// ---------------------------------------------------------------------------
// ReadPointArrayL - NOT TESTED
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::DeletePointArrayL( TPoint*& aPointArray, TInt& aNumPoints )
    {
    TInt index = 0;
    while( index < aNumPoints )
        {
        delete &(aPointArray[index]);
        index++;
        }
    delete aPointArray;
    aPointArray = NULL;
    }

// ---------------------------------------------------------------------------
// ReadRegionL
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::ReadRegionL( RRegion& aRegion )
    {
    aRegion.Clear();
    TRect rect;
    TInt count;
    ReadInt32L( count );// rect count
    for (TInt i = 0 ; i < count ; i++ )
        {
        ReadRectL( rect );
        aRegion.AddRect( rect );
        }
    }

// ---------------------------------------------------------------------------
// ReadColorL
// ---------------------------------------------------------------------------
//
void CHuiCanvasCmdBufferReader::ReadColorL( TRgb& aColor )
    {
    aColor.SetInternal( iStream.ReadUint32L());
    }

// ---------------------------------------------------------------------------
// Stream
// ---------------------------------------------------------------------------
//
RMemReadStream& CHuiCanvasCmdBufferReader::Stream()
    {
    return iStream;    
    }
    
