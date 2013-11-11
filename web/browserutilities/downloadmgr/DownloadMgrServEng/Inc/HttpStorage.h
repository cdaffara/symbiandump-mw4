/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements storage functionality in the DownloadManager
*
*/



#ifndef CHTTPSTORAGE_H
#define CHTTPSTORAGE_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class CHttpDownload;
class RFotaEngineSession;
class RWriteStream;
class CDocumentHandler;
class CBuffStorage;


// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 v2.8
*/

NONSHARABLE_CLASS( CHttpStorage ) : public CBase
    {
    public:  // Constructors and destructor
        
        enum TStorageMethod
            {
            EStoreFile,
            EStoreFota
            };
    
        enum TFileCloseOperation
            {
            EKeepFile,
            EDeleteFile,
            EReplaceFile
            };
            
        /**
        * Two-phased constructor.
        */
        static CHttpStorage* NewL( CHttpDownload* aDownload );
        
        /**
        * Destructor.
        */
        virtual ~CHttpStorage();

    public: // New functions

        /**
        * Sets storage method. Cannot be set if already storing.
        * @param aMethod Storage method
        */
        void SetStorageMethod( TStorageMethod aMethod );
        
        /**
        * Checks if the content file is 
        * @since Series v2.8 Series60
        * @return ETrue if content file is valid
        */
        TBool CheckContentFileIntegrityL();

        /**
        * Creates the destination filename from URL
        * @since Series v2.8 Series60
        * @return none. Leaves on error.
        */
        void CreateDestinationFileL();

        /**
        * Closes destination file. Downloaded but not persisted
        * chunks remains in memory until ResetAndDestroy
        * @since Series v2.8 Series60
        * @return none
        */
        void CloseDestinationFile( TFileCloseOperation aOperation );

        /**
        * Persist received chunk
        * @since Series 60 v2.8
        * @param aBuf new chunk to be added
        * @return EFalse if content-length had to be updated.
        */
        TBool WriteOutNextBodyDataL( const TDesC8& aBuf );

        /**
        * Called when download is completed.
        * @since Series 60 v2.8
        * @return none
        */
        void OnComplete();

        /**
        * Reinitializes storage.
        * @since Series 60 v2.8
        * @param aOnExit if ETrue content files are not deleted
        * @return none.
        */
        void ResetAndDestroy();

        /**
        * Adopt file handle from client.
        */
        void AdoptFileHandleL( RFile* aFile );
        
        /**
        * @return ETrue if the download is progressive
        */
        TBool ProgressiveMode(){ return iProgressiveDownload; };
        
        /**
        * Set progressive mode download
        * @param aValue ETrue if progressive mode
        */
        void SetProgressiveMode( TBool aValue );

        /**
        * Updates destination file name
        * @param aFilename new filename
        * @param aUserSet filename set by client application
        * @return none. Leaves on error.
        */
        void UpdateDestinationFilenameL( const TDesC16& aFilename, TBool aUserSet );
                
        /**
        * Append storage info to buffer
        * @param aBuf buffer where storage info is to be appended
        */
        void AppendStorageInfoL( TPtr8& aBuf ) const;
        
        /**
        * Loads storage info from file
        * @param aOutFile open file where storage info is loaded from
        */
        void LoadStorageInfoL( RFile& aInFile );

        /**
        * Returns the destination drive's id.
        * @since Series v2.8 Series60
        * @return id of the destination file's drive
        */
        TUint GetDestinationDriveId();

        /**
        * Updates file extension
        */
        void UpdateExtensionL();

        /**
        * Set size of disk buffer
        * @param aBufferSize size in bytes
        */
        void SetBufferSizeL( TInt aBufferSize );

        /**
        * Turn on disk buffering
        */
        void EnableBufferingL();

        /**
        * Turn off disk buffering
        */
        void DisableBufferingL();

        /**
        * If buffering enabled, flush data to disk
        */
        void FlushL();
        
        /**
        *Returns iBufferingEnabled
        */
        inline TBool BufferingEnabled() { return iBufferingEnabled; };
      
        HBufC*  LocalFilename()const { return iLocalFilename; };
        HBufC*  DestFilename()const { return iDestFilename; };
        HBufC*  DdFileName()const { return iDdFilename; };
        TBool   DestFNameSet()const { return iDestFNameSet; };
        TBool   RemovableDest()const { return iRemovableDest; };
        TBool   ProgressiveDownload()const { return iProgressiveDownload; }
        TInt32  Length()const { return iLength; };
        TInt32  DownloadedSize()const { return iDownloadedSize; };
        TInt32  MoDownloadedSize()const{ return iMoDownloadedSize; };
        RFile*  File(){ return iFile; };
        
        void    SetLength( TInt32 aLength ){ iLength = aLength; };
        void    SetDownloadedSize( TInt32 aSize ){ iDownloadedSize = aSize; iBufferedSize = aSize; };
        void    SetRemovableDest( TBool aRemovable ){ iRemovableDest = aRemovable; };
        void    SetLocalFilenameL(const TDesC16& aValue);
        void    SetPartialContentLength( TInt32 aLength ){ iPartialLength = aLength; };
        void    SetMoDownloadedSize( TInt32 aMoSize ){ iMoDownloadedSize = aMoSize ;};
        TBool   RFileSetByClient()const { return iRFileSetByClient; };
        
    protected:  // New functions
        
        /**
        * Check if there's enough disk space for the content
        * Returns drive letter where content can be safely stored.
        * Leaves on error
        * @since Series v2.8 Series60
        * @return Returns drive letter where content can be safely stored.
        *         Leaves on error.
        */
        TInt CheckFreeDiskSpaceL();

        /**
        * Creates the destination content file.
        * @since Series v2.8 Series60
        * @return none. Leaves on error.
        */
        void CreateDestinationFilenameL();

    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CHttpStorage( CHttpDownload* aDownload );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
    
    protected:  // Data

        CHttpDownload* iDownload;       // Pointer to the download object that owns
                                        // this storage class
    private:    // Data

        CActiveSchedulerWait*   iWait;
        
        TBool   iRFileSetByClient;

#ifdef __SYNCML_DM_FOTA        
        RFotaEngineSession* iFotaEngine;
        RWriteStream*       iFotaStream;
#endif        

        TBool   iProgressiveDownload;   // EDlAttrProgressive
        
        TBool   iRemovableDest;
        HBufC*  iDestFilename;          // EDlAttrDestFilename
        TBool   iDestFNameSet;          // iDestFilename is set by client app -> 
                                        // do not delete it in Reset()
        RFile*  iFile;                  // destination file

        TInt32  iLength;                // EDlAttrLength
        TInt32  iDownloadedSize;        // How much data actually written to finally
        TInt32 iBufferedSize;           // How much data received over the air (but not necessarily all in file yet)
        TInt32  iMoDownloadedSize;      // EDlAttrMultipleMODownloadedSize
  
		
        HBufC*  iLocalFilename;         // Generated from URL if destination EDlAttrDestFilename
                                        // is not specified before start.
		HBufC*	iDdFilename;			// Name of DD File.
        TUint   iDriveId;               // Unique id of the drive where content is saved.

        TStorageMethod  iStorageMethod;

        // Reserved pointer for future extension
        //TAny* iReserved;

        HBufC8* iWriteBuff;             // for buffering download data - owned

        TBool   iBufferingEnabled;

        TInt    iBufferSize;            // disk io buffer size - bytes
        TInt32  iPartialLength;         //partial Content Length in case of server Response 206        
        CBuffStorage *iStorage;  
        HBufC*  iTempFilename;          // EDlAttrTempFilename
        
    public:     // Friend classes
    
        friend class CBuffStorage;
    

        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CHTTPSTORAGE_H   
            
// End of File
