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
* Description:  The feeds server database.
*
*/


#include <badesca.h>
#include <e32math.h>

#include "FeedAttributes.h"
#include "FeedsDatabase.h"
#include "FolderAttributes.h"
#include "PackedAttributes.h"
#include "LeakTracker.h"
#include "Logger.h"
#include "PackedFeed.h"
#include "PackedFolder.h"
#include "FeedsServer.h"

// Constants
// Database file name.
const TUid KSecureUid = {0x10281F95};
_LIT(KSecure,	"SECURE");
_LIT(KDatabaseFileName, "FeedsDatabase");
_LIT(KDbExtension, ".db");    

// Table names.
_LIT(KVersionTable, "VersionTable");
_LIT(KFolderListTable, "FolderListTable");
_LIT(KFeedTable, "FeedTable");
_LIT(KItemTable, "ItemTable");
_LIT(KEnclosureTable, "EnclosureTable");
_LIT(KSettingsTable, "SettingsTable");

// Table index names.
_LIT(KFolderListTableIndex, "FolderListTableIndex");
_LIT(KFeedTableIndex, "FeedTableIndex");
_LIT(KItemTableIndex, "ItemTableIndex");
_LIT(KEnclosureTableIndex, "EnclosureTableIndex");
_LIT(KSettingsTableIndex, "SettingsTableIndex");

// Table field names.
_LIT(KFolderListId, "FolderListId");
_LIT(KFolderItemId, "FolderItemId");
_LIT(KIsFolder, "IsFolder");
_LIT(KItemId, "ItemId");
_LIT(KParentId, "ParentId");
_LIT(KSiblingOrder, "SiblingOrder");
_LIT(KStatus,"Status");
_LIT(KVersion, "Version");
_LIT(KVersionId, "VersionID");
_LIT(KTitle_100MaxLen, "Title");
_LIT(KFeedId, "FeedId");
_LIT(KDate, "Date");
_LIT(KFeedUrl, "FeedUrl");
_LIT(KWebUrl, "WebUrl");
_LIT(KDescription, "Description");
_LIT(KItemStatus, "ItemStatus");
_LIT(KEnclosureId, "EnclosureId");
_LIT(KLength_100MaxLen, "Length");
_LIT(KContentType_100MaxLen, "ContentType");
_LIT(KItemIdStr, "ItemIdStr");
_LIT(KUnreadCount, "UnreadCount");
_LIT(KAccessPoint, "AccessPoint");
_LIT(KAutoUpdate, "AutoUpdate");
_LIT(KAutoUpdateWhileRoaming, "AutoUpdateWhileRoaming");
_LIT(KAutoUpdateFreq, "AutoUpdateFreq");
_LIT(KLastAutoUpdate, "LastAutoUpdate");

// Misc string consts.
_LIT(KSpace, " ");
_LIT(KNew, "new");
_LIT(KUnread, "unread");
_LIT(KRead, "read");

// Root folder related.
const TInt  KRootFolderId = 0;
_LIT(KRootFolder, "Root Folder");

// Misc consts.
const TInt K100MaxLen = 100;
const TInt KIntLength = 15;
const TInt KInt64Length = 25;
const TInt KVersion31 = 1;
//const TInt KVersion32 = 2;
const TInt KVersion50 = 3;
const TInt KVersion71 = 4;

const TInt KResvSpaceForDupTitles = 6; //Space should be left in feed/folder title to append number in case of duplicate titles.
const TInt KAutoUpdatingOff = 0;

// DATA TYPES
struct TAttribute
    {
    TUint  token;
    TPtrC  value;
    };
    
  
// TODO: RDbDatabase::Begin/Commit/RollBack and RDbRowSet::Cancel needs to be
//      used to guard the consistancy of the databse.  See CMidRecordStoreDb 
//      (CMidRecordStoreDb.cpp) for a good example.

