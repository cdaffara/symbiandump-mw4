/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "GraphicsContext.h"

#include "FloatRect.h"
#include "Font.h"
#include "IntRect.h"
#include "DeprecatedValueList.h"
#include "WebCoreGraphicsContext.h"
#include "DebugStream.h"
#include "PlatformFontCache.h"
#include "StaticObjectsContainer.h"
#include "ZoomFactor.h"

#include <e32std.h>
#include <fbs.h>

#include <math.h>
#include <wtf/MathExtras.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace WebCore {

template<typename T>
void swap( T& a, T& b)
{
    T tmp = a;
    a = b;
    b = tmp;
}
    
class RectEdge
{
public:
    RectEdge() : direction(0) {}
    RectEdge(const IntRect& r, int n) : rect(r), direction(n) {}
    RectEdge(const RectEdge& o) : rect(o.rect), direction(o.direction) { }
    bool operator==(const RectEdge& o) const { return rect==o.rect && direction==o.direction; }
    IntPoint begin() const { return begin(direction); }
    IntPoint end() const { return begin((direction+1)%4); }
    bool contains(const IntPoint& p) const {
        IntPoint s = begin();
        IntPoint e = end();
        if (e.x()<s.x())
            swap(s,e);
        if (e.y()<s.y())
            swap(s,e);
        if (s.x()==e.x())
            return p.x()==s.x() && s.y()<=p.y() && p.y()<=e.y();
        else if (s.y()==e.y())
            return p.y()==s.y() && s.x()<=p.x() && p.x()<=e.x();
        else
            return false;
    }

    bool extends(const RectEdge& edge) const {
        if (&edge==this || edge.direction != direction)
            return false;
        IntPoint p = edge.end();
        return contains(p) && p!=end();
    }
    
    IntPoint begin(int edgeDirection) const;

    IntRect rect;
    int direction;

};

struct PolyRect {
    IntRect rect;
    bool used;
};

bool lineCrosses(IntPoint h0, IntPoint h1, IntPoint v0, IntPoint v1, IntPoint& crossing)
{
    if (h0.y()!=h1.y()) {
        swap(h0,v0);
        swap(h1,v1);
    }
    if (h1.x()<h0.x())
        swap(h0,h1);
    if (v1.y()<v0.y())
        swap(v0,v1);
    if ( (h0.x()<=v0.x() && v0.x()<=h1.x()) &&
        v0.y()<=h0.y() && h0.y()<=v1.y() ) {
        crossing = IntPoint(v0.x(),h0.y());
        return true;
    }
    return false;
}

class RectPolygon
{
public:
    void append(const IntRect& r) {
        if (r.width()<=0 || r.height()<=0)
            return;
        PolyRect pr;
        pr.rect = r;
        pr.used = false;
        rects.append(pr);
    }
    bool isEmpty() const {
        return rects.isEmpty();
    }
    DeprecatedValueList<IntPoint>* nextContour();

private:
    bool startingEdge(RectEdge& edge);
    bool findCrossingEdge(const RectEdge& current, RectEdge& edge, IntPoint& crossing);
    bool findExtendEdge(const RectEdge& current, RectEdge& edge);
    RectEdge nextEdge(const RectEdge& current, IntPoint& crossing);
    void removeUsed();

private:
    DeprecatedValueList<PolyRect> rects;
};

// zoom utility functions
int fontZoomFactor()
{
    return StaticObjectsContainer::instance()->fontCache()->fontZoomFactor();
}

TInt xForm(TInt x)
{
    return x*fontZoomFactor()/100;
}

TPoint xForm(const TPoint& pt)
{
    return TPoint(pt.iX*fontZoomFactor()/100, pt.iY*fontZoomFactor()/100);
}

TSize xForm(const TSize& sz)
{
    return TSize(sz.iWidth*fontZoomFactor()/100, sz.iHeight*fontZoomFactor()/100);
}

TRect xForm(const TRect& rect)
{
    return TRect(xForm(rect.iTl), xForm(rect.Size()));
}

