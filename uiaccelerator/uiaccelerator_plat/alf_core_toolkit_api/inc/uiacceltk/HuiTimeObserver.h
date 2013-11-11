/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#ifndef __HUITIMEOBSERVER_H__
#define __HUITIMEOBSERVER_H__


#include <e32def.h>


/**
 * Interface for classes that follow the progress of time.
 */
class MHuiTimeObserver
    {
public:

    /**
     * Called when time progresses.
     *
     * @param aElapsedTime  Elapsed time in seconds.
     */
    virtual void AdvanceTime(TReal32 aElapsedTime) __SOFTFP = 0;
        
    };


#endif // __HUITIMEOBSERVER_H__
