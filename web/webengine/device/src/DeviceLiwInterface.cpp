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
* Description:  Implementation of Service Interface Object
*
*/

#include <config.h>
#include <e32std.h>
#include <lookup.h>
#include <value.h>
#include <interpreter.h>
#include "liwservicehandler.h"
#include "DeviceLiwInterface.h"
#include "ServiceEventHandler.h"
#include "DeviceBinding.h"
#include "DeviceLiwResult.h"
#include "DeviceLiwPeer.h"

using namespace KJS;

const ClassInfo DeviceLiwInterface::info = { "DeviceLiwInterface", 0, 0, 0 };

// ============================= LOCAL FUNCTIONS ===============================
/*
@begin DeviceLiwInterfaceTable 1
    close DeviceLiwInterface::close DontDelete|Function 0
@end
*/

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// DeviceLiwInterface::DeviceLiwInterface
//
// ----------------------------------------------------------------------------
//
DeviceLiwInterface::DeviceLiwInterface(
    ExecState* exec,
    MDeviceBinding* deviceBinding,
    MDevicePeer* devicePeer )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() )
    {
    m_valid = EFalse;
    m_privateData = NULL;
    if ( deviceBinding && devicePeer)
        {
        m_privateData = new DeviceLiwInterfacePrivate(this, deviceBinding, devicePeer);
        if ( m_privateData )
            m_valid = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// DeviceLiwInterface::~DeviceLiwInterface
//
// ----------------------------------------------------------------------------
//
DeviceLiwInterface::~DeviceLiwInterface()
    {
    Close();
    }

// ----------------------------------------------------------------------------
// DeviceLiwInterface::Close
//
// ----------------------------------------------------------------------------
//
void DeviceLiwInterface::Close()
    {
    if(!m_valid)
        return;
    
    m_valid = EFalse;
    delete m_privateData;
    m_privateData = NULL;
    }

// ----------------------------------------------------------------------------
// DeviceLiwInterface::getOwnPropertySlot
//
// ----------------------------------------------------------------------------
//
bool DeviceLiwInterface::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& aSlot)
{    
    // When the DeviceLiwInterface is valid, the check order is
    //      own property => prototype property
    // When the DeviceLiwInterface is invalid (after close), the check order is
    //      close function in table => prototype property

    // 1 when it is valid
    if(m_valid)
        {
        m_privateData->m_interfaceName = propertyName;
        m_privateData->m_exec = exec;
            
        // 1.1 check own property
        if (!getDirect(propertyName))
            {
            // 1.2 check prototype
            JSObject *proto = static_cast<JSObject *>(this->prototype());
            while (!proto->isNull() && proto->isObject())
                {
                if (proto->getOwnPropertySlot(exec, propertyName, aSlot))
                    return true;

                proto = static_cast<JSObject *>(proto->prototype());
                }

            DeviceLiwInterfaceFunc *wf = new DeviceLiwInterfaceFunc( exec, propertyName );
            const_cast<DeviceLiwInterface*>(this)->putDirect(
                propertyName, wf, DontDelete|ReadOnly );
            }

        return JSObject::getOwnPropertySlot(exec, propertyName, aSlot);
        }
    // 2. when it is invalid
    else
        {
        // 2.1 check close() function
        if (propertyName == "close")
            {
            if(JSObject::getOwnPropertySlot(exec, propertyName, aSlot))
                return true;
            else
                {
                DeviceLiwInterfaceFunc *wf = new DeviceLiwInterfaceFunc( exec, propertyName );
                const_cast<DeviceLiwInterface*>(this)->putDirect(
                    propertyName, wf, DontDelete|ReadOnly );
                return JSObject::getOwnPropertySlot(exec, propertyName, aSlot);
                }        
            }

        // 2.2 check prototypes
        JSObject *proto = static_cast<JSObject *>(this->prototype());
        while (!proto->isNull() && proto->isObject())
            {
            if (proto->getOwnPropertySlot(exec, propertyName, aSlot))
                return true;

            proto = static_cast<JSObject *>(proto->prototype());
            }
        }
    return false;
}

// ----------------------------------------------------------------------------
// DeviceLiwInterface::SetObserver
//
// ----------------------------------------------------------------------------
//
void DeviceLiwInterface::SetObserver( ServiceEventHandler* aObserver )
    {
    if (m_valid && m_privateData->m_devicePeer )
        {
        m_privateData->m_devicePeer->InstallCallback( aObserver );
        }
    }


// ----------------------------------------------------------------------------
// DeviceLiwInterface::InvokeOp
//
// ----------------------------------------------------------------------------
//
JSValue* DeviceLiwInterface::InvokeOp(
    ExecState* exec,
    const Identifier& propertyName,
    const List& aArgs )
    {
    if (m_valid && m_privateData->m_deviceBinding && m_privateData->m_devicePeer )
        {
        return ( m_privateData->m_deviceBinding->InvokeOp(
            exec, propertyName, aArgs, m_privateData->m_devicePeer ) );
        }
    else
        {
        return jsUndefined();
        }
    }

// ----------------------------------------------------------------------------
// DeviceLiwInterface::IsRunningCallBack()
//
// ----------------------------------------------------------------------------
//
TBool DeviceLiwInterface::IsRunningCallBack() const
    {
    if(!isValid())
        return EFalse;
    
    DeviceLiwPeer* devicePeer = static_cast<DeviceLiwPeer*> (m_privateData->m_devicePeer);
    
    if(devicePeer && devicePeer->IsRunningCallBack())
        return ETrue;
    else
        return EFalse;
    }

// ---------------------------------------------------------------------------
// DeviceLiwInterfacePrivate constructor
//
// ---------------------------------------------------------------------------
DeviceLiwInterfacePrivate::DeviceLiwInterfacePrivate(DeviceLiwInterface* jsobj, MDeviceBinding* deviceBinding, MDevicePeer* devicePeer)
    {
    TRAP_IGNORE(
        m_deviceBinding = deviceBinding;    
        m_devicePeer = devicePeer;
        m_exec = NULL;
        m_jsobj = jsobj;
        )
    }

// ---------------------------------------------------------------------------
// DevicePrivate destructor
//
// ---------------------------------------------------------------------------
DeviceLiwInterfacePrivate:: ~DeviceLiwInterfacePrivate()
    {
    // invalid the DeviceLiwInterface
    if (m_jsobj)
        m_jsobj->m_valid = EFalse; 
       
    delete m_devicePeer;
    m_devicePeer = NULL;
    }

// ----------------------------------------------------------------------------
// DeviceLiwInterfaceFunc::DeviceLiwInterfaceFunc
//
// ----------------------------------------------------------------------------
//
DeviceLiwInterfaceFunc::DeviceLiwInterfaceFunc(
    ExecState* exec,
    const Identifier& propertyName )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
    m_funcName( propertyName )
    {
    }

// ----------------------------------------------------------------------------
// DeviceLiwInterfaceFunc::~DeviceLiwInterfaceFunc
//
// ----------------------------------------------------------------------------
//
DeviceLiwInterfaceFunc::~DeviceLiwInterfaceFunc()
    {
    }

// ----------------------------------------------------------------------------
// DeviceLiwInterfaceFunc::implementsCall
//
// ----------------------------------------------------------------------------
//
bool DeviceLiwInterfaceFunc::implementsCall() const
    {
    return true;
    }

// ----------------------------------------------------------------------------
// DeviceLiwInterfaceFunc::callAsFunction
//
// ----------------------------------------------------------------------------
//
JSValue* DeviceLiwInterfaceFunc::callAsFunction(ExecState *exec, JSObject *aThisObj, const List &aArgs)
    {
    JSValue* ret = jsUndefined();

    if ( !aThisObj->inherits( &KJS::DeviceLiwInterface::info ) )
        {
        return throwError(exec, GeneralError);
        }

    DeviceLiwInterface* sapiif = static_cast<DeviceLiwInterface*>(aThisObj);

    if(!sapiif->isValid())
        return ret;

    if (m_funcName == "close")
        {
        // the close function cann't be called in the callback function
        if(sapiif->IsRunningCallBack())
            {
            return throwError(exec, GeneralError, "Can not close interface object in callback function.");
            }
        sapiif->Close();
        return ret;
        }

    ret = sapiif->InvokeOp( exec, m_funcName, aArgs );
    
    if(ret->isObject() && (static_cast<JSObject*> (ret))->inherits( &KJS::DeviceLiwResult::info ))
    {
        // insert into jsobject array
        DevicePrivateBase* ifData = sapiif->getInterfaceData();
        DevicePrivateBase* retData =(static_cast<DeviceLiwResult*> (ret))->getResultData();
        retData->SetParent( ifData ); 
        ifData->AddChild( retData );
        if ( aArgs.size() > 1 )
        {
            bool ok;
            TInt32 err; // check error code before extract transaction id
            err = ret->getObject()->get(exec, Identifier("ErrorCode"))->toInt32( exec, ok );
            if (!ok)
                return throwError( exec, GeneralError, "error occurred during asynchronous call." );
            if ( err == KErrNone )
            {
                TInt32 tranId;
                tranId = ret->getObject()->get( exec, Identifier( "TransactionID" ))->toInt32( exec, ok );
                if ( !ok )
                    return throwError( exec, GeneralError, "invalid transaction id" );
                sapiif->SetObserver( new ServiceEventHandler(
                                        exec->lexicalInterpreter()->globalExec(),
                                        this->toObject( exec ), aArgs[1], tranId ) );
            }
        }
    }

    return ret;
    }


//END OF FILE

