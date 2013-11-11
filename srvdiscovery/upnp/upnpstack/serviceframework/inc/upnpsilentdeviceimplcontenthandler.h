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
* Description:  Declares the CUpnpSilentDeviceImplContentHandler class
*
*/



#ifndef __UPNPSILENTDEVICEIMPLCONTENTHANDLER_H__
#define __UPNPSILENTDEVICEIMPLCONTENTHANDLER_H__

#include "upnpdevicecontenthandler.h"

/**
*  This class handles xml device descriptions
*  It extends CUpnpDeviceContentHandler class, thus it is subsequent implementation of MContentHandler interface
*  It is designed to be used by xml parser that generates events using callback methods   
*
*  @since Series60 2.6
*/


class CUpnpSilentDeviceImplContentHandler: public CUpnpDeviceContentHandler
{
public:
    
    /**    
    * Two phased constructor   
    * @return a pointer to the created instance of CUpnpSilentDeviceImplContentHandler
    */
    static CUpnpSilentDeviceImplContentHandler* NewLC( );
    
    /**    
    * Creates instance of CUpnpDevice class   
    * @return a pointer to the created instance of CUpnpDevice
    */                
    virtual CUpnpDevice* CreateDeviceL();
    
    /**    
    * Destructor of CUpnpSilentDeviceImplContentHandler class      
    */  
    virtual ~CUpnpSilentDeviceImplContentHandler();
        
private:
    
    /**    
    * Deafult C++ constructor
    */  
    CUpnpSilentDeviceImplContentHandler();        
        
};


#endif __UPNPSILENTDEVICEIMPLCONTENTHANDLER_H__