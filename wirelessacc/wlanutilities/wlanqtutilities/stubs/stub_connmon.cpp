/*
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
* Description:
* This is a source file for connmon library stub functions for emulator.
*/

#include <QString>
#include <QObject>
#include <QVariant>
#include <rconnmon.h>
#include <nifvar.h>
#include <cmmanager_shim.h>

#include "wlanqtutilsap.h"

#ifdef __WINS__

// -----------------------------------------------------------------------------
// CConnMonEventBase::CConnMonEventBase
// -----------------------------------------------------------------------------
//
CConnMonEventBase::CConnMonEventBase( const TInt aEventType, const TUint aConnectionId )
    {
    iEventType = aEventType;
    iConnectionId = aConnectionId;
    }

// Destructor
CConnMonEventBase::~CConnMonEventBase()
    {
    }

// -----------------------------------------------------------------------------
// CConnMonEventBase::EventType
// -----------------------------------------------------------------------------
//
TInt CConnMonEventBase::EventType() const
    {
    return iEventType;
    }

// -----------------------------------------------------------------------------
// CConnMonConnectionStatusChange::CConnMonConnectionStatusChange
// -----------------------------------------------------------------------------
//
CConnMonConnectionStatusChange::CConnMonConnectionStatusChange(
    const TUint aConnectionId,
    const TUint aSubConnectionId,
    const TInt aConnectionStatus ) :
    CConnMonEventBase(EConnMonConnectionStatusChange, aConnectionId)
    {
    iSubConnectionId = aSubConnectionId;
    iConnectionStatus = aConnectionStatus;
    }

// Destructor
CConnMonConnectionStatusChange::~CConnMonConnectionStatusChange()
    {
    }

#endif // __WINS__
