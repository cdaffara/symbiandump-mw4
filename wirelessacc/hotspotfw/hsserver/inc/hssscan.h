/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Implementation of Active Object
*
*/



#ifndef HSSSCAN_H
#define HSSSCAN_H

//  INCLUDES
#include "HSSInterface.h"
#include "hotspotsession.h"

// Active object for Notification support
// FORWARD DECLARATIONS
class CHotSpotSession;


class CWlanMgmtClient;

// CLASS DECLARATION
/**
* Active object for Notification support.
*/
class CHssScan : public CActive
    {
    public:

        /**
        * C++ constructor and destructor
        */
        CHssScan( CWlanMgmtClient* aWlanMgmtClient, CHotSpotSession& aCallback);
        virtual ~CHssScan();
        
        /**
        * Initialize Scan to WlanEngine
        * @return None
        */
        void IssueRequest();

        /**
        * RunL to catch answer from WlanEngine
        * @return None
        */
        void RunL();
        
        /**
        * RunError to handle error situations
        * @return KErrNone
        */
        TInt RunError( TInt aError );
        
        /**
        * DoCancel to handle Cancel
        * @return None
        */
        void DoCancel();

    private:
    
        /**
        * Callback interface to Session.
        */
        CHotSpotSession& iCallback;

        /**
        * handle to wlanMgmtClient
        */
        CWlanMgmtClient* iMgmtClient;
        
        /**
        * pointer to scaninfo.
        */
        CWlanScanInfo* iScanInfo;
    };


#endif