// -----------------------------------------------------------------------------
// CFeedsDatabase::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsDatabase* CFeedsDatabase::NewL(CFeedsServer* aFeedsServer, TBool &aDatabaseCreated)
    {
    CFeedsDatabase* self = new (ELeave) CFeedsDatabase(aFeedsServer);

    aDatabaseCreated = EFalse;
    
    CleanupStack::PushL(self);
    self->ConstructL(aDatabaseCreated);
    CleanupStack::Pop();
    
    return self;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::NewL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ConstructL(TBool &aDatabaseCreated)
    {
    TInt     err;
    User::LeaveIfError( iDBs.Connect() );

    // Create the path to the database file.
    TParse aParse;
    TDriveName cDrive = TDriveUnit( EDriveC ).Name(); 
    User::LeaveIfError( aParse.SetNoWild( KDatabaseFileName, &cDrive, &KDbExtension() ) );
    

    // Determine if the database exists.    
    TBool isDbExists = EFalse;  
    CDbDatabaseNames *dbNames = iDBs.DatabaseNamesL(EDriveC, KSecureUid);   
    for(int i = 0; i < dbNames->Count(); i++)
    	{
    	if( (*dbNames)[i] == aParse.NameAndExt() )
    		{
    		isDbExists = ETrue;
    		break;
    		}
    	}
    delete dbNames;
    
    iDatabasePath = aParse.FullName();
    // If the database file doesn't exit create it and create the tables.
    if ( !isDbExists )
        {
        TRAP( err, ResetDatabaseL() );

        // Ensure that the database isn't partially initialized.
        if ( err != KErrNone )
            {
            iDBs.DeleteDatabase( iDatabasePath, KSecureUid );
            User::Leave( err );
            }
        
        aDatabaseCreated = ETrue;
        }
        
    // Otherwise, just open the database.
    else
        {
    	TBuf<32> format;
        format.Copy(KSecure);
        format.Append(KSecureUid.Name());
    
        User::LeaveIfError(iDatabase.Open(iDBs, iDatabasePath, format));

        // check version
        UseVersionTableLC( RDbTable::EReadOnly );
        // Get the row.
        iVersionTable.FirstL();
        iVersionTable.GetL();
        // Get version
        TInt version = iVersionTable.ColUint16(iVersionColSet->ColNo(KVersionId));
        //
        CleanupStack::PopAndDestroy(/*version Table*/);
        // If version is lower than 5.0, add a new column into old database, then update the version table.
        if ( version < KVersion50 )
            {                
                            
            UseFolderListTableLC(RDbTable::EUpdatable);
		    
            // get the exiting col set
            CDbColSet* colSet = iDatabase.ColSetL(KFolderListTable);
		    
            colSet->AddL(TDbCol(KStatus, EDbColInt32));
            iDatabase.AlterTable(KFolderListTable, *colSet);
		    
            CleanupStack::PopAndDestroy(/* FolderListTable */);  		     
		    
		    // Set the status as KErrorNone
            // Prep the item table.
    	    UseFolderListTableLC(RDbTable::EUpdatable);

            while( iFolderListTable.NextL() )
                {
                // Get the row. and add the default value.
                iFolderListTable.GetL();
                iFolderListTable.UpdateL();

                iFolderListTable.SetColL(iFolderListColSet->ColNo(KStatus), KErrNone);
		        
                iFolderListTable.PutL();

                }
					
            CleanupStack::PopAndDestroy(/* FolderListTable */);
            
            //Add a new colum KAutoUpdateFreq to FeedTable
            AlterFeedTableWithAutoFequencyL();
            // Add the version as 7.1
            UseVersionTableLC(RDbTable::EUpdatable);
		    
            // Update the version table.There is only one row in this table.
            iVersionTable.FirstL();
		    
            if (iVersionTable.AtRow())
                {        
                iVersionTable.GetL();
                iVersionTable.UpdateL();
                }
            else
                {
                iVersionTable.Reset();
                iVersionTable.InsertL();
                }
		    
		    // Add the version as 7.1
            iVersionTable.SetColL(iVersionColSet->ColNo(KVersionId), KVersion71);
            iVersionTable.PutL();
            CleanupStack::PopAndDestroy(/* version table */);

            }
        else if( version < KVersion71 )
            {
            //Add a new colum KAutoUpdateFreq to FeedTable
            AlterFeedTableWithAutoFequencyL();
	        // Add the version as 7.1 in DB
            UseVersionTableLC(RDbTable::EUpdatable);
		    
            // Update the database.  There is only one row in this table...
            iVersionTable.FirstL();
		    
            if (iVersionTable.AtRow())
                {        
                iVersionTable.GetL();
                iVersionTable.UpdateL();
                }
            else
                {
                iVersionTable.Reset();
                iVersionTable.InsertL();
                }
		    
            // Add the version as 7.1
            iVersionTable.SetColL(iVersionColSet->ColNo(KVersionId), KVersion71);
            iVersionTable.PutL();
            CleanupStack::PopAndDestroy(/* version table */);
		    
            }
        else if ( version == KVersion31 )
            {                
            // JH todo: transform here!
            }
        }        
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::CFeedsDatabase
//
// C++ default constructor can NOT contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
CFeedsDatabase::CFeedsDatabase(CFeedsServer* aFeedsServer):
    iLeakTracker(CLeakTracker::EFeedsDatabase),iFeedsServer(aFeedsServer),iIsFolderTableUpdateNeeded(EFalse)
    {
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::~CFeedsDatabase
//
// Destructor
// -----------------------------------------------------------------------------
//
CFeedsDatabase::~CFeedsDatabase()
    {
    delete iFolderListColSet;
    iFolderListTable.Close();

    delete iFeedColSet;
    iFeedTable.Close();

    delete iItemColSet;
    iItemTable.Close();

    delete iEnclosureColSet;
    iEnclosureTable.Close();

    delete iVersionColSet;
    iVersionTable.Close();

    delete iSettingsColSet;
    iSettingsTable.Close();

    iDatabase.Close();
    iDBs.Close();
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::FeedIdFromUrlL
//
// Returns the feed if of the feed with the given url.
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::FeedIdFromUrlL(const TDesC& aFeedUrl, TInt aFolderListId, TInt& aFeedId)
    {
    RDbView  view;
    TBool    found = EFalse;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    // SELECT FeedId FROM FeedTable WHERE FeedUrl = 'aFeedUrl' AND FolderListId = aFolderListId
    _LIT(KQuery, "SELECT FeedId FROM FeedTable WHERE FeedUrl = \'%S\' AND FolderListId = %d");
    
    query = HBufC::NewLC( KQuery().Length() + aFeedUrl.Length() + KIntLength );
            
    query->Des().Format( KQuery, &aFeedUrl, aFolderListId );

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    // Search for the feed.
    if (view.Evaluate() >= 0)
        {
        if (view.FirstL())
            {
            // Get the feed id.
            view.GetL();        
            aFeedId = view.ColUint16(colSet->ColNo(KFeedId));
            found = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);
    
    return found;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderListIdFromFeedIdL
//
// Returns the feed if of the feed with the given url.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderListIdFromFeedIdL( TInt aFeedId, TInt& aFolderListId )
    {
    TDbSeekKey  seekKey((TUint16) aFeedId);
    
    // Prep the feed table.
    UseFeedTableLC(RDbTable::EReadOnly);

    // Get the information about the feed.
    if (iFeedTable.SeekL(seekKey))
        {        
        // Get the row.
        iFeedTable.GetL();
        
        // Extract the fields.
        aFolderListId = iFeedTable.ColInt32(iFeedColSet->ColNo(KFolderListId));        
        }
    else
        {
        User::Leave(KErrCorrupt);
        }
                
    CleanupStack::PopAndDestroy(/*Feed Table*/);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::FeedIdFromFolderItemIdL
//
// Return the feed-id of the given folder-item-id.
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::FeedIdFromFolderItemIdL(TInt aFolderItemId, TInt& aFeedId)
    {
    TDbSeekKey  seekKey((TUint16) aFolderItemId);
    TBool       found = EFalse;
    
    // Prep the FolderList table.
    UseFolderListTableLC(RDbTable::EReadOnly);

    // Get the folder item.
    if (iFolderListTable.SeekL(seekKey))
        {        
        TBool  isFolder;
        
        // Get the row.
        iFolderListTable.GetL();
        
        isFolder = iFolderListTable.ColUint8(iFolderListColSet->ColNo(KIsFolder));
        
        if (!isFolder)
            {                
            aFeedId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KFeedId));
            found = ETrue;
            }
        }
                
    CleanupStack::PopAndDestroy(/*folder list Table*/);
    
    return found;
    }
    

// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderItemInfoL
//
// Return the folder item info from folder-item-id.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderItemInfoL(TInt aFolderItemId, TBool &aIsFolder, HBufC*& aTitle, HBufC*& aFeedUrl)
	{
	TDbSeekKey  seekKey((TUint16) aFolderItemId);

	// Prep the FolderList table.
	UseFolderListTableLC(RDbTable::EReadOnly);

	// Get the folder item.
	if (iFolderListTable.SeekL(seekKey))
		{        
		// Get the row.
		iFolderListTable.GetL();

		aIsFolder = iFolderListTable.ColUint8(iFolderListColSet->ColNo(KIsFolder));
	
		*aTitle = iFolderListTable.ColDes16(iFolderListColSet->ColNo(KTitle_100MaxLen));
	
		TInt FeedId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KFeedId));

		if(!aIsFolder)
			{

			RDbView  view;
			HBufC*   query = NULL;
    
			// Create a view given this select...    
			// SELECT FeedUrl FROM FeedTable WHERE FeedId = aFeedId
			_LIT(KQuery, "SELECT FeedUrl FROM FeedTable WHERE FeedId = %d");
    
			query = HBufC::NewLC(KQuery().Length() + KIntLength);
            
			query->Des().Format(KQuery, FeedId);

			User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
			CleanupClosePushL(view);

			CDbColSet* colSet = view.ColSetL();
			CleanupStack::PushL(colSet);
    
			// Search for the feed.
			if (view.Evaluate() >= 0)
				{
				if (view.FirstL())
					{
					// Get the feed id.
					view.GetL();        
					ReadLongTextL(view, colSet->ColNo(KFeedUrl), aFeedUrl);
					}
				}
        
			CleanupStack::PopAndDestroy(/*colSet*/);
			CleanupStack::PopAndDestroy(/*view*/);
			CleanupStack::PopAndDestroy(/*query*/);
			}
        }

	CleanupStack::PopAndDestroy(/*folder list Table*/);
	}

// -----------------------------------------------------------------------------
// CFeedsDatabase::UrlFromFeedIdL
//
// Return the url of the feed with the given feed-id.
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::UrlFromFeedIdL(TInt aFeedId, HBufC*& aFeedUrl)
    {
    RDbView  view;
    TBool    found = EFalse;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    // SELECT FeedUrl FROM FeedTable WHERE FeedId = aFeedId
    _LIT(KQuery, "SELECT FeedUrl FROM FeedTable WHERE FeedId = %d");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength);
            
    query->Des().Format(KQuery, aFeedId);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    // Search for the feed.
    if (view.Evaluate() >= 0)
        {
        if (view.FirstL())
            {
            // Get the feed id.
            view.GetL();        
            ReadLongTextL(view, colSet->ColNo(KFeedUrl), aFeedUrl);
            found = ETrue;
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);

    return found;
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::FindItemL
//
// Finds the item with the given item-id-str and returns its id.
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::FindItemL(TInt aFeedId, const TDesC& aItemIdStr, TInt& aItemId)
    {
    RDbView  view;
    TBool    found = EFalse;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    // SELECT ItemId FROM ItemTable WHERE FeedId = aFeedId AND ItemIdStr = 'aItemIdStr'
    _LIT(KQuery, "SELECT ItemId FROM ItemTable WHERE FeedId = %d AND ItemIdStr = \'%S\'");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength + aItemIdStr.Length()); 
            
    query->Des().Format(KQuery, aFeedId, &aItemIdStr);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    // Find the item.
    if (view.Evaluate() >= 0)
        {
        if (view.FirstL())
            {
            // Get the item id.
            view.GetL();        
            aItemId = view.ColUint16(colSet->ColNo(KItemId));
            found = ETrue;
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);
    return found;
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::FindFolderItemL
//
// Finds the folder item with the given name.
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::FindFolderItemL(TInt& aFolderListId, const TDesC& aName, 
        TInt& aFolderItemId)
    {
    RDbView  view;
    TBool    found = EFalse;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    // SELECT FolderItemId FROM FolderListTable WHERE FolderListId = aFolderListId 
    // AND Title = 'aName'
    _LIT(KQuery, "SELECT FolderItemId FROM FolderListTable WHERE FolderListId = %d AND Title = '%S'");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength + aName.Length());
            
    query->Des().Format(KQuery, aFolderListId, &aName);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    // Search for the feed.
    if (view.Evaluate() >= 0)
        {
        if (view.FirstL())
            {
            // Get the feed id.
            view.GetL();        
            aFolderItemId = view.ColUint16(colSet->ColNo(KFolderItemId));
            found = ETrue;
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);
    
    return found;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::GetDuplicateFolderCounter
//
// Parses the Folder/Feed name to get the postfix integer for duplicated names 
//    (ex Folder, Folder (1), Folder (2) ...)
// -----------------------------------------------------------------------------
//
//TBool CFeedsDatabase::GetDuplicateFolderCounter(const TDesC& aFolderTitle, TInt &aCounterVal)
TBool CFeedsDatabase::GetDuplicateFolderCounter(const TDesC& aFolderToBeAdded, const TDesC& aFolderTitle, TInt &aCounterVal)
	{
	
	if(aFolderToBeAdded == aFolderTitle)
		return ETrue;
	
	// return EFalse if FolderTitle is empty
	if(aFolderTitle.Length()<=0)
		return EFalse;
	
	// Use a lexical parser to parse the FolderTitle ( FolderName (%d))
	TLex parser(aFolderTitle);
	
	// Go til first '('
	while(!parser.Eos())
		{
		TChar c(parser.Peek());
		if( c != '(')
			parser.Get();
		else
			break;
		}

	// If end of string return EFalse
	if(parser.Eos())
		return EFalse;
	
	// skip '('
	parser.Get();

    // Get postfix integer value
	if(parser.Val(aCounterVal) != KErrNone)	
		return EFalse;
	
	// If not ending with ')' return EFalse
	if(parser.Get() != ')')
		return EFalse;
	
	// If end of string return ETrue
	if(parser.Eos())
		return ETrue;	

	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CFeedsDatabase::ValidateFeedURLL
//
// Checks for the FeedURL's existance
// Returns EFalse if the specified URL exists else ETrue
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::ValidateFeedURLL(const TInt &aFolderListId,  const TDesC& aUrl)
	{

	RDbView	view;
	HBufC*	query = NULL;
	TBool	IsValid = ETrue;	
	
	// If URL is empty return EFalse
	if(aUrl.Length() <= 0)
		return EFalse;

	// Check for the URL duplicate
	_LIT(KQuery, "SELECT FeedId FROM FeedTable WHERE FeedUrl = \'%S\' AND FolderListId = %d");
	query = HBufC::NewLC(KQuery().Length() + KIntLength + aUrl.Length());

	query->Des().Format(KQuery, &aUrl, aFolderListId);

	User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
	CleanupClosePushL(view);

	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL(colSet);

	// If there exists atleast one record we are not suppose to add the feed again
	if (view.Evaluate() >= 0)
		{
		if (view.FirstL())
			{
			// FeedURL aleady exists, no need to process this feed
			IsValid = EFalse;
			}
		}
			
	CleanupStack::PopAndDestroy(/*colSet*/);
	CleanupStack::PopAndDestroy(/*view*/);
	CleanupStack::PopAndDestroy(/*query*/);
	
	return IsValid;
	}

// -----------------------------------------------------------------------------
// CFeedsDatabase::GenerateNewFolderTitleL
//
// This will check for the duplicate folder names and suggest a new folder title if duplicated
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::GenerateNewFeedFolderTitleL(
											const TInt &aFolderListId, 
											const TInt &aParentEntryId, 
											const TDesC& aTitle, 
											TDes& aNewFeedFolderTitle
											)
	{
	RDbView  view;
	HBufC*   query = NULL;

	// Create a view given this select...    
	_LIT(KQuery, "SELECT Title FROM FolderListTable WHERE FolderListId = %d AND ParentId = %d AND Title LIKE '%S*'");

	if(aTitle.Length() <= 0)
		return;

	query = HBufC::NewLC(KQuery().Length() + KIntLength + aTitle.Length());

	query->Des().Format(KQuery, aFolderListId, aParentEntryId, &aTitle);

	User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
	CleanupClosePushL(view);

	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL(colSet);

	// Search for the feed.
	if (view.EvaluateAll() >= 0)
		{
		
		if(view.CountL() <= 0)
			{
			// Name doesnt exist, so no problem adding this name
			aNewFeedFolderTitle = aTitle;
			}
		else
			{
			RArray<TInt> FolderPostfixArray;
			CleanupClosePushL(FolderPostfixArray);
	
			// Store all folder name extensions ('FolderName (%d)') in an array
			for (view.FirstL(); view.AtRow(); view.NextL())
				{
				// Get the current row.
				view.GetL();

				HBufC *FolderTitle = NULL;
				ReadLongTextL(view, colSet->ColNo(KTitle_100MaxLen), FolderTitle);
				CleanupStack::PushL(FolderTitle);
				
				TInt PostfixVal=0;
				if(GetDuplicateFolderCounter(aTitle, FolderTitle->Des(), PostfixVal))
					FolderPostfixArray.Append(PostfixVal);

				CleanupStack::PopAndDestroy(/*FolderTitle*/);
				}

			// Search for a free postfix counter for duplicated folder/feed title
			TInt PostfixVal = 1;
			TInt length = FolderPostfixArray.Count();
			if(length <= 0)
				{
					aNewFeedFolderTitle = aTitle;		
				}
				else
				{
				do
					{
					TInt index = 0;
				    for (index = 0 ; index < length ; index++)
						{
						if(FolderPostfixArray[index] == PostfixVal)
							break;
						}

					if(index >= length)
						break;
					
					PostfixVal++;
					}
				while(ETrue);
				// Generate a new folder/feed title by appending the new counter
				_LIT(KFolderNameFormat, "%S (%d)");
				aNewFeedFolderTitle.Format(KFolderNameFormat, &aTitle, PostfixVal);
				}
				
			CleanupStack::PopAndDestroy(/*FolderPostfixArray*/);
			}
		}
	else
		{
		aNewFeedFolderTitle = aTitle;		
		}

	CleanupStack::PopAndDestroy(/*colSet*/);
	CleanupStack::PopAndDestroy(/*view*/);
	CleanupStack::PopAndDestroy(/*query*/);
	}
	
// -----------------------------------------------------------------------------
// CFeedsDatabase::ValidateFeedFolderTitleL
//
// This will check for the duplicate folder names and feed URLs and suggest the next title
// Returns ETrue if the Feed/Folder can be added to database else EFalse
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::ValidateFeedFolderTitleL(
												const TInt &aFolderListId, 
												const TInt &aParentEntryId, 
												const TDesC& aTitle, 
												const TBool aIsFolder, 
												TDes& aNewFeedTitle
											  )
	{
	if(!aIsFolder)
		{

		// If Feed URL is duplicated return EFalse
		//if(!ValidateFeedURLL(aFolderListId, aUrl))
		//	return EFalse;

		}

	if(aTitle.Length() <=0)
		return EFalse;

	GenerateNewFeedFolderTitleL(aFolderListId, aParentEntryId, aTitle, aNewFeedTitle);

	return ETrue;
	}
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::AllFeedIds
//
// Extract all of the feed-ids.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::AllFeedIdsL( RArray<TInt>& aFeedIds, TInt aFolderListId )
    {
    RDbView  view;
    HBufC*   query = NULL;
    
    if( aFolderListId == KAllFolderListId )
        {
        // Create a view given this select...    
        // SELECT FeedId FROM FeedTable
        _LIT(KQuery, "SELECT FeedId FROM FeedTable");
    
        query = HBufC::NewLC( KQuery().Length() );             
        }
    else
        {
        // Create a view given this select...    
        // SELECT FeedId FROM FeedTable WHERE FolderListId = aFolderListId
        _LIT(KQuery, "SELECT FeedId FROM FeedTable WHERE FolderListId = %d");
    
        query = HBufC::NewLC( KQuery().Length() + KIntLength );             
        query->Des().Format( KQuery, aFolderListId );
        }

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    // Find the item.
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            // Get the current row.
            view.GetL();
            
            aFeedIds.AppendL(view.ColUint16(colSet->ColNo(KFeedId)));
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::IsFreshL
//
// Determines if the given feed is newly created (not ok to read from the database).
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::IsNewlyCreatedL(TInt aFeedId)
    {
    TDbSeekKey  seekKey((TUint16) aFeedId);
    TBool       isNewlyCreated = EFalse;
    TTime       now;
    
    // Prep the feed table.
    UseFeedTableLC(RDbTable::EReadOnly);

    // Get the information about the feed.
    if (iFeedTable.SeekL(seekKey))
        {        
        // Get the row.
        iFeedTable.GetL();
        
        // Extract the fields.
        TTime  date = iFeedTable.ColTime(iFeedColSet->ColNo(KDate));
        
        // Determine if the feed is fresh.
        if (date == 0)
            {
            isNewlyCreated = ETrue;
            }
        }
    else
        {
        User::Leave(KErrCorrupt);
        }
                
    CleanupStack::PopAndDestroy(/*Feed Table*/);
    
    return isNewlyCreated;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ExtractFeedL
//
// Extracts the feed from the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ExtractFeedL(TInt aFeedId, CPackedFeed& aFeed)
    {
    // Pack the feed.
    PackFeedL(aFeedId, aFeed);
    
    // Pack the feed's items.
    PackItemsL(aFeedId, aFeed);

    // Signal the end of the feed.
    aFeed.FeedEndsL();
    aFeed.Trim();    
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ExtractRootFolderL
//
// Extracts the folder from the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ExtractRootFolderL( TInt aFolderListId, CPackedFolder& aPackedFolder, TBool aItemTitleNeed )
    {
    // Use the tables.
    UseFeedTableLC(RDbTable::EReadOnly);
    
    // Extract the root folder and all of its children.
    aPackedFolder.FolderBeginsL(KRootFolder, KRootFolderId);
    PackFolderL( aFolderListId, KRootFolderId, aPackedFolder, aItemTitleNeed );
    aPackedFolder.FolderEndsL();
    aPackedFolder.DoneL();

    // Clean up.
    CleanupStack::PopAndDestroy(/*feed table*/);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::UpdateFeedL
//
// Update the database given the packed feed.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::UpdateFeedL(const CPackedFeed& aPackedFeed, const TDesC& /* aFeedUrl */,TInt aFeedId,
        TInt aFolderListId, TBool aPurgeOldItems)
    {
    RArray<TInt>        itemIds;
    RArray<TAttribute>  feedAttributes;
    RArray<TAttribute>  itemAttributes;
    RArray<TAttribute>  enclosureAttributes;
    TBool               isFeed = EFalse;
    TBool               isItem = EFalse;
    TBool               isEnclosure = EFalse;
    TInt                feedId = 0;
    TInt                itemId = 0;
    TInt                enclosureId = 0;
    TAttribute          attribute;
    TUint               attributeToken;
    TPtrC               attributeValue;
    TBool               isNewFeed = EFalse;
    TInt                newItemCount = 0;
    TBool               newItem = EFalse;
    feedId = aFeedId;

    // This array holds the ids of the items found in the feed.  The array is then 
    // used to remove items from the database that aren't in the updated feed.
    CleanupClosePushL(itemIds);

    CleanupClosePushL(feedAttributes);
    CleanupClosePushL(itemAttributes);
    CleanupClosePushL(enclosureAttributes);

    // Use the various tables
    UseFeedTableLC(RDbTable::EUpdatable);
    UseItemTableLC(RDbTable::EUpdatable);
    UseEnclosureTableLC(RDbTable::EUpdatable);

    // Unpack the packed feed.
    while (aPackedFeed.HasNextToken())
        {
        TUint token;
        
        token = aPackedFeed.NextToken();
        
        switch (token)
            {
            case EFeedTokenFeedBegin:
                isFeed = ETrue;  
                //isNewFeed = ETrue;
                // Resolve the url to determine if this feed is already in the database.
                //if (FeedIdFromUrlL(aFeedUrl, aFolderListId, feedId))
                //    {
                //    isNewFeed = EFalse;
                //    }
                    
                // Otherwise lookup a new feed id to use.
                //else
                //    {
                //    // Get a new id for the feed.
                //    feedId = NextFeedIdL();
                //    }
                break;

            case EFeedTokenFeedEnd:
                {
                // TODO: Track whether or not any new item were added.  The feed's timestamp
                //       should only be updated if this happened.  So rather than updating the
                //       timestamp in CommitFeedL it should be updated here.
                //
                //       Or prehaps this shouldn't be done.  If this is done the user's "last update"
                //       timestamp won't be updated - so they may _feel_ the update didn't happen.
        
                // Remove any items that weren't in the packed feed (this removes old items).
                TInt purgedUnreadNewCount = 0;
                if (aPurgeOldItems)
                    {        
                    purgedUnreadNewCount = PurgeOtherItemsL(feedId, itemIds);
                    }
                newItemCount -= purgedUnreadNewCount;
    
                TTime  now;
                
                // Commit the feed to the database.
                now.UniversalTime();
                CommitFeedL(aFolderListId, isNewFeed, feedId, feedAttributes, now, newItemCount);
                
                feedAttributes.Reset();
                isFeed = EFalse;
                }
                break;

            case EFeedTokenItemBegin:
                isItem = ETrue; 
                itemId = NextItemIdL();              
                enclosureId = 0;
                break;

            case EFeedTokenItemEnd:
                // Commit the item to the database.
                newItem = CommitItemL(itemId, feedId, itemAttributes, itemIds);

                itemAttributes.Reset();
                isItem = EFalse;
                if( newItem )
                    {
                    newItemCount++;
                    }
                break;

            case EFeedTokenEnclosureBegin:
                isEnclosure = ETrue;
                break;

            case EFeedTokenEnclosureEnd:
                // Commit the enclosure to the database.
                CommitEnclosureL(enclosureId++, itemId, feedId, enclosureAttributes);

                enclosureAttributes.Reset();
                isEnclosure = EFalse;
                break;
                
            case EPackedTokenAttribute:
                // Apply the given attribute to the current object (enclosure, item, or feed).
                aPackedFeed.ExtractAttributeValue(attributeToken, attributeValue);

                attribute.token = attributeToken;
                attribute.value.Set(attributeValue);
                
                if (isEnclosure)
                    {
                    enclosureAttributes.AppendL(attribute);
                    }
                else if (isItem)
                    {
                    itemAttributes.AppendL(attribute);
                    }
                else if (isFeed)
                    {
                    feedAttributes.AppendL(attribute);
                    }
                break;
            }
        }

    // Clean up.
    CleanupStack::PopAndDestroy(/*enclosure table*/);
    CleanupStack::PopAndDestroy(/*item table*/);
    CleanupStack::PopAndDestroy(/*feed table*/);

    CleanupStack::PopAndDestroy(/*enclosureAttributes*/);
    CleanupStack::PopAndDestroy(/*attributeValue*/);
    CleanupStack::PopAndDestroy(/*feedAttributes*/);

    CleanupStack::PopAndDestroy(/*itemIds*/);
    iDatabase.Compact();
    }
        
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::ImportFolderL
//
// Update the database given the packed folder.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ImportFolderL(TInt aFolderListId, const CPackedFolder& aPackedFolder)
    {
    RArray<TInt>        parentStack;
    RArray<TInt>        siblingIndexStack;
    TInt                parent;
    TInt                siblingIndex;
    TInt                folderId;
    TBool               foundRootFolder = EFalse;
    TPtrC               title;
    TPtrC               url;
    TInt                entryId;
    TInt                feedId;
    TTime               timestamp;
    TInt                unreadCount;
    TInt 				statusCode;
    TInt                freq;

    // Open the various tables
    UseFolderListTableLC(RDbTable::EUpdatable);
    UseFeedTableLC(RDbTable::EUpdatable);

    // A simple stack is used to track the folder-nesting.  Start with the root-folder.
    CleanupClosePushL(parentStack);
    CleanupClosePushL(siblingIndexStack);
    
    parentStack.AppendL(KRootFolderId);    
    siblingIndexStack.AppendL(0);
    
    // Unpack the packed folder.
    while (aPackedFolder.HasNextToken())
        {
        TUint token;
        
        token = aPackedFolder.NextToken();
        
        switch (token)
            {
            case EFolderTokenFolderBegin:
                // Extract the attributes.
                aPackedFolder.ExtractAttributes(title, url, entryId, feedId, timestamp, unreadCount, statusCode, freq);
                
                // The first folder in the PackedFolder is the root-folder.  There is no
                // reason to store this folder in the database, as such this folder is skipped.
                if (!foundRootFolder)
                    {
                    foundRootFolder = ETrue;
                    continue;
                    }

                // Determine the parent and its sibling index.
                parent = parentStack[parentStack.Count() - 1];
                siblingIndex = siblingIndexStack[siblingIndexStack.Count() - 1];
                siblingIndexStack[siblingIndexStack.Count() - 1]++;

                // Add the folder.
				TInt feedId;
                FolderItemAddHelperL(aFolderListId, title, KNullDesC, 
                        ETrue, siblingIndex, parent, folderId, feedId, freq);

                // Push this folder on the stack as the active parent.
                parentStack.AppendL(folderId);
                siblingIndexStack.AppendL(0);
                break;

            case EFolderTokenFolderEnd:
                // Pop this folder off of the stacks.
                parentStack.Remove(parentStack.Count() - 1);
                siblingIndexStack.Remove(siblingIndexStack.Count() - 1);
                break;

            case EFolderTokenFeed:
                // Extract the attributes.
                aPackedFolder.ExtractAttributes(title, url, entryId, feedId, timestamp, unreadCount, statusCode, freq);

                // Determine the parent and its sibling index.
                parent = parentStack[parentStack.Count() - 1];
                siblingIndex = siblingIndexStack[siblingIndexStack.Count() - 1];
                
                // Add the feed.                

				TInt folderId, newfeedId;
                TRAPD( err, FolderItemAddHelperL(aFolderListId, title, url, 
                        EFalse, siblingIndex, parent, folderId, newfeedId, freq) );

                // Ignore problematic ones and continue to the next token
                if( err == KErrNone )
                    {
                    siblingIndexStack[siblingIndexStack.Count() - 1]++;
                    }
                break;               
            }
        }

    CleanupStack::PopAndDestroy(/*siblingIndexStack*/);
    CleanupStack::PopAndDestroy(/*parentStack*/);
    CleanupStack::PopAndDestroy(/*feed table*/);
    CleanupStack::PopAndDestroy(/*folder list table*/);
    }

// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderItemAddL
//
// Add a new entry.
// -----------------------------------------------------------------------------
//
TInt CFeedsDatabase::FolderItemAddL(TInt aFolderListId, const TDesC& aTitle,
        const TDesC& aUrl, TBool aIsFolder, TInt aParentEntryId, TInt aFreq)
    {
    TInt  entryId = 0;
    
    // Open the various tables
    UseFolderListTableLC(RDbTable::EUpdatable);
    UseFeedTableLC(RDbTable::EUpdatable);
    
    // Adjust the sibling indexes of the existing folder items such that this
    // item can be inserted at index zero.
    CreateSiblingIndexHoleL(aParentEntryId, 0, 1);
    
    // Add the folder item at sibling index 0.
	TInt feedId = 0;
    TRAPD( err, FolderItemAddHelperL(aFolderListId, aTitle, aUrl, aIsFolder, 
            0, aParentEntryId, entryId, feedId, aFreq) );
    User::LeaveIfError( err );
    
    // Clean up.
    CleanupStack::PopAndDestroy(/*feed table*/);
    CleanupStack::PopAndDestroy(/*folder list table*/);
    if(feedId != KUnassignedId)
        {
        iFeedsServer->UpdateFeedL(aFolderListId,feedId,EFalse);
        }
    return entryId;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderItemAddHelperL
//
// Add a new entry.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderItemAddHelperL(TInt aFolderListId, const TDesC& aTitle,
                const TDesC& aUrl, TBool aIsFolder, TInt aSiblingIndex, TInt aParentEntryId,
                TInt& aOutFolderId, TInt& aOutFeedId, TInt aFreq)
    {

	HBufC*		safeTitle = NULL;
	TBool		found(ETrue);
	TBuf<K100MaxLen>	NewTitle;
	
	aOutFeedId = KUnassignedId;

	// Get a unique id for this new folder item.
	aOutFolderId = NextFolderItemIdL();

	// Remove any chars that may not be database safe.
	safeTitle = aTitle.AllocLC();

	do
		{
		TInt pos(0);
		if ((pos = safeTitle->Locate('\'')) != KErrNotFound)
			{            
			safeTitle->Des().Delete(pos, 1);
			}
		else
			{
			found = EFalse;
			}
		}
	while (found);
	
    if(safeTitle->Des().Length()>NewTitle.MaxLength()-KResvSpaceForDupTitles)
    {
    	safeTitle->Des().SetLength(NewTitle.MaxLength()-KResvSpaceForDupTitles);
    }
    
    // Fixed for Bug id - JJUN-78VES7 (FeedsServer crashes under IPC attack)
    // It is a common mistake to use Des() to create a TDesC16& reference. 
    // While not incorrect, it is simpler and much more efficient to simply dereference
    // the heap descriptor.

	if( ValidateFeedFolderTitleL(aFolderListId, aParentEntryId, *safeTitle, aIsFolder, NewTitle) )
		{
		// Add it to the feed table if its a new feed.
		if (!aIsFolder)
			{
            aOutFeedId = CommitFeedL(aFolderListId, NewTitle, aUrl, aFreq);
			}
		// Add it to the folder list table.
		CommitFolderListL(aFolderListId, aOutFolderId, aParentEntryId, aSiblingIndex, 
							aIsFolder, aOutFeedId, NewTitle);
		}
		
	CleanupStack::PopAndDestroy(safeTitle);
	}

// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderItemUpdateL
//
// Update an entry.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderItemUpdateL(TInt aFolderItemId, 
        const TDesC& aTitle, const TDesC& aUrl, TInt aFreq)
    {
    TDbSeekKey  folderListKey(aFolderItemId);

    // Open the various tables
    UseFolderListTableLC(RDbTable::EUpdatable);
    TInt  feedId = KUnassignedId;
    TInt  folderListId = 0;
    if (iFolderListTable.SeekL(folderListKey))
        {
        TBool  isFolder;
    //    TInt   folderListId;

        // Update the title in the folder list table.
        iFolderListTable.GetL();
        iFolderListTable.UpdateL();
        
        isFolder = iFolderListTable.ColUint8(iFolderListColSet->ColNo(KIsFolder));
     //   folderListId = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KFolderListId));

        iFolderListTable.SetColL(iFolderListColSet->ColNo(KTitle_100MaxLen), 
                aTitle.Left(K100MaxLen));
            
        // If the entry is a feed then make sure the feed id is still valid.
        if (!isFolder)
            {    
            feedId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KFeedId));
            folderListId = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KFolderListId));
            // Resolve the url to determine if this feed is already subscribed to.  If it's
            // a new feed it is added to the database.
            //if (!FeedIdFromUrlL(aUrl, folderListId, feedId))
            //    {
            //    feedId = CommitFeedL(folderListId, aTitle, aUrl);
            //    }


            UseFeedTableLC(RDbTable::EUpdatable);
            TDbSeekKey  seekKey((TUint16) feedId);

            if (iFeedTable.SeekL(seekKey))
                {
                iFeedTable.GetL();
                iFeedTable.UpdateL();
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            iFeedTable.SetColL(iFeedColSet->ColNo(KTitle_100MaxLen), aTitle.Left(K100MaxLen));
            if (aUrl.Length() > 0)
                {
                WriteLongTextL(iFeedTable, iFeedColSet->ColNo(KFeedUrl), aUrl);
                }
            iFeedTable.SetColL(iFeedColSet->ColNo(KAutoUpdateFreq), aFreq);
            iFeedTable.PutL();
            CleanupStack::PopAndDestroy(/*feed table*/);
            // Set the feed id.
            //iFolderListTable.SetColL(iFolderListColSet->ColNo(KFeedId), feedId);
            }

        iFolderListTable.PutL();
        if(feedId != KUnassignedId)
            {
            iFeedsServer->UpdateFeedL(folderListId,feedId,EFalse);
            }
        }
    
    // Clean up.
    CleanupStack::PopAndDestroy(/*folder list table*/);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderItemDeleteL
//
// Delete the given folder items and any newly unreferenced feeds.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderItemDeleteL(const RArray<TInt>& aFolderItemIds, TBool aSessionCalled)
    {
    RArray<TInt> feedIds;

    CleanupClosePushL(feedIds);
	if(aSessionCalled)
		{
	    iDeleteFolderArray.Reset();
	    for(TInt i = 0; i < aFolderItemIds.Count(); i++)
		    {
			iDeleteFolderArray.AppendL(aFolderItemIds[i]);
		    }
    	}
    // Delete the items, including all the children of any folders.
    FolderItemDeleteHelperL(aFolderItemIds, feedIds);

    // Purge unreferenced feeds.
    for (TInt i = 0; i < feedIds.Count(); i++)
        {                
        PurgeFeedIfNotReferencedL(feedIds[i]);
        }
        
	CleanupStack::PopAndDestroy(/*feedIds*/);
    }
    

// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderListDeleteL
//
// Delete anything under the folder list.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderListDeleteL( TInt aFolderListId )
    {
    HBufC*  query = NULL;
    RArray<TInt> feedIds;

    // Purge the feeds and their items, enclosures.
    CleanupClosePushL(feedIds);  
    AllFeedIdsL( feedIds, aFolderListId );
    for (TInt i = 0; i < feedIds.Count(); i++)
        {                
        FeedPurgeL(feedIds[i], ETrue);
        }       
	CleanupStack::PopAndDestroy(/*feedIds*/);

    // Delete the folder items from the FolderListTable.
    // DELETE FROM FolderListTable WHERE FolderListId = aFolderListId
    _LIT(KQuery, "DELETE FROM FolderListTable WHERE FolderListId = %d");
    query = HBufC::NewLC(KQuery().Length() + KIntLength);                 
    query->Des().Format(KQuery, aFolderListId);   
    // err is KErrNone even multiple records are deleted
    TInt err = iDatabase.Execute(*query);
    CleanupStack::PopAndDestroy(query);
    iFeedsServer->ScheduleUpdateManagerL(aFolderListId);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderItemDeleteHelperL
//
// Delete the given folder items and store the feedIds in aFeedIds.  aFeedIds 
// can then be used to delete any newly unreferenced feeds.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderItemDeleteHelperL(const RArray<TInt>& aFolderItemIds,
        RArray<TInt>& aFeedIds)
    {
    TInt  parentId = 0;
    TInt  err;
    TInt  statusCode;
    
    if (aFolderItemIds.Count() == 0)
        {
        return;
        }
        
    // Use the table.
    UseFolderListTableLC(RDbTable::EUpdatable);
    
    // Delete each of the entries.
    for (TInt i = 0; i < aFolderItemIds.Count(); i++)
        {        
        // Find the entry.
        TDbSeekKey  seekKey(aFolderItemIds[i]);
        
        if (iFolderListTable.SeekL(seekKey))
            {            
            TInt   folderItemId;
            TInt   feedId;
            TBool  isFolder;

            iFolderListTable.GetL();

            // Get the parent, so it can adjust the sibling order.  This only 
            // needs to be done once -- on the first iter...
            if (i == 0)
                {
                parentId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KParentId));
                }

            folderItemId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KFolderItemId));
            feedId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KFeedId));
            isFolder = iFolderListTable.ColUint8(iFolderListColSet->ColNo(KIsFolder));
            statusCode = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KStatus));
            // Delete it.
            iFolderListTable.DeleteL();
            
            // If this is a folder then all of the children need to be deleted too.
            if (isFolder)
                {
                RArray<TInt>  children;
                
                // Get the children.
                CleanupClosePushL(children);                
                FolderItemGetChildrenL(folderItemId, children);
                
                // Delete the children.
                FolderItemDeleteL(children);                
                CleanupStack::PopAndDestroy(/*children*/);                
                }
               
            // Otherwise add the feed-id to the array of potentially unreferenced feeds.
            else
                {
                err = aFeedIds.InsertInOrder(feedId);
                if ((err != KErrNone) && (err != KErrAlreadyExists))
                    {
                    User::Leave(err);
                    }
                }
            if(statusCode != KErrNone && iDeleteFolderArray.Find(folderItemId) != KErrNotFound)
            	{
            	TInt parent = parentId;
            	TInt previousStatus = KErrNone;
        	    while(parent != KRootFolderId)
        	    	{
            		TDbSeekKey  folderListKey(parent);
            		if(iFolderListTable.SeekL(folderListKey))
            			{
            	        iFolderListTable.GetL();
            			parent = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KParentId));
            			previousStatus = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KStatus));
        				iFolderListTable.UpdateL(); 
        				// Folder status value should be -ve of total errorneous folder/feeds contained by it.
        				TInt status = previousStatus + 1;
        				iFolderListTable.SetColL(iFolderListColSet->ColNo(KStatus), status);
        				iFolderListTable.PutL();

            			if(status != KErrNone)
            				{
            					break;
            				}
            			}
        	    	}

            	}
            }
        }
        
    // Fix up the sibling indexes.
    AdjustSiblingIndexesL(parentId);
    
    // Clean up.
    CleanupStack::PopAndDestroy(/*folder list table*/);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderItemMoveL
