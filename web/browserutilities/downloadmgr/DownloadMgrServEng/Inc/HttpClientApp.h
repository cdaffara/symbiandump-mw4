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
* Description:  ?Description
*
*/



#ifndef HTTPCLIENTAPP_H
#define HTTPCLIENTAPP_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
_LIT( KInfoFilesDirName, "downloads" );
_LIT( KContentsDirName, "contents" );
_LIT( KCodsDirName, "cod" );
_LIT( KInfoFilesCodDirName, "codinfo" );

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class CHttpDownloadManagerServerEngine;
class CHttpConnHandler;
class CHttpDownload;
class MDownloadStateObserver;
class CHttpClientAppInstance;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 v2.8
*/
NONSHARABLE_CLASS( CHttpClientApp ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHttpClientApp* NewL( TUint32 aAppUid, 
                                     CHttpDownloadManagerServerEngine* aEngine );
        
        /**
        * Destructor.
        */
        virtual ~CHttpClientApp();

    public: // New functions
        
         /**
        * Removes the given download from downloads array.
        * This download is deleted!
        * @since Series 60 v2.8
        * @param aDownload download to be unregistered
        * @return none.
        */
        IMPORT_C void UnregisterDownload( CHttpDownload* aDownload );

        /**
        * Creates a new client instances.
        * @since Series 60 v2.8
        * @param aObserver pointer to a download state observer. Not owned.
        * @param aGetAllPendingDownloads this client instance gets all non-attached downloads.
        * @return created client instance.
        */
        CHttpClientAppInstance* CreateNewInstanceL( MDownloadStateObserver* aObserver,
                                                    TBool aGetAllPendingDownloads );

        /**
        * Closes the given client instance.
        * @since Series 60 v2.8
        * @param aInstance client instance to be closed
        * @return none.
        */
        void CloseInstance( CHttpClientAppInstance* aInstance );

        /**
        * Closes the entire client app with its instances and downloads
        * @since Series 60 v2.8
        * @param bStore if ETrue the current state is stored.
        * @return none.
        */
        void CloseClientApp( TBool bStore = ETrue );

        /**
        * Creates a new download.
        * @since Series 60 v2.8
        * @param aClAppInstance new download attached to this client instance
        * @param aUrl URL of the new download
        * @return pointer to the new download. Ownership is not passed.
        */
        CHttpDownload* CreateNewDownloadL( CHttpClientAppInstance* aClAppInstance,
                                           const TDesC8& aUrl );

        /**
        * Returns the client applications's UId.
        * @since Series 60 v2.8
        * @return UId of the client application
        */
        TUint32 AppUid() const;

        /**
        * Returns the array of the instance of the client application
        * @since Series 60 v2.8
        * @return array of the instances. Ownership is not passed.
        */
        CArrayPtrFlat<CHttpClientAppInstance>* Instances() const;

        /**
        * Returns the array of the all the downloads within this client
        * @since Series 60 v2.8
        * @return array of all the downloads within this client. Ownership is not passed.
        */
        CArrayPtrFlat<CHttpDownload>* Downloads() const;

        /**
        * Returns the engine where this client applications is created.
        * @since Series 60 v2.8
        * @return pointer to the engine. Ownership is not passed.
        */
        CHttpDownloadManagerServerEngine* Engine() const;

        /**
        * Registers the given download in the downloads array
        * @since Series 60 v2.8
        * @param aDownload download to be registered
        * @return none. Leaves on error.
        */
        void RegisterDownloadL( CHttpDownload* aDownload );

       
        /**
        * Returns the number of downloads attached to the master
        * instance.
        * @since Series 60 v2.8
        * @param aNoMediasOnly only downloads of which EDlAttrNoMedia attribute is ETrue
        * @return number of downloads attached to the master instance
        */
        TInt32 DownloadsInMaster( TBool aNoMediasOnly );

        /**
        * This connhandler is not assign to any client instance
        * and already disconnected. To avoid memory leak it
        * has to be destroyed.
        * @since Series 60 v2.8
        * @param aConnHandler ConnHandler to be deleted
        * @return none
        */
        void DestroyConnHandler( CHttpConnHandler* aConnHandler );
        
        /**
        * Loads info of this client application.
        * @since Series 60 v2.8
        * @param aFolder Folder where the info file stored
        * @return none. Leaves on error.
        */
        void LoadClientInfoL( const TDesC& aFolder );

    public: // Functions from base classes
    protected:  // New functions
        /**
        * Stores info of this client application.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void StoreClientInfoL();

        /**
        * Load all downloads from disk.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void LoadDownloadsL();

        /**
        * Converts filename to download id.
        * @since Series 60 v2.8
        * @param aFilename download filename
        * @return id of the download. Leaves on error.
        */
        TInt32 DownloadIdL( const TDesC& aFilename ) const;

    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CHttpClientApp( TUint32 aAppUid, 
                        CHttpDownloadManagerServerEngine* aEngine );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CHttpClientApp( const CHttpClientApp& );
        // Prohibit assigment operator if not deriving from CBase.
        // CHttpClientApp& operator=( const CHttpClientApp& );
        
        /**
        * Checks whether the downlod is already loaded or not.
        * @returns whether the downlod is already loaded or not
        */
        TBool IsDownloadAlreadyLoaded( TInt32 aId ) const;

    public:     // Data

        // ?one_line_short_description_of_data
    
    protected:  // Data

        // ?one_line_short_description_of_data
        TUint32 iAppUid;
        CHttpDownloadManagerServerEngine* iEngine;          // not owned
        CArrayPtrFlat<CHttpClientAppInstance>* iInstances;  // owner
        CArrayPtrFlat<CHttpDownload>* iDownloads;           // owner
        CArrayPtrFlat<CHttpConnHandler>* iConnections;      // owned

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
         
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // HTTPCLIENTAPP_H
            
// End of File
