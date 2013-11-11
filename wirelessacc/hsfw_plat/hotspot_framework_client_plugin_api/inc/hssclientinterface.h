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
* Description:   ECom interface for clients to implement and inherit.
*
*/



#ifndef HSSCLIENTINTERFACE_H
#define HSSCLIENTINTERFACE_H

// INCLUDES
#include "hssmgmtcommon.h"
#include "hssmgmtscaninfo.h"

// CLASS DECLARATION
/**
* @brief ECom interface class for HSS management services.
*
* This class contains the methods for managing HSS connections
* and querying the statuses of various connection variables.
* ECom Interface UID to be used: 10282ED1
*/
class MHssClientInterface
    {
    public:
    
        /**
        * Request to provide login information..
        * 
        * @param aIapID ID of the IAP.
        * @param aNetworkeID ID of the selected network.
        */    
        virtual void Login( const TUint aIapID, const TUint aNetworkID ) = 0;       
        
        /**
        * Cancel Login request.
        * 
        * @param aIapID ID of the IAP.
        */    
        virtual void CancelLogin( const TUint aIapID ) = 0;
        
        /**
        * Start associating IAP.
        * 
        * @param aIapID ID of the IAP.
        */    
        virtual void Start( const TUint aIapID ) = 0;
        
        /**
        * Update request of IAP associating.
        * 
        * @param aIapID ID of the IAP.
        */    
        virtual void Update( const TUint aIapID ) = 0;
        
        /**
        * Do end of connection procedures.
        * 
        * @param aIapID ID of the IAP.
        */    
        virtual void Logout( const TUint aIapID ) = 0;
        
        /**
        * Send connection opening status to client
        * Extended API "HSFWExt". 
        * 
        * @param aIapID ID of the IAP.
        * @param aAssociated ETrue if associated ok, otherwise EFalse.
        */
        virtual void WlanAssociationStatus( const TUint aIapID, TBool aAssociated ) = 0;
    };

#endif // HSSCLIENTINTERFACE_H
            
// End of File
