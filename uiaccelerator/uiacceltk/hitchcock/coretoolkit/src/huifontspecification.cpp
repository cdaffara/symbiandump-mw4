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
* Description:   Implementation of THuiFontSpecification.
*
*/



#include <uiacceltk/huifontspecification.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creates the THuiFontSpecification. Sets the specification parameters to default values.
// ---------------------------------------------------------------------------
//
THuiFontSpecification::THuiFontSpecification()
	{
	}

// ---------------------------------------------------------------------------
// Destroys the THuiFontSpecification. Makes sure all allocated resources are freed.
// ---------------------------------------------------------------------------
//
THuiFontSpecification::~THuiFontSpecification()
	{
	}

// ---------------------------------------------------------------------------
// Returns the typeface family description
// ---------------------------------------------------------------------------
//
const TDesC& THuiFontSpecification::TypefaceFamily() const
	{
    return iFontFamily;
	}

// ---------------------------------------------------------------------------
// Sets the typeface family
// ---------------------------------------------------------------------------
//
void THuiFontSpecification::SetTypefaceFamily(const TDesC& aName)
	{
	iFontFamily.Copy(aName);
	}
