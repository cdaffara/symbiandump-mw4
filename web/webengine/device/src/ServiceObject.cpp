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
* Description:  Implementation of Device Service Object
*
*/


#include <config.h>
#include "ServiceObject.h"
#include "Device.h"
#include "DeviceBinding.h"
#include "DeviceLiwInterface.h"
#include "DeviceLiwResult.h"
#include "DeviceLiwPeer.h"
#include <PropertyNameArray.h>

using namespace KJS;
const ClassInfo ServiceObject::info = { "ServiceObject", 0, 0, 0 };
_LIT8( KWildChar, "*" );
// ============================ MEMBER FUNCTIONS ===============================

/*
@begin ServiceObjectTable 1
    close ServiceObject::close DontDelete|Function 1
@end
*/
// ----------------------------------------------------------------------------
// ServiceObject::ServiceObject
//
//
//
// ----------------------------------------------------------------------------
ServiceObject::ServiceObject(
    ExecState* exec,
    HBufC8* svcName,
    MDeviceBinding* deviceBinding)
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() )
    {
    m_valid = EFalse;
    m_privateData = NULL;
    if ( deviceBinding )
        {
        m_privateData = new ServiceObjectPrivate(this, svcName, deviceBinding);
        if ( m_privateData )
            m_valid = ETrue;
        }
    }


// ----------------------------------------------------------------------------
// ServiceObject::~ServiceObject
//
//
//
// ----------------------------------------------------------------------------
ServiceObject::~ServiceObject()
    {
    Close();
    }

// ----------------------------------------------------------------------------
// ServiceObject::Close
//
// ----------------------------------------------------------------------------
void ServiceObject::Close()
    {
    // avoid double close    
    if ( !m_valid ) 
        return;

    m_valid = EFalse; 
    delete m_privateData;
    m_privateData = NULL;   
   }

// ----------------------------------------------------------------------------
// ServiceObject::toString
//
//
//
// ----------------------------------------------------------------------------
UString ServiceObject::toString( ExecState* exec ) const
    {
    return "[object ServiceObject]";
    }


