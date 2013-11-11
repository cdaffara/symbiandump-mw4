/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Wrapper class for instantiating an implementation of
*                MHssScanInfoBase and MHssScanInfoIteratorBase
*                via ECom framework.
*
*/



#ifndef HSSMGMTSCANINFO_H
#define HSSMGMTSCANINFO_H

// INCLUDES
#include <ecom/ecom.h>
#include "hssmgmtscaninfointerface.h"

// CLASS DECLARATION
/**
* @brief Class for instantiating an implementation of MHssScanInfoBase
* and MHssScanInfoIteratorBase via ECom.
*
* This class encapsulates both the HSS scan results and methods
* needed for parsing and iterating through them.
*
* Scan results consist of a list of all the BSSs that the scan discovered
* and their parameters. Methods from MHssScanInfoIteratorBase are used
* to iterate through list of BSSs. Parameters of an individual BSS are
* parsed with methods from MHssScanInfoBase.
*/
class CHssMgmtScanInfo : public CBase, public MHssMgmtScanInfoBase,
                      public MHssMgmtScanInfoIteratorBase
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
        * Static constructor.
        * @return Pointer to the constructed object.
        */
        inline static CHssMgmtScanInfo* NewL();
        
        /**
        * Destructor.
        */
        inline virtual ~CHssMgmtScanInfo();

        
    private: // Data

        /**
         * Identifies the instance of an implementation created by
         * the ECOM framework.
         */
        TUid iInstanceIdentifier;
    };

#include "hssmgmtscaninfo.inl"

#endif // HSSMGMTSCANINFO_H 
            
// End of File
