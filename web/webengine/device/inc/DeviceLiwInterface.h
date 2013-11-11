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
* Description:  Declaration of Service Interface Object
*
*/


#ifndef _KJS_LIWIINTERFACE_H_
#define _KJS_LIWIINTERFACE_H_

#include <object.h>
#include "Device.h"

namespace KJS
    {
    class MDeviceBinding;
    class MDevicePeer;
    class ServiceEventHandler;
    class DeviceLiwInterface;
    class DeviceLiwResult;

   class DeviceLiwInterfacePrivate : public DevicePrivateBase
        {
        friend class DeviceLiwInterface;
        friend class DeviceLiwInterfaceFunc;
        public:
            DeviceLiwInterfacePrivate(DeviceLiwInterface* jsobj, MDeviceBinding* deviceBinding, MDevicePeer* devicePeer);
            ~DeviceLiwInterfacePrivate();
            MDeviceBinding* m_deviceBinding;                 // not owned
            MDevicePeer* m_devicePeer;                       // owned
            Identifier m_interfaceName;
            ExecState* m_exec;                               // not owned
            DeviceLiwInterface* m_jsobj;                     // not owned
        };
    
    class DeviceLiwInterface : public JSObject
        {
        friend class DeviceLiwInterfacePrivate;
        friend class DeviceLiwInterfaceFunc;
        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceLiwInterface( ExecState* exec, MDeviceBinding* deviceBinding,
                MDevicePeer* devicePeer );

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceLiwInterface();

        public:

           /**
            * getValueProperty
            * @return JSValue*
            * @since 5.0
            **/
            virtual JSValue* getValueProperty(KJS::ExecState*, int token) const { return jsUndefined(); };
           /**
            * getOwnPropertySlot
            * @return bool
            * @since 5.0
            **/
            bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);

           /**
            * Set callback
            * @return none
            * @since 5.0
            **/
            void SetObserver( ServiceEventHandler* aObserver );

           /**
            * Invoke SAPI function
            * @return Value
            * @since 5.0
            **/
            JSValue* InvokeOp( ExecState* exec,
                const Identifier& propertyName, const List& aArgs );

            /**
            * Close
            **/
            void Close();
            
            /**
            * isValid
            */
            TBool isValid() const { return m_valid; }

            /**
            * getInterfaceData
            * @return DevicePrivateBase*
            * @since 7.x
            */
            DevicePrivateBase* getInterfaceData() { return m_privateData; }
            
            /**
            * IsRunningCallBack
            */
            TBool IsRunningCallBack() const;

           /**
            * Get class info
            * @return const ClassInfo*
            * @since 5.0
            **/
            virtual const ClassInfo* classInfo() const { return &info; }

            static const ClassInfo info;

    private:
            DeviceLiwInterfacePrivate* m_privateData; // private object to hold data
            TBool m_valid;                             // object is valid or not
            
        };
        
    class DeviceLiwInterfaceFunc : public JSObject
        {
        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceLiwInterfaceFunc( ExecState* exec,
                const Identifier &propertyName );

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceLiwInterfaceFunc();

        public: // From JSObject

           /**
            * Whether implements the call
            * @return bool
            * @since 5.0
            **/
            bool implementsCall() const;

           /**
            * Call the function
            * @return Value
            * @since 5.0
            **/
            JSValue* callAsFunction(ExecState *exec, JSObject *aThisObj, const List &aArgs);

        private:
            Identifier m_funcName;
        };

    } // namespace

#endif //_KJS_DEVICELIWIINTERFACE_H_