class GraphicsContextPlatformPrivate {
public:
    GraphicsContextPlatformPrivate();
    ~GraphicsContextPlatformPrivate();

    void save();
    void restore();

    WebCoreGraphicsContext* iContext;
    Vector<TWebCoreSavedContext> iContextStack;
};

void GraphicsContextPlatformPrivate::save()
{
    iContextStack.append(iContext->save());
}

void GraphicsContextPlatformPrivate::restore()
{
    iContext->restore(iContextStack.last());
    iContextStack.removeLast();
}

static inline void setPenColor(WebCoreGraphicsContext* aContext, const Color& col)
{
    aContext->gc().SetPenColor(TRgb(col.red(),col.green(),col.blue()));
    aContext->gc().SetPenStyle(CGraphicsContext::ESolidPen);
    aContext->gc().SetBrushStyle(CGraphicsContext::ENullBrush);
}


static inline void setBrushColor(WebCoreGraphicsContext* aContext, const Color& col)
{
    aContext->gc().SetBrushColor(TRgb(col.red(),col.green(),col.blue()));
    aContext->gc().SetPenStyle(CGraphicsContext::ENullPen);
    aContext->gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
}


// A fillRect helper
static inline void fillRectSourceOver(WebCoreGraphicsContext* aContext, const IntRect& rect, const Color& col)
{
      setBrushColor(aContext, col);
      TRect r(rect);
      aContext->gc().DrawRect(xForm(r));
}

GraphicsContextPlatformPrivate::GraphicsContextPlatformPrivate()
    :  iContext(0)
{
}

GraphicsContextPlatformPrivate::~GraphicsContextPlatformPrivate()
{
}


GraphicsContext::GraphicsContext(PlatformGraphicsContext* context)
    : m_common(createGraphicsContextPrivate())
    , m_data(new GraphicsContextPlatformPrivate)
{
    m_data->iContext = context;
}


GraphicsContext::~GraphicsContext()
{
    destroyGraphicsContextPrivate(m_common);
    delete m_data;
}

WebCoreGraphicsContext* GraphicsContext::platformContext() const
{
    return m_data->iContext;
}

void GraphicsContext::savePlatformState()
{
    m_data->save();
}

void GraphicsContext::restorePlatformState()
{
    m_data->restore();
}

// Draws a filled rectangle with a stroked border.
void GraphicsContext::drawRect(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    WebCoreGraphicsContext* context = m_data->iContext;
    if (!context) 
        return;

    if (fillColor().alpha())
        fillRectSourceOver(context, rect, fillColor());

    setPenColor(context, fillColor());
    TRect r = rect;
    context->gc().DrawRect(xForm(r));
}


