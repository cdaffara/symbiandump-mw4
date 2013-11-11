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


#include "FeedsDatabase.h"
#include "FeedHandler.h"
#include "FeedsServer.h"
#include "FeedsServerMsg.h"
#include "FeedsServerSession.h"
#include "OpmlParser.h"
#include "OpmlWriter.h"
#include "PackedFolder.h"
#include "ServerHttpConnection.h"
#include "Logger.h"
#include "XMLUtils.h"
#include "BackRestoreHandler.h"
#include "UpdateManager.h" 

//  CONSTANTS
_LIT(KFeedsServerPanic, "FeedsServer");

//_LIT(KFileSchema, "file://c:");
_LIT(KDefaultFeed, "default_feeds.xml");

_LIT(KDefaultOPMLFileParam, "");


// -----------------------------------------------------------------------------
// CFeedsServer::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsServer* CFeedsServer::NewL()
    {
    CFeedsServer* self = new (ELeave) CFeedsServer(EPriorityNormal);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }


// -----------------------------------------------------------------------------
// CFeedsServer::NewL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeedsServer::ConstructL()
    {
		StartL(KFeedsServerName);
		iBackRestoreHandler = CBackRestoreHandler::NewL(this);
		iBackRestoreHandler->StartListening();
    }


// -----------------------------------------------------------------------------
// CFeedsServer::CFeedsServer
//
// C++ default constructor can NOT contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
CFeedsServer::CFeedsServer(TInt aPriority):
        CServer2(aPriority), iLeakTracker(CLeakTracker::EFeedsServer), iIsSleeping(ETrue)
    {
    }


// -----------------------------------------------------------------------------
// CFeedsServer::~CFeedsServer
//
// Destructor
// -----------------------------------------------------------------------------
//
CFeedsServer::~CFeedsServer()
    {
    SleepServer();    
    delete iBackRestoreHandler;
    iBackRestoreHandler = NULL;   
    }


// -----------------------------------------------------------------------------
// CFeedsServer::NewSessionL
//
// Create session(s) to client(s)
// -----------------------------------------------------------------------------
//
CSession2* CFeedsServer::NewSessionL(const TVersion& aVersion, 
        const RMessage2& /*aMessage*/) const
    {
    // check we're the right version
    TVersion srvVersion(KFeedsServerMajorVersionNumber,
                        KFeedsServerMinorVersionNumber,
                        KFeedsServerBuildVersionNumber);

    if (!User::QueryVersionSupported(srvVersion, aVersion))
        {
        User::Leave(KErrNotSupported);
        }

    // make new session
    return (CFeedsServerSession::NewL(*(const_cast<CFeedsServer*>(this))));
    }


// -----------------------------------------------------------------------------
// CFeedsServer::RunServerL
// 
// Runs the server.
// -----------------------------------------------------------------------------
//
void CFeedsServer::RunServerL()
	{
	CActiveScheduler*  scheduler = NULL;
	CFeedsServer*      server = NULL;

    // Naming the server thread after the server helps to debug panics.
	User::LeaveIfError(User::RenameThread(KFeedsServerName));

	// Create and install the active scheduler.
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create the server (leave it on the cleanup stack)
	server = CFeedsServer::NewL();
	CleanupStack::PushL(server);

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

	// Ready to run.  This method doesn't return until the server is shutdown.
	CActiveScheduler::Start();

	// Cleanup.
	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(scheduler);
	}

// -----------------------------------------------------------------------------
// CFeedsServer::PanicClient
//
// Utility to panic the client.
// -----------------------------------------------------------------------------
void CFeedsServer::PanicClient(const RMessage2& aMessage, TInt aPanic)
    {
    aMessage.Panic(KFeedsServerPanic, aPanic);
    }


// -----------------------------------------------------------------------------
// CFeedsServer::PanicServer
//
// Utility to panic the server.
// -----------------------------------------------------------------------------
void CFeedsServer::PanicServer(TInt aPanic)
    {
    User::Panic(KFeedsServerPanic, aPanic);
    }


