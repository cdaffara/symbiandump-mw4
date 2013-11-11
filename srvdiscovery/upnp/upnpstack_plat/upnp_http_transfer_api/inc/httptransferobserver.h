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
* Description:  Observer for downloads / uploads
*
*/


#ifndef C_MHTTPTRANSFEROBSERVER_H
#define C_MHTTPTRANSFEROBSERVER_H


/**
 * Interface class for transfer events  
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 * @deprecated
 */
class MHttpTransferObserver
    {
public:

    /**
     * Callback method to inform transfer progress to the observer.
     * Transfer progress is indicated every time httpframework informs that
     * it has received a buffer of data or needs new data to be sent
     *
     * @since S60 v3.2
     * @param aKey Identifier of the file
     * @param aBytes Bytes transferred so far
     * @param aTotalBytes Total bytes of the file
     * @deprecated
     */
    virtual void TransferProgress( TAny* aKey,
                                   TInt aBytes, 
                                   TInt aTotalBytes )=0;

    /**
     * Callback method to inform that file is ready to be transfered
     *
     * @since S60 v3.2
     * @param aKey Identifier of the file
     * @deprecated
     */
    virtual void ReadyForTransferL( TAny* aKey )=0;

    /**
     * Callback method to inform the transfer completion to the observer
     *
     * @since S60 v3.2
     * @param aKey Identifier of the file
     * @param aStatus Status of the process
     * @deprecated
     */
    virtual void TransferCompleted( TAny* aKey, TInt aStatus )=0;

    };

#endif // C_MHTTPTRANSFEROBSERVER_H
