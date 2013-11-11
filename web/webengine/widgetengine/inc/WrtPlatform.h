/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __WRT_PLATFORM_H
#define __WRT_PLATFORM_H

//  INCLUDES
#include "config.h"
#include <e32base.h>
#include <kjs/object.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  JSWrtPlatform
*
*  @lib widgetengine.dll
*  @since 5.0
*/
namespace KJS {

class ExecState;

struct WrtPlatformPrivate
{
    WrtPlatformPrivate() {}
    
    virtual ~WrtPlatformPrivate() {}
    
    UString m_sisVersion;
    UString m_manufacturer;
    UString m_model;
//    UString m_revision;
//    UString m_serialNumber;
};

class JSWrtPlatform: public JSObject
{

public:
        JSWrtPlatform( ExecState* exec );
        virtual ~JSWrtPlatform();

//From JSObject
public:
    JSType type() const;
    JSValue* getValueProperty(KJS::ExecState*, int token) const;
    bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);
  
    const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;

    virtual UString toString(ExecState *exec) const;

    enum {                
        id,
        romVersion,
        packageVersion,
        manufacturer, /** Phone manufacturer identification, in character string format. */
        model, /** Phone model identification, in character string format. */
//        revision /** Phone revision identification, in character string format. */
//        serialNumber /** Phone serial number (IMEI or ESN), in character string format. */
        };

private:
    UString SISVersionL();
    void GetPhoneInfoL();

private:
    WrtPlatformPrivate* pltPriv;
};

};

#include "WrtPlatform.lut.h"

#endif
