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


#ifndef C_WRTCONTENTHARVESTER_H
#define C_WRTCONTENTHARVESTER_H 

// INCLUDES
#include <apgcli.h> // RApaLsSession 
#include <contentharvesterplugin.h>
#include <widgetappdefs.rh>
#include "wrtharvesterregistryaccess.h"
#include "wrtusbhandler.h"

// FORWARD DECLARATIONS
class CWrtHarvesterPSNotifier;
class CWrtHarvesterPublisherObserver;
class MLiwInterface;
class CLiwGenericParamList;
class CWrtInfo;
class CWrtUsbHandler;


// CONSTANTS

enum TWidgetState
    {
    EActivatedState = 0,
    EResumeState
    };

class TWrtState
    {
    public:
        /**
        * Constructor
        */
        inline TWrtState(TUid aUid, TWidgetState aState)
            {
            iUid = aUid;
            iState = aState;
            }

    public:
        TUid            iUid;
        TWidgetState    iState;
    };

// CLASS DECLARATION
/**
 */
class CWrtHarvester: public CContentHarvesterPlugin
	{
	struct SWidgetOperation
	    {
	    TWidgetOperations iOperation;
	    TUid iUid;
	    };
	
	public: // Constructors and destructor
		/**
		 * Two-phased constructor.
		 */
		static CWrtHarvester* NewL( MLiwInterface* aCPSInterface );
		
		/**
		 * Destructor.
		 */
		virtual ~CWrtHarvester();

        // from base class 

		/**
    	* 
    	*/
		void UpdateL();
		
		/**
    	* 
    	*/
		void TryLaunchNextOperationL();

        /**
		* @param aContentId
		* @param aTrigger
		*/
		void HandlePublisherNotificationL( const TDesC& aContentId, const TDesC8& aTrigger );
       
        /**
		*Clears the Event Queue 
		*/
		
		void ClearAllOperations();
		
		/**
		* Queues widget operation
		* @param aOperation Operation to be queued
    	* @param aUid Uid of the widget
    	*/
		void QueueOperationL(  TWidgetOperations aOperation, TUid aUid );	
		
		void DialogShown(){ iDialogShown = EFalse; }
		//to check if the device is in mass memory mode
		TInt IsInMSMode() { return iMSMode; }
		void  SetMSMode(TInt val)  { iMSMode = val; }
		 
		
	private:
        
    	/**
    	* Perform the second phase construction 
    	*/		
		void ConstructL();
        
        /** 
		* Default constructor.
		* @param aCPSInterface
		*/
		CWrtHarvester( MLiwInterface* aCPSInterface );

		/**
    	* 
    	*/
		void UpdatePublishersL();
		
		/**
        * 
        */
		void RemoveObsoletePublishersL();
		
		/**
        * 
        */
		void RegisteredPublishersL( RPointerArray<HBufC>& publishers );
		
		/**
		* 
    	* @param aWrtInfo
    	* @return TInt
    	*/
		TInt RegisterPublisherL( CWrtInfo& aWrtInfo );
		
		/**
    	* 
    	* @param aBundleId
    	*/
		void RemovePublisherAndObserverL( const TDesC& aBundleId );
		
		/**
		* 
		* @param aBundleId
		*/
		void RemoveObserver(const TDesC& aBundleId);
		
		/**
    	* 
    	* @param aWrtInfo
    	*/
		void RequestForNotificationL( CWrtInfo& aWrtInfo );
		
		/**
        * 
        */
		TUid WidgetUid( TPtrC aBundleId );
		
		/**
    	* 
    	* @param aInParamList
    	* @return TInt
    	*/
		TInt ExtractItemId( const CLiwGenericParamList& aInParamList );
		
		
		/**
		 * 
		 */
		void LaunchWidgetOperationL( SWidgetOperation aOperation );
		
		/**
		 * Check If widget Ui Process Exists
		 */		
		TBool CheckTaskExistsL();
				
		/**
		 * Queues widget resume operation
		 * @param aUid Uid of the widget
		 * @return void
		 */
		void QueueResumeL( TUid& aUid );
		
		/**
		 * Processes online/offline event
		 * @param aUid Uid of the widget
		 * @param aOperation Online/Offline operation to be queued
		 * @return void
		 */
		 void ProcessNetworkModeL( TUid& aUid, TWidgetOperations aOperation );
		       
		/**
		 * Finds widget in the widget state array which has
		 * the same uid and state as specified in the params
		 * @param aUid Uid of the widget to be found
		 * @param aState Widget state
		 * @returns the array index of the widget found
		 */
		TInt FindWidget( TUid& aUid, TWidgetState aState );
		
		/**
		 * Finds widget in the widget state array which has
		 * the same uid as specified in the params
		 * @param aUid Uid of the widget to be found
		 * @returns the array index of the widget found
		 */
		TInt FindWidget( TUid& aUid );
		
		/**
		 * Checks if the widget has network access
		 * @param aUid Uid of the widget to be checked
		 * @returns ETrue if widget has network access,
		 * else returns EFalse
		 */
		TBool CheckNetworkAccessL( TUid& aUid );
		
		/**
		 * Prompt for Allow Platform Access
		 * @param aUid Uid of the widget
		 * @return void
		 */
		void AllowPlatformAccessL( TUid& aUid );

	private: // data
       	
       	/**
        * Instance of CPS interface used for update with CPS.
        */
        MLiwInterface* iCPSInterface;
        
       	/**
       	 * Publish & Subscribe listener
       	 * own
       	 */
        CWrtHarvesterPSNotifier* iWidgetUIListener;

        /**
         * Publish & Subscribe listener
         * own
         */
        CWrtHarvesterPSNotifier* iWidgetRegListener;
        
        /**
         * Publish & Subscribe listener
         * own
         */
        CWrtHarvesterPSNotifier* iWidgetMMCListener;
        
        /**
         * Publish & Subscribe listener
         * own
         */        
        CWrtUsbHandler* iWidgetUsbListener;
        
		/**
    	* 
    	*/
        RWrtArray<CWrtHarvesterPublisherObserver> iObservers;
        
        /**
        * 
        */
        RWrtArray<CWrtInfo> iWidgetInfo;
        
        /**
        * 
        */
        RWrtArray<TWrtState> iWidgetStateArray;

		/**
    	* 
    	*/
        WrtHarvesterRegistryAccess iRegistryAccess;
        
		/**
    	* 
    	*/
        RArray<SWidgetOperation> iWidgetOperations;
        
        /**
         * 
         */
        RApaLsSession iApaSession;
        
        /**
         * Resource offset
         */
        TInt iResourceFileOffset;
		
        /**
         * 
         */        
        TInt          iHSCount;		
        /**
         *
         *
         */
         TBool        iDialogShown;
        /**
         *
         *
         */
         TInt        iMSMode;
         
         RFs iFs;
        /**
         *
         *
         */
         RPointerArray<HBufC> iHSWidgets;
    };

#endif // C_WRTCONTENTHARVESTER_H 
