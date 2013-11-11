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


#include "GfxGeneralPath.h"
#include "GfxGeneralPathIteratorP.h"
#include "GfxFlatteningPathIterator.h"



// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  CGfxGeneralPath* CGfxGeneralPath::NewL()
// ---------------------------------------------------------------------------
 CGfxGeneralPath* CGfxGeneralPath::NewL()
    {
    CGfxGeneralPath* self = NewLC();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
//  CGfxGeneralPath* CGfxGeneralPath::NewLC()
// ---------------------------------------------------------------------------
 CGfxGeneralPath* CGfxGeneralPath::NewLC()
    {
    CGfxGeneralPath* self = new ( ELeave ) CGfxGeneralPath;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CGfxGeneralPath::CGfxGeneralPath()
// ---------------------------------------------------------------------------
CGfxGeneralPath::CGfxGeneralPath()
{
}

// --------------------------------------------------------------------------
// void CGfxGeneralPath::ConstructL()
// ---------------------------------------------------------------------------
void CGfxGeneralPath::ConstructL()
    {
    iPointTypes = new ( ELeave ) RArray<TUint32>( 4 );
    iPointTypes->AppendL(NULL);
    iPointTypes->Remove(0);
    iPointCoords = new ( ELeave ) RArray<TFloatFixPt>( 8 );
    iPointCoords->AppendL(NULL);
    iPointCoords->Remove(0);
    iPathSegmentTypeArray = NULL;
    iCount = 0;
    }

// --------------------------------------------------------------------------
//  CGfxGeneralPath::~CGfxGeneralPath()
// ---------------------------------------------------------------------------
 CGfxGeneralPath::~CGfxGeneralPath()
    {
	if ( iPointTypes )
		{
		iPointTypes->Close();
		delete iPointTypes;
		iPointTypes = NULL;
		}

	if ( iPointCoords )
		{
	    iPointCoords->Close();
		delete iPointCoords;
		iPointCoords = NULL;
		}
    if (iPathSegmentTypeArray)
       {
    	delete [] iPathSegmentTypeArray;
    	iPathSegmentTypeArray = NULL;
       }
    }

const TUint32 KSegClose = ( TUint32 ) EGfxSegClose;

// --------------------------------------------------------------------------
// TFloatFixPt CGfxGeneralPath::AreaSize()
// ---------------------------------------------------------------------------
TFloatFixPt CGfxGeneralPath::AreaSize()
    {
    return TFloatFixPt(0);
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::ClosePathL()
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::ClosePathL()
    {
    TInt count = iPointTypes->Count();
    if ( count > 0 && (*iPointTypes)[count - 1] != KSegClose )
        {
        iPointTypes->AppendL( KSegClose );
            }
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::HorizonToL( TFloatFixPt& aX, TBool aAbsolute )
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::HorizonToL( TFloatFixPt& aX, TBool aAbsolute )
    {
    if ( !aAbsolute && iPointCoords->Count() >= 2 )
        {
        // If the last command was 'Z'/'z'
        // the current point is the initial point
        // of the previous segment - 'M'/'m' usually
        if ( iPointTypes->operator[]( iPointTypes->Count() - 1 ) == KSegClose )
            {
            aX += iLastX;
            }
        else
            {
            aX += iPointCoords->operator[]( iPointCoords->Count() - 2 );
            }
        }

    TUint32 KSegLineto = ( TUint32 ) EGfxSegLineTo;  // const

    iPointTypes->AppendL( KSegLineto );
    iPointCoords->AppendL( aX );
    TFloatFixPt tmpY = ( iPointCoords->Count() < 2 ) ? TFloatFixPt( 0 ) : iPointCoords->operator[]( iPointCoords->Count() - 2 );
    iPointCoords->AppendL( tmpY );
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::VertToL( TFloatFixPt& aY, TBool aAbsolute )
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::VertToL( TFloatFixPt& aY, TBool aAbsolute )
    {
    if ( !aAbsolute && iPointCoords->Count() >= 2 )
        {
        // If the last command was 'Z'/'z'
        // the current point is the initial point
        // of the previous segment - 'M'/'m' usually
        if ( iPointTypes->operator[]( iPointTypes->Count() - 1 ) == KSegClose )
            {
            aY += iLastY;
            }
        else
            {
            aY += iPointCoords->operator[]( iPointCoords->Count() - 1 );
            }
        }

    TUint32 KSegLineto = ( TUint32 ) EGfxSegLineTo;  // const

    iPointTypes->AppendL( KSegLineto );
    TFloatFixPt tmpX = ( iPointCoords->Count() < 2 ) ? TFloatFixPt( 0 ) : iPointCoords->operator[]( iPointCoords->Count() - 2 );
    iPointCoords->AppendL( tmpX );
    iPointCoords->AppendL( aY );
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::LineToL( TFloatFixPt& aX,
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::LineToL( TFloatFixPt& aX,
                                        TFloatFixPt& aY,
                                        TBool aAbsolute )
    {
    if ( !aAbsolute && iPointCoords->Count() >= 2 )
        {
        TFloatFixPt lastX = iPointCoords->operator[]( iPointCoords->Count() - 2 );
        TFloatFixPt lastY = iPointCoords->operator[]( iPointCoords->Count() - 1 );
        // If the last command was 'Z'/'z'
        // the current point is the initial point
        // of the previous segment - 'M'/'m' usually
        if ( iPointTypes->operator[]( iPointTypes->Count() - 1 ) == KSegClose )
            {
            lastX = iLastX;
            lastY = iLastY;
            }
        aX += lastX;
        aY += lastY;
        }

    TUint32 KSegLineto = ( TUint32 ) EGfxSegLineTo;  // const

    iPointTypes->AppendL( KSegLineto );
    iPointCoords->AppendL( aX );
    iPointCoords->AppendL( aY );
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::RemoveLastPathCommand()
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::RemoveLastPathCommand()
	{
	TInt aTypeCount = iPointTypes->Count();
	TInt aPointCount = iPointCoords->Count();

	if ( aPointCount >= 2 )
    	{
        //we need to remove the last amount from iLastX
        iLastX -= (*iPointCoords)[aPointCount - 2];
        iLastY -= (*iPointCoords)[aPointCount - 1];
	    }

	if ( aTypeCount > 0 )
		{
		iPointTypes->Remove( aTypeCount - 1 );

		if ( aPointCount >= 2 )
			{
            iPointCoords->Remove( aPointCount - 1 );
            iPointCoords->Remove( aPointCount - 2 );
			}
		}
	}
// --------------------------------------------------------------------------
//  void CGfxGeneralPath::MoveToL( TFloatFixPt& aX,
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::MoveToL( TFloatFixPt& aX,
                                        TFloatFixPt& aY,
                                        TBool aAbsolute )
    {
    if ( !aAbsolute && iPointCoords->Count() >= 2 )
        {
        // If the last command was 'Z'/'z'
        // the current point is the initial point
        // of the previous segment - 'M'/'m' usually
        if ( iPointTypes->operator[]( iPointTypes->Count() - 1 ) == KSegClose )
            {
            aX += iLastX;
            aY += iLastY;
            }
        else
            {
            TFloatFixPt lastX = iPointCoords->operator[]( iPointCoords->Count() -
                                                     2 );
            TFloatFixPt lastY = iPointCoords->operator[]( iPointCoords->Count() -
                                                     1 );
            aX += lastX;
            aY += lastY;
            }
        }

    TUint32 KSegMoveto = ( TUint32 ) EGfxSegMoveTo;  // const

    iPointTypes->AppendL( KSegMoveto );
    iPointCoords->AppendL( aX );
    iPointCoords->AppendL( aY );
    // Save initial point of a new segment
    // Could be the next initial point
    iLastX = aX;
    iLastY = aY;
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::QuadToL( TFloatFixPt& aX1,
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::QuadToL( TFloatFixPt& aX1,
                                        TFloatFixPt& aY1,
                                        TFloatFixPt& aX2,
                                        TFloatFixPt& aY2,
                                        TBool aAbsolute )
    {
    if ( !aAbsolute && iPointCoords->Count() >= 2 )
        {
        TFloatFixPt lastX = iPointCoords->operator[]( iPointCoords->Count() - 2 );
        TFloatFixPt lastY = iPointCoords->operator[]( iPointCoords->Count() - 1 );

        // If the last command was 'Z'/'z'
        // the current point is the initial point
        // of the previous segment - 'M'/'m' usually
        if ( iPointTypes->operator[]( iPointTypes->Count() - 1 ) == KSegClose )
            {
            lastX = iLastX;
            lastY = iLastY;
            }

        aX1 += lastX;
        aY1 += lastY;
        aX2 += lastX;
        aY2 += lastY;
        }

       TUint32 KSegQuadto = ( TUint32 ) EGfxSegQuadTo;
       iPointTypes->AppendL( KSegQuadto );
       iPointCoords->AppendL( aX1 );
       iPointCoords->AppendL( aY1 );  
       iPointCoords->AppendL( aX2 );
       iPointCoords->AppendL( aY2 );	
       
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::CubicToL( TFloatFixPt& aX1,
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::CubicToL( TFloatFixPt& aX1,
                                         TFloatFixPt& aY1,
                                         TFloatFixPt& aX2,
                                         TFloatFixPt& aY2,
                                         TFloatFixPt& aX3,
                                         TFloatFixPt& aY3,
                                         TBool aAbsolute )
    {
    if ( !aAbsolute && iPointCoords->Count() >= 2 )
        {
        TFloatFixPt lastX = iPointCoords->operator[]( iPointCoords->Count() - 2 );
        TFloatFixPt lastY = iPointCoords->operator[]( iPointCoords->Count() - 1 );
        // If the last command was 'Z'/'z'
        // the current point is the initial point
        // of the previous segment - 'M'/'m' usually
        if ( iPointTypes->operator[]( iPointTypes->Count() - 1 ) == KSegClose )
            {
            lastX = iLastX;
            lastY = iLastY;
            }
        aX1 += lastX;
        aY1 += lastY;
        aX2 += lastX;
        aY2 += lastY;
        aX3 += lastX;
        aY3 += lastY;
        }

    TUint32 KSegCubicto = ( TUint32 ) EGfxSegCubicTo;  // const

    iPointTypes->AppendL( KSegCubicto );
    iPointCoords->AppendL( aX1 );
    iPointCoords->AppendL( aY1 );
    iPointCoords->AppendL( aX2 );
    iPointCoords->AppendL( aY2 );
    iPointCoords->AppendL( aX3 );
    iPointCoords->AppendL( aY3 );
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::ShortQuadToL( TFloatFixPt& aX2,
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::ShortQuadToL( TFloatFixPt& aX2,
                                             TFloatFixPt& aY2,
                                             TBool aAbsolute )
    {
    if ( !aAbsolute && iPointCoords->Count() >= 2 )
        {
        // If the last command was 'Z'/'z'
        // the current point is the initial point
        // of the previous segment - 'M'/'m' usually
        if ( iPointTypes->operator[]( iPointTypes->Count() - 1 ) == KSegClose )
            {
            aX2 += iLastX;
            aY2 += iLastY;
            }
        else
            {
            aX2 += iPointCoords->operator[]( iPointCoords->Count() - 2 );
            aY2 += iPointCoords->operator[]( iPointCoords->Count() - 1 );
            }
        }

    TFloatFixPt aX1, aY1;
    GetCurveRefPoint( aX1, aY1 );
    QuadToL( aX1, aY1, aX2, aY2, ETrue );
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::ShortCubicToL( TFloatFixPt& aX2,
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::ShortCubicToL( TFloatFixPt& aX2,
                                              TFloatFixPt& aY2,
                                              TFloatFixPt& aX3,
                                              TFloatFixPt& aY3,
                                              TBool aAbsolute )
    {
    if ( !aAbsolute && iPointCoords->Count() >= 2 )
        {
        TFloatFixPt lastX = iPointCoords->operator[]( iPointCoords->Count() - 2 );
        TFloatFixPt lastY = iPointCoords->operator[]( iPointCoords->Count() - 1 );
        // If the last command was 'Z'/'z'
        // the current point is the initial point
        // of the previous segment - 'M'/'m' usually
        if ( iPointTypes->operator[]( iPointTypes->Count() - 1 ) == KSegClose )
            {
            lastX = iLastX;
            lastY = iLastY;
            }
        aX2 += lastX;
        aY2 += lastY;
        aX3 += lastX;
        aY3 += lastY;
        }
    TFloatFixPt aX1, aY1;
    GetCurveRefPoint( aX1, aY1 );
    // If there is no previous command or if the previous command
   //  was not an C, c, S or s, the first control point shall be coincident 
   //  with the current point.
    TUint32 KSegCubicto = ( TUint32 ) EGfxSegCubicTo;  // const    
    if ( iPointTypes->operator[]( iPointTypes->Count() - 1 ) != KSegCubicto )
        {
        aX1 = iLastX;
        aY1 = iLastY;
        }

    CubicToL( aX1, aY1, aX2, aY2, aX3, aY3, ETrue );
    }

// --------------------------------------------------------------------------
// void CGfxGeneralPath::GetCurveRefPoint( TFloatFixPt& aX, TFloatFixPt& aY )
// ---------------------------------------------------------------------------
void CGfxGeneralPath::GetCurveRefPoint( TFloatFixPt& aX, TFloatFixPt& aY )
    {
    if ( iPointCoords->Count() < 4 )
        {
        aX = TFloatFixPt( 0 );
        aY = TFloatFixPt( 0 );
        return;
        }
    TFloatFixPt x0 = iPointCoords->operator[]( iPointCoords->Count() - 4 );
    TFloatFixPt y0 = iPointCoords->operator[]( iPointCoords->Count() - 3 );
    TFloatFixPt x1 = iPointCoords->operator[]( iPointCoords->Count() - 2 );
    TFloatFixPt y1 = iPointCoords->operator[]( iPointCoords->Count() - 1 );
    aX = x1 + x1 - x0;
    aY = y1 + y1 - y0;
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::Reset()
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::Reset()
    {
    iPointTypes->Reset();
    iPointCoords->Reset();
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::AppendL( CGfxPathIterator* aItr )
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::AppendL( CGfxPathIterator* aItr )
    {
    TFloatFixPt tmpcoords[6];

    // Set path data...
    while ( !aItr->IsDone() )
        {
        switch ( aItr->CurrentSegment( tmpcoords ) )
            {
            case EGfxSegMoveTo:
                MoveToL( tmpcoords[0], tmpcoords[1], ETrue );
                break;
            case EGfxSegLineTo:
                LineToL( tmpcoords[0], tmpcoords[1], ETrue );
                break;
            case EGfxSegQuadTo:
                QuadToL( tmpcoords[0],
                         tmpcoords[1],
                         tmpcoords[2],
                         tmpcoords[3],
                         ETrue );
                break;
            case EGfxSegCubicTo:
                CubicToL( tmpcoords[0],
                          tmpcoords[1],
                          tmpcoords[2],
                          tmpcoords[3],
                          tmpcoords[4],
                          tmpcoords[5],
                          ETrue );
                break;
            case EGfxSegClose:
                ClosePathL();
            }
        aItr->NextL();
        }
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::GetBounds( const TGfxAffineTransform& aAt,
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::GetBounds( const TGfxAffineTransform& aAt,
                                          TGfxRectangle2D& aRect )
    {
     TFloatFixPt xMin(KMAXFLOATFIX), yMin(KMAXFLOATFIX), xMax(KMINFLOATFIX), yMax(KMINFLOATFIX);
     for (TInt i=0; i<iPointCoords->Count(); i+=2)
     {
        TGfxPoint2D p((*iPointCoords)[i], (*iPointCoords)[i+1]);
        aAt.Transform(&p, &p, 1);
        if (p.iX < xMin) xMin = p.iX;
        if (p.iX > xMax) xMax = p.iX;
        if (p.iY < yMin) yMin = p.iY;
        if (p.iY > yMax) yMax = p.iY;
     }
     aRect.iX = xMin;
     aRect.iY = yMin;
     aRect.iWidth = xMax - xMin;
     aRect.iHeight = yMax - yMin;
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::GetPathIteratorL( TGfxAffineTransform* aAt,
                                                 CGfxPathIterator*& aPitr )
    {
    aPitr = new ( ELeave ) CGfxGeneralPathIteratorP( this, aAt );
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::GetPathIteratorL( TGfxAffineTransform* aAt,
                                                 TInt aLimit,
                                                 CGfxPathIterator*& aPitr )
    {
    aPitr = CGfxFlatteningPathIterator::NewL( this, aAt, aLimit );

    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPath::SetPointTypeArrayL( RArray<TUint32>* aTypesArray )
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::SetPointTypeArrayL( RArray<TUint32>* aTypesArray )
    {
    TInt count = aTypesArray->Count();
    iPointTypes->Reset();

    for ( TInt i = 0; i < count; i++ )
        {
        iPointTypes->AppendL( (*aTypesArray)[i] );
        }
    }


// --------------------------------------------------------------------------
//  void CGfxGeneralPath::SetPointTypeArrayRef( RArray<TUint32>*& aTypesArray )
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::SetPointTypeArrayRef( RArray<TUint32>*& aTypesArray )
	{
	if (iPointTypes)
		{
		iPointTypes->Reset();
		delete iPointTypes;
		iPointTypes= NULL;
		}
	iPointTypes= aTypesArray;
	}


// --------------------------------------------------------------------------
//  void CGfxGeneralPath::SetPointCoordsArrayRef( RArray<TFloatFixPt>*& aPointCoords )
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::SetPointCoordsArrayRef( RArray<TFloatFixPt>*& aPointCoords )
    {
    TInt count = aPointCoords->Count();
    if (iPointCoords)
        {
        iPointCoords->Reset();
        delete iPointCoords;
        iPointCoords= NULL;
        }
    iPointCoords= aPointCoords;
    }


// --------------------------------------------------------------------------
//  void CGfxGeneralPath::SetPointCoordsArrayL( RArray<TFloatFixPt>* aPointCoords )
// ---------------------------------------------------------------------------
 void CGfxGeneralPath::SetPointCoordsArrayL( RArray<TFloatFixPt>* aPointCoords )
    {
    TInt count = aPointCoords->Count();
    iPointCoords->Reset();

    for ( TInt i = 0; (i+1) < count; i+=2 )
        {
        iPointCoords->AppendL( (*aPointCoords)[i] );
        iPointCoords->AppendL( (*aPointCoords)[i+1] );
        }
    }

// --------------------------------------------------------------------------
//  RArray<TUint32>* CGfxGeneralPath::PointTypeArray()
// ---------------------------------------------------------------------------
 RArray<TUint32>* CGfxGeneralPath::PointTypeArray()
    {
    return iPointTypes;
    }

 unsigned char* CGfxGeneralPath::PathSegmentTypeArray()
 {
 	return iPathSegmentTypeArray;
 }
 
 void CGfxGeneralPath::PathSegmentTypeArray(unsigned char* aSegmentTypeArray)
 {
 	iPathSegmentTypeArray = aSegmentTypeArray;
 }
// --------------------------------------------------------------------------
//  TFloatFixPt* CGfxGeneralPath::PointCoordsArray()
// ---------------------------------------------------------------------------
 TFloatFixPt* CGfxGeneralPath::PointCoordsArray()
    {
    if((iPointCoords) && (iPointCoords->Count() > 0))
		return &( iPointCoords->operator[]( 0 ) );
	else
		return NULL;
    }

// --------------------------------------------------------------------------
//  RArray<TFloatFixPt>* CGfxGeneralPath::PointCoordsArrayAll()
// ---------------------------------------------------------------------------
 RArray<TFloatFixPt>* CGfxGeneralPath::PointCoordsArrayAll()
    {
    return iPointCoords;
    }
// --------------------------------------------------------------------------
//  TInt CGfxGeneralPath::GetSegmentCount()
// ---------------------------------------------------------------------------
 TInt CGfxGeneralPath::GetSegmentCount()
    {
    return iPointTypes->Count();
    }
// --------------------------------------------------------------------------
//  TInt CGfxGeneralPath::GetSegmentType(TInt aSegmentIndex)
// ---------------------------------------------------------------------------
 TInt CGfxGeneralPath::GetSegmentType(TInt aSegmentIndex)
	{
	if(aSegmentIndex > 0 && aSegmentIndex <= iPointTypes->Count())
		{
		TInt SegmentType =  iPointTypes->operator[]( aSegmentIndex - 1 );
		TInt KSegLineto = ( TInt ) EGfxSegLineTo;
		if( SegmentType == KSegLineto)
			{
			// differentiate between vertical and horizontal lines.
			TBool isVertical = EFalse;
			TBool isHorizontal = EFalse;
			// the following function will tel us that this line segment is horizontal
			// or vertical.
			IsHorizontalLineOrVertical(aSegmentIndex-1, isHorizontal, isVertical);
			if( !isHorizontal )
				{
				if(isVertical)
					{
					TUint32 KVertical = 6;
					return KVertical ;
					}
				}
			else
				{
				TUint32 KHorizontal = 5;
				return KHorizontal ;
				}
			return SegmentType;
			}
		return SegmentType;
		}
	else
		{
		return KErrBadHandle;
		}
	}
// --------------------------------------------------------------------------
//  TReal32 CGfxGeneralPath::GetSegmentParameter(TInt aSegmentIndex , TInt aParameterIndex)
// ---------------------------------------------------------------------------
 TReal32 CGfxGeneralPath::GetSegmentParameter(TInt aSegmentIndex , TInt aParameterIndex)
{
	if( aSegmentIndex <= 0 && aSegmentIndex >= iPointTypes->Count())
		{
		return 0.0;
		}

	TInt count =0;
	TInt Param = 0;

	while(count < (aSegmentIndex -1))
		{

		switch(iPointTypes->operator[]( count))
			{

			case EGfxSegClose:
			break;
			case EGfxSegLineTo:
			Param+=2;
			break;
			case EGfxSegQuadTo:
			Param+= 4;
			break;
			case EGfxSegCubicTo:
			Param +=6;
			break;
			case EGfxSegMoveTo:
			Param+= 2;
			break;
			default:
			break;
			}
		count++;
		}

	Param--;

	if((Param + aParameterIndex) >= iPointCoords->Count())
		{
		return 0;
		}

	return (TReal32)iPointCoords->operator[](Param + aParameterIndex);
}
// --------------------------------------------------------------------------
// void CGfxGeneralPath::IsHorizontalLineOrVertical(TInt aSegmentIndex, TBool& aHorizontal, TBool& aVertical)
// ---------------------------------------------------------------------------
void CGfxGeneralPath::IsHorizontalLineOrVertical(TInt aSegmentIndex, TBool& aHorizontal, TBool& aVertical)
{
	// see the previous segment.
	if( aSegmentIndex <= 0 && aSegmentIndex >= iPointTypes->Count())
		{
		return;
		}
	TInt count =0;
	TInt Param =0;
	TInt LastPram = 0;
	while(count <= aSegmentIndex )
		{
		// this will contain the Index to the last Segment.
		LastPram= Param;

		// search for the current segment.
		switch(iPointTypes->operator[]( count))
			{
			// here increment the Param based on the number of attribute each command needs.
			case EGfxSegClose:
			break;
			case EGfxSegLineTo:
			Param+=2;
			break;
			case EGfxSegQuadTo:
			Param+= 4;
			break;
			case EGfxSegCubicTo:
			Param +=6;
			break;
			case EGfxSegMoveTo:
			Param+= 2;
			break;
			default:
			break;
			}
		count++;
		}


	Param--;
	LastPram--;

	if ( ( (Param-1) >= 0) && ( (LastPram - 1) >= 0) )
	{

	if(iPointCoords->operator[](Param-1) == iPointCoords->operator[](LastPram-1))
		{
		aVertical = ETrue;
		return;
		}
	if(iPointCoords->operator[](Param) == iPointCoords->operator[](LastPram))
		{
		aHorizontal = ETrue;
		return;
		}
	}
	else if ( (Param - 1 ) >= 0 )
	{
		//this is the case where there is just a LineTo and nothing else
		if ( iPointCoords->operator[](Param - 1) >= TFloatFixPt(0) )
		{
			aHorizontal = ETrue;
			return;
		}
		else if ( iPointCoords->operator[](Param) >= TFloatFixPt(0) )
		{
			aVertical = ETrue;
			return;
		}
	}
}

TUint8 CGfxGeneralPath::Count()
{
	return iCount;
}

void CGfxGeneralPath::Count(TUint8 aCount)
{
	iCount = aCount;
}
// --------------------------------------------------------------------------
//  CGfxGeneralPath*  CGfxGeneralPath::CloneL()
// ---------------------------------------------------------------------------
 CGfxGeneralPath*  CGfxGeneralPath::CloneL()
{
    CGfxGeneralPath* path = CGfxGeneralPath::NewLC();
    TInt index = 0;

    for( index = 0; index < iPointTypes->Count(); index++ )
        {
        path->iPointTypes->AppendL( iPointTypes->operator[]( index ) );
        }
    for( index = 0; index < iPointCoords->Count(); index++ )
        {
        path->iPointCoords->AppendL( iPointCoords->operator[]( index ) );
        }
    path->iLastX = iLastX;
    path->iLastY = iLastY;
    if(TUint8 count = path->Count())
    {
        path->iPathSegmentTypeArray = new unsigned char[count];
        for(int i =0; i<count ; i++)
            path->iPathSegmentTypeArray[i] = iPathSegmentTypeArray[i];
    }
    else 
    {
    	path->iPathSegmentTypeArray = NULL;
    }
    CleanupStack::Pop();
    return path;
}

// --------------------------------------------------------------------------
//  void  CGfxGeneralPath::QuadToLWithNoControlPoint()
//  If there is no previous command or if the previous command was not a 
//  Q, q, T or t, the control point shall be current point.
// ---------------------------------------------------------------------------
void CGfxGeneralPath::QuadToLWithNoControlPoint(TFloatFixPt aX, TFloatFixPt aY)
{
    TUint32 KSegQuadto = ( TUint32 ) EGfxSegQuadTo;
    iPointTypes->AppendL( KSegQuadto );
    iPointCoords->AppendL( iLastX );
    iPointCoords->AppendL( iLastY );  
    iPointCoords->AppendL( aX );
    iPointCoords->AppendL( aY );	
}
