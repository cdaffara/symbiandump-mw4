/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of THuiLayoutPrivateData.
*
*/

#ifndef HUILAYOUTDATA_H_
#define HUILAYOUTDATA_H_

// Private structure
//
/**
 * TScrollState contains information about the scrolling state of the
 * layout. This is only used when scrolling is enabled. Most layouts
 * don't use this.
 */
class TScrollState
    {
public:
    TScrollState() : iOffset(), iVirtualSize(0, 0) {}
    THuiTimedPoint iOffset;

    /** Size of the layout considering the placement of children and outer
        borders. This may be larger than the size of the layout on the
        screen. Used when scrolling. */
    TSize iVirtualSize;
    };

struct CHuiLayout::THuiLayoutPrivateData
    {
public:
    THuiLayoutPrivateData() : 
        iInnerPadding(0, 0),
            iScroll(NULL),
            iTransitionTime(0),
            iBaseUnit(THuiMetric(), THuiMetric()) // defaults to [1px, 1px]
    {    
        }
    
public:
    /** Visuals that are children of the layout. None of these is owned by
        by the layout. */
    RPointerArray<CHuiVisual> iChildren;

    /** Inner horizontal padding. */
    //TInt16 iXPadding;

    /** Inner vertical padding. */
    //TInt16 iYPadding;
    
    /** Inner paddings (horizontal and vertical). */
    THuiXYMetric iInnerPadding;

    /** State of scrolling. */
    TScrollState* iScroll;

    /** Used to store the local transition time */
    TInt iTransitionTime;

    /** Metric for the layout's base measurement unit. The positions and sizes
        of the children of this layout are interpreted as multiples of this
        base unit. */
    THuiXYMetric iBaseUnit;
    CHuiCanvasGc *iGc;
    };


#endif /* HUILAYOUTDATA_H_ */