//
// Move the folder items within their parent.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderItemMoveL(const RArray<TInt>& aFolderItemIds, 
        TInt aNewIndex)
    {
    TInt  parentId = 0;
    TInt  count;
                
    if (aFolderItemIds.Count() == 0)
        {
        return;
        }
        
    // Use the table.
    UseFolderListTableLC(RDbTable::EUpdatable);
    count = iFolderListTable.CountL();
    
    // 1) Move the entries to the end, by changing there sibling indexes.
    for (TInt i = 0; i < aFolderItemIds.Count(); i++)
        {        
        // Find the entry.
        TDbSeekKey  seekKey(aFolderItemIds[i]);
        
        if (iFolderListTable.SeekL(seekKey))
            {
            iFolderListTable.GetL();

            // Get the parent This only needs to be done once -- on the first iter...
            if (i == 0)
                {   
                parentId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KParentId));         
                }
                
            iFolderListTable.UpdateL();
            iFolderListTable.SetColL(iFolderListColSet->ColNo(KSiblingOrder), count + i);
            iFolderListTable.PutL();
            }
        }

    // 2) Create a hole for the entries to be moved to, this also adjusts all of the
    //    indexes to remove any holes that may have been created by the move.
    CreateSiblingIndexHoleL(parentId, aNewIndex, aFolderItemIds.Count());

    // 3) Move the entries into the newly created hole by changing their sibling indexes.
    for (TInt i = 0; i < aFolderItemIds.Count(); i++)
        {        
        // Find the entry.
        TDbSeekKey  seekKey(aFolderItemIds[i]);
        
        if (iFolderListTable.SeekL(seekKey))
            {
            iFolderListTable.GetL();

            iFolderListTable.UpdateL();
            iFolderListTable.SetColL(iFolderListColSet->ColNo(KSiblingOrder), aNewIndex + i);
            iFolderListTable.PutL();
            }
        }
    
    // Clean up.
    CleanupStack::PopAndDestroy(/*folder list table*/);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderItemMoveToL
//
// Move the entries to another parent.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderItemMoveToL(const RArray<TInt>& aFolderItemIds, 
        TInt aNewParent)
    {
    TInt  parentId = 0;
    TInt  statusCode = KErrNone;

    if (aFolderItemIds.Count() == 0)
        {
        return;
        }
        
    // Use the table.
    UseFolderListTableLC(RDbTable::EUpdatable);
    
    // Move each of the entries.
    for (TInt i = 0; i < aFolderItemIds.Count(); i++)
        {        
        // Find the entry.
        TDbSeekKey seekKey(aFolderItemIds[i]);
        
        if (iFolderListTable.SeekL(seekKey))
            {
            iFolderListTable.GetL();

            // Create a hole in the new parent folder for the items to be moved.  This only 
            // needs to be done once -- on the first iter...
            if (i == 0)
                {                   
                parentId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KParentId));         
                CreateSiblingIndexHoleL(aNewParent, 0, aFolderItemIds.Count());
                }
            statusCode = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KStatus));
            // Change the new parent and sibling index.  The moved folder items are placed
            // at the beginning of the new parent.
            iFolderListTable.UpdateL();
            iFolderListTable.SetColL(iFolderListColSet->ColNo(KParentId), aNewParent);
            iFolderListTable.SetColL(iFolderListColSet->ColNo(KSiblingOrder), i);
            iFolderListTable.PutL();
            
            if(statusCode != KErrNone)
            	{
            	TInt parent = parentId;
            	TInt previousStatus = KErrNone;
        	    while(parent != KRootFolderId)
        	    	{
            		TDbSeekKey  folderListKey(parent);
            		if(iFolderListTable.SeekL(folderListKey))
            			{
            	        iFolderListTable.GetL();
            			parent = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KParentId));
            			previousStatus = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KStatus));
        				iFolderListTable.UpdateL();
        				TInt status = previousStatus + 1;
        				iFolderListTable.SetColL(iFolderListColSet->ColNo(KStatus), status);
        				iFolderListTable.PutL();

            			if(status != KErrNone)
            				{
            					break;
            				}
            			}
        	    	}
            	parent = aNewParent;
        	    while(parent != KRootFolderId)
        	    	{
            		TDbSeekKey  folderListKey(parent);
            		if(iFolderListTable.SeekL(folderListKey))
            			{
            	        iFolderListTable.GetL();
            			parent = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KParentId));
            			previousStatus = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KStatus));
        				iFolderListTable.UpdateL();
        				iFolderListTable.SetColL(iFolderListColSet->ColNo(KStatus), previousStatus - 1);
        				iFolderListTable.PutL();

            			if(previousStatus != KErrNone)
            				{
            					break;
            				}
            			}
        	    	}
            		
            	}
            }
        }
    
    // Fix up the sibling indexes of the old parent.
    AdjustSiblingIndexesL(parentId);
    
    // Clean up.
    CleanupStack::PopAndDestroy(/*folder list table*/);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::AdjustSiblingIndexesL
