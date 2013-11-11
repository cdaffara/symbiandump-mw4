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
* Description:  The client side connection to a feed on the FeedsSever.
*
*/


#ifndef FEEDS_SERVER_FEED_H
#define FEEDS_SERVER_FEED_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32base.h>
#include <e32std.h>

#include <FeedsServerRequestHandler.h>
#include <FeedsServerMsg.h>
#include "FeedsEntity.h"

#include "LeakTracker.h"
#include "Transaction.h"

// CONSTANTS

// MACROS


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CEnclosure;
class CFeed;
class CFeedsEntity;
class CItem;
class CPackedFeed;
class RFeedsServer;
class RFile;
class CFeedRequestHandler;

/**
*  The RFeed observer interface.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class MFeedObserver : public MConnectionObserver
    {
    public:  // New Methods
        /**
        * Called when the asynchronous request is complete.
        *
        * @since 3.1
        * @param aStatus The completion status code.
        * @return void.
        */
        virtual void FeedRequestCompleted(TInt aStatus) = 0;

        /**
        * Notifies the observer that the feed has changed.  This method is called
        * when the feed is updated by the feeds server.
        *
        * @since 3.1
        * @return void.
        */
        virtual void FeedChanged() = 0;

        /**
        * Notifies the observer that the feed's item status has changed.  
        * This method is called when __another__ client updates the feed's 
        * item status (via UpdateFeedItemStatusL).
        *
        * @since 3.1
        * @return void.
        */
        virtual void FeedItemStatusChanged() = 0;
    };


/**
*  The client side interface to a server resident Feed.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class RFeed: public RSubSessionBase, public MRequestHandlerObserver
    {
    public:
        /**
        * C++ default constructor.
        */
        IMPORT_C RFeed(MFeedObserver& aObserver);


    public:  // From MRequestHandlerObserver
        /**
        * Requests the observer to send a command to the server.
        *
        * @since 7.1
        * @param aFunction The function number identifying the request. 
        * @param aArgs A set of up to 4 arguments and their types to be passed to the server.
        * @param aStatus The request status object used to contain the completion status of the request.
        * @return void.
        */
        virtual void SendAsyncCommand(TInt /*aFunction*/, const TIpcArgs& /*aArgs*/, 
                TRequestStatus& /*aStatus*/) const {} ;

        /**
        * Preserving Binary Compatibility.
        *
        * @since 7.1
        * Empty function does nothing.
        * @return void.
        */
        virtual void RequestHandlerCompleted(CRequestHandler& /*aRequestHandler*/,
                TInt /*aStatus*/)  {} ;

        /**
        * Preserving Binary Compatibility.
        *
        * @since 7.1
        * Empty function does nothing.
        * @return void.
        */
        virtual void CancelAllCommandsL() { } ;

        /**
        * Request to create a network connection.
        *
        * @since 3.1
        * @param aConnectionPtr A pointer to the new connection. If NULL, the 
        *                       proxy filter will automatically create a network connection.
        * @param aSockSvrHandle A handle to the socket server.
        * @param aNewConn A flag if a new connection was created. If the 
        *                 connection is not new, proxy filter optimization will not 
        *                 read the proxy again from CommsBd.
        * @param aBearerType The bearer type of the new connection
        * @return void
        */
        virtual void NetworkConnectionNeededL(TInt* aConnectionPtr, TInt& aSockSvrHandle,
            TBool& aNewConn, TApBearerType& aBearerType);

    public:  // New methods

        /**
        * Notifies the observer that the request is completed.
        *
        * @since 3.1
        * @param aRequestHandler The request handler.
        * @param aStatus The result code of the request.
        * @return void.
        */
        void RequestHandlerCompletedL(CTransaction& aTrans, TInt aStatus);
 
        /**
        * Opens a session to the given feed on the server.
        *
        * @since 3.1
        * @param aFeedsServer The shared connection to the feeds server.
        * @return Error/success code
        */
        IMPORT_C TInt Open(RFeedsServer& aFeedsServer);

        /**
        * Closes the session.
        *
        * @since 3.1
        * @return void
        */
        IMPORT_C void Close();

        /**
        * Fetches the given feed.  FetchFeedCompleted is called upon completion.
        * The same url could appear multiple times in the feeds DB if each has different folder list ID.
        *
        * @since 3.1
        * @param aUrl The url of the feed.
        * @param aFolderListId The folder list ID associate with the feed.
        * @param aForceUpdate When true the server will ignore the cache.
        * @param aNoCache When true the server will not cache the feed.
        * @return void
        */
        IMPORT_C void FetchFeedL(const TDesC& aUrl, TInt aFolderListId = 0, TBool aForceUpdate = EFalse, 
                TBool aNoCache = EFalse);

        /**
        * Fetches the given feed -- using the feed's id.  FetchFeedCompleted
        * is called upon completion.
        *
        * @since 3.1
        * @param aFeedId The id of the feed (from the associated CFolderItem).
        * @param aForceUpdate When true the server will ignore the cache.
        * @param aNoCache When true the server will not cache the feed.
        * @return void
        */
        IMPORT_C void FetchFeedL(TInt aFeedId, TBool aForceUpdate = EFalse, 
                TBool aNoCache = EFalse);

        /**
        * Request the feeds server to update the feed's item status. 
        *
        * @since 3.1
        * @param aItemIds The id's of the items to update.
        * @param aItemStatus The new item status.
        * @return void
        */
        IMPORT_C void UpdateFeedItemStatusL(const RArray<TInt>& aItemIds,
                const RArray<TItemStatus>& aItemStatus, TInt aUnreadCount);

        /**
        * Returns ETrue if it has an available current feed.
        *
        * @since 3.1
        * @return Returns ETrue if it has an available current feed.
        */
        IMPORT_C TBool HasCurrentFeed() const;

        /**
        * Once fetched, it returns the CFeed.  This method panics the
        * client if it is called before the feed item is available.
        *
        * @since 3.1
        * @return The root CFeed.
        */
        IMPORT_C const CFeed& Feed() const;
    public:
        friend class RFeedsServer;

    private:  // Data
        MFeedObserver&                  iObserver;
        CFeed*                          iFeed;

        RPointerArray<CFeedRequestHandler>  iActiveRequests;
        TBool                           iPendingFetchFeed;
        RFeedsServer*                   iFeedsServer;

    };


