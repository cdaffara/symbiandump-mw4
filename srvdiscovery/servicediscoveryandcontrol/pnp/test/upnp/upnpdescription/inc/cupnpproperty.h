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

#ifndef __CUPNPPROPERTY_H_
#define __CUPNPPROPERTY_H_
#include <e32base.h>
#include <stringpool.h>

/** Provides a mapping for the tags of an xml element and the corresponding content
 @publishedPartner
 @prototype
 */ 
class CAttribute;
class CUPnPProperty: public CBase
{
protected:
	inline CUPnPProperty();
	inline void ConstructL();

public:
	IMPORT_C void SetPropertyL(const RString aAttributeName, const TDesC8& aAttributeVal);
	IMPORT_C void SetPropertyL(const RString aAttributeName, TInt aAttributeVal);
	void SetPropertyStatus(const RString aAttributeName, TBool aStatus);
	IMPORT_C const TDesC8& Property(const RString aAttributeName) const;
	IMPORT_C virtual ~CUPnPProperty();
private:
	RPointerArray<CAttribute> iAttributeArray;
	friend class TAttributeIter;
};


void CUPnPProperty::ConstructL()
	{
	}

CUPnPProperty::CUPnPProperty()
	{
	}

#endif /*CUPNPPROPERTY_H_*/