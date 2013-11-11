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

#ifndef __TATTRIBUTEITER_H_
#define __TATTRIBUTEITER_H_

#include "cupnpproperty.h"

class TAttributeIter
	{
public:
	TAttributeIter(const CUPnPProperty* aParamPart);
	~TAttributeIter();

	void First();
	TBool AtEnd();
	void operator++();
	const CAttribute* operator()();

private:
	void CheckInvalidation();
	
private:
	const CUPnPProperty* iParamPart;
	TInt iPosIdx;
	};

#endif /*TATTRIBUTEITER_H_*/