// This is only used to draw borders.
void GraphicsContext::drawLine(const IntPoint& point1, const IntPoint& point2)
{
    if (paintingDisabled())
        return;

    WebCoreGraphicsContext* context = m_data->iContext;
    if (!context) 
        return;

    CFbsBitGc& gc = context->gc();

    int width = strokeThickness();
    if (width < 1)
        width = 1;

    setPenColor(context,strokeColor());

    int x1 = point1.x();
    int y1 = point1.y();
    int x2 = point2.x();
    int y2 = point2.y();
    
    switch(strokeStyle()) {
        case DottedStroke:
        case DashedStroke: {
            int dx = (x1 == x2) ? width : (strokeStyle() == DashedStroke) ? 2*width : width;
            int dy = (x1 == x2) ? ((strokeStyle() == DashedStroke) ? 2*width : width ) : width;
            //Need to do this because Symbian doesnt draw dotted lines with thickness
            setBrushColor(context,strokeColor());

            //Draw the first rect and the last rect
            TPoint pt1((x1 == x2)? x1 - dx/2 : x1,(x1 == x2)? y1 : y1 - dy/2);
            gc.DrawRect(TRect(xForm(pt1),xForm(TSize(dx,dy))));
            TPoint pt2((x1 == x2)? x1 - dx/2 : x2 - dx,(x1 == x2)? y2 - dy : y1 - dy/2);
            gc.DrawRect(TRect(xForm(pt2),xForm(TSize(dx,dy))));

            //Calculate the length of the remaining segments and apply correction
            //so that error because of remainder evens out
            int length = (x1 == x2) ? (y2 - y1) : (x2 - x1);
            length -= 2*((x1 == x2) ? dy : dx);
            int n = length/((x1 == x2) ? dy : dx);
            int reminder = length%width;
            bool even = (n%2 == 0);
            int delta = (x1 == x2) ? dy : dx;

            delta -= (reminder) ? ((even) ? (delta - reminder)/2 : -reminder/2) : ((even) ? -delta/2 : 0);

            int pos = ((x1 == x2) ? (y1 + dy) : (x1 + dx)) + delta;
            //Draw the remaining rects
            for(int i = 0; i < n/2 ; i++) {
                TPoint pt3((x1 == x2) ? x1 - dx/2 : pos,(x1 == x2) ? pos : y1 - dy/2);
                gc.DrawRect(TRect(xForm(pt3),xForm(TSize(dx,dy))));
                pos += (2*(( x1 == x2 ) ? dy : dx ));
            }
            break;
        }
        case SolidStroke:
        default:
            gc.DrawLine(xForm(TPoint(x1, y1)), xForm(TPoint(x2, y2)));
            break;
    }
}

// This method is only used to draw the little circles used in lists.
void GraphicsContext::drawEllipse(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    WebCoreGraphicsContext* context = m_data->iContext;
    if (!context)
        return;

    CFbsBitGc& gc = context->gc();

    if( fillColor().alpha() ) {
        setBrushColor( context, fillColor() );
        gc.DrawEllipse( xForm(rect) );
    }

    setPenColor( context, strokeColor() );
    gc.DrawEllipse( xForm(rect) );        
}

void GraphicsContext::strokeArc(const IntRect& rect, int startAngle, int angleSpan)
{
    if (paintingDisabled())
        return;

    WebCoreGraphicsContext* context = m_data->iContext;
    if (!context)
        return;

    CFbsBitGc& gc = context->gc();

    int x = xForm(rect.x());
    int y = xForm(rect.y());
    float w = xForm((float)rect.width());

    // starting point & end point
    float r = w/2;
    float fa = startAngle;
    TPoint ps( x + r + r*cos(fa * M_PI/180), y + r - r*sin( fa * M_PI/180 ) );
    fa += angleSpan;
    TPoint pe( x + r + r*cos(fa * M_PI/180), y + r - r*sin( fa * M_PI/180 ) );

    setPenColor( context, strokeColor() );
    gc.DrawArc( xForm(rect), ps, pe );
}

void GraphicsContext::drawConvexPolygon(size_t nPoints, const FloatPoint* points, bool /*shouldAntialias*/)
{
    if( paintingDisabled() )
        return;

    if( nPoints <= 1 )
        return;

    // points
    CArrayFixFlat<TPoint>* ptArray = new CArrayFixFlat<TPoint>(4);
    if( !ptArray ) return;

    for( int i=0; i<nPoints; ++i )
        ptArray->AppendL( xForm(points[i]) );

    WebCoreGraphicsContext* context = m_data->iContext;
    
    if (!context)
        return;
    
    CFbsBitGc& gc = context->gc();

    if( fillColor().alpha() ) {
        // symbian GDI drawPolygon for some reason doesn't draw the border of polygon
        // properly.  We need to draw the line segments using drawLineTo.
        if( nPoints == 4 ) {
            TPoint& p0 = ptArray->At(0);
            TPoint& p1 = ptArray->At(1);
            TPoint& p2 = ptArray->At(2);
            TPoint& p3 = ptArray->At(3);

            p1.iY--, p2.iY--, p2.iX--, p3.iX--;
            if (p0.iY>p3.iY) p0.iY--;
            if (p1.iX>p0.iX) p1.iX--;
            if (p2.iY<p1.iY) p2.iY++;
            if (p3.iX<p2.iX) p3.iX++;
        }

        setBrushColor( context, fillColor() );
        gc.DrawPolygon( ptArray );

        setPenColor( context, fillColor() );
        gc.MoveTo( ptArray->At(0) );
        for( int i=1; i<nPoints; ++i ) {
            gc.DrawLineTo( ptArray->At(i) );
        }
        gc.DrawLineTo( ptArray->At(0) );
    }

    if (strokeStyle() != NoStroke) {
        setPenColor( context, strokeColor() );
        gc.SetPenSize( TSize( strokeThickness(), strokeThickness() ) );
        gc.DrawPolygon( ptArray );
    }

    delete ptArray;
}

