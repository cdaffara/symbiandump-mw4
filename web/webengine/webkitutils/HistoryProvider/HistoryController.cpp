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
* Description:  Implementation of HistoryController
*
*/


// INCLUDE FILES
#include <../bidi.h>
#include "HistoryController.h"
#include "BrCtlDefs.h"
#include "HistoryView.h"
#include "HistoryEntry.h"
#include "BrCtlDialogsProvider.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================= CLASSES METHODS===============================

// ============================ MEMBER FUNCTIONS ===============================

EXPORT_C HistoryControllerInterface* HistoryController::initWithCallback( HistoryCallback* historyCallback,
                                                      bool historyAllowed, bool backListAllowed )
{
    HistoryController* self = new HistoryController(historyCallback, historyAllowed, backListAllowed);
    return self;
}


HistoryController::~HistoryController()
{
	m_historyStack.ResetAndDestroy();
}

/**
*/
void HistoryController::insert( const TPtrC8& url, const TPtrC8& requestUrl, 
                               TPtrC& formContentType, WebCore::FormData* formData)
{
    updateCurrentEntryPositionIfNeeded();
    if (m_historyLoadOffset) {
        m_historyLoadOffset = 0;
        m_tempCurrentIndex = m_currentIndex;
        return;
    }
    HistoryEntry* entry = HistoryEntry::initWithUrlAndFormData(url, requestUrl, formContentType, 
        formData, m_historyCallback, m_historyCallback->wmlMode());
    if (entry) {
        int i;
        // Remove all entries after this entry
        for (i = historyLength() - 1; i > m_currentIndex; i--) {
            HistoryEntry* deadEntry = m_historyStack[i];
            m_historyStack.Remove(i);
            delete deadEntry;
        }
        // Make sure we don't exceed the limit
        if (historyLength() >= KHistoryStackSize) {
            HistoryEntry* deadEntry = m_historyStack[0];
            m_historyStack.Remove(0);
            delete deadEntry;
            m_currentIndex--;
        }
        int err = m_historyStack.Append(entry);
	    m_historyStack.Compress();
        if (err != KErrNone) { 
            delete entry;
        }
        else {
            m_currentIndex++;
        }
        m_tempCurrentIndex = m_currentIndex;
    }    
}

/**
*/
bool HistoryController::containsItemForURL (const TPtrC& url)
{
    HBufC8* tempHBuf8 = HBufC8::New(url.Length());
    if (! tempHBuf8)  return EFalse;
	tempHBuf8->Des().Copy( url );

    for (int i = historyLength() - 1; i>=0; i--){
        HistoryEntry* deadEntry = m_historyStack[i];
        if(deadEntry->requestUrl().Compare(tempHBuf8->Des()) == 0 ) {
            if(tempHBuf8) delete tempHBuf8;    
            return ETrue;
        }
    }
    if(tempHBuf8) delete tempHBuf8;
    return EFalse;
}

/**
*/
HBufC* HistoryController::pageInfoLC( TBrCtlDefs::TBrCtlPageInfo brCtlPageInfo )
{
    HBufC* pageInfo = NULL;
    
    switch( brCtlPageInfo )
    {
    case TBrCtlDefs::EPageInfoTitle:
        {
            TPtrC pageTitle = m_historyView->getCenterEntryTitle();
            if( pageTitle.Length() != 0 ) {
                pageInfo = pageTitle.AllocL();
            }
            break;
        }
    case TBrCtlDefs::EPageInfoUrl:
        {
            const HistoryEntry* currentEntry = entryByIndex(m_currentIndex);
            if( currentEntry && currentEntry->responseUrl().Length() ) {
                pageInfo = HBufC::NewL(currentEntry->responseUrl().Length());
                pageInfo->Des().Copy(currentEntry->responseUrl());
            }
            break;
        }
    default:
        {
            break;
        }
    }
    CleanupStack::PushL( pageInfo );
    return pageInfo;
}

