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


#ifndef __DEVICE_H
#define __DEVICE_H

//  INCLUDES
#include <object.h>

/**
*  Device
*
*  @lib jsdevice.dll
*  @since 5.0
*/


namespace KJS
    {

    class MDeviceBinding;
    class ServiceObject;
    class Device;
    
    class DevicePrivateBase
        {
        public:
            DevicePrivateBase();
            virtual ~DevicePrivateBase();
            void AddChild( DevicePrivateBase* aValue );
            void RemoveChild( DevicePrivateBase* aValue );
            void SetParent( DevicePrivateBase* aValue );

        private: 
            TBool m_isDeleting;   
            DevicePrivateBase* m_parent;
            RPointerArray<DevicePrivateBase>* m_children;
        };
        
    class DevicePrivate : public DevicePrivateBase
        {
        friend class Device;
        friend class DeviceFunc;
        public:
            DevicePrivate(Device* jsobj);
            ~DevicePrivate();

        private:
            Identifier m_propName;
            ExecState* m_exec;                               // not owned
            Device* m_jsobj;                                 // not owned
        };
    
    class Device: public JSObject
        {
        friend class DevicePrivate;
        friend class DeviceFunc;
        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            Device( ExecState* exec );

           /**
            * Pre-Destructor
            * Clean up resources before garbage collection destroys us
            * @return none
            * @since 5.0
            **/
            void Close();

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~Device();


        public: // From JSObject

           /**
            * getOwnPropertySlot
            * @return bool
            * @since 5.0
            **/
            bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);
           /**
            * getValueProperty
            * @return JSValue*
            * @since 5.0
            **/
            JSValue* getValueProperty(KJS::ExecState* exec, int token) const;

           /**
            * class info
            * @return const ClassInfo*
            * @since 5.0
            **/
            const ClassInfo* classInfo() const { return &info; }

           /**
            * whether this object is valid
            * @return boolean
            * @since 5.0
            */
            const TBool valid() const { return m_valid; }

            /**
            * getServiceData
            * @return DevicePrivateBase*
            * @since 7.x
            */
            DevicePrivateBase* getDeviceData() { return m_privateData; }
            
            static const ClassInfo info;

           /**
            * entries in lookup table
            */
            enum
                {
                getServiceObject,
                listProviders
                };

           /**
            * toString
            * @return UString
            * @since 5.0
            **/
            virtual UString toString( ExecState* exec ) const;

           /**
            * SetUid
            * @return none
            * @since 5.0
            **/
            void SetUid( const TUint& aValue);
            
            MDeviceBinding* GetDeviceBinding();

        private:
            DevicePrivate* m_privateData;   // private object to hold data
            MDeviceBinding* m_deviceBinding; // Owned
            TBool m_valid;                   // object is valid or not
        };

    class DeviceFunc : public JSObject
        {

        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceFunc( ExecState* exec, MDeviceBinding* deviceBinding,
                int token );

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceFunc() {}

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
            MDeviceBinding* m_deviceBinding;    // not owned
            int m_func;                         // not owned
        };

    }

#include "Device.lut.h"

#endif //__DEVICE_H
