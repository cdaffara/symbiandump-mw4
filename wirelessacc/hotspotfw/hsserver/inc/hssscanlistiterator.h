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
* Description:   Declaration of the ScanListIterator class
*
*/



#ifndef HSSSCANLISTITERATOR_H
#define HSSSCANLISTITERATOR_H

#include "hssscanlist.h"

class HssScanList;

/**
*  Iterator for TScanList.
*/
class HssScanListIterator
{
public:

    /**
    * Return the size of the scan info. The size includes Status Info,
    * MAC header and Frame Body.
    * @return The size of the scan info in bytes.
    */
    inline TUint16 Size() const;

    /**
    * Find the data of the first access point.
    * @return Pointer at the beginning of the first access point stored 
    *         in the scan list. NULL if not any.
    */
    const HssScanFrame* First();

    /**
    * Find the data of the next access point.
    * @return Pointer at the beginning of the next access point stored
    *         in the scan list. NULL if not any.
    */
    const HssScanFrame* Next();

    /**
    * Find the data of the current access point.
    * @return Pointer at the beginning of the current access point stored 
    *         in the scan list. NULL if not any.
    */
    const HssScanFrame* Current() const;

    /**
    * Find is there any more unhandled access points.
    * @return False if there is access points in the list left, 
    *         True if not.
    */
    TBool IsDone() const;
    
protected: // Methods

    /**
    * C++ default constructor.
    *
    * Constructor is protected, so, class has to be inherited before use.
    */
    explicit HssScanListIterator( const HssScanList& scan_list );


private: //Methods

    // Prohibit copy constructor if not deriving from CBase.
    HssScanListIterator( const HssScanListIterator& );
    // Prohibit assigment operator if not deriving from CBase.
    HssScanListIterator& operator= ( const HssScanListIterator& );

protected: // Data
    /** Pointer to the beginning of the current item. */
    const HssScanFrame* current_m;

private:   // Data
    /** Information about scanned WLAN networks. */
    const HssScanList& scan_list_m;
};

#include "hssscanlistiterator.inl"

#endif      // HssSCANLISTITERATOR_H  
