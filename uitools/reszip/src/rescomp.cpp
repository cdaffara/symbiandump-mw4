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



#include "rescomp.h"
#include <e32cons.h>
#include <s32file.h>
#include <s32mem.h>

#include <barsc.h>

#include "resdict.h"
#include "resentry.h"

//#define __DEBUG_WRITE_UNCOMPRESSED_BAFL_RESOURCE

// TBit Writer

TBitWriter::TBitWriter(RWriteStream& aStream)
:iStream(aStream)
	{
	iValue = 0;
	iBitsWritten = 0;
	}

void TBitWriter::WriteL(TInt aValue, TInt aBits)
	{
	// 1<< aBits is the max Value;

	__ASSERT_DEBUG((1<<aBits) > aValue, User::Panic(_L("TBitWriter"),KErrOverflow));
	if (aBits + iBitsWritten < 8)
		{
		// Not enough data to write a byte
		iValue += (aValue << iBitsWritten);
		iBitsWritten += aBits;
		}
	else
		{
		TInt bitsLeft = 8-iBitsWritten;
		TInt mask = (1<<bitsLeft)-1;
		TInt maskValue = (aValue & mask) << iBitsWritten;

		iValue += maskValue;
		iStream.WriteUint8L(iValue);
//		RDebug::Print(_L("Writing: %b [0x%x]"), iValue, iValue);
		
		TInt newValue = aValue >> bitsLeft;
		TInt bits = aBits - bitsLeft;
		while (bits > 7)
			{
			iStream.WriteUint8L(newValue & 0xff);
			bits -= 8;
			newValue = (newValue >> 8);
			}
		iValue = newValue;
		iBitsWritten=bits;
		}
	}

void TBitWriter::FlushL()
	{
	if (iBitsWritten > 0)
		{
		iStream.WriteUint8L(iValue);
		iValue = 0;
		iBitsWritten=0;
		}
	}




CResComp::CResComp(CConsoleBase* aConsole)
:iConsole(aConsole)
	{
	}

CResComp::~CResComp()
	{
	if (iResArray)
		{
		iResArray->ResetAndDestroy();
		}
	delete iResArray;
	delete iResData;
	delete iDict;
	}


