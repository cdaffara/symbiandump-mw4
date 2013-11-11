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
* Description:   Observer class for total polling time
*
*/




#ifndef ICTSPOLLINGTIMEOBSERVER_H
#define ICTSPOLLINGTIMEOBSERVER_H

#include <e32base.h>

class MIctsPollingTimeObserver
    {

    public:
    
        /**
        * Timeout occurred
        *
        * @return None
        */
        virtual void PollingTimeTimeout() = 0;
    
    };



#endif // ICTSPOLLINGTIMEOBSERVER_H
