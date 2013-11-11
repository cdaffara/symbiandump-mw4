/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Observer class for polling interval
*
*/




#ifndef ICTSPOLLINGINTERVALOBSERVER_H
#define ICTSPOLLINGINTERVALOBSERVER_H

#include <e32base.h>

class MIctsPollingIntervalObserver
    {

    public:
    
        /**
        * Timeout occurred ( HTTP response )
        *
        * @return None
        */
        virtual void PollingIntervalTimeout() = 0;
    
    };



#endif // ICTSPOLLINGINTERVALOBSERVER_H
