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



#ifndef __MENUITEMFUNCS_H
#define __MENUITEMFUNCS_H

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

/**
*  CMenuItem
*
*  @lib widgetengine.dll
*  @since 3.1
*/
namespace KJS {

class WidgetEventHandler;

class JSMenuItemFunc : public JSObject
    {
    public:    
        JSMenuItemFunc(ExecState *exec, MJSMenuItemCallbacks* callbacks, int functionIndex );

    //From ObjectImp
    public:
        bool implementsCall() const;
        JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);        

        enum {
            Append,
            Remove,           
            Disable,
            Enable,
            ToString
            };

    private:
        int m_functionId;
        MJSMenuItemCallbacks* m_callbacks;  //not owned
    };

};


#endif