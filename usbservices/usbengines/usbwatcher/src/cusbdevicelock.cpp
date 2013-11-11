/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements device lock feature in case of USB
*
*/


#include <featmgr.h>
#include <bldvariant.hrh>

#include <e32base.h>
#include <e32property.h>
#include <coreapplicationuisdomainpskeys.h>
#include <e32property.h>

#include "debug.h"
#include "cusbdevicelock.h"
#include "cusbwatcher.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// C++ constructor
// ----------------------------------------------------------------------------
//
CUsbDeviceLock::CUsbDeviceLock( CUsbWatcher& aOwner )
    : CActive( EPriorityNormal )
    , iOwner( aOwner )
    {
    LOG_FUNC

    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUsbDeviceLock::~CUsbDeviceLock()
    {
    LOG_FUNC

    if( FeatureManager::FeatureSupported( KFeatureIdUsbDeviceLock ) )
        {
        Cancel();
        iProperty.Close();
        }

    FeatureManager::UnInitializeLib();
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUsbDeviceLock* CUsbDeviceLock::NewL( CUsbWatcher& aOwner )
    {
    LOG_FUNC

    CUsbDeviceLock* self = new ( ELeave ) CUsbDeviceLock( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // pop self
    return self;
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CUsbDeviceLock::ConstructL()
    {
    LOG_FUNC

    FeatureManager::InitializeLibL();

    if( FeatureManager::FeatureSupported( KFeatureIdUsbDeviceLock ) )
        {
        LEAVEIFERROR( iProperty.Attach( KPSUidCoreApplicationUIs,
            KCoreAppUIsAutolockStatus ) );
        }
    }

// ----------------------------------------------------------------------------
// Subscribes to P&S property.
// ----------------------------------------------------------------------------
//
void CUsbDeviceLock::Subscribe()
    {
    LOG_FUNC

    if( FeatureManager::FeatureSupported( KFeatureIdUsbDeviceLock ) )
        {
        if ( !IsActive() )
            {
            iProperty.Subscribe( iStatus );
            SetActive();
            }
        else
            {
            LOG( "ERROR: request exists" );
            }
        }
    }

// ----------------------------------------------------------------------------
// Get device lock state.
// ----------------------------------------------------------------------------
//
TBool CUsbDeviceLock::Locked()
    {
    LOG_FUNC

    TInt command;
    if( FeatureManager::FeatureSupported( KFeatureIdUsbDeviceLock ) )
        {
        TInt err = iProperty.Get( command );
        if(  KErrNone == err )
            {
            LOG1( "Autolock status = %d", command );
            if( EAutolockOff == command )
                {
                LOG("Device UNLOCKED" );
                return EFalse;
                }
            else
                {
                LOG( "Device LOCKED"  );
                }
            }
        else
            {
            LOG1( "ERROR: RProperty::Get = %d", err );
            }

        return ETrue;
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// Standard active object error function.
// ----------------------------------------------------------------------------
//
TInt CUsbDeviceLock::RunError( TInt /*aError*/ )
    {
    LOG_FUNC

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// When device lock state changes this method is executed.
// ----------------------------------------------------------------------------
//
void CUsbDeviceLock::RunL()
    {
    LOG_FUNC
    LOG1( "iStatus: %d", iStatus.Int() );
    Subscribe();
    TInt command = EAutolockStatusUninitialized;
    TInt err = iProperty.Get( command );
    if ( err == KErrNone )
        {
        LOG1( "Autolock status: %d", command );
        switch ( command )
            {
            case EAutolockOff:
                iOwner.Unlock();
                break;
            case ERemoteLocked:
                iOwner.Lock();
                break;
            default:
                LOG1( "Unhandled state: %d",  command );
                break;
            }
        }
    else
        {
        LOG1( "RProperty::Get ERROR %d", err );
        }
    }

// ----------------------------------------------------------------------------
// CUsbMediaWatcher::CUsbDeviceLock::DoCancel
// Basic DoCancel.
// ----------------------------------------------------------------------------
//
void CUsbDeviceLock::DoCancel()
    {
    LOG_FUNC

    if( FeatureManager::FeatureSupported( KFeatureIdUsbDeviceLock ) )
        {
        iProperty.Cancel();
        }
    }

// End if file
