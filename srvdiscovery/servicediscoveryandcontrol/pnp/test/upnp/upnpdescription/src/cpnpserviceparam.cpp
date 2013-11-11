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

#include <upnp/cpnpserviceparam.h>
#include "inetprottextutils.h"
#include <upnpdescriptionschema.h>
#include <e32cons.h>


CUPnPArgument* CUPnPArgument::NewL()
    {
    CUPnPArgument* self = new (ELeave) CUPnPArgument();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CUPnPArgument::CUPnPArgument()
    {
    }

void CUPnPArgument::ConstructL()
    {
	CUPnPProperty::ConstructL();
    }
//Destructor
CUPnPArgument::~CUPnPArgument()
    {
    }

 
//Perform validation

TBool CUPnPArgument::Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const
	{
	TBool result=EFalse;
	if(	(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EName, aStringTable)).Length()!=0) &&
			(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EArgumentDirection, aStringTable)).Length()!=0) &&	
			(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::ERelatedStateVar, aStringTable)).Length()!=0))
				result = ETrue;
	
	return result;
	
	}

void CUPnPArgument::SetRetVal( TBool aValue)
	{
	iRetVal = aValue;
	}

TBool CUPnPArgument::RetVal()
	{
	return iRetVal;
	}
//--------------------------------------------------------------------

EXPORT_C CUPnPValueRange* CUPnPValueRange::NewL()
	{
	CUPnPValueRange* self = new (ELeave) CUPnPValueRange();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CUPnPValueRange::CUPnPValueRange()
	{
	}

void CUPnPValueRange::ConstructL()
	{
	CUPnPProperty::ConstructL();
	}
 
//Destructor

EXPORT_C CUPnPValueRange::~CUPnPValueRange()
	{
	}


//--------------------------------------------------------------------

CUPnPArgValueList* CUPnPArgValueList::NewL()
	{
	CUPnPArgValueList* self = new (ELeave) CUPnPArgValueList();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CUPnPArgValueList::CUPnPArgValueList()
{
}

void CUPnPArgValueList::ConstructL()
	{
	CUPnPProperty::ConstructL();
	}

 
//Destructor

CUPnPArgValueList::~CUPnPArgValueList()
{
	for(TInt i =0;i< iAllowedValueList.Count();i++ )
    	{
    	iAllowedValueList[i].Close();
    	}
	iAllowedValueList.Reset();
}

//------------------------------------------------------------------------

CUPnPStateVariable* CUPnPStateVariable::NewL()
	{
	CUPnPStateVariable* self = new (ELeave) CUPnPStateVariable();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CUPnPStateVariable::CUPnPStateVariable()
	{
	}

void CUPnPStateVariable::ConstructL()
	{
	CUPnPProperty::ConstructL();
	}


/** 
Destructor
*/
CUPnPStateVariable::~CUPnPStateVariable()
	{
	delete iAllowedValue;
	}

/** 
validation
*/
TBool CUPnPStateVariable::Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const
	{
	TBool result=ETrue;
	// name cannot be of zero chars
	
	if(	(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EName, aStringTable)).Length()!=0) &&
			(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EStateVariableDataType, aStringTable)).Length()!=0) &&
			(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::ESendEvents, aStringTable)).Length()!=0)	)
				result = ETrue;

	
	return result;

	}

//----------------------------------------------------------

CUPnPAction* CUPnPAction::NewL()
	{
	CUPnPAction* self = new (ELeave) CUPnPAction();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CUPnPAction::CUPnPAction()
	{
	}

void CUPnPAction::ConstructL()
	{
	CUPnPProperty::ConstructL();
	}

/** 
Destructor
*/
CUPnPAction::~CUPnPAction()
	{
	iArgumentList.ResetAndDestroy();
	}

/** 
validation
*/
TBool CUPnPAction::Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const
	{
	TBool result=ETrue;
	// name cannot be of zero chars
	if(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EName, aStringTable)).Length()==0)
		result = EFalse;
	
	return result;

	}

//-------------------------------------------------------------

EXPORT_C CUPnPServiceDescription* CUPnPServiceDescription::NewL()
	{
	CUPnPServiceDescription* self = new (ELeave) CUPnPServiceDescription();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CUPnPServiceDescription::CUPnPServiceDescription()
	{
	}

void CUPnPServiceDescription::ConstructL()
	{
	CUPnPProperty::ConstructL();
	}
	

/** 
Destructor
*/
EXPORT_C CUPnPServiceDescription::~CUPnPServiceDescription()
	{
	iActionList.ResetAndDestroy();
	iServiceStateTable.ResetAndDestroy();
	
	}

/** 
validation
*/
TBool CUPnPServiceDescription::Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const
	{
	
	TBool result=ETrue;
	if(iServiceStateTable.Count()<=0)
		result = EFalse;
	if(result)
		{
		for(TInt i =0;i< CountOfActionList();i++ )
			{
			result = iActionList[i]->Validate(aStringPool,aStringTable );
			if(result==EFalse)
				break;
			}
		}

	if(result)
		{
		for(TInt i =0;i< CountOfServiceStateTable();i++ )
			{
			result = iServiceStateTable[i]->Validate(aStringPool, aStringTable );
			if(result==EFalse) 
				break;
			}
		}
	return result;
	}

