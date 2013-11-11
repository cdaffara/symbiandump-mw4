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



#ifndef __DOWNLOADMGR_TRANSOBSERVER_H__
#define __DOWNLOADMGR_TRANSOBSERVER_H__

#include <e32base.h>
#include <f32file.h>
#include <http.h>

// FORWARD DECLARATIONS
class RHttpDownload;

// CLASS DECLARATION


/**
*  Handles the responses of the asyncron services.
*  It can be created by a RHttpDownloadMgr session or 
*  RHttpDownload subsession.
*
*  @lib -
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS( CTransactionObserver ) : public CBase,
                                            public MHTTPTransactionCallback
	{
    public:

        /**
        * Two-phased constructor.
        */
        static CTransactionObserver* NewL( RHttpDownload* aDownload );
        
        /**
        * Destructor.
        */
        virtual ~CTransactionObserver();

    public: // From MHTTPTransactionCallback

	    void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);


	    TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);


    public:

        /**
        * .
        * @param -
        * @return None.
        */

    private:

        /**
        * C++ default constructor.
        */
        CTransactionObserver( RHttpDownload* aDownload );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:

        RHttpDownload* iDownload;  ///< NOT Owned.
	};


#endif /* __DOWNLOADMGR_TRANSOBSERVER_H__ */