/**
*/
void HistoryController::handleHistoryCommandL(int command)
{
    m_tempCurrentIndex = m_currentIndex;
    switch( command )
    {
    case TBrCtlDefs::ECommandReload:
        {
        // Get current url from history and call LoadUrlL(url)
        loadHistoryUrl( EHistoryStackDirectionCurrent, TBrCtlDefs::ECacheModeNoCache, m_historyLoadOffset);
        break;
        }
    case TBrCtlDefs::ECommandBack:
        {
        if (m_historyViewEnabled && m_backListAllowed) {
            showHistoryViewL(true);
        }
        else {
            loadHistoryUrl( EHistoryStackDirectionPrevious, TBrCtlDefs::ECacheModeHistory, -1);
        }
        break;
        }
    case TBrCtlDefs::ECommandOneStepBack:
        {
        loadHistoryUrl( EHistoryStackDirectionPrevious, TBrCtlDefs::ECacheModeHistory, -1);
        break;
        }
    case TBrCtlDefs::ECommandForward:
        {
        // Get next url from history and call LoadUrlL(url)
        loadHistoryUrl( EHistoryStackDirectionNext, TBrCtlDefs::ECacheModeHistory, 1);
        break;
        }
    case TBrCtlDefs::ECommandShowHistory:
        {
        if (m_historyViewEnabled && m_backListAllowed) {
            showHistoryViewL(false);
        }
        else {
            showHistoryListL();
        }
        break;
        }
    case TBrCtlDefs::ECommandClearHistory:
        {
        // Clear History contents
        clearHistoryList();
        break;
        }
    case TBrCtlDefs::ECommandCancel:
        {
        if (m_historyViewEnabled && m_backListAllowed) {
            closeHistoryView();
            m_historyLoadOffset = 0;
        }
        break;
        }
    default:
        {
        break;
        }
    }
}

/**
*/
void HistoryController::clearHistoryList()
{
    int i;
    for (i = historyLength() - 1; i >= 0; i--) {
        if (i != m_currentIndex) {
            HistoryEntry* deadEntry = m_historyStack[i];
            m_historyStack.Remove(i);
            delete deadEntry;
        }
    }

    m_currentIndex  = (m_currentIndex != -1) ? 0: m_currentIndex ;
    m_tempCurrentIndex = m_currentIndex;
    m_historyLoadOffset = 0;
}

/**
*/
bool HistoryController::canGoBackOrForward(int distance)
{
    int newCurrent = m_currentIndex + distance;
    if ( newCurrent >= 0 && newCurrent < m_historyStack.Count()) {
        return true;
    }
    return false;
}

/**
*/
void HistoryController::updateHistoryEntryThumbnailL(const CFbsBitmap* bitmap)
{
    if(m_historyViewEnabled) {
        HistoryEntry* entry = entryByIndex(m_currentIndex);
        if (entry) {
            TSize bmsize = bitmap->SizeInPixels();
            TRect parentControlRect = m_historyCallback->parent()->Rect();
            int historyViewWidth = parentControlRect.Width();
            int historyViewHeight( parentControlRect.Height());
            // Find out that in either portrait or landscape view what is the max height
            int maxDimension = (historyViewWidth > historyViewHeight)? historyViewWidth:historyViewHeight;
            int thumbnailHeight = Min(bmsize.iHeight, maxDimension*KCenterThumbnailHeightPercent/100);
            int thumbnailWidth = Min(bmsize.iWidth, maxDimension*KCenterThumbnailWidthPercent/100);
            entry->storeThumbnail(bitmap, TRect(0,0,thumbnailWidth, thumbnailHeight));
        }
    }
}

/**
*/
void HistoryController::setCurrentEntryTitle( const TPtrC& pageTitle )
{
    HistoryEntry* entry = entryByIndex(m_currentIndex);
    if (entry) {
        entry->setPageTitle(pageTitle);
    }
}

/**
*/
void HistoryController::updateCurrentEntryPositionIfNeeded()
{
    HistoryEntry* entry = entryByIndex(m_tempCurrentIndex);
    if (entry) {
        TPoint cp(m_historyCallback->currentPosition());
        if (!m_historyCallback->wmlMode()) {
            // update the entry position
            entry->setPosition(cp) ;
        }
    }
}



/**
*/
TPoint HistoryController::currentEntryPosition()
{
    HistoryEntry* entry = entryByIndex(m_currentIndex);
    if (entry) {
        return entry->position();
    }
    return TPoint(0, 0);
}

/**
 */
//void HistoryController::setRequestUrlL(const TPtrC& aUrl);

/**
 * Sets Url Response 
 */
//void HistoryController::setResponseUrlL(const TPtrC& aUrl);

/**
 * Gets the entry 
 */
