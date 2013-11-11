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
* Description:  A task to import Feeds from an OPML file.
*
*/


#ifndef IMPORT_FEEDS_TASK_H
#define IMPORT_FEEDS_TASK_H


// INCLUDES
#include <e32base.h>
#include <f32file.h>

#include "Task.h"
#include "PackedFolder.h"
#include "PackedAttributes.h"
#include "FeedsDatabase.h"
#include "FolderAttributes.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIdle;
class RFile;

// CLASS DECLARATION


/**
*  The ImportFeedsTask's observer.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.2
*/
class MImportFeedsTaskObserver
    {
    public: 
        /**
        * Notifies the observer that the task is about to start a lengthy 
        * operation.  In response the observer could display somekind of 
        * status indicator.
        *
        * @since 3.2
        * @return void.
        */
        virtual void StartImportWait() = 0;

        /**
        * Called upon completion of the task.
        *
        * @since 3.2
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        virtual void ImportCompleted(TInt aStatusCode) = 0;
    };


enum TImportFeedsTaskStates
    {
    EImportOpenOPMLFile,
    EImportPreparePackedFolder,
    EImportIntoDB,
    EImportDone
    };
    

/**
*  A task to import Feeds.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.2
*/
class CImportFeedsTask: public CTask
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CImportFeedsTask* NewL(CFeedsServer& aFeedsServer, MImportFeedsTaskObserver& aObserver, const RMessage2& aMessage);

        /**
        * Destructor.
        */        
        virtual ~CImportFeedsTask();
        
    public:  // From CTask
        /**
        * Starts the task.
        *
        * @since 3.2
        * @return Void
        */
        virtual void StartTaskL();
        

    private:
        /**
        * C++ default constructor.
        */
        CImportFeedsTask(CFeedsServer& aFeedsServer, MImportFeedsTaskObserver& aObserver, const RMessage2& aMessage);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

	private:
	
        /**
        * Callback function for CIdle task
        *
        * @since 3.2
        * @param aPtr The this pointer.
        * @return TBool - ETrue if callback has to be called again, EFalse if nomore calls has to be made
        */        
        static TBool StartImportStateMachine(TAny *aPtr);
	
        /**
        * Opens the OPML file name
        *
        * @since 3.2
        * @return Void.
        */	
        void OpenOPMLFileL();

        /**
        * Creates the packed folder by parsing the OPML file
        *
        * @since 3.2
        * @return Void.
        */	
		void PreparePackedFolderL();
		
        /**
        * Starts importing the packed folder into DB
        *
        * @since 3.2
        * @return Void.
        */	
		void ImportIntoDBL();
		
        /**
        * Import the packed folder into DB
        *
        * @since 3.2
        * @return Void.
        */	
		void ImportFoldersL();

        /**
        * Cancel the ongoing OPML import task
        *
        * @since 3.2
        * @return Void.
        */	
		void CleanupImportTaskL();

        /**
        * Callback function for import task completion
        *
        * @since 3.2
        * @param aPtr The this pointer.
        * @return Void.
        */	
		static void ReleaseImportTablesCallback(TAny *ptr);
		
        /**
        * Closes all the OPML import related tables
        *
        * @since 3.2
        * @return Void.
        */	
		void ReleaseImportTables();

    private:

   		TInt						iImportState;
   		MImportFeedsTaskObserver&	iObserver;
		const RMessage2& 			iMessage;
        CIdle*						iIdle;
        TBool						iMoreToAdd;

		TInt						iFolderListId;
		TBool						iClearFolderList;

		RFs 						iRFs;
		RFile						iImportFile;

		HBufC*						iFullImportFileName;
		TFileName					iDispImportFileName;

		CPackedFolder*				iPackedFolder;
		CFeedsDatabase*				iFeedsDatabase;
		
		TBool						iDBTransactInited;
		
	    RArray<TInt>        		iParentStack;
	    RArray<TInt>        		iSiblingIndexStack;
		
		TBool						iFoundRootFolder;
		
		RArray<TInt>				iFolderItemIds;
		RArray<TInt>				iFeedIds;
		
		
	private:
	
		friend class CFeedsServerSession;		
        
    };
    
#endif      // IMPORT_FEEDS_TASK_H
            
// End of File