// -----------------------------------------------------------------------------
// CFeedsServer::WakeupServerL
//
// Ensures the server is ready for processing requests.
// -----------------------------------------------------------------------------
//
void CFeedsServer::WakeupServerL()
    {
    if (iIsSleeping)
        {
        // Wake up the server.
        TRAPD(err, WakeupServerHelperL());
        
        // If the server didn't fully wake up then sleep it and re-leave the error.
        if (err != KErrNone)
            {
            SleepServer();
            User::Leave(err);
            }
        }
    }

// -----------------------------------------------------------------------------
// CFeedsServer::WakeupServerHelperL
//
// A helper method to ensure the server is fully started.
// -----------------------------------------------------------------------------
//
void CFeedsServer::WakeupServerHelperL()
    {
    TBool    created;

    if (iIsSleeping)
        {            
        // Create the shared xml utils instance.
        iXmlUtils = CXmlUtils::NewL();

        // Create the handlers.
        iFeedHandler = CFeedHandler::NewL(*iXmlUtils);
        iFolderHandler = COpmlParser::NewL(*iXmlUtils);

        // Create the database instance.
        iFeedsDatabase = CFeedsDatabase::NewL(this,created);
        
        // If the database was newly created then load the default feeds into the 
        // the database.
        if (created)
            {
            RFs                 rfs;
            TBuf<KMaxFileName>  path;
            RFile               file;
            CPackedFolder*      packedFolder = NULL;
            
            // Ensure that the default folder file is copied from ROM. 
            EnsureDefaultFolderFileL();
            
            // Get the path to the default folder.
            User::LeaveIfError(rfs.Connect());
            CleanupClosePushL(rfs);

            rfs.PrivatePath(path);
            path.Append(KDefaultFeed);
            User::LeaveIfError(file.Open(rfs, path, EFileRead | EFileShareReadersOnly));
            CleanupClosePushL(file);

            // Import the default folder file synchronously.
            packedFolder = ImportFolderL(file, KDefaultOPMLFileParam);
            
            CleanupStack::PopAndDestroy(/*file*/);
            CleanupStack::PopAndDestroy(/*rfs*/);

            CleanupStack::PushL(packedFolder);

            // Load it into the database.
            iFeedsDatabase->ImportFolderL(KDefaultFolderListId, *packedFolder); 
            CleanupStack::PopAndDestroy(packedFolder);
            
            // Compact the database.
            iFeedsDatabase->Compact();

            }

        ScheduleUpdateManagerL(); 
                
        iIsSleeping = EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CFeedsServer::SleepServer
//
// Put the server in a resource reduced mode.
// -----------------------------------------------------------------------------
//
void CFeedsServer::SleepServer()
    {
    iIsSleeping = ETrue;
    
    for( int i=0; i<iUpdateManagerList.Count(); i++ )
        {
        delete iUpdateManagerList[ i ];
        }
    iUpdateManagerList.Reset();

    delete iFeedHandler;
    iFeedHandler = NULL;
    
    delete iFolderHandler;
    iFolderHandler = NULL;
    
    delete iFeedsDatabase;
    iFeedsDatabase = NULL;
    
    delete iXmlUtils;
    iXmlUtils = NULL;
    }


// -----------------------------------------------------------------------------
// CFeedsServer::NotifyFolderListChanged
//
// Notify the open sessions that the given folder-list changed.
// -----------------------------------------------------------------------------
//
void CFeedsServer::NotifyFolderListChanged(TInt aFolderListId)
    {
    CSession2*  session = NULL;

    // Notify all of the open sessions.    
    iSessionIter.SetToFirst();
    while ((session = iSessionIter++) != NULL)
        {
        static_cast<CFeedsServerSession*>(session)->NotifyFolderListChanged(aFolderListId);
        }
    }


// -----------------------------------------------------------------------------
// CFeedsServer::NotifySettingsChanged
//
// Notify the open sessions that the given folder-list changed.
// -----------------------------------------------------------------------------
//
void CFeedsServer::NotifySettingsChanged(TInt aFolderListId)
    {
    CSession2*  session = NULL;

    // Notify all of the open sessions.    
    iSessionIter.SetToFirst();
    while ((session = iSessionIter++) != NULL)
        {
        static_cast<CFeedsServerSession*>(session)->NotifySettingsChanged(aFolderListId);
        }
    }
    
    
#if 0
// -----------------------------------------------------------------------------
// CFeedsServer::PurgeOldItems
//
// A method used to purge old feed-items in the background every so often.
// -----------------------------------------------------------------------------
//
TInt CFeedsServer::PurgeOldItems(TAny *aPtr)
    {
    CFeedsServer*  self = static_cast<CFeedsServer*>(aPtr);
    
    TRAPD(err, self->PurgeOldItemsL());    
    return err;
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsServer::PurgeOldItemsL
//
// A method used to purge old feed-items in the background every so often.
// -----------------------------------------------------------------------------
//
void CFeedsServer::PurgeOldItemsL()
    {
    WakeupServerL();

    // Purge any old items.
    TTime yesterday;
    
    yesterday.UniversalTime();
    yesterday -= TTimeIntervalDays(1);
    
    iFeedsDatabase->PurgeOldItemsL(yesterday);
    }
#endif
    
    
// -----------------------------------------------------------------------------
// CFeedsServer::ImportFolderL
//
// Loads and parsers the folder of feeds at the given path.
// -----------------------------------------------------------------------------
//
CPackedFolder* CFeedsServer::ImportFolderL(const RFile aFile, const TDesC& aOPMLFileName)
    {
    TInt            size;
    HBufC8*         buffer = NULL;
    TPtr8           bufferPtr(NULL, 0);
    CPackedFolder*  packedFolder = NULL;

    // Read file
    User::LeaveIfError(aFile.Size(size));

    buffer = HBufC8::NewLC(size);
    bufferPtr.Set(buffer->Des());

    User::LeaveIfError(aFile.Read(bufferPtr, size));

    // Parse the buffer
    packedFolder = iFolderHandler->ParseL(*buffer, KNullDesC, KNullDesC, aOPMLFileName);

    // Clean up.
    CleanupStack::PopAndDestroy(buffer);

    return packedFolder;
    }

void CFeedsServer::ExportFolderL(RArray<TInt> &aEntries, const TDesC& aExportFileName)
	{
    // Export folders to an OPML buffer
	OpmlWriter::ExportL(aEntries, aExportFileName);
	}

// -----------------------------------------------------------------------------
// CFeedsServer::EnsureDefaultFolderFileL
//
// If need be copy default folder file from ROM.
// -----------------------------------------------------------------------------
//
void CFeedsServer::EnsureDefaultFolderFileL()
    {
    TInt                err;
    RFs                 defaultRfs;
    TUint               attValue = 0;
    TBuf<KMaxFileName>  path;

    // Open a connection to the working drive.
    User::LeaveIfError(defaultRfs.Connect());
    CleanupClosePushL(defaultRfs);
    User::LeaveIfError(defaultRfs.SetSessionPath(_L("c:\\")));
    
    // Build the path to the file.
    defaultRfs.PrivatePath(path);
    path.Append(KDefaultFeed);

    // Test whether or not the folder file is present.
    err = defaultRfs.Att(path, attValue);

    // The file is there, just return.
    if (err == KErrNone)
        {
        CleanupStack::PopAndDestroy(/*defaultRfs*/);    
        return;
        }
        
    // If the file is missing copy it from ROM.
    if ((err == KErrNotFound) || (err == KErrPathNotFound))
        {
        RFs      romRfs;
        RFile    file;
        RFile    romFile;
        TInt     size;
        HBufC8*  buffer = NULL;    

        // Ensure the private path is defined on the c drive.
        (void) defaultRfs.CreatePrivatePath(EDriveC);
        
        // Open an rfs for the z drive.        
        User::LeaveIfError(romRfs.Connect());
        CleanupClosePushL(romRfs);
        User::LeaveIfError(romRfs.SetSessionPath(_L("z:\\")));

        // Create the destination file.
        User::LeaveIfError(file.Create(defaultRfs, path, EFileWrite));
        CleanupClosePushL(file);
        
        // Open the source file.
        User::LeaveIfError(romFile.Open(romRfs, path, EFileRead));
        CleanupClosePushL(romFile);
        
        // Copy the file.
        User::LeaveIfError(romFile.Size(size));
        buffer = HBufC8::NewLC(size);
        TPtr8 bufferPtr(buffer->Des());
        
        User::LeaveIfError(romFile.Read(bufferPtr, size));
        User::LeaveIfError(file.Write(bufferPtr, size));
          
        // Clean up
        CleanupStack::PopAndDestroy(buffer);
        CleanupStack::PopAndDestroy(/*romFile*/);
        CleanupStack::PopAndDestroy(/*file*/);
        CleanupStack::PopAndDestroy(/*romRfs*/);    
        CleanupStack::PopAndDestroy(/*defaultRfs*/);    
        }
        
    // Otherwise leave.
    else
        {
        User::LeaveIfError(err);
        }
    }

// -----------------------------------------------------------------------------
// CFeedsServer::SessionOpened
//
// Called from CFeedsServerSession when a session is opened.
// -----------------------------------------------------------------------------
//
void CFeedsServer::SessionOpened()
    {
    iSessionCount++;
    }

// -----------------------------------------------------------------------------
// CFeedsServer::SessionClosed
//
// Called from CFeedsServerSession when a session is closed.
// -----------------------------------------------------------------------------
//
void CFeedsServer::SessionClosed()
    {
    iSessionCount--;
    
    // If all of the sessions are closed then terminate the server.
    //
    // TODO: Create a timer to delay the shutdown for 20 mins or so, in case
    //       the client crashes.
    // Don't want to shut down feedsserver to ensure autoupdate can occur
    if ( iSessionCount <= 0 && !CheckForUpdate() )
         {
         CActiveScheduler::Stop();
         }
    }

// -----------------------------------------------------------------------------
// CFeedsServer::SetAutoUpdateSettingsL
//
// Called from CFeedsServerSession when setting changed.
// -----------------------------------------------------------------------------
//
void CFeedsServer::SetAutoUpdateSettingsL( TInt aFolderListId, TBool aAutoUpdate, TInt aAutoUpdateFreq, TUint32 aAutoUpdateAP, TBool aAutoUpdateWhileRoaming ) 
    {
    TBool    autoUpdate = EFalse;
    TInt     autoUpdateFreq = 0;
    TUint32  autoUpdateAP = 0;
    TBool	 autoUpdateWhileRoaming = EFalse;
    
    TRAPD( err, iFeedsDatabase->ExtractAutoUpdateSettingsL( aFolderListId, autoUpdate, autoUpdateFreq, autoUpdateAP, autoUpdateWhileRoaming ) );
    if( err != KErrNotFound && err != KErrNone )
        {
        User::Leave( err );
        }

    // if any setting changed regarding auto update, restart the scheduler
    if( autoUpdate != aAutoUpdate || autoUpdateFreq != aAutoUpdateFreq || autoUpdateAP != aAutoUpdateAP || autoUpdateWhileRoaming != aAutoUpdateWhileRoaming)
        {
        iFeedsDatabase->CommitAutoUpdateSettingsL( aFolderListId, aAutoUpdate, aAutoUpdateFreq, aAutoUpdateAP, aAutoUpdateWhileRoaming );
                
        // notify all clients with the same folderListId about setting changes
        NotifySettingsChanged( aFolderListId );

        ScheduleUpdateManagerL( aFolderListId, aAutoUpdate, aAutoUpdateFreq, aAutoUpdateAP, aAutoUpdateWhileRoaming );        
        }
    }

// -----------------------------------------------------------------------------
// CFeedsServer::ScheduleUpdateManagerL
//
// Schedule the update manager for all folder list ID
// -----------------------------------------------------------------------------
//
void CFeedsServer::ScheduleUpdateManagerL()
    {
    RArray<TInt>     folderListIds;
    TBool    autoUpdate = EFalse;
    TInt     autoUpdateFreq = 0;
    TUint32  autoUpdateAP = 0;
	TBool    autoUpdateWhileRoaming = EFalse;
    
    TRAPD( err, iFeedsDatabase->ExtractFolderListIdInSettingsL( folderListIds ) );
    if (err == KErrNone)
        {               
        for( int i=0; i < folderListIds.Count(); i++)
            {
            // When err is KErrNotFound, autoUpdate will remain false
            TRAPD( err, iFeedsDatabase->ExtractAutoUpdateSettingsL( folderListIds[i], autoUpdate, autoUpdateFreq, autoUpdateAP, autoUpdateWhileRoaming ) );
            if (err == KErrNone)
                {
                ScheduleUpdateManagerL( folderListIds[i], autoUpdate, autoUpdateFreq, autoUpdateAP, autoUpdateWhileRoaming );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CFeedsServer::ScheduleUpdateManagerL
//
// Schedule the update manager for all folder list ID
// -----------------------------------------------------------------------------
//
void CFeedsServer::ScheduleUpdateManagerL( TInt aFolderListId)
    {
    TBool    autoUpdate = EFalse;
    TInt     autoUpdateFreq = 0;
    TUint32  autoUpdateAP = 0;
	TBool    autoUpdateWhileRoaming = EFalse;

    // When err is KErrNotFound, autoUpdate will remain false
    TRAPD( err, iFeedsDatabase->ExtractAutoUpdateSettingsL( aFolderListId, autoUpdate, autoUpdateFreq, autoUpdateAP, autoUpdateWhileRoaming ) );
    if (err == KErrNone)
        {
        ScheduleUpdateManagerL( aFolderListId, autoUpdate, autoUpdateFreq, autoUpdateAP, autoUpdateWhileRoaming );
        }
    }

// -----------------------------------------------------------------------------
// CFeedsServer::ScheduleUpdateManagerL
//
// Schedule the background update all task for one folder list ID.
// -----------------------------------------------------------------------------
//
void CFeedsServer::ScheduleUpdateManagerL( TInt aFolderListId, TBool /*aAutoUpdate*/, TInt aAutoUpdateFreq, TUint32 aAutoUpdateAP, TBool aAutoUpdateWhileRoaming )
    {
    // Clear the old timer.

    for( int i=0; i < iUpdateManagerList.Count(); i++)
        {
        if( iUpdateManagerList[ i ]->FolderListId() == aFolderListId )
            {
            delete iUpdateManagerList[ i ];
            iUpdateManagerList.Remove(i); 
            break;
            }
        }
    CUpdateManager* updateManager = CUpdateManager::NewL( aFolderListId, aAutoUpdateAP, aAutoUpdateFreq, aAutoUpdateWhileRoaming, *this );
    updateManager->StartL();
    iUpdateManagerList.AppendL( updateManager );
    } 

// -----------------------------------------------------------------------------
// CFeedsServer::ResetXmlUtilsL
//
// Called from CUpdateFeedTask::LoadCompleted when an error occurs while parsing
// the feed.
// -----------------------------------------------------------------------------
//
void CFeedsServer::ResetXmlUtilsL()
    {
    if (iFolderHandler != NULL)
        {
        delete iFolderHandler;
        iFolderHandler = NULL;
        }
        
    if (iFeedHandler != NULL)
        {
        delete iFeedHandler;
        iFeedHandler = NULL;
        }
        
    if (iXmlUtils != NULL)
        {
        delete iXmlUtils;
        iXmlUtils = NULL;
        }
    
    iXmlUtils = CXmlUtils::NewL();
    iFeedHandler = CFeedHandler::NewL(*iXmlUtils);
    iFolderHandler = COpmlParser::NewL(*iXmlUtils);
    }

// -----------------------------------------------------------------------------
// CFeedsServer::CheckForUpdate
//
// Checks whether update is happening
// -----------------------------------------------------------------------------
//
TBool CFeedsServer::CheckForUpdate()
    {
    for(int i = 0;i < iUpdateManagerList.Count();i++)
        {
        if(iUpdateManagerList[i]->iQueueArray.Count() > 0)
            return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CFeedsServer::UpdateFeedL()
//
// Update the feed ID in Update manager
// -----------------------------------------------------------------------------
//
void CFeedsServer::UpdateFeedL(TInt aFolderListId, TInt aFeedId, TBool aDeleteFeed)
    {
    for( int i=0; i < iUpdateManagerList.Count(); i++)
        {
        if( iUpdateManagerList[ i ]->FolderListId() == aFolderListId )
            {
            iUpdateManagerList[ i ]->UpdateFeedL(aFeedId,aDeleteFeed);
            return;
            }
         else if(aFolderListId == KNoFolderListId)
            {
            iUpdateManagerList[ i ]->UpdateFeedL(aFeedId,aDeleteFeed);
            }
        }	
    }