void CResComp::LoadBAFLCompressedFileL(RFs& aFs, const TDesC& aName)
	{

	RResourceFile res;
	res.OpenL(aFs, aName);
	CleanupClosePushL(res);

	// Count resources
	TInt count = 0;

// This code around ConfirmSigniture had to be added because some S60 resources don't seem to have signature and panic.
// See	DEF019933 - RResourceFile::ConfirmSignatureL() needs to be more robust  

	HBufC8* const resource=res.AllocReadLC(1);
	if(resource->Length()==8) // 8 =Sizeof SSigRecord
		{
		res.ConfirmSignatureL(0);
		}
	CleanupStack::PopAndDestroy(resource);

	TInt offset = res.Offset();
	while (res.OwnsResourceId(count+offset+1))
		{
		count++;
		};

	const TInt KBufferGranularity = 1000;
	CBufFlat* buffer = CBufFlat::NewL(KBufferGranularity);
	CleanupStack::PushL(buffer);

	CBufFlat* indexbuffer = CBufFlat::NewL(KBufferGranularity);
	CleanupStack::PushL(indexbuffer);

	RBufWriteStream stream;
	stream.Open(*buffer);
	CleanupClosePushL(stream);

	RBufWriteStream indexstream;
	indexstream.Open(*indexbuffer);
	CleanupClosePushL(indexstream);

	TInt total = 0;

	for (TInt ii=0; ii<count; ii++)
		{
		indexstream.WriteUint16L(total+4);
		HBufC8* data = res.AllocReadLC(ii+offset+1);
		TInt len = data->Length();
		total += len;
		stream.WriteL(*data, len);
		CleanupStack::PopAndDestroy();
		}
	indexstream.WriteUint16L(total+4);


	indexstream.CommitL();
	CleanupStack::PopAndDestroy();	// close indexstream

	stream.CommitL();
	CleanupStack::PopAndDestroy();	// close stream

	CBufFlat* outputBuffer = CBufFlat::NewL(KBufferGranularity);
	CleanupStack::PushL(outputBuffer);

	RBufWriteStream outStream;
	outStream.Open(*outputBuffer);
	CleanupClosePushL(outStream);

	
	outStream.WriteUint16L(total + 4);	// 4 byte header
	outStream.WriteUint16L(((count+1)*2)+1);

	TPtr8 ptr = buffer->Ptr(0);
	outStream.WriteL(ptr.Ptr(), ptr.Length());
	TPtr8 indexptr = indexbuffer->Ptr(0);
	outStream.WriteL(indexptr.Ptr(), indexptr.Length());
	outStream.CommitL();
	CleanupStack::PopAndDestroy();// outStream


/* DEBUG - OUTPUT UNCOMPRESSED REPRESENTATION TO A FILE - FOR VERIFICATION */
#ifdef __DEBUG_WRITE_UNCOMPRESSED_BAFL_RESOURCE
	{
	TFileName newName(aName);
	newName.Append(_L(".ER5u"));

	RFileWriteStream outStream;
	User::LeaveIfError(outStream.Replace(aFs, newName, EFileWrite));
	CleanupClosePushL(outStream);

	
	outStream.WriteUint16L(total + 4);	// 4 byte header
	outStream.WriteUint16L(((count+1)*2)+1);

	TPtr8 ptr = buffer->Ptr(0);
	outStream.WriteL(ptr.Ptr(), ptr.Length());
	TPtr8 indexptr = indexbuffer->Ptr(0);
	outStream.WriteL(indexptr.Ptr(), indexptr.Length());
	outStream.CommitL();
	CleanupStack::PopAndDestroy(); // outStream
	}
#endif /* END __DEBUG_WRITE_UNCOMPRESSED_BAFL_RESOURCE */

	iDict = new(ELeave)CDictArray();

	iFileLength = outputBuffer->Size();

	iResData = (TUint8*)User::AllocL(iFileLength);

	Mem::Copy(iResData, const_cast<TUint8*>(outputBuffer->Ptr(0).Ptr()), iFileLength );

	CleanupStack::PopAndDestroy(outputBuffer);

	CleanupStack::PopAndDestroy(indexbuffer);
	CleanupStack::PopAndDestroy(buffer);

	CleanupStack::PopAndDestroy(); // res

	// Need to keep original size in order to compare with new compression for IsValid()
	TEntry entry;
	if(	aFs.Entry(aName, entry) ==KErrNone)
		{
		iOriginalCompressedSize = entry.iSize;
		}

	}

TBool CResComp::FileIsBAFLCompressedL(RFs& aFs, const TDesC& aFileName)
	{
	const TUid KBAFLCompressedFileUid = {0x101f4a6b};	// rcomp unicode compressed

	TEntry entry;
	User::LeaveIfError(aFs.Entry(aFileName,entry));
	
	return  entry.IsUidPresent(KBAFLCompressedFileUid);
	}

void CResComp::LoadFileL(RFs& aFs, const TDesC& aFileName)
	{
	ASSERT(iDict==NULL);

	TParsePtrC parse(aFileName);
	iConOutput.Format(parse.NameAndExt());
	iConOutput.Append(_L("\t"));
	if (parse.NameAndExt().Length() < 16)
		{
		iConOutput.Append(_L("\t"));
		}

	if(FileIsBAFLCompressedL(aFs, aFileName))
		{
		LoadBAFLCompressedFileL(aFs, aFileName);
		}
	else
		{
		LoadUnCompressedFileL(aFs, aFileName);
		}
	
	}

