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



#if !defined(__RESENTRY_H__)
#define __RESENTRY_H__

#include <e32base.h>

class CDictArray;
class CResComp;
class TBitWriter;

enum TResType
	{
	EResTypePlain = 0,
	EResTypeToken = 1,
	EResTypeZeroSpaced = 2
	};

class TResComponent
	{
public:
	TInt Size(TInt aDictionaryBits);
	void WriteBitStreamL(TBitWriter& aWriter,TInt aDictionaryBits);
public:
	TResType iType;
	TUint8* iData;
	TInt iLength;
	TInt iToken;
	};

class CResEntry : public CBase
	{
public:
	CResEntry(CDictArray* aDict);
	~CResEntry();
	void AddPlainDataL(TUint8* aData, TInt aLength);
	TInt Size(TInt aDictionaryBits);
	void MatchDictL(TInt aDictIndex = -1);
	void MatchSelfL(TInt aRes,CResComp* aResComp);
//	void MatchZeroSpacedL();
	void WriteBitStreamL(TBitWriter& aWriter,TInt aDictionaryBits);
	void CheckForLongDataStringsL();
private:
	TBool DoMatchDictL(TResComponent& aComp, TInt aOffset, TInt aDictIndex);
	TBool DoMatchSelfL(TResComponent& aComp, TInt aOffset, TInt aRes,CResComp* aResComp);
//	TBool DoMatchZeroSpacedL(TResComponent& aComp, TInt aOffset);
	void SplitPlainDataWithTokenL(TInt aComponent, TInt aToken, TInt aPosition);
	void InsertTokenL(TInt aPosition, TInt aToken);
	TBool MatchPreviousComponent(TInt aCompIndex, TDesC8& aMatch);
	void SplitPlainDataWithZeroSpacedL(TInt aComponent, TInt aZeroSpacedStart, TInt aLength);
public:
	CArrayFixFlat<TResComponent>* iComponent;
	CDictArray* iDict;
	};



#endif
