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
#include "Path.h"
#include "PathSymbian.h"

using namespace WebCore;

/*
The following code is based on an algorithm presented in "Algorithms" by Robert Sedgewick,
Addison-Wesley, 1988, 2nd ed. ISBN 0201066734 
*/

bool pointInPolygon(WTF::Vector<FloatPoint>& rgpts, const FloatPoint& ptTest);

bool pointInPolyRect(WTF::Vector<FloatPoint>& rgpts, const FloatPoint& ptTest, FloatRect *prbound) ;

bool intersects(const FloatPoint& p1, const FloatPoint& p2, const FloatPoint& p3, const FloatPoint& p4) ;

int  CCW(const FloatPoint& p0, const FloatPoint& p1, const FloatPoint& p2) ;

bool pointInPolygon(WTF::Vector<FloatPoint>& rgpts, const FloatPoint& ptTest)
{
    int wnumpts = rgpts.size();
    if (!wnumpts)
        return false;
    FloatRect r;
    int   i ;
    FloatPoint  pt1, pt2 ;
    int   wnumintsct = 0 ;

    if (!pointInPolyRect(rgpts,ptTest,&r)) return false;

    pt1 = ptTest ;
    pt2 = FloatPoint(r.right() + 50, ptTest.y()) ;

    // Now go through each of the lines in the polygon and see if it
    // intersects
    for (i = 0 ; i < wnumpts-1 ; i++) {
        if (intersects(pt1, pt2, rgpts[i], rgpts[i+1]))
            wnumintsct++ ;
    }

    // And the last line
    if (intersects(pt1, pt2, rgpts[wnumpts-1], rgpts[0]))
        wnumintsct++ ;

    return (wnumintsct&1) ;
}


bool pointInPolyRect(WTF::Vector<FloatPoint>& rgpts, const FloatPoint& ptTest, FloatRect *res)
{
    int wnumpts = rgpts.size();
    if (!wnumpts)
        return false;

    FloatRect r ;
    int   xmin, xmax, ymin, ymax ;
    FloatPoint *ppt ;
    int  i ;

    xmin = ymin = INT_MAX ;
    xmax = ymax = -INT_MAX ;

    for (i=0; i < wnumpts ; i++) {
        ppt = &rgpts[i];
        if (ppt->x() < xmin)
           xmin = ppt->x() ;
        if (ppt->x() > xmax)
           xmax = ppt->x() ;
        if (ppt->y() < ymin)
           ymin = ppt->y() ;
        if (ppt->y() > ymax)
           ymax = ppt->y() ;
    }
    r = FloatRect(xmin, ymin, xmax-xmin, ymax-ymin);

    if (res)
        *res = r;

    return r.contains(ptTest.x(),ptTest.y());
}

bool intersects(const FloatPoint& p1, const FloatPoint& p2, const FloatPoint& p3, const FloatPoint& p4)
{
    return ((( CCW(p1, p2, p3) * CCW(p1, p2, p4)) <= 0)
        && (( CCW(p3, p4, p1) * CCW(p3, p4, p2)  <= 0) )) ;
}


int CCW(const FloatPoint& p0, const FloatPoint& p1, const FloatPoint& p2)

{
    int dx1, dx2 ;
    int dy1, dy2 ;

    dx1 = p1.x() - p0.x() ; dx2 = p2.x() - p0.x() ;
    dy1 = p1.y() - p0.y() ; dy2 = p2.y() - p0.y() ;

    /* This is basically a slope comparison: we don't do divisions because
    * of divide by zero possibilities with pure horizontal and pure
    * vertical lines.
    */
    return ((dx1 * dy2 > dy1 * dx2) ? 1 : -1) ;
}


// FIXME: PlatformPath only supports Rectangle, Ellipse and Polygon right now

PlatformPath::~PlatformPath()
{
}

void PlatformPath::addRect( const FloatRect& rect )
{
    m_type = Rectangle;
    m_boundingRect = rect;
}

bool PlatformPath::contains( const FloatPoint& point, WindRule rule )
{
    if( m_type == Rectangle )
        return m_boundingRect.contains( point );
    else if( m_type == Polygon )
        return pointInPolygon( m_points, point );
    else if( m_type == Ellipse ) {
        /**
          in order for point(x,y) to stay in the ellipse,
          it has to fulfill the ellipse equation:
          sqr(dx)/sqr(a) + sqr(dy)/sqr(b) <= 1
          where 'a' is half the long axis of the ellipse, and 'b' is half the short axis
          x = a * cos(t) and y = b * sin(t)
          (see "http://www.3dsoftware.com/Math/PlaneCurves/EllipseAlgebra" for details)
        **/
        const FloatRect& r = m_boundingRect;
        int r2x = (r.width() * r.width()) / 4;      // sqr(a)
        int r2y = (r.height() * r.height()) / 4;    // sqr(b)

        if (!r2x || !r2y)  /*avoid division by zero*/
            return false;

        int cx = (r.x() + r.width()/2);             // center of ellipse, x
        int cy = (r.y() + r.height()/2);            // center of ellipse, y
        int dx = point.x() - cx;                    // dx
        int dy = point.y() - cy;                    // dy
        return (dx*dx*r2y + dy*dy*r2x <= r2x*r2y);        
    }
    else
        return false;
}

void PlatformPath::addLineTo( const FloatPoint& point )
{
    if( m_type == Unknown )
        m_type = Polygon;

    m_points.append( point );    
}

void PlatformPath::addEllipse( const FloatRect& rect )
{
    m_type = Ellipse;
    m_boundingRect = rect;
}

void PlatformPath::closeSubPath()
{
    // since the above intersection algorithm takes an extra step to test
    // last line segment in this polygon, we don't need to close the polygon here.
}

void PlatformPath::translate( const FloatSize& sz )
{
    float deltaX = sz.width();
    float deltaY = sz.height();

    if (m_type == Polygon) {
        for (int i = 0 ; i < m_points.size() ; i++) {
            m_points[i] = FloatPoint(m_points[i].x()+deltaX, m_points[i].y()+deltaY);
        }
    } else {
        m_boundingRect.setX(m_boundingRect.x()+deltaX);
        m_boundingRect.setY(m_boundingRect.y()+deltaY);
    }    
}

// WebCore::Path member functions

Path::Path()
{
    m_path = new PlatformPath();
}

Path::~Path()
{
    delete m_path;
}

Path::Path(const Path& p)
{
    m_path = new PlatformPath();
    // deep copy
    *m_path = *(p.m_path);
}

Path& Path::operator=(const Path& other)
{ 
    if( !m_path ) 
        m_path = new PlatformPath();
    // deep copy
    *m_path = *(other.m_path);

    return *this; 
}

bool Path::contains(const FloatPoint& point, WindRule rule) const
{ 
    return m_path->contains( point, rule ); 
}

void Path::addRect(const FloatRect& rect) 
{ 
    m_path->addRect( rect );
}

FloatRect Path::boundingRect() const 
{ 
    return m_path->m_boundingRect; 
}

void Path::translate(const FloatSize& sz)
{ 
    m_path->translate( sz );
}

void Path::addEllipse(const FloatRect& rect ) 
{ 
    m_path->addEllipse( rect );
}

void Path::addLineTo(const FloatPoint& point) 
{ 
    m_path->addLineTo( point );
}

void Path::closeSubpath()
{
    m_path->closeSubPath();
}

void Path::clear()
{
    m_path->m_type = PlatformPath::Unknown;
    m_path->m_points.clear();
    m_path->m_boundingRect = FloatRect();
}

// END OF FILE
