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


#include "GfxRendererInfoP.h"

#define EInitialised  0
#define ECurrent      1
#define EComplete     2


// ---------------------------------------------------------------------------
// Create a new instance and push it onto the cleanup stack.
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxEdgeListP* CGfxEdgeListP::NewLC( TGfxRendererInfoP* aRenderInfo )
// ---------------------------------------------------------------------------
CGfxEdgeListP* CGfxEdgeListP::NewLC( TGfxRendererInfoP* aRenderInfo )
    {
    CGfxEdgeListP* self = new ( ELeave ) CGfxEdgeListP( aRenderInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Create a new instance.
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxEdgeListP* CGfxEdgeListP::NewL( TGfxRendererInfoP* aRenderInfo )
// ---------------------------------------------------------------------------
CGfxEdgeListP* CGfxEdgeListP::NewL( TGfxRendererInfoP* aRenderInfo )
    {
    CGfxEdgeListP* self = NewLC( aRenderInfo );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxEdgeListP::~CGfxEdgeListP()
// ---------------------------------------------------------------------------
CGfxEdgeListP::~CGfxEdgeListP()
    {
     delete[] iPoints;
     iPoints = NULL;
     delete[] iSegset;
     iSegset = NULL;
     delete[] iEdges;
     iEdges = NULL;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxEdgeListP::CGfxEdgeListP( TGfxRendererInfoP* aRenderInfo ) : iWaitingSegList( NULL ),
// ---------------------------------------------------------------------------
CGfxEdgeListP::CGfxEdgeListP( TGfxRendererInfoP* aRenderInfo ) : iWaitingSegList( NULL ),
                                                                 iActiveSegList( NULL ),
                                                                 iPointsCount( 0 ),
                                                                 iSegsetCount( 0 ),
																 iPointsCountMax( 20 ),
                                                                 iSegsetCountMax( 10 ),
                                                                 iEdgesCountMax( 10 )
    {
    // From CGfxEdgeListP
    iRenderInfo = aRenderInfo;
    iMinX = KMAXFLOATFIX;
    iMinY = KMAXFLOATFIX;
    iMaxX = KMINFLOATFIX;
    iMaxY = KMINFLOATFIX;
    iEdgesCount = 0;
    iLastSegLenSq = 0xffffffff;
    }

// ---------------------------------------------------------------------------
// Second phase in constructing this object.
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// void CGfxEdgeListP::ConstructL()
// ---------------------------------------------------------------------------
void CGfxEdgeListP::ConstructL()
    {
    iPoints = new ( ELeave ) TPoint16[iPointsCountMax];
    iSegset = new ( ELeave ) TSegSet[iSegsetCountMax];
    iEdges = new ( ELeave ) TGfxSegEdge[iEdgesCountMax];
    }


// ---------------------------------------------------------------------------
// reset
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// void CGfxEdgeListP::Reset()
// ---------------------------------------------------------------------------
void CGfxEdgeListP::Reset()
    {
    // From CGfxEdgeListP
    iEdgesCount = 0;
    iLastSegLenSq = 0xffffffff;
    iPolygonAreaSize = 0;
    iMinX = KMAXFLOATFIX;
    iMinY = KMAXFLOATFIX;
    iMaxX = KMINFLOATFIX;
    iMaxY = KMINFLOATFIX;

    iPointsCount = 0;
    iSegsetCount = 0;

    iActiveSegList = 0;
    iWaitingSegList = 0;
    iSortedEdges = 0;

    iStartSeg = iSegset;
    iIsNewOutlineStarted = EFalse;
    iSubPathSegmentCount = 0;
    }

// --------------------------------------------------------------------------
// void CGfxEdgeListP::AdjustShapeComplexityL( TInt32 aVertexSize )
// ---------------------------------------------------------------------------
void CGfxEdgeListP::AdjustShapeComplexityL( TInt32 aVertexSize )
    {
    if ( aVertexSize < iPointsCountMax )
        return;
    //
    delete[] iPoints;
    delete[] iSegset;
    delete[] iEdges;
    iPoints = NULL;
    iSegset = NULL;
    iEdges = NULL;
    iPointsCountMax = aVertexSize + 4;
    iSegsetCountMax = aVertexSize;
    iEdgesCountMax = aVertexSize;
    iPoints = new ( ELeave ) TPoint16[iPointsCountMax];
    iSegset = new ( ELeave ) TSegSet[iSegsetCountMax];
    iEdges = new ( ELeave ) TGfxSegEdge[iEdgesCountMax];
    }

// --------------------------------------------------------------------------
// void CGfxEdgeListP::StartPoint( TFixPt& aX, TFixPt& aY )
// ---------------------------------------------------------------------------
void CGfxEdgeListP::StartPoint( TFixPt& aX, TFixPt& aY )
    {
    if ( iPointsCount >= iPointsCountMax - 1 ) // No more segments?
        return;

    // New segment set. Add one point and set flag
    TPoint16& p1 = iPoints[iPointsCount++];
    p1.iX = ( TInt16 ) ( TInt ) aX;
    p1.iY = ( TInt16 ) ( TInt ) aY;

    // Min/Max update
    if ( p1.iY < iMinY )
        iMinY = p1.iY;
    else if ( p1.iY > iMaxY )
        iMaxY = p1.iY;
    if ( p1.iX < iMinX )
        iMinX = p1.iX;
    else if ( p1.iX > iMaxX )
        iMaxX = p1.iX;

    iIsNewOutlineStarted = ETrue;
    iSubPathSegmentCount = 0;
    }

// --------------------------------------------------------------------------
// void CGfxEdgeListP::AddPoint( TFixPt& aX, TFixPt& aY, TBool aFillOnly, TBool aClosePath )
// ---------------------------------------------------------------------------
void CGfxEdgeListP::AddPoint( TFixPt& aX, TFixPt& aY, TBool aFillOnly, TBool aClosePath )
    {
    if ( iPointsCount >= iPointsCountMax - 1 ) // No more segments?
        return;

    TBool newSegsetFlag = EFalse;
    TBool completeSubPathFlag = EFalse;

    if ( iIsNewOutlineStarted )
        {
        CheckAndConnect();
        newSegsetFlag = ETrue;
        }
    else if ( aClosePath && ( iSubPathSegmentCount == 1 ) )
        {
        completeSubPathFlag = ETrue;
        newSegsetFlag = ETrue;
        }
    else
        {
        TInt x = ( TInt ) aX, y = ( TInt ) aY;
        switch ( iSegset[iSegsetCount - 1].AddSegment( x,
                                                       y,
                                                       iLastSegLenSq,
                                                       aClosePath ) )
            {
            case ESegAdd:
                // Add point
                iPointsCount++;
                break;
            case ESegMerge:
                // Do nothing
                break;
            case ESegStartNew:
                newSegsetFlag = ETrue;
                break;
            default:
                return;// ESegIgnore - do nothing
            }
        }

    if ( newSegsetFlag && iSegsetCount < iSegsetCountMax )
        {
        // New segment set. Add two points
        TPoint16* p =& iPoints[iPointsCount++];
        p->iX = ( TInt16 ) ( TInt ) aX;
        p->iY = ( TInt16 ) ( TInt ) aY;

        iSubPathSegmentCount++;
        TSegSet& seg = iSegset[iSegsetCount++];
        seg.Construct( p - 1, p );

        if ( iSegsetCount > 1 )
            iSegset[iSegsetCount - 2].iNext = &seg;

        if ( iIsNewOutlineStarted )
            iStartSeg = &seg;

        if ( completeSubPathFlag )      // (cdm)
            {
            iSegset[iSegsetCount - 1].iSign = -1;
            iSegset[iSegsetCount - 2].iSign = 1;
            iSegset[iSegsetCount - 1].iConnectFlag = EFalse;
            iSegset[iSegsetCount - 2].iConnectFlag = EFalse;
            }
        }

    // Min/Max update
    TPoint16& p = iPoints[iPointsCount - 1];
    if ( p.iY < iMinY )
        iMinY = p.iY;
    else if ( p.iY > iMaxY )
        iMaxY = p.iY;
    if ( p.iX < iMinX )
        iMinX = p.iX;
    else if ( p.iX > iMaxX )
        iMaxX = p.iX;

    if ( aFillOnly )
        {
        iSegset[iSegsetCount - 1].iFillOnlySeg = &iPoints[iPointsCount - 1];
        }

    iIsNewOutlineStarted = EFalse;
    }

// --------------------------------------------------------------------------
// void CGfxEdgeListP::SetupEdgeList()
// ---------------------------------------------------------------------------
void CGfxEdgeListP::SetupEdgeList()
    {
    TInt i;

    //
    CheckAndConnect();

    // Sort waiting list
    TSegSet* segset = iSegset;
    for ( i = 0; i < iSegsetCount; i++ )
        {
        if ( segset->iValid )
            {
            InsertToList( ETrue, * segset );
            if ( segset->iActualBottomY < 0 )
                segset->iActualBottomY = segset->iBottomPoint->iY;
            }
        segset++;
        }

    // Normalize x coordinates
    for ( i = 0; i < iPointsCount; i++ )
        {
        iPoints[i].iX = ( TInt16 ) ( iPoints[i].iX - iMinX );
        }

    // Setting up for simple polygon, if applicable
    if ( iSegsetCount < 4 )
        {
        TInt ix = 0;
        segset = iWaitingSegList;

        iSegListForSimple[0] = iSegListForSimple[1] = NULL;     // (cdm).

        while ( segset )
            {
            if ( segset->iTopPoint->iY <= iMinY )
                {
                iSegListForSimple[ix++] = segset;
                segset->InitDDA();
                if ( ix > 2 )
                    break;
                }
            segset = segset->iNext;
            }
        }

    }

// --------------------------------------------------------------------------
// void CGfxEdgeListP::UpdateActiveEdge( TInt aY )
// ---------------------------------------------------------------------------
void CGfxEdgeListP::UpdateActiveEdge( TInt aY )
    {
    // if number of segment is less than 4, it means the polygon is
    // a simple polygon, which we can reduce some processing.
    if ( iSegsetCount < 4 )
        {
        UpdateActiveEdgeSimple( aY );
        return;
        }

    // Remove from active segment list
    while ( iActiveSegList )
        {
        if ( aY <= iActiveSegList->iActualBottomY )
            break;


        iActiveSegList = iActiveSegList->iNext;
        }

    // Add newly activated segment to active segment list
    while ( iWaitingSegList )
        {
        if ( aY < iWaitingSegList->iTopPoint->iY )
            break;
        TSegSet* seg = iWaitingSegList;
        iWaitingSegList = iWaitingSegList->iNext;

        InsertToList( EFalse, * seg );  // add to active list

        seg->InitDDA();
        }

    // Add edges from active segements
    TSegSet* asl = iActiveSegList;
    TGfxSegEdge* edge;
    TInt x1, x2;
    TBool fillOnly;

    iSortedEdges = 0;
    iEdgesCount = 0;
    while ( asl )
        {
        fillOnly = asl->GetEdge( x1, x2 );
        edge = AddEdge( x1, x2 );
        if ( !edge )    // no more edges can be added.
            return;
        edge->iSign = ( TInt16 ) asl->iSign;
        edge->iOnlyForFill = ( TInt16 ) fillOnly;

        asl = asl->iNext;
        }
    }

// --------------------------------------------------------------------------
// TGfxSegEdge* CGfxEdgeListP::EdgeList()
// ---------------------------------------------------------------------------
TGfxSegEdge* CGfxEdgeListP::EdgeList()
    {
    return iSortedEdges;
    }

// --------------------------------------------------------------------------
// void CGfxEdgeListP::UpdateActiveEdgeSimple( TInt aY )   // (cdm)
// ---------------------------------------------------------------------------
void CGfxEdgeListP::UpdateActiveEdgeSimple( TInt aY )   // (cdm)
    {
    // Add edges from active segments
    TGfxSegEdge* edge;
    TInt x01, x02, x11, x12;
    TBool fillOnly0, fillOnly1;
    iEdgesCount = 0;

    if ( iSegListForSimple[0] && iSegListForSimple[1] )
        {

        if ( aY > iSegListForSimple[0]->iActualBottomY ) // (cdm)
            iSegListForSimple[0] = iSegListForSimple[0]->iNext;

        if ( aY > iSegListForSimple[1]->iActualBottomY )
            iSegListForSimple[1] = iSegListForSimple[1]->iNext;

        if ( !iSegListForSimple[0] || !iSegListForSimple[1] )
            return;

        iEdgesCount = 2;
        fillOnly0 = iSegListForSimple[0]->GetEdge( x01, x02 );
        fillOnly1 = iSegListForSimple[1]->GetEdge( x11, x12 );

        if ( x01 < x11 )
            {
            // iSegListForSimple[0] is left
            iSortedEdges = iEdges;
            edge = iSortedEdges;
            edge->iStart = ( TInt16 ) x01;
            edge->iEnd = ( TInt16 ) x02;
            edge->iSign = ( TInt16 ) iSegListForSimple[0]->iSign;
            edge->iOnlyForFill = ( TInt16 ) fillOnly0;
            edge->iNext = iEdges + 1;

            edge = edge->iNext;
            edge->iStart = ( TInt16 ) x11;
            edge->iEnd = ( TInt16 ) x12;
            edge->iSign = ( TInt16 ) iSegListForSimple[1]->iSign;
            edge->iOnlyForFill = ( TInt16 ) fillOnly1;
            edge->iNext = NULL;
            }
        else
            {
            // iSegListForSimple[1] is left
            iSortedEdges = iEdges;
            edge = iSortedEdges;
            edge->iStart = ( TInt16 ) x11;
            edge->iEnd = ( TInt16 ) x12;
            edge->iSign = ( TInt16 ) iSegListForSimple[1]->iSign;
            edge->iOnlyForFill = ( TInt16 ) fillOnly1;
            edge->iNext = iEdges + 1;

            edge = edge->iNext;
            edge->iStart = ( TInt16 ) x01;
            edge->iEnd = ( TInt16 ) x02;
            edge->iSign = ( TInt16 ) iSegListForSimple[0]->iSign;
            edge->iOnlyForFill = ( TInt16 ) fillOnly0;
            edge->iNext = NULL;
            }
        }
    }

// --------------------------------------------------------------------------
// void CGfxEdgeListP::InsertToList( TBool aWaitingList, TSegSet& aSegSet )
// ---------------------------------------------------------------------------
void CGfxEdgeListP::InsertToList( TBool aWaitingList, TSegSet& aSegSet )
    {
    TSegSet** next;
    if ( aWaitingList )
        next = &iWaitingSegList;
    else
        next = &iActiveSegList;
	if(aWaitingList)
	{
		while ( *next )
			{
	            if ( ( *next )->iTopPoint->iY >= aSegSet.iTopPoint->iY )
                break;
		        next = &( ( *next )->iNext );
            }
	}
    else
    {
		while( *next )
			{
            if ( ( *next )->iActualBottomY >= aSegSet.iActualBottomY )
                break;
	        next = &( ( *next )->iNext );

            }
    }
    //
    aSegSet.iNext = *next;
    *next = &aSegSet;
    }

// --------------------------------------------------------------------------
// TGfxSegEdge* CGfxEdgeListP::AddEdge( TInt32 aSt, TInt32 aEd )//, TInt aSegSign)
// ---------------------------------------------------------------------------
TGfxSegEdge* CGfxEdgeListP::AddEdge( TInt32 aSt, TInt32 aEd )//, TInt aSegSign)
{
    if ( iEdgesCount >= iEdgesCountMax - 1 ) // No more edges?
        return NULL;

    TGfxSegEdge& newEdge = iEdges[iEdgesCount++];
    newEdge.iStart = ( TInt16 ) aSt;
    newEdge.iEnd = ( TInt16 ) aEd;

    // Insert this edge in sorted link list
    TGfxSegEdge** next;
    next = &iSortedEdges;
    TInt16 lastEnd = KMINFLOATFIX;
    while ( *next )
        {
        if ( newEdge.iStart < ( *next )->iStart )       // (cdm).
            break;

        lastEnd = ( *next )->iEnd;
        next = &( ( *next )->iNext );
        }
    //
    newEdge.iNext = *next;
    *next = &newEdge;

    return &newEdge;
    }

// --------------------------------------------------------------------------
// void CGfxEdgeListP::CheckAndConnect()
// ---------------------------------------------------------------------------
void CGfxEdgeListP::CheckAndConnect()
    {
    if ( iSegsetCount < 3 )     // (cdm)
        return;

    // Merge the first one and the last one if they have same sign
    TSegSet* lastSegset =& iSegset[iSegsetCount - 1];
    TPoint16* ps1 = iStartSeg->iTopPoint;
    TPoint16* ps2 = iStartSeg->iBottomPoint;
    TPoint16* pl1 = lastSegset->iTopPoint;
    TPoint16* pl2 = lastSegset->iBottomPoint;

    if ( iStartSeg->iPointAryInc == lastSegset->iPointAryInc &&
        iStartSeg->iConnectFlag &&
         ( ( ps1->iX == pl1->iX && ps1->iY == pl1->iY ) ||
           ( ps1->iX == pl2->iX && ps1->iY == pl2->iY ) ||
           ( ps2->iX == pl1->iX && ps2->iY == pl1->iY ) ||
           ( ps2->iX == pl2->iX && ps2->iY == pl2->iY ) ) )
        {
        if ( iStartSeg->iPointAryInc == 1 )
            {
            iStartSeg->iValid = EFalse;
            lastSegset->iConnect = iStartSeg;
            lastSegset->iActualBottomY = iStartSeg->iBottomPoint->iY;
            }
        else
            {
            lastSegset->iValid = EFalse;
            iStartSeg->iConnect = lastSegset;
            iStartSeg->iActualBottomY = lastSegset->iBottomPoint->iY;
            }
        }
    }

void TSegSet::Construct( TPoint16* aP1, TPoint16* aP2 )
    {
    if ( aP1->iY < aP2->iY )
        {
        iTopPoint = aP1;
        iBottomPoint = aP2;
        iSign = 1;
        iPointAryInc = 1;
        }
    else if ( aP1->iY == aP2->iY )
        {
        iTopPoint = aP1;
        iBottomPoint = aP2;
        iSign = 0;
        iPointAryInc = 1;
        }
    else
        {
        iTopPoint = aP2;
        iBottomPoint = aP1;
        iSign = -1;
        iPointAryInc = -1;
        }

    iNext = NULL;
    iConnect = NULL;
    iValid = ETrue;
    iActualBottomY = KMINFLOATFIX;
    iFillOnlySeg = NULL;
    iConnectFlag = ETrue;
    }

TAddSegStatus TSegSet::AddSegment( TInt aX,
                                   TInt aY,
                                   TUint& aLastSegLenSq,
                                   TBool aClosePath )
    {
    TPoint16* p;    // the last vertex in the array of vertex
    if ( iPointAryInc == 1 )
        p = iBottomPoint;
    else
        p = iTopPoint;

    // Check sign of y diff
    TInt16 x3 = ( TInt16 ) aX, y3 = ( TInt16 ) aY;

    if ( p->iX == x3 && p->iY == y3 )
        return ESegIgnore;

    TInt sign;
    if ( p->iY < y3 )
        sign = 1;
    else if ( p->iY == y3 )
        sign = 0;
    else
        sign = -1;
    if ( iSign == 0 )
        {
        iSign = ( TInt16 ) sign;
        if ( sign == -1 )
            {
            // swap top point and bottom point
            TPoint16* tmp = iTopPoint;
            iTopPoint = iBottomPoint;
            iBottomPoint = tmp;
            iPointAryInc = -1;
            }
        }
    else if ( sign != 0 && iSign != sign )
        return ESegStartNew;    // sign is different. start new segment

    // Vertex merging
    TUint lenSq = ( p->iX - x3 ) * ( p->iX - x3 ) +
                  ( p->iY - y3 ) * ( p->iY - y3 );
    if ( !aClosePath )
        {
        if ( aLastSegLenSq < 9 && lenSq < 9 )
            {
            p->iX = x3;
            p->iY = y3;
            TPoint16* pp = p + iPointAryInc;
            aLastSegLenSq = ( p->iX - pp->iX ) * ( p->iX - pp->iX ) +
                            ( p->iY - pp->iY ) * ( p->iY - pp->iY );
            return ESegMerge;
            }
        }
    aLastSegLenSq = lenSq;


    // Add point
    TPoint16* nextp;
    if ( iPointAryInc == 1 )
        nextp = ++iBottomPoint;
    else
        nextp = ++iTopPoint;
    nextp->iX = x3;
    nextp->iY = y3;

    return ESegAdd;
    }

TBool TSegSet::GetEdge( TInt& aX1, TInt& aX2 )
    {
    TPoint16 p1, p2;
    TBool fillOnly;
    TBool done = SingleScanline( p1, p2 );

    if ( p1.iX < p2.iX )
        {
        aX1 = p1.iX;
        aX2 = p2.iX;
        }
    else
        {
        aX1 = p2.iX;
        aX2 = p1.iX;
        }

    if ( done )   // if done, construct new DDA with next segment
        {
        do
            {
            fillOnly = ( ( iTopPoint += iPointAryInc ) == iFillOnlySeg ); // (cdm)

            if ( iTopPoint == iBottomPoint )
                {
                if ( iConnect )
                    {
                    iTopPoint = iConnect->iTopPoint;
                    iBottomPoint = iConnect->iBottomPoint;
                    iFillOnlySeg = iConnect->iFillOnlySeg;
                    iConnect = NULL;
                    }
                else
                    {
                    return fillOnly;
                    }
                }
            ConstructDDA( iTopPoint, iTopPoint + iPointAryInc );

            done = SingleScanline( p1, p2 );

            if ( p1.iX < p2.iX )  // update edge minX & maxX
                {
                if ( p1.iX < aX1 )
                    aX1 = p1.iX;
                if ( aX2 < p2.iX )
                    aX2 = p2.iX;
                }
            else
                {
                if ( p2.iX < aX1 )
                    aX1 = p2.iX;
                if ( aX2 < p1.iX )
                    aX2 = p1.iX;
                }
            }
        while ( done );
        }
    else
        {
        fillOnly = ( iTopPoint == iFillOnlySeg || iTopPoint + iPointAryInc == iFillOnlySeg );
        }
    return fillOnly;
    }

void TSegSet::InitDDA()
    {
    ConstructDDA( iTopPoint, iTopPoint + iPointAryInc );
    }

// ---------------------------------------------------------------------------
// DDA functions, which is mostly from Symbian's TLinearDDA
// ---------------------------------------------------------------------------
void TSegSet::ConstructDDA( TPoint16* aStart, TPoint16* aFinish )
    {
    iStart = aStart;
    iFinish = aFinish;
    iDifference.iWidth = ( TInt16 ) Abs( iStart->iX - iFinish->iX );
    iDifference.iHeight = ( TInt16 ) Abs( iStart->iY - iFinish->iY );
    iInc.iX = ( TInt16 ) ( ( iStart->iX > iFinish->iX ) ? -1 : 1 );
    iInc.iY = ( TInt16 ) ( ( iStart->iY > iFinish->iY ) ? -1 : 1 );
    if ( iDifference.iWidth )
        {
        iGradient = ( TInt16 )
                    ( ( iFinish->iY - iStart->iY ) /
                      ( iFinish->iX - iStart->iX ) );
        }
    else
        {
        iGradient = 0;  // (cdm)
        }

    iPos = *iStart;
    if ( !iGradient )
        {
        iCount = iDifference.iWidth;
        }
    else
        {
        iCount = iDifference.iHeight;
        }
    iCount >>= 1;
    iStatus = EInitialised;
    if ( aStart == aFinish )
        iStatus = EComplete;
    }

TBool TSegSet::SingleStep( TPoint16& aPosition )
    {
    if ( iStatus == ECurrent )
        {
        UpdatePosition();
        aPosition = iPos;
        if ( iPos.iX == iFinish->iX && iPos.iY == iFinish->iY )
            {
            iStatus = EComplete;
            return( ETrue );
            }
        return( EFalse );
        }
    else if ( iStatus == EInitialised )
        {
        aPosition = *iStart;
        iStatus = ECurrent;
        return( EFalse );
        }
    else
        {
        aPosition = *iFinish;
        return( ETrue );
        };
    }

TBool TSegSet::SingleScanline( TPoint16& aStartPosition,
                               TPoint16& aEndPosition )
    {
    TBool done = EFalse;
    if ( iDifference.iHeight == 0 )
        {
        aStartPosition = *iStart;
        aEndPosition = *iFinish;
        iStatus = EComplete;  //KM
        return( ETrue );
        }
    if ( iDifference.iWidth == 0 || iGradient )
        {
        done = SingleStep( aStartPosition );
        aEndPosition = aStartPosition;
        return( done );
        }
    done = SingleStep( aStartPosition );
    aEndPosition = aStartPosition;
    while ( iCount - iDifference.iHeight >= 0 && !done )
        {
        done = SingleStep( aEndPosition );
        }
    return( done );
    }

void TSegSet::UpdatePosition()
    {
    if ( iDifference.iHeight == 0 ) // horizontal line
        iPos.iX = ( TInt16 ) ( iPos.iX + iInc.iX );
    else if ( iDifference.iWidth == 0 ) // vertical line
        iPos.iY = ( TInt16 ) ( iPos.iY + iInc.iY );
    else
        {
        if ( !iGradient )
            {
            iCount = ( TInt16 ) ( iCount - iDifference.iHeight );
            if ( iCount < 0 )
                {
                iCount = ( TInt16 ) ( iCount + iDifference.iWidth );
                iPos.iY = ( TInt16 ) ( iPos.iY + iInc.iY );
                }
            iPos.iX = ( TInt16 ) ( iPos.iX + iInc.iX );
            }
        else
            {
            iCount = ( TInt16 ) ( iCount - iDifference.iWidth );
            if ( iCount < 0 )
                {
                iCount = ( TInt16 ) ( iCount + iDifference.iHeight );
                iPos.iX = ( TInt16 ) ( iPos.iX + iInc.iX );
                }
            iPos.iY = ( TInt16 ) ( iPos.iY + iInc.iY );
            }
        }
    }
