/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef __DOWNLOADMGR_EVENTQUEUE_H__
#define __DOWNLOADMGR_EVENTQUEUE_H__

//  INCLUDES
#include <e32base.h>
#include <HttpDownloadMgrCommon.h>

// FORWARD DECLARATIONS
class CHttpDownload;
class CEventRecord;

// CLASS DECLARATION

/**
*  Record the download and its current states.
*  internal component
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
NONSHARABLE_CLASS( CEventRecord ) : public CBase
	{
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
	    CEventRecord( CHttpDownload* aDownload, 
                      TInt32 aDownloadState,
                      TInt32 aProgressState,
                      TInt32 aMoIndex );

        /**
        * Destructor.
        */
	    ~CEventRecord();

    public: // New function

        /**
        * .
        * @param -
        * @return None.
        */
        void States( TInt32& aDownloadState,
                     TInt32& aProgressState,
                     TInt32& aMoIndex );

        /**
        * .
        * @param -
        * @return None.
        */
        CHttpDownload* Download();

        /**
        * .
        * @param -
        * @return None.
        */
        TBool IsDownload( CHttpDownload* aDownload );

    private:  // Data
        // download on the  event occured
	    CHttpDownload* iDownload;  ///< NOT Owned.
        // download state
	    TInt32         iDownloadState;
        // download progress state
        TInt32         iProgressState;
        // media object index
        TInt32		   iMoIndex;
	};

/**
*  Queue the event sent by the download mgr engine.
*  internal component
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
NONSHARABLE_CLASS( CDownloadMgrEventQueue ) : public CBase
    {
    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */
	    CDownloadMgrEventQueue();

        /**
        * Destructor.
        */
	    ~CDownloadMgrEventQueue();

    public: // New functions

        /**
        * Checks the emptiness of the queue.
        * @param -
        * @return TBool.
        */
	    TBool IsEmpty() const;

        /**
        * Inserts an entry into the FIFO buffer.
        * @param -
        * @return TBool - returns ETrue in case of succes and EFalse otherwise (OOM).
        */ 
	    TBool In(  CHttpDownload* aDownload, 
                   THttpDownloadState aDlState,
                   THttpProgressState aProgState,
                   TInt32 aMoIndex );

        /**
        * Returns the next entry from the FIFO buffer.
        * @param -
        * @return TBool.
        */
	    CHttpDownload* Next( TInt32& aDownloadState,
                             TInt32& aProgressState,
                             TInt32& aMoIndex );

        /**
        * Removes the next entry from the queue.
        * @param -
        * @return None.
        */
	    void Out();

       /**
        * Remove download element(s) from the queue referenced by the parameter.
        * @param aDownload
        * @return None.
        */
        void Remove( CHttpDownload* aDownload );

    private:   // Data

        RPointerArray<CEventRecord> iEventQueue; ///< Has.
    };

#endif      // __DOWNLOADMGR_EVENTQUEUE_H__   
            
// End of File
