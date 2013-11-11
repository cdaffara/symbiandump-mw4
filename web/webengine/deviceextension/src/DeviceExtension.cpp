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
* Description:  Implementation of the Device Extension object which is the
*                webengine part of the device interface
*
*/



// INCLUDE FILES
#include <e32std.h>
#include "DeviceExtension.h"

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CDeviceExtension::NewL
//
//
//
// ----------------------------------------------------------------------------

CDeviceExtension* CDeviceExtension::NewL(
	CWebKitView& aWebKitView )
    {
    CDeviceExtension* self = new ( ELeave ) CDeviceExtension( aWebKitView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CDeviceExtension::CDeviceExtension
//
//
//
// ----------------------------------------------------------------------------
CDeviceExtension::CDeviceExtension( CWebKitView& aWebKitView )
    :iWebKitView( &aWebKitView )
    {	
    }


// ----------------------------------------------------------------------------
// CDeviceExtension::~CDeviceExtension
//
//
//
// ----------------------------------------------------------------------------
CDeviceExtension::~CDeviceExtension()
    {
    if ( iDeviceBridge )
        {
        delete iDeviceBridge;
        }

    dLibrary.Close();
    }

// ----------------------------------------------------------------------------
// CDeviceExtension::ConstructL
//
//
//
// ----------------------------------------------------------------------------
void CDeviceExtension::ConstructL()
    {
    LoadDeviceDllL(); //Load the device interface implementation
    }

    
// ----------------------------------------------------------------------------
// CDeviceExtension::createDeviceObject
//
//
//
// ----------------------------------------------------------------------------
void* CDeviceExtension::createDeviceObject( void* aExec )
    {
    if ( iDeviceBridge )
        {
        return iDeviceBridge->Device( aExec );
        }

    return NULL;
    }    	
 

// -----------------------------------------------------------------------------
// CDeviceExtension::LoadDeviceDllL
//
// -----------------------------------------------------------------------------
//
void CDeviceExtension::LoadDeviceDllL()
    {
    if ( !iDeviceBridge )
        {
        _LIT( KBrowserDeviceLibName, "jsdevice.dll" );
        User::LeaveIfError( dLibrary.Load( KBrowserDeviceLibName ) );

        TLibraryFunction entry = dLibrary.Lookup( 1 );

        if ( !entry )
            User::Leave( KErrNotFound );

        //Create instance of DeviceBridge class
        iDeviceBridge = ( MDeviceBridge* ) entry();
        }
    }
    
//END OF FILE
