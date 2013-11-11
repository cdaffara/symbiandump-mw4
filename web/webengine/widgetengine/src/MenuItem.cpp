/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDES

#include <eikamnt.h>

#include "MenuItem.h"
#include "MenuItemFuncs.h"
#include "MenuItemCallbacks.h"
#include "WidgetJSObjectProtector.h"

using namespace KJS;

// ----------------------------------------------------------------------------
// JSMenuItemConstructor::JSMenuItemConstructor
// Default constructor
//
//
// ----------------------------------------------------------------------------
JSMenuItemConstructor::JSMenuItemConstructor(MJSMenuItemCallbacks* callbacks, MJSObjectProtector* protector)
    : JSObject(),
	d(new MenuItemConstructorPrivate(callbacks, protector, 0))
{
}

JSMenuItemConstructor::~JSMenuItemConstructor()
{
	delete d;
}

// ----------------------------------------------------------------------------
// JSMenuItemConstructor::implementsConstruct
// returns true if this object implements the construct() method, otherwise
// returns false
//
// ----------------------------------------------------------------------------
bool JSMenuItemConstructor::implementsConstruct() const
{
    return true;
}

// ----------------------------------------------------------------------------
// JSMenuItemConstructor::construct
// Creates a new JSMenuItem object
//
//
// ----------------------------------------------------------------------------
JSObject* JSMenuItemConstructor::construct( ExecState* exec, const List& aList)
{

    UString text;
    int cmdId = 0;

    if ( aList.size() > 1 ) {
    
        JSValue* t = aList.at( 0 );
        JSValue* id = aList.at( 1 );
        cmdId = id->toInt32( exec );
        text = t->toString( exec );

        if ( ( t->type() == StringType ) && 
             ( id->type() == NumberType ) && 
             ( cmdId >= 0 ) && 
             ( text.size() > 0 ) ) {
            
            text = t->toString( exec );
           
            int textlen = (text.size() > CEikAutoMenuTitle::ENominalTextLength) ?
                        CEikAutoMenuTitle::ENominalTextLength : text.size(); 

            TPtrC tptrc((const unsigned short*)(text.data()), textlen);

            JSMenuItem* mi = new JSMenuItem(exec, d->m_callbacks, d->m_protector, tptrc, cmdId, d->m_internalId, 0);
			
            d->m_callbacks->createOptionsMenuItem( tptrc, cmdId, d->m_internalId++, (void*) mi );
           
            return static_cast<KJS::JSObject*>(mi);
        }
    }

    return throwError(exec, GeneralError);
}


// ------------------------------ JSMenuItem -----------------------------

const ClassInfo JSMenuItem::info = { "MenuItem", 0, &TMenuItemTable, 0 };

/*
@begin TMenuItemTable 5
  append    JSMenuItem::Append      DontDelete|Function 1
  remove    JSMenuItem::Remove      DontDelete|Function 1
  setDimmed JSMenuItem::SetDim      DontDelete|Function 1
  onSelect  JSMenuItem::OnSelect    DontDelete|ReadOnly
  toString  JSMenuItem::ToString    DontDelete|Function 0
@end
*/


// ----------------------------------------------------------------------------
// JSMenuItem::JSMenuItem
// Default constructor
//
//
// ----------------------------------------------------------------------------
JSMenuItem::JSMenuItem(  
    ExecState* exec,   
    MJSMenuItemCallbacks* callbacks,
	MJSObjectProtector* protector,
    TDesC& text,
    int cmdId, 
    int internalId,
    WidgetEventHandler* onSelectCallback )
    : JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()),      
      d(new MenuItemPrivate(callbacks, protector, cmdId, internalId, onSelectCallback))  
    {
		d->m_protector->Protect(this);
    }
    
// ----------------------------------------------------------------------------
// JSMenuItem::~JSMenuItem
// Destructor
//
//
// ----------------------------------------------------------------------------
JSMenuItem::~JSMenuItem()
{
    delete d;
}

int JSMenuItem::Id() const
{ 
    return d->m_cmdId; 
} 
       
