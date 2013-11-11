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
* Description:  Liw Device binding
*
*/


#ifndef __DEVICE_BINDING_H__
#define __DEVICE_BINDING_H__

#include <e32std.h>
#include <object.h>
#include <protect.h>

class CRTSecMgrScriptSession;

namespace KJS
    {
    class ServiceEventHandler;

    //Device peer holds platform-specific per-service object state
    class MDevicePeer
        {
        public:
            virtual ~MDevicePeer() {};
            virtual UString toString( ExecState* exec ) = 0;
            virtual void InstallCallback( ServiceEventHandler* callback ) = 0;
        };

    //Device binding holds common platform-specific state
    class MDeviceBinding
        {
        public:
            virtual ~MDeviceBinding() {};
            virtual TInt LoadServiceProvider(
                ExecState* exec, const List& args ) = 0;
            virtual void UnloadServiceProvider(const TDesC8& aServiceName, const TDesC8& aInterfaceName) = 0;
            virtual MDevicePeer* ServiceObjectPeer(
                ExecState* exec, const List& args ) = 0;
            virtual JSValue* InvokeOp( ExecState* exec, const Identifier& prop,
                const List& args, MDevicePeer* peer ) = 0;
            virtual JSValue* CreateInterface( ExecState* exec,
                HBufC8* serviceName,
                const Identifier& prop ) = 0;
            virtual JSValue* ListProviders( ExecState* exec, const List& args ) = 0;
            virtual void SetUid ( const TUint& aValue) = 0;
            virtual CRTSecMgrScriptSession* GetSecuritySession () = 0;
        };
    }

#endif
