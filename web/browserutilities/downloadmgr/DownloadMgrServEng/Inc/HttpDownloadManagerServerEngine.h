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
* Description:  
*       This class implements download manager.
*
*/



#ifndef HTTPDOWNLOADMANAGER_H
#define HTTPDOWNLOADMANAGER_H

//  INCLUDES
#include "HttpDownload.h"

#include <HttpDownloadMgrCommon.h>
#include <e32std.h>
#include <f32file.h>

#include <e32property.h>
#include <UikonInternalPSKeys.h>

// CONSTANTS
const TInt KMaxDriveListStrLen = KMaxDrives << 1;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
#if defined(_DEBUG)
void DMPanic( TInt aPanicCode );
#else
#define DMPanic( a )
#endif

// FORWARD DECLARATIONS
class CHttpDownload;
class CHttpClientApp;
class CHttpClientAppInstance;
class RFs;
class CDocumentHandler;

// CLASS DECLARATION

NONSHARABLE_CLASS( MMMCObserver )
    {
    public:
    
        virtual void MMCModeChanged( TBool aAllowed ) = 0;
    };
    
NONSHARABLE_CLASS( CMMCNotifier ) : public CActive
	{
	public:
	
		/**
		* constructor
		*/
		CMMCNotifier( RProperty* aProperty );
		
		/**
		* Destructor
		*/
		virtual ~CMMCNotifier();
		
		void StartObserving( MMMCObserver* aObserver );
		
	public: // from CActive
	
	    virtual void RunL();
	    virtual void DoCancel();
	
	private:
	
		RProperty* iProperty;
		MMMCObserver* iObserver;
	};

typedef struct
    {
    TInt    iDriveLetter;
    TInt    iDriveId;
    TBool   iRemovable;
    }TDriveStr;
    
/**
*  This class maintains the list of all existing downloads and applications
*  they initiated from.
*
*  @lib DManager.lib
*  @since S60 2.8
*/
NONSHARABLE_CLASS( CHttpDownloadManagerServerEngine ) : public CBase
                                                       ,public MMMCObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CHttpDownloadManagerServerEngine* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CHttpDownloadManagerServerEngine();

    public: // New functions
        
        /**
        * Closes the engine.
        * @since Series 60 v2.8
        * @param aStore if ETrue 
        * @return none.
        */
        IMPORT_C void CloseEngine( TBool aStore = ETrue );

        /**
        * Creates a new client application instance
        * @since Series 60 v2.8
        * @param aAppUid UId of the client application
        * @param aObserver download state observer
        * @param aGetAllPendingDownloads if ETrue this instance gets all non-attached downloads
        * @return pointer to the created instance. Ownership now passed.
        */
        IMPORT_C CHttpClientAppInstance* CreateNewClientAppInstanceL( 
                                        TUint32 aAppUid, 
                                        MDownloadStateObserver* aObserver,
                                        TBool aGetAllPendingDownloads );

        /**
        * Closes the given client application instance.
        * @since Series 60 v2.8
        * @param aAppInst client instance to be closed
        * @return none.
        */
        IMPORT_C void CloseClientInstance( CHttpClientAppInstance* aAppInst );

        /**
        * Returns the number of active download in the engine. 
        * @since Series 60 v2.8
        * @param ?arg1 ?description
        * @return ?description
        */
        IMPORT_C TInt ActiveDownloads() const;

        /**
        * Calculates the total size of all on-going downloads in the 
        * Download manager. Downloads which have been completed are not
        * taken to be part of this size.
        * @since Series 60 v3.2
        * @return TInt : Total size of all downloads currently under progress. 
        */
		TInt AllDownloadsSizeL( const CHttpDownload *aDownload ) const;


        /**
        * Calculates the total size of all on-going downloads in the current 
        * drive (where current download is in progress)
        * @since Series 60 v3.2
        * @return TInt : Total size of all downloads currently under progress in the drive. 
        */
		TInt AllDownloadsSizeInDriveL( const CHttpDownload *aDownload, TInt aDriveId ) const;

        /**
        * Search for download identified with id in every 
        * client application and check if download with the
        * aDownloadId has been attached already. It is not allowed to
		* attach the same download to more than one application.
        * @since Series 60 v3.2
        * @param aDownload id of the download to be search for
        * @return Bool ETrue - if the download has been attached already. 
        */
		TBool IsAttachedAlready(TInt32 aDownloadId);

        /**
        * Search for download identified with id in every 
        * client application.
        *
        * @since Series 60 v3.2
        * @param aDownload id of the download to be found
        * @return download identified with id
        */
        CHttpDownload* FindDownload( TInt32 aDownloadId );

        /**
        * Returns the client specific folder.
        * @since Series 60 v2.8
        * @param aClientApp client application
        * @param aFolder private folder of the client application
        * @return none.
        */
        void ClientAppFolder( const CHttpClientApp* aClientApp, 
                              TDes& aFolder );

        /**
        * Returns the client specific folder w/o the drive letter.
        * @since Series 60 v2.8
        * @param aClientApp client application
        * @param aFolder private folder of the client application
        * @return none.
        */
        void ClientAppFolderWithoutDriveLetter( const CHttpClientApp* aClientApp, 
                                                TDes& aFolder );

        /**
        * Returns the folder name of the download info files.
        * @since Series 60 v2.8
        * @param aDownload download
        * @param aFolder private folder of the download
        * @return none.
        */
        void DownloadInfoFolder( const CHttpClientApp* aClientApp, 
                                 TDes& aFolder ) const;

        /**
        * Returns the folder name of the download info files.
        * @since Series 60 v2.8
        * @param aDownload download
        * @param aFolder private folder of the download
        * @return none.
        */
        void CODDownloadInfoFolder( const CHttpClientApp* aClientApp, 
                                 TDes& aFolder ) const;
                                 
        /**
        * Returns the folder name of the download content files.
        * @since Series 60 v2.8
        * @param aDownload download
        * @param aFolder private folder of the download
        * @return none.
        */
        void DownloadContentFolder( const CHttpClientApp* aClientApp, 
                                    TDes& aFolder ) const;

        /**
        * Return the clients app's COD folder
        */
        void CodFolder( const CHttpClientApp* aClientApp, 
                        TDes& aFolder ) const;

