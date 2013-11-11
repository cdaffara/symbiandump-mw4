/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition for CHuiCurvePath. Curve paths are composed of path 
*                segments and can be used as mapping functions, and in line visuals.
*
*/



#ifndef __HUICURVEPATH_H__
#define __HUICURVEPATH_H__


#include <e32base.h>

#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/HuiTimedPoint.h>
#include <uiacceltk/HuiRealPoint.h>
#include <uiacceltk/HuiImage.h>


class CHuiGc;
class CHuiVisual;
struct THuiCurvePathExtension;


/**
 * CHuiCurvePath is a 2D path composed out of line segments and ellipse arcs.
 * Each segment can have a length factor that causes it to be shorter or
 * longer than its real length.
 */
class CHuiCurvePath : public CBase, public MHuiMappingFunction
    {
    
    friend class CHuiLineVisual;
    
public:

    /* Constructors and destructor. */

    /**
     * Constructor.
     */
    IMPORT_C static CHuiCurvePath* NewL();
    
    /**
     * Constructor. The new brush is left on the cleanup stack.
     */
    IMPORT_C static CHuiCurvePath* NewLC();
    
	/**
	 * Constructor. Not exported.
	 */
    IMPORT_C CHuiCurvePath();

    /** 
     * Destructor.
     */
    IMPORT_C ~CHuiCurvePath();
    
    
    /* Methods. */
    
    /** 
     * Resets the path by removing all segments.
     */
    IMPORT_C virtual void Reset();
    
    /** 
     * Enables or disables looping of the path. Looping means that all 
     * positions are wrapped to the path, so that, e.g., positions past
     * the end are wrapped to the beginning of the path. Looping is enabled by
     * default.
     */
    IMPORT_C void EnableLoop(TBool aEnable = ETrue);
    
    /**
     * Determines if the path is wrapped. 
     */
    IMPORT_C TBool Loop() const; 
    
    /**
     * Determines if the path needs updating.
     */
    IMPORT_C TBool NeedUpdate() const;
    
    /**
     * Called when the path needs updating.
     */
    IMPORT_C void SetNeedsUpdate();
    
    /**
     * Sets the position that is at path location zero.
     *
     * @param aPosOrigin  Added to positions when evaluating.
     */ 
    IMPORT_C void SetOrigin(TReal32 aPosOrigin) __SOFTFP;
    
    /**
     * Appends a line segment to the path.
     * @deprecated     
     *
     * @param aLength  Negative length means that the length will be the 
     *                 actual length of the line segment.
     */
    IMPORT_C void AppendLineL(const TPoint& aStart, const TPoint& aEnd, 
                              TReal32 aLength = -1) __SOFTFP;

    /**
     * Appends a line segment to the path. Note that the coordinate values should 
     * match to the metrics unit type of the visual that uses this class.
	 *
     * @param aStart  	Start point of the line segment. 
     * @param aEnd  	End point of the line segment. 
     * @param aLength  	Negative length means that the length will be the 
     *                 	actual length of the line segment.
     */
    IMPORT_C void AppendLineL(const THuiRealPoint& aStart, const THuiRealPoint& aEnd, 
                              TReal32 aLength = -1) __SOFTFP;

    /**
     * Appends a curve segment to the path.
     * @deprecated     
     *
     * @param aOrigin      Origin of the arc ellipse.
     * @param aSize        Horiz and vert radii of the arc ellipse.
     * @param aStartAngle  Start angle of the arc (degrees).
     * @param aEndAngle    End angle of the arc (degrees).
     * @param aLength      Negative length means that the length will be the
     *                     actual length of the arc.
     */                              
    IMPORT_C void AppendArcL(const TPoint& aOrigin, const TSize& aSize,
                             TReal32 aStartAngle, TReal32 aEndAngle,
                             TReal32 aLength = -1) __SOFTFP;               
   
    /**
     * Appends a curve segment to the path. Note that the coordinate values should 
     * match to the metrics unit type of the visual that uses this class.
     *
     * @param aOrigin      Origin of the arc ellipse.
     * @param aSize        Horiz and vert radii of the arc ellipse.
     * @param aStartAngle  Start angle of the arc (degrees).
     * @param aEndAngle    End angle of the arc (degrees).
     * @param aLength      Negative length means that the length will be the
     *                     actual length of the arc.
     */                              
    IMPORT_C void AppendArcL(const THuiRealPoint& aOrigin, const THuiRealSize& aSize,
                             TReal32 aStartAngle, TReal32 aEndAngle,
                             TReal32 aLength = -1) __SOFTFP;               
   
    /**
     * Determines the total length of the path.
     */                
    IMPORT_C TReal32 Length() const __SOFTFP;
   
    /**
     * Evaluates a point along the curve path.
     *
     * @param aPos    Position along the path.
     * @param aPoint  X and Y coordinates of the point on the path.
     */
    IMPORT_C void Evaluate(TReal32 aPos, THuiRealPoint& aPoint) const __SOFTFP;                                     
    
    /**
     * Recalculate the path's vertices and colors. 
     */
    IMPORT_C virtual void Update(TReal32 aStartPos, TReal32 aEndPos, 
                        TReal32 aAlphaFactor,
                        MHuiMappingFunction* aAlphaFunction,
                        MHuiMappingFunction* aWidthFunction) __SOFTFP;
    
    /** 
     * Draws the path's triangles.
     */
    IMPORT_C virtual void Draw(const TPoint& aOrigin, CHuiGc* aGc) const;


    /* Implements the MHuiMappingFunction interface. */
    
    IMPORT_C TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    
    IMPORT_C TBool MappingFunctionChanged() const;
    
    IMPORT_C void MappingFunctionClearChanged();
    IMPORT_C void SetVisual(const CHuiVisual* aVisual) const;
    IMPORT_C const CHuiVisual* GetVisual() const;
    IMPORT_C TBool IsLegacyApiUsed() const;
    
protected:

    /* Methods. */

    /**
     * Determines the total length of the path by summing the lengths of
     * all the segments.
     */                         
    IMPORT_C TReal32 CalculateLength() const;
            
        
protected:

    /* Types. */
    
    /** Segment type. */
    enum TSegmentType
        {
        ESegmentTypeLine,
        ESegmentTypeArc
        };
    
	/**
	 * A single segment of a path, of shape TSegmentType. These are joined
	 * together to make a complex path.
	 * 
	 */
    struct TSegment
        {
        /** Straight or curved. */
        TSegmentType iType;
        /** Length of the path segment. */
        TReal32 iLength;
        /** Point where it starts. */
        THuiRealPoint iOrigin;
        /** Point where it ends. */
        THuiRealPoint iDelta;
        /** Starting angle (if curved). */
        TReal32 iStartAngle;
        /** Arc size (if curved). */
        TReal32 iAngleDelta;
        
        /** Start position of the segment measured from the start. */
        TReal32 iTotalPos;
        };


protected:

    /* Methods. */
  
    /**
     * Returns the number of segments.
     */
    IMPORT_C TInt SegmentCount() const;
                         
    /**
     * Returns a segment.
     *
     * @return  Segment data.
     */                         
    IMPORT_C TSegment Segment(TInt aIndex) const;                         

    /**
     * Evaluates a point on a segment.
     *
     * @param aPos      Position.
     * @param aSegment  Segment (arc or line).
     * @param aPoint    Result point.
     * @param aNormal   If specified, returns the normal of the path point.
     */
    IMPORT_C void EvaluateSegment(TReal32 aPos, const TSegment& aSegment, 
                         THuiRealPoint& aPoint, 
                         THuiRealPoint* aNormal = 0) const;
                             
    
    /**
     * Provides expandability, helps keeping the binary compatibility. Since virtual
     * table is now exported and this class is dll derivable and the implementation
     * is more difficult to change, hence this method, which can provide additional
     * extension APIs.
     * 
     * @param aExtensionUid     UID, which is being used for recognizing the extension
     * @param aExtensionParams  Return pointer to the extension API, once recognized from the extension uid
     */
    IMPORT_C virtual void CurvePathExtension(const TUid& aExtensionUid, TAny** aExtensionParams);


private:

    /** Array of line and arc segments. */
    RArray<TSegment> iSegments;

    /** Path location origin. */
    TReal32 iPosOrigin;

    TReal32 iTotalLength;

    THuiCurvePathExtension* iExtension;
    
    /** Flag to determine if the path has changed. */
    TBool iChanged;

    TBool iNeedUpdate;   

    /** Spare member to help keeping binary compatibility, since HuiCurvePath
        is now dll derivable **/
    TInt iSpare;

public:

    /* Public properties. */
    
    /** Offset to the entire path. */
    THuiTimedPoint iOffset;
    
    };


#endif
