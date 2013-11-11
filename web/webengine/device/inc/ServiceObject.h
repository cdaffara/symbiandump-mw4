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
* Description:  Device Service Object
*
*/


#ifndef __DEVICE_SVC_OBJ_H__
#define __DEVICE_SVC_OBJ_H__

#include <e32base.h>
#include <object.h>

#include "Device.h"

namespace KJS
    {
    class JSObject;
    class Identifier;
    class ClassInfo;
    class Value;
    class ExecState;
    class UString;
    class ServiceEventHandler;
    class MDeviceBinding;
    class ServiceObjectFunc;
    class ServiceObject;

    class ServiceObjectPrivate : public DevicePrivateBase
        {
        friend class ServiceObject;
        friend class ServiceObjectFunc;
        public:
            ServiceObjectPrivate(ServiceObject* jsobj, HBufC8* svcName, MDeviceBinding* deviceBinding );
            ~ServiceObjectPrivate();
            MDeviceBinding* m_deviceBinding; // Not owned
            Identifier m_propName;
            HBufC8* m_svcName;    // owned
            ServiceObject* m_jsobj;
        };
            
    class ServiceObject : public JSObject
        {
        friend class ServiceObjectPrivate;
        friend class ServiceObjectFunc;

        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            ServiceObject( ExecState* exec,
                HBufC8* svcName,
                MDeviceBinding* deviceBinding );

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            ~ServiceObject();

        public://From JSObject

           /**
            * Get property
            * @return Value
            * @since 5.0
            **/
            JSValue* getValueProperty(KJS::ExecState*, int token) const;

            /**
            * getOwnPropertySlot
            * @return bool
            * @since 5.0
            **/
            bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);

            /**
            * IsRunningCallBack
            * @return TBool
            **/
            TBool IsRunningCallBack(ExecState *exec);

            /**
            * isValid
            */
            TBool isValid() const { return m_valid; }
            
            /**
            * getServiceData
            * @return DevicePrivateBase*
            * @since 7.x
            */
            DevicePrivateBase* getServiceData() { return m_privateData; }

           /**
            * Get class info
            * @return const ClassInfo*
            * @since 5.0
            **/
            const ClassInfo* classInfo() const { return &info; }

            static const ClassInfo info;

           /**
            * toString
            * @return UString
            * @since 5.0
            **/
            virtual UString toString( ExecState* exec ) const;

        public:
            void Close();
            enum
                {
                close
                };
            private:
            ServiceObjectPrivate* m_privateData;   // private object to hold data
            TBool m_valid;                          // object is valid or not
        };

        class ServiceObjectFunc : public JSObject
                {

                public: // constructor and destructor

                   /**
                    * Constructor
                    * @return none
                    * @since 5.0
                    **/
                    ServiceObjectFunc( ExecState* exec, int token );

                   /**
                    * Destructor
                    * @return none
                    * @since 5.0
                    **/
                    virtual ~ServiceObjectFunc() {}

                public: // From JSObject

                   /**
                    * implementsCall
                    * @return bool
                    * @since 5.0
                    **/
                    bool implementsCall() const { return true; }

                   /**
                    * call
                    * @return Value
                    * @since 5.0
                    **/
                    JSValue* callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);
                private:
                    int m_func;
                };

    }

#include "ServiceObject.lut.h"
#endif //__DEVICE_SVC_OBJ_H__



