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
*      Declaration of class CCodSaver.   
*      
*
*/


#ifndef COD_SAVER_H
#define COD_SAVER_H

// INCLUDE FILES

#include <e32base.h>
#include <apmstd.h>

// FORWARD DECLARATION

class MCodLoadObserver;
class CCodData;
class CAiwGenericParamList;

// CLASS DECLARATION

/**
* Content saver base class. Owns data in download-, install- and post-install
* phases.
* - Provides temp storage for data being downloaded.
* - Installs content from temp storage to proper place.
* - Provides cleanup in all phases (until content is Release()-d).
* This class is driven by CCodEngBase and methods are expected to be called
* in order.
*/
NONSHARABLE_CLASS( CCodSaver ): public CBase
    {
    public:     // Construct / destruct
    
        /**
        * Constructor.
        * @param aType Data type.
        */      
        inline CCodSaver( const TDesC8& aType );
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CCodSaver();

    public:     // new methods

        /**
        * Open temporary store for download.
        */
        virtual void OpenStoreL() = 0;

        /**
        * Append data to store.
        * @param aData Data to append.
        * @return Error code.
        */
        virtual TInt AppendData( const TDesC8& aData ) = 0;
        
        /**
        * Download finished, close temporary store.
        */
        virtual void CloseStore() = 0;

        /**
        * Check if attributes of received content match descriptor data.
        * Leaves with KErrCodAttributeMismatch in case of mismatch.
        * @param aData Data to match.
        */
        virtual void CheckResponseAttributesL( const CCodData& aData ) = 0;

        /**
        * Install downloaded content.
        * @param aStatus Completes when done.
        * @param aName Preferred name
        * @param aAttached ETrue if download is attached (playing progressively)
        */
        virtual void InstallL( TRequestStatus* aStatus, const TDesC& aName, const TBool aAttached ) = 0;
        

        /**
        * Perform buklk install for multiple downloaded media objects
        * @param aStatus Completes when done.
        * @param aData CODData object
        * @param aAttached ETrue if download is attached (playing progressively)
        */        
        virtual void BulkInstallL( TRequestStatus* aStatus, const CCodData &aData, const TBool aAttached ) = 0;
        
        /**
        * Cancel installation.
        */
        virtual void CancelInstall() = 0;

        /**
        * Release content (take ownership of content from saver).
        * @param aFname File name of content returned here.
        * @param aHandler Handler UID returned here.
        */
        virtual void ReleaseContent( TFileName& aFname, TUid& aHandler ) = 0;

        /**
        * Clean up.
        */
        virtual void Cleanup( TBool aDeleteFile ) = 0;
        
        /**
        * Release file name (take ownership of content from saver).
        * @param aFname File name of content returned here.
        */
        virtual void ResetL() = 0;
        
        /**
        * Get the downloaded file size.
        */
        virtual TInt DownloadedFileSize() = 0;
       
        /**
        * Release file name (take ownership of content from saver).
        * @param aFname File name of content returned here.
        */
        virtual void ReleaseFileName(TFileName& aFname) = 0;
        
        /**
        * Set progressive mode download
        * @param aValue ETrue if progressive mode
        */
        virtual void SetProgressiveMode( TBool /*aValue*/ ) {}
        
        
        /**
        * Called if download completed.
        * @since Series 60 v2.8
        * @param ?arg1 ?description
        * @return ?description
        */
        virtual void OnComplete(){}
        
        

    public:     // new methods

        /**
        * Get data type.
        * @return Data type.
        */
        inline const TDataType& DataType() const;

        /**
        * Get current data size.
        * @return Data size.
        */
        inline TInt DataSize() const;

        /**
        * Set source URI.
        * @param aSourceUri Source URI.
        */
        inline void SetSourceUriL( const TDesC8& aSourceUri );

        /**
        * Set or unset observer (pass NULL to unset).
        * Panics if already set; unsetting more is allowed.
        * @param aObserver Observer to set or NULL to unset.
        */
        void SetObserver( MCodLoadObserver* aObserver );

        /**
        * Set or unset parameters (pass NULL to unset).
        * @param aParams Parameters or NULL.
        */
        inline void SetParams( const CAiwGenericParamList* aParams );
        
        /**
        * Get Name 
        * @return Name of Saver
        */        
        virtual HBufC* NameL ( ) const { return NULL; }

    public:     // new methods

        /**
        * Set maximum size based on expected size. (Multiply with a safety
        * factor, to allow for transport formats, encodings, etc.)
        */
        void SetMaxSize( TInt aSize );

        /**
        * Check if appending new data chunk would go over the limit.
        * @param aDataChunkSize Data chunk size.
        * @return KErrCodAttributeMismatch if over the limit, or KErrNone.
        */
        TInt CheckMaxSize( TInt aDataChunkSize );
        
        /**
        * Creates a buffer having info to be stored.
        * @Added in OMA pause-resume feature
        */
        void AppendStorageInfoL(TPtr8&)const;        
        /**
        * Creates a unique download name 
        */
        void ConvertDownloadNameUniqueL( HBufC*& filePath,
                                         HBufC*& fileName,
                                         HBufC*& fileExtn );

        /**
        * Support function for ConvertDownloadNameUniqueL for creating a indexed unique name 
        */        
        void CreateIndexedNameL( HBufC*& aUniqueName, 
                                         TDesC& aOrgName, 
                                         TInt& aIndex );
        
        /**
        * Calls ConvertDownloadNameUniqueL to create unique name based on source file name
        * and destination path. 
        */
        void FindUniqueDestinationFileNameL( TDesC& srcFile, HBufC*& destPath );

    protected:  // types

        enum TState         ///< State (debug).
            {
            EInit,          ///< Initial state.
            EStoreOpen,     ///< Store open.
            EStoreClosed,   ///< Store closed.
            ERespChecked,   ///< Response attributes checked.
            EInstalling,    ///< Install in progress.
            EInstalled      ///< Install done.
            };

    protected:  // data

        MCodLoadObserver* iObserver;            ///< Observer or NULL. Not own.
        const CAiwGenericParamList* iParams;    ///< Params or NULL. Not own.
        HBufC8* iSourceUri;                     ///< Source URI. Owned.
        TDataType iType;                        ///< Data type.
        TInt iSize;                             ///< Data size.
        TInt iMaxSize;                          ///< Size limit (0 == off).
        TUid iHandler;                          ///< Handler app UID.
        TState iState;                          ///< State (debug).
        TFileName iFname;
    };

/**
* Saver factory.
*/
NONSHARABLE_CLASS( MCodSaverFactory )
    {
    public:     // new methods
    
        /**
        * Create saver object.
        * @param aType Data type.
        * @return Saver object for this data type. Ownership not passed.
        */
        virtual CCodSaver* CreateSaverL( const TDesC8& aType ) = 0;

    };

#include "CodSaver.inl"

#endif /* def COD_SAVER_H */
