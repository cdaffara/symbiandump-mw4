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
* Description:   scan info
*
*/



#ifndef HSSMGMTSCANINFOIMPL_H
#define HSSMGMTSCANINFOIMPL_H

// INCLUDES
#include "hssinterface.h"
#include "hssscaninfo.h"
#include "hssscanlist.h"
#include <hssmgmtscaninfo.h>

class HssScanInfo;

// CLASS DECLARATION
/**
* Implementation for MHssMgmtScanInfoBase and MHssMgmtScanInfoIteratorBase interfaces.
*
* This class uses services from CHssMgmtScanInfo and HssScanList classes to implement
* most of the required functionality.
*/
class CHssMgmtScanInfoImpl : public CHssMgmtScanInfo 

    {
    public:  // Methods

       // Constructors and destructor
        
        /**
        * Static constructor.
        */
        static CHssMgmtScanInfoImpl* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CHssMgmtScanInfoImpl();

       // New methods

		/**
		* Return the reference to the internal scan list.
		* @return Internal scan list.
		*/
        HssScanList& GetInternalList();

		/**
		* Update the internal variables after the internal
		* scan list has been updated.
		*/
        void UpdateResultsL();

       // Methods from base classes

        /**
        * (From MHssMgmtScanInfoIteratorBase) Return the size of the scan info.
		* The size includes Status Info, MAC header and Frame Body.
        * @return The size of the scan info in bytes.
        */
        virtual TUint16 Size() const;

        /**
        * (From MHssMgmtScanInfoIteratorBase) Find the data of the first access point.
        * @return Pointer at the beginning of the first access point stored 
        *         in the scan list. NULL if not any.
        */
        virtual const THssMgmtScanInfoFrame* First();

        /**
        * (From MHssMgmtScanInfoIteratorBase) Find the data of the next access point.
        * @return Pointer at the beginning of the next access point stored
        *         in the scan list. NULL if not any.
        */
        virtual const THssMgmtScanInfoFrame* Next();

        /**
        * (From MHssMgmtScanInfoIteratorBase) Find the data of the current access point.
        * @return Pointer at the beginning of the current access point stored 
        *         in the scan list. NULL if not any.
        */
        virtual const THssMgmtScanInfoFrame* Current() const;

        /**
        * (From MHssMgmtScanInfoIteratorBase) Find is there any more unhandled access points.
        * @return EFalse if there is access points in the list left, 
        *         ETrue if not.
        */
        virtual TBool IsDone() const;

		/**
        * (From MHssMgmtScanInfoBase) Return RX level of the BSS.
        * @return RX level.
        */
        virtual TUint8 RXLevel() const;

        /**
        * (From MHssMgmtScanInfoBase) Return BSSID of the BSS.
        * @param aBssid ID of the access point or IBSS network.
        */
		virtual void Bssid( THssBssid& aBssid ) const;

        /**
        * (From MHssMgmtScanInfoBase) Get beacon interval of the BSS.
        * @return the beacon interval.
        */
        virtual TUint16 BeaconInterval() const;

        /**
        * (From MHssMgmtScanInfoBase) Get capability of the BSS (see IEEE 802.11 section 7.3.1.4.
        * @return The capability information.
        */
        virtual TUint16 Capability() const;

        /**
        * (From MHssMgmtScanInfoBase) Get security mode of the BSS.
        * @return security mode.
        */
        virtual THssConnectionSecurityMode SecurityMode() const;

        /**
        * (From MHssMgmtScanInfoBase) Return requested information element.
        * @param aIE        Id of the requested IE data.
        * @param aLength    Length of the IE. Zero if IE not found.
        * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
        * @return           General error code.
        */
        virtual TInt InformationElement( TUint8 aIE, 
                                         TUint8& aLength, 
                                         const TUint8** aData );

        /**
        * (From MHssMgmtScanInfoBase) Return WPA information element.
        * @param aLength    Length of the IE. Zero if IE not found.
        * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
        * @return           General error code.
        */
        virtual TInt WpaIE( TUint8& aLength, 
                            const TUint8** aData );

        /**
        * (From MHssMgmtScanInfoBase) Return the first information element.
        * @param aIE        Id of the IE. See IEEE 802.11 section 7.3.2.
        * @param aLength    Length of the IE. Zero if IE not found.
        * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
        * @return           General error code.
        */
        virtual TInt FirstIE( TUint8& aIE, 
                              TUint8& aLength, 
                              const TUint8** aData );

        /**
        * (From MHssMgmtScanInfoBase) Return next information element.
        * @param aIE        Id of the IE. See IEEE 802.11 section 7.3.2.
        * @param aLength    Length of the IE. Zero if IE not found.
        * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
        * @return           General error code.
        */
        virtual TInt NextIE( TUint8& aIE, 
                             TUint8& aLength, 
                             const TUint8** aData );
        
    private: // Methods

        /**
        * C++ default constructor.
        */
        CHssMgmtScanInfoImpl();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
        
    private: // Data

        /** Scan results */
        HssScanList iScanList;

        /** Wrapper class for parsing */
        HssScanInfo* iScanInfo;
    };

#endif // HSSMGMTSCANINFOIMPL_H
            
// End of File
