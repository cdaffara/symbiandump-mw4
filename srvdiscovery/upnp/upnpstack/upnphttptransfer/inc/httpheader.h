/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Represents one header in transferred file
*
*/


#ifndef C_CHTTPHEADER_H
#define C_CHTTPHEADER_H


// System include files
#include <e32def.h>
#include <e32base.h>

/**
 * Class which represents one header in transferred file
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS ( CHttpHeader ) : public CBase
    {
public:
    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    ~CHttpHeader();
    
    /**
     * Default constructor
     *
     * @since S60 v3.2
     */
    CHttpHeader();
    
    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aFieldName Name of the header field
     * @param aFieldValue Value of the header field
     */
    static CHttpHeader* NewL( const TDesC8& aFieldName, 
                              const TDesC8& aFieldValue );
    
    /**
     * Getter for the name of the field
     *
     * @since S60 v3.2
     * @return Name of the field
     */
    const TDesC8& FieldName();
    
    /**
     * Getter for the value of the field
     *
     * @since S60 v3.2
     * @return Value of the field
     */
    const TDesC8& FieldValue();

private:
    /**
     * Second phase constructor
     *
     * @since S60 v3.2
     * @param aFieldName Name of the header field
     * @param aFieldValue Value of the header field
     */
    void ConstructL( const TDesC8& aFieldName, const TDesC8& aFieldValue );
    
private:

    /**
     * Header field name
     * Own.
     */
    HBufC8* iFieldName;
    
    /**
     * Value of the field
     * Own.
     */
    HBufC8* iFieldValue;
    };

#endif // C_CHTTPHEADER_H
