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
* Description:  Client application preferencies container class
*
*/



#ifndef HTTPCLIENTAPPINSTANCE_H
#define HTTPCLIENTAPPINSTANCE_H

//  INCLUDES
#include "HttpDownloadMgrCommon.h"

#include <e32base.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

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
class CHttpClientApp;
class CHttpConnHandler;

// CLASS DECLARATION

// CLASS DECLARATION
class MDownloadStateObserver
    {
    public:

        virtual void Event( CHttpDownload* aDownload, 
                            THttpDownloadState aDlState,
                            THttpProgressState aProgState,
                            TInt32 aMoIndex ) = 0;
    };

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS( CHttpClientAppInstance ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHttpClientAppInstance* NewL( CHttpClientApp* aClientApp,
                                             CHttpConnHandler* aConnHandler,
                                             TBool aMaster,
                                             TInt aInstanceId = 0,
                                             MDownloadStateObserver* aObserver = NULL );
        
        /**
        * Destructor.
        */
        virtual ~CHttpClientAppInstance();

    public: // New functions

        /**
        * This function is only for internal test usage.
        * Do NOT use it!!!
        * @since Series 60 2.8
        * @param aObserver download state observer
        */
        IMPORT_C void SetObserver( MDownloadStateObserver* aObserver );

        /**
        * Search for a download that is already registered with the given URL.
        * If found this dowload object is return. NULL anyway.
        * @since Series 60 2.8
        * @param aUrl URL to be searched for
        * @return download object registered with this URL or NULL.
        */
        IMPORT_C CHttpDownload* SearchDownloadWithUrl( const TDesC8& aUrl );

        /**
        * Creates a new download with the given URL.
        * 
        * @since Series 60 2.8
        * @param aURL URL address of this download
        * @return a new created download object
        */
        IMPORT_C CHttpDownload* CreateNewDownloadL( const TDesC8& aUrl );

        /**
        * Get pointer to a download object that is already created,
        * and identified with the id.
        * 
        * @since Series 60 3.2
        * @param aDownloadId id of the download attached to
        * @return download object identified with the id
        */
        IMPORT_C CHttpDownload* AttachDownloadL( TInt32 aDownloadId );

        /**
        * Returns array of the downloads owned by this client
        * instance.
        * Ownership is passed to the caller.
        * @since Series S60 2.8
        * @return pointer to an array of the downloads owned by this client instance
        */
        IMPORT_C CArrayPtrFlat<CHttpDownload>* DownloadsL() const;

        /**
        * Returns the UId of this application
        * @since Series 60 2.8
        * @return UId of this application
        */
        IMPORT_C TUint32 AppUid() const;

        /**
		* Gets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TInt attribute.
        * @return void.
		*/
		IMPORT_C void GetIntAttributeL( THttpDownloadMgrAttrib aAttribute, 
									    TInt32& aValue );

        /**
		* Gets the value for a TBool attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TBool attribute.
        * @return pointer to the string attribute.
		*/
		IMPORT_C void GetBoolAttributeL( THttpDownloadMgrAttrib aAttribute, 
                                         TBool& aValue );

		/**
		* Gets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aDelete ETrue if returned buffer has to be deleted.
        * @return pointer to the string attribute.
		*/
		IMPORT_C HBufC* GetStringAttributeL( THttpDownloadMgrAttrib aAttribute, 
                                             TBool& aDelete );

		/**
		* Gets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aDelete ETrue if returned buffer has to be deleted.
        * @return void.
		*/
		IMPORT_C HBufC8* GetString8AttributeL( THttpDownloadMgrAttrib aAttribute, 
                                               TBool& aDelete );

        /**
		* Sets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return KErrNone if successful, a system-wide error code if not.
		*/
		IMPORT_C void SetIntAttributeL( THttpDownloadMgrAttrib aAttribute, 
                                        const TInt32 aValue );
		
		/**
		* Sets the value for a TBool attribute.
		* This method can be used to stop a connection (KConnectionStop).
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
		*/
		IMPORT_C void SetBoolAttributeL( THttpDownloadMgrAttrib aAttribute, 
                                         const TBool aValue );
		
		/**
		* Sets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The string value to be set. Must be a 16-bit descriptor!.
		*/
		IMPORT_C void SetStringAttributeL( THttpDownloadMgrAttrib aAttribute, 
                                           const TDesC16& aValue );

		/**
		* Sets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The string value to be set. Must be a 16-bit descriptor!.
		*/
		IMPORT_C void SetStringAttributeL( THttpDownloadMgrAttrib aAttribute, 
                                           const TDesC8& aValue );

        /**
        * Disconnects the instance's connection and pauses all downloads.
        * @since Series 60 2.8
        * @return none
        */
        IMPORT_C void Disconnect();
        
        /**
        * Returns the client application object.
        * @since Series 60 2.8
        * @return pointer to the client application object. Ownership not passed.
        */
        IMPORT_C CHttpClientApp* ClientApp() const;


        /**
        * Returns the id of this client instance
        * @since Series 60 2.8
        * @return id of this client instance
        */
        TInt InstanceId() const;

        
        /**
        * Returns the connection handler of this instance.
        * @since Series 60 2.8
        * @return pointer to the connection handler of this instance. Ownership not passed.
        */
        CHttpConnHandler* ConnHandler() const;

        /**
        * Returns the state observer of this instance.
        * @since Series 60 2.8
        * @return pointer to the state observer. Ownership not passed.
        */
        MDownloadStateObserver* Observer() const;

        /**
        * Return the exit action attribute.
        * @since Series 60 2.8
        * @return exit action.
        */
        THttpDownloadMgrExitAction ExitAction() const;

        /**
        * Returns master attribute.
        * @since Series 60 2.8
        * @return master attribute.
        */
        TBool Master() const;

        /**
        * Returns cookie usage setting.
        * @return ETrue if cookie usage is enabled.
        */
        TBool Cookies() const;

        /**
        * Returns auto connectn setting.
        * @return ETrue if auto-connect enabled.
        */
        TBool AutoConnect() const;

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CHttpClientAppInstance( CHttpClientApp* aClientApp,
                                CHttpConnHandler* aConnHandler,
                                TBool aMaster,
                                TInt aInstanceId,
                                MDownloadStateObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        CHttpClientApp* iClientApp; // not owned
        TInt    iInstanceId;    // Unique id of this instance 
                                // of the client application
        TBool   iSilentMode;    // See EDlMgrSilentMode
        TBool   iNotifications; // No user notification needed
        THttpDownloadMgrExitAction   iExitAction;   // See EDlMgrDeleteOnExit
        CHttpConnHandler* iConnHandler; // pointer to the CHttpConnHandler object
                                        // used by this instance. Not owned only
                                        // contained.
        
        MDownloadStateObserver* iObserver;  // now owner
        TBool   iMaster;        // Indicates that this instance inherits all
                                // downloads another instance with the same UID
                                // left in the download manager

        TBool   iCookies;       // EDlMgrEnableCookies
        TBool   iAutoConnect;   // EDlAutoConnect
        
        TBool   iFotaDownload;  // EDlMgrFotaDownload
        
        TBool iProgressiveDownload; // EDlMgrProgressiveDownload

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
    protected:  // Friend classes
        friend class CHttpClientApp;

    private:    // Friend classes
    };

#endif      // HTTPCLIENTAPPINSTANCE_H
            
// End of File
