/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* 
*/

// System includes
#include <QGraphicsWebView>

// User includes
#include "wlanloginwebview.h"
#include "wlanloginwebpage.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanloginwebviewTraces.h"
#endif

/*!
    \class WlanLoginWebView
    \brief WLAN Login web view class implementation.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor       
 */
WlanLoginWebView::WlanLoginWebView(): 
    QGraphicsWebView(),
    mWebPage(NULL),
    mMousePressed(false)
{
    OstTraceFunctionEntry0( WLANLOGINWEBVIEW_WLANLOGINWEBVIEW_ENTRY );

    installEventFilter(this); //Event filter for preventing text selection when scrolling
    
    mWebPage = new WlanLoginWebPage(this);
    setPage(mWebPage);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
           
    OstTraceFunctionExit0( WLANLOGINWEBVIEW_WLANLOGINWEBVIEW_EXIT );
}


/*!
    Destructor
        
 */
WlanLoginWebView::~WlanLoginWebView()
{
    OstTraceFunctionEntry0( WLANLOGINWEBVIEW_DESTRUCTOR_ENTRY );
    OstTraceFunctionExit0( WLANLOGINWEBVIEW_DESTRUCTOR_EXIT );
}

/*!
    This eventfilter filters QEvent::GraphicsSceneMouseMove events from Web View while
    mouse (finger) is pressed down. If filtering is not done it causes text selection on
    the web page when page is scrolled with finger.
    
    @param [in] event Pointer to received event
        
 */
bool WlanLoginWebView::eventFilter(QObject *, QEvent *event)
{ 
     switch (event->type()) {
         case QEvent::GraphicsSceneMousePress:
                 mMousePressed = true;            
             break;
         case QEvent::GraphicsSceneMouseRelease:
                 mMousePressed = false;
             break;
         case QEvent::GraphicsSceneMouseMove:
             if (mMousePressed)
                 return true;
             break;
     default:
         break;
     }      
     return false;
}