#ifdef DEAD_CODE        
        /**
        * Returns every download maintained in the engine.
        * @since Series 60 v2.8
        * @return array of every download in the engine.
        */
        CArrayPtrFlat<CHttpDownload>* AllDownloadsL() const;
#endif

        /**
        * Returns the engine's own RFs
        * @since Series 60 v2.8
        * @return reference to the engine's own RFs
        */
        RFs& Fs();

        /**
        * Returns a connected socket server.
        * @since Series 60 v2.8
        * @return reference to the socket server
        */
        RSocketServ& SocketServ();

        /**
        * Returns a global unique download id
        * @since Series 60 v2.8
        * @return global unique download id
        */
        TInt NextDownloadId();
        
        /**
        * Returns the server's document handler
        * @return pointer to a document instance
        */
        CDocumentHandler* DocHandler() const;

        /**
        * Returns a Bool
        * @since Series 60 v3.2
        * @return Bool
        */
        TBool IsEngineClosing()const { return iEngineClosing; };

#ifdef RD_MULTIPLE_DRIVE
        /**
        * Query the dynamic drive list.
        */
        HBufC8* QueryDynDriveListLC();
#endif

        /**
        *
        */
        TBool ProgressiveDownloadFeature(){ return iFeatProgressiveDownload; }

    public: // Functions from base classes

        void MMCModeChanged( TBool aAllowed );

    protected:  // New functions
        
        /**
        * Creates a new client application.
        * @since Series 60 v2.8
        * @param aAppUid UId of the new client application
        * @return created client application. Ownership not passed.
        */
        CHttpClientApp* CreateNewClientAppL( TUint32 aAppUid );

        /**
        * Loads client from file. Not used. For later implemenations.
        * @since Series 60 v2.8
        * @return none. Leaves on failure.
        */
        void LoadClientsL();

        /**
        * Converts folder name to client id.
        * @since Series 60 v2.8
        * @param aClientDir folder of the client application
        * @return id of the client application
        */
        TUint32 CheckClientDirName( TDesC& aClientDir );

        /**
        * Set iMMCUniqueID with unique id of MMC card inserted into the phone.
        * KErrNotFound is not inserted.
        */
        void QueryMMCUniqueId();

        /**
        * Query the drive list from the CentRep.
        */
        void QueryDriveListL();

        /**
        * Returns iMMCUniqueID
        * @return iMMCUniqueID
        */
        TUint MMCUniqueId() const;
        
        /**
        *
        */
        TInt CheckFreeDiskSpaceL( HBufC* aFilename, TInt aLength );

    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CHttpDownloadManagerServerEngine();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data

        TBuf<KMaxDriveListStrLen> iDriveLettersCenRep;
        
    protected:  // Data

        CArrayPtrFlat<CHttpClientApp>* iClientApps;
        RFs                 iRfs;

		RProperty *iProperty;
		CMMCNotifier *iMMCNotifier;

        RSocketServ         iSocketServ;
        
        TInt        iNextDownloadId;

        // KErrNotFound is not present
        TUint iMMCUniqueID;
        
        CDocumentHandler*   iDocHandler;

        TBool               iEngineClosing;
        
        TBool               iFeatProgressiveDownload;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
         
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        // to provide access to client apps array
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // HTTPDOWNLOADMANAGER_H
            
// End of File
