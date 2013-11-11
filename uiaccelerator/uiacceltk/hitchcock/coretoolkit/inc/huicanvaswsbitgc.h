/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of CHuiCanvasWsBitGc.
*
*/



#ifndef __HUICANVASWSBITGC_H__
#define __HUICANVASWSBITGC_H__

#include <uiacceltk/HuiVisual.h>
#include "huicanvaswsgc.h"
#include "huicanvaswsswgc.h"
#include "huicanvaspainter.h"
#include <bitstd.h>

const TInt KHuiCanvasWsBitGcRegionGranularity = 10;

/**
 * This class is used internally by the toolkit and thus should
 * not be used by the applications directly.
 */
NONSHARABLE_CLASS(CHuiCanvasWsBitGc) : public CHuiCanvasWsSwGc
{
private:
    /**
     * Constructor.
     */
    CHuiCanvasWsBitGc();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

public:
    /**
     * Constructor.
     */
    static CHuiCanvasWsBitGc* NewL();
    
    /**
     * Destructor.
     */
    ~CHuiCanvasWsBitGc();

public:

    // Canvas functionality
	virtual void BeginActionL(TInt aAction, TRect aDisplayRect,const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion);
	virtual void EndActionL(const TRegion& aUpdateRegion, TBool aUpdateDisplay);
    virtual THuiCanvasWsGcType Type() const;
    virtual void ClearCache();
    
    virtual TBool IsRenderBufferEnabled() const;
	virtual void ClearCapturingBufferArea(const TRect& aRect);
	
};    
#endif  // __HUICANVASWSBITGC_H__
