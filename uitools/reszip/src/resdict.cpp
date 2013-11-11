/*
* Copyright (c) 1997-1999 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32svr.h>

#include "resdict.h"
#include "resentry.h"

// TDictEntry

TDictEntry::TDictEntry()
	{
	iUses = 0;
	iRef = 0;
	iEmbedded = 0;
	}

void TDictEntry::Close()
	{
	delete iEmbedded;
	iEmbedded = NULL;
	}



TDesC8& TDictEntry::Data()
	{
	return iData;
	}

TInt TDictEntry::Use()
	{
	return ((iUses + iRef - 1) * (Data().Length()-1));
	}


void TDictEntry::CreateEmbeddedDict(TInt aDictIndex,CDictArray* aDict)
	{
	// Create list of embedded dictionary strings
	iEmbedded = new(ELeave)CResEntry(aDict);
	iEmbedded->AddPlainDataL((TUint8*)iData.Ptr(), iData.Length());
	iEmbedded->MatchDictL(aDictIndex);
	}


// CDictArray

CDictArray::CDictArray()
:CArrayFixFlat<TDictEntry>(10)
	{
	}

CDictArray::~CDictArray()
	{
	TInt count = Count();
	for (TInt ii=0; ii<count; ii++)
		{
		At(ii).Close();
		}
	}


TInt CDictArray::AddEntryL(TDesC8& aEntry)
	{
	TInt size = aEntry.Size();
	TInt pos = 0;
	TInt count = Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TInt dSize = At(ii).Data().Size();
		if (dSize < size)
			{
			break;
			}
		else
			{
			if (At(ii).Data().Compare(aEntry) == 0)
				{
				// Already in dict
				return ii;
				}
			pos++;
			}
		}
	TDictEntry entry;
	entry.iData = aEntry;
	entry.iUses=0;
	InsertL(pos, entry);
	return pos;
	}


void CDictArray::OrderedInsertL(TDictEntry& aEntry)
	{
	TInt use = aEntry.Use();
	if (use < 1)
		return;	 // Don't bother to insert entries that aren't used

	TInt count = Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TInt entryUse = At(ii).Use();
		if (entryUse < use)
			{
			InsertL(ii, aEntry);
			return;
			}
		}
	AppendL(aEntry);
	}

void CDictArray::SizedInsertL(TDictEntry& aEntry)
	{
	TInt len = aEntry.Data().Length();

	TInt count = Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TInt entryLen = At(ii).Data().Length();
		if (entryLen < len)
			{
			InsertL(ii, aEntry);
			return;
			}
		}
	AppendL(aEntry);
	}

TInt CDictArray::DictionarySize()
	{
	TInt count = Count();
	TInt size = 0;
	for (TInt jj=0; jj<count; jj++)
		{
		size += At(jj).iEmbedded->Size(iDictionaryBits);
		}

	size = (size + 7) / 8;

	// Add dictionary index
	size += (count * 2);

	// Add dictionary header

	size += 2;

	return size;
	}

TInt CDictArray::DictionarySizeWithoutIndex()
	{
	TInt count = Count();
	TInt size = 0;
	for (TInt jj=0; jj<count; jj++)
		{
		size += At(jj).iEmbedded->Size(iDictionaryBits);
		}
	size = (size + 7) / 8;
	return size;
	}



TInt CDictArray::BitSize(TInt aIndex)
	{
	return At(aIndex).iEmbedded->Size(iDictionaryBits);
	}


void CDictArray::WriteBitStreamL(TInt aIndex, TBitWriter& aWriter)
	{
	RDebug::Print(_L("Dictionary Entry: %d"), aIndex);
	At(aIndex).iEmbedded->WriteBitStreamL(aWriter,iDictionaryBits);
	}



CDictArray* CDictArray::DuplicateL()
	{
	CDictArray* newDict = new(ELeave)CDictArray();
	CleanupStack::PushL(newDict);
	TInt count = Count();
	for (TInt ii=0; ii<count; ii++)
		{
		newDict->AppendL(At(ii));
		}
	CleanupStack::Pop();	// newDict;
	return newDict;
	}

