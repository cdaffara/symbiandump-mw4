/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef C_SEN_ATTRIBUTES_H
#define C_SEN_ATTRIBUTES_H

// INCLUDES

#include <e32base.h>
#include <flogger.h>

#include "senattribute.h"

#include "senpointermap.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DEFINITIONS

class CSenAttribute;

/**
 * @author lewontin
 * A list of CSenAttribute objects.
 * Each supports multiple values (for example, multiple groups). 
 */
class CSenAttributes : public CBase
    {

    public: 
    
        // Constructors and Destructors

        IMPORT_C static CSenAttributes* NewL();

        IMPORT_C static CSenAttributes* NewLC();


        IMPORT_C static CSenAttributes* NewL(const CSenAttributes& aAttributes);

        IMPORT_C static CSenAttributes* NewLC(const CSenAttributes& aAttributes);


        IMPORT_C virtual ~CSenAttributes();

        // New methods:

        IMPORT_C void AddAttributeL(const CSenAttribute& aValue);	//codescannerwarnings


        IMPORT_C const CSenAttribute& GetAttribute(const TDesC8& aName) const;


        // Number of attributes
        IMPORT_C TInt Count() const;


        // Attribute equality condition test
        IMPORT_C TBool Equals(const CSenAttribute& aAttr) const;


        // Match any attribute
        IMPORT_C TBool MatchAny(const CSenAttributes& aAttrs);


    protected:
        void BaseConstructL();
        void BaseConstructL(const CSenAttributes& aAttributes);

        

    private:

        CSenAttributes();

        // Returns attribute at index position
        const CSenAttribute& AttributeAt(TInt aIndex) const;


        /**
         * File logger.
         *
         */
        //RFileLogger* Log() const;


    private: // Data

        RSenPointerMap<TDesC8, CSenAttribute> iAttrList;

        //RFileLogger iLog;

    };

#endif // C_SEN_ATTRIBUTES_H

