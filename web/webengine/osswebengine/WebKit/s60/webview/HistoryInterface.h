/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of HistoryController
*
*/


#ifndef HISTORYINTERFACE_H
#define HISTORYINTERFACE_H

//  INCLUDES
#include <e32base.h>
#include <BrCtlDefs.h>

// CONSTANTS


// MACROS
class HistoryView;

// DATA TYPES
enum THistoryStackDirection
{

    EHistoryStackDirectionPrevious,
    EHistoryStackDirectionCurrent,
    EHistoryStackDirectionNext
};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TBrCtlDefs;
class TBrCtlSelectOptionData;
class HistoryEntry;
class HistoryCallback;
class CCoeControl;
class CFbsBitmap;
class WebSurface;

namespace WebCore {
    class FormData;
}

typedef CArrayFixFlat<TBrCtlSelectOptionData> SelectArray;

// CLASS DECLARATION

class HistoryCallback {
public:
    virtual CCoeControl* parent() = 0;
    virtual TPoint currentPosition() = 0;
    virtual int currentZoomLevel() = 0;
    virtual int minZoomLevel() = 0;
    virtual bool wmlMode() = 0;
    virtual void setWmlMode(bool aWml) = 0;
    virtual void doHistoryGet(TPtrC8 url, TBrCtlDefs::TBrCtlCacheMode cacheMode) = 0;
    virtual int doHistoryPost(TPtrC8 url, TBrCtlDefs::TBrCtlCacheMode cacheMode, TPtrC contentType, WebCore::FormData* formData) = 0;
    virtual void stateChanged(bool enter) = 0;
    virtual void navigationStateChanged(TBrCtlDefs::TBrCtlState state, bool value) = 0;
    virtual CFbsBitmap* scaledPage() = 0;
    virtual void makeVisible(bool visible) = 0;
    virtual WebSurface* surface() = 0;
    virtual void handleWmlBackL() = 0;
    virtual void setUse(bool use, WebCore::FormData* formData) = 0;
    virtual bool dialogSelectOption(SelectArray* historyList) = 0;
    virtual void deferTimers(bool option) = 0;
};

class HistoryControllerInterface {

    public: 
        virtual ~HistoryControllerInterface() {}

       /**
        * Insert
        * Add Url to History List
        * @since 5.x
        * @return void
        */
        virtual void insert( const TPtrC8& url, const TPtrC8& requestUrl, 
            TPtrC& formContentType, WebCore::FormData* formData ) = 0;

        /**
        * Implements virtual bool ContainsItemForURL(const TDesC& aUrl) = 0;)
        * @return bool
        * Return TRUE if URL request is in the history list
        */
        virtual bool containsItemForURL (const TPtrC& url) = 0;
        virtual int historyLength() const = 0;

        /**
        * PageInfoLC
        * Return the pageInfo requested, such as page's url or title
        * @since 3.x
        * @return HBufC* - The page information that was requested
        */
        virtual HBufC* pageInfoLC( TBrCtlDefs::TBrCtlPageInfo aBrCtlPageInfo ) = 0;

        /**
        * HandleHistoryCommandL
        * Handle the commands related to history object
        * @since 3.x
        * @return void
        */
        virtual void handleHistoryCommandL( int command ) = 0;

        /**
        * ClearHistoryList
        * Clears History List
        * @since 3.x
        * @return void
        */
        virtual void clearHistoryList() = 0;

        /**
        * CanGoBackOrForward
        * Check if back/forward is possible
        * @return void
        */
        virtual bool canGoBackOrForward( int distance ) = 0;

        /**
        * Updates HistoryEntryThumbnail
        *
        */
        virtual void updateHistoryEntryThumbnailL(const CFbsBitmap* bitmap) = 0;

        /**
        * Set the title
        *
        */
        virtual void setCurrentEntryTitle( const TPtrC& pageTitle ) = 0;

        /**
        * CurrentEntryPosition
        * Asks the history controller to update the current entry position
        */
        virtual void updateCurrentEntryPositionIfNeeded() = 0;

        /**
        * CurrentEntryPosition
        * Gets the current entry position
        */
        virtual TPoint currentEntryPosition() = 0;

        /**
        * CurrentEntryZoomLevel
        * Asks the history controller to update the current entry zoom level
        */
        virtual void updateCurrentEntryZoomLevelIfNeeded() = 0;

        /**
        * CurrentEntryZoomLevel
        * Gets the current entry zoom level
        */
        virtual int currentEntryZoomLevel() = 0;

        /**
        * CurrentEntryMinZoomLevel
        * Asks the history controller to update the current entry zoom level
        */
        virtual void updateCurrentEntryMinZoomLevelIfNeeded() = 0;

        /**
        * CurrentEntryMinZoomLevel
        * Gets the current entry zoom level
        */
        virtual int currentEntryMinZoomLevel() = 0;

        /**
         * Sets Url request 
         */
        //virtual void setRequestUrlL(const TPtrC& aUrl) = 0;

        /**
         * Sets Url Response 
         */
        //virtual void setResponseUrlL(const TPtrC& aUrl) = 0;
        
        /**
         * Gets the entry 
         */
        virtual HistoryEntry* entry (THistoryStackDirection direction) = 0;
        
        /**
         * Gets Url Request 
         */
        virtual TPtrC8 requestUrl() = 0;
        
        /**
        * Returns a pointer to the URL response
        * @return TPtrC to the response
        */
        virtual TPtrC8 responseUrl () = 0;
        
        /**
        * Returns a pointer to HistoryEntry
        * @return HistoryEntry* to the wml hystory
        */
        virtual HistoryEntry* entryByIndex (int historyIndex) = 0;
        
        /**
         * Gets the index 
         */
        virtual int index (THistoryStackDirection direction) = 0;
        
        /**
        * Delete the entry from the list
        */
        virtual void deleteEntry (int index) = 0;

        /**
        * set the current entry
        */
        virtual void setCurrentL ( THistoryStackDirection direction ) = 0;

        /**
        * Change the time stamp during a reload
        */
        virtual void updateGlobalHistoryForReload () = 0;

        virtual HistoryCallback* historyCallback() = 0;
        virtual void setBackListAllowed( bool enabled ) = 0;
        virtual HistoryView* historyView() = 0;
        virtual TBool historyViewEnabled() = 0;
        virtual void setHistoryViewEnabled( TBool aEnabled ) = 0;
        virtual void setCurrentIndex (int index) = 0;
        virtual void goBackOrForward( int distance ) = 0;
        virtual void setPossibleWmlOEB( TBool flag ) = 0;
        virtual TBool possibleWmlOEB() = 0;
        
		virtual void performTransition(int direction) = 0;

};



#endif      // HISTORYINTERFACE_H

// End of File
