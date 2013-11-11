// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "cattribute.h"

CAttribute* CAttribute::NewL(const RString aAttributeName, const TDesC8& aAttributeVal)
	{
	return new(ELeave)CAttribute(aAttributeName, aAttributeVal);
	}

CAttribute::~CAttribute()
	{
	iAttributeName.Close();
	iAttributeValue.Close();
	}

const RString& CAttribute::AttributeName() const
	{
	return iAttributeName;
	}

const TDesC8& CAttribute::AttributeValue() const
	{
	return iAttributeValue;
	}

void CAttribute::SetValueL(const TDesC8& aAttributeVal)
	{
	if(iParsedStatus)
		{
		iAttributeValue.Close();
		iAttributeValue.Create(aAttributeVal.Length());
		iAttributeValue.Copy(aAttributeVal);
		iParsedStatus = EFalse;
		}
	else
		{
		TInt len = iAttributeValue.Length();
		iAttributeValue.ReAllocL( len + aAttributeVal.Length() );
		iAttributeValue.Append ( aAttributeVal );
		}
	}

void CAttribute::SetStatus(TBool aStatus)
	{
	iParsedStatus = aStatus;
	}

CAttribute::CAttribute(RString aAttributeName, const TDesC8& aAttributeVal)
	{
	iAttributeName = aAttributeName.Copy();
	iAttributeValue.Create(aAttributeVal.Length());
	iAttributeValue.Copy(aAttributeVal);
	}

