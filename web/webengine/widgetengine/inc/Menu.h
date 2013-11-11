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



#ifndef __MENU_H
#define __MENU_H

//  INCLUDES
#include "config.h"
#include <e32base.h>
#include <kjs/object.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION
class MJSMenuCallbacks;
class MJSObjectProtector;

/**
*  CMenu
*
*  @lib widgetengine.dll
*  @since 3.1
*/
namespace KJS {

class WidgetEventHandler;

struct MenuPrivate
{
    MenuPrivate(MJSMenuCallbacks* callbacks,
				MJSObjectProtector* protector,
                WidgetEventHandler* onShowCallback = NULL,
                WidgetEventHandler* rightKeyCallback = NULL,
                WidgetEventHandler* leftKeyCallback = NULL) : 
                    m_callbacks(callbacks),
					m_protector(protector),
                    m_onShowCallback(onShowCallback),
                    m_rightKeyCallback(rightKeyCallback),
                    m_leftKeyCallback(leftKeyCallback)
    {        
    }

    
    virtual ~MenuPrivate() { delete m_onShowCallback;
                             delete m_rightKeyCallback;
                             delete m_leftKeyCallback;}
    
    MJSMenuCallbacks* m_callbacks;
	MJSObjectProtector* m_protector;
    WidgetEventHandler* m_onShowCallback;
    WidgetEventHandler* m_rightKeyCallback;        
    WidgetEventHandler* m_leftKeyCallback;
};

class JSMenu: public JSObject
    {

    public:
        JSMenu(MJSMenuCallbacks* aMenuCallbacks, MJSObjectProtector* aProtector);
        virtual ~JSMenu();

    //From JSObject
    public:        
        JSType type() const;

        JSValue* getValueProperty(KJS::ExecState*, int token) const;
        bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);
      
        bool canPut(ExecState *exec, const Identifier &propertyName) const;
        void put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr = None);

        const ClassInfo* classInfo() const { return &info; }
        static const ClassInfo info;

        enum {
            Append,
            Remove,
            GetMenuItemById,
            GetMenuItemByName,
            SetRightSoftKeyLabel,
            OnShow,
            Clear,
            ToString,
            HideSoftkeys,
            ShowSoftkeys,
            Location,
            SetLeftSoftKeyLabel
            };


    public:
        virtual UString toString(ExecState *exec) const;
        void setRightKeyCallback( ExecState *exec, JSValue *value );
        WidgetEventHandler* rightKeyCallback() { return d->m_rightKeyCallback; };
        WidgetEventHandler* onShowCallback() { return d->m_onShowCallback; };    
        
        void setLeftKeyCallback( ExecState *exec, JSValue *value );
        WidgetEventHandler* leftKeyCallback() { return d->m_leftKeyCallback; };

    private:
        MenuPrivate* d;
    };

};

#include "Menu.lut.h"


#endif
