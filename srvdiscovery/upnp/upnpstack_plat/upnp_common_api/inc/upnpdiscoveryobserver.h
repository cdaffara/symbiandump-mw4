/** @file
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Dispacther parent interface
*
*/


#ifndef C_MUpnpDiscoveryObserver_H
#define C_MUpnpDiscoveryObserver_H

// FORWARD DECLARATIONS
class CUpnpDevice;

/** 
    @brief An interface from dispatcher to its parent (device/control point)
    
    This interface class is used to provide an interface from dispather to 
    its users. 
*/
class MUpnpDiscoveryObserver
{
public:
    /** 
    * This function is used by dispatcher to get the path of the messages coming
    *       to this dispatcher.
    * @return The path identifying the parent of the dispatcher
    */
    virtual const TDesC8 & Path() = 0;  

    /** 
    * Device found
    * @param aDevice which has been found
    */
    virtual void DeviceFoundL( CUpnpDevice& aDevice ) = 0;

    /** 
    * Device lost
    * @param aDevice which has been lost
    */
    virtual void DeviceLostL( CUpnpDevice& aDevice ) = 0;
};

#endif // C_MUpnpDiscoveryObserver_H

// End of File
