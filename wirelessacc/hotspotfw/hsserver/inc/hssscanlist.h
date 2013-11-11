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
* Description:   Declaration of the ScanList class
*
*/




#ifndef HSSSCANLIST_H
#define HSSSCANLIST_H

#include <e32std.h>

const TUint32 HSS_SCAN_LIST_DATA_SIZE   =  8000; ///< HssScanList container size (bytes)
const TUint32 HSS_APPEND_FAILED_NO_MEMORY = 0;    ///< Return value for Append function.

const TUint16 HSS_MANDATORY_IE_LIST_SIZE  = 3;
const TUint8  HSS_MANDATORY_IE_LIST[]     = { 0, 1, 3 };

typedef TUint8 HssScanFrame;

/**
*  Container class for delivering scan results.  
*/
class HssScanList
{

friend class HssScanListIterator;
friend class HssCScanTimer;

public: // Methods

    // Constructors and destructor

    /**
    * C++ default constructor.
    */
    HssScanList();

    /**
    * Returns the number of access points the instance contains.
    * @return the number of access points the instance contains.
    */
    inline TUint32 Count() const;

    /**
    * Appends one access point to list.
    * @param size          size of the accces point frame to be appended.
    * @param data_pointer   pointer to the frame.
    * @return HSS_APPEND_FAILED_NO_MEMORY if there is not enough space otherwise 
    *         the new size of the list.
    */
    TUint32 Append(TUint32 size, const HssScanFrame* data_pointer);

    /**
    * Returns the size of the list.
    * @return the size of the list.
    */
    inline TUint32 Size() const;

    /**
    * Returns the size of the container.
    * @return the size of the container.
    */
    inline TUint32 MaxSize() const;

    /**
    * Clears the list.
    */
    inline void ClearAll(); 

    /**
    * The scan result's alignment should be four bytes. This method returns
    * the size with needed padding.
    * @param  size Size of added data.
    * @return Total size with additional padding.
    */
    inline static TUint32 AddPadding( TInt size );

private: // Methods

    // Prohibit copy constructor.
    HssScanList( const HssScanList& );

    /**
    * Appends one acces point to list variables, but does not do the actual 
    * copying.
    * @param  size     The size of added memory block in bytes.
    * @return The new size of the list.
    */
    TUint32 Append( unsigned long size );

    /**
    * Check that the appended data is valid, i.e. it has all mandatory 
    * fields included. Once more: the data being checked has to be in
    * the buffer (iData).
    * @param  data Pointer to the beginning of the data being appended.
    * @return True if data can be append, False if it should be rejected.
    */
    TBool CheckData( HssScanFrame* data ) const;

private: // Data

    TUint8 data_m[HSS_SCAN_LIST_DATA_SIZE];  ///< Data Container (~4kB).
    TUint32 count_m;                     ///< Number of access points.
    TUint32 size_m;                      ///< Used bytes.
};

#include "hssscanlist.inl"

#endif // HssSCANLIST_H
