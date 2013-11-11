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
* Description: Header declaration
*
*/






/*
 * =========================================================================
 *  Name     : SenCertUtils.h
 */

#ifndef C_SEN_CERT_UTILS_H
#define C_SEN_CERT_UTILS_H

// INCLUDES

#include <e32base.h>


// CONSTANTS
_LIT8(KSenCASNDelimiter, "-"); // Delimiter for TCertInfo fields in string representation


// FORWARD DECLARATIONS
class TCertInfo; // from ssl.h

// CLASS DEFINITIONS

/**
 * Set of static convenience methods to process X.509 certificates
 *
 * <p>
 * (c) Copyright Nokia 2004, 2005
 * <p>
 * @author Norbert Leser
 */
class SenCertUtils
    {

    public:
        /**
         * CertInfoToStringLC
         * Converts TCertInfo type to string representation (TDesC8),
         * where fields are delimited by KSenCASNDelimiter
         *
         * @since Series60 4.0
         *
         * @param aCertInfo The certificate info object that provides a unique
         * identification of a certificate.
         *
         * @return newly allocated string representation of cert info, which is
         *         left on cleanupstack.
         */
        IMPORT_C static HBufC8* CertInfoToStringLC(const TCertInfo& aCertInfo);


    private:
        /**
        * Hide default C++ constructor.
        */
        SenCertUtils() { }

        /**
        * Prohibit copy constructor if not deriving from CBase.
        */
        SenCertUtils( const SenCertUtils& );

        /**
        * Prohibit assignment operator if not deriving from CBase.
        */
        SenCertUtils& operator=( const SenCertUtils& );
    };

#endif // C_SEN_CERT_UTILS_H

