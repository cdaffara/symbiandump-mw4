/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <fbs.h>
//#include <oom.h>
#include "BitmapTiler.h"
#include "MaskedBitmap.h"

//===============================================================================
// CScanLineFetcher
//===============================================================================
CScanLineFetcher::CScanLineFetcher( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine )
    : iImage( aImage ), iLine( aStartLine ), iOffsetX( aStartPos ), iBmpLine(0), iMskLine(0)
    {
    }

void CScanLineFetcher::operator++()
    {
    iLine++;
    }
    
void CScanLineFetcher::operator+=( TInt aSteps )
    {
    iLine += aSteps;
    }

//===============================================================================
// CCompressedScanLineFetcher
//===============================================================================
CCompressedScanLineFetcher* CCompressedScanLineFetcher::NewL( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine )
    {
    CCompressedScanLineFetcher* self = new (ELeave) CCompressedScanLineFetcher( aImage, aStartPos, aStartLine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;         
    }
    
CCompressedScanLineFetcher::CCompressedScanLineFetcher( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine )
    : CScanLineFetcher( aImage, aStartPos, aStartLine )
    {
    }

void CCompressedScanLineFetcher::ConstructL()
    {
    // scanline sizes
    iBmpLineSize = iImage->Bitmap().ScanLineLength( iImage->SizeInPixels().iWidth, iImage->Bitmap().DisplayMode() );
    if( iImage->HasMask() )
        iMskLineSize = iImage->Mask().ScanLineLength( iImage->SizeInPixels().iWidth, iImage->Mask().DisplayMode() );
    
    // create the buffer for bitmap and mask scanlines
    //iBmpLine = (TUint16*)MemoryManager::Alloc( iBmpLineSize );
    iBmpLine = (TUint16*)User::Alloc( iBmpLineSize );
    User::LeaveIfNull( iBmpLine );
    if( iImage->HasMask() )
        {
        //iMskLine = (TUint16*)MemoryManager::Alloc( iMskLineSize );    
        iMskLine = (TUint16*)User::Alloc( iMskLineSize );    
        User::LeaveIfNull( iMskLine );
        }
    }    
        
CCompressedScanLineFetcher::~CCompressedScanLineFetcher()
    {
    //if( iBmpLine ) MemoryManager::Free( iBmpLine );
    //if( iMskLine ) MemoryManager::Free( iMskLine );
    if( iBmpLine ) User::Free( iBmpLine );
    if( iMskLine ) User::Free( iMskLine );
    }

void CCompressedScanLineFetcher::Fetch( TSrcScanLine& aScanLine )
    {
    const CFbsBitmap& bmp = iImage->Bitmap();
    TPtr8 ptr( (TUint8*)iBmpLine, iBmpLineSize );
    bmp.GetScanLine( ptr, TPoint(0, iLine), bmp.SizeInPixels().iWidth, bmp.DisplayMode() );
    aScanLine.Set( (TUint16*)iBmpLine, iOffsetX );
    }

void CCompressedScanLineFetcher::Fetch( TSrcScanLineMasked& aScanLine )
    {
    const CFbsBitmap& bmp = iImage->Bitmap();
    const CFbsBitmap& msk = iImage->Mask();
    TPtr8 bmpPtr( (TUint8*)iBmpLine, iBmpLineSize );
    TPtr8 mskPtr( (TUint8*)iMskLine, iMskLineSize );
    bmp.GetScanLine( bmpPtr, TPoint(0, iLine), bmp.SizeInPixels().iWidth, bmp.DisplayMode() );    
    msk.GetScanLine( mskPtr, TPoint(0, iLine), msk.SizeInPixels().iWidth, msk.DisplayMode() );
        
    aScanLine.Set( (TUint16*)iBmpLine, (TUint16*)iMskLine, iOffsetX );
    }

void CCompressedScanLineFetcher::Fetch( TSrcScanLineAlpha& aScanLine )
    {
    const CFbsBitmap& bmp = iImage->Bitmap();
    const CFbsBitmap& msk = iImage->Mask();
    TPtr8 bmpPtr( (TUint8*)iBmpLine, iBmpLineSize );
    TPtr8 mskPtr( (TUint8*)iMskLine, iMskLineSize );
    bmp.GetScanLine( bmpPtr, TPoint(0, iLine), bmp.SizeInPixels().iWidth, bmp.DisplayMode() );    
    msk.GetScanLine( mskPtr, TPoint(0, iLine), msk.SizeInPixels().iWidth, msk.DisplayMode() );
    aScanLine.Set( (TUint16*)iBmpLine, (TUint8*)iMskLine, iOffsetX );
    }

//==============================================================================
// CFastScanLineFetcher
//==============================================================================
CFastScanLineFetcher* CFastScanLineFetcher::NewL( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine )
    {
    CFastScanLineFetcher* self = new (ELeave) CFastScanLineFetcher( aImage, aStartPos, aStartLine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;         
    }

void CFastScanLineFetcher::ConstructL()
    {
    // scanline sizes
    iBmpLineSize = iImage->Bitmap().ScanLineLength( iImage->SizeInPixels().iWidth, iImage->Bitmap().DisplayMode() );
    if( iImage->HasMask() )
        iMskLineSize = iImage->Mask().ScanLineLength( iImage->SizeInPixels().iWidth, iImage->Mask().DisplayMode() );
        
    // start addresses for direct access to bitmap memory
    iImage->Bitmap().LockHeap();
    iBmpLine = (TUint16*)iImage->Bitmap().DataAddress();
    if( iImage->HasMask() )
        {
        iImage->Mask().LockHeap();
        iMskLine = (TUint16*)iImage->Mask().DataAddress();
        }
    }    

CFastScanLineFetcher::CFastScanLineFetcher( CMaskedBitmap* aImage, TInt aStartPos, TInt aStartLine )
    : CScanLineFetcher( aImage, aStartPos, aStartLine )
    {
    }
    
CFastScanLineFetcher::~CFastScanLineFetcher()
    {
    if( iImage->HasMask() ) 
        iImage->Mask().UnlockHeap();
    iImage->Bitmap().UnlockHeap();
    }

// composite scanlines
void CFastScanLineFetcher::Fetch( TSrcScanLine& aScanLine )
    {
    TUint8* p = (TUint8*)iBmpLine;
    p += iLine * iBmpLineSize;
    aScanLine.Set( (TUint16*)p, iOffsetX );    
    }

void CFastScanLineFetcher::Fetch( TSrcScanLineMasked& aScanLine )
    {
    TUint8* p1 = (TUint8*)iBmpLine;
    p1 += iLine * iBmpLineSize;
    TUint8* p2 = (TUint8*)iMskLine;
    p2 += iLine * iMskLineSize;
    aScanLine.Set( (TUint16*)p1, (TUint16*)p2, iOffsetX );    
    }

void CFastScanLineFetcher::Fetch( TSrcScanLineAlpha& aScanLine )
    {
    TUint8* p1 = (TUint8*)iBmpLine;
    p1 += iLine * iBmpLineSize;
    TUint8* p2 = (TUint8*)iMskLine;
    p2 += iLine * iMskLineSize;
    aScanLine.Set( (TUint16*)p1, (TUint8*)p2, iOffsetX );    
    }
    
// END OF FILE
