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
* Description:   Defines CHuiLineVisual, Visual that is able to draw lines 
*                onto a curve path. 
*
*/



#ifndef __HUILINEVISUAL_H__
#define __HUILINEVISUAL_H__


#include <e32base.h>
#include <uiacceltk/HuiVisual.h>
#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiOwnedPointer.h>
#include <uiacceltk/HuiCurvePath.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiMappingFunctions.h>


/* Forward declarations */
class CHuiEnv;
class CHuiControl;


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
 *
 * @see CHuiCurvePath
 */
NONSHARABLE_CLASS(CHuiLineVisual) : public CHuiVisual
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructs and appends a new text visual to the owner control.
     *
     * @param aOwnerControl  Control that will own the new visual.
     *
     * @return  The new visual.
     */
    IMPORT_C static CHuiLineVisual* AddNewL(CHuiControl& aOwnerControl,
                                            CHuiLayout* aParentLayout);

    /**
     * Constructor.
     */
    CHuiLineVisual(MHuiVisualOwner& aOwner);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    virtual ~CHuiLineVisual();


    /* Methods. */

    /**
     * Sets the path instance of the visual.
     *
     * @param aPath
     * @param aOwnership
     */
    IMPORT_C void SetPath(CHuiCurvePath* aPath, THuiOwnership aOwnership);

    /**
     * Returns the path of the visual. The path can be modified via this
     * reference.
     */
    IMPORT_C CHuiCurvePath& Path();

    virtual TBool Changed() const;

    virtual void ClearChanged();

    /**
     * Sets the image used by the line.
     */
    IMPORT_C void SetImage(const THuiImage& aImage);

    /**
     * Sets the alpha mapping function that generates alpha values for the
     * path.
     */
    IMPORT_C void SetAlphaFunction(MHuiMappingFunction* aFunction);

    /**
     * Sets the width mapping function that generates width values for the
     * path.
     */
    IMPORT_C void SetWidthFunction(MHuiMappingFunction* aFunction);

     /**
     * Sets the color of the line.
     *
     * @param aColor           Line color.
     */
    IMPORT_C void SetColor(const TRgb& aColor);

    /**
     * Draw the curve path.
     */
    virtual void DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const;
    
    /**
     * Expand dirty region
     */
    void ExpandRectWithContent(TRect& aRect) const;

private:

    /* Private methods */


private:

    /** The path of the visual. */
    RHuiOwnedPointer<CHuiCurvePath> iPath;

    /** Image used by the line. */
    THuiImage iImage;

    /** Value mapping function that generates alpha values for path
        positions. */
    MHuiMappingFunction* iAlphaFunction;

    /** Custom width mapping function. */
    MHuiMappingFunction* iWidthFunction;

    /** Determines the width of the line. Depends on the current value
        of iThickness. */
    mutable THuiConstantMappingFunction iDefaultWidthFunction;

public:

    /* Public properties. */

    /** Thickness of the line. */
    THuiTimedValue iThickness;

    /** Thickness of the shadow around the line. */
    THuiTimedValue iShadowThickness;

    /** Current start position on the path. */
    THuiTimedValue iStartPos;

    /** Current end position on the path. */
    THuiTimedValue iEndPos;

private:

    /** Line color */
    TRgb iColor;
    };

#endif  // __HUILINEVISUAL_H__

