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



#if !defined(__RESCOMP_H__)
#define __RESCOMP_H__

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>

class CDictArray;
class CResEntry;

class TBitWriter
	{
public:
	TBitWriter(RWriteStream& aStream);
	void WriteL(TInt aValue, TInt aBits);
	void FlushL();
private:
	RWriteStream& iStream;
	TInt iValue;
	TInt iBitsWritten;
	};

class CResComp : public CBase
	{
public:
	CResComp(CConsoleBase* aConsole);
	~CResComp();
	void LoadFileL(RFs& aFs, const TDesC& aFileName);
	void CompressL();
	void WriteFileL(RFs& aFs, const TDesC& aName);
	TPtrC8 Resource(TInt aRes);
	TInt FindInResources(TDesC8& aBuf, TInt aMax);
	TInt OriginalFileSize();
	TInt CompressedSize();
	void DisplayStats();
	TInt ResourceSize();
	TBool IsValid();
private:
	void LoadBAFLCompressedFileL(RFs& aFs, const TDesC& aName);
	void LoadUnCompressedFileL(RFs& aFs, const TDesC& aName);
	TBool FileIsBAFLCompressedL(RFs& aFs, const TDesC& aName);

	void OptimizeDict();
	void FindEmbeddedDict(TDesC8& aMatch, TInt aStart);
	void WriteHeaderL(RWriteStream& aStream);
	void WriteDictionaryL(RWriteStream& aStream);
	void WriteDictionaryHeaderL(RWriteStream& aStream);
	void WriteDictionaryIndexL(RWriteStream& aStream);
	void WriteDictionaryDataL(RWriteStream& aStream);
	void WriteResourceL(RWriteStream& aStream);
	void WriteResourceIndexL(RWriteStream& aStream);
	void WriteResourceDataL(RWriteStream& aStream);
	TInt CompressResourcesL(TInt aMaxDictEntries);
	TInt AttemptCompressionL(TInt aMaxDictEntries);
	void CheckForLongDataStringsL();
private:
	CConsoleBase* iConsole;
	TUint8* iResData;
	CDictArray* iDict;
	TUint16* iResIndex;
	CArrayPtrFlat<CResEntry>* iResArray;
	TInt iResources;
	TInt iFileLength;
	TInt iCompressedSize;
	TInt iLargestResourceSize;
	TInt iMaxEntries;
	TInt iBitsForDict;
	TInt iBestCompression;
	TInt iBestSize;
	TInt iOriginalCompressedSize;
public:
	TBuf<80> iConOutput;
	};


#endif
