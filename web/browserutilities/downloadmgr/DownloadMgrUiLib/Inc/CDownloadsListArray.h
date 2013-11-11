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
* Description:  Associate list box text array with array of downloads.
*
*/



#ifndef CDOWNLOADSLISTARRAY_H
#define CDOWNLOADSLISTARRAY_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <DownloadMgrClient.h>
#include <badesca.h>
#include <bamdesca.h>
#include <apgcli.h>


// MACRO DEFINITIONS
/** Mask download state in order more than one state can be passed in an 
    integer variable. EHttpDlCreated is the first enum. */
#define MASKED_DL_STATE( aState ) ( 0x01<<(TInt)((TInt)aState-(TInt)EHttpDlCreated) )

// CONSTANTS
LOCAL_D const TInt KMaxDownloadItemTextPartLength = 64; // Be careful when using this!

// FORWARD DECLARATIONS
class RApaLsSession;
class CCoeEnv;
class CGulIcon;
class CProgressInfoCreator;
class CFbsBitmap;
class CApaMaskedBitmap;

// CLASS DECLARATION

NONSHARABLE_CLASS( TDownloadUiData )
    {
    public:
        TDownloadUiData();
    public:
        TBuf<KMaxPath> iName;
        TBuf<KMaxPath> iDestFileName;
        TBuf8<KMaxContentTypeLength> iContentType;
        TInt32 iDownloadState;
        TInt32 iFullSize;
        TInt32 iDownloadedSize;
        TInt   iIconIndex;
        TBool  iPausable;
        TBool  iIsOnExternalMemory;
		TInt32 iProgressState;
		TInt32 iNumMediaObjects;
		TInt32 iActiveMoIndex;
    };
/// Array of UI data T objects.
typedef CArrayFixFlat< TDownloadUiData > CDownloadUiDataArray;


