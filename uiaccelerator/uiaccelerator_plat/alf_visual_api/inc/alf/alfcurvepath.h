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
* Description:   Curve paths are composed of path segments
*                and can be used as mapping functions, and in line visuals.
*
*/



#ifndef ALFCURVEPATH_H
#define ALFCURVEPATH_H

#include <e32base.h>
#include <alf/alfmappingfunctions.h>

class CAlfEnv;
class CAlfGenComponent;

/**
 *  Curve paths are composed of path segments
 *  and can be used as mapping functions, and to define the shape of line visuals.
 *
 *  The curve path can be constructed from two primitives: arcs and lines. The arcs
 *  and lines can be defined through methods AppendArcL() and AppendLineL() respectively.
 *
 *  Coordinate values used when defining arcs and lines have to match the base unit type
 *  of the visual associated with this curve path. So for instance, if this curve path
 *  is used to define a layout using CAlfCurvePathLayout the definition of arcs and
 *  lines have to match the base unit of the layout. If this path is used to define a
 *  CAlfLineVisual, the coordinate values has to be defined in terms of base units affecting
 *  the CAlfLineVisual.
 *
 *  In the code example below a curve path is defined to a curve path layout. The curve
 *  path is constructed from two line segments and one arc segment and defines a path
 *  that starts from the bottom left corner of the layout and ends in the bottom right corner
 *  of the layout. The total length of the curve path is 4.0 units. Notice that the coordinates
 *  of the curve path are defined in base units of the layout.
 *
 *  @code
 * 	// Create a new curve path layout
 * 	CAlfCurvePathLayout* layout = CAlfCurvePathLayout::AddNewL(*this);
 * 	layout->SetBaseUnit(TAlfMetric(1.0f, EAlfUnitNormalized));
	
 *	// Construct the layout path
 *	CAlfCurvePath& path = layout->CurvePath();
 *	path.AppendLineL(TAlfRealPoint(0.0f, 1.0f), TAlfRealPoint(0.0f, 0.5f), 1);
 *	path.AppendArcL(TAlfRealPoint(0.5f, 0.5f), TAlfRealSize(0.5f, 0.5f), 180.0f, 360.0f, 2);
 *	path.AppendLineL(TAlfRealPoint(1.0f, 0.5f), TAlfRealPoint(1.0f, 1.0f), 1);
 *  @endcode
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfCurvePath ): public CBase, public MAlfMappingFunction
    {

public:

    /** 
     * Construct a new curve path.
     *
     * @param aEnv The current environment.
     * @param aCurvePathProviderHandle Handle to the server side curve path provider.
     *        If 0, the server side will create a new instance of CHuiCurvePath.
     * @return The new curve path.
     * @see CAlfGenComponent
     */
    IMPORT_C static CAlfCurvePath* NewL( 
        CAlfEnv& aEnv,
        TInt aCurvePathProviderHandle = 0 );

    /** 
     * Construct a new curve path, leave on cleanup stack.
     *
     * @param aEnv The current environment.
     * @param aCurvePathProviderHandle Handle to the server side curve path provider.
     *        If 0, the server side will create a new instance of CHuiCurvePath.
     * @return The new curve path.
     * @see CAlfGenComponent
     */
    IMPORT_C static CAlfCurvePath* NewLC( 
        CAlfEnv& aEnv,
        TInt aCurvePathProviderHandle = 0  );

    /** 
     * Destructor.
     */
    IMPORT_C virtual ~CAlfCurvePath();

    /** 
     * Resets the path by removing all segments.
     */
    IMPORT_C void Reset();
    
    /** 
     * Enables or disables looping of the path. Looping means that all 
     * positions are wrapped to the path, so that, e.g., positions past
     * the end are wrapped to the beginning of the path. Looping is enabled by
     * default.
     */
    IMPORT_C void EnableLoop(TBool aEnable = ETrue);
    
    /**
     * Determines if the path is looping. 
     */
    IMPORT_C TBool Loop() const; 
    
    /**
     * Sets the position that is at path location zero.
     *
     * @param aPosOrigin  Added to positions when evaluating.
     */ 
    IMPORT_C void SetOrigin(TReal32 aPosOrigin) __SOFTFP;
    
    /**
     * Appends a line segment to the path. Note that the coordinate values should 
     * match to the metrics unit type of the visual that uses this class. 
     * @deprecated Use the AppendLineL() - method with real values instead.
     *
     * @param aStart Start point
     * @param aEnd End point
     * @param aLength  Negative length means that the length will be the 
     *                 actual length of the line segment.
     */
    IMPORT_C void AppendLineL(
        const TPoint& aStart, 
        const TPoint& aEnd, 
        TReal32 aLength = -1) __SOFTFP;
        
    /**
     * Appends a line segment to the path. Note that the coordinate values should 
     * match to the metrics unit type of the visual that uses this class.
     *
     * @param aStart Start point
     * @param aEnd End point
     * @param aLength  Negative length means that the length will be the 
     *                 actual length of the line segment.
     */
    IMPORT_C void AppendLineL(
        const TAlfRealPoint& aStart, 
        const TAlfRealPoint& aEnd, 
        TReal32 aLength = -1) __SOFTFP;

    /**
     * Appends a curve segment to the path. Note that the coordinate values should 
     * match to the metrics unit type of the visual that uses this class. The curve segment is defined
     * as a segment of an ellipse.
     *
     * The span of the arc is defined in a range of degrees traversed on the
     * ellipse. The direction of zero degrees points towards the positive x - axis and 
     * 180 degrees points towards negative x - axis. The degrees are specified in
     * clockwise manner so that 90 degrees points towards the positive y - axis (down).
     * 
     * @deprecated Use the AppendArcL(const TAlfRealPoint&, const TAlfRealSize&, TReal32, TReal32, TReal32) - method instead.    
     *
     * @param aOrigin      Origin of the arc ellipse.
     * @param aSize        Horiz and vert radii of the arc ellipse.
     * @param aStartAngle  Start angle of the arc in degrees.
     * @param aEndAngle    End angle of the arc in degrees.
     * @param aLength      Negative length means that the length will be the
     *                     actual length of the arc.
     */                              
    IMPORT_C void AppendArcL(
        const TPoint& aOrigin, 
        const TSize& aSize,
        TReal32 aStartAngle, 
        TReal32 aEndAngle,
        TReal32 aLength = -1) __SOFTFP;
        
    /**
     * Appends a curve segment to the path. Note that the coordinate values should 
     * match to the metrics unit type of the visual that uses this class. The curve segment is defined
     * as a segment of an ellipse.
     *
     * The span of the arc is defined in a range of degrees traversed on the circle of an
     * ellipse. The direction of zero degrees points towards the positive x - axis and 
     * 180 degrees points towards negative x - axis. The degrees are specified in
     * clockwise manner so that 90 degrees points towards the positive y - axis (down).      
     *
     * Image below depicts how a curve segment can be defined in the path.
     *
     * @image html CAlfCurvePath_ArcDefinition.png "Curve segment defined using AppendArcL()."
     *
     * @param aOrigin      Origin of the arc ellipse.
     * @param aSize        Horiz and vert radii of the arc ellipse.
     * @param aStartAngle  Start angle of the arc in degrees.
     * @param aEndAngle    End angle of the arc in degrees.
     * @param aLength      Negative length means that the length will be the
     *                     actual length of the arc.
     */                              
    IMPORT_C void AppendArcL(
        const TAlfRealPoint& aOrigin, 
        const TAlfRealSize& aSize,
        TReal32 aStartAngle, 
        TReal32 aEndAngle,
        TReal32 aLength = -1) __SOFTFP;
                             
    /**
     * Determines the total length of the path.
     *
     * @return Length of the curve.
     */                
    IMPORT_C TReal32 Length() const __SOFTFP;
   
    /**
     * Evaluates a point along the curve path.
     *
     * @param aPos    Position along the path.
     * @param aPoint  Returns the X and Y coordinates of the point on the path.
     */
    IMPORT_C void Evaluate(TReal32 aPos, TAlfRealPoint& aPoint) const __SOFTFP;  
        
    /**
     * Get offset of the entire path.
     *
     * @return Offset
     * @see SetOffset()
     */                
    IMPORT_C const TAlfTimedPoint& Offset() const;
                         
    /**
     * Set offset of the entire path.
     * Sets the translation (ie. origin) of the path. Default is (0,0).
     *
     * @param aOffset Offset of the path.
     */                
    IMPORT_C void SetOffset( const TAlfTimedPoint& aOffset );

    // From MAlfMappingFunction
    
    /**
     * From MAlfMappingFunction
     * Return mapping function ID.
     * @return The mapping function identifier of this curve path. NULL if this class
     *         was constructed with an existing CAlfGenComponent.
     * @see CAlfGenComponent::Identifier()
     * @see NewL()
     */
    IMPORT_C TInt MappingFunctionIdentifier() const;
    
    /**
     * From MAlfMappingFunction
     * Evaluate x/y component on a position on the path.
     * @param aValue Position along the path to get the coordiantes of.
     * @param aMode If equal to 0, evaluates X component, otherwise evaluates Y component.
     * @return The x or y position evaluated for the given position on the curve.
     */
    IMPORT_C TReal32 MapValue(TReal32 aValue, TInt aMode) const __SOFTFP;

private:

    /**
     * Constructor.
     */
    CAlfCurvePath();

    /**
     * Second phase constructor.
     */
    void ConstructL( 
        CAlfEnv& aEnv,
        TInt aCurvePathProviderHandle );

private: // data

    /** Private data.*/
    struct TPrivateData;
    
    /** Private data.*/
    TPrivateData* iData;
    };

#endif // ALFCURVEPATH_H
