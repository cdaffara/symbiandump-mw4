/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <config.h>
#include <lookup.h>
#include <PropertyNameArray.h>
#include "DeviceLiwMap.h"
#include "DeviceLiwBinding.h"
#include "array_instance.h"
#include <liwvariant.h>

const TInt KMaxKeySize = 255;

using namespace KJS;
const ClassInfo DeviceLiwMap::info = { "DeviceLiwMap", 0/*&ArrayInstance::info*/, 0, 0 };

// ============================= LOCAL FUNCTIONS ===============================
/*
@begin DeviceLiwMapTable 1
    close DeviceLiwMap::close DontDelete|Function 0
@end
*/

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// DeviceLiwMap::DeviceLiwMap
//
// ----------------------------------------------------------------------------
//
DeviceLiwMap::DeviceLiwMap( JSValue* proto, const CLiwMap* liwMap, CDeviceLiwBinding* liwBinding)
    : JSObject(proto)
    {
    m_valid = EFalse;
    m_privateData = NULL;
    if (liwMap && liwBinding )
        {
        m_privateData = new DeviceLiwMapPrivate(this, liwMap, liwBinding);
        if ( m_privateData )
            m_valid = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// DeviceLiwMap::~DeviceLiwMap
//
// ----------------------------------------------------------------------------
//
DeviceLiwMap::~DeviceLiwMap()
    {
    Close();
    }
    
// ----------------------------------------------------------------------------
// DeviceLiwMap::Close()
//
// ----------------------------------------------------------------------------
//
void DeviceLiwMap::Close()
    {
    if ( !m_valid )
        return;
    
    m_valid = EFalse;
    delete m_privateData;
    m_privateData = NULL;
    }
    
// ----------------------------------------------------------------------------
// Device::toString
//
//
// ----------------------------------------------------------------------------
//
UString DeviceLiwMap::toString( ExecState* /*exec*/ ) const
    {
    return "[object DeviceLiwMap]";
    }

// ----------------------------------------------------------------------------
// DeviceLiwMap::getOwnPropertySlot
//
//
// ----------------------------------------------------------------------------
bool DeviceLiwMap::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    // When the DeviceLiwMap is valid, the check order is
    //      DeviceLiwMap table => own property => prototype property
    // When the DeviceLiwMap is invalid (after close), the check order is
    //      close function in table => prototype property

    // 1. when it is valid    
    if(m_valid) 
        {
        // 1.1 check DeviceLiwMap table
        const HashEntry* entry = Lookup::findEntry(&DeviceLiwMapTable, propertyName);
        if (entry)
            {
            slot.setStaticEntry(this, entry, staticValueGetter<DeviceLiwMap>);
            return true;
            }

        // 1.2 check own property
        m_privateData->m_propName = propertyName;
        if (!getDirect(propertyName))
            {
            // 1.3 check prototypes
            JSObject *proto = static_cast<JSObject *>(this->prototype());

            while (!proto->isNull() && proto->isObject()) {
                if (proto->getOwnPropertySlot(exec, propertyName, slot))
                    return true;

                proto = static_cast<JSObject *>(proto->prototype());
            }

            if (!m_privateData->m_liwMap)
                return false;

            if ( propertyName.size() >= KMaxKeySize )
                throwError( exec, GeneralError, "key name too long" );

            // Store the property in the object so we get the same one each time.
            TLiwVariant detail;
            char prop_name8[KMaxKeySize];
            UChar * u_propname = (UChar *)propertyName.data();
            for (int i = 0; i < propertyName.size(); i++)
            {
                prop_name8[i] = u_propname[i].low();
            }
            prop_name8[propertyName.size()] = NULL;
            TBuf8<KMaxKeySize> prop_name((const unsigned char *)prop_name8);

            if (!m_privateData->m_liwMap->FindL(prop_name, detail))
                return false;
            JSValue* result = m_privateData->m_liwBinding->LiwVariant2JsVal(exec, detail);
            if ( result->type() == UndefinedType || exec->hadException() )
                return false;
            else
                this->putDirect( propertyName, result, DontDelete|ReadOnly );
            }
        return JSObject::getOwnPropertySlot(exec, propertyName, slot);
        }
    // when it is invalid
    else
        {
        // 2.1 check close() function
        if (propertyName == "close")
            {
            const HashEntry* entry = Lookup::findEntry(&DeviceLiwMapTable, propertyName);
            if (entry)
                {
                slot.setStaticEntry(this, entry, staticValueGetter<DeviceLiwMap>);
                return true;
                }
            }

        // 2.2 check prototypes
        JSObject *proto = static_cast<JSObject *>(this->prototype());
        while (!proto->isNull() && proto->isObject())
            {
            if (proto->getOwnPropertySlot(exec, propertyName, slot))
                return true;

            proto = static_cast<JSObject *>(proto->prototype());
            }
        }

    return false;
}


// ----------------------------------------------------------------------------
// DeviceLiwMap::getValueProperty
//
// ----------------------------------------------------------------------------
JSValue* DeviceLiwMap::getValueProperty(ExecState *exec, int token) const
    {
    switch (token)
            {
            case close:
                {
                return new DeviceLiwMapFunc(exec, token);;
                }
            default:
                if(m_valid)
                    return getDirect(m_privateData->m_propName);
                else
                    return jsUndefined();                
            }
    }
    
// ---------------------------------------------------------------------------
// DeviceLiwMapPrivate constructor
//
// ---------------------------------------------------------------------------
DeviceLiwMapPrivate::DeviceLiwMapPrivate(DeviceLiwMap* jsobj, const CLiwMap* liwMap, CDeviceLiwBinding* liwBinding)
    {
    TRAP_IGNORE(
            m_liwBinding = liwBinding;    
            m_liwMap = (CLiwMap*) liwMap;
            m_jsobj = jsobj;
            if ( m_liwMap )
                m_liwMap->IncRef();
            )
    }
    
// ---------------------------------------------------------------------------
// DeviceLiwMapPrivate::Close
//
// ---------------------------------------------------------------------------
DeviceLiwMapPrivate::~DeviceLiwMapPrivate()
    {
    // invalid the DeviceLiwMap
    if (m_jsobj)
        m_jsobj->m_valid = EFalse; 

    // release the map    
    if ( m_liwMap ) 
        {
        m_liwMap->DecRef();
        m_liwMap = NULL;
        }        
    }

// ----------------------------------------------------------------------------
// DeviceLiwMapFunc::DeviceLiwMapFunc
//
// ----------------------------------------------------------------------------
//
DeviceLiwMapFunc::DeviceLiwMapFunc( ExecState* exec, int token )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
    m_func( token )
    {
    }

// ----------------------------------------------------------------------------
// DeviceLiwMapFunc::call
//
// ----------------------------------------------------------------------------
//
JSValue* DeviceLiwMapFunc::callAsFunction(ExecState* exec, JSObject *thisObj, const List& aArgs )
    {
    if (!thisObj->inherits(&DeviceLiwMap::info))
        {
        return throwError(exec, GeneralError);
        }

    DeviceLiwMap *map = static_cast<DeviceLiwMap *>(thisObj);

    if ( m_func == DeviceLiwMap::close ) 
        {
        map->Close();
        }
    return jsUndefined();
    }

//END OF FILE




