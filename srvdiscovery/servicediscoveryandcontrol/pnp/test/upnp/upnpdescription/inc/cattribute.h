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

#ifndef __CATTRIBUTE_H_
#define __CATTRIBUTE_H_

#include <e32base.h>
#include <stringpool.h>

class CAttribute: public CBase
	{
public:
	static CAttribute* NewL(const RString aAttributeName, const TDesC8& aAttributeVal);
	~CAttribute();
	
	const RString& AttributeName() const;
	const TDesC8& AttributeValue() const;
	void SetValueL(const TDesC8& aAttributeVal);
	void SetStatus(TBool aStatus);
	
private:
	CAttribute(RString aAttributeName, const TDesC8& aAttributeVal);

private:
	RString 	iAttributeName;
	RBuf8		iAttributeValue;
	TBool		iParsedStatus;
	};

#endif /*CATTRIBUTE_H_*/