//
// Reorders the sibling indexes.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::AdjustSiblingIndexesL(TInt aParentId)
    {
    RDbView  view;
    HBufC*   query = NULL;
    TInt     siblingIndex = 0;
    
    // Create a view given this select...    
    // SELECT SiblingOrder FROM FolderListTable WHERE ParentId = aParentId ORDER BY SiblingOrder    
    _LIT(KQuery, "SELECT SiblingOrder FROM FolderListTable WHERE ParentId = %d ORDER BY SiblingOrder");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength); 
            
    query->Des().Format(KQuery, aParentId);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EUpdatable));

    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
        
    // Reorder each entry in the given folder-list and parent folder.
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            // Get the current row.
            view.GetL();

            // Update the order.
            view.UpdateL();
            view.SetColL(colSet->ColNo(KSiblingOrder), siblingIndex++);
            view.PutL();
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::CreateSiblingIndexHoleL
//
// Creates a hole in the sibling index.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CreateSiblingIndexHoleL(TInt aParentId, TInt aNewIndex, TInt aHoleSize)
    {
    RDbView  view;
    HBufC*   query = NULL;
    TInt     siblingIndex = 0;
    
    // Create a view given this select...    
    // SELECT SiblingOrder FROM FolderListTable 
    // WHERE ParentId = aParentId
    // ORDER BY SiblingOrder
    _LIT(KQuery, "SELECT SiblingOrder FROM FolderListTable WHERE ParentId = %d ORDER BY SiblingOrder");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength); 
                        
    query->Des().Format(KQuery, aParentId);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EUpdatable));

    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
        
    // Reorder each entry in the given folder-list and parent folder.
    if (view.EvaluateAll() >= 0)
        {
        TInt  index = 0;

        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            // Get the current row.
            view.GetL();

            siblingIndex = view.ColUint16(colSet->ColNo(KSiblingOrder));
            
            if (index == aNewIndex)
                {
                index += aHoleSize;
                }
                
            // Update the order.
            if (index != siblingIndex)
                {                
                view.UpdateL();
                view.SetColL(colSet->ColNo(KSiblingOrder), index);
                view.PutL();
                }
                
            index++;
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::FolderItemGetChildrenL
//
// Extract the folder-item-ids of the children of the given parent.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FolderItemGetChildrenL(TInt aFolderItemId, RArray<TInt>& aChildren)
    {
    RDbView  view;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    // SELECT FolderItemId FROM FolderListTable WHERE ParentId = aFolderItemId 
    _LIT(KQuery, "SELECT FolderItemId FROM FolderListTable WHERE ParentId = %d");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength); 
                        
    query->Des().Format(KQuery, aFolderItemId);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));

    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);

    // Extract and add the children's folder-item-id to aChildren.        
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            TInt   folderItemId;
            
            // Get the current row.
            view.GetL();
        
            // Get the fields
            folderItemId = view.ColUint16(colSet->ColNo(KFolderItemId));
            
            // Append it.
            aChildren.AppendL(folderItemId);
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::FeedUpdateItemStatusL
//
// Update the status of each of the items in the given feed.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FeedUpdateItemStatusL(TInt aFeedId, 
        const RArray<TInt>& aItemIds, const RArray<TInt>& aItemStatus, TInt aUnreadCount)
    {
    HBufC*   query = NULL;
    RDbView  view;
    
    if (aItemIds.Count() == 0)
        {
        return;
        }
        
    // Perpare the query.    
    // SELECT ItemId, ItemStatus FROM ItemTable WHERE FeedId = aFeedId
    _LIT(KQuery, "SELECT ItemId, ItemStatus FROM ItemTable WHERE FeedId = %d");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength); 
                        
    query->Des().Format(KQuery, aFeedId);

    // Execute the query.
    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EUpdatable));
    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
        
    // Purge the old items.
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            TInt  itemId;
            TInt  itemStatus;
            TInt  pos;
            
            // Get the current row.
            view.GetL();

            // Get the itemId and current Status.
            itemId = view.ColUint16(colSet->ColNo(KItemId));
            itemStatus = view.ColUint16(colSet->ColNo(KItemStatus));
            
            // If found in aItemIds update its status.
            if ((pos = aItemIds.Find(itemId)) != KErrNotFound)
                {
                if (aItemStatus[pos] != itemStatus)
                    {                    
                    view.UpdateL();
                    view.SetColL(colSet->ColNo(KItemStatus), aItemStatus[pos]);
                    view.PutL();
                    }
                }
            } // for loop

        // Prep the feed table.
        UseFeedTableLC(RDbTable::EUpdatable);
        // update the unread count for the feed
        TDbSeekKey  seekKey((TUint16) aFeedId);
        if (iFeedTable.SeekL(seekKey))
            {        
            // Write the count
            iFeedTable.GetL();
            iFeedTable.UpdateL();
            iFeedTable.SetColL(iFeedColSet->ColNo(KUnreadCount), aUnreadCount);    
            iFeedTable.PutL();
            }
        CleanupStack::PopAndDestroy(/*Feed Table*/);
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    }
    

// -----------------------------------------------------------------------------
// CFeedsDatabase::PurgeFeedIfNotReferencedL
//
// Deletes the feed if the feedId isn't referenced in the folder-list table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::PurgeFeedIfNotReferencedL(TInt aFeedId)
    {
    RDbView  view;
    HBufC*   query = NULL;
    TBool    deleted = EFalse;
    
    // Create a view given this select...    
    // SELECT FolderItemId FROM FolderListTable WHERE FeedId = aFeedId 
    _LIT(KQuery, "SELECT FolderItemId FROM FolderListTable WHERE FeedId = %d");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength); 
                        
    query->Des().Format(KQuery, aFeedId);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));

    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);

    // Evalulate the query if no rows are returned then purge the feed.
    if (view.Evaluate() >= 0)
        {
        view.FirstL();
        if (!view.AtRow())
            {
            FeedPurgeL(aFeedId, ETrue);
            deleted = ETrue;
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);    
    if(deleted)
        {
        iFeedsServer->UpdateFeedL(KNoFolderListId,aFeedId,ETrue);
        }
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::FeedPurgeL
//
// Removes the feed's items also deletes the feed if aDeleteFeed is ETrue.
// It is only safe to call this method with aDeleteFeed set to ETrue if
// aFeedId is not found in the FolderListTable.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::FeedPurgeL(TInt aFeedId, TBool aDeleteFeed)
    {
    HBufC*  query = NULL;
        
    // Delete the enclosures from the Enclosure Table.  This is done first to 
    // Ensure that enclosures don't get ophaned if the delete-items query leaves.
    // DELETE FROM EnclosureTable WHERE FeedId = aFeedId
    _LIT(KQueryE, "DELETE FROM EnclosureTable WHERE FeedId = %d");

    query = HBufC::NewLC(KQueryE().Length() + KIntLength);                 
    query->Des().Format(KQueryE, aFeedId);
    
    // err is KErrNone even multiple records are deleted
    TInt err = iDatabase.Execute(*query);
    CleanupStack::PopAndDestroy(query);

    // Delete the items from the Item Table
    // DELETE FROM ItemTable WHERE FeedId = aFeedId
    _LIT(KQueryI, "DELETE FROM ItemTable WHERE FeedId = %d");
    query = HBufC::NewLC(KQueryI().Length() + KIntLength);                 
    query->Des().Format(KQueryI, aFeedId);
    
    err = iDatabase.Execute(*query);
    CleanupStack::PopAndDestroy(query);

    // Delete the feed from the Feed Table
    if (aDeleteFeed)
        {        
        // DELETE FROM FeedTable WHERE FeedId = aFeedId
        _LIT(KQueryF, "DELETE FROM FeedTable WHERE FeedId = %d");
        query = HBufC::NewLC(KQueryF().Length() + KIntLength);                 
        query->Des().Format(KQueryF, aFeedId);
        
        err = iDatabase.Execute(*query);
        CleanupStack::PopAndDestroy(query);
        }
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::PurgeOldItemsL
//
// Purge any old items in the given feed.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::PurgeOldItemsL(TInt aFeedId, const TTime& aTimestamp)
    {
    HBufC*                        query = NULL;
    TBuf<KMaxTimeFormatSpec * 2>  tsStr;
    TBuf<KMaxTimeFormatSpec * 2>  temp;    
    RDbView                       view;
        // Convert aTimestamp to the query form -- i.e #1997-12-31 23:59:59#.
    aTimestamp.FormatL(tsStr, TShortDateFormatSpec());
    tsStr.Append(KSpace);
    aTimestamp.FormatL(temp, TTimeFormatSpec());
    tsStr.Append(temp);

    // Perpare the query.    
    // SELECT ItemId, Date FROM ItemTable WHERE FeedId = aFeedId AND Date < #tsStr#
    _LIT(KQuery, "SELECT ItemId, Date FROM ItemTable WHERE FeedId = %d AND Date < #%S#");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength + tsStr.Length()); 
                        
    query->Des().Format(KQuery, aFeedId, &tsStr);

    // Execute the query.
    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EUpdatable));
    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
        
    // Purge the old items.
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            TInt    itemId;
            
            // Get the current row.
            view.GetL();

            // Get the itemId and date.
            itemId = view.ColUint16(colSet->ColNo(KItemId));

            // Purge the associated enclosures.  This is done first to ensure that 
            // enclosures don't get ophaned if the delete-item statement leaves.
            PurgeEnclosuresL(aFeedId, itemId);
            
            // Delete the item.
            view.DeleteL();
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::PurgeEnclosuresL
//
// Purge the associated enclosures
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::PurgeEnclosuresL(TInt aFeedId, TInt aItemId)
    {
    HBufC*  query = NULL;

    // DELETE FROM EnclosureTable WHERE FeedId = aFeedId AND ItemId = aItemId
    _LIT(KQuery, "DELETE FROM EnclosureTable WHERE FeedId = %d AND ItemId = %d");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength + KIntLength); 
                        
    query->Des().Format(KQuery, aFeedId, aItemId);
    
    User::LeaveIfError(iDatabase.Execute(*query));
    CleanupStack::PopAndDestroy(query);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::PurgeOtherItemsL
//
// Purge all items not found in the given array.
// -----------------------------------------------------------------------------
//
TInt CFeedsDatabase::PurgeOtherItemsL(TInt aFeedId, const RArray<TInt>& aItemIds)
    {
    HBufC*   query = NULL;
    RDbView  view;
    TInt purgedUnreadNewCount = 0;

    // SELECT ItemId, ItemStatus FROM ItemTable WHERE FeedId = aFeedId
    _LIT(KQuery, "SELECT ItemId, ItemStatus FROM ItemTable WHERE FeedId = %d");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength); 
                        
    query->Des().Format(KQuery, aFeedId);

    // Execute the query.
    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EUpdatable));
    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
        
    // Purge the old items.
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            TInt    itemId;
            
            // Get the current row.
            view.GetL();

            itemId = view.ColUint16(colSet->ColNo(KItemId));
            
            // If this item isn't in aItemIds then delete the item and 
            // its enclosures.
            if (aItemIds.Find(itemId) == KErrNotFound)
                {                
                TInt itemStatus = view.ColUint8(colSet->ColNo(KItemStatus));
                if( itemStatus == EUnreadItem || itemStatus == ENewItem )
                    {
                    purgedUnreadNewCount++;
                    }
                // Purge the associated enclosures.  This is done first to ensure that 
                // enclosures don't get ophaned if the delete-item statement leaves.
                PurgeEnclosuresL(aFeedId, itemId);
                
                // Delete the item.
                view.DeleteL();
                }
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);

    return purgedUnreadNewCount;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ResetDatabaseL
