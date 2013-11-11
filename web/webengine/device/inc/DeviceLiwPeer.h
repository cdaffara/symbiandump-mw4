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
* Description:  Liw Device Peer
*
*/

#ifndef __DEVICELIWPEER_H
#define __DEVICELIWPEER_H

#include <e32std.h>
#include <liwcommon.h>
#include "DeviceBinding.h"

namespace KJS
    {
    class JSValue;
    class List;
    class UString;
    class ExecState;
    class ServiceEventHandler;
    class CDeviceLiwBinding;

    class DeviceLiwPeer : public MDevicePeer, public MLiwNotifyCallback
        {

        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceLiwPeer( ExecState* exec,
                CDeviceLiwBinding* liwbinding, MLiwInterface* liwinterface );

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceLiwPeer();

        public:

           /**
            * Set the service name
            * @return none
            * @since 5.0
            **/
            void SetServiceNameL( ExecState* exec, const List& args );

           /**
            * Set the callback
            * @return none
            * @since 5.0
            **/
            virtual void InstallCallback( ServiceEventHandler* eventHandler );

           /**
            * toString
            * @return UString
            * @since 5.0
            **/
            UString toString( ExecState* exec );

           /**
            * Get the interface
            * @return MLiwInterface*
            * @since 5.0
            **/
            MLiwInterface* Interface() { return m_interface; }

           /**
            * Get the service name
            * @return const HBufC8*
            * @since 5.0
            **/
            const HBufC8* ServiceName() const { return m_serviceName; }

           /**
            * Is running callback function
            * @return bool
            * @since 5.0
            **/
            TBool IsRunningCallBack() const { return m_isRunningCallBack; }

        public: // From MLiwNotifyCallback

           /**
            * Handle invoking callback function for asynchronous calls
            * @return TInt error code
            * @since 5.0
            **/
            TInt HandleNotifyL(
                TInt cmdId,
                TInt eventId,
                CLiwGenericParamList& eventParamList,
                const CLiwGenericParamList& inParamList );

        private:

            ExecState* m_globalExecState; // Not owned
            CDeviceLiwBinding* m_binding; // Not owned
            MLiwInterface* m_interface; // Not owned, managed by LIW
            HBufC8* m_serviceName; //owned
            // a map to contain transaction ID, and event handler
            RPointerArray<ServiceEventHandler>* m_callbackArray;  // owned
            TBool m_isRunningCallBack;
        };
    }

#endif //__DEVICELIWPEER_H
