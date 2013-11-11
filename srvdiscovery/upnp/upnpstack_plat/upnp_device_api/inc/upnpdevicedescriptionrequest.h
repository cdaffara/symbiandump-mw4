/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of MUpnpSubscriberLibraryObserver
*
*/

//INCLUDES
#ifndef C_MUPNPDESCRIPTIONPROVIDER_H
#define C_MUPNPDESCRIPTIONPROVIDER_H

#include <e32base.h>
#include <in_sock.h>


//FORWARD DECLARATIONS
class CUpnpHttpMessage;

// CLASS DECLARATION
/**
*  
*/
NONSHARABLE_CLASS( CUpnpDeviceDescriptionRequest ): public CBase
{
public: // New functions 

   /**
    * Method is called on request of device icon 
    * @param aServiceUri service description uri
    * @param aServiceFile open handle to icon file    
    * @return System wide error code
    */    
    static CUpnpDeviceDescriptionRequest* NewL( const TDesC8& aUri,
                                                 const TInetAddr& aInetAddress );
    
    static CUpnpDeviceDescriptionRequest* NewLC( const TDesC8& aUri,
                                                  const TInetAddr& aInetAddress );
        
   
    /**
     * Method matches uri and returs a handle to an open file 
     * @param aServiceUri service description uri
     * @param aServiceFile open handle to icon file    
     * @return System wide error code
     */
    IMPORT_C void InetAddress( TInetAddr& aInetAddr );

    /**
     * Method matches uri and returs a handle to an open file 
     * @param aServiceUri service description uri
     * @param aServiceFile open handle to icon file    
     * @return System wide error code
     */
    IMPORT_C TPtrC8 Uri();
                    

    CUpnpDeviceDescriptionRequest::~CUpnpDeviceDescriptionRequest();
    
private:
    
    CUpnpDeviceDescriptionRequest::CUpnpDeviceDescriptionRequest( const TInetAddr& aInetAddress );

    void CUpnpDeviceDescriptionRequest::ConstructL( const TDesC8& aUri );
    
    RBuf8 iUri;
    
    TInetAddr iAddress;
        
    
};


#endif  // C_MUPNPDESCRIPTIONPROVIDER_H

//End of File