//
// Reset the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ResetDatabaseL()
    {
    TBuf<32> format;
    format.Copy(KSecure);
    format.Append(KSecureUid.Name());
    
    // Create the database file.
    TInt err = iDatabase.Create( iDBs, iDatabasePath, format); 
    User::LeaveIfError( err );
        
    // Create the tables.
    CreateVersionTableL();
    CreateFolderListTableL();
    CreateFeedTableL();
    CreateFeedItemTableL();
    CreateItemEnclosureTableL();
    CreateSettingsTableL();
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::CreateVersionTableL
//
// Creates the version table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CreateVersionTableL()
    {
    CDbColSet*  colSet = NULL;
    
    // Create the table's column set.
    colSet = CDbColSet::NewLC();
    colSet->AddL(TDbCol(KVersion,         EDbColText16));
    colSet->AddL(TDbCol(KVersionId,       EDbColUint16));
    
    // Create the table.
    User::LeaveIfError(iDatabase.CreateTable(KVersionTable, *colSet));
    CleanupStack::PopAndDestroy(colSet);

    // Prep the settings table.
    UseVersionTableLC(RDbTable::EUpdatable);
    
    // Update the database.  There is only one row in this table...
    iVersionTable.FirstL();
    
    if (iVersionTable.AtRow())
        {        
        iVersionTable.GetL();
        iVersionTable.UpdateL();
        }
    else
        {
        iVersionTable.Reset();
        iVersionTable.InsertL();
        }
    
    iVersionTable.SetColL(iVersionColSet->ColNo(KVersionId), KVersion71);
    
    iVersionTable.PutL();
    CleanupStack::PopAndDestroy(/* version table */);
    }
    

// -----------------------------------------------------------------------------
// CFeedsDatabase::CreateFolderListTableL
//
// Creates the folder-list table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CreateFolderListTableL()
    {
    CDbColSet*  colSet = NULL;
    TDbKeyCol   folderItemIdCol(KFolderItemId);
    CDbKey*     indexKey = NULL;    
    
    // Create the table's column set.
    colSet = CDbColSet::NewLC();
    colSet->AddL(TDbCol(KFolderListId,    EDbColInt32));
    colSet->AddL(TDbCol(KFolderItemId,    EDbColUint16));
    colSet->AddL(TDbCol(KParentId,        EDbColUint16));
    colSet->AddL(TDbCol(KSiblingOrder,    EDbColUint16));
    colSet->AddL(TDbCol(KIsFolder,        EDbColUint8));
    colSet->AddL(TDbCol(KFeedId,          EDbColUint16));
    colSet->AddL(TDbCol(KTitle_100MaxLen, EDbColText16, K100MaxLen));
    colSet->AddL(TDbCol(KStatus,		  EDbColInt32));
    // Create the table.
    User::LeaveIfError(iDatabase.CreateTable(KFolderListTable, *colSet));
    CleanupStack::PopAndDestroy(colSet);

    // Index the table.    
    indexKey = CDbKey::NewLC();

    indexKey->AddL(folderItemIdCol);
    User::LeaveIfError(iDatabase.CreateIndex(KFolderListTableIndex, 
            KFolderListTable, *indexKey));
    CleanupStack::PopAndDestroy(indexKey);
    }
    

// -----------------------------------------------------------------------------
// CFeedsDatabase::CreateFeedTableL
//
// Creates the feed table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CreateFeedTableL()
    {
    CDbColSet*  colSet = NULL;
    TDbKeyCol   feedIdCol(KFeedId);
    CDbKey*     indexKey = NULL;    
    
    // Create the table's column set.
    colSet = CDbColSet::NewLC();
    
    colSet->AddL(TDbCol(KFolderListId,      EDbColInt32));
    colSet->AddL(TDbCol(KFeedId,            EDbColUint16));
    colSet->AddL(TDbCol(KDate,              EDbColDateTime));
    colSet->AddL(TDbCol(KTitle_100MaxLen,   EDbColText16, K100MaxLen));
    colSet->AddL(TDbCol(KFeedUrl,           EDbColLongText16));
    colSet->AddL(TDbCol(KDescription,       EDbColLongText16));
    colSet->AddL(TDbCol(KWebUrl,            EDbColLongText16));
    colSet->AddL(TDbCol(KUnreadCount,       EDbColUint16));
    colSet->AddL(TDbCol(KAutoUpdateFreq,      EDbColUint32));
    
    // Create the table.
    User::LeaveIfError(iDatabase.CreateTable(KFeedTable, *colSet));
    CleanupStack::PopAndDestroy(colSet);

    // Index the table.    
    indexKey = CDbKey::NewLC();
    
    indexKey->AddL(feedIdCol);
    User::LeaveIfError(iDatabase.CreateIndex(KFeedTableIndex, KFeedTable, *indexKey));
    CleanupStack::PopAndDestroy(indexKey);
    }
    

// -----------------------------------------------------------------------------
// CFeedsDatabase::CreateFeedItemTableL
//
// Creates the feed-list table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CreateFeedItemTableL()
    {
    CDbColSet*  colSet = NULL;
    TDbKeyCol   feedIdCol(KFeedId);
    TDbKeyCol   itemIdCol(KItemId);
    CDbKey*     indexKey = NULL;    
    
    // Create the table's column set.
    colSet = CDbColSet::NewLC();
    
    colSet->AddL(TDbCol(KItemId,            EDbColUint16));
    colSet->AddL(TDbCol(KFeedId,            EDbColUint16));
    colSet->AddL(TDbCol(KDate,              EDbColDateTime));
    colSet->AddL(TDbCol(KItemStatus,        EDbColUint8));
    colSet->AddL(TDbCol(KTitle_100MaxLen,   EDbColText16, K100MaxLen));
    colSet->AddL(TDbCol(KDescription,       EDbColLongText16));
    colSet->AddL(TDbCol(KWebUrl,            EDbColLongText16));
    colSet->AddL(TDbCol(KItemIdStr,         EDbColLongText16));
    
    // Create the table.
    User::LeaveIfError(iDatabase.CreateTable(KItemTable, *colSet));
    CleanupStack::PopAndDestroy(colSet);

    // Index the table.    
    indexKey = CDbKey::NewLC();
    
    indexKey->AddL(itemIdCol);
    User::LeaveIfError(iDatabase.CreateIndex(KItemTableIndex, KItemTable, *indexKey));
    CleanupStack::PopAndDestroy(indexKey);
    }
    

// -----------------------------------------------------------------------------
// CFeedsDatabase::CreateItemEnclosureTableL
//
// Creates the item-enclosure table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CreateItemEnclosureTableL()
    {
    CDbColSet*  colSet = NULL;
    TDbKeyCol   feedIdCol(KFeedId);
    TDbKeyCol   itemIdCol(KItemId);
    TDbKeyCol   enclosureIdCol(KEnclosureId);        
    CDbKey*     indexKey = NULL;    
    
    // Create the table's column set.
    colSet = CDbColSet::NewLC();
    
    colSet->AddL(TDbCol(KEnclosureId,               EDbColUint16));
    colSet->AddL(TDbCol(KItemId,                    EDbColUint16));
    colSet->AddL(TDbCol(KFeedId,                    EDbColUint16));
    colSet->AddL(TDbCol(KLength_100MaxLen,          EDbColText16, K100MaxLen));
    colSet->AddL(TDbCol(KTitle_100MaxLen,           EDbColText16, K100MaxLen));
    colSet->AddL(TDbCol(KContentType_100MaxLen,     EDbColText16, K100MaxLen));
    colSet->AddL(TDbCol(KWebUrl,                    EDbColLongText16));
    
    // Create the table.
    User::LeaveIfError(iDatabase.CreateTable(KEnclosureTable, *colSet));
    CleanupStack::PopAndDestroy(colSet);

    // Index the table.    
    indexKey = CDbKey::NewLC();
    
    indexKey->AddL(feedIdCol);
    indexKey->AddL(itemIdCol);
    indexKey->AddL(enclosureIdCol);
    User::LeaveIfError(iDatabase.CreateIndex(KEnclosureTableIndex, KEnclosureTable, *indexKey));
    CleanupStack::PopAndDestroy(indexKey);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::CreateSettingsTableL
//
// Creates the settings table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CreateSettingsTableL()
    {
    CDbColSet*  colSet = NULL;
    TDbKeyCol   folderListIdCol( KFolderListId );
    CDbKey*     indexKey = NULL;    
    
    // Create the table's column set.
    colSet = CDbColSet::NewLC();
    colSet->AddL(TDbCol(KFolderListId,				EDbColInt32));
    colSet->AddL(TDbCol(KAutoUpdate,				EDbColUint8));
    colSet->AddL(TDbCol(KAutoUpdateWhileRoaming,	EDbColUint8));
    colSet->AddL(TDbCol(KAccessPoint,				EDbColUint32));
    colSet->AddL(TDbCol(KAutoUpdateFreq,			EDbColUint32));
    colSet->AddL(TDbCol(KLastAutoUpdate,			EDbColDateTime));
    
    // Create the table.
    User::LeaveIfError(iDatabase.CreateTable(KSettingsTable, *colSet));
    CleanupStack::PopAndDestroy(colSet);

    // Index the table.    
    indexKey = CDbKey::NewLC();

    indexKey->AddL( folderListIdCol );
    User::LeaveIfError(iDatabase.CreateIndex(KSettingsTableIndex, 
            KSettingsTable, *indexKey));
    CleanupStack::PopAndDestroy(indexKey);
    }
    

// -----------------------------------------------------------------------------
// CFeedsDatabase::NextFolderItemIdL
//
// Returns an available folder-item id.
// -----------------------------------------------------------------------------
//
TInt CFeedsDatabase::NextFolderItemIdL()
    {
    TBool     found;
    TUint16   id;
    
    // Prep the folder table.
    UseFolderListTableLC(RDbTable::EReadOnly);
    
    // Search for a unquie id.
    do
        {
        id = (TUint16) Math::Random();
        TDbSeekKey  seekKey(id);
        
        found = iFolderListTable.SeekL(seekKey);
            
        // Don't allow the root folder id to be picked.
        if (id == KRootFolderId)
            {
            found = ETrue;
            }
        }
    while (found);
    
    CleanupStack::PopAndDestroy(/*Folder list Table*/);
    return id;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::NextFeedIdL
//
// Returns an available feed id.
// -----------------------------------------------------------------------------
//
TInt CFeedsDatabase::NextFeedIdL()
    {
    TBool     found = ETrue;
    TUint16   id;
    
    // Prep the folder table.
    UseFeedTableLC(RDbTable::EReadOnly);
    
    // Search for a unquie id.
    do
        {
        id = (TUint16) Math::Random();
        
        // A feed id of 0 is special (meaning unassigned).
        if (id != 0)
            {            
            TDbSeekKey  seekKey(id);
            
            found = iFeedTable.SeekL(seekKey);
            }
        }
    while (found);
    
    CleanupStack::PopAndDestroy(/*Feed Table*/);
    return id;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::NextItemIdL
//
// Returns an available item id.
// -----------------------------------------------------------------------------
//
TInt CFeedsDatabase::NextItemIdL()
    {
    TBool     found = ETrue;
    TUint16   id;
    
    // Prep the folder table.
    UseItemTableLC(RDbTable::EReadOnly);
    
    // Search for a unquie id.
    do
        {
        id = (TUint16) Math::Random();
        
        // A item id of 0 is special (meaning unassigned).
        if (id != 0)
            {            
            TDbSeekKey  seekKey(id);
            
            found = iItemTable.SeekL(seekKey);
            }
        }
    while (found);
    
    CleanupStack::PopAndDestroy(/*item Table*/);
    return id;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::CommitFolderListL
//
// Commit the folder entry to the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CommitFolderListL(TInt aFolderListId, TInt aFolderItemId, 
        TInt aParentId, TInt aSiblingIndex, TBool aIsFolder, TInt aFeedId, 
        const TDesC& aTitle)
    {
    if (aFeedId == KUnassignedId)
        {
        aFeedId = 0;
        }
        
    // Update the database.
    iFolderListTable.Reset();
    iFolderListTable.InsertL();
    
    iFolderListTable.SetColL(iFolderListColSet->ColNo(KFolderListId), aFolderListId);
    iFolderListTable.SetColL(iFolderListColSet->ColNo(KFolderItemId), aFolderItemId);
    iFolderListTable.SetColL(iFolderListColSet->ColNo(KParentId), aParentId);
    iFolderListTable.SetColL(iFolderListColSet->ColNo(KSiblingOrder), aSiblingIndex);
    iFolderListTable.SetColL(iFolderListColSet->ColNo(KIsFolder), aIsFolder);
    iFolderListTable.SetColL(iFolderListColSet->ColNo(KFeedId), aFeedId);
    iFolderListTable.SetColL(iFolderListColSet->ColNo(KTitle_100MaxLen), aTitle.Left(K100MaxLen));
    iFolderListTable.SetColL(iFolderListColSet->ColNo(KStatus), KErrNone);
        
    iFolderListTable.PutL();
    }

// -----------------------------------------------------------------------------
// CFeedsDatabase::CommitFeedL
//
// Commit the feed to the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CommitFeedL(TInt aFolderListId, TBool aIsNewFeed, TInt aFeedId, 
        const RArray<TAttribute>& aAttributes, const TTime& aDefaultTime, TInt aUnreadCount)
    {
    TPtrC  title(KNullDesC);
    TPtrC  description(KNullDesC);
    TPtrC  link(KNullDesC);
    TPtrC  timeStamp(KNullDesC);
    TPtrC  feedUrl(KNullDesC);
    TInt unreadCount = aUnreadCount;

    // Get the values.
    for (TInt i = 0; i < aAttributes.Count(); i++)
        {
        switch (aAttributes[i].token)
            {
            case EFeedAttributeTitle:
                title.Set(aAttributes[i].value);
                break;
                
            case EFeedAttributeLink:
                link.Set(aAttributes[i].value);
                break;
                
            case EFeedAttributeDescription:
                description.Set(aAttributes[i].value);
                break;
                
            case EFeedAttributeFeedUrl:
                feedUrl.Set(aAttributes[i].value);
                break;
                
            case EFeedAttributeTimestamp:
                timeStamp.Set(aAttributes[i].value);
                break;                
            }
        }
    
    // If timeStamp was provided convert it into a TTime otherwise just
    // use the default time.
    TTime  date = aDefaultTime;

    // TODO: Two timestamps are needed -- one for tracking when the
    //       author changed their feed and one for the last time the
    //       feed was updated.
#if 0    
    if (timeStamp.Length() > 0)
        {
        TLex16  lex(timeStamp);
        TInt64  ts;
        
        User::LeaveIfError(lex.Val(ts));
        date = ts;
        }
#endif

    // If this is a new feed then get ready to insert a new entry.
    if (aIsNewFeed)
        {
     
        iFeedTable.Reset();
        iFeedTable.InsertL();
    
        }
        
    // Otherwise get ready to update the existing entry.
    else
        {
        TDbSeekKey  seekKey((TUint16) aFeedId);
        
        if (iFeedTable.SeekL(seekKey))
            {        
            iFeedTable.GetL();
            iFeedTable.UpdateL();
            }
        else
            {
            User::Leave(KErrCorrupt);
            }

        unreadCount = iFeedTable.ColUint16(iFeedColSet->ColNo(KUnreadCount));
        unreadCount += aUnreadCount;    
        }
    // Write the entry. aFolderListId       
    iFeedTable.SetColL(iFeedColSet->ColNo(KFolderListId), aFolderListId);
    iFeedTable.SetColL(iFeedColSet->ColNo(KFeedId), aFeedId);
    iFeedTable.SetColL(iFeedColSet->ColNo(KDate), date);
    iFeedTable.SetColL(iFeedColSet->ColNo(KTitle_100MaxLen), title.Left(K100MaxLen));
    WriteLongTextL(iFeedTable, iFeedColSet->ColNo(KFeedUrl), feedUrl);
    WriteLongTextL(iFeedTable, iFeedColSet->ColNo(KDescription), description);
    WriteLongTextL(iFeedTable, iFeedColSet->ColNo(KWebUrl), link);
    iFeedTable.SetColL(iFeedColSet->ColNo(KUnreadCount), unreadCount);    
        
    iFeedTable.PutL();

    if (iIsFolderTableUpdateNeeded)
        {
        //update the folder table.
        TInt entryId;
        EntryIdFromFeedIdL(aFeedId, aFolderListId, entryId);
        FolderItemUpdateL(entryId, title, KNullDesC, KAutoUpdatingOff);
        }
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::CommitFeedL
//
// Commit a new feed to the database.
// -----------------------------------------------------------------------------
//
TInt CFeedsDatabase::CommitFeedL(TInt aFolderListId, const TDesC& aTitle, const TDesC& aUrl, TInt aFreq)
    {    
    RArray<TAttribute>  attributes;
    TAttribute          attribute;
    TTime               zero(0);
    TInt                feedId;
    TInt                zeroCount = 0;
    
    UseFeedTableLC(RDbTable::EUpdatable);

    CleanupClosePushL(attributes);
    attributes.Reset();        

    // Add the title to the attribute list.
    attribute.token = EFeedAttributeTitle;
    attribute.value.Set(aTitle);
    attributes.AppendL(attribute);

    // Add the url to the attribute list.
    attribute.token = EFeedAttributeFeedUrl;
    attribute.value.Set(aUrl);
    attributes.AppendL(attribute);

    // Get a id for the feed.
    feedId = NextFeedIdL();

    // Add an entry for the feed in the feed table.
    CommitFeedL(aFolderListId, ETrue, feedId, attributes, zero, zeroCount, aFreq);

    // Clean up.
    CleanupStack::PopAndDestroy(/*attributes*/);  
    CleanupStack::PopAndDestroy(/*feed table*/);  
    
    return feedId;  
    }
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::CommitFeedL
//
// Commit the feed to the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CommitFeedL(TInt aFolderListId, TBool aIsNewFeed, TInt aFeedId, 
        const RArray<TAttribute>& aAttributes, const TTime& aDefaultTime, TInt aUnreadCount,TInt aFreq)
    {
    TPtrC  title(KNullDesC);
    TPtrC  description(KNullDesC);
    TPtrC  link(KNullDesC);
    TPtrC  timeStamp(KNullDesC);
    TPtrC  feedUrl(KNullDesC);
    TInt unreadCount = aUnreadCount;

    // Get the values.
    for (TInt i = 0; i < aAttributes.Count(); i++)
        {
        switch (aAttributes[i].token)
            {
            case EFeedAttributeTitle:
                title.Set(aAttributes[i].value);
                break;

            case EFeedAttributeLink:
                link.Set(aAttributes[i].value);
                break;

            case EFeedAttributeDescription:
                description.Set(aAttributes[i].value);
                break;

            case EFeedAttributeFeedUrl:
                feedUrl.Set(aAttributes[i].value);
                break;

            case EFeedAttributeTimestamp:
                timeStamp.Set(aAttributes[i].value);
                break;                
            }
        }

    // If timeStamp was provided convert it into a TTime otherwise just
    // use the default time.
    TTime  date = aDefaultTime;

    // TODO: Two timestamps are needed -- one for tracking when the
    //       author changed their feed and one for the last time the
    //       feed was updated.
#if 0
    if (timeStamp.Length() > 0)
        {
        TLex16  lex(timeStamp);
        TInt64  ts;
        
        User::LeaveIfError(lex.Val(ts));
        date = ts;
        }
#endif

    // If this is a new feed then get ready to insert a new entry.
    if (aIsNewFeed)
        {
        iFeedTable.Reset();
        iFeedTable.InsertL();
        }

    // Otherwise get ready to update the existing entry.
    else
        {
        TDbSeekKey  seekKey((TUint16) aFeedId);

        if (iFeedTable.SeekL(seekKey))
            {        
            iFeedTable.GetL();
            iFeedTable.UpdateL();
            }
        else
            {
            User::Leave(KErrCorrupt);
            }

        unreadCount = iFeedTable.ColUint16(iFeedColSet->ColNo(KUnreadCount));
        unreadCount += aUnreadCount;
        }
    // Write the entry. aFolderListId
    iFeedTable.SetColL(iFeedColSet->ColNo(KFolderListId), aFolderListId);
    iFeedTable.SetColL(iFeedColSet->ColNo(KFeedId), aFeedId);
    iFeedTable.SetColL(iFeedColSet->ColNo(KDate), date);
    iFeedTable.SetColL(iFeedColSet->ColNo(KTitle_100MaxLen), title.Left(K100MaxLen));
    WriteLongTextL(iFeedTable, iFeedColSet->ColNo(KFeedUrl), feedUrl);
    WriteLongTextL(iFeedTable, iFeedColSet->ColNo(KDescription), description);
    WriteLongTextL(iFeedTable, iFeedColSet->ColNo(KWebUrl), link);
    iFeedTable.SetColL(iFeedColSet->ColNo(KUnreadCount), unreadCount);
    iFeedTable.SetColL(iFeedColSet->ColNo(KAutoUpdateFreq), aFreq);

    iFeedTable.PutL();

    }
// -----------------------------------------------------------------------------
// CFeedsDatabase::CommitItemL
//
// Commit the item to the database.  The itemIdStr is also appended to aItemIdStrs.
// -----------------------------------------------------------------------------
// 
TBool CFeedsDatabase::CommitItemL(TInt aItemId, TInt aFeedId, 
        const RArray<TAttribute>& aAttributes, RArray<TInt>& aItemIds)
    {
    TPtrC  timeStamp(KNullDesC);
    TPtrC  title(KNullDesC);
    TPtrC  description(KNullDesC);
    TPtrC  link(KNullDesC);
    TPtrC  itemIdStr(KNullDesC);
    TBool  isNewItem(EFalse);

    // Get the values.
    for (TInt i = 0; i < aAttributes.Count(); i++)
        {
        switch (aAttributes[i].token)
            {
            case EItemAttributeIdStr:
                itemIdStr.Set(aAttributes[i].value);
                break;                

            case EItemAttributeTitle:
                title.Set(aAttributes[i].value);
                break;
                
            case EItemAttributeDescription:
                description.Set(aAttributes[i].value);
                break;                

            case EItemAttributeLink:
                link.Set(aAttributes[i].value);
                break;                
                
            case EItemAttributeTimestamp:
                timeStamp.Set(aAttributes[i].value);
                break;                
            }
        }

    // If timeStamp was provided convert it into a TTime otherwise just
    // use the current time.
    TTime  date;

    if (timeStamp.Length() > 0)
        {
        TLex16  lex(timeStamp);
        TInt64  ts;
        
        User::LeaveIfError(lex.Val(ts));
        date = ts;
        }
    else
        {
        date.UniversalTime();
        }

    // Ignore the item if it is already in the database.
    // If the timestamp chnages the item needs to be updated 
    // rather than inserted.
    TInt  id;

    if (FindItemL(aFeedId, itemIdStr, id))
        {
        TDbSeekKey  seekKey( id );
        // Update the database.
        iItemTable.SeekL(seekKey);
        iItemTable.GetL();

        TBuf<(KMaxTimeFormatSpec + KMaxShortDateFormatSpec) * 2>  previousTimestamp;
        TTime previousDate = iItemTable.ColTime(iItemColSet->ColNo(KDate));
        previousDate.FormatL(previousTimestamp, TTimeFormatSpec());

        if (date == previousDate)
            {
            // If the item is found and timestamp remains same then just 
            // append it's id to aItemIds.
            aItemIds.AppendL(id);
            return isNewItem;
            }
        else
            {
            // If the item is found and the timestamp has changed then make sure that 
            // the new itemId is appened to aItemIds.
            aItemIds.AppendL(aItemId);
            iItemTable.GetL();
            iItemTable.UpdateL();
            }
        }

    // Otherwise this is a new item so append the provided id to aItemIds.
    else
        {
        aItemIds.AppendL(aItemId);

        // Update the database.
        iItemTable.Reset();
        iItemTable.InsertL();
        isNewItem = ETrue;
        }

    iItemTable.SetColL(iItemColSet->ColNo(KItemId), aItemId);
    iItemTable.SetColL(iItemColSet->ColNo(KFeedId), aFeedId);
    iItemTable.SetColL(iItemColSet->ColNo(KDate), date);
    iItemTable.SetColL(iItemColSet->ColNo(KItemStatus), ENewItem);
    iItemTable.SetColL(iItemColSet->ColNo(KTitle_100MaxLen), title.Left(K100MaxLen));
    WriteLongTextL(iItemTable, iItemColSet->ColNo(KDescription), description);
    WriteLongTextL(iItemTable, iItemColSet->ColNo(KWebUrl), link);
    WriteLongTextL(iItemTable, iItemColSet->ColNo(KItemIdStr), itemIdStr);

    iItemTable.PutL();
    return isNewItem;
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::CommitEnclosureL
//
// Commit the enclosure to the database
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CommitEnclosureL(TInt aEnclosureId, TInt aItemId, 
        TInt aFeedId, const RArray<TAttribute>& aAttributes)
    {
    TPtrC  title(KNullDesC);
    TPtrC  length(KNullDesC);
    TPtrC  contentType(KNullDesC);
    TPtrC  link(KNullDesC);

    // Get the values.
    for (TInt i = 0; i < aAttributes.Count(); i++)
        {
        switch (aAttributes[i].token)
            {
            case EEnclosureAttributeTitle:
                title.Set(aAttributes[i].value);
                break;
                
            case EEnclosureAttributeSize:
                length.Set(aAttributes[i].value);
                break;
                
            case EEnclosureAttributeContentType:
                contentType.Set(aAttributes[i].value);
                break;
                
            case EEnclosureAttributeLink:
                link.Set(aAttributes[i].value);
                break;
            }
        }
    
    // Update the database.
    iEnclosureTable.Reset();
    iEnclosureTable.InsertL();
    
    iEnclosureTable.SetColL(iEnclosureColSet->ColNo(KTitle_100MaxLen), title.Left(K100MaxLen));
    iEnclosureTable.SetColL(iEnclosureColSet->ColNo(KEnclosureId), aEnclosureId);
    iEnclosureTable.SetColL(iEnclosureColSet->ColNo(KItemId), aItemId);
    iEnclosureTable.SetColL(iEnclosureColSet->ColNo(KFeedId), aFeedId);
    iEnclosureTable.SetColL(iEnclosureColSet->ColNo(KLength_100MaxLen), length.Left(K100MaxLen));
    iEnclosureTable.SetColL(iEnclosureColSet->ColNo(KContentType_100MaxLen), contentType.Left(K100MaxLen));
    WriteLongTextL(iEnclosureTable, iEnclosureColSet->ColNo(KWebUrl), link);
    
    iEnclosureTable.PutL();
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ExtractFolderListIdInSettingsL
//
// Get the folder list Ids from the settings table from the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ExtractFolderListIdInSettingsL( RArray<TInt>& aFolderListIds )
    {
    TInt folderListId;

    // Prep the settings table.
    UseSettingsTableLC(RDbTable::EReadOnly);

    while( iSettingsTable.NextL() )
        {
        iSettingsTable.GetL();
        folderListId = iSettingsTable.ColInt32(iSettingsColSet->ColNo(KFolderListId));
        aFolderListIds.AppendL( folderListId );
        }

    CleanupStack::PopAndDestroy(/* settings table */);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ExtractAutoUpdateSettingsL
//
// Get the settings from the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ExtractAutoUpdateSettingsL( TInt aFolderListId, TBool& aAutoUpdate, TInt& aAutoUpdateFreq,
        TUint32& aAutoUpdateAP, TBool &autoUpdateWhileRoaming  )
    {
    // Prep the settings table.
    UseSettingsTableLC(RDbTable::EReadOnly);
    
    TDbSeekKey  seekKey( aFolderListId );
    // Extract the values.
    if (iSettingsTable.SeekL(seekKey))
        {        
        iSettingsTable.GetL();

        aAutoUpdate = iSettingsTable.ColUint8(iSettingsColSet->ColNo(KAutoUpdate));
        aAutoUpdateAP = iSettingsTable.ColUint32(iSettingsColSet->ColNo(KAccessPoint));
        aAutoUpdateFreq = iSettingsTable.ColUint32(iSettingsColSet->ColNo(KAutoUpdateFreq));
		autoUpdateWhileRoaming = iSettingsTable.ColUint8(iSettingsColSet->ColNo(KAutoUpdateWhileRoaming));
        }
    else
        {
        User::Leave(KErrNotFound);
        }
        
    CleanupStack::PopAndDestroy(/* settings table */);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::CommitAutoUpdateSettingsL
//
// Commit the settings to the database
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CommitAutoUpdateSettingsL( TInt aFolderListId, TBool aAutoUpdate, TInt aAutoUpdateFreq,
        TUint32 aAutoUpdateAP, TBool aAutoUpdateWhileRoaming )
    {
    TTime  never(0);

    // Prep the settings table.
    UseSettingsTableLC(RDbTable::EUpdatable);
    
    TDbSeekKey  seekKey( aFolderListId );
    // Update the database.
    if (iSettingsTable.SeekL(seekKey))
        {           
        iSettingsTable.GetL();
        iSettingsTable.UpdateL();

        iSettingsTable.SetColL(iSettingsColSet->ColNo(KAutoUpdate), aAutoUpdate);
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KAccessPoint), aAutoUpdateAP);
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KAutoUpdateFreq), aAutoUpdateFreq);
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KAutoUpdateWhileRoaming), aAutoUpdateWhileRoaming);
        }
    else
        {
        iSettingsTable.Reset();
        iSettingsTable.InsertL();

        // initiate last auto update to 0
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KFolderListId), aFolderListId);
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KAutoUpdate), aAutoUpdate);
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KAccessPoint), aAutoUpdateAP);
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KAutoUpdateFreq), aAutoUpdateFreq);
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KAutoUpdateWhileRoaming), aAutoUpdateWhileRoaming);
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KLastAutoUpdate), never);
        }
    
    iSettingsTable.PutL();
        
    CleanupStack::PopAndDestroy(/* settings table */);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ExtractLastAutoUpdateSettingsL
