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



#ifndef __MENUITEM_H
#define __MENUITEM_H

//  INCLUDES
#include "config.h"
#include <e32base.h>
#include <kjs/object.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION
class MJSMenuItemCallbacks;
class MJSObjectProtector;

/**
*  CMenuItem
*
*  @lib widgetengine.dll
*  @since 3.1
*/
namespace KJS {

class WidgetEventHandler;

struct MenuItemConstructorPrivate
{
	MenuItemConstructorPrivate(	MJSMenuItemCallbacks* callbacks, 
								MJSObjectProtector* protector, 
								int internalId) : m_callbacks(callbacks), m_protector(protector), m_internalId(internalId)
	{
	}

	MJSMenuItemCallbacks*	m_callbacks;
	MJSObjectProtector*		m_protector;
    int						m_internalId;
};

class JSMenuItemConstructor : public JSObject 
{

public:
    JSMenuItemConstructor(MJSMenuItemCallbacks* callbacks, MJSObjectProtector* protector);
    virtual bool implementsConstruct() const;    
    virtual JSObject* construct( ExecState *exec, const List &args );

	virtual ~JSMenuItemConstructor();

private:
    MenuItemConstructorPrivate*  d;

};


struct MenuItemPrivate
{
    MenuItemPrivate(MJSMenuItemCallbacks* callbacks,
					MJSObjectProtector* protector,
                    int cmdId, int internalId,
                    WidgetEventHandler* selectCallback = NULL) 
					: m_callbacks(callbacks),
                    m_cmdId(cmdId),
                    m_internalId(internalId),
                    m_dimmed(false),
                    m_show(false),
                    m_onSelectCallback(selectCallback),
					m_protector(protector)
    {        
    }

    
    virtual ~MenuItemPrivate() { delete m_onSelectCallback; }
    
    MJSMenuItemCallbacks*   m_callbacks;
    MJSObjectProtector*		m_protector;
    const int               m_cmdId;
    bool                    m_dimmed;
    bool                    m_show;
    WidgetEventHandler*     m_onSelectCallback;
    const int               m_internalId;    

};


class JSMenuItem : public JSObject
{
public:
    JSMenuItem(ExecState* exec, MJSMenuItemCallbacks* callbacks, 
				MJSObjectProtector* protector,
                TDesC& text, int cmdId, int internalId,
                WidgetEventHandler* selectCallback = NULL );

    virtual ~JSMenuItem();    
    
//From JSObject
public:
    JSValue* getValueProperty(ExecState*, int token) const;
    bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);
  
    bool canPut(ExecState *exec, const Identifier &propertyName) const;
    void put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr = None);

    const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;

    enum {
        Append,
        Remove,
        SetDim,
        OnSelect,
        ToString
        };

    virtual UString toString(ExecState *exec) const;

public:    
    int Id() const;
    int InternalId() const;
    bool Show() const;
    bool Dimmed() const;
    void SetShow(bool val);
    void SetDimmed(bool val);
    WidgetEventHandler* callback() const;
    
    void AddOptionsMenuItem(bool show);
    void DeleteMenuItem();        


private:
    MenuItemPrivate *d;

};

};

#include "MenuItem.lut.h"


#endif
