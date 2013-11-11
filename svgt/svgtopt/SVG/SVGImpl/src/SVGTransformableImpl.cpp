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
* Description:  SVG Implementation source file
 *
*/


#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGTransformableImpl.h"


// ==========================================================================
// Need method description
// ==========================================================================
CSvgTransformableImpl* CSvgTransformableImpl::NewL()
    {
    CSvgTransformableImpl* self = new ( ELeave ) CSvgTransformableImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgTransformableImpl* CSvgTransformableImpl::NewLC()
    {
    CSvgTransformableImpl* self = new ( ELeave ) CSvgTransformableImpl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgTransformableImpl::~CSvgTransformableImpl()
    {
	if ( iTransform )
		{
		delete iTransform;
		iTransform = NULL;
		}
    }

// From MSvgTransformable

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgTransformableImpl::GetTransform( MSvgTransformList*& aTransformList )
    {
    aTransformList = ( MSvgTransformList * ) iTransform;
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgTransformableImpl::SetTransform( MSvgTransformList*& aTransformList )
    {
    iTransform = ( CSvgTransformListImpl * ) aTransformList;
	iTransform->SetAvoidConsolidation(EFalse);
    }

// From MSvgLocatable

// ==========================================================================
// Need method description
// ==========================================================================
const TGfxAffineTransform& CSvgTransformableImpl::GetCTM()
    {
    return iCTM;
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgTransformableImpl::SetCTM( TGfxAffineTransform& aTr )
    {
    iCTM = aTr;
    }

// Private

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgTransformableImpl::ConstructL()
    {
    iTransform = CSvgTransformListImpl::NewL();
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgTransformableImpl::CSvgTransformableImpl()
    {
    }

// ==========================================================================
// perform a deep clone of this object
// ==========================================================================
CSvgTransformableImpl* CSvgTransformableImpl::CloneL()
    {
    CSvgTransformableImpl* newObject = CSvgTransformableImpl::NewL();
    CleanupStack::PushL(newObject);

    // copy iTransform
    if (newObject->iTransform != NULL)
        {
        delete newObject->iTransform;
        newObject->iTransform= NULL;
        }
    newObject->iTransform = this->iTransform->CloneL();

    // copy iCTM
    newObject->iCTM = this->iCTM;
    CleanupStack::Pop();

    return newObject;
    }


void  CSvgTransformableImpl::SetTransformList(TGfxAffineTransform& aTr)
	{
	iTransform->ReplaceItem(aTr, 0);
	iTransform->SetAvoidConsolidation(EFalse);
	}

