/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Observer for worker objects
*
*/


#ifndef C_MHTTPWORKEROBSERVER_H
#define C_MHTTPWORKEROBSERVER_H


/**
 * Interface class for worker events
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 * @deprecated
 */
class MHttpWorkerObserver
    {
public:

    /**
     * Call back method used to inform that worker has completed
     *
     * @since S60 v3.2
     * @deprecated
     */
    virtual void WorkerCompleted()=0;
    };

#endif // C_MHTTPWORKEROBSERVER_H