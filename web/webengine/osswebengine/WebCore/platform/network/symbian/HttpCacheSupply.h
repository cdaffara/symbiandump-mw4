/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of CHttpCacheSupply
*
*/


#ifndef CHTTPCACHESUPPLY_H
#define CHTTPCACHESUPPLY_H

//  INCLUDES
#include <e32base.h>
#include <BrCtlDefs.h>
#include "httpcachemanager.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class HttpConnection;
class CPeriodic;
class CHttpCacheDataSupplier;

// CLASS DECLARATION

/**
*  This class handles http transactions.
*  @lib resLoader.lib
*  @since 3.1
*/
class CHttpCacheSupply : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param
        * @param
        * @param
        * @return Http cache  object.
        */
        static CHttpCacheSupply* NewL( HttpConnection* aHttpTransaction );

        /**
        * Destructor.
        */
        virtual ~CHttpCacheSupply();

    public: // new functions

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt StartRequestL( TBrCtlDefs::TBrCtlCacheMode aCacheMode );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void CloseRequest();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void HeadersReceivedL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void BodyReceivedL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ResponseCompleteL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt ResponseState() const { return m_reponseState;}

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool IsSupplying()             { return m_responseTimer!=0; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
		void PauseSupply();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
		void ResumeSupply();

    private:

        /**
        * Construct.
        * @param
        * @param
        * @parem
        * @return
        */
        CHttpCacheSupply( HttpConnection* aHttpTransaction );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: //

        /**
        * Callback for async response
        * @since 3.1
        * @param  aAny this pinter
        * @return TInt KErrNone
        */
        static TInt ResponseCallbackL( TAny* aAny );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SupplyResponseL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SendBodyL();

    private:    // Data

        //
        CHttpCacheManager*              m_cacheManager;          // not owned
        //
        HttpConnection*                 m_httpTransaction;       // not owned
        //
        TInt                            m_reponseState;
        //
        CPeriodic*                      m_responseTimer;         // owned
        //
        TBool                           m_failed;
        //
        TBool                           m_notModified;
        //
        THttpCacheEntry                 m_cacheEntry;
        // ETrue if the cache object is closed
        TBool							m_closed;

        CHttpCacheDataSupplier*         m_dataSupplier;
  };

#endif      // CHTTPCACHESUPPLY_H

// End of File
