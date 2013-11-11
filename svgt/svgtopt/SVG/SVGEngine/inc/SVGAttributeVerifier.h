/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation header file
 *
*/


#ifndef _INC_CSVGATTRIBUTEVERIFIER_
#define _INC_CSVGATTRIBUTEVERIFIER_

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif


#include <badesca.h>

#define KSVG_COORDINATE_TYPE 2
#define KSVG_LENGTH_TYPE 1
const TInt KMax16BitValue = 32765;

/**
 * This class provides methods to verify attributes values, to reorder
 * the attributes and to check for required attributes for specific elements,
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgAttributeVerifier : public CBase
{
    public:


		 /**
         * Parse the given string for a decimal value.
         *
         * @since 1.0
         * @param aNumberString : decimal value string
         * @param aValueStorage : value storage
         * @return ETrue if string is a valid decimal value.
		 */
		static TBool ParseForDecimalValue( const TDesC& aNum2Str, TReal32& aValue, const TDesC& aAttrValue, TBool isSvgElement );

		/**
		 * Verify the range of the value of an attribute based on its type
		 * The string value is converted into the required type and passed back to
		 * the calling function
		 * @since 1.0
		 * @param aValues   : value of the attribute
		 * @param aConvVal  : reference parameter to return the converted value
		 * @param aDatatype : flag to determine the type of datatype
		 * @return ETrue if string is a valid decimal value and lies in a valid range.
		 */

		static TBool ValidAttrValue	(
		    const TDesC& aValue,
			TReal32& aConvVal,
			TUint8 aDatatype,
			const TDesC& aAttrValue,
			TBool isSvgElement
            );
};

#endif
