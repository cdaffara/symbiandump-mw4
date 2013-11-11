/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Declaration of class RFotaSaver.   
*      
*
*/


#ifndef FOTA_SAVER_H
#define FOTA_SAVER_H

// INCLUDE FILES

#include <e32base.h>
#include <fotaengine.h>
#include "CodSaver.h"

// CLASS DECLARATION

/**
* FOTA saver. FOTA update package is downloaded to dedicated FOTA store.
* Installation is do-nothing (COD just downloads).
*/
NONSHARABLE_CLASS( CFotaSaver ): public CCodSaver
    {
    public:     // Construct / destruct

        /**
        * Two phase constructor. Leaves on failure.
        * @param aType Data type.
        * @param aPkgId FOTA package id.
        * @return The constructed response.
        */      
        static CFotaSaver* NewL( const TDesC8& aType, TInt aPkgId );
        
        /**
        * Destructor.
        */
        virtual ~CFotaSaver();

    public:     // from CCodSaver

        /**
        * Open temporary store for download.
        */
        virtual void OpenStoreL();
        
        /**
        * Append data to store.
        * @param aData Data to append.
        * @return Error code.
        */
        virtual TInt AppendData( const TDesC8& aData );
        
        /**
        * Download finished, close temporary store.
        */
        virtual void CloseStore();

        /**
        * Check if attributes of received content match descriptor data.
        * Leaves with KErrCodAttributeMismatch in case of mismatch.
        * @param aData Data to match.
        */
        virtual void CheckResponseAttributesL( const CCodData& aData );

        /**
        * Install downloaded content.
        * @param aStatus Completes when done.
        * @param aName Preferred name.
        */
        virtual void InstallL( TRequestStatus* aStatus, const TDesC& aName, const TBool aAttached );

        /**
        * BulkInstallL - No support for FOTA
        */
        virtual void BulkInstallL( TRequestStatus* /*aStatus*/, const CCodData &/*aData*/, const TBool /*aAttached*/ ){}
        /**
        * Cancel installation.
        */
        virtual void CancelInstall();

        /**
        * Release content (take ownership of content from saver).
        * @param aFname File name of content returned here.
        * @param aHandler Handler UID returned here.
        */
        virtual void ReleaseContent( TFileName& aFname, TUid& aHandler );

        /**
        * Get the downloaded file size.
        */
        virtual TInt DownloadedFileSize();
        
        /**
        * Discard the old contents
        */        
        virtual void ResetL();

        /**
        * Clean up.
        */
        virtual void Cleanup( TBool aDeleteFile  );        

        /**
        * Release file name (take ownership of content from saver).
        * @param aFname File name of content returned here.
        */
        virtual void ReleaseFileName( TFileName& aFname );


    protected:  // Construct / destruct

        /**
        * Constructor.
        * @param aType Data type.
        * @param aPkgId FOTA package id.
        */      
        CFotaSaver( const TDesC8& aType, TInt aPkgId );
        
    private:    // data

        RFotaEngineSession iEng;    ///< FOTA Engine. Owned.
        RWriteStream* iStore;       ///< FOTA Store. Not owned.
        TInt iPkgId;                ///< FOTA package id.
        
    };

#endif /* def FOTA_SAVER_H */