//
// Get the last auto update settings from the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ExtractLastAutoUpdateSettingsL( TInt aFolderListId, TTime& aLastAutoUpdate )
    {
    // Prep the settings table.
    UseSettingsTableLC(RDbTable::EReadOnly);
    
    TDbSeekKey  seekKey( aFolderListId );
    // Extract the values.
    if (iSettingsTable.SeekL(seekKey))
        {        
        iSettingsTable.GetL();

        aLastAutoUpdate = iSettingsTable.ColTime(iSettingsColSet->ColNo(KLastAutoUpdate));                    
        }
    else
        {
        User::Leave(KErrNotFound);
        }
        
    CleanupStack::PopAndDestroy(/* settings table */);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::CommitLastAutoUpdateSettingsL
//
// Commit the last auto update settings to the database
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CommitLastAutoUpdateSettingsL( TInt aFolderListId, TTime aLastAutoUpdate )
    {
    // Prep the settings table.
    UseSettingsTableLC(RDbTable::EUpdatable);
    
    TDbSeekKey  seekKey( aFolderListId );
    // Update the database.
    if (iSettingsTable.SeekL(seekKey))
        {           
        iSettingsTable.GetL();
        iSettingsTable.UpdateL();
    
        iSettingsTable.SetColL(iSettingsColSet->ColNo(KLastAutoUpdate), aLastAutoUpdate);
    
        iSettingsTable.PutL();
        }
    else
        {
        User::Leave(KErrNotFound);
        }
        
    CleanupStack::PopAndDestroy(/* settings table */);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::Compact
//
// Compacts the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::Compact()
    {
    iDatabase.Compact();
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::PackFolderL
//
// Extracts and pack the folder.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::PackFolderL( TInt aFolderListId, TInt aFolderId, 
        CPackedFolder& aPackedFolder, TBool aItemTitleNeed )
    {
    RDbView  view;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    // SELECT FolderItemId, SiblingOrder, IsFolder, FeedId, Title FROM FolderListTable 
    // WHERE FolderListId = aFolderListId AND ParentId = aFolderId 
    // ORDER BY SiblingOrder
    _LIT(KQuery, "SELECT FolderItemId, SiblingOrder, IsFolder, FeedId, Title, Status FROM FolderListTable WHERE FolderListId = %d AND ParentId = %d ORDER BY SiblingOrder");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength + KIntLength); 
                        
    query->Des().Format(KQuery, aFolderListId, aFolderId);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));

    CleanupStack::PopAndDestroy(query);
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
        
    // For each entry either write the feed or folder to aPackedFolder (where folders
    // are written recursively.
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            TInt   folderItemId;
            TInt   isFolder;
            TPtrC  title;
            TInt   statusCode;
            // Get the current row.
            view.GetL();
        
            // Get the fields
            folderItemId = view.ColUint16(colSet->ColNo(KFolderItemId));
            isFolder = view.ColUint8(colSet->ColNo(KIsFolder));
            title.Set(view.ColDes16(colSet->ColNo(KTitle_100MaxLen)));
            statusCode = view.ColInt32(colSet->ColNo(KStatus));
            
            // Process the folder.
            if (isFolder)
                {
                // Recursively pack the folder.
                aPackedFolder.FolderBeginsL(title, folderItemId, statusCode);
                PackFolderL( aFolderListId, folderItemId, aPackedFolder, aItemTitleNeed );
                aPackedFolder.FolderEndsL();
                }
            
            // Otherwise append the feed.
            else
                {
                TDbSeekKey  seekKey;   
                TInt        feedId; 
        
                // Init the key to the feedId of this folder item.
                feedId = view.ColUint16(colSet->ColNo(KFeedId));
                seekKey.Add(feedId);
                
                if(iFeedTable.SeekL(seekKey))
                    {
                    HBufC*  url;
                    TTime   lastUpdate;
                    
                    iFeedTable.GetL();                    

                    // Get the feed's feed url.
                    ReadLongTextL(iFeedTable, iFeedColSet->ColNo(KFeedUrl), url);
                    CleanupStack::PushL(url);
                    
                    // Get the feed's feed last update timestamp.
                    lastUpdate = iFeedTable.ColTime(iFeedColSet->ColNo(KDate));                    

                    TInt unreadCount = iFeedTable.ColUint16(iFeedColSet->ColNo(KUnreadCount));
                    TInt freq        = iFeedTable.ColUint32(iFeedColSet->ColNo(KAutoUpdateFreq));
                   
                    // Pack the feed.
                    if (aItemTitleNeed)
                        {
                        aPackedFolder.AddFeedL( title, *url, lastUpdate, freq, statusCode, unreadCount, folderItemId, feedId );
                        SelectMiniItemsL( feedId, ENewItem, aPackedFolder );
                        }
                    else
                        {
                        aPackedFolder.AddFeedL( title, *url, lastUpdate, freq, statusCode, unreadCount, folderItemId, feedId );
                        }

                    CleanupStack::PopAndDestroy(url);                    
                    }
                }
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::PackFeedL
//
// Extracts and pack the feed.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::PackFeedL(TInt aFeedId, CPackedFeed& aFeed)
    {
    TDbSeekKey  seekKey((TUint16) aFeedId);
    
    // Prep the feed table.
    UseFeedTableLC(RDbTable::EReadOnly);
    
    // Get the information about the feed.
    if (iFeedTable.SeekL(seekKey))
        {        
        HBufC*  feedUrl = NULL;
        HBufC*  description = NULL;
        HBufC*  webUrl = NULL;

        // Get the row.
        iFeedTable.GetL();
        
        // Extract the fields.
        TTime    date = iFeedTable.ColTime(iFeedColSet->ColNo(KDate));
        TPtrC16  title(iFeedTable.ColDes16(iFeedColSet->ColNo(KTitle_100MaxLen)));
                
        ReadLongTextL(iFeedTable, iFeedColSet->ColNo(KFeedUrl), feedUrl);
        CleanupStack::PushL(feedUrl);
        ReadLongTextL(iFeedTable, iFeedColSet->ColNo(KDescription), description);
        CleanupStack::PushL(description);
        ReadLongTextL(iFeedTable, iFeedColSet->ColNo(KWebUrl), webUrl);
        CleanupStack::PushL(webUrl);

        // Copy the data into the packed feed.
        aFeed.FeedBeginsL();
        
        aFeed.AddAttributeL(EFeedAttributeFeedId, aFeedId);
        
        if (title.Length() > 0)
            {
            aFeed.AddAttributeL(EFeedAttributeTitle, title);
            }
        if (feedUrl->Length() > 0)
            {
            aFeed.AddAttributeL(EFeedAttributeFeedUrl, *feedUrl);
            }
        if (description->Length() > 0)
            {
            aFeed.AddAttributeL(EFeedAttributeDescription, *description);
            }
        if (webUrl->Length() > 0)
            {
            aFeed.AddAttributeL(EFeedAttributeLink, *webUrl);
            }

        // Add the timestamp.
        TBuf16<KInt64Length>  dateStr;
        
        dateStr.Format(_L("%Ld"), date.Int64());
        aFeed.AddAttributeL(EFeedAttributeTimestamp, dateStr);
        
        CleanupStack::PopAndDestroy(webUrl);
        CleanupStack::PopAndDestroy(description);
        CleanupStack::PopAndDestroy(feedUrl);
        }
    else
        {
        User::Leave(KErrCorrupt);
        }
                
    CleanupStack::PopAndDestroy(/*Feed Table*/);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::PackItemsL
//
// Extracts and pack the items.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::PackItemsL(TInt aFeedId, CPackedFeed& aFeed)
    {
    RDbView  view;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    // SELECT * FROM ItemTable WHERE FeedId = aFeedId ORDER BY Date DESC
    _LIT(KQuery, "SELECT * FROM ItemTable WHERE FeedId = %d ORDER BY Date DESC");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength); 
                        
    query->Des().Format(KQuery, aFeedId);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), 
            RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            HBufC*  description = NULL;
            HBufC*  webUrl = NULL;

            // Get the row.
            view.GetL();
            
            // Extract the fields.
            TInt     itemId = view.ColUint16(colSet->ColNo(KItemId));
            TTime    date = view.ColTime(colSet->ColNo(KDate));
            TInt     itemStatus = view.ColUint8(colSet->ColNo(KItemStatus));
            TPtrC16  title(view.ColDes16(colSet->ColNo(KTitle_100MaxLen)));
            
            ReadLongTextL(view, colSet->ColNo(KDescription), description);
            CleanupStack::PushL(description);
            ReadLongTextL(view, colSet->ColNo(KWebUrl), webUrl);
            CleanupStack::PushL(webUrl);

            // Copy the data into the packed feed.
            aFeed.ItemBeginsL();
            
            if (title.Length() > 0)
                {
                aFeed.AddAttributeL(EItemAttributeTitle, title);
                }
            if (description->Length() > 0)
                {
                aFeed.AddAttributeL(EItemAttributeDescription, *description);
                }
            if (webUrl->Length() > 0)
                {
                aFeed.AddAttributeL(EItemAttributeLink, *webUrl);
                }
            
            aFeed.AddAttributeL(EItemAttributeItemId, itemId);            

            // Add the read status.
            switch( itemStatus )
                {
                case EUnreadItem:
                    {
                    aFeed.AddAttributeL(EItemAttributeStatus, KUnread);
                    }
                    break;
                case EReadItem:
                    {            
                    aFeed.AddAttributeL(EItemAttributeStatus, KRead);
                    }
                    break;
                case ENewItem:
                    {            
                    aFeed.AddAttributeL(EItemAttributeStatus, KNew);
                    }
                    break;
                }
            
            // Add the timestamp.
            TBuf16<KInt64Length>  dateStr;
            
            dateStr.Format(_L("%Ld"), date.Int64());
            aFeed.AddAttributeL(EItemAttributeTimestamp, dateStr);
            
            CleanupStack::PopAndDestroy(webUrl);
            CleanupStack::PopAndDestroy(description);

            // Pack the enclosures.
            PackEnclosuresL(aFeedId, itemId, aFeed);
            
            // Signal the end of the item.
            aFeed.ItemEndsL();
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::PackEnclosuresL
//
// Extracts and pack the enclosure information.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::PackEnclosuresL(TInt aFeedId, TInt aItemId, CPackedFeed& aFeed)
    {
    RDbView  view;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    // SELECT * FROM EnclosureTable WHERE FeedId = aFeedId AND ItemId = aItemId 
    _LIT(KQuery, "SELECT * FROM EnclosureTable WHERE FeedId = %d AND ItemId = %d");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength + KIntLength); 
                        
    query->Des().Format(KQuery, aFeedId, aItemId);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            HBufC*  webUrl = NULL;

            // Get the row.
            view.GetL();
            
            // Extract the fields.
            TPtrC16  title(view.ColDes16(colSet->ColNo(KTitle_100MaxLen)));
            TPtrC16  length(view.ColDes16(colSet->ColNo(KLength_100MaxLen)));
            TPtrC16  contentType(view.ColDes16(colSet->ColNo(KContentType_100MaxLen)));

            ReadLongTextL(view, colSet->ColNo(KWebUrl), webUrl);
            CleanupStack::PushL(webUrl);

            // Copy the data into the packed feed.
            aFeed.EnclosureBeginsL();
            
            if (title.Length() > 0)
                {
                aFeed.AddAttributeL(EEnclosureAttributeTitle, title);
                }
            if (length.Length() > 0)
                {
                aFeed.AddAttributeL(EEnclosureAttributeSize, length);
                }
            if (contentType.Length() > 0)
                {
                aFeed.AddAttributeL(EEnclosureAttributeContentType, contentType);
                }
            if (webUrl->Length() > 0)
                {
                aFeed.AddAttributeL(EEnclosureAttributeLink, *webUrl);
                }

            // Signal the end of the enclosure.
            aFeed.EnclosureEndsL();
            
            CleanupStack::PopAndDestroy(webUrl);
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::SelectMiniItemsL
//
// Select the minimum items information with the desired status.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::SelectMiniItemsL( TInt aFeedId, TInt aStatus, CPackedFolder& aFolder )
    {
    RDbView  view;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    _LIT(KQuery, "SELECT Title, ItemId FROM ItemTable WHERE FeedId = %d AND ItemStatus = %d");
    
    query = HBufC::NewLC( KQuery().Length() + KIntLength + KIntLength ); 
                        
    query->Des().Format( KQuery, aFeedId, aStatus );

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), 
            RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    if (view.EvaluateAll() >= 0)
        {
        for (view.FirstL(); view.AtRow(); view.NextL())
            {
            // Get the row.
            view.GetL();
            
            // Extract the fields.
            TPtrC title( view.ColDes16(colSet->ColNo(KTitle_100MaxLen)) );
            TInt  itemId = view.ColUint16(colSet->ColNo(KItemId));

            // Copy the data into the packed folder.
            aFolder.ItemBeginsL();
            
            if (title.Length() > 0)
                {
                aFolder.AddAttributeL( EFolderAttributeMiniItemTitle, title );
                aFolder.AddAttributeL( EFolderAttributeMiniItemId, itemId );            
                }            

            // Signal the end of the item.
            aFolder.ItemEndsL();
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::WriteLongTextL
//
// Writes "long" text to the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::WriteLongTextL(RDbRowSet& aTable, TInt aColumnIndex, 
        const TDesC& aValue)
    {
    RDbColWriteStream  stream;
    
    stream.OpenLC(aTable, aColumnIndex);
    stream.WriteL(aValue);
    stream.Close();
    CleanupStack::Pop(/*stream*/);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::ReadLongTextL
//
// Reads "long" text from the database.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ReadLongTextL(RDbRowSet& aTable, TInt aColumnIndex, 
        HBufC*& aValue)
    {
    RDbColReadStream  stream;
    HBufC*            value = NULL;
    TInt              length;
    
    length = aTable.ColLength(aColumnIndex);
    value = HBufC::NewLC(length);
    TPtr  v(value->Des());
    
    if (length > 0)
        {        
        stream.OpenLC(aTable, aColumnIndex);
        stream.ReadL(v, length);
        stream.Close();
        CleanupStack::Pop(/*stream*/);
        }
        
    CleanupStack::Pop(value);    
    aValue = value;
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::UseFolderListTableLC
//
// Pushes the release table function onto cleanup stack 
// and opens the FolderList table for use.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::UseFolderListTableLC(RDbRowSet::TAccess aAccess)
	{
	// Push the clean up function on the stack.
	CleanupStack::PushL(TCleanupItem(&ReleaseFolderListTable, this));

	OpenFolderListTableL(aAccess);
	}


// -----------------------------------------------------------------------------
// CFeedsDatabase::OpenFolderListTableL
//
// Closes the Feed table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::OpenFolderListTableL(RDbRowSet::TAccess aAccess)
	{
    // Inc the ref count.
    iFolderListTableRefCount++;

    // If need be open the table.
    if (iFolderListTableRefCount == 1)
        {        
        // Open the database a get the colSet.
        User::LeaveIfError(iFolderListTable.Open(iDatabase, KFolderListTable, aAccess));
        iFolderListColSet = iFolderListTable.ColSetL();

        User::LeaveIfError(iFolderListTable.SetIndex(KFolderListTableIndex));
        }    
	}

    
// -----------------------------------------------------------------------------
// CFeedsDatabase::ReleaseFolderListTable
//
// Closes the Folder List table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ReleaseFolderListTable(TAny *aPtr)
    {
    CFeedsDatabase*  self = static_cast<CFeedsDatabase*>(aPtr);

    // Dec the ref count.
    self->iFolderListTableRefCount--;
    
    // If need be close the table.
    if (self->iFolderListTableRefCount == 0)
        {        
        delete self->iFolderListColSet;
        self->iFolderListColSet = NULL;    
        self->iFolderListTable.Close();
        }
    else if (self->iFolderListTableRefCount < 0)
        {
        User::Panic(_L("Feeds Server"), KErrArgument);
        }
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::UseFeedTableLC
//
// Pushes the release table function onto cleanup stack 
// and opens the Feed table for use.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::UseFeedTableLC(RDbRowSet::TAccess aAccess)
    {
    // Push the clean up function on the stack.
    CleanupStack::PushL(TCleanupItem(&ReleaseFeedTable, this));
    
    OpenFeedTableL(aAccess);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::OpenFeedTableL
//
// Opens the Feed table for use.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::OpenFeedTableL(RDbRowSet::TAccess aAccess)
	{
    // Inc the ref count.
    iFeedTableRefCount++;
    
    // If need be open the table.
    if (iFeedTableRefCount == 1)
        {        
        User::LeaveIfError(iFeedTable.Open(iDatabase, KFeedTable, aAccess));
        iFeedColSet = iFeedTable.ColSetL();

        User::LeaveIfError(iFeedTable.SetIndex(KFeedTableIndex));
        }  
	}

// -----------------------------------------------------------------------------
// CFeedsDatabase::ReleaseFeedTable
//
// Closes the Feed table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ReleaseFeedTable(TAny *aPtr)
    {
    CFeedsDatabase*  self = static_cast<CFeedsDatabase*>(aPtr);
    
    // Dec the ref count.
    self->iFeedTableRefCount--;
    
    // If need be close the table.
    if (self->iFeedTableRefCount == 0)
        {        
        delete self->iFeedColSet;
        self->iFeedColSet = NULL;
        self->iFeedTable.Close();
        }
    else if (self->iFeedTableRefCount < 0)
        {
        User::Panic(_L("Feeds Server"), KErrArgument);
        }
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::PrepareImportTransationsL
//
// Opens the OPML import related tables (FeedTable and FolderListTable)
//	 and starts a transaction
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::PrepareImportTransationsL()
	{
	
	OpenFeedTableL(RDbTable::EUpdatable);
	OpenFolderListTableL(RDbTable::EUpdatable);

	// Start transaction        
	iDatabase.Begin();        

	}

// -----------------------------------------------------------------------------
// CFeedsDatabase::ReleaseImportTables
//
// Closes the OPML import related tables (FeedTable and FolderListTable)
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ReleaseImportTables()
	{
	ReleaseFolderListTable(this);
	ReleaseFeedTable(this);
	}


// -----------------------------------------------------------------------------
// CFeedsDatabase::CommitImportTransaction
//
// Commits current database transaction
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CommitImportTransaction()
	{
	if(iDatabase.InTransaction())
		iDatabase.Commit();
	}

// -----------------------------------------------------------------------------
// CFeedsDatabase::CancelImportTransaction
//
// Cancels/Rollbacks current database transaction
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::CancelImportTransaction()
	{
	if(iDatabase.InTransaction())
		iDatabase.Rollback();
	}

// -----------------------------------------------------------------------------
// CFeedsDatabase::DeleteFeedTableRecordsL
//
// Deletes records from a FeedTable
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::DeleteFeedTableRecordsL(RArray<TInt>& aFeedIds)
	{
	if(aFeedIds.Count() <= 0)
		return;
	
	DeleteRecordsFromTableL(KFeedTable, KFeedId, aFeedIds);
	}
	
// -----------------------------------------------------------------------------
// CFeedsDatabase::DeleteFolderListTableRecordsL
//
// Deletes records from a FolderListTable
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::DeleteFolderListTableRecordsL(RArray<TInt>& aFolderItemIds)
	{
	if(aFolderItemIds.Count() <= 0)
		return;
	
	DeleteRecordsFromTableL(KFolderListTable, KFolderItemId, aFolderItemIds);
	}

// -----------------------------------------------------------------------------
// CFeedsDatabase::DeleteRecordsFromTableL
//
// Deletes records from a given table
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::DeleteRecordsFromTableL(const TDesC& aTableName, const TDesC& aColumnName, RArray<TInt>& aIds)
	{
	_LIT( KSQLDeleteFormat, "DELETE FROM %S WHERE %S = %d");
	
	TInt numIds = aIds.Count();
	if(numIds <= 0)
		{
		return;
		}	

	HBufC*   query = NULL;
    query = HBufC::NewLC( 
    						KSQLDeleteFormat().Length()
    					  	+ aTableName.Length()
    					  	+ aColumnName.Length()
    					  	+ KIntLength
    					 );

	for(TInt i=0; i<numIds; i++)
		{
		query->Des().Format(KSQLDeleteFormat, &aTableName, &aColumnName, aIds[i]);
		iDatabase.Execute(*query);
		}

 	CleanupStack::PopAndDestroy(query);
	}

// -----------------------------------------------------------------------------
// CFeedsDatabase::UseItemTableLC
//
// Opens the Item table for use.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::UseItemTableLC(RDbRowSet::TAccess aAccess)
    {
    // Inc the ref count.
    iItemTableRefCount++;
    
    // Push the clean up function on the stack.
    CleanupStack::PushL(TCleanupItem(&ReleaseItemTable, this));

    // If need be open the table.
    if (iItemTableRefCount == 1)
        {        
        User::LeaveIfError(iItemTable.Open(iDatabase, KItemTable, aAccess));
        iItemColSet = iItemTable.ColSetL();

        User::LeaveIfError(iItemTable.SetIndex(KItemTableIndex));
        }
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ReleaseItemTable
//
// Closes the Item table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ReleaseItemTable(TAny *aPtr)
    {
    CFeedsDatabase*  self = static_cast<CFeedsDatabase*>(aPtr);
    
    // Dec the ref count.
    self->iItemTableRefCount--;
    
    // If need be close the table.
    if (self->iItemTableRefCount == 0)
        {        
        delete self->iItemColSet;
        self->iItemColSet = NULL;
        self->iItemTable.Close();
        }
    else if (self->iItemTableRefCount < 0)
        {
        User::Panic(_L("Feeds Server"), KErrArgument);
        }
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsDatabase::UseEnclosureTableLC
//
// Opens the Enclosure table for use.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::UseEnclosureTableLC(RDbRowSet::TAccess aAccess)
    {
    // Inc the ref count.
    iEnclosureTableRefCount++;
    
    // Push the clean up function on the stack.
    CleanupStack::PushL(TCleanupItem(&ReleaseEnclosureTable, this));

    // If need be open the table.
    if (iEnclosureTableRefCount == 1)
        {        
        User::LeaveIfError(iEnclosureTable.Open(iDatabase, KEnclosureTable, aAccess));
        iEnclosureColSet = iEnclosureTable.ColSetL();

        User::LeaveIfError(iEnclosureTable.SetIndex(KEnclosureTableIndex));
        }
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ReleaseEnclosureTable
//
// Closes the Enclosure table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ReleaseEnclosureTable(TAny *aPtr)
    {
    CFeedsDatabase*  self = static_cast<CFeedsDatabase*>(aPtr);
    
    // Dec the ref count.
    self->iEnclosureTableRefCount--;
    
    // If need be close the table.
    if (self->iEnclosureTableRefCount == 0)
        {        
        delete self->iEnclosureColSet;
        self->iEnclosureColSet = NULL;
        self->iEnclosureTable.Close();
        }
    else if (self->iEnclosureTableRefCount < 0)
        {
        User::Panic(_L("Feeds Server"), KErrArgument);
        }
    }
    

// -----------------------------------------------------------------------------
// CFeedsDatabase::UseVersionTableLC
//
// Opens the Version table for use.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::UseVersionTableLC(RDbRowSet::TAccess aAccess)
    {
    // Inc the ref count.
    iVersionTableRefCount++;
    
    // Push the clean up function on the stack.
    CleanupStack::PushL(TCleanupItem(&ReleaseVersionTable, this));

    // If need be open the table.
    if (iVersionTableRefCount == 1)
        {        
        User::LeaveIfError(iVersionTable.Open(iDatabase, KVersionTable, aAccess));
        iVersionColSet = iVersionTable.ColSetL();
        }
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ReleaseVersionTable
//
// Closes the Version table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ReleaseVersionTable(TAny *aPtr)
    {
    CFeedsDatabase*  self = static_cast<CFeedsDatabase*>(aPtr);
    
    // Dec the ref count.
    self->iVersionTableRefCount--;
    
    // If need be close the table.
    if (self->iVersionTableRefCount == 0)
        {        
        delete self->iVersionColSet;
        self->iVersionColSet = NULL;
        self->iVersionTable.Close();
        }
    else if (self->iVersionTableRefCount < 0)
        {
        User::Panic(_L("Feeds Server"), KErrArgument);
        }
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::UseSettingsTableLC
//
// Opens the Settings table for use.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::UseSettingsTableLC(RDbRowSet::TAccess aAccess)
    {
    // Inc the ref count.
    iSettingsTableRefCount++;
    
    // Push the clean up function on the stack.
    CleanupStack::PushL(TCleanupItem(&ReleaseSettingsTable, this));

    // If need be open the table.
    if (iSettingsTableRefCount == 1)
        {        
        User::LeaveIfError(iSettingsTable.Open(iDatabase, KSettingsTable, aAccess));
        iSettingsColSet = iSettingsTable.ColSetL();

        User::LeaveIfError(iSettingsTable.SetIndex( KSettingsTableIndex ));
        }
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::ReleaseSettingsTable
//
// Closes the Settings table.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::ReleaseSettingsTable(TAny *aPtr)
    {
    CFeedsDatabase*  self = static_cast<CFeedsDatabase*>(aPtr);
    
    // Dec the ref count.
    self->iSettingsTableRefCount--;
    
    // If need be close the table.
    if (self->iSettingsTableRefCount == 0)
        {        
        delete self->iSettingsColSet;
        self->iSettingsColSet = NULL;
        self->iSettingsTable.Close();
        }
    else if (self->iSettingsTableRefCount < 0)
        {
        User::Panic(_L("Feeds Server"), KErrArgument);
        }
    }   

// -----------------------------------------------------------------------------
// CFeedsDatabase::UpdateFeedStatus
//
// Update feed with status Codes
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::UpdateFeedStatusL(TInt aFeedId, TInt aFeedStatus)
    {
    RDbView  view;
    TBool    found = EFalse;
    HBufC*   query = NULL;
    TInt previousStatus = KErrNone;
    TInt parentId = KRootFolderId;
    
    // Create a view given this select...    
    _LIT(KQuery, "SELECT ParentId,Status FROM FolderListTable WHERE FeedId = %d AND IsFolder = %d");
    query = HBufC::NewLC( KQuery().Length() + KIntLength + KIntLength );
            
    query->Des().Format( KQuery, aFeedId, 0 );

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EUpdatable));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    // Search for the feed.
    if (view.Evaluate() >= 0)
        {
        if (view.FirstL())
            {
            // Get the feed id.
            view.GetL();
            view.UpdateL();        
            previousStatus = view.ColInt32(colSet->ColNo(KStatus));
            parentId = view.ColUint16(colSet->ColNo(KParentId));
            view.SetColL(colSet->ColNo(KStatus),aFeedStatus);
            view.PutL();
            found = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);
	if(!found)
	{
		return;
	}

    if(previousStatus == KErrNone && aFeedStatus == KErrNone)
    	{
    	return;
    	}
    else if(previousStatus != KErrNone && aFeedStatus != KErrNone)
    	{
    	return;
    	}
    else if(previousStatus == KErrNone && aFeedStatus != KErrNone)
    	{
    	    // Open the various tables
    	    UseFolderListTableLC(RDbTable::EUpdatable);
    	    while(parentId != KRootFolderId)
    	    	{
        		TDbSeekKey  folderListKey(parentId);
        		if(iFolderListTable.SeekL(folderListKey))
        			{
        	        iFolderListTable.GetL();
        			parentId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KParentId));
        			previousStatus = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KStatus));
    				iFolderListTable.UpdateL();
    				iFolderListTable.SetColL(iFolderListColSet->ColNo(KStatus), previousStatus-1);
    				iFolderListTable.PutL();
    				if(previousStatus != KErrNone)
        				{
        					break;
        				}
        			}
    	    	}
    	    // Clean up.
    	    CleanupStack::PopAndDestroy(/*folder list table*/);
    	}
    else if(previousStatus != KErrNone && aFeedStatus == KErrNone)
    	{
    	    // Open the various tables
    	    UseFolderListTableLC(RDbTable::EUpdatable);
    	    while(parentId != KRootFolderId)
    	    	{
        		TDbSeekKey  folderListKey(parentId);
        		if(iFolderListTable.SeekL(folderListKey))
        			{
        	        iFolderListTable.GetL();
        			parentId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KParentId));
        			previousStatus = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KStatus));
    				iFolderListTable.UpdateL();
    				iFolderListTable.SetColL(iFolderListColSet->ColNo(KStatus), previousStatus+1);
    				iFolderListTable.PutL();

        			if(previousStatus+1 != KErrNone)
        				{
        					break;
        				}
        			}
    	    	}
    	    // Clean up.
    	    CleanupStack::PopAndDestroy(/*folder list table*/);
    	}
    }   

// -----------------------------------------------------------------------------
// CFeedsDatabase::FreqFromFeedIdL
//
// Return the auto update freq of the feed with the given feed-id.
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::FreqFromFeedIdL(TInt aFeedId, TInt& aFreq)
    {
    RDbView  view;
    TBool    found = EFalse;
    HBufC*   query = NULL;
    
    // Create a view given this select...    
    // SELECT AutoUpdateFreq FROM FeedTable WHERE FeedId = aFeedId
    _LIT(KQuery, "SELECT AutoUpdateFreq FROM FeedTable WHERE FeedId = %d");
    
    query = HBufC::NewLC(KQuery().Length() + KIntLength);
            
    query->Des().Format(KQuery, aFeedId);

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    
    // Search for the feed.
    if (view.Evaluate() >= 0)
        {
        if (view.FirstL())
            {
            // Get the feed id.
            view.GetL();        
            aFreq = view.ColUint32(colSet->ColNo(KAutoUpdateFreq));
            }
        }
        
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);

    return found;
    }

// -----------------------------------------------------------------------------
// CFeedsDatabase::AlterFeedTableWithAutoFequencyL
//
// Adds a new column AutoUpdateFreq to FeedTable in the case of versions less 
// than 7.1.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::AlterFeedTableWithAutoFequencyL()
    {
    UseFeedTableLC(RDbTable::EUpdatable);

    // get the exiting col set
    CDbColSet* colSet = iDatabase.ColSetL(KFeedTable);

    colSet->AddL(TDbCol(KAutoUpdateFreq, EDbColUint32));
    iDatabase.AlterTable(KFeedTable, *colSet);

    CleanupStack::PopAndDestroy(/* FeedTable */);  		     

    // Set the auto update frequency as KErrorNotSet
    // Prep the item table.
    UseFeedTableLC(RDbTable::EUpdatable);

    while( iFeedTable.NextL() )
          {
          // Get the row. and add the default value.
          iFeedTable.GetL();
          iFeedTable.UpdateL();

          iFeedTable.SetColL(iFeedColSet->ColNo(KAutoUpdateFreq), KAutoUpdatingOff);

          iFeedTable.PutL();
          }

    CleanupStack::PopAndDestroy(/* iFeedTable */);
    }

// -----------------------------------------------------------------------------
// CFeedsDatabase::FeedIdFromEntryIdL
//
// Returns the feed id of the feed with the given entry id.
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::FeedIdFromEntryIdL(const TInt& aEntryId, TInt aFolderListId, TInt& aFeedId)
    {
    RDbView  view;
    TBool    found = EFalse;
    HBufC*   query = NULL;

    // Create a view given this select...
    // SELECT FeedId FROM FeedTable WHERE FeedUrl = 'aFeedUrl' AND FolderListId = aFolderListId
    _LIT(KQuery, "SELECT FeedId FROM FolderListTable WHERE FolderItemId = %d AND FolderListId = %d");

    query = HBufC::NewLC( KQuery().Length() + KIntLength + KIntLength );

    query->Des().Format( KQuery, aEntryId, aFolderListId );

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);

    // Search for the feed.
    if (view.Evaluate() >= 0)
        {
        if (view.FirstL())
            {
            // Get the feed id.
            view.GetL();
            aFeedId = view.ColUint16(colSet->ColNo(KFeedId));
            found = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);

    return found;
    }

// -----------------------------------------------------------------------------
// CFeedsDatabase::EntryIdFromFeedIdL
//
// Returns the feed id of the feed with the given entry id.
// -----------------------------------------------------------------------------
//
TBool CFeedsDatabase::EntryIdFromFeedIdL(const TInt& aFeedId, TInt aFolderListId, TInt& aEntryId)
    {
    RDbView  view;
    TBool    found = EFalse;
    HBufC*   query = NULL;

    // Create a view given this select...
    // SELECT FeedId FROM FeedTable WHERE FeedUrl = 'aFeedUrl' AND FolderListId = aFolderListId
    _LIT(KQuery, "SELECT FolderItemId FROM FolderListTable WHERE FeedId = %d AND FolderListId = %d");

    query = HBufC::NewLC( KQuery().Length() + KIntLength + KIntLength );

    query->Des().Format( KQuery, aFeedId, aFolderListId );

    User::LeaveIfError(view.Prepare(iDatabase, TDbQuery(*query), RDbView::EReadOnly));
    CleanupClosePushL(view);

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);

    // Search for the feed.
    if (view.Evaluate() >= 0)
        {
        if (view.FirstL())
            {
            // Get the feed id.
            view.GetL();
            aEntryId = view.ColUint16(colSet->ColNo(KFolderItemId));
            found = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(/*view*/);
    CleanupStack::PopAndDestroy(query);

    return found;
    }

#if defined(_DEBUG)
// -----------------------------------------------------------------------------
// CFeedsDatabase::DebugPrintTables
//
// Prints the tables to the log file.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::DebugPrintTablesL()
    {
    DebugPrintFolderListTableL();
    DebugPrintItemTableL();
    }

// -----------------------------------------------------------------------------
// CFeedsDatabase::DebugPrintFolderListTableL
//
// Prints the folder list table to the log file.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::DebugPrintFolderListTableL()
    {   
    // Prep the FolderList table.
    UseFolderListTableLC(RDbTable::EReadOnly);

    // column names
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend, _L(""));				
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend,
        _L("============================================================================================================="));				
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend, _L("FolderListTable"));
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend, _L(""));				
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend, _L("ListId\t\tItemId\tParent\tSiblin\tType\tFeedId\tTitle"));
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend,
        _L("============================================================================================================="));				

    while( iFolderListTable.NextL() )
        {
        // Get the row.
        iFolderListTable.GetL();
        
        TInt folderListId = iFolderListTable.ColInt32(iFolderListColSet->ColNo(KFolderListId));
        TInt folderItemId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KFolderItemId));
        TInt parentId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KParentId));
        TInt SiblingOrder = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KSiblingOrder));
        TBool isFolder = iFolderListTable.ColUint8(iFolderListColSet->ColNo(KIsFolder));
        TInt feedId = iFolderListTable.ColUint16(iFolderListColSet->ColNo(KFeedId));
        if( isFolder )
            {
            feedId = KUnassignedId;
            }
        TPtrC title( KNullDesC );
        title.Set(iFolderListTable.ColDes16(iFolderListColSet->ColNo(KTitle_100MaxLen)));

        // folder or feed
        if( isFolder )
            {
            if( folderListId == 0 )
                {
                FEED_LOG6(_L("Feeds"), _L("Feeds_DB.log"), 
                    EFileLoggingModeAppend, _L("%d\t\t%d\t%d\t%d\tfolder\t%d\t%S"), 
                    folderListId, folderItemId, parentId, SiblingOrder, feedId, &title);
                }
            else
                {
                FEED_LOG6(_L("Feeds"), _L("Feeds_DB.log"), 
                    EFileLoggingModeAppend, _L("%d\t%d\t%d\t%d\tfolder\t%d\t%S"), 
                    folderListId, folderItemId, parentId, SiblingOrder, feedId, &title);
                }
            }
        else
            {
            if( folderListId == 0 )
                {
                FEED_LOG6(_L("Feeds"), _L("Feeds_DB.log"), 
                    EFileLoggingModeAppend, _L("%d\t\t%d\t%d\t%d\tfeed\t%d\t%S"), 
                    folderListId, folderItemId, parentId, SiblingOrder, feedId, &title);
                }
            else
                {
                FEED_LOG6(_L("Feeds"), _L("Feeds_DB.log"), 
                    EFileLoggingModeAppend, _L("%d\t%d\t%d\t%d\tfeed\t%d\t%S"), 
                    folderListId, folderItemId, parentId, SiblingOrder, feedId, &title);
                }
            }
        }
                
    CleanupStack::PopAndDestroy(/*folder list Table*/);
    }


