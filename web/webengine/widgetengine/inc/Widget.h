/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __WIDGET_H
#define __WIDGET_H

//  INCLUDES
#include "config.h"
#include <e32base.h>
#include <kjs/object.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

class MJSWidgetCallbacks;
class MJSObjectProtector;

/**
*  CWidget
*
*  @lib widgetengine.dll
*  @since 3.1
*/
namespace KJS {

class WidgetEventHandler;
class JSWrt;

struct WidgetPrivate
{
    WidgetPrivate( MJSWidgetCallbacks* callbacks,
                    WidgetEventHandler* showCallback,
                    WidgetEventHandler* hideCallback,
                    WidgetEventHandler* exitCallback,
                    bool visibility,
					MJSObjectProtector* protector) :
                        m_callbacks(callbacks),
                        m_showCallback(showCallback),
                        m_hideCallback(hideCallback),
                        m_exitCallback(exitCallback),
                        m_visibility(visibility),
						m_protector(protector),
                        m_wrt(0)
    {
    }


    virtual ~WidgetPrivate() { delete m_exitCallback;
                               delete m_hideCallback;
                               delete m_showCallback; }

    MJSWidgetCallbacks*		m_callbacks;
    WidgetEventHandler*		m_showCallback;
    WidgetEventHandler*		m_hideCallback;
    WidgetEventHandler*		m_exitCallback;
	MJSObjectProtector*		m_protector;
    bool m_visibility;
    JSWrt* m_wrt;

};

class JSWidget : public JSObject
{

public:
    JSWidget(MJSWidgetCallbacks* aWidgetCallbacks, MJSObjectProtector* aProtector);
    virtual ~JSWidget();

//From JSObject
public:
    JSType type() const;
    JSValue* getValueProperty(KJS::ExecState*, int token) const;
    bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);
    bool canPut(ExecState *exec, const Identifier &propertyName) const;
    void put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr = None);
    const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;
    virtual void mark();
    virtual UString toString(ExecState *exec) const;
    void setVisibility(bool visibility);
    void handleExit();

    enum {
        openApplication,
        openURL,
        preferenceForKey,
        prepareForTransition,
        performTransition,
        setPreferenceForKey,
        setNavigationEnabled,
        setNavigationType,
        setDisplayLandscape,
        setDisplayPortrait,
        isRotationSupported,
        identifier,
        onhide,
        onremove,
        onshow,
        onexit,
        wrt
        };


private:
    WidgetPrivate* d;

};

};

#include "Widget.lut.h"

#endif
