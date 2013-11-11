/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Array of points
*
*/


#include "pointarray.h"

using namespace RT_GestureHelper;

// ----------------------------------------------------------------------------
// TPointArray constructor
// ----------------------------------------------------------------------------
//
TPointArray::TPointArray( const RArray< TPointEntry >& aPoints )
        : iPoints( aPoints )
    {}
    
// ----------------------------------------------------------------------------
// TPointArray length
// ----------------------------------------------------------------------------
//
TInt TPointArray::Count() const
    {
    return iPoints.Count();
    }

// ----------------------------------------------------------------------------
// point at index, no filtering
// ----------------------------------------------------------------------------
//
TPoint TPointArray::operator[]( TInt aIndex ) const 
    {
    return iPoints[aIndex].iPos;
    }

// ----------------------------------------------------------------------------
// Unfiltered point at index
// ----------------------------------------------------------------------------
//
TPoint TPointArray::Raw( TInt aIndex ) const
    {
    return iPoints[aIndex].iPos;
    }

