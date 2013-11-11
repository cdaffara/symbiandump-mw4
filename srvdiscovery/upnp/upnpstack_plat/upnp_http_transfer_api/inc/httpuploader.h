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
* Description:  Handles uploads
*
*/


#ifndef C_CHTTPUPLOADER_H
#define C_CHTTPUPLOADER_H


// System include files
#include "httptransferbase.h"
#include "httptransferobserver.h"

/**
 * This class handles the queue of the uploaded files and array of workers.
 *  
 * @lib httptransfer.lib
 * @since S60 v3.2
 * @deprecated
 */
class CHttpUploader : public CHttpTransferBase
    {
public:

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aObserver Transfer observer
     * @param aIAPId IAP id
     * @param aBufferSize size of the upload buffer
     * @param aParallelTransfers Number of parallel transfers
     * @deprecated
     */
    IMPORT_C static CHttpUploader* NewL(
                                    MHttpTransferObserver& aObserver,
                                    TUint aIAPId,
                                    TUint aBufferSize,
                                    TUint aParallelTransfers );

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aObserver Transfer observer
     * @param aIAPId IAP id
     * @param aBufferSize size of the upload buffer
     * @param aParallelTransfers Number of parallel transfers
     * @deprecated
     */
    IMPORT_C static CHttpUploader* NewLC(
                                    MHttpTransferObserver& aObserver,
                                    TUint aIAPId,
                                    TUint aBufferSize,
                                    TUint aParallelTransfers );

    /**
     * Destructor
     *
     * @since S60 v3.2
     * @deprecated
     */
    virtual ~CHttpUploader();

    /**
     * Method for uploading the file. Creates and enqueues the file
     *
     * @since S60 v3.2
     * @param aUri Uri of the downloaded object
     * @param aTargetPath Local path of the object
     * @param aKey Identifier of the object
     * @deprecated
     */
    IMPORT_C void UploadFileL( const TDesC8& aUri, 
                               const TDesC& aTargetPath, 
                               TAny* aKey );

private:

    /**
     * Default constructor
     *
     * @since S60 v3.2
     * @param aObserver Transfer observer
     * @param aIAPId IAP id
     * @param aBufferSize size of the upload buffer
     * @param aParallelTransfers Number of parallel transfers
     * @deprecated
     */
    CHttpUploader( MHttpTransferObserver& aObserver,
                   TUint aIAPId,
                   TUint aBufferSize,
                   TUint aParallelTransfers );

    /**
     * Second phase constructor
     *
     * @since S60 v3.2
     * @deprecated
     */
    void ConstructL();
    
    };

#endif // C_CHTTPDOWNLOADER_H
