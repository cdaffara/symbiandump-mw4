/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Notifications from HotSpot Server to HSS management.
*
*/


#ifndef HSSSRVNOTIFICATIONS_H
#define HSSSRVNOTIFICATIONS_H

#include <wlanmgmtcommon.h>
#include <wlanmgmtinterface.h>

class RHssInterface;


/**
* MHssSrvNotifications specifies a callback interface for notification services.
* These virtual methods should be implemented by HSS management if it
* needs any notifications.
*/
class MHssSrvNotifications
{
   public:

        /**
        * New networks have been detected during scan.
        *
        * @param aIapId ID of the IAP.
        */
        virtual void NewNetworksDetected( TUint /*aIapId*/ ) {};

        /**
        * One or more networks have been lost since the last scan.
        *
        * @param aIapId ID of the IAP.
        */
        virtual void OldNetworksLost( TUint /*aIapId*/ ) {};
        
        virtual void WlanConnModeNotConnected() {};
};
    
/**
* Notifications' data.
*/
const TUint KHssMaxNotificationLength  = 128;   // Max data length for notification data.

/**
* HSS package data for notifications.
*/
struct THssPckgData
    {
    TBuf8<KHssMaxNotificationLength> data;
    };

/**
* CHssSrvNotifications offers the notification service.
* Active object that waits notifications from server.
*/
NONSHARABLE_CLASS( CHssSrvNotifications ) : public CActive
    {
    public: // Methods

        // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CHssSrvNotifications();
        
        /**
        * Create and activate notification service.
        * @note The notification service is activated in this call.
        * @param aCallback Callback interface to forward notifications to
        *                  user process.
        * @param aServer   Interface to send requests to server.
        * @return Pointer to a new constructed CHssSrvNotifications object.
        */
        static CHssSrvNotifications* NewL( MHssSrvNotifications& aCallback, RHssInterface& aServer );

        /**
        * Change the callback interface
        * and activate notifications
        * @param aCallback New callback interface.
        */
        void Activate( MHssSrvNotifications& aCallback );
        
        /**
         * Sets the requests cancelled flag so that no new notification requests
         * are done once current notification is complete.
         */
        void SetCancelled();

    protected: // from CActive
        /**
        * (From CActive) Receive notification.
        */
        void RunL();
        
        /**
        * (From CActive) This is called by Cancel() of CActive framework.
        */
        void DoCancel();

    private:
        /** 
        * Constructor.
        * @param aCallback Callback interface to forward notifications to
        *                  application.
        * @param aServer   Interface to send requests to server.
        */
        CHssSrvNotifications(MHssSrvNotifications& aCallback, RHssInterface& aServer);

        /**
        * Second phase construction.
        */
        void ConstructL();
        
        /**
        * Notification query loop.
        */
        void WaitForNotifications();

    private:    // Members.
        /**
         * Callback interface to the client
         */
        MHssSrvNotifications* iCallback;
        /**
         * Interface to send messages to the server
         */
        RHssInterface& iServer;
        /**
         * Return data value from server for the asynchronous messages
         */
        THssPckgData iReturnData;
        /**
         * Data package for return data
         */
        TPckg<THssPckgData> iDataPckg;
        /**
         * Cancel has been requested
         */
        TBool iCancelRequested;
        
        /**
         * IapId for notifications
         */
        TUint iIapId;
   };    
#endif // HSSSRVNOTIFICATIONS_H