void GraphicsContext::fillRect(const IntRect& rect, const Color& color)
{
    if (paintingDisabled())
        return;

    if (color.alpha())
        fillRectSourceOver(m_data->iContext, rect, color);
}

void GraphicsContext::fillRect(const FloatRect& rect, const Color& color)
{
    if (paintingDisabled())
        return;

    if (color.alpha())
        fillRectSourceOver(m_data->iContext, enclosingIntRect(rect), color);
}

void GraphicsContext::clip(const IntRect& rect)
{
    CFbsBitGc* gContext = NULL;
    if (paintingDisabled())
        return;

    if (m_data->iContext)
        m_data->iContext->setClippingRect(xForm(rect));
}

void paintEdge(CFbsBitGc& gc, IntPoint p0, IntPoint p1, int w, Color col)
{
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    gc.SetBrushStyle(CGraphicsContext::ENullBrush);
    bool down = p0.y()<p1.y();
    bool up = p0.y()>p1.y();
    bool right = p0.x()<p1.x();
    for (int n=0; n<w; n++) {
        TRgb c(std::max(0,col.red()-50*n/w),std::max(0,col.green()-50*n/w),std::max(0,col.blue()-50*n/w),192);
        gc.SetPenColor(c);
        TPoint tp0 = p0;
        TPoint tp1 = p1;
        if (down) {
            tp0.iX = tp0.iX + w/2 - n;
            tp0.iY = tp0.iY - w/2 + (n==0?1:n);
            tp1.iX = tp1.iX + w/2 - n;
            tp1.iY = tp1.iY + w/2 - n;
        } else if (up) {
            tp0.iX = tp0.iX - w/2 + n;
            tp0.iY = tp0.iY + w/2 - (n==0?1:n);
            tp1.iX = tp1.iX - w/2 + n;
            tp1.iY = tp1.iY - w/2 + n;
        } else if (right) {
            tp0.iY = tp0.iY - w/2 + n;
            tp0.iX = tp0.iX - w/2 + (n==0?1:n);
            tp1.iY = tp1.iY - w/2 + n;
            tp1.iX = tp1.iX + w/2 - n;
        } else {
            tp0.iY = tp0.iY + w/2 - n;
            tp0.iX = tp0.iX + w/2 - (n==0?1:n);
            tp1.iY = tp1.iY + w/2 - n;
            tp1.iX = tp1.iX - w/2 + n;
        }
        gc.DrawLine(tp0,tp1);
    }
}

void GraphicsContext::drawFocusRing(const Color& color)
{
   if (paintingDisabled())
        return;
    int radius = (focusRingWidth() - 1) / 2;
    int offset = /*radius +*/ focusRingOffset();

    const Vector<IntRect>& rects = focusRingRects();
    RectPolygon* rectContour = new RectPolygon();

    // prepare the contour of polygons
    for( int i=0; i<rects.size(); ++i ) {
        const IntRect& r = rects[i];
        rectContour->append( IntRect( r.x() - offset, r.y() - offset, r.width() + 2*offset, r.height() + 2*offset ) );
    }

    WebCoreGraphicsContext* context = m_data->iContext;
    if (!context)
        return;

    CFbsBitGc& gc = context->gc();
    
    // paint the contour
    DeprecatedValueList<IntPoint>* contour = rectContour->nextContour();
    while (contour) {
        DeprecatedValueListIterator<IntPoint> it;
        IntPoint p0 = contour->last();
        IntPoint p1;
        for ( it = contour->begin(); it != contour->end() ; it++)
            {
            p1 = *it;
            //int radius = (data->focusRingWidth-1)/2;
            //Apply translation for zoomlevel when painting
            paintEdge(gc,xForm(p0),xForm(p1),focusRingWidth()-1,color);
            p0 = p1;
            }
        delete contour;
        contour = rectContour->nextContour();
    }

    delete rectContour;
}

