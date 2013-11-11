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
#include <eikcba.h>


#include "Menu.h"
#include "MenuItem.h"
#include "MenuFuncs.h"
#include "MenuCallbacks.h"

using namespace KJS;

// ----------------------------------------------------------------------------
// JSMenuFunc::JSMenuFunc
// Default constructor
//
//
// ----------------------------------------------------------------------------
JSMenuFunc::JSMenuFunc( ExecState* exec, MJSMenuCallbacks* callbacks, int functionIndex ) : 
    JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()),    
    m_callbacks( callbacks ),    
    m_functionId( abs( functionIndex ) )
{
}

JSMenuFunc::~JSMenuFunc()
{   
}

// ----------------------------------------------------------------------------
// JSMenuFunc::implementsCall
// Whether or not the object implements the call() method
//
//
// ----------------------------------------------------------------------------
bool JSMenuFunc::implementsCall() const
{
    return true;
}


// ----------------------------------------------------------------------------
// JSMenuFunc::call
// Calls this object as if it is a function
//
//
// ----------------------------------------------------------------------------
JSValue* JSMenuFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
{
    
    JSMenu *thisItem = static_cast<JSMenu*>(thisObj);

    switch ( m_functionId)
    {
        case JSMenu::Append:
        {
            if ( args.size() > 0 && !args[0]->isNull() ) {
                JSMenuItem* mitem = static_cast<JSMenuItem*>( args[0]->toObject(exec) );
                if ( mitem->type() == ObjectType && mitem->inherits(&JSMenuItem::info) ) {                    
                    mitem->AddOptionsMenuItem(true);
                }
            }
        }
        break;        
        case JSMenu::Remove:
        {
            if ( args.size() > 0 && !args[0]->isNull() ) {
                JSMenuItem* mitem = static_cast<JSMenuItem*>( args[0]->toObject(exec) );                
                if ( mitem->type() == ObjectType && mitem->inherits(&JSMenuItem::info) ) {
                    mitem->DeleteMenuItem();                                          
                }
            }
            
        }
        break;        
        case JSMenu::GetMenuItemById:
        {
            if ( args.size() > 0 && !args[0]->isNull() ) {
                
                TInt cmdId = args[0]->toInt32( exec );
                JSMenuItem* mi = static_cast<JSMenuItem*>(m_callbacks->getFromMenuItemCollection( cmdId ));

                if ( !mi )
                    return jsUndefined();
                
                return mi;
            }
        }
        break;            
        case JSMenu::GetMenuItemByName:
        {
            if ( args.size() > 0 && !args[0]->isNull() ) {
                UString str = args[0]->toString( exec );
                TBuf<CEikAutoMenuTitle::ENominalTextLength+1> textVal;
                textVal.Copy( (const unsigned short*)( str.data() ),( str.size() > CEikAutoMenuTitle::ENominalTextLength ) ? CEikAutoMenuTitle::ENominalTextLength : str.size() );

                JSMenuItem* mi = static_cast<JSMenuItem*>( m_callbacks->getFromMenuItemCollection( textVal ));
                 if ( !mi )
                    return jsUndefined();
                
                return mi;
            }
            
        }
        break;
        case JSMenu::SetRightSoftKeyLabel:
        {        
            if (args.size() > 1) {
                TInt labelSize(0);
                if (!args[0]->isNull() ) {
                    UString label = args[0]->toString( exec );
                    labelSize = (label.size() < KMaxCbaLabelLength) ? label.size() : KMaxCbaLabelLength;
                    m_callbacks->setRightSoftKeyLabel(TPtrC((const unsigned short*)(label.data()), labelSize) );
                }

                JSValue* val = (!args[1]->isNull() && labelSize) ? args[1] : 0;
                thisItem->setRightKeyCallback( exec, val );                                                       
            }
            
        }
        break;
        case JSMenu::SetLeftSoftKeyLabel:
        {        
            if (args.size() > 1) {
                TInt labelSize(0);
                if (!args[0]->isNull() ) {
                    UString label = args[0]->toString( exec );
                    labelSize = (label.size() < KMaxCbaLabelLength) ? label.size() : KMaxCbaLabelLength;
                    m_callbacks->setLeftSoftKeyLabel(TPtrC((const unsigned short*)(label.data()), labelSize) );
                }

                JSValue* val = (!args[1]->isNull() && labelSize) ? args[1] : 0;
                thisItem->setLeftKeyCallback( exec, val );                                                       
            }
            
        }
        break;
        case JSMenu::Clear:
        {
            m_callbacks->clearMenuItems();
        }
        break;
        case JSMenu::ToString:
        {
            thisItem->toString( exec );
        }
        break;
        case JSMenu::HideSoftkeys:
        {
            m_callbacks->hideSoftkeys( );
        }
        break;
        case JSMenu::ShowSoftkeys:
        {
            m_callbacks->showSoftkeys( );
        }
        break;
        default:
          break;
    }
            
    return jsUndefined();    
}



