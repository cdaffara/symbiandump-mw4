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
* Description:  
*
*/

// INCLUDE FILES

#include "kjs_console.h"
#include "kjs/lookup.h"
#include "Chrome.h"
#include "PlatformString.h"
#include "kjs/context.h"
#include "kjs/nodes.h"

//#include "flogger.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KFormattedMaxLength=0x200;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
using namespace KJS;
using namespace WebCore;
const ClassInfo ConsoleObject::info = { "ConsoleObject", 0, &ConsoleTable, 0 };

// ============================= LOCAL FUNCTIONS ===============================
/*
@begin ConsoleTable 7
error ConsoleObject::error DontDelete|Function 1
info ConsoleObject::information DontDelete|Function 1
warn ConsoleObject::warn DontDelete|Function 1
log ConsoleObject::log DontDelete|Function 1
debug ConsoleObject::debug DontDelete|Function 1
assert ConsoleObject::assert DontDelete|Function 1
lineno ConsoleObject::lineno DontDelete|ReadOnly
sourceURL ConsoleObject::sourceURL DontDelete|ReadOnly
@end
*/

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// ConsoleObject::toString
// Returns string representation of the object
//
//
// ----------------------------------------------------------------------------
UString ConsoleObject::toString(ExecState *exec) const
{
    return "[object ConsoleObject]";
}


// ----------------------------------------------------------------------------
// ConsoleObject::getValueProperty
//
//
// ----------------------------------------------------------------------------
JSValue* ConsoleObject::getValueProperty(ExecState *exec, int token) const
    {
    switch( token )
        {
        case error:
		case information:
		case warn:
		case log:
		case debug:
		case assert:
            {
            ConsoleFunc *cf = new ConsoleFunc( exec,token );
            return cf;
            }
        default:
            return throwError(exec, GeneralError);
        }
    return jsUndefined();
    }

// ----------------------------------------------------------------------------
// ConsoleObject::getOwnPropertySlot
//
//
//
// ----------------------------------------------------------------------------
bool ConsoleObject::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    const HashEntry* entry = Lookup::findEntry(&ConsoleTable, propertyName);
    if (entry)
        {
        slot.setStaticEntry(this, entry, staticValueGetter<ConsoleObject>);
        return true;
        }

    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}

