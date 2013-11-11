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
#include "Font.h"
#include "GraphicsContext.h"
#include "Widget.h"
#include "WebCoreWidget.h"
#include "IntRect.h"
#include "IntPoint.h"
#include "Cursor.h"
#include "Event.h"
#include "EventNames.h"
#include "KeyboardEvent.h"

namespace WebCore {
using namespace EventNames;
static bool deferFirstResponderChanges;
static Widget *deferredFirstResponder;

class WidgetPrivate
    {
    public:
        Font font;
        MWebCoreWidget* view;
        WidgetClient* client;
        bool visible;
        bool mustStayInWindow;
        bool removeFromSuperviewSoon;
    };

Widget::Widget() : data(new WidgetPrivate)
    {
    data->view = NULL;
    data->client = NULL;
    data->visible = true;
    data->mustStayInWindow = EFalse;
    data->removeFromSuperviewSoon = EFalse;
    }

Widget::Widget(MWebCoreWidget* view) : data(new WidgetPrivate)
    {
    data->view = view;
    data->client = NULL;
    data->visible = true;
    data->mustStayInWindow = EFalse;
    data->removeFromSuperviewSoon = EFalse;
    }

Widget::~Widget()
    {
    if( data->view )
        {
        data->view->deref();
        }
    if (deferredFirstResponder == this)
        {
        deferredFirstResponder = NULL;
        }
    delete data;
    }

void Widget::setEnabled(bool enabled)
    {
    if( getView() )
        {
        getView()->makeVisible(enabled);
        }
    }

bool Widget::isEnabled() const
    {
    if( getView() )
        {
        return getView()->isVisible();
        }
    return EFalse;
    }

IntRect Widget::frameGeometry() const
    {
    if( getOuterView() )
        {
        return IntRect( getOuterView()->rect() );
        }
    return IntRect();
    }

bool Widget::hasFocus() const
    {
    if( getView() )
        {
        return  getView()->isFocused();
        }
    return EFalse;
   }

void Widget::setFocus()
    {
    if (hasFocus())
        {
        return;
        }

    if( getView() )
        {
        getView()->setFocus( ETrue );
        }
    }

void Widget::setCursor(const Cursor& cursor)
{
    /*for (id view = data->view; view; view = [view superview]) {
        if ([view respondsToSelector:@selector(setDocumentCursor:)]) {
            if ([view respondsToSelector:@selector(documentCursor)] && cursor.impl() == [view documentCursor])
                break;
            [view setDocumentCursor:cursor.impl()];
            break;
        }
    }*/
    getView()->setCursor( cursor.impl() ? cursor.impl()->type() : PointerCursor );
}

void Widget::show()
    {
    if (!data || data->visible)
        {
        return;
        }

    data->visible = true;

    if( getOuterView() )
        {
        getOuterView()->makeVisible( ETrue );
        }
    }

void Widget::hide()
    {
    if (!data || !data->visible)
        {
        return;
        }

    data->visible = false;

    if( getOuterView() )
        {
        getOuterView()->makeVisible( EFalse );
        }
    }

void Widget::setFrameGeometry(const IntRect &rect)
    {
    if( getOuterView() )
        {
        MWebCoreWidget* view = getOuterView();
        if (view->rect() != rect.Rect() )
            {
            view->setRect(rect.Rect());
            view->setNeedsDisplay( EFalse );
            }
        }
    }


MWebCoreWidget* Widget::getView() const
    {
    return data->view;
    }

void Widget::setView(MWebCoreWidget* view)
    {
    if (data->view)
        data->view->deref();
    data->view = view;
    if (data->view)
        data->view->ref();
    }

MWebCoreWidget* Widget::getOuterView() const
    {
    // If this widget's view is a WebCoreFrameView the we resize its containing view, a WebFrameView.

    /*
    NSView* view = data->view;
    if ([view conformsToProtocol:@protocol(WebCoreFrameView)]) {
        view = [view superview];
        ASSERT(view);
    }*/

    MWebCoreWidget* view = getView();
    return view;
    }

void Widget::paint(GraphicsContext* p, const IntRect& r)
    {
    if (p->paintingDisabled())
        {
        return;
        }
    if( getView() )
        {
        getView()->draw( *p->platformContext(), r.Rect() );
        }
    }

void Widget::setIsSelected(bool isSelected)
    {
    //[FrameMac::bridgeForWidget(this) setIsSelected:isSelected forView:getView()];
    }

void Widget::setClient(WidgetClient* c)
    {
    data->client = c;
    }

WidgetClient* Widget::client() const
    {
    return data->client;
    }

bool Widget::isFocusable() const
    {
    if (getView())
        return getView()->isFocusable();
    return false;
    }

void Widget::handleEvent(Event* event) 
{ 
    if (event->type() == keydownEvent && event->isKeyboardEvent()) {
        KeyboardEvent* kevt = static_cast<KeyboardEvent*>(event);
        if (kevt->keyIdentifier() == "Enter") {
            MWebCoreWidget* view = getView();
            if(view && view->isObjectView()) {
                view->activate();
            }
        }
    }
}

}
