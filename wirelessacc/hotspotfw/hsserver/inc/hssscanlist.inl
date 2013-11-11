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
* Description:   Implementation of HssScanList inline methods.
*
*/



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 HssScanList::Count() const
{
    return count_m;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 HssScanList::Size() const
{
    return size_m;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 HssScanList::MaxSize() const
{
    return HSS_SCAN_LIST_DATA_SIZE;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void HssScanList::ClearAll()
{
    count_m = 0;
    size_m = 0;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 HssScanList::AddPadding( TInt size )
{
    // Note! size&3 has the same result than size%4
    return ( size&3 ) ? size + ( 4 - size&3 ) : size;
}
