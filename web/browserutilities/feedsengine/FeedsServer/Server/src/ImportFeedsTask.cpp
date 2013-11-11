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
* Description:  A task to import Feeds.
*
*/


#include <s32mem.h>

#include "ImportFeedsTask.h"
#include "FeedsDatabase.h"
#include "FeedsServer.h"
#include "FeedsServerMsg.h"

// Root folder related.
const TInt  KRootFolderId = 0;

// -----------------------------------------------------------------------------
// CImportFeedsTask::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImportFeedsTask* CImportFeedsTask::NewL(CFeedsServer& aFeedsServer, 
        					MImportFeedsTaskObserver& aObserver, const RMessage2& aMessage)
    {
    CImportFeedsTask* self = new (ELeave) CImportFeedsTask(aFeedsServer, aObserver, aMessage);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// -----------------------------------------------------------------------------
// CImportFeedsTask::CImportFeedsTask
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImportFeedsTask::CImportFeedsTask(CFeedsServer& aFeedsServer, MImportFeedsTaskObserver& aObserver, const RMessage2& aMessage):
        CTask(aFeedsServer), iObserver(aObserver), iMessage(aMessage)
    {
    	
    }
        

// -----------------------------------------------------------------------------
// CImportFeedsTask::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImportFeedsTask::ConstructL()
    {
    BaseConstructL(ETrue);

    iIdle = CIdle::NewL(CActive::EPriorityStandard);
  	iPackedFolder = NULL;
  	iMoreToAdd = ETrue;
  	iFeedsDatabase = &(iFeedsServer.Database());
    }        

// -----------------------------------------------------------------------------
// CImportFeedsTask::CImportFeedsTask
// Deconstructor.
// -----------------------------------------------------------------------------
//
CImportFeedsTask::~CImportFeedsTask()
    {
    TRAP_IGNORE( CleanupImportTaskL() );

	iImportFile.Close();
	iRFs.Close();
	
	delete iFullImportFileName;
    delete iPackedFolder;
    delete iIdle;
    }

// -----------------------------------------------------------------------------
// CImportFeedsTask::StartTaskL.
//
// Starts the task.
// -----------------------------------------------------------------------------
//
void CImportFeedsTask::StartTaskL()
    {
	iImportState = EImportOpenOPMLFile;
	iDBTransactInited = EFalse;
	iFoundRootFolder = EFalse;

	iParentStack.AppendL(KRootFolderId);    
	iSiblingIndexStack.AppendL(0);
	
	iIdle->Start(TCallBack(StartImportStateMachine, this));
	}
	

// -----------------------------------------------------------------------------
// CImportFeedsTask::StartImportStateMachine.
//
// Starts the OPML import task
// -----------------------------------------------------------------------------
//
TBool CImportFeedsTask::StartImportStateMachine(TAny *aPtr)
	{
	TInt err(KErrNone);
	
    CImportFeedsTask *self = static_cast<CImportFeedsTask *>(aPtr);
	
	switch(self->iImportState)
		{
		case EImportOpenOPMLFile :
			TRAP(err, self->OpenOPMLFileL());
			if(err != KErrNone)
				{
				self->iObserver.ImportCompleted(err);
				return EFalse;
				}
			self->iImportState = EImportPreparePackedFolder;
			return ETrue;
			
		case EImportPreparePackedFolder:
			TRAP(err, self->PreparePackedFolderL());
			if(err != KErrNone)
				{
				self->iObserver.ImportCompleted(err);
				return EFalse;
				}
			self->iImportState = EImportIntoDB;
			return ETrue;

		case EImportIntoDB:
			
			TRAP(err, self->ImportIntoDBL());
			if(err != KErrNone)
				{
				self->iObserver.ImportCompleted(err);
				return EFalse;
				}
			if(self->iMoreToAdd)
				{
				return ETrue;
				}
			self->iImportState = EImportDone;
			self->iObserver.ImportCompleted(KErrNone);
			return EFalse;
		}
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CImportFeedsTask::CleanupImportTask.
//
// Cancel the ongoing OPML import task
// -----------------------------------------------------------------------------
//
void CImportFeedsTask::CleanupImportTaskL()
	{
	if(iImportState ==  EImportIntoDB && iMoreToAdd)
		{
		iFeedsDatabase->CommitImportTransaction();
		
		// Rollback didnt work with tables having index, so deleting all the added records :(
		iFeedsDatabase->DeleteFeedTableRecordsL(iFeedIds);
		iFeedsDatabase->DeleteFolderListTableRecordsL(iFolderItemIds);
		
		if(iDBTransactInited)
			ReleaseImportTables();
		}
	}

// -----------------------------------------------------------------------------
// CImportFeedsTask::ExtractClientImportMessageL.
//
// Extract the client passes message
// -----------------------------------------------------------------------------
//
void CImportFeedsTask::OpenOPMLFileL()
	{
    User::LeaveIfError(iRFs.Connect());
	User::LeaveIfError(iImportFile.Open(iRFs, *iFullImportFileName, EFileRead | EFileShareReadersOnly));
	}

// -----------------------------------------------------------------------------
// CImportFeedsTask::PreparePackedFolderL.
//
// Creates the packed folder by parsing the OPML file
// -----------------------------------------------------------------------------
//
void CImportFeedsTask::PreparePackedFolderL()
	{
	iPackedFolder = iFeedsServer.ImportFolderL(iImportFile, iDispImportFileName);	
	}

// -----------------------------------------------------------------------------
// CImportFeedsTask::ReleaseImportTablesCallback.
//
// Callback function for import task completion
// -----------------------------------------------------------------------------
//
void CImportFeedsTask::ReleaseImportTablesCallback(TAny *ptr)
	{
	CImportFeedsTask *self = static_cast<CImportFeedsTask *>(ptr);
	self->iFeedsDatabase->CommitImportTransaction();

	self->ReleaseImportTables();
	}

// -----------------------------------------------------------------------------
// CImportFeedsTask::ReleaseImportTables.
//
// Closes all the OPML import related tables
// -----------------------------------------------------------------------------
//
void CImportFeedsTask::ReleaseImportTables()
	{
	iFeedsDatabase->ReleaseImportTables();
	}

// -----------------------------------------------------------------------------
// CImportFeedsTask::ImportIntoDBL.
//
// Starts importing the packed folder into DB
// -----------------------------------------------------------------------------
//
void CImportFeedsTask::ImportIntoDBL()
	{

	CleanupStack::PushL(TCleanupItem(&ReleaseImportTablesCallback, this));

	if(!iDBTransactInited)
		{
		iFeedsDatabase->PrepareImportTransationsL(); // Open tables, mark start of transactions
  		iDBTransactInited = ETrue;
		}

	ImportFoldersL(); // Import record by record

	if(!iMoreToAdd)
		{
    	CleanupStack::PopAndDestroy(/*ReleaseImportTablesCallback*/);
		}
	else
		CleanupStack::Pop(/*ReleaseImportTablesCallback*/);
	}

// -----------------------------------------------------------------------------
// CImportFeedsTask::ImportFoldersL.
//
// Imports the packed folder into DB
// -----------------------------------------------------------------------------
//
void CImportFeedsTask::ImportFoldersL()
    {
    TInt                parent;
    TInt                siblingIndex;
    TInt                folderId;
    TPtrC               title;
    TPtrC               url;
    TInt                entryId;
    TInt                feedId;
    TInt                newfeedId;
    TTime               timestamp;
    TInt                unreadCount;
    TInt 				statusCode;
    TInt 				freq;
	if(!iPackedFolder->HasNextToken()) 
		{
		iMoreToAdd = EFalse;
		return;
		}
	
	iMoreToAdd = ETrue;
		
	TUint token = iPackedFolder->NextToken();
        
	switch (token)
		{
		case EFolderTokenFolderBegin:
                // Extract the attributes.
			iPackedFolder->ExtractAttributes(title, url, entryId, feedId, timestamp, unreadCount, statusCode, freq);
                
            // The first folder in the PackedFolder is the root-folder.  There is no
            // reason to store this folder in the database, as such this folder is skipped.
            if (!iFoundRootFolder)
                {
                iFoundRootFolder = ETrue;
                return;
                }

            // Determine the parent and its sibling index.
            parent = iParentStack[iParentStack.Count() - 1];
            siblingIndex = iSiblingIndexStack[iSiblingIndexStack.Count() - 1];
            iSiblingIndexStack[iSiblingIndexStack.Count() - 1]++;

            //create a sibling index hole
            iFeedsDatabase->CreateSiblingIndexHoleL(parent, siblingIndex, 1);

            // Add the folder.      
            iFeedsDatabase->FolderItemAddHelperL(iFolderListId, title, KNullDesC, 
                    ETrue, siblingIndex, parent, folderId, newfeedId, freq);

            // Push this folder on the stack as the active parent.
            iParentStack.AppendL(folderId);
            iSiblingIndexStack.AppendL(0);
            
            iFolderItemIds.AppendL(folderId);
            break;

        case EFolderTokenFolderEnd:
            // Pop this folder off of the stacks.
            iParentStack.Remove(iParentStack.Count() - 1);
            iSiblingIndexStack.Remove(iSiblingIndexStack.Count() - 1);
            break;

        case EFolderTokenFeed:
            // Extract the attributes.
            iPackedFolder->ExtractAttributes(title, url, entryId, feedId, timestamp, unreadCount, statusCode, freq);

            // Determine the parent and its sibling index.
            parent = iParentStack[iParentStack.Count() - 1];
            siblingIndex = iSiblingIndexStack[iSiblingIndexStack.Count() - 1];
            
            // Add the feed.                
            TRAPD( err, iFeedsDatabase->FolderItemAddHelperL(iFolderListId, title, url, 
                    EFalse, siblingIndex, parent, folderId, newfeedId, freq) );

            // Ignore problematic ones and continue to the next token
            if( err == KErrNone && newfeedId >= 0)
                {
                iSiblingIndexStack[iSiblingIndexStack.Count() - 1]++;
                
                iFolderItemIds.AppendL(folderId);
                iFeedIds.AppendL(newfeedId);
                }
            break;               
        }
    }

