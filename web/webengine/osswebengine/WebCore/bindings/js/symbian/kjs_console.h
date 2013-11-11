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
* Description: console Object that can perform loggings. 
*
*/

#ifndef __KJS_CONSOLE_H
#define __KJS_CONSOLE_H

//  INCLUDES
#include "config.h"
#include <e32base.h>
#include <kjs/object.h>
#include "PlatformString.h"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION
namespace WebCore {
    class Chrome;
    class String;
}

/**
*  ConsoleObject
*
*  @lib javascriptcore.dll
*  @since 3.2
*/
namespace KJS {
class UString;

struct ConsolePrivate
{
    int m_lineno;
    WebCore::String m_url;
    WebCore::String m_formattedMsg;
};

class ConsoleObject: public JSObject
    {
    friend class ConsoleFunc;
    public:
        ConsoleObject( ExecState *exec, WebCore::Chrome* chrome)
        : JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()), m_chrome(chrome) { }
	~ConsoleObject() {}

    //From ObjectImp
    public:
    JSValue* getValueProperty(KJS::ExecState*, int token) const;
    bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);

	const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
	virtual UString toString( ExecState *exec ) const;

	// enums of properties + functions of console obj
	enum {
		error,
		information,
		warn,
		log,
		debug,
		assert,
		lineno,
		sourceURL
		};

    private:
        WebCore::Chrome* m_chrome; //not owned
    };


class ConsoleFunc : public JSObject
    {
    public:
        ConsoleFunc(ExecState *exec, int aFuncType);
        virtual ~ConsoleFunc() {delete d;}

    //From JSObject
    public:
        bool implementsCall() const { return true;}
        JSValue* callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);

    private:
        ConsolePrivate* d;
        int funcType;
        void formatMsg(ExecState *exec,const List &args);
    };
}

#include "kjs_console.lut.h"
#endif
