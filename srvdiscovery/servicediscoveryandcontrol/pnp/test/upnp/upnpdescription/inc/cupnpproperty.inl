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


void CUPnPProperty::SetPropertyL(const RString aAttributeName, const TDesC8& aAttributeVal)
	{
	iPropertySet.SetPropertyL(aAttributeName,aAttributeVal);
	}
const TDesC8& CUPnPProperty::Property(const RString aAttributeName) const
	{
	return iPropertySet.Property(aAttributeName);
	}

void CUPnPProperty::ConstructL()
	{
	iPropertySet.OpenL();
	}

CUPnPProperty::CUPnPProperty()
	{
	}
CUPnPProperty::~CUPnPProperty()
	{
	iPropertySet.Close();
	}