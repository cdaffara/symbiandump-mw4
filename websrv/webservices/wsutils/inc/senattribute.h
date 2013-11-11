/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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









#ifndef SEN_ATTRIBUTE_H
#define SEN_ATTRIBUTE_H

#include <e32base.h>

typedef RPointerArray<HBufC8> RAttrValueArray;


// CLASS DECLARATION

/**
 *  ?one_line_short_description.
 *  ?other_description_lines
 *
 *  @lib SenUtils.lib
 *  @since Series60 3.0
 */
class CSenAttribute : public CBase
    {
public:  // Constructors and destructor

    /**
     * Basic constructor.
     * @since Series60 3.0
     * @param aName the name of the attribute
     * @param aType the type of the attribute
     * @param aValue the value of the attribute
     */
    IMPORT_C static CSenAttribute* NewL(const TDesC8& aName,
                                        const TDesC8& aType,
                                        const TDesC8& aValue);
    /**
     * Basic constructor.
     * @since Series60 3.0
     * @param aName the name of the attribute
     * @param aType the type of the attribute
     * @param aValue the value of the attribute
     * @param aFriendlyName if the name is not descriptive, the friendly name
     * can be used to give morehuman-readble information
     */
    IMPORT_C static CSenAttribute* NewL(const TDesC8& aName,
                                        const TDesC8& aType,
                                        const TDesC8& aValue,
                                        const TDesC8& aFriendlyName);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CSenAttribute();

public: // New functions
    /**
     * Returns the attribute value as string.
     * If attribute is multi-valued, the first value added will be returned.
     * @since Series60 3.0
     * @return ?description
     */
    IMPORT_C TPtrC8 Value() const;

    /**
     * Returns the attribute name as string
     * @since Series60 3.0
     * @return ?description
     */
    IMPORT_C TPtrC8 Name() const;

    /**
     * Returns the actor string
     * @since Series60 3.0
     * @return ?description
     */
    IMPORT_C TPtrC8 Type() const;

    /**
     * Returns the friendly name
     * @since Series60 3.0
     * @return the firendly name for the attribute
     */
    IMPORT_C TPtrC8 FriendlyName() const;

    /**
     * Sets the value of the attribute, separately from instantiation.
     * Replaces existing value(s), if present.
     * @since Series60 3.0
     * @return void
     */
    IMPORT_C void SetValueL(const TDesC8& aValue);	//codescannerwarnings

    /**
     * Adds the value to the attribute, separately from instantiation.
     * If value(s) are already present in attribute, value is appended
     * to create multi-valued attribute.
     * @since Series60 4.0
     * @return void
     */
    IMPORT_C void AddValueL(const TDesC8& aValue);	//codescannerwarnings

    /**
     * Returns the values of the (multi-valued) attribute.
     * @since Series60 4.0
     * @return RAttrValueArray
     */
    IMPORT_C const RAttrValueArray GetValues() const;

    /**
     * Returns the number of values in the attribute.
     * @since Series60 4.0
     * @return TInt
     */
    IMPORT_C TInt NumValues() const;


    /**
     * Returns true if attributes are equal
     * (string equality of values, type and friendly name equality)
     * @since Series60 4.0
     * @return TBool
     */
    IMPORT_C TBool Equals(const CSenAttribute& aAttr) const;

public: // Functions from base classes

    protected:  // New functions
    /***
     * Constructor
     */
    IMPORT_C CSenAttribute();

    /**
     * Basic ConstructL function
     * @since Series60 3.0
     */

    IMPORT_C void ConstructL(const TDesC8& aName,
                             const TDesC8& aType,
                             const TDesC8& aValue,
                             const TDesC8& aFriendlyName);
private:    // Data

    HBufC8* ipName ;
    HBufC8* ipType ;
    HBufC8* ipFriendlyName ;

    RAttrValueArray iValues;

    // Reserved pointer for future extension
    TAny* ipReserved;
    };


#endif // SEN_ATTRIBUTE_H

