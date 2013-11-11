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
* Description:  Encapsulates all the necessary information for a single page 
*      to be able to re-display the page.  
*      History Entry objects are stored on the History Stack.
*
*/



// INCLUDE FILES
#include "HistoryEntry.h"
#include <fbs.h>
#include <s32mem.h>
#include <uri8.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ========================== FUNCTIONS =========================

HistoryEntry* HistoryEntry::initWithUrlAndFormData( const TPtrC8& url, const TPtrC8& requestUrl, 
    TPtrC& formContentType, WebCore::FormData* formData, HistoryCallback* callback, TBool ifWmlType)
{
    HistoryEntry* entry = new HistoryEntry;
    if (entry) {
        entry->m_ifWmlMode = ifWmlType;
	    entry->m_requestUrl = requestUrl.Alloc();
        entry->m_responseUrl = url.Alloc();
        int offset = 0;
        if (requestUrl.Find(_L8("http://")) == 0) {
            offset = 7;
        }
        else if (requestUrl.Find(_L8("https://")) == 0) {
            offset = 8;
        }
        entry->m_pageTitle = HBufC::New(requestUrl.Length() - offset);
        if (entry->m_pageTitle) {
            entry->m_pageTitle->Des().Copy(requestUrl.Mid(offset));
        }
        entry->m_callback = callback;
        entry->m_zoomLevel = 0;
        entry->m_minZoomLevel = 0;
        if (formData && formContentType.Length()) {
            entry->m_postContentType = formContentType.Alloc();
            entry->m_formData = formData;
            entry->m_callback->setUse(true, entry->m_formData);
        }

        if (!entry->m_requestUrl || !entry->m_pageTitle || (formData && formContentType.Length() && !entry->m_postContentType)) {
            delete entry;
            entry = NULL;
        }
    }
    return entry;
}

/**
*/
//void HistoryEntry::setRequestUrlL (const TDesC& requestUrl);

/**
*/
//void HistoryEntry::setResponseUrlL (const TDesC& responseUrl);

/**
*/
void HistoryEntry::setPosition (const TPoint& position)
{
    m_position.SetXY(position.iX, position.iY);
}

/**
*/
void HistoryEntry::setZoomLevel (int zoomLevel)
{
    m_zoomLevel = zoomLevel;
}
/**
*/
void HistoryEntry::setMinZoomLevel (int zoomLevel)
{
    m_minZoomLevel = zoomLevel;
}

/**
*/
void HistoryEntry::setPageTitle(const TDesC& pageTitle)
{
    if (pageTitle.Length()) {
        HBufC* title = m_pageTitle;
        m_pageTitle = NULL;
        m_pageTitle = pageTitle.Alloc();
        if (m_pageTitle) {
            delete title;
        }
        else {
            m_pageTitle = title;
        }
    }
}

/**
*/
void HistoryEntry::setActiveBoxId (int id)
{
    m_activeBoxId = id;
}


/**
*/
//void HistoryEntry::addFormData (WebCore::FormData* formData);

/**
*/
//void HistoryEntry::setPostContentTypeL (TPtrC8 postContentType);

void HistoryEntry::storeThumbnail(const CFbsBitmap* thumbnail, const TRect& area)
{
    delete m_thumbnail;
    m_thumbnail = 0;
    if (!m_thumbnailBuf) {
        TRAP_IGNORE(m_thumbnailBuf = CBufFlat::NewL(4096));
    }
    if (m_thumbnailBuf) {
        RBufWriteStream stream(*m_thumbnailBuf);
		CleanupClosePushL(stream);
        TRAPD(err, thumbnail->ExternalizeRectangleL(stream, area));
        stream.Close();
		CleanupStack::PopAndDestroy(); //stream
        if (err) {
            delete m_thumbnailBuf;
            m_thumbnailBuf = 0;
        }
    }
}

/**
*/
CFbsBitmap* HistoryEntry::constructThumbnailL()
{
    if (!m_thumbnail && m_thumbnailBuf) {
        m_thumbnail = new (ELeave) CFbsBitmap();
        RBufReadStream stream(*m_thumbnailBuf);
        CleanupClosePushL(stream);
        m_thumbnail->InternalizeL(stream);
        CleanupStack::PopAndDestroy(); // stream
    }
    return m_thumbnail;
}

/**
*/
void HistoryEntry::deleteThumbnail()
{
    delete m_thumbnail;
    m_thumbnail = NULL;
}

/**
*/
HistoryEntry::~HistoryEntry()
{
    delete m_requestUrl;
    delete m_responseUrl;
    delete m_pageTitle;
    
	if(m_formData) {
            m_callback->setUse(false, m_formData);
    }	
    delete m_thumbnail;
	delete m_postContentType;
    delete m_thumbnailBuf;
}

/**
*/
HistoryEntry::HistoryEntry()
{
    m_requestUrl = NULL;;
    m_responseUrl = NULL;
    m_pageTitle = NULL;;
    m_position = TPoint(0,0);
    m_activeBoxId = 0;
    m_formData = NULL;
    m_postContentType = NULL;
    m_thumbnail = NULL;
    m_creationTime.HomeTime();
    m_thumbnailBuf = NULL;
    m_ifWmlMode = false;
}











/*
    
void HistoryEntry::SetRequestUrlL (const TDesC& aRequestUrl)
    {
    delete m_requestUrl;
    m_requestUrl = NULL;
	m_requestUrl = aRequestUrl.AllocL();
    }

void HistoryEntry::SetResponseUrlL (const TDesC& aResponseUrl)
    {
	delete m_responseUrl;
    m_responseUrl = NULL;
	m_responseUrl = aResponseUrl.AllocL();
    }


void HistoryEntry::AddPostData (CPostDataParts* aPostData)
    {
    if(iPostData)
		{
		iPostData->Deref();
		}
	iPostData = aPostData;
	if(iPostData)
		{
		iPostData->Ref();
		}
    }


void HistoryEntry::SetPostContentTypeL (TPtrC8 aPostContentType)
	{
	delete m_postContentType;
	m_postContentType = NULL;
    //
    m_postContentType = HBufC::NewL( aPostContentType.Length() ); 
    m_postContentType->Des().Copy( aPostContentType );
	}

*/
//  End of File
