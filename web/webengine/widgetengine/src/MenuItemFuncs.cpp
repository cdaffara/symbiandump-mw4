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
#include <kjs/object.h>
#include <e32std.h>
#include <lookup.h>
#include <eikamnt.h>

#include "MenuItem.h"
#include "MenuItemFuncs.h"
#include "MenuItemCallbacks.h"

using namespace KJS;

// ----------------------------------------------------------------------------
// JSMenuItemFunc::JSMenuItemFunc
// Default constructor
//
//
// ----------------------------------------------------------------------------
JSMenuItemFunc::JSMenuItemFunc(  
    ExecState *exec,  
    MJSMenuItemCallbacks* callbacks,
    int functionIndex )
    : JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()),
    m_functionId( abs( functionIndex ) ),
    m_callbacks( callbacks )    
    {
    }


// ----------------------------------------------------------------------------
// JSMenuItemFunc::implementsCall
// Whether or not the object implements the call() method
//
//
// ----------------------------------------------------------------------------
bool JSMenuItemFunc::implementsCall() const
    {
    return true;
    }


// ----------------------------------------------------------------------------
// JSMenuItemFunc::call
// Calls this object as if it is a function
//
//
// ----------------------------------------------------------------------------
JSValue* JSMenuItemFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
    {

    JSMenuItem *thisItem = static_cast<JSMenuItem*>(thisObj);

    switch ( m_functionId )
    {
        case JSMenuItem::Append:
        {
            if ( args.size() > 0 && !args[0]->isNull() ) {
                
                JSMenuItem* mitem = static_cast<JSMenuItem*>(args[0]->toObject(exec));
                
                if ( mitem->type() == ObjectType &&  mitem->inherits(&JSMenuItem::info) && 
                     thisItem->type() == ObjectType && thisItem->inherits(&JSMenuItem::info)) {                                        
                    
                    int itemId = mitem->InternalId();
                    int pitemId = thisItem->InternalId();
                    if (m_callbacks->addOptionsMenuItem(itemId, pitemId, thisItem->Show()) == KErrNone) {                        
                        mitem->SetShow( thisItem->Show() );
                        if ( mitem->callback() )
                            m_callbacks->setMenuItemObserver(itemId, mitem->callback());                        
                    }
                    
                }
            }
        }
        break;
        case JSMenuItem::Remove:
        {
            if ( args.size() > 0 && !(args[0]->isNull()) ) {
            
                JSMenuItem* mitem = static_cast<JSMenuItem*>(args[0]->toObject(exec));
                
                if (mitem->type() == ObjectType && 
                    mitem->inherits(&JSMenuItem::info) && 
                    thisItem->type() == ObjectType && 
                    thisItem->inherits(&JSMenuItem::info)) {
                        m_callbacks->deleteMenuItem(mitem->InternalId());
                }
            }
        }       
        break;
        case JSMenuItem::SetDim:
        {
            if ( args.size() > 0 && 
                args[0]->type() == BooleanType && 
                thisItem->type() == ObjectType && 
                thisItem->inherits(&JSMenuItem::info)) {
                thisItem->SetDimmed( args[0]->toBoolean( exec ) );
            }
            
        }
        break;
        case JSMenuItem::ToString:
        {
            thisItem->toString( exec );
        }
        break;
        default:
          break;
    }
    
    return jsUndefined();
}



