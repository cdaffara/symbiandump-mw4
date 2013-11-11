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
*      Header file of RecentUrlStore.cpp
*
*
*/


// INCLUDE FILES


#ifndef RECENTURLSTORE_H
#define RECENTURLSTORE_H

#warning The Recent URL Store API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32base.h>
#include <s32file.h>
#include <d32dbms.h>
#include <badesca.h>

// CONSTANTS

#define KMaxSQLLength (4096)// must handle name length + value length + insert statement length

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
* CRecentUrlStore
*
*  \b Library: ?library
*
*  @since ?Series60_version
*/
class CRecentUrlStore : public CBase
    {

    public:

        /**
        * Two-phased constructor.
        * @return CRecentUrlStore *
        */
        IMPORT_C static CRecentUrlStore* NewL();

        /**
        * Returns url list.
        */
        IMPORT_C TInt GetData (CDesCArray&  aUrls, CDesCArray&  aTitles, const TDesC& aUrl = KNullDesC);
        
	    /**
	    * DeleteData
	    * Deletes a row from the database
	    */
	    IMPORT_C void DeleteData (const TDesC& aUrl);

	    /**
	    * Save the url and title in store.
	    */
	    IMPORT_C void SaveDataL (const TDesC& aUrl, const TDesC& aTitle);

	    /**
	    * Clear the saved data
	    */
	    IMPORT_C void ClearData ();

        /**
        * Destructor
        */
        ~CRecentUrlStore();

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

	    /**
	    *  Default constructor
	    */
	    CRecentUrlStore();
	    
        /**
        * Returns url list.
        */
        void GetDataL (RDbNamedDatabase& aDataBase, 
        				CDesCArray&  aUrls, CDesCArray&  aTitles, const TDesC& aUrl);

        /**
        * Delete old rows
        */
        void DeleteOldRowsL (RDbNamedDatabase& aDataBase);
        
	    /**
	    *  Open the database
	    */
	    TInt OpenDatabase(RDbNamedDatabase& aDataBase);

	    /**
	    *  Create the database
	    */
	    void CreateDatabaseL();
	    
    /*  Private Members  */

    private :

	    RFs 				iDbSession; ///< client side access
	    TBuf<KMaxSQLLength> iSQLStatement;
	    HBufC*          	iRecentUrlDBFile;
	    TBool				iFirstTimeOpened;
    };

#endif  // RECENTURLSTORE_H

// End of File
