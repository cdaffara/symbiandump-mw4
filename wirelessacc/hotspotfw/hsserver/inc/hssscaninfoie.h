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
* Description:   Declaration of the ScanInfoIe class.
*
*/



#ifndef HSSSCANINFOIE_H
#define HSSSCANINFOIE_H

#include "hssscaninfo.h"

/** Defines the possible types of security information elements. */
enum HssScanInfoIeType
    {
    HssScanInfoIeTypeRsn,
    HssScanInfoIeTypeWpa
    };

/** Defines the possible types of key management suites. */
enum HssScanInfoIeKeyManagement
    {
    HssScanInfoIeKeyManagementEap,
    HssScanInfoIeKeyManagementPsk
    };

/**
* Class determining the security mode of an access point.
*/
class HssScanInfoIe
    {
    public: // Methods

       // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        HssScanInfoIe();

        /**
        * Destructor.
        */
        virtual ~HssScanInfoIe();

       // New methods

        /**
        * Parses the given scan information and returns the security
        * mode of the access point.
        * @param info Scan information to be parsed.
        * @return Security mode of the access point.
        */
        HssSecurityMode SecurityMode(
            HssScanInfo& info );

    private: // Methods

        /**
        * Checks whether the given memory buffers are equal.
        * 
        */
        int Compare(
            const unsigned char* pl,
            int ll, 
            const unsigned char* pr, 
            int rl );

        /**
        * Checks whether the given information element defines
        * the expected key management suite.
        */
        TBool IsKeyManagement(
            HssScanInfoIeType ie_type,
            HssScanInfoIeKeyManagement key_type,
            TUint8 ie_length,
            const TUint8* ie_data
            );

        /**
        * Checks whether the given information element defines
        * valid ciphers suites for WPA2.
        */
        TBool IsWpa2Ciphers(
            HssScanInfoIeType ie_type,
            TUint8 ie_length,
            const TUint8* ie_data
            );

        // Prohibit copy constructor if not deriving from CBase.
        HssScanInfoIe(
            const HssScanInfoIe& );
        // Prohibit assigment operator if not deriving from CBase.
        HssScanInfoIe& operator= (
            const HssScanInfoIe& );
    };

#endif // HssSCANINFOIE_H
