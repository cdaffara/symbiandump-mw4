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



#ifndef HISTORYENTRY_H
#define HISTORYENTRY_H

//  INCLUDES
#include <e32base.h>
#include "HistoryInterface.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
namespace WebCore {
    class FormData;
}

// CLASS DECLARATION
class CFbsBitmap;

/**
* HistoryEntry
*
*  @lib ?library
*  @since ?Series60_version
*/
class HistoryEntry
{
    public:
        /**
        * Two-phased constructor.
        * @return HistoryEntry *
        */
        static HistoryEntry* initWithUrlAndFormData( const TPtrC8& url, const TPtrC8& requestUrl, 
            TPtrC& formContentType, WebCore::FormData* formData, HistoryCallback* callback, TBool wmlType);

        /**
        * Overwrite the last request URL with a new one. 
        * The original URL is released and the new one is added.
        */
        void setRequestUrlL (const TDesC& requestUrl);

        /**
        * Overwrite the last URL response with a new one. 
        * The original response is released and the new one is added.
        */
        void setResponseUrlL (const TDesC& responseUrl);

        /**
        * Sets the new x and y coordinates
        */
        void setPosition (const TPoint& position);
        /**
        * Sets the new zoom level
        */
        void setZoomLevel (int zoomLevel);
        /**
        * Sets the new min zoom level
        */
        void setMinZoomLevel (int zoomLevel);
        /**
        * Sets the page title
        */
        void setPageTitle (const TDesC& pageTitle);

        /**
        * Set the field in focus
        */
        void setActiveBoxId (int id);

        /**
        * Add a string to the list of post data
        * Leaves with error code KErrNoMemory if unable to allocate memory
        */
        void addFormData (WebCore::FormData* formData);

		/**
        * Set post content type
        * Leaves with error code KErrNoMemory if unable to allocate memory
        */
        void setPostContentTypeL (TPtrC postContentType);

        /**
        * Gets the URL Request
        * @return TPtrC to the request URL
        */
        TPtrC8 requestUrl () const {return *m_requestUrl;}

        /**
        * Returns a pointer to the URL response
        * @return TPtrC to the response
        */
		TPtrC8 responseUrl () const { return m_responseUrl ? *m_responseUrl : KNullDesC8(); } 

        /**
        * Returns the x and y coordinates
        * @return TPoint
        */

        TPoint position () const {return m_position;}
        /**
        * Returns zoom level
        * @return int
        */
        int zoomLevel () const {return m_zoomLevel;}

        /**
        * Returns min zoom level
        * @return int
        */
        int minZoomLevel () const {return m_minZoomLevel;}

        /**
        * Returns a pointer to the page title
        * @return TPtrC
        */
        TPtrC pageTitle () const {return *m_pageTitle;}

        /**
        * Returns the id of the field in focus
        * @return int
        */
        int activeBoxId () const {return m_activeBoxId;}

        /**
        * Returns a pointer to a post data string. 
        * Returns null if no post data is found or if the index is out of range.
        * @return TPtrC8
        */
        WebCore::FormData* formData () const { return m_formData; }

		/**
        * Returns a post data content type. 
        * Returns null if no post data is found or if the index is out of range.
        * @return TDesC
        */
        TPtrC postContentType () const {return m_postContentType ? m_postContentType->Des() : KNullDesC();}

        /**
        * Sets page thumbnail
        * @param aThumbnail - page thumbnail
        * @return void
        */
        void storeThumbnail(const CFbsBitmap* thumbnail, const TRect& area);

        /**
        * Returns page thumbnail
        * @return CFbsBitmap
        */
        CFbsBitmap* thumbnail() const {return m_thumbnail;}
 
        /**
        * Returns page thumbnail
        * @return CFbsBitmap
        */
        CFbsBitmap* constructThumbnailL();

        /**
        * Delets page thumbnail
        * @return void
        */
        void deleteThumbnail();

		/**
		*  Destructor
		*/
		~HistoryEntry();
		
        const TTime& creationTime() const { return m_creationTime; }

        /**
        * Changes the time during reload
        * @return void
        */
        void touch() {m_creationTime.HomeTime();}

        /**
        * Returns boolean indicator if WML content WAS at one time
        * (things change and a different content type may be current on reload or back nav...like 404 error might be html)
        * associated with the url stored in the history stack
        * @return TBool
        */
        TBool ifWml() { return m_ifWmlMode; }

    private:

		/**
		*  Default constructor
		*/
		HistoryEntry();  

		/*  Private Members  */

    private :

        // URL request string
        HBufC8* m_requestUrl;
        
		// HTML string response from URL request.  Used for caching the page
        HBufC8* m_responseUrl;
        
		// String containing the page title
        HBufC* m_pageTitle;
        
		// X and y coordinates of last viewing position.
        TPoint m_position;
        
		// Id of box with focus
        int m_activeBoxId;

        // Post body buffer
        WebCore::FormData* m_formData;

		//Post Content Type
		HBufC* m_postContentType;

		//void CWebKitBridge::SaveDocumentState(const TArray<TPtrC>& aDocumentState)
		//TArray<TPtrC>* CWebKitBridge::DocumentState() const

		// Number of entries allowed in the stack
		int m_historyStackStackSize;

        // Page thumbnail
        CFbsBitmap* m_thumbnail; // owned

        // The time this entry was created
        TTime m_creationTime;
		
        CBufFlat* m_thumbnailBuf; //owned
        HistoryCallback* m_callback;

        // true = wml; false = other stuff
        TBool m_ifWmlMode;

        // zoom level
        int m_zoomLevel;

        // min zoom level
        int m_minZoomLevel;
    };

#endif  // HISTORYENTRY_H

// End of File
