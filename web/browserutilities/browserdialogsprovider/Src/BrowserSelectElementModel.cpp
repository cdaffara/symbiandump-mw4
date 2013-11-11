/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*
*/


// INCLUDE FILES
#include "BrowserSelectElementModel.h"
#include "BrowserSelectElementListBox.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CBrowserSelectElementModel::CBrowserSelectElementModel
// ----------------------------------------------------------------------------
//
CBrowserSelectElementModel::CBrowserSelectElementModel( 
							const CArrayFix<TBrCtlSelectOptionData>& aOptions )
                            :   iOptions ( aOptions )           
	{
	}

// ----------------------------------------------------------------------------
// CBrowserSelectElementModel::ConstructL
// ----------------------------------------------------------------------------
//
void CBrowserSelectElementModel::ConstructL()
	{
    iFilterTexts = new ( ELeave ) TBrCtlSelectElementFilterTextArray(iOptions);
    iItemTexts = new ( ELeave ) TBrCtlSelectElementItemTextArray( iOptions );
    }

// ----------------------------------------------------------------------------
// CBrowserSelectElementModel::NewLC
// ----------------------------------------------------------------------------
//
CBrowserSelectElementModel* CBrowserSelectElementModel::NewLC( 
							const CArrayFix<TBrCtlSelectOptionData>& aOptions )
	{
	CBrowserSelectElementModel* result =
					new ( ELeave ) CBrowserSelectElementModel( aOptions );
	CleanupStack::PushL( result );
	result->ConstructL();
	return result;
	}

// ----------------------------------------------------------------------------
// CBrowserSelectElementModel::NewL
// ----------------------------------------------------------------------------
//
CBrowserSelectElementModel* CBrowserSelectElementModel::NewL(
							const CArrayFix<TBrCtlSelectOptionData>& aOptions )
	{
	CBrowserSelectElementModel* result = NewLC( aOptions );
	CleanupStack::Pop();
	return result;
	}

// ----------------------------------------------------------------------------
// CBrowserSelectElementModel::~CBrowserSelectElementModel
// ----------------------------------------------------------------------------
//
CBrowserSelectElementModel::~CBrowserSelectElementModel()
	{
    delete iFilterTexts;
    delete iItemTexts;
	}


// ----------------------------------------------------------------------------
// CBrowserSelectElementModel::SetItemArray
// ----------------------------------------------------------------------------
//
void CBrowserSelectElementModel::SetItemArray()
    {
    // Need to set them after the listbox has been constructed; otherwise
    // listbox construction will reset the item text array to NULL.
    SetItemTextArray( iItemTexts );
    SetOwnershipType( ELbmDoesNotOwnItemArray ); 
    }

// ----------------------------------------------------------------------------
// CBrowserSelectElementModel::MatchableTextArray() const
// ----------------------------------------------------------------------------
//
const MDesCArray* CBrowserSelectElementModel::MatchableTextArray() const

	{
	return iFilterTexts;
	}

// ----------------------------------------------------------------------------
// CBrowserSelectElementModel::MdcaCount
// ----------------------------------------------------------------------------
//
TInt TBrCtlSelectElementItemTextArray::MdcaCount() const
	{
    return iOptions.Count();
	}


//  End of File
