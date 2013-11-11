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
* Description:   Viewport visual.
*
*/



#ifndef C_HUIVIEWPORTLAYOUT_H
#define C_HUIVIEWPORTLAYOUT_H

#include <e32base.h>
#include <uiacceltk/HuiLayout.h>

/* Forward declarations */
class CHuiEnv;


/**
 * A CHuiViewportLayout is similar to a CHuiDeckLayout, but it can contain
 * visuals larger than itself. The larger visuals can be scrolled around.
 *
 * The viewport visual can be thought of as providing a "window" onto a larger visual.
 * Different parts of the visual can be viewed by moving the viewport. By default, the
 * clipping region is enabled.
 *
 * To use the viewport visual, you first need to call SetVirtualSize() to define the
 * coordinate system for the visual. You pass in the work area of the visual, in whatever
 * units you choose. For example, if you set the virtual work area size to (1.0, 1.0), it
 * will use a normalised (u, v) coordinate mapping system.
 *
 * Next, you can set the dimensions and position of the viewport (or "window") using SetViewportSize()
 * and SetViewportPos(). This viewport can be moved around the virtual work area. The contents of the
 * viewport will be fitted to the ViewportVisual.
 *
 * Possible uses for CHuiViewportVisual are for text marquee scrolling effects and photo zooming.
 *
 * @todo - insert diagram here.
 * @see CHuiDeckLayout
 *
 */

NONSHARABLE_CLASS(CHuiViewportLayout) : public CHuiLayout
	{
public:	

    /** @beginAPI */
    
    // Bitfield of binary flags.
	enum TFlags
		{
		/** Wrap contents of visual horizontally. */
		EWrapHorizontal = 0x0001,
		
		/** Wrap contents of visual vertically. */
		EWrapVertical = 0x0002
		};
	
    /* Constructors and destructor. */

    /**
     * Construct a new viewport layout and give its ownership to a control.
     *
     * @param aOwnerControl  Owner control.
     * @param aParentLayout  Parent layout for the new deck layout.
     */
    IMPORT_C static CHuiViewportLayout* AddNewL(CHuiControl& aOwnerControl, 
                                                CHuiLayout* aParentLayout = 0);

    /** @endAPI */

public:

    /**
     * Constructor.
     *
     * @param aOwner  Owner control.
     */
    CHuiViewportLayout(MHuiVisualOwner& aOwner);    // Made public for unit tests.

    void ConstructL();  // Made public for unit tests.

    ~CHuiViewportLayout();  // Made public for unit tests.

public:

    /** @beginAPI */

    /* Methods. */

	/**
	 * Set virtual size of the viewable area.
	 *
	 * This is different from the physical size of the viewport -
	 * it defines the coordinate space that is used for scrolling.
	 * To use a normalised (u, v) coordinate mapping system, set aSize to (,1.0, 1.0).
	 *
	 * @param aSize The bottom-right corner of the child visual, in the units you wish to use for positioning the viewport.
	 * @param aTransitionTime The time it will take to complete the resizing of the virtual size.
	 */
	IMPORT_C void SetVirtualSize(const THuiRealSize& aSize, TInt aTransitionTime=0);

	/**
	 * Set size of the viewport area.
	 *
	 * This is the size of the "window" we are using to look at the visual.
	 *
	 * @param aSize The size of the viewport in the units defined by SetVirtualSize().
	 * @param aTransitionTime The time it will take to complete the resizing of the viewport area.
	 * @see SetVirtualSize, SetViewportPos
	 */
	IMPORT_C void SetViewportSize(const THuiRealSize& aSize, TInt aTransitionTime=0);

	/**
	 * Set position of the viewport area.
	 *
	 * This is the offset of the "window" we are using to look at the visual.
	 *
	 * @param aSize The size of the viewport in the units defined by SetVirtualSize().
	 * @param aTransitionTime The time it will take to complete moving the viewport area.
	 * @see SetVirtualSize, SetViewportSize
	 */
	IMPORT_C void SetViewportPos(const THuiRealPoint& aSize, TInt aTransitionTime=0);

    /** @endAPI */

    /**
     * Overridden version of size changing updates the layout of child visuals.
     *
     * @param aSize  New size for the layout.
     * @param aTransitionTime  Time for the transition to new size to take 
     *                          place.
     */
    void SetSize(const THuiRealSize& aSize, TInt aTransitionTime=0);

    TBool ChildSize(TInt aOrdinal, TSize& aSize);
    
    TBool ChildPos(TInt aOrdinal, TPoint& aPos);

    TInt ChildRect(TInt aOrdinal, THuiRealRect& aRect);
    	
    void GetClassName(TDes& aName) const
        {
        aName = _L("CViewportLayout");
        }
        
private: 

    /** The bottom-right corner of the child visual, in the units defined by SetVirtualSize. */
    THuiRealSize iVirtualSize;
    
    /** The top-left corner of the viewport window, relative to te position defined by iVirtualSize. */
    THuiRealPoint iViewportPos;
    
    /** The size of the viewport window, relative to te position defined by iVirtualSize. */
    THuiRealSize iViewportSize;
    
    /** Bitfield of flags to change the way the vieport is rendered. */
    TInt iWrappingFlags;

	};

#endif // C_HUIVIEWPORTLAYOUT_H