void CResComp::LoadUnCompressedFileL(RFs& aFs, const TDesC& aFileName)
	{

	iDict = new(ELeave)CDictArray();

	TEntry entry;
	User::LeaveIfError( aFs.Entry(aFileName, entry) );
	iFileLength = entry.iSize;

	iResData = (TUint8*)User::AllocL(entry.iSize + 2); // +2 for file size

	RFileReadStream stream;
	User::LeaveIfError( stream.Open(aFs, aFileName, EFileRead) );
	CleanupClosePushL(stream);
	stream.ReadL(iResData, entry.iSize);
	CleanupStack::PopAndDestroy();	 // stream
	}


void CResComp::CompressL()
	{
	if (iFileLength == 0)
		{
		iCompressedSize = iFileLength;
		return;
		}

	TUint16* resData16 = (TUint16*)iResData;
	TInt indexOffset = resData16[0];
	TInt indexLength = resData16[1];

	if (indexOffset == 4)
		{
		// File is already compressed
		iCompressedSize = iFileLength;
		return;
		}

	if (indexOffset > iFileLength)
		{
		iCompressedSize = iFileLength;
		return;
		}

	iResources = (indexLength / 2) - 1;

//	iConsole->Printf(_L("Resources: %d\n"), iResources);

//	iConOutput.AppendFormat(_L("%d\t\t"), iResources);


	iResIndex = (TUint16*)(iResData + indexOffset);


	iResArray = new(ELeave)CArrayPtrFlat<CResEntry>(10);

	iResIndex[iResources] = (TUint16)(iFileLength-((iResources+1) * 2)); // end of file - index

	// Build dictionary

//	iConsole->Printf(_L("Building Dictionary\n"));

	for (TInt ii=0; ii<iResources; ii++)
		{
		TBuf<80> lineOut(_L("\r"));
		lineOut.Append(iConOutput);
		lineOut.AppendFormat(_L("%3d / %3d"), ii+1, iResources);
		iConsole->Printf(lineOut);
//		iConsole->Printf(_L("\r%d / %d"), ii+1, iResources);
		CResEntry* rEntry = new(ELeave)CResEntry(iDict);
		CleanupStack::PushL(rEntry);

		TInt resSize = iResIndex[ii+1] - iResIndex[ii];
		if (resSize > iLargestResourceSize)
			iLargestResourceSize = resSize;

		rEntry->AddPlainDataL(iResData+iResIndex[ii], iResIndex[ii+1] - iResIndex[ii]);
		iResArray->AppendL(rEntry);
		CleanupStack::Pop();	//rEntry


		rEntry->MatchSelfL(ii, this);
		}

	// Compress using dictionary


//	iConsole->Printf(_L("\nCompressing\n"));

	CDictArray* backupDict = iDict->DuplicateL();
	CleanupStack::PushL(backupDict);

	AttemptCompressionL(32);
	iDict = backupDict->DuplicateL();
	AttemptCompressionL(64);
	iDict = backupDict->DuplicateL();
	AttemptCompressionL(128);
	iDict = backupDict->DuplicateL();
	AttemptCompressionL(256);
	iDict = backupDict->DuplicateL();
	AttemptCompressionL(512);
	iDict = backupDict->DuplicateL();


	CompressResourcesL(iBestCompression);


	CleanupStack::PopAndDestroy();	// backupDict
	}

TInt CResComp::AttemptCompressionL(TInt aDictEntries)
	{
	TInt size = CompressResourcesL(aDictEntries);
//	RDebug::Print(_L("Entries: %d  Size:%d"), aDictEntries, size);

	TBool valid = IsValid();
	if (!valid)
		{
		RDebug::Print(_L("Compression Not Valid"));
		}

	if ((size < iBestSize || iBestSize == 0) && valid)
		{
		iBestCompression = aDictEntries;
		iBestSize = size;
		}
	delete iDict;
    iDict = NULL;
	return size;
	}


