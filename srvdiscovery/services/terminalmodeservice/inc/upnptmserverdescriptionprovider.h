/**
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
* Description: CUpnpTmServerDescriptionProvider class declaration
*
*/

#ifndef __UPNPTMSERVERDESCRIPTIONPROVIDER_H__
#define __UPNPTMSERVERDESCRIPTIONPROVIDER_H__

// Include Files
#include <e32base.h>
#include <upnpdevicedescriptionrequest.h>
#include <upnpdevicedescriptionprovider.h>
#include <upnpfileutils.h>
#include <upnpcons.h>

const TUint KForwardSlash = '/';

/**
 * CUpnpTmServerDescriptionProvider 
 * Provides description of device and its services, and icons. 
 */
class CUpnpTmServerDescriptionProvider : public CBase,
                                         public MUpnpDeviceDescriptionProvider
    {
public:
    // Destructor.
    ~CUpnpTmServerDescriptionProvider();

    // Two-phased constructor.
    static CUpnpTmServerDescriptionProvider* NewL();
    
private:    // from MUpnpDeviceDescriptionProvider
    
    virtual TInt OnDeviceDescription( CUpnpDeviceDescriptionRequest& aRequest );
    
    virtual TInt OnServiceDescription( CUpnpDeviceDescriptionRequest& aRequest );    
    
    virtual TInt OnIcon( CUpnpDeviceDescriptionRequest& aRequest );            
    
    virtual TInt GetFile( const TDesC8& aUri, RFile& aFile ); 

private:
     // Constructor for performing 1st stage construction
    CUpnpTmServerDescriptionProvider();

    //Default constructor for performing 2nd stage construction
    void ConstructL();

    /**
     * Main function to service requests.
     * @param aRequest request to service
     * @return error code
     */
    TInt OnRequest( CUpnpDeviceDescriptionRequest& aRequest );

    void OnRequestL( const TDesC8& aUrlPath, const TDesC8& aFileName );

private:
    // File server session used to open file
    RFs                                  iFs;
    TBuf<UpnpString::KMaxFilenameLength> iFilePath;  
    };

#endif // __UPNPTMSERVERDESCRIPTIONPROVIDER_H__
