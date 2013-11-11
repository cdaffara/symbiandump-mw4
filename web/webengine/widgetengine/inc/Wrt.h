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



#ifndef __WRT_H
#define __WRT_H

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
*  JSWrt
*
*  @lib widgetengine.dll
*  @since 5.0
*/
namespace KJS {

class ExecState;
class JSWrtPlatform;

struct WrtPrivate
{
    WrtPrivate( ) : m_platform(0) { }

    virtual ~WrtPrivate() {}

    JSWrtPlatform* m_platform;
};

class JSWrt: public JSObject
{

public:
        JSWrt( ExecState* exec );
        virtual ~JSWrt();

//From JSObject
public:
    JSType type() const;
    JSValue* getValueProperty(KJS::ExecState*, int token) const;
    bool getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot);
    const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;
    virtual void mark();
    virtual UString toString(ExecState *exec) const;

    enum {
        version,
        platform
        };

private:
    WrtPrivate* wrtPriv;
    HBufC* m_wrtVersion;
};

};

#include "Wrt.lut.h"

#endif
