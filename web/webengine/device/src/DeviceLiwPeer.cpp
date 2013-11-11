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
* Description:  Implementation of LIW Device Peer
*
*/


#include <config.h>
#include "DeviceLiwPeer.h"
#include "DeviceLiwBinding.h"
#include "ServiceEventHandler.h"

using namespace KJS;

const TInt INIT_CALLBACK_ARRAY_SIZE = 10;
   
// ============================ MEMBER FUNCTIONS ===============================


// ============================================================================
// DeviceLiwPeer::DeviceLiwPeer
// C++ constructor
//
// @since 5.0
// ============================================================================
//
DeviceLiwPeer::DeviceLiwPeer(
    ExecState* exec,
    CDeviceLiwBinding* liwbinding,
    MLiwInterface* liwinterface )
    : m_globalExecState( exec ),
    m_binding( liwbinding ),
    m_interface( liwinterface ),
    m_serviceName( 0 ),
    m_isRunningCallBack(EFalse)
{
    m_callbackArray = new RPointerArray<ServiceEventHandler>(INIT_CALLBACK_ARRAY_SIZE );
}

// ============================================================================
// DeviceLiwPeer::~DeviceLiwPeer
// destructor
//
// @since 5.0
// ============================================================================
//
DeviceLiwPeer::~DeviceLiwPeer()
{
    delete m_serviceName;
    m_callbackArray->ResetAndDestroy();
    m_callbackArray->Close();
    delete m_callbackArray;
    m_callbackArray = NULL;
    m_interface->Close();
    m_interface = NULL; // in majority cases, the interface close methods delete them selves.
}


// ============================================================================
// DeviceLiwPeer::SetServiceNameL
//
//
// @since 5.0
// ============================================================================
//
void DeviceLiwPeer::SetServiceNameL( ExecState* exec, const List& args )
{
    delete m_serviceName;
    m_serviceName = KJS::GetAsciiBufferL( args[0]->toString( exec ) );
}


// ============================================================================
// DeviceLiwPeer::SetObserver
//
//
// @since 5.0
// ============================================================================
//
void DeviceLiwPeer::InstallCallback( ServiceEventHandler* eventHandler )
{
    m_callbackArray->Append((const ServiceEventHandler*)eventHandler);
}


// ============================================================================
// DeviceLiwPeer::HandleNotifyL
// Implement MLiwNofityCallback
//
// @since 5.0
// ============================================================================
//
TInt DeviceLiwPeer::HandleNotifyL(
    TInt cmdId,
    TInt eventId,
    CLiwGenericParamList& eventParamList,
    const CLiwGenericParamList& /*inParamList*/ )
{
    // set running flag
    m_isRunningCallBack = ETrue;

    // look up the transaction id in the table to find the callback.
    int i;
    for (i = 0; i < m_callbackArray->Count(); i++)
    {
        ServiceEventHandler * event_handler = (*m_callbackArray)[i];
        if (event_handler && event_handler->TransId() == cmdId)
        {
            if (eventId == KLiwEventCanceled)
            {
                m_callbackArray->Remove(i); // cmdId is actually transaction Id.
                // clear running flag
                m_isRunningCallBack = EFalse;
                return KErrNone;
            }
            //Convert params to JS list and call m_onServiceEventCallback->InvokeCall()
            List params;
            JSLock::lock();
            JSValue* vEventParams = m_binding->LiwGenericParamList2JsArray(
                                                m_globalExecState, &eventParamList, ETrue );
            JSLock::unlock();
            params.append( jsNumber( cmdId ) );
            params.append( jsNumber( eventId ) );
            params.append( vEventParams );

            event_handler->InvokeCall( params );
            break;
        }
    }

    // clear running flag
    m_isRunningCallBack = EFalse;

    if ( i >= m_callbackArray->Count())
        return KErrNotFound;

    if (eventId == KLiwEventCompleted
        || eventId == KLiwEventError
        || eventId == KLiwEventStopped)
    {
        m_callbackArray->Remove(i); // cmdId is actually transaction Id.
    }

    return KErrNone;
}


// ============================================================================
// DeviceLiwPeer::toString
//
//
// @since 5.0
// ============================================================================
//
UString DeviceLiwPeer::toString( ExecState* /*exec*/ )
{
    if( m_interface )
    {
        return "[Interface Peer]";
    }

    return "[Service Peer]";
}


//END OF FILE
