/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Font specification is a platform-independent way of describing
*                font properties supported by Hitchcock UI Toolkit.
*
*/



#ifndef HUIFONTSPECIFICATION_H
#define HUIFONTSPECIFICATION_H

#include <e32std.h>
#include <gdi.h>

/**
 * THuiFontSpecification allows user to specify required font parameters for
 * a created font or the Hitchcock UI Toolkit to describe the parameters of
 * any given supported system font. This specification class can be used in
 * conjunction with THuiFontManager to create platform-supported fonts and query them.
 * @see THuiFontManager
 */
NONSHARABLE_CLASS(THuiFontSpecification)
{
public:
	
	/**
	 * Default constructor of font specification. Will assign default values to
	 * the specification parameters.
	 */
	THuiFontSpecification();
	
	/**
	 * Default destructor.
	 */
	virtual ~THuiFontSpecification();
	
    /** @beginAPI */
	
	/**
	 * Returns the typeface family name this font specification represents. E.g. "Helvetica".
	 */
	const TDesC& TypefaceFamily() const;
	
	/**
	 * Sets the typeface family name
	 * 
	 * @param aName The typeface family name used by the system or required by the user.
	 */
	void SetTypefaceFamily(const TDesC& aName);
	
    /** @endAPI */
	
private:

	/** Font typeface family */
	TBuf<KMaxTypefaceNameLength> iFontFamily;

};

#endif // HUIFONTSPECIFICATION_H
