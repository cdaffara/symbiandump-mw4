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
* Description:  Declaration of class CRoapSaver.   
*
*/


#ifndef ROAP_SAVER_H
#define ROAP_SAVER_H

// INCLUDES

#include <e32base.h>
#include <RoapEng.h>
#include <RoapObserver.h>
#include "CodSaver.h"

// FORWARD DECLARATIONS

class CRoapData;
class CAiwGenericParamList;
class TCodProgress;

// CLASS DECLARATION

/**
* ROAP saver. ROAP is downloaded to memory buffer; installation is executing
* the ROAP.
*/
NONSHARABLE_CLASS( CRoapSaver ): public CCodSaver, public Roap::MRoapObserver
    {
    public:     // Constructors and destructor.

        /**
        * Two phased constructor. Leaves on failure.
        * @param aType Data type.
        * @param aData ROAP data created here. Ownership not taken.
        * @param aProgress Progress.
        * @param aProgressBytes Estimated number of bytes (progress scale).
        * @param aTempPath Temp path (used for multipart ROAP response).
        * @param aRootPath Root path (used for multipart ROAP response).
        * @param aFname File name (used for progressive downloads).
        * @return The created object.
        */      
        static CRoapSaver* NewL
            (
            const TDesC8& aType,
            CRoapData*& aData,
            TCodProgress* aProgress,
            TInt aProgressBytes,
            const TFileName& aTempPath,
            const TFileName& aRootPath,
            const TFileName& aFname
            );

        /**
        * Destructor.
        */
        virtual ~CRoapSaver();

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
        * Install downloaded content. Executes the ROAP.
        * @param aStatus Completes when done.
        * @param aName Preferred name (ignored).
        * @param aAttached. ETrue if the download is attached and playing progressively (ignored).
        */
        virtual void InstallL( TRequestStatus* aStatus, const TDesC& aName, const TBool aAttached );
        

        /**
        * BulkInstallL - No support for ROAP
        */        
        virtual void BulkInstallL( TRequestStatus* /*aStatus*/, const CCodData &/*aData*/, const TBool /*aAttached*/ ){}
        
        /**
        * Cancel installation.
        */
        virtual void CancelInstall();

        /**
        * Release content (take ownership of content from saver).
        * @param aFname Empty string returned here.
        * @param aHandler Handler UID returned here.
        */
        virtual void ReleaseContent( TFileName& aFname, TUid& aHandler );
           
        /**
        * Get the downloaded file size.
        */
        virtual TInt DownloadedFileSize()
            {
            return 0;
            };
        
        /**
        * Discard the old contents
        */        
        virtual void ResetL()
            {

            };

        /**
        * Clean up.
        */
        virtual void Cleanup( TBool aDeleteFile  );

        /**
        * Release file name (take ownership of content from saver).
        * @param aFname File name of content returned here.
        */
        virtual void ReleaseFileName( TFileName& aFname );

    private:    // Constructors and destructor.

        /**
        * Constructor.
        * @param aType Data type.
        * @param aData ROAP data created here. Ownership not taken.
        * @param aProgress Progress.
        * @param aProgressBytes Estimated number of bytes (progress scale).
        * @param aTempPath Temp path (used for multipart ROAP response).
        * @param aRootPath Root path (used for multipart ROAP response).
        * @param aFname File name of content.
        */
        CRoapSaver
            (
            const TDesC8& aType,
            CRoapData*& aData,
            TCodProgress* aProgress,
            TInt aProgressBytes,
            const TFileName& aTempPath,
            const TFileName& aRootPath,
            const TFileName& aFname
            );

    private:    // from Roap::MRoapObserver
    
        /**
        * Confirm creating network connection.
        * @return ETrue if network connection confirmed.
        */
        virtual TBool ConnectionConfL();

        /**
        * Confirm contacting Rights Issuer.
        * @return ETrue if confirmed.
        */
        virtual TBool ContactRiConfL();
		
        /**
        * Confirm transaction ID tracking.
        * @return ETrue if transaction ID tracking confirmed.
        */
        virtual TBool TransIdConfL();

       /**
        * Handle right objects after a successful RoAcq.
        * @param aRightsList Received right objects.
        */
		virtual void RightsObjectDetailsL
	        ( const RPointerArray<CDRMRights>& aRightsList );

        /**
        * ROAP response returned multipart response, return parameters
        * needed saving.
        * @param aTempFolder Folder for temp file returned here.
        * @param aContentName Name for content file returned here.
        * @param aMaxSize Safety size limit returned here.
        */
        virtual void ContentDownloadInfoL
            ( TPath& aTempFolder, TFileName& aContentName, TInt& aMaxSize );

        /**
        * Details of DCF file received in ROAP (saved by ROAP Engine).
        * @param aPath: The path and the filename of the saved DCF file.
        * @param aType: The plain MIME type of the saved DCF file.
        * @param aAppUid: The handler app UID of the saved DCF file.
        */
		virtual void ContentDetailsL
		    ( const TDesC& aPath, const TDesC8& aType, const TUid& aAppUid );
                                      
        /**
        * Handle DCF file (received in multipart RO response).
        * @aFilename File name.
        */
        virtual void HandleDCFPartL( const TDesC8& aFilename );

        /**
        * Progress callback.
        * @param aProgressInfo Percentage completed.
        */
		virtual void RoapProgressInfoL( const TInt aProgressInfo );

       /**
        * ErrorUrl
        * @param aErrorUrl: The error URL
        * @return void
        */
        virtual void ErrorUrlL( const TDesC8& aErrorUrl );

        /**
        * To handle PostResponseUrlL as part of metering response
        * @param aPrUrl: The PostResponseURL
        * @return void
        */
        virtual void PostResponseUrlL( const TDesC8& aPrUrl );
        		
    private:    // data

        HBufC8* iBuf;                   ///< Trigger buffer. Owned.
        Roap::CRoapEng* iEng;           ///< Engine. Owned.
        CRoapData*& iData;              ///< Data. Created but not owned.
        TCodProgress* iProgress;        ///< Progress. Not owned.
        TInt iProgressBytes;            ///< Progress scale.
        const TFileName& iTempPath;     ///< Temp file path.
        const TFileName& iRootPath;     ///< Root path.
        TFileName iFname;               ///< File name (DCF response).
        TPtrC iContentName;             ///< Content name.

    };

#endif /* def ROAP_SAVER_H */
