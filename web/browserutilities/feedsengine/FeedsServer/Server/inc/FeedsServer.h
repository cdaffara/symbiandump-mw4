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
* Description:  The FeedsServer main server class.
*
*/


#ifndef FEEDS_SERVER_H
#define FEEDS_SERVER_H


//  INCLUDES
#include <e32base.h>

#include "LeakTracker.h"
#include "UpdateAllFeedsTask.h"
#include "etel3rdparty.h"

// CONSTANTS
const TInt KDefaultFolderListId = 0;
const TInt KAllFolderListId = -1;
const TInt KNoFolderListId = -2;

// DATA TYPES
enum TFeedsServerPanic
    {
    EFeedsServerBadRequest,
    EFeedsServerNotImplementedYet
    };

// FORWARD DECLARATIONS
class CFeedsDatabase;
class CFeedHandler;
class CBackgroundUpdater;
class CHttpConnection;
class CServerHttpConnection;
class COpmlParser;
class CPackedFolder;
class CXmlUtils;
class RFile; 
class CBackRestoreHandler;
class CUpdateManager;

// CLASS DECLARATION


/**
*  The FeedsServer main server class.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
NONSHARABLE_CLASS(CFeedsServer): public CServer2
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CFeedsServer* NewL();

        /**
        * Destructor.
        */
        ~CFeedsServer();


    private: // Methods from CServer2
        /**
        * Creates a new session to client.
        *
        * @since ?
        * @param aVersion the version.
        * @param aMessage the message.
        * return A newly created session.
        */
        virtual CSession2* NewSessionL(const TVersion& aVersion, 
                const RMessage2& aMessage) const;
        

    public:  // New methods
        /**
        * Runs the server.
        *
        * @since 3.1
        * return The exit code.
        */
        static void RunServerL();

        /**
        * Returns the server's database instance;
        *
        * @since 3.1
        * @return The database.
        */
        inline CFeedsDatabase& Database() {return *iFeedsDatabase;}
        
        /**
        * Returns the server's feed-handler instance;
        *
        * @since 3.1
        * @return The database.
        */
        inline CFeedHandler& FeedHandler() {return *iFeedHandler;}
        
        /**
        * Returns the server's folder-handler instance;
        *
        * @since 3.1
        * @return The database.
        */
        inline COpmlParser& FolderHandler() {return *iFolderHandler;}
        
        /**
        * Returns the server's xml-utils instance;
        *
        * @since 3.1
        * @return The database.
        */
        inline CXmlUtils& XmlUtils() {return *iXmlUtils;}     

        /**
        * Sets AutoUpdate related settings;
        *
        * @since 3.1
        * @param aFolderListId The folder list associate with this background update.
        * @param aAutoUpdate The flag.
        * @param aAutoUpdateFreq The frequency of this background update.
        * @param aAutoUpdateAP The access point to use for this background update.
        * @param aAutoUpdateWhileRoaming The flag.
        * @return void.
        */
        void SetAutoUpdateSettingsL( TInt aFolderListId, TBool aAutoUpdate, TInt aAutoUpdateFreq, TUint32 aAutoUpdateAP, TBool aAutoUpdateWhileRoaming ) ;

        /**
        * Returns If the update all task associated with the folderListId is running or not;
        *
        * @since 3.2
        * @return If the update all task associated with the folderListId is running or not.
        */
        TBool UpdateAllFeedsTaskRunning( TInt aFolderListId );

        /**
        * Schedule the update manager for all folder list ID.
        *
        * @since 7.1
        * @return Void.
        */
        void ScheduleUpdateManagerL();

        /**
        * Schedule the update manager for all folder list ID.
        *
        * @param aFolderListId The folder list associate with this update manager.
        * @since 7.1
        * @return Void.
        */
        void ScheduleUpdateManagerL(TInt aFolderListId);

        /**
        * Schedule the update manager all task for one folder list ID.
        *
        * @since 7.1
        * @param aFolderListId The folder list associate with this update manager.
        * @param aAutoUpdate The flag.
        * @param aAutoUpdateFreq The frequency of this update manager.
        * @param aAutoUpdateAP The access point to use for this update manager.
        * @param aAutoUpdateWhileRoaming The flag.
        * @return Void.
        */
        void ScheduleUpdateManagerL( TInt aFolderListId, TBool aAutoUpdate, TInt aAutoUpdateFreq, TUint32 aAutoUpdateAP, TBool aAutoUpdateWhileRoaming );

        /**
        * Creates a new instance of XmlUtils, FeedHandler and FolderHandler
        *
        * @since 7.x
        * @return void
        */
        void ResetXmlUtilsL();

    private:  // New methods
        /**
        * C++ default constructor.
        */
        CFeedsServer(TInt aPriority);

        /**
        * Two-Phase constructor
        */
        void ConstructL();

        /**
        * Utility to panic the client.
        *
        * @since 3.1
        * @param aMessage the message that caused the panic.
        * @param aPanic The panic.
        * @return Void
        */
        static void PanicClient(const RMessage2& aMessage, TInt aPanic);

        /**
        * Utility to panic the server.
        *
        * @since 3.1
        * @param aPanic The panic.
        * @return Void
        */
        static void PanicServer(TInt aPanic);

        /**
        * Ensures the server is ready for processing requests.
        *
        * @since 3.1
        * @return void.
        */
        void WakeupServerL();

        /**
        * A helper method to ensure the server is fully started.
        *
        * @since 3.1
        * @return void.
        */
        void WakeupServerHelperL();

        /**
        * Put the server in a resource reduced mode.
        *
        * @since 3.1
        * @return void.
        */
        void SleepServer();
        
        /**
        * Notify the open sessions that the given folder-list changed.
        *
        * @since 3.1
        * @param aFolderListId The folder list's id.
        * @return void.
        */
        void NotifyFolderListChanged(TInt aFolderListId);
        
        /**
        * Notify the open sessions that the settings of given folder-list changed.
        *
        * @since 3.2
        * @param aFolderListId The folder list's id.
        * @return void.
        */
        void NotifySettingsChanged(TInt aFolderListId);

