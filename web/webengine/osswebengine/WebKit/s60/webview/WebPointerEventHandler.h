/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implemetation of WebPointerEventHandler
*
*/

#ifndef __WEBPOINTEREVENTHANDLER_H__
#define __WEBPOINTEREVENTHANDLER_H__

#include <e32base.h>
#include <w32std.h>
#include "Timer.h"
#include <rt_gesturehelper.h>

namespace WebCore
{
    class Node;
    class Element;
};

class WebView;
class CActiveSchedulerWait;

class WebPointerEventHandler : public CBase, public RT_GestureHelper::MGestureObserver
{
public:
    static WebPointerEventHandler* NewL(WebView* view);     
    void ConstructL();
    virtual ~WebPointerEventHandler();

public:
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    void HandleHighlightChange(const TPoint &aPoint);

    virtual void HandleGestureL( const RT_GestureHelper::TGestureEvent& aEvent );
    
    
private:    
    bool checkForEventListener(WebCore::Node* node);
    bool canDehighlight(const TPoint &aPoint);
    void dehighlight();

    bool isHighlitableElement(TBrCtlDefs::TBrCtlElementType& elType);
    TBrCtlDefs::TBrCtlElementType highlitableElement();
    void buttonDownTimerCB(WebCore::Timer<WebPointerEventHandler>* t);
    void handleTouchDownL(const RT_GestureHelper::TGestureEvent& aEvent);
    void handleTouchUp(const RT_GestureHelper::TGestureEvent& aEvent);
    void handleTapL(const RT_GestureHelper::TGestureEvent& aEvent);
    void handleDoubleTap(const RT_GestureHelper::TGestureEvent& aEvent);
    void handleMove(const RT_GestureHelper::TGestureEvent& aEvent);
    void doTapL();
    void updateCursor(const TPoint& pos);
private:
    WebPointerEventHandler(WebView* view);            
    WebView* m_webview;

               
    TPointerEvent m_buttonDownEvent;
    TPointerEvent m_lastTapEvent;
    
    // Support for link highlight/dehighlight
    TBool m_isHighlighted;
    TInt m_offset;
    TPoint m_highlightPos;
    WebCore::Node* m_highlightedNode;
    TPointerEvent m_currentEvent;
    WebCore::Timer<WebPointerEventHandler> m_buttonDownTimer; 
    
    RT_GestureHelper::CGestureHelper* m_gestureHelper;
    TPointerEvent m_lastPointerEvent;
    bool   m_ignoreTap; 
    CActiveSchedulerWait*    m_waiter; 
    
};



#endif
