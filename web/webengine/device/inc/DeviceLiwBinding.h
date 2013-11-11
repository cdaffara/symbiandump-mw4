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
* Description:  Device Liw Binding
*
*/


#ifndef __DEVICELIWBINDING_H
#define __DEVICELIWBINDING_H

#include <e32std.h>
#include <liwcommon.h>
#include "DeviceLiwMap.h"
#include "DeviceBinding.h"
#include "DeviceLiwIterable.h"

class CLiwServiceHandler;
class CRTSecMgrScriptSession;
class CRTSecManager;
namespace KJS
    {
    class List;
    class UString;

    class CDeviceLiwBinding : public CBase, public MDeviceBinding
        {
        friend class DeviceLiwMap;
        friend class DeviceLiwIterableFunc;
        friend class DeviceLiwIterable;

        public: // constructor and destructor

        static CDeviceLiwBinding* NewL();

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~CDeviceLiwBinding();

        public: // from MDeviceBinding

           /**
            * Load the service provider
            * @return TInt error code
            * @since 5.0
            **/
            TInt LoadServiceProvider( ExecState* exec, const List& args );

           /**
            * Factory method to create the device peer object
            * @return MDevicePeer*
            * @since 5.0
            **/
            MDevicePeer* ServiceObjectPeer( ExecState* exec,
                const List& args );

           /**
            * Invoke SAPI call
            * @return JSValue - output params of the SAPI call
            * @since 5.0
            **/
            JSValue* InvokeOp( ExecState* exec, const Identifier& propertyName,
                const List& args, MDevicePeer* peer );

            /**
            * Query and List providers
            * @return JSValue* - output params of the LIW call to list providers
            * @since 5.0
            **/
            JSValue* ListProviders( ExecState* exec, const List& args );

        public:

           /**
            * Create SAPI interface
            * @return JSValue - DeviceLiwInterface object
            * @since 5.0
            **/
            JSValue* CreateInterface( ExecState* exec,
                HBufC8 * serviceName,
                const Identifier& interfaceName );

           /**
            * Convert LiwGenericParamList to javascript list
            * @return JSValue - javascript list
            * @since 5.0
            **/
            JSValue* LiwGenericParamList2JsArray( ExecState* exec,
                CLiwGenericParamList* liwList, TBool managed = EFalse );

            /**
            * Convert Unload service provider
            * @return JSValue - javascript list
            * @since 5.0
            **/
            void UnloadServiceProvider(const TDesC8& aServiceName,const TDesC8& aInterfaceName);

            /**
            * SetUid
            * @return none
            * @since 5.0
            **/
            void SetUid( const TUint& aValue);

            CRTSecMgrScriptSession* GetSecuritySession ()
            {
                return m_scriptSession;
            }

        private:

            void ConstructL();

        private:

           /**
            * Invoke SAPI call
            * @return JSValue* - output params of the SAPI call
            * @since 5.0
            **/
            JSValue* InvokeOpInternal( ExecState* exec, const Identifier& prop,
                const List& args, MDevicePeer* peer );

           /**
            * Convert LiwVariant to javascript JSValue
            * @return JSValue - javascript JSValue
            * @since 5.0
            **/
            JSValue* LiwVariant2JsVal( ExecState* exec,
                const TLiwVariant& variant );

           /**
            * Check if the input is a javascript list
            * @return TBool
            * @since 5.0
            **/
            TBool IsAList( ExecState* exec, JSValue* value );

           /**
            * Convert javascript list to LiwGenericParamList
            * @return none
            * @since 5.0
            **/
            void JsList2LiwGenericParamListL( ExecState* exec,
                JSValue* value,
                CLiwGenericParamList& liwList );

           /**
            * Convert javscript JSValue to LiwVariant
            * @return TBool
            * @since 5.0
            **/
            TBool JsVal2LiwVariant( ExecState* exec, JSValue* value,
                TLiwVariant& variant );
#ifdef BRDO_SEC_MGR_PROMPT_ENHANCEMENT_FF
            /**
            * Set Widget Display Name
            * @return none
            * @since 5.0
            **/
            void SetAppName();
#endif
        private:

            CLiwServiceHandler* m_serviceHandler;   // Owned
            RCriteriaArray* m_critArr;               // Owned
             /// Main session to secManager
            CRTSecManager* m_secMgr;                  // Owned
            CRTSecMgrScriptSession* m_scriptSession;  // Owned
            TUid m_Uid;
        };

       /**
        * Convert 16 bit to 8 bit
        * @return HBufC8*
        * @since 5.0
        **/
        HBufC8* GetAsciiBufferL( const UString& buf );

    }

#endif //__DEVICELIWBINDING_H














