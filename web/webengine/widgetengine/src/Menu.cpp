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

#include "config.h"
#include <e32std.h>
#include <kjs/object.h>
#include <AknUtils.h>

#include "Menu.h"
#include "MenuFuncs.h"
#include "MenuCallbacks.h"


using namespace KJS;

// ------------------------------ JSMenu -----------------------------

const ClassInfo JSMenu::info = { "JSMenu", 0, &MenuTable, 0 };


/*
@begin MenuTable 11
  append            JSMenu::Append              DontDelete|Function 1
  remove            JSMenu::Remove              DontDelete|Function 1
  getMenuItemById   JSMenu::GetMenuItemById     DontDelete|Function 1
  getMenuItemByName JSMenu::GetMenuItemByName   DontDelete|Function 1
  setRightSoftkeyLabel JSMenu::SetRightSoftKeyLabel DontDelete|Function 2
  onShow            JSMenu::OnShow              DontDelete|ReadOnly
  clear             JSMenu::Clear               DontDelete|Function 0
  toString          JSMenu::ToString            DontDelete|Function 0
  hideSoftkeys      JSMenu::HideSoftkeys        DontDelete|Function 0
  showSoftkeys      JSMenu::ShowSoftkeys        DontDelete|Function 0
  setLeftSoftkeyLabel JSMenu::SetLeftSoftKeyLabel DontDelete|Function 2
  location          JSMenu::Location            DontDelete|ReadOnly
@end
*/


// ----------------------------------------------------------------------------
// JSMenu::JSMenu
// Default contructor
//
//
// ----------------------------------------------------------------------------
JSMenu::JSMenu(MJSMenuCallbacks* aMenuCallbacks, MJSObjectProtector* aProtector)
        : JSObject(), 
        d(new MenuPrivate(aMenuCallbacks, aProtector, 0,0))
{  
}


// ----------------------------------------------------------------------------
// JSMenu::~JSMenu
// Destructor
//
//
// ----------------------------------------------------------------------------
JSMenu::~JSMenu()
{    
    delete d;
}


// ----------------------------------------------------------------------------
// JSMenu::type
//
//
// ----------------------------------------------------------------------------
JSType JSMenu::type() const 
{ 
    return ObjectType; 
}

// ----------------------------------------------------------------------------
// JSMenu::toString
// Returns string representation of the object
//
//
// ----------------------------------------------------------------------------
UString JSMenu::toString( ExecState *exec ) const
{
    return "[object JSMenu]";
}


// ----------------------------------------------------------------------------
// JSMenu::setRightKeyObserver
// Sets the callback fuction for right softkey press
//
//
// ----------------------------------------------------------------------------
void JSMenu::setRightKeyCallback( ExecState *exec, JSValue *value )
{
    if (d->m_rightKeyCallback)
        d->m_rightKeyCallback->Delete();
    
    d->m_rightKeyCallback = NULL;
    if ( value && !value->isNull() ) {        
        d->m_rightKeyCallback = new WidgetEventHandler(value, exec->lexicalInterpreter()->globalExec(), d->m_protector);
    }
}

// ----------------------------------------------------------------------------
// JSMenu::setRightKeyObserver
// Sets the callback fuction for right softkey press
//
//
// ----------------------------------------------------------------------------
void JSMenu::setLeftKeyCallback( ExecState *exec, JSValue *value )
{
    if (d->m_leftKeyCallback)
        d->m_leftKeyCallback->Delete();
    
    d->m_leftKeyCallback = NULL;
    if ( value && !value->isNull() ) {        
        d->m_leftKeyCallback = new WidgetEventHandler(value, exec->lexicalInterpreter()->globalExec(), d->m_protector);
    }
}


// ----------------------------------------------------------------------------
// JSMenu::getOwnPropertySlot
//
//
//
// ----------------------------------------------------------------------------
bool JSMenu::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    
    const HashEntry* entry = Lookup::findEntry(&MenuTable, propertyName);
    if (entry) {
        slot.setStaticEntry(this, entry, staticValueGetter<JSMenu>);
        return true;
    }   

    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}


// ----------------------------------------------------------------------------
// JSMenu::get
// Retrieves the specified property from the object. If neither the object
// or any other object in it's prototype chain have the property, this function
// will return Undefined
//
//
// ----------------------------------------------------------------------------
JSValue* JSMenu::getValueProperty(KJS::ExecState* exec, int token) const
{
    
    switch( token )
    {
        case Append:
        case Remove:
        case GetMenuItemById:
        case GetMenuItemByName:
        case SetRightSoftKeyLabel:
        case Clear:
        case ToString:
        case HideSoftkeys:
        case ShowSoftkeys:  
        case SetLeftSoftKeyLabel: {
            JSMenuFunc *mf = new JSMenuFunc(exec, d->m_callbacks, token);
            return mf;
        }
            
        case Location: {
            // The return values are based on the document that describes softkey location
            // -1 The command is not supported on this system
            // 0  The softkeys are located on the top of the device screen
            // 1  The softkeys are located on the left of the device screen
            // 2  The softkeys are located on the bottom of the device screen
            // 3  The softkeys are located on the right of the device screen
        
            int ret = -1; 
            AknLayoutUtils::TAknCbaLocation loc = AknLayoutUtils::CbaLocation();
            if ( loc == AknLayoutUtils::EAknCbaLocationBottom )
            {
                ret = 2;
            }
            else if ( loc == AknLayoutUtils::EAknCbaLocationRight )
            {
                ret = 3;
            }
            else if ( loc == AknLayoutUtils::EAknCbaLocationLeft )
            {
                ret = 1;
            }    
            return jsNumber(ret);
        }
        
    }

    return throwError(exec, GeneralError);
}


// ----------------------------------------------------------------------------
// JSMenu::canPut
//
//
//
// ----------------------------------------------------------------------------
bool JSMenu::canPut(ExecState *exec, const Identifier &propertyName) const
{    
    return true;
}

// ----------------------------------------------------------------------------
// JSMenu::put
// Sets the specified property
//
//
// ----------------------------------------------------------------------------

void JSMenu::put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr)
{
    const HashEntry * entry = Lookup::findEntry( &MenuTable, propertyName );

    // if the property exists in the table
    if ( entry ){
    
        // changes the properties of the JSMenu element
        switch ( entry->value )
        {
            case OnShow: {
                delete d->m_onShowCallback;
                d->m_onShowCallback = NULL;
                d->m_onShowCallback = new WidgetEventHandler(value, exec->lexicalInterpreter()->globalExec(), d->m_protector);
                break;
            }
            default:
                break;
        }
    }
}





