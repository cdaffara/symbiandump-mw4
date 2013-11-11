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
* Description:  Base class for downloader / uploader classes
*
*/


#ifndef C_CHTTPTRANSFERTIMEROBSERVER_H
#define C_CHTTPTRANSFERTIMEROBSERVER_H

/**
 * This interface class is used by timer to start next transfer
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 */
class MHttpTransferTimerObserver 
    {
public:
    /**
     * Callback to start new transfer
     *
     * @since S60 v3.2
     */
    virtual void TimerCallback()=0;

    };

#endif //C_CHTTPTRANSFERTIMEROBSERVER_H