#if 0
        /**
        * A method used to purge old feed-items in the background every so often.
        *
        * @since 3.1
        * @param aPtr The this pointer.
        * @return General errors.
        */
        static TInt PurgeOldItems(TAny *aPtr);

        /**
        * A method used to purge old feed-items in the background every so often.
        *
        * @since 3.1
        * @return void.
        */
        void PurgeOldItemsL();
#endif

        /**
        * Loads and parsers the folder of feeds at the given path.
        *
        * @since 3.1
        * @param aFile The file handle to the file to import.
        * @return The resulting packed folder.
        */
        CPackedFolder* ImportFolderL(const RFile aFile, const TDesC& aOPMLFileName);

        /**
        * Export folder\feed entries.
        *
        * @since 3.2
        * @param aEntries - List of files\folders.
        * @return void.
        */
        void ExportFolderL(RArray<TInt> &aEntries, const TDesC& aExportFileName);

        /**
        * If need be copy default folder file from ROM.
        *
        * @since 3.1
        * @return void.
        */
        void EnsureDefaultFolderFileL();
        
        /**
        * Called from CFeedsServerSession when a session is opened.
        *
        * @since 3.1
        * @return Void.
        */
    	void SessionOpened();

        /**
        * Called from CFeedsServerSession when a session is closed.
        *
        * @since 3.1
        * @return Void.
        */
    	void SessionClosed();

        /**
        * Checks whether update is happening.
        *
        * @since 7.1
        * @return Bool.
        */
        TBool CheckForUpdate();

    public:

        /**
        * Updates the Feed in UpdateManager.
        *
        * @since 7.1
        * @return void.
        */
        void UpdateFeedL(TInt aFolderListId, TInt aFeedId, TBool aDeleteFeed);

    private:  // Data
        TLeakTracker          iLeakTracker;
        
        TBool                 iIsSleeping;
        
        CFeedsDatabase*             iFeedsDatabase;
        CFeedHandler*               iFeedHandler;
        COpmlParser*                iFolderHandler;
        CXmlUtils*                  iXmlUtils;

        TInt                  iSessionCount;
        CBackRestoreHandler *iBackRestoreHandler;
        RPointerArray<CUpdateManager> iUpdateManagerList; 


    private: // Friend class
        friend class CFeedsServerSession;
        friend class CImportFeedsTask;
        friend class CBackRestoreHandler;
        friend class CUpdateManager;
    };



#endif      // FEEDS_SERVER_H

// End of File
