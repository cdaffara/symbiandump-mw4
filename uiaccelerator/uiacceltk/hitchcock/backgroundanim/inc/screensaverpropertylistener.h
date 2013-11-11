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

#ifndef __SCREENSAVERPROPERTYLISTENER_H__
#define __SCREENSAVERPROPERTYLISTENER_H__

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>

class CScreenSaverPropertyListener : public CActive
    {
    public:
        static CScreenSaverPropertyListener* NewL(TCallBack aCallback);
        virtual ~CScreenSaverPropertyListener();
        void RunL();
        void DoCancel();
        void IssueRequest();
        TInt GetScreenSaverStatus();
    protected:
        void ConstructL();
    private:
        CScreenSaverPropertyListener(TCallBack aCallback);
    
        RProperty iSCProperty;
        TCallBack iCallback;
        TInt iSCStatus;
    };
    
#endif
