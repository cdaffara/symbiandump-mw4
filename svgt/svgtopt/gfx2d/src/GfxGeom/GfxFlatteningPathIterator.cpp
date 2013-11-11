/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Graphics Extension Library source file
*
*/


#include "GfxFlatteningPathIterator.h"
#include "GfxShape.h"
#include "GfxAffineTransform.h"


// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------
CGfxFlatteningPathIterator::CGfxFlatteningPathIterator( TInt32 aLimit )
    : iIdx( 0 ),
      iSegType( EGfxSegClose ),
      iIsDone( EFalse ),
      iLimit( aLimit )
    {
    }


// ---------------------------------------------------------------------------
// Create a new instance.
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  CGfxFlatteningPathIterator* CGfxFlatteningPathIterator::NewL( MGfxShape* aSrc,
// ---------------------------------------------------------------------------
 CGfxFlatteningPathIterator* CGfxFlatteningPathIterator::NewL( MGfxShape* aSrc,
                                                                       TGfxAffineTransform* aAt,
                                                                       TInt32 aLimit )
    {
    CGfxFlatteningPathIterator* self = new ( ELeave )
                                       CGfxFlatteningPathIterator( aLimit );

    CleanupStack::PushL( self );
    self->ConstructL( aSrc, aAt );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Second phase in constructing this object.
// ---------------------------------------------------------------------------
void CGfxFlatteningPathIterator::ConstructL( MGfxShape* aSrc,
                                             TGfxAffineTransform* aAt )
    {
    aSrc->GetPathIteratorL( aAt, iSrc );
    iCoords = new ( ELeave ) RArray<TFloatFixPt>( 10 );
    iCoords->AppendL( NULL );
    iCoords->Remove(0);
    NextL();
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  CGfxFlatteningPathIterator::~CGfxFlatteningPathIterator()
// ---------------------------------------------------------------------------
 CGfxFlatteningPathIterator::~CGfxFlatteningPathIterator()
    {
	if ( iSrc )
		{
		delete iSrc;
		iSrc = NULL;
		}
	if ( iCoords )
		{
		iCoords->Close();
		delete iCoords;
		iCoords = NULL;
		}
    }


// --------------------------------------------------------------------------
//  TGfxSegType CGfxFlatteningPathIterator::CurrentSegment( TFloatFixPt* aCoords )
// ---------------------------------------------------------------------------
 TGfxSegType CGfxFlatteningPathIterator::CurrentSegment( TFloatFixPt* aCoords )
    {
    if ( iCoords->Count() > 0 && iCoords->Count() > iIdx )
        {
        aCoords[0] = ( *iCoords )[iIdx];
        aCoords[1] = ( *iCoords )[iIdx + 1];
        }

    return iSegType;
    }


// --------------------------------------------------------------------------
//  TBool CGfxFlatteningPathIterator::IsDone()
// ---------------------------------------------------------------------------
 TBool CGfxFlatteningPathIterator::IsDone()
    {
    return iIsDone;
    }


// --------------------------------------------------------------------------
//  void CGfxFlatteningPathIterator::Next()
// ---------------------------------------------------------------------------
 void CGfxFlatteningPathIterator::NextL()
    {
    iIdx += 2;

    if ( iCoords->Count() <= 0 || iCoords->Count() <= iIdx )
        {
        TFloatFixPt tmpcoords[6];
        TInt i;

        iCoords->Reset();
        iIdx = 0;
        if ( iSrc->IsDone() )
            {
            iIsDone = ETrue;
            }
        else
            {
            iSegType = iSrc->CurrentSegment( tmpcoords );
            switch ( iSegType )
                {
                case EGfxSegMoveTo:
                    iLastX = tmpcoords[0];
                    iLastY = tmpcoords[1];
                    iCoords->AppendL( iLastX );
                    iCoords->AppendL( iLastY );
                    break;
                case EGfxSegLineTo:
                    iLastX = tmpcoords[0];
                    iLastY = tmpcoords[1];
                    iCoords->AppendL( iLastX );
                    iCoords->AppendL( iLastY );
                    break;
                case EGfxSegQuadTo:
                    iCoords->AppendL( iLastX );
                    iCoords->AppendL( iLastY );
                    iCoords->AppendL( tmpcoords[0] );
                    iCoords->AppendL( tmpcoords[1] );
                    iCoords->AppendL( tmpcoords[2] );
                    iCoords->AppendL( tmpcoords[3] );

                    for ( i = 0; i < iLimit; i++ )
                        CreateSubcurveQuadL( iCoords );
                    //CreateSubcurveQuad(iCoords);
                    //CreateSubcurveQuad(iCoords);
                    //CreateSubcurveQuad(iCoords);

                    iIdx = 2;
                    iLastX = tmpcoords[2];
                    iLastY = tmpcoords[3];
                    iSegType = EGfxSegLineTo;
                    break;
                case EGfxSegCubicTo:
                    iCoords->AppendL( iLastX );
                    iCoords->AppendL( iLastY );
                    iCoords->AppendL( tmpcoords[0] );
                    iCoords->AppendL( tmpcoords[1] );
                    iCoords->AppendL( tmpcoords[2] );
                    iCoords->AppendL( tmpcoords[3] );
                    iCoords->AppendL( tmpcoords[4] );
                    iCoords->AppendL( tmpcoords[5] );

                    for ( i = 0; i < iLimit; i++ )
                        CreateSubcurveCubicL( iCoords );

                    iIdx = 2;
                    iLastX = tmpcoords[4];
                    iLastY = tmpcoords[5];
                    iSegType = EGfxSegLineTo;
                    break;
                case EGfxSegClose:              // (cdm)
                    iLastX = tmpcoords[0];
                    iLastY = tmpcoords[1];
                    iCoords->AppendL( iLastX );
                    iCoords->AppendL( iLastY );
                    break;

                 default:
                    break;
                }
            iSrc->NextL();
            }
        }
    }

// --------------------------------------------------------------------------
//  void CGfxFlatteningPathIterator::SetFlatness( TFloatFixPt& aFlatness )
// ---------------------------------------------------------------------------
 void CGfxFlatteningPathIterator::SetFlatness( TFloatFixPt& aFlatness )
    {
    iFlatness = aFlatness;
    }

// --------------------------------------------------------------------------
//  void CGfxFlatteningPathIterator::SetRecursionLimit( TInt32 aLimit )
// ---------------------------------------------------------------------------
 void CGfxFlatteningPathIterator::SetRecursionLimit( TInt32 aLimit )
    {
    iLimit = aLimit;
    }

// --------------------------------------------------------------------------
//  TFloatFixPt CGfxFlatteningPathIterator::Flatness()
// ---------------------------------------------------------------------------
 TFloatFixPt CGfxFlatteningPathIterator::Flatness()
    {
    return iFlatness;
    }

// --------------------------------------------------------------------------
//  TInt32 CGfxFlatteningPathIterator::RecursionLimit()
// ---------------------------------------------------------------------------
 TInt32 CGfxFlatteningPathIterator::RecursionLimit()
    {
    return iLimit;
    }




// --------------------------------------------------------------------------
// void CGfxFlatteningPathIterator::CreateSubcurveQuad( RArray<TFloatFixPt>* aCtrlPoints )
// ---------------------------------------------------------------------------
void CGfxFlatteningPathIterator::CreateSubcurveQuadL( RArray<TFloatFixPt>* aCtrlPoints )
    {
    TInt32 ix = 0;
    TFloatFixPt x0, y0, xm, ym, x1, y1;
	TInt32 lCount;

    while ( ix + 5 < aCtrlPoints->Count() )
        {
		lCount = ix;
        x0 = ( *aCtrlPoints )[lCount++];
        y0 = ( *aCtrlPoints )[lCount++];
        xm = ( *aCtrlPoints )[lCount++];
        ym = ( *aCtrlPoints )[lCount++];
        x1 = ( *aCtrlPoints )[lCount++];
        y1 = ( *aCtrlPoints )[lCount++];
	#ifdef SVG_FLOAT_BUILD
        x0 = ( x0 + xm ) * .5f;
        y0 = ( y0 + ym ) * .5f;
        x1 = ( x1 + xm ) * .5f;
        y1 = ( y1 + ym ) * .5f;
        xm = ( x0 + x1 ) * .5f;
        ym = ( y0 + y1 ) * .5f;
	#else
        x0 = ( x0 + xm ) >> 1;
        y0 = ( y0 + ym ) >> 1;
        x1 = ( x1 + xm ) >> 1;
        y1 = ( y1 + ym ) >> 1;
        xm = ( x0 + x1 ) >> 1;
        ym = ( y0 + y1 ) >> 1;
	#endif
		lCount=ix + 2;
        ( *aCtrlPoints )[lCount] = xm;
        ( *aCtrlPoints )[lCount + 1] = ym;
        aCtrlPoints->InsertL( TFloatFixPt( x0 ), lCount++);
        aCtrlPoints->InsertL( TFloatFixPt( y0 ), lCount );
        aCtrlPoints->InsertL( TFloatFixPt( x1 ), ix + 6 );
        aCtrlPoints->InsertL( TFloatFixPt( y1 ), ix + 7 );
        ix += 8;
        }
    }

// --------------------------------------------------------------------------
// void CGfxFlatteningPathIterator::CreateSubcurveCubic( RArray<TFloatFixPt>* aCtrlPoints )
// ---------------------------------------------------------------------------
void CGfxFlatteningPathIterator::CreateSubcurveCubicL( RArray<TFloatFixPt>* aCtrlPoints )
    {
    TInt32 ix = 0;
    TFloatFixPt x0, y0, xm, ym, xm1, ym1, xm2, ym2, x1, y1;
	TInt32 lCount;
    while ( ix + 7 < aCtrlPoints->Count() )
        {
		lCount = ix;
        x0 = ( *aCtrlPoints )[lCount++];
        y0 = ( *aCtrlPoints )[lCount++];
        xm1 = ( *aCtrlPoints )[lCount++];
        ym1 = ( *aCtrlPoints )[lCount++];
        xm2 = ( *aCtrlPoints )[lCount++];
        ym2 = ( *aCtrlPoints )[lCount++];
        x1 = ( *aCtrlPoints )[lCount++];
        y1 = ( *aCtrlPoints )[lCount ];

	#ifdef SVG_FLOAT_BUILD
        x0 = ( x0 + xm1 ) * .5f;
        y0 = ( y0 + ym1 ) * .5f;
        x1 = ( x1 + xm2 ) * .5f;
        y1 = ( y1 + ym2 ) * .5f;
        xm = ( xm1 + xm2 ) * .5f;
        ym = ( ym1 + ym2 ) * .5f;
        xm1 = ( x0 + xm ) * .5f;
        ym1 = ( y0 + ym ) * .5f;
        xm2 = ( x1 + xm ) * .5f;
        ym2 = ( y1 + ym ) * .5f;
        xm = ( xm1 + xm2 ) * .5f;
        ym = ( ym1 + ym2 ) * .5f;
	#else
        x0 = ( x0 + xm1 ) >> 1;
        y0 = ( y0 + ym1 ) >> 1;
        x1 = ( x1 + xm2 ) >> 1;
        y1 = ( y1 + ym2 ) >> 1;
        xm = ( xm1 + xm2 ) >> 1;
        ym = ( ym1 + ym2 ) >> 1;
        xm1 = ( x0 + xm ) >> 1;
        ym1 = ( y0 + ym ) >> 1;
        xm2 = ( x1 + xm ) >> 1;
        ym2 = ( y1 + ym ) >> 1;
        xm = ( xm1 + xm2 ) >> 1;
        ym = ( ym1 + ym2 ) >> 1;
	#endif
		lCount=ix + 2;
        ( *aCtrlPoints )[lCount++] = x0;
        ( *aCtrlPoints )[lCount++] = y0;
        ( *aCtrlPoints )[lCount++] = xm1;
        ( *aCtrlPoints )[lCount++] = ym1;

        aCtrlPoints->InsertL( TFloatFixPt( y1 ), lCount);
        aCtrlPoints->InsertL( TFloatFixPt( x1 ), lCount );
        aCtrlPoints->InsertL( TFloatFixPt( ym2 ), lCount );
        aCtrlPoints->InsertL( TFloatFixPt( xm2 ), lCount );
        aCtrlPoints->InsertL( TFloatFixPt( ym ), lCount );
        aCtrlPoints->InsertL( TFloatFixPt( xm ), lCount );
        ix += 12;
        }
    }


// --------------------------------------------------------------------------
//  void CGfxFlatteningPathIterator::PolygonizeL( CGfxEdgeListP* /* aRenderer */, TInt /* aFlatness */ )
// ---------------------------------------------------------------------------
 void CGfxFlatteningPathIterator::PolygonizeL( CGfxEdgeListP* /* aRenderer */, TInt /* aFlatness */ )
{
}
