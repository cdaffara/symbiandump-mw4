/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Line visual
*
*/




#ifndef C_ALFLINEVISUAL_H
#define C_ALFLINEVISUAL_H

#include <alf/alfvisual.h>
#include <alf/alfownership.h>

class CAlfControl;
class CAlfLayout;
class CAlfCurvePath;
class MAlfMappingFunction;
class TAlfImage;
class TRgb;


/**
 * Line visual draws a line along a curve path.
 *
 * A line visual will draw a bendy line that follows a specified
 * curve path. Width and alpha functions can be set to change the
 * opacity and thickness of the line along its length.
 *
 * Note that the Bitgdi renderer contains only a minimal implementation
 * of line drawing. There is no aliasing or shading on the line, and
 * thickness and alpha functions have no effect. The entire thickness of a Bitgdi
 * line is set by the initial thickness at point 0.
 * Usage:
 * 
 * @code
 * 
 * //Create curvepath, which can be used as a path for linevisual
 * CAlfCurvePath* path = CAlfCurvePath::NewL( &iEnv );
 * path->AppendArcL( TPoint(120, 120), TSize(60, 60), 180, 180 + 270, 0.5 );
 * path->AppendLineL( TPoint(120, 180), TPoint(20, 180), 0.5 );
 * 
 * //Create linevisual
 * CAlfLineVisual* line = CAlfLineVisual::AddNewL( control );
 * //set curvepath instance, with ownership transfered to visual
 * line->SetPath( path, EAlfHasOwnership );
 * 
 * //Line opacity function. Line visuals can use a mapping function to determine the opacity
 * //of the line along the path. 
 * //member variable : LinearMappingFunction linearFunc;
 * linearFunc.SetFactor( 1 );
 * //setlinevisual alpha function
 * line->SetAlphaFunction( &linearFunc );
 * 
 * //Similerly, line visuals can use a mapping function to determine the width of the
 * //line along the path. 
 * linearFunc.SetFactor( 60 );
 * line->SetWidthFunction( &linearFunc ); * 
 * 
 * @endcode
 * 
 * @lib alfclient.lib
 * @since S60 v3.2
 * @see CAlfCurvePath
 */

class CAlfLineVisual : public CAlfVisual
    {

public:
    
    /**
     * Constructor, which gives ownership to the control.
     *
     * @param aOwnerControl The control
     * @param aParentLayout If given, the parent layout.
     * @return New instance. Ownership NOT transreffed (owned by control)
     */
    IMPORT_C static CAlfLineVisual* AddNewL(CAlfControl& aOwnerControl,
                                            CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfLineVisual();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAlfLineVisual();

    /**
     * Sets the path instance of the visual.
     *
     * @param aPath
     * @param aOwnership
     */
    IMPORT_C void SetPath(CAlfCurvePath* aPath, TAlfOwnership aOwnership);

    /**
     * Returns the path of the visual. The path can be modified via this
     * reference.
     *
     * @return Path. NULL if not set.
     */
    IMPORT_C CAlfCurvePath* Path();

    /**
     * Sets the image used by the line.
     */
    IMPORT_C void SetImage(const TAlfImage& aImage);
    
    /**
     * Returns the image used by the line.
     */
    IMPORT_C const TAlfImage& Image() const;

    /**
     * Sets the alpha mapping function that generates alpha values for the
     * path.
     */
    IMPORT_C void SetAlphaFunction(MAlfMappingFunction* aFunction);

    /**
     * Sets the width mapping function that generates width values for the
     * path.
     */
    IMPORT_C void SetWidthFunction(MAlfMappingFunction* aFunction);

    /** 
     * Thickness of the line. 
     */
    IMPORT_C const TAlfTimedValue& Thickness() const;
    IMPORT_C void SetThickness( const TAlfTimedValue& aThickness );

    /** 
     * Thickness of the shadow around the line. 
     */
    IMPORT_C const TAlfTimedValue& ShadowThickness() const;
    IMPORT_C void SetShadowThickness( const TAlfTimedValue& aShadowThickness );

    /** 
     * Current start position on the path. 
     */
    IMPORT_C const TAlfTimedValue& StartPos() const;
    IMPORT_C void SetStartPos( const TAlfTimedValue& aStartPos );

    /** 
     * Current end position on the path.
     */
    IMPORT_C const TAlfTimedValue& EndPos() const;
    IMPORT_C void SetEndPos( const TAlfTimedValue& aEndPos );
    
    /**
     * Sets the color of the line.
     */
    IMPORT_C void SetColor(const TRgb& aColor);

    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);


protected:
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);


private: // data

    struct TLineVisualPrivateData;
    TLineVisualPrivateData* iLineVisualData;

    };

#endif // C_ALFLINEVISUAL_H
