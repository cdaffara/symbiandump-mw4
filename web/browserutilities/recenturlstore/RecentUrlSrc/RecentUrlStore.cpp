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
* Description: 
*      Implementation of RecentUrlStore
*
*
*/



// INCLUDE FILES
#include "RecentUrlStore.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT(KRecentUrlDBFile, "c:\\private\\%08x\\RecentUrlStore.db");

_LIT(KSQLInsertVersion, "INSERT INTO dbversion (dbver) VALUES ('1.0')");

/*
*    Database columns:
*       History - counter used to order the urls latest first
*       Url - url of visited page
*/
_LIT(KVersionTableSQL, "CREATE TABLE dbversion(dbver CHAR(10))");
_LIT(KRecentUrlTableSQL, "CREATE TABLE recenturl(history COUNTER, domain VARCHAR(50), url VARCHAR(250), title VARCHAR(250))");
const TUint KDomainSize = 50;
const TUint KUrlSize = 250;
const TUint KTitleSize = 250;

_LIT(KIndex1SQL, "CREATE INDEX updateidx ON recenturl (history)");

_LIT(KSQLInsert, "INSERT INTO recenturl (domain,url,title) VALUES ('%S','%S', '%S')");
_LIT(KSQLDelete, "DELETE FROM recenturl WHERE url = '%S'");
_LIT(KSQLSelect, "SELECT url,title FROM recenturl WHERE domain LIKE '%S*' OR url LIKE '*://%S*' ORDER BY history desc");
_LIT(KSQLSelectAll, "SELECT url,title FROM recenturl ORDER BY history desc");
const TUint KUrlCol = 1;
const TUint KTitleCol = 2;

const TUint KMaxRows = 100;

_LIT(KDomainDelim, "."); // domain starts after first '.' and ends after second '.'

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRecentUrlStore::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CRecentUrlStore * CRecentUrlStore::NewL ()
    {
    CRecentUrlStore* self = new (ELeave) CRecentUrlStore();
    self->ConstructL();
    return self;

    }

// -----------------------------------------------------------------------------
// CRecentUrlStore::CRecentUrlStore
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CRecentUrlStore::CRecentUrlStore():
	iFirstTimeOpened(ETrue)
	{

	}

// -----------------------------------------------------------------------------
// CRecentUrlStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CRecentUrlStore::ConstructL()
    {
    
    // Create the file name from UID
	RProcess myProcess;	
	TBuf <256>buf;    
    buf.Format(KRecentUrlDBFile, myProcess.Identity());
	iRecentUrlDBFile = buf.AllocL();

	User::LeaveIfError(iDbSession.Connect());
	
    }
    
//-----------------------------------------------------------------------------
// CRecentUrlStore Destructor
//-----------------------------------------------------------------------------
CRecentUrlStore::~CRecentUrlStore()
    {
	iDbSession.Close();
	delete iRecentUrlDBFile;
    }

    
//-----------------------------------------------------------------------------
// CRecentUrlStore::GetData
// params aUrl - Url filter
// Returns matching urls.
//-----------------------------------------------------------------------------
EXPORT_C TInt CRecentUrlStore::GetData (CDesCArray& aUrls, CDesCArray& aTitles, const TDesC& aUrl)
    {
    RDbNamedDatabase dataBase;
    TInt err = OpenDatabase(dataBase);
    if (err == KErrNone)
    	{
    	TRAP(err, GetDataL(dataBase, aUrls, aTitles, aUrl));
    	dataBase.Close();
    	}
    return err;
    }
//-----------------------------------------------------------------------------
// CRecentUrlStore::GetData
// params aUrl - Url filter
// Returns matching urls.
//-----------------------------------------------------------------------------
void CRecentUrlStore::GetDataL (RDbNamedDatabase& aDataBase, 
			CDesCArray& aUrls, CDesCArray& aTitles, const TDesC& aUrl)
    {
    // shouldn't happen but if it's too long just skip it!
    if ( aUrl.Length() <= KUrlSize)
		{	
		TInt rowCount(0);

		// select values from the database filtered by url
		if (aUrl.Length())
			{
			HBufC* domain = aUrl.AllocLC();
			domain->Des().LowerCase();
			iSQLStatement.Format(KSQLSelect, domain, domain);
			CleanupStack::PopAndDestroy();
			}
		else
			{
			iSQLStatement.Format(KSQLSelectAll);
			}
		
		RDbView view;    
		CleanupClosePushL( view );
		User::LeaveIfError( view.Prepare(aDataBase, TDbQuery(iSQLStatement)) );
		User::LeaveIfError( view.EvaluateAll() ); 
		view.FirstL();
			// loop through rows and build the list
		while (view.AtRow() && rowCount++ < KMaxRows)
			{
			view.GetL();
			aUrls.AppendL(view.ColDes(KUrlCol));
			aTitles.AppendL(view.ColDes(KTitleCol));
			view.NextL();
			}
		/*
		* This loop will keep the number of rows in the database at a reasonable size by
		* deleting old rows (deletes rows beyond KMaxRows).  Should be at most 1 row deleted.
		* Its more efficiant to delete it here than in the SaveData because in this function
		* we already have the list and know its size
		*/
		while (view.AtRow())
			{
			view.GetL();
			iSQLStatement.Format(KSQLDelete, &aUrl);
			aDataBase.Execute(iSQLStatement);
			view.NextL();
			}
		CleanupStack::PopAndDestroy( &view );
		}
    }

