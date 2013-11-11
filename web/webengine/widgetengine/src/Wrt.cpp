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



// INCLUDE FILES

#include "Wrt.h"
#include "WrtPlatform.h"
#include <CUserAgent.h>
//#include "WrtFuncs.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT(KWrtVersion, "1.1");
_LIT(KNotAvailable, "N/A");

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
using namespace KJS;
const ClassInfo JSWrt::info = { "JSWrt", 0, &WrtTable, 0 };

// ============================= LOCAL FUNCTIONS ===============================
/*
@begin WrtTable 1
    version JSWrt::version DontDelete|ReadOnly
    platform JSWrt::platform DontDelete|ReadOnly
@end
*/

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// JSWrt::JSWrt
// C++ constructor
//
//
// ----------------------------------------------------------------------------
JSWrt::JSWrt( ExecState* exec )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
	wrtPriv(new WrtPrivate())
    {
    m_wrtVersion = NULL;
    TRAP_IGNORE(
            CUserAgent* usrAgnt = CUserAgent::NewL();
            // Get User Agent String
            if (usrAgnt)
                m_wrtVersion = usrAgnt->BrowserNameAndVersionL()->AllocL();
            delete usrAgnt;
    )
    }

// ----------------------------------------------------------------------------
// JSWrt::~JSWrt
// Destructor
//
//
// ----------------------------------------------------------------------------
JSWrt::~JSWrt()
    {
    delete wrtPriv;
    delete m_wrtVersion;
    }

// ----------------------------------------------------------------------------
// JSWrt::mark
// mark
//
//
// ----------------------------------------------------------------------------
void JSWrt::mark()
    {
    JSObject::mark();
    if(wrtPriv->m_platform)
       wrtPriv->m_platform->mark();
    }

// ----------------------------------------------------------------------------
// JSWrt::type
//
//
//
// ----------------------------------------------------------------------------
JSType JSWrt::type() const
{
    return ObjectType;
}

// ----------------------------------------------------------------------------
// JSWrt::toString
// Returns string representation of the object
//
//
// ----------------------------------------------------------------------------
UString JSWrt::toString(ExecState *exec) const
    {
    return "[object JSWrt]";
    }

// ----------------------------------------------------------------------------
// JSWrt::getValueProperty
//
//
// ----------------------------------------------------------------------------
JSValue* JSWrt::getValueProperty(ExecState *exec, int token) const
    {
    switch (token) {
        case version: {
            TPtrC wrtVersionPtr;
            if (m_wrtVersion)
                {
                wrtVersionPtr.Set( *m_wrtVersion );
                }
            else
                {
                wrtVersionPtr.Set( KNotAvailable );
                }
 
            return jsString( UString((UChar*) wrtVersionPtr.Ptr(), wrtVersionPtr.Length()) );
        }
        case platform: {
            return wrtPriv->m_platform ? wrtPriv->m_platform : wrtPriv->m_platform = new JSWrtPlatform(exec);
        }
        default:
            return throwError(exec, GeneralError);
        }
    }

// ----------------------------------------------------------------------------
// JSWrt::getOwnPropertySlot
//
//
//
// ----------------------------------------------------------------------------
bool JSWrt::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    const HashEntry* entry = Lookup::findEntry(&WrtTable, propertyName);
    if (entry)
        {
        slot.setStaticEntry(this, entry, staticValueGetter<JSWrt>);
        return true;
        }

    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}


//END OF FILE