// ----------------------------------------------------------------------------
// ServiceObject::getOwnPropertySlot
//
//
//
// ----------------------------------------------------------------------------
bool ServiceObject::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    // When the ServiceObject is valid, the check order is
    //      ServiceObject table => own property => prototype property
    // When the ServiceObject is invalid (after close), the check order is
    //      close function in table => prototype property

    // 1. when it is valid
    if(m_valid)
        {
        // 1.1 check ServiceObject table
        const HashEntry* entry = Lookup::findEntry(&ServiceObjectTable, propertyName);
        if (entry)
            {
            slot.setStaticEntry(this, entry, staticValueGetter<ServiceObject>);
            return true;
            }

        // 1.2 check own property
        m_privateData->m_propName = propertyName;
        JSValue* val = getDirect( propertyName );

        // if the property is an interface which has been closed
        bool need_recreate = false;
        if ( val && val->isObject() &&
             val->toObject(exec)->inherits( &KJS::DeviceLiwInterface::info ) )
            {
            DeviceLiwInterface* interface = static_cast<DeviceLiwInterface*>(val);
            if ( !interface->isValid() )
                {
                need_recreate = true;
                }
            }

        if ( !val || need_recreate )
            {
            if ( !val )
                {
                // 1.3 check prototypes
                JSObject *proto = static_cast<JSObject *>(this->prototype());

                while (!proto->isNull() && proto->isObject()) 
                    {
                    if (proto->getOwnPropertySlot(exec, propertyName, slot))
                        return true;

                    proto = static_cast<JSObject *>(proto->prototype());
                    }
                }
            
            // Create an interface for me, please!
            // Store the interface in the object so we get the same one each time.
            JSValue* resultVal = m_privateData->m_deviceBinding->CreateInterface(
                exec, m_privateData->m_svcName, m_privateData->m_propName );

            if ( resultVal->type() == UndefinedType || exec->hadException() )
                return false;
            else
                {
                JSValue* s = resultVal->toObject(exec)->get( exec, m_privateData->m_propName );
                DeviceLiwInterface* ifObj = static_cast<DeviceLiwInterface*>(s);
                DevicePrivateBase* ifData = ifObj->getInterfaceData();
                DevicePrivateBase* soData = this->getServiceData();
                ifData->SetParent( soData ); 
                soData->AddChild( ifData );
                this->putDirect( propertyName, s, DontDelete|ReadOnly );
                }

            // clean the DeviceLiwResult which is useless
            if(resultVal->isObject()) 
                {
                JSObject * jsobj = resultVal->toObject( exec );
                if(jsobj->inherits( &KJS::DeviceLiwResult::info ))
                    {
                    DeviceLiwResult* result = static_cast<DeviceLiwResult*>(jsobj);
                    result->Close();
                    }
                }
            }
        return JSObject::getOwnPropertySlot(exec, propertyName, slot);
        }
    // 2. when it is invalid
    else
        {
        // 2.1 check close function
        if (propertyName == "close")
            {
            const HashEntry* entry = Lookup::findEntry(&ServiceObjectTable, propertyName);
            if (entry)
                {
                slot.setStaticEntry(this, entry, staticValueGetter<ServiceObject>);
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
// ServiceObject::get
//
//
//
// ----------------------------------------------------------------------------
JSValue* ServiceObject::getValueProperty(KJS::ExecState* exec, int token) const
    {
    if (token == close)
        {
            return new ServiceObjectFunc( exec, token );
        }
    if(m_valid)
        return getDirect(m_privateData->m_propName);
    else
        return jsUndefined();
    }

// ----------------------------------------------------------------------------
// ServiceObject::IsRunningCallBack
//
//
//
// ----------------------------------------------------------------------------
TBool ServiceObject::IsRunningCallBack(ExecState *exec)
    {    
    PropertyNameArray propertyNames;
    this->getPropertyNames( exec, propertyNames );
    unsigned size = static_cast<unsigned>(propertyNames.size());

    for (unsigned i = 0; i < size; i++)
        {
        JSValue * jsvalue = this->get( exec, propertyNames[i] );
        if(jsvalue->isObject()) 
            {          
            JSObject * prop = jsvalue->toObject( exec );
            if (prop->inherits( &DeviceLiwInterface::info ))
                {
                if((static_cast<DeviceLiwInterface*>(prop))->IsRunningCallBack())
                    {
                    return ETrue;
                    }
                }
            }
        }  
    return EFalse;      
    }

ServiceObjectFunc::ServiceObjectFunc( ExecState* exec, int token )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
    m_func( token )
    {
    }

JSValue* ServiceObjectFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
    {
    JSValue* ret = jsUndefined();
    if (!thisObj->inherits( &KJS::ServiceObject::info ) )
        {
        return throwError(exec, GeneralError);
        }
    ServiceObject* so = static_cast<ServiceObject*>(thisObj);

    if(!so->isValid())
        return ret;
    
    if ( m_func == ServiceObject::close )
        {
        // the close function cann't be called in the callback function
        if(so->IsRunningCallBack(exec))
            {
            return throwError(exec, GeneralError, "Can not close service object in callback function.");
            }
        so->Close();
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// DeviceLiwMapPrivate constructor
//
// ---------------------------------------------------------------------------
ServiceObjectPrivate::ServiceObjectPrivate(ServiceObject* jsobj, HBufC8* svcName, MDeviceBinding* deviceBinding )
    {
    m_svcName = svcName;    
    m_deviceBinding = deviceBinding;
    m_jsobj = jsobj;
    }
    
// ---------------------------------------------------------------------------
// DeviceLiwMapPrivate::destructor
//
// ---------------------------------------------------------------------------
ServiceObjectPrivate::~ServiceObjectPrivate()
    {
    // invalid the ServiceObject
    if (m_jsobj)
        m_jsobj->m_valid = EFalse;
        
    m_deviceBinding->UnloadServiceProvider(KWildChar(), m_svcName->Des());
    m_deviceBinding = NULL;
        
    delete m_svcName;
    m_svcName = NULL;
    }

//END OF FILE
