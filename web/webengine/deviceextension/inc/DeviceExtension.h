/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declaration of the Device Extension object
*
*/

#ifndef __DEVICE_EXT_H__
#define __DEVICE_EXT_H__

//  INCLUDES
#include <e32base.h>
#include "DeviceBridge.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CWebKitView;


// CLASS DECLARATION
/**
*  CDeviceExtension
*
*  @lib jsdevice.dll
*  @since 5.0
*/

/*
 * The device extension holds the browser engine's part of the device object implementation. 
 */
 
 
class CDeviceExtension: public CBase
{
    public:
        
        static CDeviceExtension* NewL( CWebKitView& aWebKitView );
        
        virtual ~CDeviceExtension();
        
    protected:
        
        void ConstructL();

        CDeviceExtension( CWebKitView& aWebKitView );
        
    public: // DeviceExtension public methods 
        
        void* createDeviceObject( void* aExec );
        
    private: // DeviceExtension private methods
    
        void LoadDeviceDllL();
        
            
    private: // Data members
    
    	CWebKitView* iWebKitView; // Reference to the browser control that created us
    	MDeviceBridge* iDeviceBridge; //Interface to the JS Device object
    	RLibrary dLibrary; // Handle to loaded device dll
};

#endif //__DEVICE_EXT_H__