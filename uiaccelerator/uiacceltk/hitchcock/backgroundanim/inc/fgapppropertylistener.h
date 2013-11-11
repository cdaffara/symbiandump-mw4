/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

#ifndef __FGAPPPROPERTYLISTENER_H__
#define __FGAPPPROPERTYLISTENER_H__

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>

class CFGAppPropertyListener : public CActive
    {
    public:
        static CFGAppPropertyListener* NewL(TCallBack aCallback);
        virtual ~CFGAppPropertyListener();
        void RunL();
        void DoCancel();
        void IssueRequest();
        TInt GetHSFGStatus();
    protected:
        void ConstructL();
    private:
        CFGAppPropertyListener(TCallBack aCallback);
    
        RProperty iHSFGProperty;
        TCallBack iCallback;
        TInt iHSFGStatus;
    };
    
#endif