TInt CResComp::CompressResourcesL(TInt aDictEntries)
	{
	iMaxEntries = aDictEntries;
	iBitsForDict = 0;
	while ( (1<<iBitsForDict) < aDictEntries)
		{
		iBitsForDict++;
		}
	iDict->iDictionaryBits = iBitsForDict;
//	RDebug::Print(_L("Entries: %d  Bits: %d"), aDictEntries, iBitsForDict);
	iResArray->ResetAndDestroy();
	// Reset dictionary count

	TInt dCount = iDict->Count();
	for (TInt ll=0; ll<dCount; ll++)
		{
		iDict->At(ll).iUses = 0;
		}

	for (TInt kk=0; kk<iResources; kk++)
		{
		CResEntry* rEntry = new(ELeave)CResEntry(iDict);
		CleanupStack::PushL(rEntry);
		rEntry->AddPlainDataL(iResData+iResIndex[kk], iResIndex[kk+1] - iResIndex[kk]);
		rEntry->MatchDictL();
		iResArray->AppendL(rEntry);
		CleanupStack::Pop();	// rEntry
	}


	// Optimize Dictionary

	OptimizeDict();

	// Compress using optimized dictionary

	dCount = iDict->Count();
	for (TInt x=0; x<dCount; x++)
		{
		iDict->At(x).iUses = 0;
		}

	iResArray->ResetAndDestroy();
	for (TInt xx=0; xx<iResources; xx++)
		{
		CResEntry* rEntry = new(ELeave)CResEntry(iDict);
		CleanupStack::PushL(rEntry);
		rEntry->AddPlainDataL(iResData+iResIndex[xx], iResIndex[xx+1] - iResIndex[xx]);
		rEntry->MatchDictL();
		iResArray->AppendL(rEntry);
		CleanupStack::Pop();	// rEntry
		}


	// Make sure that there is no more than 256 data bytes in each resource component
	CheckForLongDataStringsL();
	
	TInt size = ResourceSize();

	TInt count = iResArray->Count();

	size += 4;			// res header
	size += 7;			// dict header
	size += (count + 1) * 2;	// res index

	TInt dSize = iDict->DictionarySize();

	iCompressedSize = size+dSize;
	return size+dSize;
	}

TInt CResComp::ResourceSize()
	{
	TInt resSize = 0;
	if (!iResArray)
		return -1;
	TInt count = iResArray->Count();
	for (TInt jj=0; jj<count; jj++)
		{
		resSize += ((iResArray->At(jj))->Size(iBitsForDict));
		}
	resSize = (resSize + 7) / 8;
	return resSize;
	}




TBool CResComp::IsValid()
	{
	TInt resSize = ResourceSize();
	TInt dSize = iDict->DictionarySizeWithoutIndex();

	RDebug::Print(_L("Ressize = %D, dsize=%D, compressedsize=%D , fileSize=%D\n"), resSize, dSize, CompressedSize(), OriginalFileSize());

	if (resSize < 8192 && dSize < 8192 && (CompressedSize() < OriginalFileSize()) && resSize > 0)
		{
		return ETrue;
		}
	return EFalse;
	}








void CResComp::WriteFileL(RFs& aFs, const TDesC& aName)
	{
	CBufFlat* buffer = CBufFlat::NewL(1024);
	CleanupStack::PushL(buffer);

	RBufWriteStream bStream;
	bStream.Open(*buffer);
	CleanupClosePushL(bStream);


	RFileWriteStream fStream;
	User::LeaveIfError(fStream.Replace(aFs, aName, EFileWrite));

	WriteHeaderL(fStream);
	WriteDictionaryL(fStream);
	WriteResourceL(fStream);

	fStream.Close();

	CleanupStack::PopAndDestroy(2);	// bStream, buffer
	}



void CResComp::OptimizeDict()
	{
	// Find referenced dicts
	TInt entries = iDict->Count();

	for (TInt ii=0; ii<entries-1; ii++)
		{		
		TBuf8<KMaxDictEntry> dc;
		dc.Copy(iDict->At(ii).iData);

		FindEmbeddedDict(dc, ii+1);
		}

	// Sort dictionary in order of use

	CDictArray* newDict = new(ELeave)CDictArray();
	CleanupStack::PushL(newDict);

	for (TInt jj=0; jj<entries; jj++)
		{
		newDict->OrderedInsertL(iDict->At(jj));
		}

	TInt newDictEntries = newDict->Count();
	if (newDictEntries > iMaxEntries)
		newDictEntries = iMaxEntries;

	iDict->Reset();

	for (TInt kk=0; kk<newDictEntries; kk++)
		{
		iDict->SizedInsertL(newDict->At(kk));
		}

	CleanupStack::PopAndDestroy();	// newDict

	for (TInt zz=0; zz<newDictEntries; zz++)
		{
		iDict->At(zz).CreateEmbeddedDict(zz, iDict);
		}
	}


