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


#ifndef __DEVICELIWMAP_H
#define __DEVICELIWMAP_H

//  INCLUDES
#include <object.h>
#include "Device.h"

/**
*  Device
*
*  @lib jsdevice.dll
*  @since 5.0
*/

class CLiwMap;
namespace KJS
    {
    class CDeviceLiwBinding;
    class DeviceLiwMap;
    
    class DeviceLiwMapPrivate : public DevicePrivateBase
        {
        friend class DeviceLiwMap;
        friend class DeviceLiwMapFunc;
        public:
            DeviceLiwMapPrivate(DeviceLiwMap* jsobj, const CLiwMap* liwMap, CDeviceLiwBinding* liwBinding);
            ~DeviceLiwMapPrivate();
            CDeviceLiwBinding* m_liwBinding;                 // not Owned
            Identifier m_propName;
            CLiwMap* m_liwMap;                               // not owned 
            DeviceLiwMap* m_jsobj;                           // not owned 
        };
   
    class DeviceLiwMap: public JSObject
        {
        friend class DeviceLiwMapPrivate; 
        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceLiwMap( JSValue* proto, const CLiwMap* liwMap, CDeviceLiwBinding* liwBinding);


           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceLiwMap();


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
            const TBool isValid() const { return m_valid; }
            
           /**
            * getMapData
            * @return DevicePrivateBase*
            * @since 7.x
            */
            DevicePrivateBase* getMapData() { return m_privateData; }
            
            /**
            * close jsobject array
            * @return 
            * @since 5.0
            **/
            void Close();

           /**
            * toString
            * @return UString
            * @since 5.0
            **/
            virtual UString toString( ExecState* exec ) const;

            static const ClassInfo info;
            enum
                {
                close
                };

        private:

            DeviceLiwMapPrivate* m_privateData;   // private object to hold data
            TBool m_valid;                          // object is valid or not
        };
        
    class DeviceLiwMapFunc : public JSObject
        {
        public: // constructor and destructor

           /**
            * Constructor
            * @return none
            * @since 5.0
            **/
            DeviceLiwMapFunc( ExecState* exec, int token );

           /**
            * Destructor
            * @return none
            * @since 5.0
            **/
            virtual ~DeviceLiwMapFunc() {}

        public: // From ObjectImp  JSObject

           /**
            * Whether implements the call
            * @return bool
            * @since 5.0
            **/
            bool implementsCall() const { return true; }

           /**
            * Call the function
            * @return Value
            * @since 5.0
            **/
            JSValue* callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);

        private:
            int m_func;
        };

    }
#include "DeviceLiwMap.lut.h"
#endif //__DEVICE_H
