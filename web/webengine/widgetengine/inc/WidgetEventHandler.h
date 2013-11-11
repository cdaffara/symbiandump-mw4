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
* Description:  Implemetation of WidgetEventHandler
*
*/
#ifndef __WIDGETEVENTHANDLER_H__
#define __WIDGETEVENTHANDLER_H__

#include <e32base.h>

class MJSObjectProtector;

namespace KJS {


class JSObject;
class JSValue;
class ExecState;

class WidgetEventHandler : public CBase
    {
    public:
        WidgetEventHandler( JSValue* aValue, ExecState* aExecState, MJSObjectProtector* aProtector);
        virtual ~WidgetEventHandler();

    public:
        void InvokeCall(const TInt& val = -1);
        void Delete();
        
    private:
        JSValue* iEventHandler;
        
        ExecState* iGlobalExecState;

		MJSObjectProtector* iProtector;
        
        TBool iInUse;
        TBool iSelfDelete;
    };

}

#endif