void CResComp::FindEmbeddedDict(TDesC8& aMatch, TInt aStart)
	{
	TInt entries = iDict->Count();
	for (TInt ii=aStart; ii<entries; ii++)
		{
		TInt found = aMatch.Find(iDict->At(ii).iData);
		if (found != KErrNotFound)
			{
			// Found embedded dict
			iDict->At(ii).iRef++;
			if (found > 1)
				{
				TPtrC8 left = aMatch.Left(found);
				FindEmbeddedDict(left, ii);
				}
			TInt right = aMatch.Length() - iDict->At(ii).iData.Length() - found;
			if (right > 1)
				{
				TPtrC8 rt = aMatch.Right(right);
				FindEmbeddedDict(rt, ii);
				}
			break;
			}
		}
	}


TPtrC8 CResComp::Resource(TInt aRes)
	{
	return TPtrC8(iResData+iResIndex[aRes], iResIndex[aRes+1] - iResIndex[aRes]);
	}


TInt CResComp::FindInResources(TDesC8& aBuf, TInt aMax)
	{
	TInt result = KErrNotFound;
	for (TInt ii=0; ii<aMax; ii++)
		{
		TPtrC8 res = Resource(ii);
		TInt found = res.Find(aBuf);
		if (found != KErrNotFound)
			return ii;
		}
	return result;
	}




void CResComp::WriteHeaderL(RWriteStream& aStream)
	{
	// Write resource header

	aStream.WriteInt16L(4);	// position of dictionary data
	aStream.WriteInt16L(iResources);
	}


void CResComp::WriteDictionaryL(RWriteStream& aStream)
	{
	WriteDictionaryHeaderL(aStream);
	WriteDictionaryIndexL(aStream);
	WriteDictionaryDataL(aStream);
	}

void CResComp::WriteDictionaryHeaderL(RWriteStream& aStream)
	{
/*	Dictionary header format

	Offset	Length(bytes)	Data
	0		1				Version number
	1		1				Number of dictionary items
	2		2				File offset of resource index
	4		2				Size of largest resource

//	2		2				File offset of dictionary
//	6		2				File offset of resource data
*/
	aStream.WriteUint8L(1);	// used to define different compression methods
	TInt dictSize = iDict->Count();
	TInt diff = (1 << iBitsForDict) - dictSize;
	aStream.WriteUint8L(diff);
	TInt headerSize = 4 + 7;
	TInt resourceIndex = headerSize + iDict->DictionarySize();
//	TInt resourceData = resourceIndex + (iResources	* 2);
	aStream.WriteUint16L(resourceIndex);
	aStream.WriteUint16L(iLargestResourceSize);
//	RDebug::Print(_L("ResourceIndex:%d bytes"), resourceIndex);
	aStream.WriteUint8L(iBitsForDict);
	}

void CResComp::WriteDictionaryIndexL(RWriteStream& aStream)
	{
	// Write bitIndex of each dictionary element
	// (as offset from start of dictionaryData)
	TInt dSize = iDict->Count();
	TInt offset = 0;
	aStream.WriteUint16L(0);
	for (TInt ii=0; ii<dSize; ii++)
		{
		offset += iDict->BitSize(ii);
		aStream.WriteUint16L(offset);
		}
	}

void CResComp::WriteDictionaryDataL(RWriteStream& aStream)
	{
	TBitWriter bitWrite(aStream);
	TInt dSize = iDict->Count();
	for (TInt ii=0; ii<dSize; ii++)
		{
		iDict->WriteBitStreamL(ii, bitWrite);
		}
	bitWrite.FlushL();
	}



