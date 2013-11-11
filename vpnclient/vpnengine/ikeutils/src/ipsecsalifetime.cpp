/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IPSec SA lifetime structure
*
*/


// CLASS HEADER
#include "ipsecsalifetime.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C TIpsecSALifetime::TIpsecSALifetime( TUint32 aAllocations,
                                             const TInt64& aBytes,
                                             const TInt64& aAddtime,
                                             const TInt64& aUsetime )
 : iAllocations( aAllocations ),
   iBytes(aBytes),
   iAddtime(aAddtime),
   iUsetime(aUsetime)
   {       
   }

// ---------------------------------------------------------------------------
// Assignment operator.
// ---------------------------------------------------------------------------
//
EXPORT_C TIpsecSALifetime& TIpsecSALifetime::operator=( const TIpsecSALifetime& aSource )
    { 
    if ( this != &aSource )
        { 
        iAllocations = aSource.iAllocations;
        iBytes = aSource.iBytes;
        iAddtime = aSource.iAddtime;
        iUsetime = aSource.iUsetime;
        }
    return *this; 
    }   
