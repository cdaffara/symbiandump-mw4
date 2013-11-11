/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* WLAN Management API stubbing for WLAN Qt Utilities.
*/

#ifndef WLANMGMTCLIENT_H
#define WLANMGMTCLIENT_H

#include "wlanmgmtcommon.h"
#include "wlanscaninfo.h"

class WlanMgmtClientContext;

// CLASS DECLARATION

class CWlanMgmtClient : public CBase
    {
    public:  // Methods

        // Constructors and destructor

        /**
         * Static constructor.
         * @return Pointer to the constructed object.
         */
        static CWlanMgmtClient* NewL();

        /**
         * Destructor.
         */
        ~CWlanMgmtClient();

        /**
         * Start Protected Setup.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aSsid SSID of the network to configure.
         * @param aWpsPin PIN value to be used. "00000000" (string of eight zeros)
         *                if push button method is used.
         * @param aCredentials Results of a successful Protected Setup operation.
         * @sa \link psetup Protected Setup-specific error codes \endlink.
         */
        virtual void RunProtectedSetup(
            TRequestStatus& aStatus,
            const TWlanSsid& aSsid,
            const TWlanWpsPin& aWpsPin,
            CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& aCredentials );
        
        /**
         * Cancel an outstanding Protected Setup operation.
         */
        virtual void CancelProtectedSetup();
        
    private: // Data
        
        CWlanMgmtClient();
        CArrayFixSeg<TWlanProtectedSetupCredentialAttribute> *iResults;
        TWlanSsid iSsid;
        TInt iCompletionCode;
        TBool iPushButtonMode;
        
        friend class WlanMgmtClientContext;
    };

#endif // WLANMGMTCLIENT_H
