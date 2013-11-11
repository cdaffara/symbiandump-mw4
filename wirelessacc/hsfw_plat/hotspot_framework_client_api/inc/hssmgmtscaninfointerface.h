/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Interface definition for HotSpot scan results.
*
*/



#ifndef HSSMGMTSCANINFOINTERFACE_H
#define HSSMGMTSCANINFOINTERFACE_H

// INCLUDES
#include "hssmgmtcommon.h"

// DATA TYPES
typedef TUint8 THssMgmtScanInfoFrame;

// CLASS DECLARATION
/**
* @brief ECom interface class for HSS scan results iterator.
*
* This class contains the methods for iterating through
* HSS scan results.
*/
class MHssMgmtScanInfoIteratorBase
    {
    public:  // Methods

        /**
        * Return the size of the scan info. The size includes Status Info,
        * MAC header and Frame Body.
        * @return The size of the scan info in bytes.
        */
        virtual TUint16 Size() const = 0;

        /**
        * Find the data of the first access point.
        * @return Pointer at the beginning of the first access point stored 
        *         in the scan list. NULL if not any.
        */
        virtual const THssMgmtScanInfoFrame* First() = 0;

        /**
        * Find the data of the next access point.
        * @return Pointer at the beginning of the next access point stored
        *         in the scan list. NULL if not any.
        */
        virtual const THssMgmtScanInfoFrame* Next() = 0;

        /**
        * Find the data of the current access point.
        * @return Pointer at the beginning of the current access point stored 
        *         in the scan list. NULL if not any.
        */
        virtual const THssMgmtScanInfoFrame* Current() const = 0;

        /**
        * Find is there any more unhandled access points.
        * @return EFalse if there is access points in the list left, 
        *         ETrue if not.
        */
        virtual TBool IsDone() const = 0;
        
    };

/**
* @brief ECom interface class for HSS scan results.
*
* This class contains the methods for parsing the scan results
* of a HSS network.
*/
class MHssMgmtScanInfoBase
    {
    public:  // Methods

        /**
        * Return RX level of the BSS.
        * @return RX level.
        */
        virtual TUint8 RXLevel() const = 0;

        /**
        * Return BSSID of the BSS.
        * @param  aBssid ID of the access point or IBSS network.
        * @return Pointer to the beginning of the BSSID. Length is always 6 bytes.
        */
        virtual void Bssid( THssBssid& aBssid ) const = 0;

        /**
        * Get beacon interval of the BSS.
        * @return the beacon interval.
        */
        virtual TUint16 BeaconInterval() const = 0;

        /**
        * Get capability of the BSS (see IEEE 802.11 section 7.3.1.4.
        * @return The capability information.
        */
        virtual TUint16 Capability() const = 0;

        /**
        * Get security mode of the BSS.
        * @return security mode.
        */
        virtual THssConnectionSecurityMode SecurityMode() const = 0;

        /**
        * Return requested information element.
        * @param aIE        Id of the requested IE data.
        * @param aLength    Length of the IE. Zero if IE not found.
        * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
        * @return           General error code.
        */
        virtual TInt InformationElement( TUint8 aIE, 
                                         TUint8& aLength, 
                                         const TUint8** aData ) = 0;
        /**
        * Return WPA information element.
        * @param aLength    Length of the IE. Zero if IE not found.
        * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
        * @return           General error code.
        */
        virtual TInt WpaIE( TUint8& aLength, 
                            const TUint8** aData ) = 0;

        /**
        * Return the first information element.
        * @param aIE        Id of the IE. See IEEE 802.11 section 7.3.2.
        * @param aLength    Length of the IE. Zero if IE not found.
        * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
        * @return           General error code.
        */
        virtual TInt FirstIE( TUint8& aIE, 
                              TUint8& aLength, 
                              const TUint8** aData ) = 0;
        /**
        * Return next information element.
        * @param aIE        Id of the IE. See IEEE 802.11 section 7.3.2.
        * @param aLength    Length of the IE. Zero if IE not found.
        * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
        * @return           General error code.
        */
        virtual TInt NextIE( TUint8& aIE, 
                             TUint8& aLength, 
                             const TUint8** aData ) = 0;
    };

#endif // HSSMGMTSCANINFOINTERFACE_H 
            
// End of File