// ----------------------------------------------------------------------------
// ConsoleFunc::formatMsg
// formats the arguments into a message according to the format string(first arg).
//
// ----------------------------------------------------------------------------
void ConsoleFunc::formatMsg(ExecState *exec, const List &args)
{
    d->m_formattedMsg = String();
    if (!args.size())
        return;

    bool validPattern = true;  // is this valid pattern, i.e. %s, %x, %f, %i, %d, etc?
    bool reachedMaxLen = false;
    String KTruncateSign("...");
    TInt lenReserved = 50;
    TInt64 maxInteger = 0x100000000;

    // retrieve the format string
    ListIterator it = args.begin();
    String formatStr((*it)->toString(exec));
    it++;
    bool endOfArgs = (it != args.end())? false: true;

    // format the value according to the converters:
    for (TInt index = formatStr.find('%', 0);
    (index != KErrNotFound) && !reachedMaxLen;
    index = formatStr.find('%',0))
    {
        // copy the buffer before next %
        if (index + d->m_formattedMsg.length() > KFormattedMaxLength - lenReserved)
        {
            d->m_formattedMsg.append(formatStr.left(KFormattedMaxLength - d->m_formattedMsg.length() - KTruncateSign.length()));
            d->m_formattedMsg.append(KTruncateSign);
            reachedMaxLen = true;
            break;
        }
        d->m_formattedMsg.append(formatStr.left(index));
        UChar patternChar = formatStr[index +1];

        JSValue* argValue = (*it);
        switch (patternChar.unicode())
        {
        case 's':
        case 'S':
            // retrieve and copy string
            if (!endOfArgs)
            {
                if (argValue->toString(exec).size() + d->m_formattedMsg.length() <= KFormattedMaxLength - lenReserved)
                    d->m_formattedMsg.append(argValue->toString(exec));
                else
                {
                    d->m_formattedMsg.append(String(argValue->toString(exec)).left(d->m_formattedMsg.length() - KTruncateSign.length()));
                    d->m_formattedMsg.append(KTruncateSign);
                    reachedMaxLen = true;
                }
            }
            break;
        case 'f':
        case 'F':
            // convert to float and
            if (!endOfArgs)
            {
               if (argValue == jsNaN() || !argValue->isNumber())
                    d->m_formattedMsg.append(String("[not valid float]"));
                else
                    d->m_formattedMsg.append(String::number(argValue->getNumber()));
            }
            break;
        case 'i':
        case 'd':
            if (!endOfArgs)
            {
				if (argValue == jsNaN() || !argValue->isNumber())
				   d->m_formattedMsg.append(String("[not valid integer]"));
				else
                	d->m_formattedMsg.append(String::number(static_cast<int32_t>(argValue->getNumber())));
            }
            break;
        case 'u':
        case 'x':
            if (!endOfArgs)
            {
                uint32_t uintValue;
                if(!argValue->getUInt32(uintValue))
                    d->m_formattedMsg.append(String("[not valid integer]"));
                else
                    if (patternChar == 'x')
                    {
						char hex[32];
						snprintf(hex, 32, "%x", uintValue);
                         d->m_formattedMsg.append(hex);
                    }
                    else
                        d->m_formattedMsg.append(String::number(uintValue));
            }
            break;
        case '%':
            d->m_formattedMsg.append('%');
            break;
        default:
            validPattern = false;
        }   // switch
        if (!endOfArgs) ++it;
        if (it == args.end())
            endOfArgs = true;

        int remainedLen;
        if (validPattern)
            remainedLen = formatStr.length() - index - 2;
        else
            remainedLen = formatStr.length() - index - 1;
        formatStr = formatStr.right(remainedLen);
    } // for

    if (!reachedMaxLen && formatStr.length() + d->m_formattedMsg.length() <= KFormattedMaxLength)
    {
        d->m_formattedMsg.append(formatStr);
    }
    else if (!reachedMaxLen)
    {
        d->m_formattedMsg.append(formatStr.left(KFormattedMaxLength - d->m_formattedMsg.length() - KTruncateSign.length()));
        d->m_formattedMsg.append(KTruncateSign);
    }
}

// ----------------------------------------------------------------------------
// ConsoleFunc::ConsoleFunc
// Default constructor
//
//
// ----------------------------------------------------------------------------
ConsoleFunc::ConsoleFunc(ExecState* exec, int fType)
    : JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()),
    funcType(fType)
    {
    d = new ConsolePrivate;
    d->m_url = String(exec->context()->currentBody()->sourceURL());
    d->m_lineno = exec->context()->lineno(); // get the execution line no.
    }


    // ----------------------------------------------------------------------------
    // ConsoleFunc::callAsFunction
    // Calls this object as if it is a function
    //
    //
    // ----------------------------------------------------------------------------

	JSValue* ConsoleFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
    {
        //RFileLogger::Write(_L("Browser"), _L("console_debug.txt"), EFileLoggingModeAppend,
        //    _L("callAsFunction called"));

        if (!thisObj->inherits(&ConsoleObject::info)) {
            return throwError(exec, GeneralError);
        }

        if (d->m_lineno == -1)
                d->m_lineno = exec->context()->currentBody()->lineNo();
        ConsoleObject *console = static_cast<ConsoleObject *>(thisObj);

        formatMsg(exec, args);
        switch (funcType)
        {
        case ConsoleObject::error:
        case ConsoleObject::information:
        case ConsoleObject::warn:
        case ConsoleObject::log:
            console->m_chrome->addMessageToConsole(JSMessageSource,
                (WebCore::MessageLevel)funcType, String(d->m_formattedMsg), d->m_lineno, String(d->m_url));
            break;
        case ConsoleObject::debug:
            {
                break;
            }
        case ConsoleObject::assert:
            {
                break;
            }
        default:
            break;
        }

        return jsUndefined();
    }
    //END OF FILE




