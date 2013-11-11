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
*      Declaration of class CFileSaver.   
*      
*
*/


#ifndef FILE_SAVER_H
#define FILE_SAVER_H

// INCLUDE FILES

#include <e32base.h>
#include <f32file.h>
#include "CodSaver.h"

// FORWARD DECLARATION

class CDocumentHandler;
class CDrmHandler;
class CCodBuffStorage;
class CCodData;

// CLASS DECLARATION

/**
* File saver (saving to file system). Handles DRM packaged objects too.
*/
NONSHARABLE_CLASS( CFileSaver ): public CCodSaver
    {
    public:     // Construct / destruct

        /**
        * Two phase constructor. Leaves on failure.
        * @param aType Data type.
        * @param aFs File Server session.
        * @param aDocHandler Document Handler.
        * @param aTempPath Temp path.
        * @param aRootPath Root path.
        * @param aFname File name.
        * @return The constructed saver.

        */      
        static CFileSaver* NewL
            (
            const TDesC8& aType,
            RFs& aFs,
            CDocumentHandler& aDocHandler,
            const TFileName& aTempPath,
            const TFileName& aRootPath,
            const TFileName& aFname
            );
        
        /**
        * Destructor.
        */
        virtual ~CFileSaver();

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
        * @param aAttached. ETrue if the download is attached and playing progressively.
        */
        virtual void InstallL( TRequestStatus* aStatus, const TDesC& aName, const TBool aAttached );

        /**
        * Perform bulk install for the downloaded media objects
        * @param aData CODData.
        * @param aAttached. ETrue if the download is attached and playing progressively.
        */        
        virtual void BulkInstallL( TRequestStatus* aStatus, const CCodData &aData, const TBool aAttached );
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
        virtual void ReleaseFileName( TFileName& aFname);
        
        /**
        * Get Name 
        * @return Name of Saver
        */        
        virtual HBufC* NameL ( ) const ;


    protected:  // Construct / destruct

        /**
        * Constructor.
        * @param aType Data type.
        * @param aFs File Server session.
        * @param aDocHandler Document Handler.
        * @param aTempPath Temp path.
        * @param aRootPath Root path.
        * @param aFname File name of content.
        */      
        CFileSaver
            (
            const TDesC8& aType,
            RFs& aFs,
            CDocumentHandler& aDocHandler,
            const TFileName& aTempPath,
            const TFileName& aRootPath,
            const TFileName& aFname
            );

        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

        /**
        * Update saved file to DCFRepository.
        * @param aFileName File name of new file.
        */
        void UpdateDCFRepositoryL( const TDesC& aFileName );
        
        /**
        * Update repository about the new file.
        * @param aFileName File name of new file.
        */
        void NotifyMediaGalleryL( const TDesC& aFileName );
        
        /**
        * Calls MediaGallery Update method so that media gallery
        * knows update its view.
        * @param aFileName File name of new file.
        */
        void UpdateMediaGalleryIfNeededL( const TDesC& aFileName );
        
       
       /**
        * Set progressive mode download
        * @param aValue ETrue if progressive mode
        */
        void SetProgressiveMode( TBool aValue );
       
        /**
        * Called if download completed.
        */
        void OnComplete();
         
        /**
        * Writes Buffer data to file
        */
        void FlushL();

       
    protected:  // data

        CDocumentHandler& iDocHandler;  ///< Document Handler. Not owned.
        RFs& iFs;                       ///< File Server session. Not owned.
        RFile iFile;                    ///< Temp file for content body. Own.
        const TFileName& iTempPath;     ///< Temp file path.
        const TFileName& iRootPath;     ///< Root path.
        CDrmHandler* iDrmHandler;       ///< DRM Handler or NULL. Owned.
        TFileName iFname;               ///< File name.
        
        //Double buff chaanges
        TInt32  iBufferedSize;          // How much data received over the air (but not necessarily all in file yet)
        TInt32  iBufferSize;            // disk io buffer size - bytes
        TInt32  iLength;                // EDlAttrLength
        TBool   iProgressiveDownload;   // EDlAttrProgressive
        CCodBuffStorage *iStorage;      //Implenments double buffering foe storing downloaded data
        
        
    friend class CCodBuffStorage;       
    };

#endif /* def FILE_SAVER_H */
