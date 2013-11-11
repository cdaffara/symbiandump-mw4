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

void CUPnPArgValueList::AppendL(const TDesC8& aValue)
	{
	RBuf8 tmp;
	tmp.CreateL(aValue.Length());
	tmp.Copy(aValue);

	iAllowedValueList.AppendL(tmp);
	}

TInt CUPnPArgValueList::Count () const
	{
	return iAllowedValueList.Count();
	}

const TDesC8& CUPnPArgValueList::At(const TInt aIndex) const
	{
	return iAllowedValueList[aIndex];
	}

//------------------------------------------------------------------------

void CUPnPStateVariable::SetAllowedValuesL( CUPnPAllowedValue *aValue)
	{
	iAllowedValue = aValue;
	}

const CUPnPAllowedValue* CUPnPStateVariable::AllowedValues () const
	{
	return iAllowedValue;
	}

void CUPnPStateVariable::SetAllowedValueType ( TBool aStatus ) 
	{
	isValue = aStatus;
	}

TBool CUPnPStateVariable::AllowedValueType () const
	{
	return isValue;
	}
//----------------------------------------------------------

void CUPnPAction::AppendL(const CUPnPArgument* aValue)
	{
	iArgumentList.AppendL(aValue);
	}

TInt CUPnPAction::Count () const
	{
	return iArgumentList.Count();
	}

CUPnPArgument* CUPnPAction::At( TInt aIndex) const
	{
	return iArgumentList[aIndex];
	}

//-------------------------------------------------------------



void CUPnPServiceDescription::AppendToActionList(const CUPnPAction* aValue)
	{
	iActionList.AppendL(aValue);
	}

TInt CUPnPServiceDescription::CountOfActionList () const
	{
	return iActionList.Count();
	}

const CUPnPAction* CUPnPServiceDescription::AtActionList( TInt aIndex) const
	{
	return iActionList[aIndex];
	}

void CUPnPServiceDescription::AppendToServiceStateTable(const CUPnPStateVariable* aValue)
	{
	iServiceStateTable.AppendL(aValue);
	}

TInt CUPnPServiceDescription::CountOfServiceStateTable () const
	{
	return iServiceStateTable.Count();
	}

const CUPnPStateVariable* CUPnPServiceDescription::AtServiceStateTable(const TInt aIndex) const 
	{
	return iServiceStateTable[aIndex];
	}

void CUPnPServiceDescription::SetMinorNumberL(const TInt aValue)
	{
	iVersion.iMinor = aValue;
	}

const TInt CUPnPServiceDescription::MinorNumber () const
	{
	return iVersion.iMinor;
	}

void CUPnPServiceDescription::SetMajorNumberL(const TInt aValue)
	{
	iVersion.iMajor = aValue;
	}

const TInt CUPnPServiceDescription::MajorNumber () const
	{
	return iVersion.iMajor;
	}


