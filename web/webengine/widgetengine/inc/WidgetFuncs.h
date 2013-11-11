/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implemetation of CMediaGalleryWidgetFuncs
*
*/
#ifndef __WIDGETFUNCS_H__
#define __WIDGETFUNCS_H__

#include "config.h"
#include <kjs/object.h>

class MJSWidgetCallbacks;

namespace KJS {

class CWidgetEventHandler;

class JSWidgetFunc : public JSObject
    {
    public:
        JSWidgetFunc(ExecState* exec, int aFuncType, MJSWidgetCallbacks* aWidgetCallbacks);
        virtual ~JSWidgetFunc();

    //From JSObject
    public:
        bool implementsCall() const;        
        JSValue* callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);
        enum{
            openApplication,
            openURL,
            preferenceForKey,
            prepareForTransition,
            performTransition,
            setPreferenceForKey,
            setNavigationMode,
            setNavigationType,
            setDisplayLandscape,
            setDisplayPortrait
            };

            
    private:        
        int funcType;
        MJSWidgetCallbacks* m_callbacks;                
        
    };

};

#endif
