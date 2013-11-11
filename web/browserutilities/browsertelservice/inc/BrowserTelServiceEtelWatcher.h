/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#ifndef __BROWSERTELSERVICEETELWATCHER_H
#define __BROWSERTELSERVICEETELWATCHER_H

#include <etel.h>

// CLASS DECLARATION

/**
* Observer for CBrowserTelService
*/
class MBrowserTelServiceEtelWatcherObserver
	{
	public:
        /**
        * Notification of a status change.
        * @param aStatus The status.
        */
		virtual void BrowserTelServiceEtelWatcherEvent(
			RCall::TStatus& aStatus ) = 0;
	};

/**
* Watcher for possible etel status changes.
*/
class CBrowserTelServiceEtelWatcher : public CActive
	{
	public:		// Constructors and destructor
		/**
        * Destructor.
        */
		~CBrowserTelServiceEtelWatcher();

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aObserver The observer for CBrowserTelService.
        * @return The constructed CBrowserTelService object.
        */
		static CBrowserTelServiceEtelWatcher* NewL(
			MBrowserTelServiceEtelWatcherObserver* aObserver );
		static CBrowserTelServiceEtelWatcher* NewLC(
			MBrowserTelServiceEtelWatcherObserver* aObserver );

	public:		// New functions
		/**
        * Request a notification on etel status change.
        */
		void NotifyOnEtelStatusChangeL();

		/**
        * Cancel the request set with NotifyOnEtelStatusChangeL().
        */
		void NotifyOnEtelStatusChangeCancel();

	private:
	    /**
        * C++ default constructor.
        */
		CBrowserTelServiceEtelWatcher();

		/**
        * Second phase constructor. Leaves on failure.
        * @param aObserver The observer for CBrowserTelService. 
        */
		void ConstructL( MBrowserTelServiceEtelWatcherObserver* aObserver );

	private:	// Functions from base classes
		/**
		* From CActive
		*/
		void DoCancel();
		void RunL();

	private:	// Data
		MBrowserTelServiceEtelWatcherObserver* iObserver;
		RTelServer iRTelServer;
		RPhone iRPhone;
		RLine iRLine;
		RCall::TStatus iCallStatus;
	};

#endif	// __ETELWATCHER_H

