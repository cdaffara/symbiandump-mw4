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
* Description: 
*     This file contains the definition of transaction observer of Download Mgr Server.
*
*/



#ifndef __DOWNLOADMGR_CODOBSERVER_H__
#define __DOWNLOADMGR_CODOBSERVER_H__

#include <e32base.h>
#include <CodDownload.h>

// FORWARD DECLARATIONS
class RHttpDownloadMgr;

// CLASS DECLARATION
NONSHARABLE_STRUCT( SDMgrCodUserData )
    {
    public:
        MCodDownloadObserver::TEvent iPrevCodEvent;
        TInt                         iHandle;
    };

/**
*  Handles the responses of the asyncron services.
*  It can be created by a RHttpDownloadMgr session or 
*  RHttpDownload subsession.
*
*  @lib -
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS( CCodObserver ) : public CBase,
                                    public MCodDownloadObserver
	{
    public:

        /**
        * Two-phased constructor.
        */
        static CCodObserver* NewL( RHttpDownloadMgr* aDownloadMgr );
        
        /**
        * Destructor.
        */
        virtual ~CCodObserver();

    public: // From MCodDownloadObserver

	    void CodEventL( CCodDownload& aDownload, TEvent aEvent );

    private:

        /**
        * C++ default constructor.
        */
        CCodObserver( RHttpDownloadMgr* aDownloadMgr );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:

        RHttpDownloadMgr* iDownloadMgr;  ///< NOT Owned.
        TBool iDlAttrCodPdAvailableSet; // 
        TBool iReadyForPD;
        
        //Following  variables are used to store the values during progress update.
        //These values are sent to server only if they are changed
        TInt32 iCodLength;         //Stores the value of size of CodDownLoad length
        HBufC* iFilename;         //Stores the value CodDownload path name
        
	};


#endif /* __DOWNLOADMGR_CODOBSERVER_H__ */