/**
*  Holds information about a feed.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CFeed: public CBase
    {
    public: // Destructor
        /**
        * Destructor.
        */        
        IMPORT_C virtual ~CFeed();

    public: // New methods
        /**
        * Returns the feed's unique id.
        *
        * @since 3.1
        * @return The value.
        */
        IMPORT_C TInt Id() const;

        /**
        * Returns the feed's title.
        *
        * @since 3.0
        * @return The value.
        */
        IMPORT_C const TDesC& Title() const;

        /**
        * Returns the feed's description.
        *
        * @since 3.0
        * @return The value.
        */
        IMPORT_C const TDesC& Description() const;

        /**
        * Returns the feed's url.
        *
        * @since 3.0
        * @return The value.
        */
        IMPORT_C const TDesC& Url() const;

        /**
        * Returns the feed's Date.
        *
        * @since 3.0
        * @return The value.
        */
        IMPORT_C const TTime& Date() const;

        /**
        * Returns the number of items.
        *
        * @since 3.0
        * @return The number of items.
        */
        IMPORT_C TInt ItemCount(void) const;

        /**
        * Returns the item at the given index
        *
        * @since 3.0
        * @param aIndex The index of the item.
        * @return Returns the item or NULL.
        */
        IMPORT_C const CItem& ItemAt(TInt aIndex) const;

        /**
        * Returns the INITIAL status of each of the items in the feed.
        * The caller can then modify the values and call RFeed::UpdateFeedItemStatusL
        * to request the feeds server to update the feed's item status. 
        *
        * @since 3.1
        * @param aItemIds The ids of the items.
        * @param aItemStatus The coresponding status of each of the items in the feed.
        * @return void.
        */
        IMPORT_C void ItemStatusL(RArray<TInt>& aItemIds, 
                RArray<TItemStatus>& aItemStatus) const;

//#ifdef _DEBUG
        /**
        * Prints the feed to the log file.
        *
        * @since 3.0
        * @return void.
        */        
        IMPORT_C void Debug_Print(void) const;

        /**
        * Prints info about the feed.
        *
        * @since 3.0
        * @param aUrl The feeds url.
        * @param aFile The log file.
        * @return void.
        */        
        IMPORT_C void Debug_PrintInfo(const TDesC& aUrl, RFile& aFile) const;
//#endif

    private:  // New methods
        /**
        * Two-phased constructor.  aPackedFeed is adopted by this method.
        * Only called from RFeed.
        */
        static CFeed* NewL(CFeedsEntity* aFeedsEntity);

        /**
        * C++ default constructor.
        */
        CFeed(CFeedsEntity* aFeedsEntity);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Appends the new Item to the Feed.  aItem is adopted by this method.
        *
        * @since 3.0
        * @param aItem The Item
        * @return void.
        */
        void AddItemL(CItem* aItem);

        /**
        * Sets an attribute.
        *
        * @since 3.1
        * @param aAttribute The attribute.
        * @param aAttributeValue The attribute value.
        * @return void.
        */
        void SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue);

    private:  // Data
        TLeakTracker          iLeakTracker;

        CFeedsEntity*         iFeedsEntity;
        TInt                  iFeedId;
        TPtrC                 iTitle;
        TPtrC                 iDescription;
        TPtrC                 iUrl;
        TTime                 iDate;

        TInt                  iOtherTitles;
        TInt                  iOtherDescriptions;
        TInt                  iOtherLinks;

        RPointerArray<CItem>  iItems;

    public:  // Friends
        friend class CFeedRequestHandler;
        friend class RFeed;
    };


