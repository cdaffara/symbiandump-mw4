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
* Description:  Implementation of DeviceBridge
*
*/



// INCLUDE FILES
#include <config.h>
#include "Device.h"
#include "DeviceBridge.h"
#include "DeviceBinding.h"

using namespace KJS;
// ============================ LOCAL FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CreateDeviceBridge
//
//
//
// ----------------------------------------------------------------------------
//
EXPORT_C MDeviceBridge* CreateDeviceBridgeL()
    {
    return new (ELeave) TDeviceBridge();
    }


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// TDeviceBridge::TDeviceBridge
//
// ----------------------------------------------------------------------------
//
TDeviceBridge::TDeviceBridge() : m_device( 0 )
    {
    }

// ----------------------------------------------------------------------------
// TDeviceBridge::~TDeviceBridge
//
//
// ----------------------------------------------------------------------------
//
TDeviceBridge::~TDeviceBridge()
    {
    Clear();
    }    
    
// ----------------------------------------------------------------------------
// TDeviceBridge::Clear
//
//
// ----------------------------------------------------------------------------
//
void TDeviceBridge::Clear()
    {
    if ( m_device )
        {
        m_device->Close();
        KJS::Collector::unprotect(m_device);
        }
    m_device = NULL;
    }

// ----------------------------------------------------------------------------
// TDeviceBridge::Device
//
//
// ----------------------------------------------------------------------------
//
void* TDeviceBridge::Device( void* exec )
    {
    if ( !m_device )
        {
        m_device = new KJS::Device( static_cast<ExecState*>( exec ) );
        KJS::Collector::protect(m_device);
        }
    if ( m_device->valid() )
        return static_cast<void*>( m_device );
    else
        return NULL;
    }

// ----------------------------------------------------------------------------
// TDeviceBridge::SetUid
//
//
// ----------------------------------------------------------------------------
//
void TDeviceBridge::SetUid( const TUint& aValue)
    {
    if(m_device)
        m_device->SetUid( aValue);
    }
    
    
void* TDeviceBridge::GetSecuritySession()
    {
    if( m_device && (m_device->GetDeviceBinding()))
        {
        return static_cast<void*>(m_device->GetDeviceBinding()->GetSecuritySession());    
        }
    else
        {
        return NULL;
        }   
    }

//END OF FILE



