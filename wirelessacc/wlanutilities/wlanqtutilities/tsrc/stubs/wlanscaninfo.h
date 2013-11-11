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

#ifndef WLANSCANINFO_H
#define WLANSCANINFO_H

// INCLUDES
#include <e32def.h>
#include <e32base.h>
#include <wlanmgmtcommon.h>

#include <QSharedPointer>
#include "wlanqtutilsap.h"

typedef TUint8 TWlanScanInfoFrame;

// CLASS DECLARATION
class CWlanScanInfo : public CBase
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
         * Static constructor.
         * @return Pointer to the constructed object.
         */
        static CWlanScanInfo* NewL();
        
        /**
         * Destructor.
         */
        ~CWlanScanInfo();

        /**
         * Find the data of the first access point.
         * @return Pointer at the beginning of the first access point stored 
         *         in the scan list. NULL if not any.
         */
        const TWlanScanInfoFrame* First();

        /**
         * Find the data of the next access point.
         * @return Pointer at the beginning of the next access point stored
         *         in the scan list. NULL if not any.
         */
        const TWlanScanInfoFrame* Next();

        /**
         * Find is there any more unhandled access points.
         * @return EFalse if there is access points in the list left, 
         *         ETrue if not.
         */
        TBool IsDone() const;

        /**
         * Return RX level of the BSS.
         * @return RX level.
         */
        TUint8 RXLevel() const;

        /**
         * Return BSSID of the BSS.
         * @param  aBssid ID of the access point or IBSS network.
         * @return Pointer to the beginning of the BSSID. Length is always 6 bytes.
         */
        void Bssid(
            TWlanBssid& aBssid ) const;

        /**
         * Get capability of the BSS (see IEEE 802.11 section 7.3.1.4.
         * @return The capability information.
         */
        TUint16 Capability() const;

        /**
         * Return requested information element.
         * @param aIE        Id of the requested IE data.
         * @param aLength    Length of the IE. Zero if IE not found.
         * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
         * @return           General error message.
         */
        TInt InformationElement(
            TUint8 aIE, 
            TUint8& aLength, 
            const TUint8** aData );

        /**
         * Find whether Wi-Fi Protected Setup is supported.
         * @return ETrue if AP supports Wi-Fi Protected Setup,
         *         EFalse if not.
         */
        TBool IsProtectedSetupSupported();

        /**
         * Get security mode of the BSS.
         * @return security mode.
         */
        TWlanConnectionExtentedSecurityMode ExtendedSecurityMode() const;
        
    private: // Data

        /**
         * Constructor.
         */
        CWlanScanInfo();

        /**
         * Returns the currently iterated AP.
         */
        QSharedPointer<WlanQtUtilsAp> GetCurrentAp() const;
        
    };

#endif // WLANSCANINFO_H 