void CResComp::WriteResourceL(RWriteStream& aStream)
	{
	WriteResourceIndexL(aStream);
	WriteResourceDataL(aStream);
	}

void CResComp::WriteResourceIndexL(RWriteStream& aStream)
	{
	// Write bitIndex of each resource element
	// (as offset from start of resource Index)
	TInt offset = 0;
	aStream.WriteUint16L(0);
	for (TInt ii=0; ii<iResources; ii++)
		{
		offset += ((iResArray->At(ii))->Size(iBitsForDict));
		aStream.WriteUint16L(offset);
		}
	}

void CResComp::WriteResourceDataL(RWriteStream& aStream)
	{
//	RDebug::Print(_L("Writing Resource Data"));
	TBitWriter bitWrite(aStream);
	for (TInt ii=0; ii<iResources; ii++)
		{
//		RDebug::Print(_L("Writing Resource %d"), ii);
		(iResArray->At(ii))->WriteBitStreamL(bitWrite,iBitsForDict);
		}
	bitWrite.FlushL();
	}



TInt CResComp::OriginalFileSize()
	{
	if(iOriginalCompressedSize)
		return iOriginalCompressedSize;
	else
		return iFileLength;
	}

TInt CResComp::CompressedSize()
	{
	return iCompressedSize;
	}

void CResComp::DisplayStats()
	{
	TInt size = 0;
	TInt rsize = 0;
	TInt count = iResArray->Count();

	for (TInt jj=0; jj<count; jj++)
		{
		rsize += ((iResArray->At(jj))->Size(iBitsForDict));// + 7) / 8;
		}

	rsize = (rsize + 7) / 8;

	size += 4;			// res header
	size += 7;			// dict header
	size += (count + 1) * 2;	// res index

	size += rsize;
	TInt dSize = iDict->DictionarySize();

//	iConsole->Printf(_L("Original Size: %d bytes\n"), iFileLength);
//	iConsole->Printf(_L("Compressed Resource Size: %d bytes\n"), size);
//	iConsole->Printf(_L("Compressed Dictionary Size: %d bytes\n"), dSize);
//	iConsole->Printf(_L("Compressed Size: %d bytes\n"), size+dSize);
//	iConsole->Printf(_L("Dictionary Size: %d \n"), iDict->Count());

	TInt compression = ((size + dSize) * 100) / iFileLength;
//	iConsole->Printf(_L("Compression = %d %%\n"), compression);


	iConsole->Printf(_L("\t%d\t%d\t%d\t%d %%\t%d\n"), iFileLength, size+dSize, compression,iOriginalCompressedSize, 1<<iBitsForDict);

	TInt bitsForDictIndex = 0;
	while ( (1<<bitsForDictIndex) < dSize)
		{
		bitsForDictIndex++;
		}
	TInt bitsSaved = iDict->Count() * (16 - bitsForDictIndex);
	bitsSaved = (bitsSaved + 7) / 8;
//	RDebug::Print(_L("Possible saving for dictionary: %d"), bitsSaved);

	TInt bitsForResIndex = 0;
	while ( (1<<bitsForResIndex) < rsize)
		{
		bitsForResIndex++;
		}
	TInt rbitsSaved = count * (16 - bitsForResIndex);
	rbitsSaved = (rbitsSaved + 7) / 8;
//	RDebug::Print(_L("Possible saving for res: %d"), rbitsSaved);

//	RDebug::Print(_L("Total Compressed Size: %d  Possible: %d"), size+dSize, (size+dSize) - (bitsSaved+rbitsSaved));
	}



// Check that none of the data lengths exceed 256 bytes.

void CResComp::CheckForLongDataStringsL()
	{
	RDebug::Print(_L("CheckingForLongDataStrings"));
	for (TInt ii=0; ii<iResources; ii++)
		{
		RDebug::Print(_L("Checking Resource %d"), ii);
		(iResArray->At(ii))->CheckForLongDataStringsL();
		}
	}
