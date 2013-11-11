/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Observer class login timeout
*
*/




#ifndef HSSLOGINTIMEROBSERVER_H
#define HSSLOGINTIMEROBSERVER_H

#include <e32base.h>

class MHssLoginTimerObserver
    {

    public:
    
        /**
        * Timeout occurred
        *
        * @return None
        */
        virtual void LoginTimeout() = 0;
    
    };



#endif // HSSLOGINTIMEROBSERVER_H
