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
         * Perform a broadcast scan and return the detected WLAN networks.
         *
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aResults Results of the scan.
         */
        void GetScanResults(
            TRequestStatus& aStatus,
            CWlanScanInfo& aResults );

        /**
         * Perform a direct scan for an SSID and return the detected WLAN networks.
         * If the SSID has zero length, a broadcast scan will be done.
         *
         * @param aSsid name of the WLAN network
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aResults Results of the scan.
         */
        void GetScanResults(
            TWlanSsid& aSsid,
            TRequestStatus& aStatus,
            CWlanScanInfo& aResults );
        
        /**
         * Cancel an outstanding scan request.
         */
        void CancelGetScanResults();

        /**
         * Get the available WLAN IAPs.
         *
         * @param aCacheLifetime Defines how many seconds old cached results the client
         *                       is willing to accept. The valid is range is from 0 to
         *                       60 seconds. The value of -1 means the system default will
         *                       be used. The aCacheLifetime parameter has a meaning only
         *                       when the aMaxDelay parameter is zero.
         *                       Value will be changed to the actual value used by the
         *                       system.
         * @param aMaxDelay Maximum amount of seconds the client is willing to wait for
         *                  the availability results. The valid range is from 0 to 1200
         *                  seconds or KWlanInfiniteScanDelay. KWlanInfiniteScanDelay
         *                  will never cause a scan, but the request will be
         *                  completed when any other broadcast scan request is completed.
         *                  Value will be changed to the actual value used by the system.
         * @param aFilteredResults Whether availability is filtered based on signal strength.
         *                  ETrue if filtering is allowed, EFalse if not.
         * @param aStatus Status of the calling active object. On successful
         *                completion contains KErrNone, otherwise one of the
         *                system-wide error codes.
         * @param aAvailableIaps Array of IAPs available.
         */
        void GetAvailableIaps(
            TInt& aCacheLifetime,
            TUint& aMaxDelay,
            TBool aFilteredResults,
            TRequestStatus& aStatus,
            RArray<TWlanIapAvailabilityData>& aAvailableIaps );

        /**
         * Cancel an outstanding IAP availability request.
         */
        void CancelGetAvailableIaps();
        
    private: // Data
        
        CWlanMgmtClient();
    };

#endif // WLANMGMTCLIENT_H
