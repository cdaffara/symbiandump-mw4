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



#ifndef __MENUFUNCS_H
#define __MENUFUNCS_H

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

/**
*  CMenu
*
*  @lib widgetengine.dll
*  @since 3.1
*/
namespace KJS {

class WidgetEventHandler;

class JSMenuFunc : public JSObject
{
    public:
        JSMenuFunc(ExecState* exec, MJSMenuCallbacks* callbacks, int aFunctionIndex);
        ~JSMenuFunc();
        
    //From JSObject
    public:
        bool implementsCall() const;
        JSValue* callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);
        
        enum {
            Append,
            Remove,
            GetMenuItemById,
            GetMenuItemByName,
            SetRightSoftKeyLabel,
            ToString,
            HideSoftkeys,
            ShowSoftkeys,
            Location,
            SetLeftSoftKeyLabel
            };

    private:
        MJSMenuCallbacks*        m_callbacks;  //not owned
        int                      m_functionId;
};


};

#endif