//-----------------------------------------------------------------------------
// CRecentUrlStore::DeleteData
// Deletes a single row from the database.
//-----------------------------------------------------------------------------
EXPORT_C void CRecentUrlStore::DeleteData (const TDesC& aUrl)
    {  
    // shouldn't happen but if it's too long just skip it!
     if ( aUrl.Length() <= KUrlSize)
		{		
		RDbNamedDatabase dataBase;
		if (OpenDatabase(dataBase) == KErrNone)
			{
			iSQLStatement.Format(KSQLDelete, &aUrl);
			dataBase.Execute(iSQLStatement);
			dataBase.Close();
			}
		}
	}

//-----------------------------------------------------------------------------
// CRecentUrlStore::SaveDataL
// Save the url in store.
//-----------------------------------------------------------------------------
EXPORT_C void CRecentUrlStore::SaveDataL (const TDesC& aUrl, const TDesC& aTitle)
	{
	TInt urlLength (aUrl.Length());	
	// shouldn't happen but if it's too long for the data store just skip it!
	if (urlLength <= KUrlSize)
		{
		RDbNamedDatabase dataBase;
		CleanupClosePushL( dataBase );
		if (OpenDatabase(dataBase) == KErrNone)
			{
			// find the point where the domain starts and ends
			TInt domainLength(urlLength);
			TInt domainStart(0);
			
			TInt startPos = aUrl.Find(KDomainDelim);
			if (startPos != KErrNotFound)
				{
				domainStart = startPos + (KDomainDelim().Length()); // first char after delim
				TInt len = aUrl.Right(urlLength - domainStart).Find(KDomainDelim);
				if (len > 0) // ignore delim following delim.  we don't want an empty string
					{
					domainLength = len;
					}
				else
					{
					domainLength -= domainStart;
					}
				}
				
			// make sure it's not too big for the data store
			domainLength = (domainLength > KDomainSize) ? KDomainSize : domainLength;
			TInt titleLength = (aTitle.Length() > KTitleSize) ? KTitleSize : aTitle.Length();
				
			HBufC* domain = aUrl.Mid(domainStart,domainLength).AllocLC();
			domain->Des().LowerCase();
			HBufC* title = aTitle.Left(titleLength).AllocLC();
	
			// delete and re-insert
			iSQLStatement.Format(KSQLDelete, &aUrl);
			dataBase.Execute(iSQLStatement);
			iSQLStatement.Format(KSQLInsert, domain, &aUrl, title);
			dataBase.Execute(iSQLStatement);
			
			CleanupStack::PopAndDestroy(3, &dataBase );
			}
		}
	}
	
//-----------------------------------------------------------------------------
// CRecentUrlStore::ClearData
// Clear the store.
//-----------------------------------------------------------------------------
EXPORT_C void CRecentUrlStore::ClearData ()
	{
	// the quickest way to clear the data is to re-create it
	TRAP_IGNORE(CreateDatabaseL());
	}


// private
//-----------------------------------------------------------------------------
// CRecentUrlStore::DeleteOldRowsL
//-----------------------------------------------------------------------------
void CRecentUrlStore::DeleteOldRowsL (RDbNamedDatabase& aDataBase)
    {
    TInt rowCount(0);
	iSQLStatement.Format(KSQLSelectAll);
	
    RDbView view;
    CleanupClosePushL( view );
    
    User::LeaveIfError( view.Prepare(aDataBase, TDbQuery(iSQLStatement)));
    User::LeaveIfError(  view.EvaluateAll() );
	view.FirstL();
	// loop through rows we want to keep
	while (view.AtRow() && rowCount++ < KMaxRows)
		{
		view.NextL();
		}
	// delete the rows that are old
	while (view.AtRow())
		{
		view.DeleteL();
		view.NextL();
		}
    CleanupStack::PopAndDestroy( &view );	
    }

//-----------------------------------------------------------------------------
// CRecentUrlStore::OpenDatabase
// Open the data store.
//-----------------------------------------------------------------------------
TInt CRecentUrlStore::OpenDatabase(RDbNamedDatabase& aDataBase)
	{
	TInt error = KErrNone;
	// open the database - create if it doesn't exist
	error = aDataBase.Open(iDbSession, *iRecentUrlDBFile);
	// compact it once during our session
	if (error == KErrNone && iFirstTimeOpened)
		{
		TRAP_IGNORE(DeleteOldRowsL(aDataBase));
		aDataBase.Compact();
		iFirstTimeOpened = EFalse;
		}
		
	if (error != KErrNone)
		{
		TRAP(error,CreateDatabaseL());
		if (error == KErrNone)
			{
			error = aDataBase.Open(iDbSession, *iRecentUrlDBFile);
			}
		}
	return error;
	}

//-----------------------------------------------------------------------------
// CRecentUrlStore::CreateDatabaseL
// Create the data store.
//-----------------------------------------------------------------------------
void CRecentUrlStore::CreateDatabaseL()
	{
	RDbNamedDatabase dataBase;
	User::LeaveIfError(dataBase.Replace(iDbSession, *iRecentUrlDBFile));
	    	
	CleanupClosePushL(dataBase);
	User::LeaveIfError(dataBase.Execute(KVersionTableSQL));
	User::LeaveIfError(dataBase.Execute(KRecentUrlTableSQL));
	User::LeaveIfError(dataBase.Execute(KIndex1SQL));
	User::LeaveIfError(dataBase.Execute(KSQLInsertVersion));
	CleanupStack::PopAndDestroy();//dataBase	    
    
	}
	
//  End of File