// -----------------------------------------------------------------------------
// CFeedsDatabase::DebugPrintItemTableL
//
// Prints the item table to the log file.
// -----------------------------------------------------------------------------
//
void CFeedsDatabase::DebugPrintItemTableL()
    {
    // Prep the item table.
    UseItemTableLC(RDbTable::EReadOnly);

    // column names
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend, _L(""));				
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend,
        _L("============================================================================================================="));				
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend, _L("ItemTable"));
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend, _L(""));				
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend, _L("ItemId\tFeedId\tStatus\tTitle\tIdStr"));
    FEED_LOG(_L("Feeds"), _L("Feeds_DB.log"), 
        EFileLoggingModeAppend,
        _L("============================================================================================================="));				

    while( iItemTable.NextL() )
        {
        // Get the row.
        iItemTable.GetL();
        
        TInt itemId = iItemTable.ColUint16(iItemColSet->ColNo(KItemId));
        TInt feedId = iItemTable.ColUint16(iItemColSet->ColNo(KFeedId));

        TBuf<(KMaxTimeFormatSpec + KMaxShortDateFormatSpec) * 2>  timestamp;
        //TBuf<KMaxTimeFormatSpec * 2>  temp;    
        TTime date = iItemTable.ColTime(iItemColSet->ColNo(KDate));
        date.FormatL(timestamp, TTimeFormatSpec());
        //date.FormatL(temp, TShortDateFormatSpec());
        //timestamp.Append(_L("  "));
        //timestamp.Append(temp);
        // JH TO DO: why timestamp not shown up in the log

        TInt itemStatus = iItemTable.ColUint16(iItemColSet->ColNo(KItemStatus));
        TPtrC title( KNullDesC );
        title.Set(iItemTable.ColDes16(iItemColSet->ColNo(KTitle_100MaxLen)));
        HBufC* idStr = NULL;
        ReadLongTextL(iItemTable, iItemColSet->ColNo(KItemIdStr), idStr);
        CleanupStack::PushL(idStr);
        TPtrC idStrPtr(idStr->Des());

        switch( itemStatus )
            {
            case EUnreadItem:
                {
                FEED_LOG6(_L("Feeds"), _L("Feeds_DB.log"), 
                    EFileLoggingModeAppend, _L("%d\t%d\t%S\t%S\t%S\t%S"), 
                    itemId, feedId, &KUnread(), &idStrPtr, &title, &timestamp);
                }
                break;
            case EReadItem:
                {
                FEED_LOG6(_L("Feeds"), _L("Feeds_DB.log"), 
                    EFileLoggingModeAppend, _L("%d\t%d\t%S\t%S\t%S\t%S"), 
                    itemId, feedId, &KRead(), &idStrPtr, &title, &timestamp);
                }
                break;
            case ENewItem:
                {
                FEED_LOG6(_L("Feeds"), _L("Feeds_DB.log"), 
                    EFileLoggingModeAppend, _L("%d\t%d\t%S\t%S\t%S\t%S"), 
                    itemId, feedId, &KNew(), &idStrPtr, &title, &timestamp);
                }
                break;
            }

        CleanupStack::PopAndDestroy(/*idStr*/);
        }
                
    CleanupStack::PopAndDestroy(/*item Table*/);
    }
#endif
