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


#ifdef _DEBUG
#if !defined(__E32DEBUG_H__)
#include <e32debug.h>
#endif
#endif
#include "SVGStrCssValueImpl.h"



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CStrCssValueImpl::~CStrCssValueImpl()
    {
     if ( iCssText )
        {
        delete iCssText;
        iCssText = NULL;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt  CStrCssValueImpl::SetValueL( const TDesC& aValue )
    {
     if ( iCssText )
        {
        delete iCssText;
        iCssText = NULL;
        }

    iCssText = aValue.AllocL();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CStrCssValueImpl::Value()
    {
    return *iCssText;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CStrCssValueImpl::SetL(CStrCssValueImpl* aValue)
    {
    
    SetValueL(aValue->Value());
	
    }

// *******************************************************
// Private



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CStrCssValueImpl::CStrCssValueImpl( )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CStrCssValueImpl::IsEqual( CCssValue* aValue )
{
	if (((CStrCssValueImpl*)aValue)->iCssText == iCssText)
	{
		return ETrue;
	}

	return EFalse;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CStrCssValueImpl::Print()
{
	#ifdef _DEBUG
	if (iCssText && iCssText->Length() > 0)
	{
		HBufC8* buf = NULL;
		buf = HBufC8::New( Value().Length() + 1);
		buf->Des().Copy( Value() );
		buf->Des().ZeroTerminate();
		RDebug::Printf("%s", buf->Ptr());
		delete buf;
	}
	else
	{
		RDebug::Printf("NULL");
	}
	#endif
}
