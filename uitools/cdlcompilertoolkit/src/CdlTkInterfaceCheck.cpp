/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include "CdlCompilerToolkit/CdlTkProcess.h"
#include <algorithm>
using namespace std;

namespace CdlCompilerToolkit {

//
// CCdlTkApiListComparator
//

class CCdlTkApiListComparator
	{
public:
	CCdlTkApiListComparator(CCdlTkApiList::const_iterator& aIter) : iIter(aIter) {}
	bool operator()(const CCdlTkApi* aApi) { return *aApi == **iIter; }
	CCdlTkApiList::const_iterator& iIter;
	};


//
// CCdlTkApiChecker
//

CCdlTkApiChecker::CCdlTkApiChecker(const CCdlTkInterface& aLeft, 
								   const CCdlTkInterface& aRight, 
								   MCdlTkApiCheckObserver& aObserver)
: iLeft(aLeft), iRight(aRight), iObserver(aObserver)
	{
	}

CCdlTkApiChecker::~CCdlTkApiChecker()
	{
	}

void CCdlTkApiChecker::Process()
	{
	iObserver.StartCheck();
	
	const CCdlTkApiList& leftList = iLeft.ApiList();
	const CCdlTkApiList& rightList = iRight.ApiList();

	CApiListIter posLeft = leftList.begin();
	CApiListIter posRight = rightList.begin();
	CApiListIter leftEnd = leftList.end();
	CApiListIter rightEnd = rightList.end();
	
	while (posLeft!=leftEnd && posRight!=rightEnd)
		{
		// if left and right APIs don't match, 
		// move the iterators till they do, or they hit the end
		if (**posLeft != **posRight)
			ReSync(posLeft, posRight, leftEnd, rightEnd);

		if (posLeft == leftEnd || posRight == rightEnd)
			{
			// if either iterator hits the end, fail everything remaining
			FailLeft(posLeft, leftEnd);
			FailRight(posRight, rightEnd);
			}
		else
			{
			// the APIs match, so tell the observer and move on
			iObserver.ApiInBoth(**posLeft);
			++posLeft;
			++posRight;
			}
		}

	iObserver.CheckComplete();
	}

// This function moves the iterators posLeft and posRight onwards till they
// match APIs again, or they hit the ends.
void CCdlTkApiChecker::ReSync(CApiListIter& posLeft, CApiListIter& posRight, 
							  const CApiListIter& leftEnd, const CApiListIter& rightEnd) const
	{
	// lookaheadLeft and lookaheadRight are how far we are looking ahead for a match
	CApiListIter lookaheadLeft = posLeft;
	CApiListIter lookaheadRight = posRight;

	// predicates for finding an API in a range
	CCdlTkApiListComparator chkContainsLeft(lookaheadLeft);
	CCdlTkApiListComparator chkContainsRight(lookaheadRight);

	// keep going till we get a match or reach the end
	for (;;) 
		{
		// move lookahead iterators on one step
		++lookaheadLeft;
		++lookaheadRight;

		// stop and fail if we've hit the end on the left
		if (lookaheadLeft == leftEnd)
			{
			FailLeft(posLeft, leftEnd);
			break;
			}

		// stop and fail if we've hit the end on the right
		if (lookaheadRight == rightEnd)
			{
			FailRight(posRight, rightEnd);
			break;
			}

		// look through LHS lookahead range for the RHS API at the lookahead position
		CApiListIter foundLeft = find_if(posLeft, lookaheadLeft+1, chkContainsRight);
		if (foundLeft != lookaheadLeft+1)
			{
			// found a match, fail everything up till the match position and stop
			FailLeft(posLeft, foundLeft);
			FailRight(posRight, lookaheadRight);
			break;
			}

		// look through RHS lookahead range for the LHS API at the lookahead position
		CApiListIter foundRight = find_if(posRight, lookaheadRight+1, chkContainsLeft);
		if (foundRight != lookaheadRight+1)
			{
			// found a match, fail everything up till the match position and stop
			FailLeft(posLeft, lookaheadLeft);
			FailRight(posRight, foundRight);
			break;
			}
		}
	}

void CCdlTkApiChecker::FailLeft(CApiListIter& aFrom, const CApiListIter& aTo) const
	{
	while (aFrom != aTo)
		{
		iObserver.ApiNotInRight(**aFrom);
		++aFrom;
		}
	}

void CCdlTkApiChecker::FailRight(CApiListIter& aFrom, const CApiListIter& aTo) const
	{
	while (aFrom != aTo)
		{
		iObserver.ApiNotInLeft(**aFrom);
		++aFrom;
		}
	}

}	// end of namespace CdlCompilerToolkit
