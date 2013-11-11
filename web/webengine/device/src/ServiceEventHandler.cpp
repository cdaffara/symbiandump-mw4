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
* Description:  Implemetation of ServiceEventHandler
*
*/

#include <config.h>
#include "ServiceEventHandler.h"

#include <object.h>
#include <protect.h>
#include <interpreter.h>
#include <list.h>
#include <e32base.h>

// ============================ MEMBER FUNCTIONS ===============================
using namespace KJS;


// ----------------------------------------------------------------------------
// ServiceEventHandler::ServiceEventHandler
//
//
//
// ----------------------------------------------------------------------------
//
ServiceEventHandler::ServiceEventHandler(
    ExecState* execState, const JSObject* aInterfaceFunc, const JSValue* aCallbackFunc, const TInt aTransId )
    : iGlobalExecState( execState )
    {
    iInterfaceFunc = (JSObject*)aInterfaceFunc;
    iCallbackFunc = (JSObject*)aCallbackFunc;
    iTransId = aTransId;
    }


// ----------------------------------------------------------------------------
// ServiceEventHandler::~ServiceEventHandler
//
//
//
// ----------------------------------------------------------------------------
//
ServiceEventHandler::~ServiceEventHandler()
    {
    }

// ----------------------------------------------------------------------------
// ServiceEventHandler::InvokeCall
// Caller supplies a KJS::List as the param
//
//
// ----------------------------------------------------------------------------
//
void ServiceEventHandler::InvokeCall( const List& aParam )
    {
    if ( iInterfaceFunc && iCallbackFunc )
        {
        JSLock::lock();

        if ( iGlobalExecState &&
             !iCallbackFunc->isNull() && iCallbackFunc->isObject()  )
            {
            JSObject* objFunc = static_cast<JSObject*>(iCallbackFunc);
            if (objFunc->implementsCall())
            {
            objFunc->call( iGlobalExecState, iInterfaceFunc, aParam ); // note that call() creates its own execution state for the func call
            }
            if ( iGlobalExecState->hadException() )
                {
                iGlobalExecState->clearException();
                }
            }

        JSLock::unlock();
        }
    }

// ----------------------------------------------------------------------------
// ServiceEventHandler::EventHandler
//
//
//
// ----------------------------------------------------------------------------
//
JSValue* ServiceEventHandler::CallbackFunc() const
    {
    return iCallbackFunc?iCallbackFunc:jsUndefined();
    }

//END OF FILE
