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
* Description:  Implementation of LIW Iterable wrapper
*
*/


// INCLUDE FILES
#include <config.h>
#include <lookup.h>
#include <liwservicehandler.h>
#include "DeviceLiwIterable.h"
#include "DeviceLiwBinding.h"

const TInt KMaxKeySize = 128;

using namespace KJS;

const ClassInfo DeviceLiwIterable::info = { "DeviceLiwIterable", 0, 0, 0 };
// ============================= LOCAL FUNCTIONS ===============================
/*
@begin DeviceLiwIterableTable 1
    getNext DeviceLiwIterable::getNext DontDelete|Function 0
    reset   DeviceLiwIterable::reset   DontDelete|Function 0
    close   DeviceLiwIterable::close   DontDelete|Function 0
@end
*/

/*
 * The DeviceLiwIterable is a plain wrapper around the underlying LIW iterable variant
 * object. It acts as a pass through for the iterable methods to JS. This
 * is needed for efficiently processing returned iterables (instead of copying the
 * contents into a JS array). Of course much of the benefit depends on how well iterables
 * are implemented in the underlying LIW framework.
 */

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// DeviceLiwIterable::DeviceLiwIterable
//
// ----------------------------------------------------------------------------
//
DeviceLiwIterable::DeviceLiwIterable(
    ExecState* exec,
    CDeviceLiwBinding* binding,
    CLiwIterable* variant)
    : JSObject(exec->lexicalInterpreter()->builtinObjectPrototype() )
    {
    m_valid = EFalse;
    m_privateData = NULL;
    if ( binding && variant )
        {
        m_privateData = new DeviceLiwIterablePrivate(this, variant, binding);
        if ( m_privateData )
            m_valid = ETrue;
        }            
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterable::~DeviceLiwIterable
//
// ----------------------------------------------------------------------------
//
DeviceLiwIterable::~DeviceLiwIterable()
    {
    Close();
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterable::Close()
//
// ----------------------------------------------------------------------------
//
void DeviceLiwIterable::Close()
    {
    if ( !m_valid )
        return;
        
    m_valid = EFalse;
    delete m_privateData;
    m_privateData = NULL;
    }     

// ----------------------------------------------------------------------------
// DeviceLiwIterable::toString
// Returns string representation of the object
//
// ----------------------------------------------------------------------------
UString DeviceLiwIterable::toString(ExecState* /*exec*/) const
    {
    return "[object DeviceLiwIterable]";
    }

// ----------------------------------------------------------------------------
// Device::getOwnPropertySlot
//
// ----------------------------------------------------------------------------

bool DeviceLiwIterable::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
    {
    // When the DeviceLiwIterable is valid, the check order is
    //      DeviceLiwIterable table => own property => prototype property
    // When the DeviceLiwIterable is invalid (after close), the check order is
    //      close function in table => prototype property

    // 1. when it is valid
    if(m_valid) 
        {
        // 1.1 check DeviceLiwIterable table    
        const HashEntry* entry = Lookup::findEntry(&DeviceLiwIterableTable, propertyName);
        if (entry)
            {
            slot.setStaticEntry(this, entry, staticValueGetter<DeviceLiwIterable>);
            return true;
            }

        // 1.2 check own property
        if(JSObject::getOwnPropertySlot(exec, propertyName, slot))
            return true;
        
        // 1.3 check prototypes
        JSObject *proto = static_cast<JSObject *>(this->prototype());
        while (!proto->isNull() && proto->isObject())
            {
            if (proto->getOwnPropertySlot(exec, propertyName, slot))
                return true;

            proto = static_cast<JSObject *>(proto->prototype());
            }
        }
    // 2. when it is invalid
    else
        {
        // 2.1 check close() function
        if (propertyName == "close")
            {
            const HashEntry* entry = Lookup::findEntry(&DeviceLiwIterableTable, propertyName);
            if (entry)
                {
                slot.setStaticEntry(this, entry, staticValueGetter<DeviceLiwIterable>);
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
// JSVersion::getValueProperty
//
// ----------------------------------------------------------------------------
JSValue* DeviceLiwIterable::getValueProperty(ExecState *exec, int token) const
    {
    DeviceLiwIterableFunc *lf;
    switch (token)
            {
            // Unfortunately, the only operations that LIW Iterables support are next() and reset(). e.g. there
            // are no hasNext() or count() operations typical for iterators.
            case getNext:
            case reset:
            case close:
                {
                lf = new DeviceLiwIterableFunc(exec, token);
                break;
                }

            default:
                return throwError(exec, GeneralError);
            }
            return lf;
    }
    
// ---------------------------------------------------------------------------
// DeviceLiwIterablePrivate constructor
//
// ---------------------------------------------------------------------------
DeviceLiwIterablePrivate::DeviceLiwIterablePrivate(DeviceLiwIterable* jsobj, const CLiwIterable* liwIterable, CDeviceLiwBinding* liwBinding)
    {
    TRAP_IGNORE(
            m_liwBinding = liwBinding;
            m_exec = NULL;    
            m_iterable = (CLiwIterable*) liwIterable;
            m_jsobj = jsobj;
            if ( m_iterable )
                m_iterable->IncRef();
            )
    }
    
// ---------------------------------------------------------------------------
// DeviceLiwMapPrivate::Close
//
// ---------------------------------------------------------------------------
DeviceLiwIterablePrivate::~DeviceLiwIterablePrivate()
    {
    // invalid the DeviceLiwIterable
    if (m_jsobj)
        m_jsobj->m_valid = EFalse;
    
    // release the map    
    if ( m_iterable ) 
        {
        m_iterable->DecRef();
        m_iterable = NULL;
        }        
    }
    
// ----------------------------------------------------------------------------
// DeviceLiwIterablePrivate::DeviceLiwIterableFunc
//
// ----------------------------------------------------------------------------
//
DeviceLiwIterableFunc::DeviceLiwIterableFunc( ExecState* exec, int token )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
    m_func( token )
    {
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterableFunc::~DeviceLiwIterableFunc
//
// ----------------------------------------------------------------------------
//
DeviceLiwIterableFunc::~DeviceLiwIterableFunc()
    {
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterableFunc::implementsCall
//
// ----------------------------------------------------------------------------
//
bool DeviceLiwIterableFunc::implementsCall() const
    {
    return true;
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterableFunc::callAsFunction
//
// ----------------------------------------------------------------------------
//
JSValue* DeviceLiwIterableFunc::callAsFunction(ExecState* exec, JSObject *thisObj, const List& aArgs )
    {
        JSValue* rval = jsUndefined();
        TBool found = EFalse;

        if (!thisObj->inherits(&DeviceLiwIterable::info)) {
                    return throwError(exec, GeneralError);
        }

        DeviceLiwIterable *it = static_cast<DeviceLiwIterable *>(thisObj);
        
        if ( !it->isValid() || !it->m_privateData->m_liwBinding || !it->m_privateData->m_iterable) {
            return rval;
        }

        if ( m_func == DeviceLiwIterable::getNext ) {
            TLiwVariant vv;
            TRAP_IGNORE( found = it->m_privateData->m_iterable->NextL( vv ) );
            if( found ) 
                {
                rval =  it->m_privateData->m_liwBinding->LiwVariant2JsVal( exec, vv );
                if(rval->isObject()) 
                    {
                    JSObject* obj =  static_cast<JSObject*> (rval);
                    DevicePrivateBase* thisData = it->getIterableData();
                    DevicePrivateBase* childData = NULL;
                    if ( obj->inherits( &KJS::DeviceLiwIterable::info ) )
                        childData = (static_cast<DeviceLiwIterable*> (obj))->getIterableData();
                    else if ( obj->inherits( &KJS::DeviceLiwMap::info ) )
                        childData = (static_cast<DeviceLiwMap*> (obj))->getMapData();   

                    if ( childData ) 
                        {
                        childData->SetParent( thisData ); 
                        thisData->AddChild( childData );
                        }
                    }
                vv.Reset();
                }
        }
        else if ( m_func == DeviceLiwIterable::reset ){
            it->m_privateData->m_iterable->Reset();
        }
        else if ( m_func == DeviceLiwIterable::close ){
            it->Close();
        }
        return rval;
    }
//END OF FILE



