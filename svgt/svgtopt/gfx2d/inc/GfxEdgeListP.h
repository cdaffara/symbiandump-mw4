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
* Description:  Graphics Extension Library header file
*
*/


#ifndef GFXEDGELISTP_H
#define GFXEDGELISTP_H

#include <e32std.h>
#include <e32base.h>

#include "GfxFloatFixPt.h"

class TSegSet;
class TGfxRendererInfoP;


//****************************************************

enum TAddSegStatus
    {
    ESegAdd,
    ESegMerge,
    ESegStartNew,
    ESegIgnore
    };

/**
 *  Helping class to store a point using 16 bits for each value.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TPoint16
    {
    public:
        TInt16  iX;
        TInt16  iY;
    };

/**
 *  Helping class to store a size using 16 bits for each value.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TSize16
    {
    public:
        TInt16  iWidth;
        TInt16  iHeight;
    };

/**
 *  Helping class to store a segment edge info.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxSegEdge
    {
    public:
        TInt16          iStart;
        TInt16          iEnd;
        TInt16          iOnlyForFill;
        TInt16          iSign;
        TGfxSegEdge*    iNext;
    };

/**
 *  Helping class to store a segment set info.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TSegSet
    {
    public:
        /**
         * Constructor
         *
         * @since 1.0
         * @param aP1 : initial segment point 1
         * @param aP2 : initial segment point 2
         * @return
         */
        void            Construct( TPoint16* aP1, TPoint16* aP2 );

        /**
         * Add a new segment to this set.
         *
         * @since 1.0
         * @param aX : x coordinate of new segment end point
         * @param aY : y coordinate of new segment end point
         * @param aLastSegLenSq : storage for new segment length squared value
         * @param aClosePath : flag to indicate closing of path.
         * @return
         */
        TAddSegStatus   AddSegment( TInt aX,
                                    TInt aY,
                                    TUint& aLastSegLenSq,
                                    TBool aClosePath );

        /**
         *
         *
         * @since 1.0
         * @param aX1 :
         * @param aX2 :
         * @return
         */
        TBool           GetEdge( TInt& aX1, TInt& aX2 );


        /**
         * Initialize the Digital Differential Analyzer (DDA)
         *
         * @since 1.0
         * @return
         */
        void            InitDDA();

    public:
        TPoint16*       iTopPoint;
        TPoint16*       iBottomPoint;
        TPoint16*       iFillOnlySeg;
        TSegSet*        iNext;
        TSegSet*        iConnect;

        TInt16          iPointAryInc;     // -1 or 1
        TInt16          iSign;        // -1, 0, or 1
        TInt16          iValid;
        TInt16          iActualBottomY;
        TInt16          iConnectFlag;

    protected:
        // DDA info
        TPoint16*       iStart;
        TPoint16*       iFinish;

        TPoint16        iInc;
        TPoint16        iPos;
        TSize16         iDifference;
        TInt16          iGradient;
        TInt16          iCount;
        TInt16          iStatus;

    protected:

        /**
         * Construct the Digital Differential Analyzer (DDA)
         *
         * @since 1.0
         * @param aStart : start point
         * @param aFinish : end point
         * @return
         */
        void            ConstructDDA( TPoint16* aStart, TPoint16* aFinish );

        /**
         *
         *
         * @since 1.0
         * @param aPosition :
         * @return
         */
        TBool           SingleStep( TPoint16& aPosition );

        /**
         *
         *
         * @since 1.0
         * @param aStartPosition :
         * @param aEndPosition :
         * @return
         */
        TBool           SingleScanline( TPoint16& aStartPosition,
                                        TPoint16& aEndPosition );

        /**
         *
         *
         * @since 1.0
         * @return
         */
        void            UpdatePosition();
    };




