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

#include "tattributeiter.h"

TAttributeIter::TAttributeIter(const CUPnPProperty* aParamPart)
	: iParamPart(aParamPart)
	{
	First();
	}

TAttributeIter::~TAttributeIter()
	{
	}

void TAttributeIter::First()
	{
	iPosIdx = 0;
	CheckInvalidation();
	}

TBool TAttributeIter::AtEnd()
	{
	return (iPosIdx == KErrNotFound);
	}

void TAttributeIter::operator++()
	{
	__ASSERT_DEBUG ( iPosIdx != KErrNotFound, User::Invariant() );

	++iPosIdx;
	CheckInvalidation();
	}

const CAttribute* TAttributeIter::operator()()
	{
	CheckInvalidation();
	__ASSERT_DEBUG ( iPosIdx != KErrNotFound, User::Invariant() );
	
	return iParamPart->iAttributeArray.operator[](iPosIdx);
	}

void TAttributeIter::CheckInvalidation()
	{
	if (iPosIdx >= iParamPart->iAttributeArray.Count())
		iPosIdx = KErrNotFound; // Hit the end
	}

