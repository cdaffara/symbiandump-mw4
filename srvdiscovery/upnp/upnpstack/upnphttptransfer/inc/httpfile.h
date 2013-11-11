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
* Description:  Represents one downloadable file
*
*/


#ifndef C_CHTTPFILE_H
#define C_CHTTPFILE_H


// System include files
#include <e32cmn.h>
#include <e32base.h>
#include <f32file.h>

// User include files
#include "httpheader.h"

/**
 * Class which represents one dowloaded/uploaded file  
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS ( CHttpFile ) : public CBase
    {
public:

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aKey Identifier of the file
     * @param aUri Uri of the file
     * @param aPath Local path of the file
     * @return Instance of CHttpFile object
     */
    static CHttpFile* NewL( TAny* aKey, const TDesC8& aUri,
                            const TDesC& aPath );

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aKey Identifier of the file
     * @param aUri Uri of the file
     * @param aPath Local path of the file
     * @return Identifier of the object
     */
    static CHttpFile* NewLC( TAny* aKey, const TDesC8& aUri, 
                             const TDesC& aPath );

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aKey Identifier of the file
     * @param aUri Uri of the file
     * @param aFileHandle Handle to the file
     * @return Instance of CHttpFile object
     */
    static CHttpFile* NewL( TAny* aKey, const TDesC8& aUri,
                            const RFile& aFileHandle );

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aKey Identifier of the file
     * @param aUri Uri of the file
     * @param aFileHandle Handle to the file
     * @return Identifier of the object
     */
    static CHttpFile* NewLC( TAny* aKey, const TDesC8& aUri, 
                             const RFile& aFileHandle );

    /**
     * Virtual destructor
     *
     * @since S60 v3.2
     */
    ~CHttpFile();

    /**
     * Setter for the member variable iTrackProgress
     *
     * @since S60 v3.2
     * @param aValue Value of the progress flag
     * @return Identifier of the object
     */
    void TrackProgress( TBool aValue );

    /**
     * Returns if tracking is set on.
     *
     * @since S60 v3.2
     * @return True if tracking is on, false otherwise
     */
    TBool TrackingOn() const;
    
    /**
     * Getter for the member variable iKey
     *
     * @since S60 v3.2
     * @return Identifier of the object
     */
    TAny* Key() const;

    /**
     * Getter for the iUri member variable
     *
     * @since S60 v3.2
     * @return Reference to the Uri
     */
    const HBufC8* Uri() const;

    /**
     * Setter for iUri
     *
     * @since S60 v3.2
     */
    void SetUriL( const TDesC8& aUri );
    
    /**
     * Getter for the iPath member variable
     *
     * @since S60 v3.2
     * @return Pointer to the path
     */
    const HBufC* Path() const;

    /**
     * Getter for the filehandle
     *
     * @since S60 v3.2
     * @return Handle to the file
     */
    RFile FileHandle() const;

    /**
     * Sets a header
     *
     * @since S60 v3.2
     * @param aFieldName The field name, e.g. 'Accept'
     * @param aFieldValue The field value. e.g. 'text/plain'
     */
    void SetHeaderL( const TDesC8& aFieldName, const TDesC8& aFieldValue );

    /**
     * Getter for header array
     *
     * @since S60 v3.2
     * @return Array of CHttpHeader objects which are set to this file
     */
    RPointerArray<CHttpHeader>& Headers();

    /**
     * Creates the file in the file system
     *
     * @since S60 v3.2
     */
    void CreateFileInFileSystemL();

    /**
     * Deletes the file from the file system
     *
     * @since S60 v3.2
     */
    void DeleteFileFromFileSystemL();
    
    /**
     * Closes the file
     */
    void CloseFile();

private:

    /**
     * Default constructor
     *
     * @since S60 v3.2
     * @param aKey Identifier of the file
     * @return Identifier of the object
     */
    CHttpFile( TAny* aKey );

    /**
     * Default constructor
     *
     * @since S60 v3.2
     * @param aKey Identifier of the file
     * @param aFileHandle Handle to the file
     * @return Identifier of the object
     */
    CHttpFile( TAny* aKey, const RFile& aFileHandle );

    /**
     * Second phase constructor
     *
     * @since S60 v3.2
     * @param aUri Uri of the file
     * @param aPath Local path of the file
     */
    void ConstructL( const TDesC8& aUri, const TDesC& aPath );

    /**
     * Second phase constructor
     *
     * @since S60 v3.2
     * @param aUri Uri of the file
     */
    void ConstructL( const TDesC8& aUri );

private: 

    /**
     * Indicator if tracking of the progress is switched on
     */
    TBool iTrackProgress;

    /**
     * Identifier of the object
     * Not own.
     */
    TAny* iKey;

    /**
     * Uri
     * Own.
     */
    HBufC8* iUri;

    /**
     * Full path on local system
     * Own.
     */
    HBufC* iPath;
    
    /**
     * Array of headers
     */
    RPointerArray<CHttpHeader> iHeaderArray;
    
    /**
     * Handle to the file
     */
    RFile iFile;

    /**
     * File server session
     */
    RFs iFsSession;

    };

#endif // C_CHTTPFILE_H
