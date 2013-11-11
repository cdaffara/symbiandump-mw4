/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of JS Object to wrap the LIW iterable variant type
*
*/


#ifndef _KJS_DEVICELIWITERABLE_H_
#define _KJS_DEVICELIWITERABLE_H_

//  INCLUDES
#include <object.h>
#include "Device.h"

/**
*  DeviceLiwIterable
*
*  @lib jsdevice.dll
*  @since 5.0
*/
namespace KJS
    {
    class CDeviceLiwBinding;
    class DeviceLiwIterable;
    
    class DeviceLiwIterablePrivate : public DevicePrivateBase
        {
        friend class DeviceLiwIterable;
        friend class DeviceLiwIterableFunc;
        public:
            DeviceLiwIterablePrivate(DeviceLiwIterable* jsobj, const CLiwIterable* m_iterable, CDeviceLiwBinding* liwBinding);
            ~DeviceLiwIterablePrivate();
            CDeviceLiwBinding* m_liwBinding;                 // not Owned
            Identifier m_propName;
            CLiwIterable* m_iterable;                        // not owned  
            ExecState* m_exec;                               // not owned
            DeviceLiwIterable* m_jsobj;                        // not owned
        };
        
    class DeviceLiwIterable : public JSObject
        {
        friend class DeviceLiwIterablePrivate;
        friend class DeviceLiwIterableFunc;
        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceLiwIterable( ExecState* exec,
                         CDeviceLiwBinding* binding,
                         CLiwIterable* variant);

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceLiwIterable();

        public:

           /**
            * Get property
            * @return Value
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
            * Get class info
            * @return const ClassInfo*
            * @since 5.0
            **/
            const ClassInfo* classInfo() const { return &info; }
             
           /**
            * whether this object is valid
            * @return boolean
            * @since 5.0
            */
            const TBool isValid() const { return m_valid; }
            
            /**
            * getIterableData
            * @return DevicePrivateBase*
            * @since 7.x
            */
            DevicePrivateBase* getIterableData() { return m_privateData; }
                        
            /**
            * close jsobject array
            * @return 
            * @since 5.0
            **/
            void Close();
            
            static const ClassInfo info;

            enum
                {
                getNext,
                reset,
                close
                };

           /**
            * toString
            * @return UString
            * @since 5.0
            **/
            virtual UString toString( ExecState* exec ) const;


    private:
            DeviceLiwIterablePrivate* m_privateData;   // private object to hold data
            TBool m_valid;                          // object is valid or not
        };

    class DeviceLiwIterableFunc : public JSObject
        {
        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceLiwIterableFunc( ExecState* exec,
                int token );

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceLiwIterableFunc();

        public: // From ObjectImp  JSObject

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
            JSValue* callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);

        private:
            int m_func;
        };

    }; // namespace

    #include "DeviceLiwIterable.lut.h"

#endif //_KJS_DEVICELIWITERABLE_H_
