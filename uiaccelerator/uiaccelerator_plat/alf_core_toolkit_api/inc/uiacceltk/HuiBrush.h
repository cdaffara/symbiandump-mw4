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
* Description:   Definition of CHuiBrush. Brushes augment visuals with graphical effects.
*
*/



#ifndef __HUIBRUSH_H__
#define __HUIBRUSH_H__


#include <e32base.h>
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/HuiRealRect.h>
#include <uiacceltk/HuiSessionObject.h>


/* Forward declarations. */
class CHuiGc;
class CHuiSkin;
class CHuiVisual;


/** Brush layers. */
enum THuiBrushLayer
    {
    EHuiBrushLayerBackground,
    EHuiBrushLayerForeground
    };

/**
 * MHuiBrushGuide is an interface that brushes use to get information about
 * the shape on which they are being applied.
 */
class MHuiBrushGuide 
    {
public:

    /**
     * Determines the general opacity of the brush.
     *
     * @return  Opacity (0..1).
     */ 
    virtual TReal32 BrushOpacity() const __SOFTFP = 0;

    /**
     * Determines the rectangle into which the brush is being drawn.
     * Uses display coordinates, with (0, 0) being in the upper left
     * corner of the display.
     *
     * @return  Rectangle for the brush in display coordinates.
     */
    virtual THuiRealRect BrushRect() const  __SOFTFP = 0;
    
    /**
     * Returns the skin that the brush should use for getting resources.
     *
     * @return  Skin instance.
     */    
    virtual CHuiSkin& BrushSkin() const = 0;
    
    };

/**
 * A brush implements a series of graphics drawing operations. Brushes are 
 * attached to visuals and drawn whenever the visual is drawn. Brushes can 
 * be used for augmenting the normal appearance of visuals with graphical 
 * features such as background pictures, gradients, overlay icons, and 
 * selection highlights. Technically, a brush is a simple class whose most 
 * important method is Draw() that draws the brush into the specified area 
 * of the display.
 *
 * @see CHuiVisual::EnableBrushesL()
 * @see CHuiBrushArray
 */
class CHuiBrush : public CBase, public MHuiSessionObject
	{
public:

	/**
	 * Destructor.
	 */
	IMPORT_C ~CHuiBrush();


public: // new methods

    /**
     * Expands the dirty region of a visual. Called when a visual's dirty 
     * region is being determined. The brush is allowed to expand the 
     * rectangle to cover any additional drawing the brush is doing.
     * This is not called when the brush is clipped to the visual
     * rectangle, because in that case no drawing can occur outside the 
     * visual's area.
     *
     * @param aRect  Rectangle to expand. Visual's dirty region.
     */
    IMPORT_C virtual void ExpandVisualRect(TRect& aRect) const;

	/**
	 * Mark the brush as changed or unchanged.
	 */
	IMPORT_C void SetChanged();

    /**
     * Determines whether the brush has been changed and needs to be 
     * redrawn.
     */    
    IMPORT_C virtual TBool Changed() const;
    
    /**
     * Clears the changed status of the brush. This is called automatically
     * by the refresh loop after a frame has been drawn.
     */
    IMPORT_C virtual void ClearChanged();
    
    /**
     * Defines whether the brush can draw outside brush guide (i.e. visual's) rect 
     */
    IMPORT_C void SetClipToVisual(TBool aClipToVisual);
    
    IMPORT_C TBool ClipToVisual() const;
    
    /**
     * Defines the layer where brush is drawn (on top of / below visual)
     * @see THuiBrushLayer
     */
    IMPORT_C void SetLayer(THuiBrushLayer aLayer);
    
    IMPORT_C THuiBrushLayer Layer() const;
       
    /**
     * The brush is about to get drawn, usually gc parameters are set here
     */
    IMPORT_C virtual void BeginDraw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;

    /**
     * Draw the brush using the current drawing parameters.
     */
    IMPORT_C virtual void Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;
                               
    /**
     * The brush has been drawn, usually parameters changed in BeginDraw are reset here
     */
    IMPORT_C virtual void EndDraw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;
              
    /**
     * Notifies change in the brush guide
     */
    void ActivateBrushGuide(const CHuiVisual* aNewGuide) const;

    /**
     * Returns active brush guide as a visual if it exist.
     * @return Visual acting as a brush guide.
     */
    CHuiVisual* BrushGuide() const;
                                                         
public: // From MHuiSessionObject
    
    /**
     * Gets the object type.
     */
    IMPORT_C TType Type() const;

    /**
     * Gets the session id for object.
     */
    IMPORT_C TInt SessionId() const;

    /**
     * Sets the session id for object.
     */
    IMPORT_C void SetSessionId(TInt aSessionId);
public:
    enum TType2
        { // this type is used for browsing BrushArray -- to detect who should control the brush in question
        ENotKnown,
        EImageBrush,
        ECmdBufferBrush
        };
    // returns TType2
    virtual TInt Type2() const { return ENotKnown; }
    
protected:

	/**
	 * Constructor.
	 */
	IMPORT_C CHuiBrush();
    
    IMPORT_C virtual void BrushExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

public:

    /* Public properties. */

    /** Opacity of the shadow. */    
    THuiTimedValue iOpacity;
    
private:

    /** <code>ETrue</code> if the brush will be drawn. */
    TBool iEnabled;

    /** <code>ETrue</code> if the brush needs to be redrawn. */
    mutable TBool iChanged;    

    /** Layer determines drawing order. */
    THuiBrushLayer iLayer;

    /** Clip brush to the visual. */
    TBool iClipToVisual;

    /** Session id */
    TInt iSessionId;

    mutable CHuiVisual* iGuideVisual;//TAny* iSpare;
	};

#endif  // __HUIBRUSH_H__
