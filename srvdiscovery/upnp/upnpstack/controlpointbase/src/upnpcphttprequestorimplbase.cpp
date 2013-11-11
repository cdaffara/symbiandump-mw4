/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class used in WLan Lost case
*
*/


// INCLUDE FILES
#include "upnpcphttprequestorimplbase.h"


// -----------------------------------------------------------------------------
// CUpnpCpHttpRequestorImplBase::InitialiseL
// -----------------------------------------------------------------------------
//
void CUpnpCpHttpRequestorImplBase::InitialiseL(
        MUpnpHttpClientObserver& /*aObserver*/, TInt /*aActiveIap*/ )
    {
    User::Leave( KErrNotReady );
    }

// -----------------------------------------------------------------------------
// CUpnpCpHttpRequestorImplBase::SendL
// -----------------------------------------------------------------------------
//
void CUpnpCpHttpRequestorImplBase::SendL( CUpnpHttpTransaction& /*aTransaction*/ )
    {
    User::Leave( KErrNotReady );
    }

//  End of File
