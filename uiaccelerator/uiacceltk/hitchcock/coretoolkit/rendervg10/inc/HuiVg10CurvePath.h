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
* Description:   ?Description
*
*/



#ifndef __HUIVG10CURVEPATH_H__
#define __HUIVG10CURVEPATH_H__


#include "uiacceltk/HuiCurvePath.h"
#include "HuiVg10RenderPlugin.h"
#include <VG/openvg.h>

/**
 * Curve path that can be drawn using OpenVG 1.0.
 */
NONSHARABLE_CLASS(CHuiVg10CurvePath) : public CHuiCurvePath
    {
public:

    /* Constructors and destructor. */

	/**
	 * Constructor.
	 */
	CHuiVg10CurvePath();

	/**
	 * Second-phase constructor.
	 */
	void ConstructL();

    /** 
     * Destructor.
     */
    virtual ~CHuiVg10CurvePath();
    
    
    /* Methods. */
    
    /** 
     * Resets the path by removing all segments.
     */
    virtual void Reset();
    
    /**
     * Recalculate the path's vertices and colors. 
     */
    void Update(TReal32 aStartPos, TReal32 aEndPos, 
                TReal32 aAlphaFactor,
                MHuiMappingFunction* aAlphaFunction,
                MHuiMappingFunction* aWidthFunction) __SOFTFP;
    
    /** 
     * Draws the path.
     *
     * @param aOrigin Offset of drawing in pixels
     * @param aGc Graphics context to use.
     */
    void Draw(const TPoint& aOrigin, CHuiGc* aGc) const;

protected:

    /**
     *  @returns the number of tuples required for 
     *           a given segment.
     */
    TInt SegmentTuples(const TSegment& aSegment) const;
    
    /**
     *  Reserve space for tuples.
     *
     *  @param aTuplecount  Number of tuples required.
     */
    void SetTupleCountL(TInt aTupleCount);
    
    /**
     *  Prepare the tuples for a given segment.
     *
     *  @param aSegment         Segment to process.
     *  @param aStart           Start position.
     *  @param aEnd             End position.
     *  @param aIsFinal         ETrue if this is the final 
     *                          segment of the path.
     *  @param aAlphaFactor     Transparency factor.
     *  @param aAlphaFunction   Function of path alpha.
     *  @param aWidthFunction   Function of path width.
     */
    void MakeSegmentTuples(const TSegment& aSegment,
                           TReal32 aStart, TReal32 aEnd,
                           TBool aIsFinal,
                           TReal32 aAlphaFactor,
                           MHuiMappingFunction* aAlphaFunction,
                           MHuiMappingFunction* aWidthFunction);

    /**
     *  Add a single tuple to the path.
     *
     *  @param aPoint       Tuple coordinates.
     *  @param aNormal      Tuple normal (left side).
     *  @param aWidth       Path width.
     *  @param aAlpha       Path transparency.
     */
    void AddTuple(const THuiRealPoint& aPoint,
                  const THuiRealPoint& aNormal,
                  TReal32 aWidth, TReal32 aAlpha);

    /**
     *  Reset the current path.
     */
    void ResetPath();

    /**
     *  Reset the accumulated tuples.
     */
    void ResetTuples();

private:
    /** Path that encloses the whole curve */
    VGPath   iPath;

    /** Coordinates for the path */
    VGfloat* iPathCoords;

    /** Commands for the path */
    VGubyte* iPathCommands;

    /** Last opacity factor used to render the path */
    TReal32 iLastOpacityFactor;

    /** Number of vertex tuples along the path (at least 2). */
    TUint iMaxTupleCount;
    
    /** Number of vertex tuples used. */
    TUint iUsedTupleCount;
    };


#endif // __HUIVG10CURVEPATH_H__
