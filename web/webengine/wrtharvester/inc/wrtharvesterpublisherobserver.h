/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef WRTHARVESTERPUBLISHEROBSERVER_H
#define WRTHARVESTERPUBLISHEROBSERVER_H

#include <e32base.h>
#include <liwcommon.h>

// FORWARD DECLARATIONS
class CWrtHarvester;
class CLiwServiceHandler;
// CONSTANTS

// CLASS DECLARATION
/**
 *  CWrtHarvesterPublisherObserver
 *
 *  @lib wrtharvester.dll
 *  @since S60 S60 v5.0
 */

class CWrtHarvesterPublisherObserver : public CBase, public MLiwNotifyCallback
	{
	public: // Constructors and destructor
		/**
		 * Two-phased constructor.
		 */
		static CWrtHarvesterPublisherObserver* NewLC( const TDesC& aBundleId, CWrtHarvester* aHarvester );
		
		/**
		 * Destructor.
		 */
		virtual ~CWrtHarvesterPublisherObserver();

	public:  //from MLiwNotifyCallback
	    
	    /**
	    * Handles notifications caused by an asynchronous Execute*CmdL call
	    * or an event.
	    *
	    * @param aCmdId The service command associated to the event.
	    * @param aEventId Occured event, see LiwCommon.hrh.
	    * @param aEventParamList Event parameters, if any, as defined per
	    *        each event.
	    * @param aInParamList Input parameters, if any, given in the
	    *        related HandleCommmandL.
	    * @return Error code for the callback.
	    */
	    virtual TInt HandleNotifyL(
	        TInt aCmdId,
	        TInt aEventId,
	        CLiwGenericParamList& aEventParamList,
	        const CLiwGenericParamList& aInParamList);
	
	public:
	    
	    /**
	    * Registers to CPS for add, delete , update and execute notifications
	    * @aFilter - filter for input parameter list
	    * @return void.
	    */
	    void RegisterL( CLiwDefaultMap* aFilter );
	    
	    /**
	    *
	    */
	    const TDesC& BundleId();

	    /**
	    * Cancel all the registered notifications. 
	    * @return void.
	    */
	    void ReleaseL();

	private:
	    /**
        * Perform the second phase construction 
        */      
        void ConstructL( const TDesC& aBundleId );
        
        /**
		* Default constructor.
		*/
		CWrtHarvesterPublisherObserver( CWrtHarvester* aHarvester );
		
		// Prhohibited
		CWrtHarvesterPublisherObserver();
	    
	    /**
	    * Initialise Liw connection to Content publishing service.
	    */
	    void InitLiwL();
	    
	    /**
	    * Closes Liw connection to Content publishing service.
	    */
	    void ReleaseLiw();
		
	private: // data
	        
	    // Reference of 
	    // Owned
	    MLiwInterface* iInterface;
	    
	    // Reference of 
	    // Owned
	    CLiwServiceHandler* iLiwHandler;	            
	            
	    // Reference of the sapi data
	    // Not owned
	    CWrtHarvester* iHarvester;
	    
	    // Name of the observed publisher
	    // Owned
	    HBufC* iBundleId;
	    
	    // Call back error code
	    TInt iError;
    };

#endif // WRTHARVESTERPUBLISHEROBSERVER_H
