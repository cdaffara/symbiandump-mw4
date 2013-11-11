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
* Description:  Class for making Feed Engine backup and restore aware.
*
*/


#ifndef BACKRESTOREHANDLER_H
#define BACKRESTOREHANDLER_H

//  INCLUDES
#include <e32property.h>  
#include <e32base.h>

// FORWARD DECLARATIONS
class CFeedsServer;

// CLASS DECLARATION
class CBackRestoreHandler : public CActive
{
	public:	// Constructors and destructor
	     	/**
      	* Two-phased constructor.
      	*/
			 	static CBackRestoreHandler* NewL(CFeedsServer*);
			 	static CBackRestoreHandler* NewLC(CFeedsServer*);
			 	/**
        * Destructor.
        */ 	
				~CBackRestoreHandler();
	private:	//	Constructor
	
		 	 	/**
      	* C++ default constructor.
       	*/ 	
				CBackRestoreHandler();
        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param pointer to CFeedsServer class,This pointer will be used to make server sleep and to wake up server
        *
        */
        void ConstructL(CFeedsServer*);
		
	public:	//Functions from CActive Class
	      /**
        * Handles an active object's request completion event.
        *
        * @since ?
        * @return void.
        */
				void RunL();

	      /**
        * Called by CActiveScheduler when RunL() Method leaves.
        *
        * @since ?
        * @return void.
        */
				void RunError();
				
				/**
        * Called by CActiveScheduler when Cancel() Method is called.
        *
        * @since ?
        * @return void.
        */
				void DoCancel();
			
	public:	//Functions from CActive Class		
				/**
        * This method subscribe to receive event when OS level
        * variable KUidBackupRestoreKey is changed by Backup or Restore
        *	process
        *
        * @since 3.2
        * @return void.
        */
				void StartListening();

				/**
        * This method unsubscribe to the events from publish and subscribe server
        *
        * @since 3.2
        * @return void.
        */
				void StopListening();
	
	private:
				RProperty 					iProperty;
				TBool 							iIsListening;
				TBool 							iIsBkpRestoreInProgress;
				CFeedsServer*				iFeedServer;
	};
	
	
#endif			//BACKRESTOREHANDLER_H

// End of File