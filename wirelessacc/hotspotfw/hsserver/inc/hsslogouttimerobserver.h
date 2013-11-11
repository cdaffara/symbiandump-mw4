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
* Description:   Observer class logout timeout
*
*/




#ifndef HSSLOGOUTTIMEROBSERVER_H
#define HSSLOGOUTTIMEROBSERVER_H

#include <e32base.h>

class MHssLogoutTimerObserver
    {

    public:
    
        /**
        * Timeout occurred
        *
        * @return None
        */
        virtual void LogoutTimeout() = 0;
    
    };



#endif // HSSLOGOUTTIMEROBSERVER_H