HistoryEntry* HistoryController::entry(THistoryStackDirection direction)
{
    int index = 0;
    switch (direction)
        {
        case EHistoryStackDirectionPrevious:
            index = m_currentIndex - 1;
            break;
        case EHistoryStackDirectionNext:
            index = m_currentIndex + 1;
            break;
        case EHistoryStackDirectionCurrent:
        default:
            index = m_currentIndex;
            break;
        }
    return entryByIndex(index);
}

/**
 */
TPtrC8 HistoryController::requestUrl()
{
	    HistoryEntry* entry = entryByIndex(m_currentIndex);
        if (! entry) {
            return NULL;
        }
        return entry->requestUrl();
}

/**
*/
TPtrC8 HistoryController::responseUrl ()
{
	    HistoryEntry* entry = entryByIndex(m_currentIndex);
        if (! entry) {
            return NULL;
        }
        return entry->responseUrl();
}

/**
*/
HistoryEntry* HistoryController::entryByIndex (int historyIndex)
{
    if (historyIndex >= 0 && historyIndex < m_historyStack.Count()) {
        return m_historyStack[historyIndex];
    }
    return NULL;    
}

/**
 */
int HistoryController::index (THistoryStackDirection direction)
{
	//return m_historyStack.index (direction);
    TInt index = 0;
    switch (direction)
        {
        case EHistoryStackDirectionPrevious:
        index = currentIndex() - 1;
        break;

        case EHistoryStackDirectionNext:
        index = currentIndex() + 1;
        break;

        case EHistoryStackDirectionCurrent:
        default:
        index = currentIndex();
        break;
        }

    return index;
}

/**
*/
void HistoryController::deleteEntry(int index)
{
    if (index >= 0 && index < m_historyStack.Count()) {
        HistoryEntry* deadEntry = m_historyStack[index];
        m_historyStack.Remove(index);
        delete deadEntry;
        // Shift current page if removing previous pages
        if (index <= m_currentIndex && m_currentIndex >= 0)
            {
            m_currentIndex--;
            }
    }
}

/**
*/
void HistoryController::setCurrentL ( THistoryStackDirection direction )
{
	setCurrentIndex(index(direction));
}

/**
*/
void HistoryController::updateGlobalHistoryForReload()
{
	HistoryEntry* entry = entryByIndex(m_currentIndex);
	if ( entry )
		{
		entry->touch();
		updateCurrentEntryPositionIfNeeded();
		}
}

/**
*/
void HistoryController::goBackOrForward(int distance)
{    
    const HistoryEntry* currentEntry = entryByIndex(m_currentIndex+distance);
    if (currentEntry) {
		m_tempCurrentIndex = m_currentIndex;
        m_currentIndex += distance;
        m_historyLoadOffset = distance;
        TPtrC8 url = currentEntry->requestUrl();
        m_historyCallback->doHistoryGet( url, TBrCtlDefs::ECacheModeHistory );
    }
}

/**
*/
void HistoryController::closeHistoryView()
{
    m_historyLoadOffset = m_historyView->historyLoadOffset();
    delete m_historyView;
    m_historyView = NULL;
    // Update the display
    m_historyCallback->makeVisible(true);
    m_historyCallback->parent()->DrawNow();
    // inform UI that we have exited the History View
    // This should update the softkeys
    m_historyCallback->stateChanged(false);
    // delete bitmaps (they can be recreated from buffer)
    int i;
    for (i = 0; i < historyLength(); i++ ) {
        HistoryEntry* entry = m_historyStack[i];
        entry->deleteThumbnail();
    }
    //reset deferred timers on closing history view
    m_historyCallback->deferTimers(false);
}

/**
 */
void HistoryController::updateDisplay() const
{
    m_historyCallback->parent()->DrawNow();
}

/**
 */
HistoryController::HistoryController(HistoryCallback* historyCallback, bool historyAllowed, bool backListAllowed) : m_historyStack(20)
{
    m_historyView = NULL;
    m_historyViewEnabled = historyAllowed;
    m_backListAllowed = backListAllowed; 
    m_historyLoadOffset = 0;
    m_historyCallback = historyCallback;
    m_currentIndex = -1;
    m_tempCurrentIndex = m_currentIndex;
    m_possibleWmlOEB = false;
}

