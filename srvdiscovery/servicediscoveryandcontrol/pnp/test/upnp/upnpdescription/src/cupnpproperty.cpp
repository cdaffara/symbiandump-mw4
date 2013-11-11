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

#include <cupnpproperty.h>
#include "cattribute.h"

EXPORT_C void CUPnPProperty::SetPropertyL(const RString aAttributeName, const TDesC8& aAttributeVal)
	{
	for ( TInt i =0;i<iAttributeArray.Count();i++)
		{
		if ( aAttributeName == iAttributeArray[i]->AttributeName())
			{
			iAttributeArray[i]->SetValueL( aAttributeVal );
			return;
			}
		}
	CAttribute* attrib = CAttribute::NewL( aAttributeName , aAttributeVal );
	iAttributeArray.AppendL( attrib );
	}

EXPORT_C void CUPnPProperty::SetPropertyL(const RString aAttributeName, TInt aAttributeVal)
	{
	RBuf8 buffer;
	buffer.CreateL ( sizeof ( TInt ) );
	CleanupClosePushL ( buffer );
	buffer.AppendNum ( aAttributeVal, EDecimal );
	SetPropertyL ( aAttributeName, buffer );
	CleanupStack::PopAndDestroy ( &buffer );
	}


void CUPnPProperty::SetPropertyStatus(const RString aAttributeName, TBool aStatus)
	{
	for ( TInt i =0;i<iAttributeArray.Count();i++)
		{
		if ( aAttributeName == iAttributeArray[i]->AttributeName())
			{
			iAttributeArray[i]->SetStatus( aStatus );
			return;
			}
		}
	}

EXPORT_C const TDesC8& CUPnPProperty::Property(const RString aAttributeName) const
	{
	for ( TInt i =0;i<iAttributeArray.Count();i++)
		{
		if ( aAttributeName == iAttributeArray[i]->AttributeName())
			{
			return (iAttributeArray[i]->AttributeValue());
			}
		}
	return KNullDesC8;
	}
EXPORT_C CUPnPProperty::~CUPnPProperty()
	{
	iAttributeArray.ResetAndDestroy();
	}
