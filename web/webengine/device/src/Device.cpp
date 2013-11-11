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
#include "config.h"
#include "Device.h"
#include "ServiceObject.h"
#include "ServiceEventHandler.h"
#include "DeviceBridge.h"
#include "DeviceBinding.h"
#include "DeviceLiwBinding.h"
#include <liwservicehandler.h>

using namespace KJS;

const ClassInfo Device::info = { "Device", 0, 0, 0 };
const TInt INIT_ARRAY_SIZE = 10;   // initial service object array

// ============================= LOCAL FUNCTIONS ===============================
/*
@begin DeviceTable 1
    getServiceObject Device::getServiceObject DontDelete|Function 2
    listProviders Device::listProviders DontDelete|Function 2
@end
*/

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// Device::Device
//
// ----------------------------------------------------------------------------
//
Device::Device( ExecState* exec )
    : JSObject()
    {
    m_valid = EFalse;
    TRAP_IGNORE(
        m_deviceBinding = CDeviceLiwBinding::NewL();
    )    
    if (m_deviceBinding)
        {
        m_privateData = new DevicePrivate(this);
        if (m_privateData)
            m_valid = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// Device::SetUid
//
// ----------------------------------------------------------------------------
//
void Device::SetUid( const TUint& aValue)
    {
    if(m_deviceBinding)
        m_deviceBinding->SetUid( aValue);
    }

// ----------------------------------------------------------------------------
// Device::Close
//
// ----------------------------------------------------------------------------
//
void Device::Close()
    {
    if ( !m_valid )
        return;
    
    m_valid = EFalse;
    delete m_privateData;
    m_privateData = NULL;
    delete m_deviceBinding;
    m_deviceBinding = NULL;
    }


// ----------------------------------------------------------------------------
// Device::~Device
//
// ----------------------------------------------------------------------------
//
Device::~Device()
    {
    Close();
    }

// ----------------------------------------------------------------------------
// Device::toString
//
// ----------------------------------------------------------------------------
//
UString Device::toString( ExecState* /*exec*/ ) const
    {
    return "[object Device]";
    }

// ----------------------------------------------------------------------------
// Device::getOwnPropertySlot
//
//
// ----------------------------------------------------------------------------
bool Device::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    if ( !m_valid )
        return false;

    m_privateData->m_exec = exec;
    m_privateData->m_propName = propertyName;
    const HashEntry* entry = Lookup::findEntry(&DeviceTable, propertyName);
    if (entry)
        {
        slot.setStaticEntry(this, entry, staticValueGetter<Device>);
        return true;
        }

    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}


// ----------------------------------------------------------------------------
// JSVersion::getValueProperty
//
//
// ----------------------------------------------------------------------------
JSValue* Device::getValueProperty(ExecState *exec, int token) const
    {
    if ( !m_valid )
        return jsUndefined();
        
    switch( token )
        {
        case getServiceObject:
        case listProviders:
                return new DeviceFunc( exec, m_deviceBinding, token );

        default:
            return throwError(exec, GeneralError);
        }
    }

// ---------------------------------------------------------------------------
// DevicePrivateBase constructor
//
// ---------------------------------------------------------------------------
DevicePrivateBase::DevicePrivateBase()
    {
    m_parent = NULL;
    m_isDeleting = EFalse;
    TRAP_IGNORE(
        m_children = new RPointerArray<DevicePrivateBase>( INIT_ARRAY_SIZE );)
    }

// ---------------------------------------------------------------------------
// DevicePrivateBase destructor
//
// ---------------------------------------------------------------------------
DevicePrivateBase::~DevicePrivateBase()
    {
    m_isDeleting = ETrue;
    // 1. remove self from the parent
    if ( m_parent )  
        {
        m_parent->RemoveChild(this);
        }

    // 2. delete all the children
    for ( int i = 0; i < m_children->Count(); i++ )
        {
        delete (*m_children)[i];
        }

    // 3. delete array
    m_children->Close();
    delete m_children;
    }

// ---------------------------------------------------------------------------
// DevicePrivateBase setParent
//
// ---------------------------------------------------------------------------
void DevicePrivateBase::SetParent( DevicePrivateBase* aValue )
    {
    m_parent = aValue;
    }

// ---------------------------------------------------------------------------
// DevicePrivateBase add child into list
//
// ---------------------------------------------------------------------------
void DevicePrivateBase::AddChild( DevicePrivateBase* aValue )
    {
    m_children->Append( aValue );
    }

// ---------------------------------------------------------------------------
// DevicePrivateBase add child into list
//
// ---------------------------------------------------------------------------
void DevicePrivateBase::RemoveChild( DevicePrivateBase* aValue )
    {
    if ( m_isDeleting )
        return;
    
    TInt index = m_children->Find( aValue );
    if ( index != KErrNotFound )
        m_children->Remove( index );
    }

// ---------------------------------------------------------------------------
// DevicePrivate constructor
//
// ---------------------------------------------------------------------------
DevicePrivate::DevicePrivate( Device* jsobj )
    {
    m_jsobj = jsobj;
    m_exec = NULL;
    }

// ---------------------------------------------------------------------------
// DevicePrivate destructor
//
// ---------------------------------------------------------------------------
DevicePrivate::~DevicePrivate()
    {
    // invalid the Device
    if (m_jsobj)
        m_jsobj->m_valid = EFalse;
    }

// ----------------------------------------------------------------------------
// DeviceFunc::DeviceFunc
//
//
// ----------------------------------------------------------------------------
//
DeviceFunc::DeviceFunc( ExecState* exec, MDeviceBinding* deviceBinding,
    int token )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
    m_deviceBinding( deviceBinding ),
    m_func( token )
    {
    }

// ----------------------------------------------------------------------------
// DeviceFunc::callAsFunction
//
// ----------------------------------------------------------------------------
//
JSValue* DeviceFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
    {
    TInt err = KErrNone;
    JSValue* ret = jsUndefined();

    if ( !m_deviceBinding )
        {
        return ret;
        }

    if ( m_func == Device::getServiceObject )
        {
        err = m_deviceBinding->LoadServiceProvider( exec, args );
        if ( err != KErrNone )
            {
            char * err_msg = "general error ";
            // process  TLiwLoadStatus errors
            switch ( err )
                {
                case KLiwUnknown: // -5
                        err_msg = "unknown error";
                        break;
                case KLiwMetaDataInvalidFormat: //-4
                    err_msg = "meta data invalid format";
                        break;
                case KLiwInvalidVersionSpecification: //-3
                    err_msg = "invalid version specification";
                        break;
                case KLiwVersionOutOfRange: //-2
                    err_msg = "version out of range";
                        break;
                case KLiwSecurityAccessCheckFailed: //-1
                    err_msg = "security access check failed";
                        break;
                }
            throwError(exec, GeneralError, err_msg);
            }
        else
            {
            int argcount = args.size();
            if ( argcount == 0 || args[0]->type() != StringType || args[0]->toString( exec ).size() == 0 )
                return ret;
            // Get service name
            HBufC8* svcName = KJS::GetAsciiBufferL( args[0]->toString( exec ) );

            ServiceObject *so = new ServiceObject( exec, svcName, m_deviceBinding );
            if ( so != NULL )
                {
                DevicePrivateBase* devData = (static_cast<Device*>(thisObj))->getDeviceData();
                DevicePrivateBase* soData = so->getServiceData();
                soData->SetParent( devData ); 
                devData->AddChild( soData );
                ret = so;
                }
            }
        }
    else if ( m_func == Device::listProviders )
        {
        ret = m_deviceBinding->ListProviders( exec, args );
        }

    return ret;
    }

MDeviceBinding* Device::GetDeviceBinding()
{
    return m_deviceBinding;
}

//END OF FILE



