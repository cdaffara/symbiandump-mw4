/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handle a download related util functions, such as isDrm
*
*/



#ifndef CDOWNLOADUTILS_H
#define CDOWNLOADUTILS_H

//  INCLUDES
#include <e32base.h>
#include <HttpDownloadMgrCommon.h>

// FORWARD DECLARATIONS
class RHttpDownload;

// CONSTANTS
_LIT8( KSisFileMimeType, "application/vnd.symbian.install" );
_LIT8( KOctetStreamMimeType, "application/octet-stream" );

// CLASS DECLARATION

/**
*  Supports user interaction dialogs for downloads, etc.
*/
NONSHARABLE_CLASS( CDownloadUtils ) : public CBase 
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * &param aDMgrUiBase Reference to UI base functionality.
        */
        static CDownloadUtils* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CDownloadUtils();

    public: // New functions

        /**
        * Check if the download is DRM.
        * @param aDownload The actual download
        * @return ETrue/EFalse
        */
        TBool DrmDownloadL( RHttpDownload& aDownload );

        /**
        * Check if the DRM download has rights on the phone.
        * @param aDownload The actual download
        * @param aPreviewRights Output. ETrue, if the right is preview.
        * @return ETrue/EFalse
        */
        TBool DrmRightsOnThePhoneL( RHttpDownload& aDownload, TBool& aPreviewRights ) const;

        /**
        * Is the content type supported.
        * @param aDownload The actual download
        * @return True/False.
        */
        TBool IsContentTypeSupportedL( RHttpDownload& aDownload );

        /**
        * Is the content type supported.
        * @param aContentType The mime type
        * @return True/False.
        */
        TBool IsContentTypeSupportedL( const TDesC8& aContentType );

        /**
        * Is the content type supported.
        * @param aDownload The actual download.
        * @param aContentType The mime type
        * @return True/False.
        */
        TBool IsContentTypeSupportedL( RHttpDownload& aDownload, const TDesC8& aContentType );

        /**
        * Get the MIME type of the download..
        * @param aDownload The actual download.
        * @param aDrmResolve If it's ETrue, the inner DRM content type is got.
        * @return The allocated buffer containing the mime type.
        */
        HBufC8* ContentTypeL( RHttpDownload& aDownload, TBool aDrmResolve, TInt mediaObjectIndex = 0 );
        
        /**
        * Is the content type supported by gallery.
        * @param aContentType The mime type
        * @return True/False.
        */
        static TBool IsGallerySupported(const TDesC8& aContentType);

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CDownloadUtils();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // file operations
    
        /**
        * Find the content type from file using content recognizer
        * @param aDownload The actual download.
        * @param aContentType The mime type
        */
		void FindContentTypeFromFileL( RHttpDownload& aDownload, TUint8*& aContentTypeString );    
        
    private: // Data
        HBufC8* iMimeType; ///< Temp storage. Owned.
        HBufC8* iContentURI; ///< Temp storage. Owned.

    };

#endif /* CDOWNLOADUTILS_H */
