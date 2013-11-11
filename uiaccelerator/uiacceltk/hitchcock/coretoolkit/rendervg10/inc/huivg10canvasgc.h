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
* Description:   Definition of CHuiCanvasGc
*
*/



#ifndef __HUIVG10CANVASGC_H__
#define __HUIVG10CANVASGC_H__


#include <e32base.h>
#include <uiacceltk/HuiVisual.h>
#include "huicanvasgc.h"


/* Forward declarations. */    

class CHuiVg10CanvasGc : public CHuiCanvasGc
    {
public:

    CHuiVg10CanvasGc();
    ~CHuiVg10CanvasGc();
    void DoDrawPolygon(RArray<THuiRealPoint>& aPoints);
    void DoDrawEllipse(const THuiRealRect& aDestinationRect);
    void DoDrawArc(const THuiRealRect& aDestinationRect, const THuiRealPoint& aStart, const THuiRealPoint& aEnd);
    void DoDrawPie(const THuiRealRect& aDestinationRect, const THuiRealPoint& aStart, const THuiRealPoint& aEnd);
    void DoDrawRoundRect(const THuiRealRect& aDestinationRect, const THuiRealSize& aSize);
    void DoDrawRenderBuffer(const CHuiCanvasRenderBuffer& aImage, const THuiRealPoint& aDestinationPoint);
    void DoClearRenderBuffer( CHuiCanvasRenderBuffer& aImage, const TRect & aRect );
    void DoDrawRects(RArray<THuiRealRect>& aRects);

    void DrawEllipse(const THuiRealRect& aDestinationRect);
    
    void ClearWithSkinBackground(const THuiRealRect& aRect);
    void ClearWithBackgroundItems(const THuiRealRect& aRect, const RArray<THuiDisplayBackgroundItem>& aItems);
    TInt MaxNumberOfClipRects() const;
    CHuiCanvasRenderBuffer* CreateRenderBufferL(const TSize& aSize);

    void RestoreFlaggedState() const;
    
private: // New
    
    void DoDrawPieAndArc(const THuiRealRect& aDestinationRect, 
          const THuiRealPoint& aStart, const THuiRealPoint& aEnd, TBool aIsPie);
    };

#endif        