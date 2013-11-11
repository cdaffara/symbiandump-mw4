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


#ifndef __PATHSYMBIAN_H__
#define __PATHSYMBIAN_H__

#include "wtf/Vector.h"
#include "FloatRect.h"
#include "Path.h"

class PlatformPath
{
public:
    friend class WebCore::Path;

    enum PathType { 
        Ellipse, 
        Rectangle, 
        Polygon, 
        Line, 
        Circle,
        Unknown
    };

    PlatformPath() : m_points(0), m_type( Unknown )    {}
    ~PlatformPath();

    // only implemented Rectangle
    void addRect( const WebCore::FloatRect& rect );
    bool contains( const WebCore::FloatPoint& point, WebCore::WindRule );
    void addEllipse( const WebCore::FloatRect& );
    void addLineTo( const WebCore::FloatPoint& );
    void closeSubPath();
    void translate( const WebCore::FloatSize& );

private:
    WebCore::FloatRect                 m_boundingRect;
    WTF::Vector<WebCore::FloatPoint>   m_points;
    PathType                           m_type;    
};
#endif