void GraphicsContext::setFocusRingClip(const IntRect&)
{
}

void GraphicsContext::clearFocusRingClip()
{
}

void GraphicsContext::drawLineForText(const IntPoint& origin, int width, bool printing)
{
    if (paintingDisabled())
        return;

    IntPoint endPoint = origin + IntSize(width, 0);
    drawLine(origin, endPoint);
}

FloatRect GraphicsContext::roundToDevicePixels(const FloatRect& frect)
{
    return frect;
}

inline IntPoint RectEdge::begin(int edgeDirection) const 
{
    switch (edgeDirection) {
        case 0:
            return IntPoint( rect.x(), rect.y() );
        case 1:
            return IntPoint( rect.x() + rect.width(), rect.y() );
        case 2:
            return IntPoint( rect.x() + rect.width(), rect.y() + rect.height() );
        case 3:
        default:
            return IntPoint( rect.x(), rect.y() + rect.height() );
    }
}

void RectPolygon::removeUsed()
{
    // remove rectangles marked as used
    // if anything remains they are part of a different polygon
    bool hasUnused = true;
    DeprecatedValueListIterator<PolyRect> it;
    for ( it = rects.begin(); it != rects.end() ; ) {
        if ((*it).used) {
            if (hasUnused) {
                // also remove unused rectangles that touch used rectangles
                DeprecatedValueListIterator<PolyRect> it2;
                bool unusedRemaining = false;
                for ( it2 = rects.begin(); it2 != rects.end() ; ) {
                    if (!(*it2).used) {
                        if ((*it2).rect.intersects((*it).rect))
                             it2 = rects.remove(it2);
                        else {
                             unusedRemaining = true;
                             it2++;
                        }
                    } else
                        it2++;
                }
                if (!unusedRemaining)
                    hasUnused = false;
            }
            it = rects.remove(it);
        }
        else
            it++;
    }
}


bool RectPolygon::startingEdge(RectEdge& edge)
{
    // find a any point which is certain to be on an outside edge of a polygon
    IntPoint topLeft(1000000, 1000000);
    DeprecatedValueListIterator<PolyRect> it;
    PolyRect* res = 0;
    for ( it = rects.begin(); it != rects.end() ; it++) {
        IntPoint p = (*it).rect.location();
        if (p.y() < topLeft.y()) {
            res = &(*it);
            topLeft = p;
        } else if (p.y() == topLeft.y() && p.x()<topLeft.x()) {
            res = &(*it);
            topLeft = p;
        }
    }
    if (!res)
        return false;
    res->used = true;
    edge = RectEdge(res->rect,0);
    return true;
}


bool RectPolygon::findExtendEdge(const RectEdge& current, RectEdge& extendEdge)
{
    IntPoint c1 = current.end();
    DeprecatedValueListIterator<PolyRect> it;
    for ( it = rects.begin(); it != rects.end() ; it++) {
        if ((*it).rect == current.rect)
            continue;
        RectEdge searchEdge((*it).rect, current.direction);
        if ( searchEdge.contains(c1) && c1!=searchEdge.end() ){
            extendEdge = searchEdge;
            (*it).used = true;
            return true;
        }
    }
    return false;
}