int JSMenuItem::InternalId()  const
{ 
    return d->m_internalId; 
}        

bool JSMenuItem::Dimmed()  const
{ 
    return d->m_dimmed; 
}

bool JSMenuItem::Show()  const
{ 
    return d->m_show; 
}

void JSMenuItem::SetDimmed(bool val)
{ 
    d->m_dimmed=val; 
    d->m_callbacks->setDimmed(d->m_internalId, val);
}                

void JSMenuItem::SetShow(bool val) 
{ 
    d->m_show=val; 
}       

WidgetEventHandler* JSMenuItem::callback()  const
{ 
    return d->m_onSelectCallback; 
}

// ----------------------------------------------------------------------------
// JSMenuItem::AddOptionsMenuItem
//
//
// ----------------------------------------------------------------------------
void JSMenuItem::AddOptionsMenuItem(bool show)
{    
 
    // if no error (KErrNone) in adding the menu item
    if ( ( d->m_callbacks->addOptionsMenuItem(d->m_internalId, -1, ETrue ) == KErrNone ) ) {        
        SetShow( show );
        if ( d->m_callbacks ) {
            d->m_callbacks->setMenuItemObserver(d->m_internalId, d->m_onSelectCallback);
        }
    }    
}

// ----------------------------------------------------------------------------
// JSMenuItem::DeleteMenuItem
//
//
// ----------------------------------------------------------------------------
void JSMenuItem::DeleteMenuItem()
{
    d->m_callbacks->deleteMenuItem( d->m_internalId );
    d->m_protector->Unprotect(this);
}

// ----------------------------------------------------------------------------
// JSMenuItem::toString
// Returns string representation of the object
//
//
// ----------------------------------------------------------------------------
UString JSMenuItem::toString( ExecState *exec ) const
{
    return "[object MenuItem]";
}

// ----------------------------------------------------------------------------
// JSMenuItem::canPut
//
//
//
// ----------------------------------------------------------------------------
bool JSMenuItem::canPut(ExecState *exec, const Identifier &propertyName) const 
{ 
    return true; 
}        

// ----------------------------------------------------------------------------
// JSMenuItem::put
// Sets the specified property
//
//
// ----------------------------------------------------------------------------
void JSMenuItem::put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr)
{
    const HashEntry * entry = Lookup::findEntry( &TMenuItemTable, propertyName );

    // if the property exists in the table
    if ( entry ) {
        
        // changes the properties of the Menu element
        switch ( entry->value ) {
            case OnSelect: {
                delete d->m_onSelectCallback;
                d->m_onSelectCallback = NULL;
                d->m_onSelectCallback = new WidgetEventHandler(value, exec->lexicalInterpreter()->globalExec(), d->m_protector);
                d->m_callbacks->setMenuItemObserver( d->m_internalId, d->m_onSelectCallback );
                break;
            }
            default:
                break;
        }
    }
        
}


// ----------------------------------------------------------------------------
// MenuPrototypeImp::getOwnPropertySlot
//
//
//
// ----------------------------------------------------------------------------
bool JSMenuItem::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    const HashEntry* entry = Lookup::findEntry(&TMenuItemTable, propertyName);
    if (entry) {
        slot.setStaticEntry(this, entry, staticValueGetter<JSMenuItem>);
        return true;
    }
   
    return JSObject::getOwnPropertySlot(exec, propertyName, slot);    
}
      
// ----------------------------------------------------------------------------
// JSMenuItem::getValueProperty
// Retrieves the specified property from the object. If neither the object
// or any other object in it's prototype chain have the property, this function
// will return Undefined
//
//
// ----------------------------------------------------------------------------
JSValue* JSMenuItem::getValueProperty(KJS::ExecState* exec, int token) const
{
    
    switch ( token ) {
        case Append:
        case Remove:
        case SetDim:
        case ToString: {
            KJS::JSMenuItemFunc *mif = new KJS::JSMenuItemFunc(exec, d->m_callbacks, token);
            return mif;
        }
        default:
            break;
    }

    return throwError(exec, GeneralError);
}

