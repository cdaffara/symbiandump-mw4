/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef T_IPSECSALIFETIME_H
#define T_IPSECSALIFETIME_H

#include <e32base.h>

/**
 *  IPSec SA lifetime structure.
 *
 *  @lib ikeutils.lib
 */
class TIpsecSALifetime
    {    
public:    
    IMPORT_C TIpsecSALifetime( TUint32 aAllocations,
                               const TInt64& aBytes,
                               const TInt64& aAddtime,
                               const TInt64& aUsetime );
    
    IMPORT_C TIpsecSALifetime& operator=( const TIpsecSALifetime& aSource );
    
public: // data
    TUint32 iAllocations;
    TInt64 iBytes;
    TInt64 iAddtime;
    TInt64 iUsetime;    
    };

#endif // T_IPSECSALIFETIME_H
