/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#ifndef PlatformScrollbar_h
#define PlatformScrollbar_h

#include "Widget.h"
#include "ScrollBar.h"
#include "IntRect.h"

const int KScrollbarWidth = 4;

namespace WebCore {

class PlatformScrollbar : public Widget, public Scrollbar {
public:
    PlatformScrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize);
    virtual ~PlatformScrollbar();

    virtual bool isWidget() const { return true; }

    virtual int width() const;
    virtual int height() const;
    virtual void setRect(const IntRect&);
    virtual void setEnabled(bool);
    virtual bool isEnabled(void) const {return m_enabled;}
    virtual void paint(GraphicsContext*, const IntRect& damageRect);
    static int horizontalScrollbarHeight();
    static int verticalScrollbarWidth();

    virtual void handleMousePressEvent(const PlatformMouseEvent&, const IntPoint&);
    
protected:
    virtual void updateThumbPosition();
    virtual void updateThumbProportion(); 
    
    void clickAtPoint(const IntPoint& point, bool down);
    
private:
    IntRect m_rect;
    bool m_enabled;
};

}

#endif // PlatformScrollbar_h