/**
*  Associate (list box) text array with array of downloads (pointers to downloads).
*/
NONSHARABLE_CLASS( CDownloadsListArray ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDownloadsListArray* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CDownloadsListArray();

    public: // New functions

        // *********** Getters

        /// Get reference to the text array
        inline MDesCArray& ItemTextArray() const;
        /// Get reference to the icon array
        inline CArrayPtr<CGulIcon>& IconArray() const;
        /// Get reference to the download array
        inline CDownloadArray& DownloadArray() const;
        /// Array count
        inline TInt Count() const;
        /// Get pointer to the formatted list text at aIndex
        inline TPtrC Text( TInt aIndex ) const;
        /// Get reference to the download at aIndex
        inline RHttpDownload& Download( TInt aIndex ) const;
        /// Get reference to the download data data structure at aIndex
        inline TDownloadUiData& DlUiData( TInt aIndex ) const;

        // *********** Setters: append, update and delete

        /// Append icon. The item index is returned. Ownership taken.
        TInt AppendL( CGulIcon* aIcon );
        /// Append data & download. The download cannot be in the list, 
        TInt AppendL( const TDownloadUiData& aDownloadUiData, 
                      const RHttpDownload& aDownload );
        /// Update only the data at aIndex
        void UpdateL( TInt aIndex, const TDownloadUiData& aDownloadUiData );
        /// Update only the data at aIndex
        void UpdateIfNeededL( 
            TInt aIndex, 
            const TDownloadUiData& aDownloadUiData, 
            TBool& aUpdated );
        /// Delete association item at aIndex.
        void Delete( TInt aIndex );
        /// Ownership type change for icon array.
        void SetIconArrayOwnedExternally( TBool aOwnedExt );

        /**
        * Handler application icon handling
        * @return The index of the inserted icon.
        */
        TInt AddHandlerAppIconL( const TDesC8& aContentType );

        // *********** Search utility

        inline TInt Find( const RHttpDownload& aDownload, TInt& aIndex ) const;

        /**
        * Return the number of downloads in the list in the given states.
        * @param aDownloadStates Mask of download stated interested.
        * @return The count.
        */
        TInt DownloadsCount( TUint16 aDownloadStates );

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CDownloadsListArray();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:

        /**
        * Construct the list box item text from the T object.
        */
        TPtrC LBTextFromUiData( const TDownloadUiData& aDownloadUiData );

        /**
        * The length of list box item text from the T object.
        */
        TInt LBTextLength( const TDownloadUiData& aDownloadUiData );

        /**
        * Useful default icon handling
        */
        void AddDefaultIconsL();

        /**
        * Find the UID of the application which is registered for aContentType.
        * @return ETrue if the app was found. EFalse otherwise.
        */
        TBool FindHandlerApp( const TDesC8& aContentType, TUid& aAppUid ) const;

        /**
        * Load the application icon. The application is identified by aAppUid.
        * This method puts both aBitmap and aMask in the cleanup stack.
        */
        void LoadAppIconLC( TUid aAppUid, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );
        
        TBool IsSupportedL(const TDownloadUiData&);
                  

    private: // Data

        RApaLsSession iApaLsSess; ///< Owned.
        CCoeEnv& iCoeEnv; ///< Reference to CONE environment.
        
        HBufC* iMbmResourceFileName; ///< UiLib's mbm full file name. Owned.

        // iDownloadArray, iUiDataArray, iListBoxTextArray has one-to-one correlation.
        CDownloadArray*       iDownloadArray; ///< Owned.
        CDownloadUiDataArray* iUiDataArray; ///< Owned.
        CDesCArrayFlat*       iListBoxTextArray; ///< Owned.

        CArrayPtr<CGulIcon>* iListBoxIconArray; ///< Owned/not owned. See iOwnedExt.
        TBool iOwnedExt; ///< Is iListBoxIconArray owned externally?

        CProgressInfoCreator* iProgressInfoCreator; ///< Utility. Owned.
        HBufC* iNewItemText; ///< Temp storage for LBTextFromUiData(). Owned.
        TBuf<KMaxDownloadItemTextPartLength> iTempBuf; ///< Temp storage.
        /** Temp storage. Should be long enough to hold R_DMUL_DOWNLOAD_COMPLETE 
            localized string or the progress info (max 14). */
        TBuf<KMaxDownloadItemTextPartLength> iProgressInfoRes;
        TInt iPausedIconIndex; ///< Default icon index in list box model
        TInt iExternalMemoryIconIndex; ///< Default icon index in list box model
        HBufC* iCompletedString; ///< Owned.
        HBufC* iSavedToGalleryString; ///< Owned.
        

        HBufC* iSavedToDownloadsFolderString;
        
        HBufC* iFileSavedString;
        HBufC* iFilesSavedString;

        HBufC* iNullDesC; ///< Owned.

        CApaMaskedBitmap* iApaMaskedBitmap; ///< Temp storage. Owned.
        
        TBool                       iPlatformSupportsGallery;
        
        
     
    };


// INLINE FUNCTIONS

inline MDesCArray& CDownloadsListArray::ItemTextArray() const
    {
    return (*iListBoxTextArray);
    }

inline CArrayPtr<CGulIcon>& CDownloadsListArray::IconArray() const
    {
    return (*iListBoxIconArray);
    }

inline CDownloadArray& CDownloadsListArray::DownloadArray() const
    {
    return (*iDownloadArray);
    }

inline TInt CDownloadsListArray::Count() const
    {
    return (iListBoxTextArray->Count());
    }

inline TPtrC CDownloadsListArray::Text( TInt aIndex ) const
    {
    return (iListBoxTextArray->MdcaPoint(aIndex));
    }

// No index boundary check - using operator []!
inline RHttpDownload& CDownloadsListArray::Download( TInt aIndex ) const
    {
    return *(*iDownloadArray)[aIndex];
    }

// No index boundary check - using operator []!
inline TDownloadUiData& CDownloadsListArray::DlUiData( TInt aIndex ) const
    {
    return (*iUiDataArray)[aIndex];
    }

inline TInt CDownloadsListArray::Find
    ( const RHttpDownload& aDownload, TInt& aIndex ) const
    {
    TInt count = Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( (*iDownloadArray)[i] == &aDownload )
            {
            aIndex = i;
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

#endif /* CDOWNLOADSLISTARRAY_H */