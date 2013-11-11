/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  RUpnpSsdpMessageFactory is responisble for creating SSDP messages
*
*/


#ifndef C_RUPNPSSDPMESSAGEFACTORY_H
#define C_RUPNPSSDPMESSAGEFACTORY_H

//  INCLUDES
#include <e32base.h>
#include "upnpssdpmessage.h"

// DATA TYPES
typedef CUpnpSsdpMessage CSsdpAdvertise;
typedef CUpnpSsdpMessage CSsdpSearch;
typedef CUpnpSsdpMessage CSsdpSearchResponse;

// CLASS DECLARATION
/**
*  SSDP Message Factory. Used to create standard SSDP messages.
*
*  @since Series60 2.0
*/
class RUpnpSsdpMessageFactory
    {
    public:
        
     
        /**
        * Constructor function; Creates standard alive message 
        * with following parameters.
        * @since Series60 2.0
        * @param aLoc Value of the Location-header.
        * @param aNt Value of the NT-header.
        * @param aUsn Value of the USN-header.
        * @param aServerDesc Value of the server description-header.
        * @return CSsdpAdvertise pointer
        */
        IMPORT_C static CSsdpAdvertise* AdvertiseAliveL( TDesC8& aLoc,
                                                         TDesC8& aNt, 
                                                         TDesC8& aUsn, 
                                                         TDesC8& aServerDesc );

        /**
        * Constructor function; Creates standard byebye message 
        * with following parameters.
        * @since Series60 2.0
        * @param aNt Value of the NT-header.
        * @param aUsn Value of the USN-header.
        * @param aServerDesc Value of the server description-header.        
        * @return CSsdpAdvertise pointer
        */
        IMPORT_C static CSsdpAdvertise* AdvertiseByebyeL( TDesC8& aNt,
                                                          TDesC8& aUsn);
    
        /**
        * Constructor function; Creates standard search message 
        * with following parameters.
        * @since Series60 2.0
        * @param aTarget Value of the ST-header.
        * @return CSsdpSearch pointer
        */
        IMPORT_C static CSsdpSearch* SearchL( TDesC8& aTarget );
    
        /**
        * Constructor function; Creates standard search message 
        * with following parameters.
        * @since Series60 2.0
        * @param aTarget Value of the ST-header.
        * @param aMaximumWaitTime Value of the MX-header.
        * @return CSsdpSearch pointer
        */
        IMPORT_C static CSsdpSearch* SearchL( TDesC8& aTarget,
                                              TDesC8& aMaximumWaitTime );
    
        /**
        * Constructor function; Creates standard search response message 
        * with following parameters.
        * @since Series60 2.0
        * @param aSt Value of the ST-header.
        * @param aUsn Value of the USN-header.  
        * @param aLoc Value of the Location-header.  
        * @param aServerDesc Value of the Server description-header.
        * @return CSsdpSearchResponse pointer
        */
        IMPORT_C static CSsdpSearchResponse* ResponseL( TDesC8& aSt, 
                                                        TDesC8& aUsn, 
                                                        TDesC8& aLoc,
                                                        TDesC8& aServerDesc );
    };

#endif // C_RUPNPSSDPMESSAGEFACTORY_H

// End of File