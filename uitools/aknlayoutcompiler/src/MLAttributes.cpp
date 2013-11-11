/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include "MLAttributes.h"
#include "MLAttributesParse.h"

#include "LayoutCompilerErr.h"
#include "CodeGenConsts.h"
#include "UsefulDefinitions.h"

//
//  class TMLAttributeSet
//
TMLAttributeSet::TMLAttributeSet()
	{

	}

TMLAttributeSet::TMLAttributeSet(TMLAttributes* aAttributes)
	:
	iAttributes(aAttributes)
	{
	
	}

TMLAttributeSet::~TMLAttributeSet()
	{

	}

void TMLAttributeSet::Merge(TMLAttributeSet& aOther)
	{

	}

void TMLAttributeSet::Compile()
	{

	}

//
//  class TMLAttributes
//

TMLAttributes::TMLAttributes()
	{

	}

TMLAttributes::~TMLAttributes()
	{
	for(iterator pComponent = begin(); pComponent != end(); ++pComponent)
		{
		TMLAttributeSetComponent& component = pComponent->second;
		for(TMLAttributeSetComponent::iterator pAttributeSet = component.begin(); pAttributeSet != component.end(); ++pAttributeSet)
			{
			TMLAttributeSet* nextSet = pAttributeSet->second;
			delete nextSet;
			}
		}
	}

void TMLAttributes::Merge(TMLAttributes& aOther)
	{
	for(iterator pOtherComponent = aOther.begin(); pOtherComponent != aOther.end(); ++pOtherComponent)
		{
		int compId = pOtherComponent->first;
		TMLAttributeSetComponent& otherComponent = pOtherComponent->second;
		TMLAttributeSetComponent& thisComponent = (*this)[compId];
		for(TMLAttributeSetComponent::iterator pOtherAttributeSet = otherComponent.begin(); pOtherAttributeSet != otherComponent.end(); ++pOtherAttributeSet)
			{
			string name = pOtherAttributeSet->first;
			TMLAttributeSet* otherSet = pOtherAttributeSet->second;
			TMLAttributeSetComponent::iterator foundThisSet = thisComponent.find(name);
			if(foundThisSet == thisComponent.end() && otherSet)
				{
				TMLAttributeSet* otherSetCopy = new TMLAttributeSet(*otherSet);
				thisComponent.insert(make_pair(name, otherSetCopy));
				}
			}
		}
	for(TMLAttributeNames::iterator nextOtherName = aOther.iNames.begin(); nextOtherName != aOther.iNames.end(); ++nextOtherName)
		{
		// we want the superset of all the names, 
		// it doesn't matter if there are names that correspond to attributes that don't actually exist
		// as it won't take up much memory and we won't look for them
		int& nextThisNameValue = iNames[nextOtherName->first];
		nextThisNameValue = nextOtherName->second;
		}
	}

void TMLAttributes::Compile()
	{

	}


// End of File