/**
 * This class maintains a list of segment edges, used for scanning a line
 * inside two edges.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CGfxEdgeListP : public CBase
    {
    public:

        /**
         * Two-phase constructor.
         *
         * @since 1.0
         * @param aRenderInfo : rendering info
         * @return
         */
        static CGfxEdgeListP*   NewLC( TGfxRendererInfoP* aRenderInfo );

        /**
         * Two-phase constructor.
         *
         * @since 1.0
         * @param aRenderInfo : rendering info
         * @return
         */
        static CGfxEdgeListP*   NewL( TGfxRendererInfoP* aRenderInfo );

        /**
         * Destructor.
         *
         * @since 1.0
         * @return
         */
                                ~CGfxEdgeListP();

    public:

        /**
         * Get the start point in the edge list.
         *
         * @since 1.0
         * @param aX : storage for starting point x coordinate.
         * @param aY : storage for starting point y coordinate.
         * @return
         */
        void                    StartPoint( TFloatFixPt& aX, TFloatFixPt& aY );

        /**
         * Add a new point to this list.
         *
         * @since 1.0
         * @param aX : x coordinate of point
         * @param aY : y coordinate of point
         * @param aFillOnly : fill-onyl flag
         * @param aClosePath : close-path flag
         * @return
         */
         void                   AddPoint( TFloatFixPt & aX,
                                          TFloatFixPt & aY,
                                          TBool aFillOnly,
                                          TBool aClosePath );
        /**
         * Clear all points in this list.
         *
         * @since 1.0
         * @return
         */
        void                    Reset();

        /**
         * Intialize this edge list.
         *
         * @since 1.0
         * @return
         */
        void                    SetupEdgeList();

        /**
         * Update the active edge.
         *
         * @since 1.0
         * @param aY : line y coordinate
         * @return
         */
        void                    UpdateActiveEdge( TInt aY );

        /**
         * Get the active segment edge.
         *
         * @since 1.0
         * @return segment edge.
         */
        TGfxSegEdge*            EdgeList();

        /**
         * Adjust the level of detail to the given vertex size.
         *
         * @since 1.0
         * @param aVertexSize : vertex size
         * @return
         */
        void                AdjustShapeComplexityL( TInt32 aVertexSize );

    protected:

        /**
         * Constructor
         *
         * @since 1.0
         * @return
         */
                            CGfxEdgeListP();

        /**
         * Constructor
         *
         * @since 1.0
         * @param iRenderInfo : rendering info
         * @return
         */
                            CGfxEdgeListP( TGfxRendererInfoP* iRenderInfo );

        /**
         * Second-phase of construction
         *
         * @since 1.0
         * @return
         */
        void                ConstructL();

    private:

        /**
         * Add a new edge.
         *
         * @since 1.0
         * @param aSt : start point
         * @param aEd : end point
         * @return
         */
        TGfxSegEdge*        AddEdge( TInt32 aSt, TInt32 aEd );//, TInt aSegSign);

        /**
         * Add the given set of segments to this list.
         *
         * @since 1.0
         * @param aWaitingList : flag to wait
         * @param aSegSet : set of segments
         * @return
         */
        void                InsertToList( TBool aWaitingList, TSegSet& aSegSet );

        /**
         * Check and Connect
         *
         * @since 1.0
         * @return
         */
        void                CheckAndConnect();

        /**
         * Simple update of active edge.
         *
         * @since 1.0
         * @param aY : line y coordinate
         * @return
         */
        void                UpdateActiveEdgeSimple( TInt aY );

    protected:
        TInt32              iMinX, iMinY;
        TInt32              iMaxX, iMaxY;

        TGfxSegEdge*        iEdges;
        TInt                iEdgesCount;

        TUint               iPolygonAreaSize;
        TUint               iLastSegLenSq;

        TPoint16*           iPoints;
        TSegSet*            iSegset;
        TSegSet*            iWaitingSegList;
        TSegSet*            iActiveSegList;
        TGfxSegEdge*        iSortedEdges;

        TInt                iPointsCount;
       TInt                iSegsetCount;
        TInt                iPointsCountMax;
        TInt                iSegsetCountMax;
        TInt                iEdgesCountMax;

        TGfxRendererInfoP*  iRenderInfo;

    private:
        TSegSet*            iSegListForSimple[2];
        TSegSet*            iStartSeg;
        TBool               iIsNewOutlineStarted;
        TInt                iSubPathSegmentCount;

    private:
        friend class        TGfxPolygonRendererP;
        friend class        TGfxStrokeRendererP;
        friend class        CGfx2dGc;
        friend class        TSegSet;
    };

#endif      // GFXEDGELISTP_H