/**
*/
void HistoryController::showHistoryViewL(bool previous)
{
    HistoryEntry* entry = entryByIndex(m_currentIndex);
    if (entry && ! entry->thumbnail()) {
        // get scaled page from PageScaler;
        // update the history with new bitmap
        CFbsBitmap* scaledPage = m_historyCallback->scaledPage();
        if(scaledPage) {
            // Get the browser control rect
            updateHistoryEntryThumbnailL(scaledPage);
        }
            // ignore err since we will use the default image
    }
    //Defer refresh timers on showing history view
    m_historyCallback->deferTimers(true);
    m_historyView = HistoryView::NewL( *this, previous );
    m_historyCallback->makeVisible(false);
    m_historyCallback->parent()->DrawNow();
    // inform UI that we have enterd the History View
    // This should update the softkeys
    m_historyCallback->stateChanged(true);
    m_historyView->updateState(0);
}

/**
*/
void HistoryController::showHistoryListL()
{
    SelectArray* historyList = new( ELeave ) CArrayFixFlat<TBrCtlSelectOptionData>(10);
    CleanupStack::PushL( historyList );
    for( int i = m_historyStack.Count() - 1; i >= 0; i-- ) {
        TBrCtlSelectOptionData t( TBrCtlSelectOptionData(entryByIndex(i)->pageTitle(), i == m_currentIndex, false, false) );
        historyList->AppendL(t);
    }
    // Display history dialog
    bool ret = m_historyCallback->dialogSelectOption(historyList);
    if (ret) {
        int index = 0;
        for (; index < historyList->Count(); index++) {
            if ((*historyList)[index].IsSelected())
                break;
        }
        // Initialize a history load
        m_tempCurrentIndex = m_currentIndex;
        m_currentIndex = m_historyStack.Count() - index - 1;
        m_historyLoadOffset = index;
        loadHistoryUrl(EHistoryStackDirectionCurrent, TBrCtlDefs::ECacheModeNoCache, m_historyLoadOffset);
    }
    // else, user cancelled the dialog; do nothing about it
    historyList->Reset();
    CleanupStack::PopAndDestroy(); // historyList
}

/**
* LoadHistoryEntry
*
* @since 3.x
* @return void
*/
//void HistoryController::loadHistoryEntryL( CArrayFixFlat<TBrCtlSelectOptionData>& aHistoryList );

void HistoryController::loadHistoryUrl(THistoryStackDirection direction, TBrCtlDefs::TBrCtlCacheMode cacheMode, int historyLoadOffset)
{
    const HistoryEntry* currentEntry = entry(direction);
    if(currentEntry) {
        m_historyLoadOffset = historyLoadOffset;
        if (direction != EHistoryStackDirectionCurrent) {
            m_currentIndex += historyLoadOffset;
        }
        TPtrC8 url = currentEntry->requestUrl();
        if(currentEntry->postContentType().Length() && currentEntry->formData()) {
            m_historyCallback->doHistoryPost(url, cacheMode, currentEntry->postContentType(), currentEntry->formData());
        }
        else {
            m_historyCallback->doHistoryGet( url, cacheMode );
        }
    }
}

void HistoryController::performTransition(int direction)
{
	m_historyView->performTransition(direction);	
}

/**
*/
void HistoryController::updateCurrentEntryZoomLevelIfNeeded()
{
    HistoryEntry* entry = entryByIndex(m_tempCurrentIndex); 
    if (entry) {
        int cp(m_historyCallback->currentZoomLevel());
        if (!m_historyCallback->wmlMode()) {
            // update the entry position
            entry->setZoomLevel(cp) ;
        }
    }
}

/**
*/
int HistoryController::currentEntryZoomLevel()
{
    HistoryEntry* entry = entryByIndex(m_currentIndex);

    if (entry) {
        return entry->zoomLevel();
    }
    return 0; // default value should use in this case
}

/**
*/
void HistoryController::updateCurrentEntryMinZoomLevelIfNeeded()
{
    HistoryEntry* entry = entryByIndex(m_tempCurrentIndex); 

    if (entry) {
        int cp(m_historyCallback->minZoomLevel());
        if (!m_historyCallback->wmlMode()) {
            // update the entry position
            entry->setMinZoomLevel(cp) ;
        }
    }
}

/**
*/
int HistoryController::currentEntryMinZoomLevel()
{
    HistoryEntry* entry = entryByIndex(m_currentIndex);

    if (entry) {
        return entry->minZoomLevel();
    }
    return 0; // default value should use in this case
}

//  End of File
