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


#include "config.h"

#include "PlatformScrollbar.h"
#include "GraphicsContext.h"
#include "WebCoreGraphicsContext.h"

#include <e32base.h>
#include <bitstd.h>

using namespace WebCore;

int PlatformScrollbar::horizontalScrollbarHeight() { return KScrollbarWidth; }
int PlatformScrollbar::verticalScrollbarWidth() { return KScrollbarWidth; }

PlatformScrollbar::PlatformScrollbar(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize controlSize) 
: Scrollbar(client, orientation, controlSize), m_rect(0,0,KScrollbarWidth,KScrollbarWidth)
{ 
}

PlatformScrollbar::~PlatformScrollbar() { }

int PlatformScrollbar::width() const 
{ 
    return m_rect.width(); 
}

int PlatformScrollbar::height() const 
{ 
    return m_rect.height(); 
}

void PlatformScrollbar::setEnabled(bool b) 
{
    m_enabled = b;
}

void PlatformScrollbar::paint(GraphicsContext* p, const IntRect& damageRect) 
{    
    WebCoreGraphicsContext* gc = p->platformContext();
    CFbsBitGc* gcContext( &gc->gc() );

    int zoomFactor = gc->view().scalingFactor();
    TRect rect = TRect(m_rect.x() * zoomFactor / 100, m_rect.y() * zoomFactor / 100, (m_rect.x() + m_rect.width()) * zoomFactor / 100, (m_rect.y() + m_rect.height()) * zoomFactor / 100);
    
    gcContext->SetPenStyle( CGraphicsContext::ENullPen );
    gcContext->SetBrushStyle( CGraphicsContext::ESolidBrush );

    TRgb bck(150,150,180);
    TRgb frg(90,90,150);
    // draw background
    gcContext->SetBrushColor( bck );
    gcContext->DrawRect( rect );

    TInt h = rect.Height();
    TInt w = rect.Width();

    TInt kx, ky, kw, kh;
    TInt scrollbarWidth = zoomFactor*KScrollbarWidth/100;
    
    float thumb = (m_visibleSize > m_totalSize) ? 1 : (m_totalSize?(float)m_visibleSize/m_totalSize:1);
    float value = (m_totalSize-m_visibleSize)?(float)m_currentPos/(m_totalSize-m_visibleSize):1;
    if (m_orientation==HorizontalScrollbar)
        {
        // compute thumb position and size
        kw = w * thumb;
        kh = h;
        kx = rect.iTl.iX + value*(w-kw);
        ky = rect.iTl.iY;
        }
    else
        {
        // compute thumb position and size
        kw = w;
        kh = h * thumb;
        kx = rect.iTl.iX;
        ky = rect.iTl.iY+ value*(h-kh);
        }

    gcContext->SetBrushColor(frg);
    TRect r(TPoint(kx,ky),TSize(kw,kh));
    gcContext->DrawRect(r);
}

void PlatformScrollbar::updateThumbPosition() 
{
}

void PlatformScrollbar::updateThumbProportion() 
{
}

void PlatformScrollbar::setRect(const IntRect& rect) 
{ 
    m_rect = rect;
}


void PlatformScrollbar::handleMousePressEvent(const PlatformMouseEvent&, const IntPoint& p)
{
    clickAtPoint(p, true);
}

void PlatformScrollbar::clickAtPoint(const IntPoint& point, bool /*down*/)
{
}
