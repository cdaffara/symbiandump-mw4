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

#ifndef C_MUPNPDEVICEDESCRIPTIONPROVIDER_H
#define C_MUPNPDEVICEDESCRIPTIONPROVIDER_H

//INCLUDES
#include <e32base.h>
#include <f32file.h>

//FORWARD DECLARATIONS
class CUpnpDeviceDescriptionRequest;

// CLASS DECLARATION
/**
*  This interface informs local device implementation owner 
*  about incomig description request via callbacks:
*  - OnDeviceDescription
*  - OnServiceDescription
*  - OnIcon
*  If callback returns error 404 Not found is returned to 
*  remote requestor otherwse GetFile() is invoked
*  which has to return open hadle to description file. 
*/
class MUpnpDeviceDescriptionProvider
{
public: // New functions 
   /**
    * Method is called on request of device icon 
    * @param aServiceUri service description uri
    * @param aServiceFile open handle to icon file    
    * @return System wide error code
    */    
    virtual TInt OnDeviceDescription( CUpnpDeviceDescriptionRequest& aRequest ) = 0;

   /**
    * Method is called on request of service description xml
    * @param aServiceUri service description uri
    * @param aServiceFile open handle to service description file
    * @return System wide error code.
    */
    virtual TInt OnServiceDescription( CUpnpDeviceDescriptionRequest& aRequest ) = 0;
   /**
    * Method is called on request of device icon 
    * @param aServiceUri service description uri
    * @param aServiceFile open handle to icon file    
    * @return System wide error code
    */    
    virtual TInt OnIcon( CUpnpDeviceDescriptionRequest& aRequest ) = 0;
    
   /**
    * Method matches uri and returs a handle to an open file 
    * @param aServiceUri service description uri
    * @param aServiceFile open handle to icon file    
    * @return System wide error code
    */    
    virtual TInt GetFile( const TDesC8& aUri, RFile& aFile ) = 0;
    
};


#endif  // C_MUPNPDEVICEDESCRIPTIONPROVIDER_H

//End of File
