/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the ScanListIterator class.
*
*/



#include "hssscanlist.h"
#include "hssscanlistiterator.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanListIterator::HssScanListIterator( const HssScanList& scan_list )
    :current_m( &scan_list.data_m[0] ),
     scan_list_m( scan_list )
{
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const HssScanFrame* HssScanListIterator::First()
{
    if ( scan_list_m.size_m == 0 )
    {
        // No data stored -> no first elemen -> set NULL.
        return NULL;
    }
    // Data exist -> set current at the beginning od the data.
    current_m = &scan_list_m.data_m[0];
    return current_m;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const HssScanFrame* HssScanListIterator::Next()
{
    current_m += HssScanList::AddPadding( Size() );
    return Current();
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const HssScanFrame* HssScanListIterator::Current() const
{
    if( current_m >= scan_list_m.data_m + scan_list_m.size_m )
    {
        return NULL;
    }

    return current_m;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool HssScanListIterator::IsDone() const
{
    return ( current_m >= scan_list_m.data_m + scan_list_m.size_m );
}
