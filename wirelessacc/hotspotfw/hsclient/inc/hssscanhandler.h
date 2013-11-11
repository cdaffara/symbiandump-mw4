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
* Description:   Implementation of iap
*
*/


#ifndef HssScanHandler_H
#define HssScanHandler_H

// INCLUDES
#include <e32base.h>
#include "hssmgmtcommon.h" 

// FORWARD DECLARATION
class CWlanMgmtClient;
class CWlanScanInfo;
class HssScanList;

// CLASS DECLARATION
/**
* @brief IAP settings handler
*/
class CHssScanHandler : public CBase
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
        * Static constructor.
        * @return Pointer to the constructed object.
        */
        static CHssScanHandler* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CHssScanHandler();
    
   	    /**
        * Issues a GetScanResults to wlan engine
        * @param aResults, HSS scan list
        * @return KErrNone if successful, 
        * otherwise one of system wide errorcodes.
        */
        TInt ScanRequest( HssScanList& aResults  );
        
        /**
        * Get the BSSID of the currently connected AP.
        * @param aBssId, MAC address of a connection
        * @return KErrNone if successful, 
        * otherwise one of system wide error codes.
        */
        TInt GetConnectionBssid( THssBssid& aBssId );
        
        /**
        * Add an SSID list to the given IAP.
        * @param aIapId, IAP ID given.
        * @param aSsidList, SSID list to add to the IAP.
        * @return KErrNone if successful,
        * otherwise one of the other system-wide error codes.
        */
        TInt AddIapSsidList(
                TUint aIapId,
                const CArrayFixFlat<THssSsid>& aSsidList );                

    protected:

        /**
        * C++ default constructor.
        */
        CHssScanHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();	
    
    private:    
    
               
    private: // Data
    
        /**
        * wlan management client instance
        * Own
        */
        CWlanMgmtClient* iMgmtClient;
        
        /**
        * pointer to scaninfo.
        * own
        */
        CWlanScanInfo* iScanInfo;


    };


#endif