/**
*  Holds information about a feed's item.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CItem: public CBase
    {
    public:  // Destructor
        /**
        * Destructor.
        */        
        IMPORT_C virtual ~CItem();

    public: // New methods
        /**
        * Returns the item's id.
        *
        * @since 3.1
        * @return The value.
        */
        IMPORT_C TInt Id() const;

        /**
        * Returns the item's title.
        *
        * @since 3.0
        * @return The value.
        */
        IMPORT_C const TDesC& Title() const;

        /**
        * Returns the item's description.
        *
        * @since 3.0
        * @return The value.
        */
        IMPORT_C const TDesC& Description() const;

        /**
        * Returns the item's url.
        *
        * @since 3.0
        * @return The value.
        */
        IMPORT_C const TDesC& Url() const;

        /**
        * Returns the item's Date.
        *
        * @since 3.1
        * @return The value.
        */
        IMPORT_C const TTime& Timestamp() const;

        /**
        * Returns the item's status.
        *
        * @since 3.1
        * @return The value.
        */
        IMPORT_C TItemStatus ItemStatus() const;

        /**
        * Returns the number of enclosures.
        *
        * @since 3.1
        * @return The number of items.
        */
        IMPORT_C TInt EnclosureCount(void) const;

        /**
        * Returns the enclosure at the given index
        *
        * @since 3.1
        * @param aIndex The index of the enclosure.
        * @return The enclosure.
        */
        IMPORT_C const CEnclosure& EnclosureAt(TInt aIndex) const;
        
//#ifdef _DEBUG
        /**
        * Prints the item to the log file.
        *
        * @since 3.0
        * @return void.
        */        
        IMPORT_C void Debug_Print(void) const;
//#endif

    private:  // New methods
        /**
        * Two-phased constructor.
        * Only called from RFeed.
        */
        static CItem* NewL(CFeedsEntity* aFeedsEntity);

        /**
        * C++ default constructor.
        */
        CItem(CFeedsEntity* aFeedsEntity);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Appends the new enclosure to the Item.  aEnclosure is adopted by this method.
        * Only called from RFeed.
        *
        * @since 3.1
        * @param aEnclosure The enclosure
        * @return void.
        */
        void AddEnclosureL(CEnclosure* aEnclosure);

        /**
        * Sets an attribute.
        *
        * @since 3.1
        * @param aAttribute The attribute.
        * @param aAttributeValue The attribute value.
        * @return void.
        */
        void SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue);

    private:  // Data
        TLeakTracker               iLeakTracker;

        CFeedsEntity*              iFeedsEntity;
        TPtrC                      iTitle;
        TPtrC                      iDescription;
        TPtrC                      iUrl;
        TTime                      iTimestamp;
        TInt                       iId;
        TItemStatus                iItemStatus;

        RPointerArray<CEnclosure>  iEnclosures;

    public:  // Friends
        friend class CFeed;
    };


/**
*  Holds information about a feed's enclosure.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class CEnclosure: public CBase
    {
    public:  // Destructor
        /**
        * Destructor.
        */        
        IMPORT_C virtual ~CEnclosure();

    public: // New methods
        /**
        * Returns the enclosure's content-type.
        *
        * @since 3.1
        * @return The value.
        */
        IMPORT_C const TDesC& ContentType() const;

        /**
        * Returns the enclosure's Size.
        *
        * @since 3.1
        * @return The value.
        */
        IMPORT_C const TDesC& Size() const;

        /**
        * Returns the enclosure's url.
        *
        * @since 3.1
        * @return The value.
        */
        IMPORT_C const TDesC& Url() const;

//#ifdef _DEBUG
        /**
        * Prints the enclosure to the log file.
        *
        * @since 3.1
        * @return void.
        */        
        IMPORT_C void Debug_Print(void) const;
//#endif

    private:
        /**
        * Two-phased constructor.
        * Only called from RFeed.
        */
        static CEnclosure* NewL(CFeedsEntity* aFeedsEntity);

        /**
        * C++ default constructor.
        */
        CEnclosure(CFeedsEntity* aFeedsEntity);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Sets an attribute.
        *
        * @since 3.1
        * @param aAttribute The attribute.
        * @param aAttributeValue The attribute value.
        * @return void.
        */
        void SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue);

    private:  // Data
        TLeakTracker  iLeakTracker;

        CFeedsEntity* iFeedsEntity;
        TPtrC         iContentType;
        TPtrC         iSize;
        TPtrC         iUrl;

    public:  // Friends
        friend class CFeed;
    };

#endif  // FEEDS_SERVER_FEED_H
// End of File