bool RectPolygon::findCrossingEdge(const RectEdge& current, RectEdge& crossingEdge, IntPoint& lastCrossing)
{
    IntPoint c0 = lastCrossing;
    IntPoint c1 = current.end();
    DeprecatedValueListIterator<PolyRect> it;
    for ( it = rects.begin(); it != rects.end() ; it++) {
        if ((*it).rect == current.rect)
            continue;
        RectEdge searchEdge((*it).rect, current.direction==0?3:current.direction-1);
        IntPoint e0 = searchEdge.begin();
        IntPoint e1 = searchEdge.end();
        IntPoint crossingPoint;
        if ( lineCrosses(c0,c1,e0,e1,crossingPoint) && crossingPoint!=e1 ){
            crossingEdge = searchEdge;
            lastCrossing = crossingPoint;
            (*it).used = true;
            return true;
        }
    }
    return false;
}

RectEdge RectPolygon::nextEdge(const RectEdge& current, IntPoint& crossing)
{
    RectEdge next;
    bool found = findCrossingEdge(current, next, crossing);
    if (!found) {
        crossing = current.end();
        found = findExtendEdge(current, next);
    }
    if (!found)
        next = RectEdge(current.rect, (current.direction+1)%4);
    return next;
}

DeprecatedValueList<IntPoint>* RectPolygon::nextContour()
{
    DeprecatedValueList<IntPoint>* res = 0;
    RectEdge edge;
    if (startingEdge(edge)) {
        res = new DeprecatedValueList<IntPoint>();
        IntPoint start = edge.begin();
        IntPoint currentCrossing = start;
        res->append(start);
        do {
            RectEdge nextedge = nextEdge(edge, currentCrossing);
            if (!nextedge.extends(edge))
                res->append(currentCrossing);
            edge = nextedge;
        } while (edge.begin()!=start);

        removeUsed();
    }
    return res;
}

void GraphicsContext::clipOut(const Path&)
{
    notImplemented();
    // Called from RenderObject::paintBoxShadow
}

void GraphicsContext::clipOut(const IntRect& rect)
{
    if (paintingDisabled())
        return;
        
    // FIXME: Implement
    notImplemented();
    // Called from RenderObject::paintBoxShadow
    // or GraphicsContext::clipOutRoundedRect which is also called from RenderObject::paintBoxShadow
}

void GraphicsContext::fillRoundedRect(const IntRect& rect, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight, const Color& fillColor)
{
    if (paintingDisabled())
        return;
    
    // FIXME: Symbian API only supports equal-sized corners
    if (fillColor.alpha()) {
        WebCoreGraphicsContext* context = m_data->iContext;
        if (!context)
            return;

        CFbsBitGc& gc = context->gc();
        setBrushColor(context, fillColor);
        gc.DrawRoundRect(xForm(rect), xForm(topLeft));
    }
}

void GraphicsContext::clipOutEllipseInRect(const IntRect& rect)
{
    if (paintingDisabled())
        return;
    
    // FIXME: Implement.
    notImplemented();
    // Not called from anywhere 
}

void GraphicsContext::setPlatformStrokeColor(const Color& color)
{
    if (paintingDisabled())
        return;

    WebCoreGraphicsContext* context = m_data->iContext;
    if (!context)
        return;

    setPenColor(context, color);
}

void GraphicsContext::setPlatformStrokeThickness(float thickness)
{
    if (paintingDisabled())
        return;
    
    WebCoreGraphicsContext* context = m_data->iContext;
    if (!context)
        return;

    CFbsBitGc& gc = context->gc();
    gc.SetPenSize( TSize( thickness, thickness ) );
}

void GraphicsContext::setPlatformFillColor(const Color& col)
{
    if (paintingDisabled())
        return;
    
    notImplemented(); // commented-out because it's chatty and clutters output
}

void GraphicsContext::setURLForRect(const KURL& link, const IntRect& destRect)
{
    if (paintingDisabled())
        return;
    
    notImplemented();
    // Called from RenderObject::addPDFURLRect
}

void GraphicsContext::drawLineForMisspellingOrBadGrammar(const IntPoint&, int, bool)
{
    if (paintingDisabled())
        return;
    
    notImplemented();
}

}
