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
* Description:   Implementation of the ScanList class.
*
*/



#include "hssscanlist.h"
#include "hssscanoffsets.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanList::HssScanList():count_m( 0 ), size_m( 0 )
{
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 HssScanList::Append( TUint32 size, const HssScanFrame* data_pointer )
{

    if( AddPadding( size  ) >= ( HSS_SCAN_LIST_DATA_SIZE - size_m ) )
    {
    
        return HSS_APPEND_FAILED_NO_MEMORY;
    }

    TUint8* dst = data_m + size_m;
    const TUint8* src = data_pointer;
    const TUint8* end = data_pointer + size;

    for ( ; src < end; ++dst, ++src )
    {
        *dst = *src;
    }

    // The checking has to been done after copying it to the buffer
    // due it relies the buffers limits.
    if ( CheckData( data_m + size_m ) )
    { // Add data only if it is valid.
        size_m += AddPadding( size );
        ++count_m;
    }
    
    return size_m;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 HssScanList::Append( TUint32 size )
{

    if ( CheckData( data_m + size_m ) )
    { // Add data only if it is valid.
        size_m += AddPadding( size );
        ++count_m;
    }

    return size_m;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

TBool HssScanList::CheckData( HssScanFrame* data ) const
{
    // Check that the data is at the range.
    if ( data < data_m || 
         data > data_m + size_m || 
         size_m + sizeof( TUint16 ) > HSS_SCAN_LIST_DATA_SIZE )
    {
        return false;
    }

    // Calculate the length of the data.
    TUint16 size = static_cast<TUint16>( *( reinterpret_cast<const TUint32*>( data + LENGTH_OFFSET ) ) + CNTRL_HEADER_LEN);

    // Check that the whole data fits to the buffer.
    if ( data + size > data_m + HSS_SCAN_LIST_DATA_SIZE )
    {
        return false;
    }
    
    const TUint8* currentIE;

    // Search mandatory IEs.
    for ( TUint i = 0; i < HSS_MANDATORY_IE_LIST_SIZE; ++i )
    {
        // Go through all IEs.
        for ( currentIE = data + BODY_OFFSET;
              ; // No condition here -- it is in the loop.
              currentIE += *( currentIE + 1 ) + 2 )
        {
            if ( currentIE < data + size )
            { // Still searching...
                if ( *currentIE == HSS_MANDATORY_IE_LIST[i] )
                { // We have a match! Lets find out the next one.
                    break;
                }
            }
            else
            { // The IE was not found.
                return false;
            }
        }
    }
    // Data is ok.
    return true;
}
