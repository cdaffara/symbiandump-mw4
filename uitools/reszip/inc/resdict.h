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



#if !defined(__RESDICT_H__)
#define __RESDICT_H__

#include <e32base.h>

const TInt KMaxDictEntry = 80;

class CResEntry;
class CDictArray;
class TBitWriter;


class TDictEntry
	{
public:
	TDictEntry();
	void Close();
	TInt Saved();
	TDesC8& Data();
	TInt Use();
	void CreateEmbeddedDict(TInt aDictIndex,CDictArray* aDict);
public:
	TBufC8<KMaxDictEntry> iData;
	TInt iUses;
	TInt iRef;
	CResEntry* iEmbedded;
	};


class CDictArray : public CArrayFixFlat<TDictEntry>
	{
public:
	CDictArray();
	~CDictArray();
	TInt AddEntryL(TDesC8& aEntry);
	void OrderedInsertL(TDictEntry& aEntry);
	void SizedInsertL(TDictEntry& aEntry);
	TInt DictionarySize();
	TInt DictionarySizeWithoutIndex();
	TInt BitSize(TInt aIndex);
	void WriteBitStreamL(TInt aIndex, TBitWriter& aWriter);
	CDictArray* DuplicateL();
public:
	TInt iDictionaryBits;
	};


#endif
