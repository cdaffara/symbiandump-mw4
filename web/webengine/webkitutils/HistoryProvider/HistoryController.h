/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of HistoryController
*
*/


#ifndef HISTORYCONTROLLER_H
#define HISTORYCONTROLLER_H

//  INCLUDES
#include <e32base.h>
#include <BrCtlDefs.h>
#include "HistoryInterface.h"

// CONSTANTS

static const TUint KHistoryStackSize = 20;

// MACROS

// DATA TYPES

enum THistoryStackLocation
{

    EHistoryStackLocationAtBeginning,
    EHistoryStackLocationInMiddle,
    EHistoryStackLocationAtEnd
};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class HistoryView;

//typedef CArrayFixFlat<TBrCtlSelectOptionData> SelectArray;

// CLASS DECLARATION


/**
*  This is the entry point for the clients.
*  Call dispatch to have the request processed.
*  @lib resLoader.lib
*  @since 3.1
*/
class HistoryController : public HistoryControllerInterface
    {
    public:  // Constructors and destructor

        static IMPORT_C HistoryControllerInterface* initWithCallback( HistoryCallback* historyCallback,
            bool historyAllowed, bool backListAllowed);

        /**
        * Destructor.
        */
        virtual ~HistoryController();

    public: 

       /**
        * Insert
        * Add Url to History List
        * @since 5.x
        * @return void
        */
        virtual void insert( const TPtrC8& url, const TPtrC8& requestUrl,
            TPtrC& formContentType, WebCore::FormData* formData );

        /**
        * Implements virtual bool ContainsItemForURL(const TDesC& aUrl) = 0;)
        * @return bool
        * Return TRUE if URL request is in the history list
        */
        virtual bool containsItemForURL (const TPtrC& url);

        /**
        * PageInfoLC
        * Return the pageInfo requested, such as page's url or title
        * @since 3.x
        * @return HBufC* - The page information that was requested
        */
        virtual HBufC* pageInfoLC( TBrCtlDefs::TBrCtlPageInfo aBrCtlPageInfo );

        /**
        * HandleHistoryCommandL
        * Handle the commands related to history object
        * @since 3.x
        * @return void
        */
        virtual void handleHistoryCommandL( int command );

        /**
        * ClearHistoryList
        * Clears History List
        * @since 3.x
        * @return void
        */
        virtual void clearHistoryList();

        /**
        * CanGoBackOrForward
        * Check if back/forward is possible
        * @return void
        */
        virtual bool canGoBackOrForward( int distance );

        /**
        * Updates HistoryEntryThumbnail
        *
        */
        virtual void updateHistoryEntryThumbnailL(const CFbsBitmap* bitmap);

        /**
        * Set the title
        *
        */
        virtual void setCurrentEntryTitle( const TPtrC& pageTitle );

        /**
        * CurrentEntryPosition
        * Asks the history controller to update the current entry position
        */
        virtual void updateCurrentEntryPositionIfNeeded();

        /**
        * CurrentEntryPosition
        * Gets the current entry position
        */
        virtual TPoint currentEntryPosition();
        /**
        * CurrentEntryZoomLevel
        * Asks the history controller to update the current entry zoom level
        */
        virtual void updateCurrentEntryZoomLevelIfNeeded();

        /**
        * CurrentEntryZoomLevel
        * Gets the current entry zoom level
        */
        virtual int currentEntryZoomLevel();

        /**
        * CurrentEntryMinZoomLevel
        * Asks the history controller to update the current entry min zoom level
        */
        virtual void updateCurrentEntryMinZoomLevelIfNeeded();

        /**
        * CurrentEntryMinZoomLevel
        * Gets the current entry min zoom level
        */
        virtual int currentEntryMinZoomLevel();

        /**
         * Sets Url request 
         */
        //virtual void setRequestUrlL(const TPtrC& aUrl);

        /**
         * Sets Url Response 
         */
        //virtual void setResponseUrlL(const TPtrC& aUrl);
        
        /**
         * Gets the entry 
         */
        virtual HistoryEntry* entry (THistoryStackDirection direction);
        
        /**
         * Gets Url Request 
         */
        virtual TPtrC8 requestUrl();
        
        /**
        * Returns a pointer to the URL response
        * @return TPtrC to the response
        */
        virtual TPtrC8 responseUrl ();
        
        /**
        * Returns a pointer to HistoryEntry
        * @return HistoryEntry* to the wml hystory
        */
        virtual HistoryEntry* entryByIndex (int historyIndex);
        
        /**
         * Gets the index 
         */
        virtual int index (THistoryStackDirection direction);
        
        /**
        * Delete the entry from the list
        */
        virtual void deleteEntry (int index);

        /**
        * set the current entry
        */
        virtual void setCurrentL ( THistoryStackDirection direction );

        /**
        * Change the time stamp during a reload
        */
        virtual void updateGlobalHistoryForReload ();

        virtual HistoryCallback* historyCallback() { return m_historyCallback;}

        /**
        */
        virtual void setBackListAllowed( bool enabled ) { m_backListAllowed = enabled; }

        /**
        * History View
        * return History View
        */
        HistoryView* historyView() { return m_historyView; }

        /**
        * HistoryStack
        * return the HistoryStack pointer
        */
        RPointerArray<HistoryEntry>* HistoryStack() { return &m_historyStack; }

        /**
        * HistoryLength
        * @return the number of entries in the history list
        */
        int historyLength() const { return m_historyStack.Count(); }

        /**
        * GoBackOrForward
        * Load url from history + or - distance from current page
        * @return void
        */
        virtual void goBackOrForward( int distance );

        /**
        * CloseHistoryView
        * Closes history view
        */
        void closeHistoryView();

        /**
        * HistoryViewEnabled
        * returns true if BrCtl capability fow HistoryView was on
        */
        TBool historyViewEnabled() { return m_historyViewEnabled; }
        void setHistoryViewEnabled( TBool aEnabled ) { m_historyViewEnabled = aEnabled; }
        bool backListAllowed() { return m_backListAllowed; }

        /**
         * Update Display
         */
        void updateDisplay() const;

        int currentIndex() { return m_currentIndex; }
        void setCurrentIndex (int index) { if (index >= 0 && index < m_historyStack.Count()) m_currentIndex = index; }

        void setPossibleWmlOEB(TBool aFlag) { m_possibleWmlOEB = aFlag; }
        TBool possibleWmlOEB() { return m_possibleWmlOEB; }

        void performTransition(int direction);

        
    private:

        /**
        * Construct.
        * @param
        * @return
        */
        HistoryController(HistoryCallback* historyCallback, bool historyAllowed, bool backListAllowed);

        /**
        * ShowHistoryViewL
        *
        * @since 3.x
        * @return void
        */
        void showHistoryViewL(bool previous);

        /**
        * ShowHistoryListL
        *
        * @since 3.x
        * @return void
        */
        void showHistoryListL();

        /**
        * LoadHistoryEntry
        *
        * @since 3.x
        * @return void
        */
        void loadHistoryEntryL( CArrayFixFlat<TBrCtlSelectOptionData>& aHistoryList );

        void loadHistoryUrl(THistoryStackDirection direction, TBrCtlDefs::TBrCtlCacheMode cacheMode, int historyLoadOffset);

    private:    // Data

        HistoryCallback* m_historyCallback;
        // History Object
        RPointerArray<HistoryEntry> m_historyStack;
        // HistoryView
        HistoryView* m_historyView; // owned
        // denotes the offset if the load was as a result of history offset
        int m_historyLoadOffset;
        int m_currentIndex;
        int m_tempCurrentIndex; // keeps the current index while we do a history load
        // indicates if history view is enabled from BrowserControl capabilities
        bool m_historyViewEnabled;
        // User allows back list
        bool m_backListAllowed;
        // flag to indicate possible OEB when navigating from HTML, back through history, to WML content
        bool m_possibleWmlOEB;
    };

#endif      // HISTORYCONTROLLER_H

// End of File
