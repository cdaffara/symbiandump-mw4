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



#include "resentry.h"
#include "resdict.h"
#include "rescomp.h"

//TResComponent

TInt TResComponent::Size(TInt aDictionaryBits)
	{
	if (iType == EResTypePlain)
		{
		if (iLength == 0)
			return 0;
		TInt bits = iLength * 8;
		bits++;			// data flag
		if (iLength == 1)
			bits ++;	// first bit zero if 1 byte of data
		else 
			{
			bits++;		// first bit 1 if >1 byte of data
			if (iLength == 2)
				{
				bits ++;	// second bit zero if 2 bytes of data
				}
			else
				{
				bits ++;		// second bit 1 if >2 bytes of data
				if (iLength < 11)
					{
					// write 4 bits of data + 1 zero bit flag
					bits += 4;
					}
				else
					{
					// write 8 bits of data + 1 '1' bit flag
					bits += 9;
					}
				}
			}

//		console->Printf(_L("Type Plain: %d\n"), bits);
		return bits;
		}
	if (iType == EResTypeToken)
		{
		return 1+aDictionaryBits;
//		console->Printf(_L("Type Token: %d\n"), iToken);
//		return 8;		// dictionary entries are stored in 6 bits
		}
	return 0;
	}


void TResComponent::WriteBitStreamL(TBitWriter& aWriter, TInt aDictionaryBits)
	{
	if (iType == EResTypePlain)
		{
		if (iLength == 0)
			return;
// Debugging code
/*
		RDebug::Print(_L("Data Length: %d"), iLength);
		TBuf<100> b;
		for (TInt xx=0; xx<iLength; xx++)
			{
			b.AppendFormat(_L("%02x "), iData[xx]);
			}
		RDebug::Print(b);
*/
//
		// Write 'data' flag
		aWriter.WriteL(1, 1);
		if (iLength == 1)
			{
			aWriter.WriteL(0, 1); // first bit zero if 1 byte of data
			}
		else 
			{
			aWriter.WriteL(1, 1);		// first bit 1 if >1 byte of data
			if (iLength == 2)
				{
				aWriter.WriteL(0, 1); // second bit zero if 2 bytes of data
				}
			else
				{
				aWriter.WriteL(1, 1); // second bit 1 if >2 bytes of data
				if (iLength < 11)
					{
					aWriter.WriteL(0, 1);
					aWriter.WriteL(iLength-3, 3);
					// write 4 bits of data + 1 zero bit flag
					}
				else
					{
					aWriter.WriteL(1, 1);
					aWriter.WriteL(iLength, 8);
					// write 8 bits of data + 1 '1' bit flag
					if (iLength > 255)
						{
						User::Panic(_L("Entry too long"),0);
						}
					}
				}
			}
		// Now write data to output
		for (TInt ii=0; ii<iLength; ii++)
			{
			aWriter.WriteL(iData[ii], 8);
			}
		}
	if (iType == EResTypeToken)
		{
		// Write 'token' flag
		aWriter.WriteL(0, 1);

		// Write token
		aWriter.WriteL(iToken, aDictionaryBits);
//		RDebug::Print(_L("Token [%d] "), iToken);
		}
	}



// CResEntry
CResEntry::CResEntry(CDictArray* aDict)
:iDict(aDict)
	{
	iComponent = new(ELeave)CArrayFixFlat<TResComponent>(4);
	}

CResEntry::~CResEntry()
	{
	delete iComponent;
	}


void CResEntry::AddPlainDataL(TUint8* aData, TInt aLength)
	{
	TResComponent comp;
	comp.iType = EResTypePlain;
	comp.iData = aData;
	comp.iLength = aLength;
	iComponent->AppendL(comp);
	}

void CResEntry::InsertTokenL(TInt aPosition, TInt aToken)
	{
	TResComponent comp;
	comp.iType = EResTypeToken;
	comp.iToken = aToken;
	comp.iLength=0;
	iComponent->InsertL(aPosition,comp);
	(*iDict)[aToken].iUses++;
	}

void CResEntry::SplitPlainDataWithZeroSpacedL(TInt aComponent, TInt aZeroSpacedStart, TInt aLength)
	{
	TResComponent comp = (*iComponent)[aComponent];

	TResComponent newComp1;
	newComp1.iType = EResTypePlain;
	newComp1.iData = comp.iData;
	newComp1.iLength = aZeroSpacedStart;

	TResComponent newComp2;
	newComp2.iType = EResTypePlain;
	newComp2.iData = comp.iData + aZeroSpacedStart + aLength;
	newComp2.iLength = comp.iLength-(aZeroSpacedStart+aLength);

	TResComponent newComp3;
	newComp3.iType = EResTypeZeroSpaced;
	newComp3.iData = comp.iData + aZeroSpacedStart;
	newComp3.iLength = aLength;


	iComponent->Delete(aComponent);
	TInt componentPos = aComponent;
    if (newComp1.iLength != 0)
		{
		iComponent->InsertL(componentPos, newComp1);
		componentPos++;
		}

	iComponent->InsertL(componentPos, newComp3);
	componentPos++;

	if (newComp2.iLength != 0)
		{
		iComponent->InsertL(componentPos, newComp2);
		}
	}


void CResEntry::SplitPlainDataWithTokenL(TInt aComponent, TInt aToken, TInt aPosition)
	{
	TResComponent comp = (*iComponent)[aComponent];
	TDesC8& dEntry = (*iDict)[aToken].Data();
	TInt dictLength = dEntry.Length();

	TResComponent newComp1;
	newComp1.iType = EResTypePlain;
	newComp1.iData = comp.iData;
	newComp1.iLength = aPosition;

	TResComponent newComp2;
	newComp2.iType = EResTypePlain;
	newComp2.iData = comp.iData + aPosition + dictLength;
	newComp2.iLength = comp.iLength - dictLength - aPosition;

	iComponent->Delete(aComponent);
	TInt componentPos = aComponent;
    if (newComp1.iLength != 0)
		{
		iComponent->InsertL(componentPos, newComp1);
		componentPos++;
		}
	InsertTokenL(componentPos, aToken);
	componentPos++;

	if (newComp2.iLength != 0)
		{
		iComponent->InsertL(componentPos, newComp2);
		}

	}


TInt CResEntry::Size(TInt aDictionaryBits)
	{
	TInt size = 0;
	TInt count = iComponent->Count();
	for (TInt ii=0; ii<count; ii++)
		{
		size += iComponent->At(ii).Size(aDictionaryBits);
		}
	return size;
	}

void CResEntry::WriteBitStreamL(TBitWriter& aWriter, TInt aDictionaryBits)
	{
	TInt count = iComponent->Count();
	for (TInt ii=0; ii<count; ii++)
		{
		iComponent->At(ii).WriteBitStreamL(aWriter, aDictionaryBits);
		}
	}



TBool CResEntry::DoMatchDictL(TResComponent& aComp, TInt aOffset, TInt aDictIndex)
	{
	TInt dCount = iDict->Count();
	TInt cSize = aComp.iLength;
	for (TInt ii=0; ii<dCount; ii++)
		{
		if (ii != aDictIndex)
			{
			TDesC8& dEntry = (*iDict)[ii].Data();
			if (dEntry.Length() <= cSize)
				{
				TPtrC8 data(aComp.iData, cSize);
				TInt pos = data.Find(dEntry);
				if (pos != KErrNotFound)
					{
//					console->Printf(_L("Found %d Offset: %d\n"),ii, pos);
					SplitPlainDataWithTokenL(aOffset, ii, pos);
					return ETrue;
				}
			}
			}
		}
	return EFalse;
	}


void CResEntry::MatchDictL(TInt aDictIndex)
	{
	// Match plain text components against dictionary
	TInt count = iComponent->Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TResComponent comp = (*iComponent)[ii];
		if (comp.iType == EResTypePlain)
			{
			if (DoMatchDictL(comp, ii, aDictIndex))
				{
				count = iComponent->Count();
				ii--;
				}
			}
		}
	}


/*
void CResEntry::MatchZeroSpacedL()
	{
	// Find zero spaced data
	TInt count = iComponent->Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TResComponent comp = (*iComponent)[ii];
		if (comp.iType == EResTypePlain)
			{
			if (DoMatchZeroSpacedL(comp, ii))
				{
				count = iComponent->Count();
				ii--;
				}
			}
		}
	}


TBool CResEntry::DoMatchZeroSpacedL(TResComponent& aComp, TInt aOffset)
	{
	// Analyse data and see if there is a string of zero spaced data
	// Data, followed by zero
	TInt length = aComp.iLength;
	TInt pos = 0;
	TInt dataSize = 0;
	for (TInt ii=0; ii<length-1; ii++)
		{
		if (aComp.iData[ii+1] == 0)
			{
			// Found zero spaced data
			if (dataSize == 0)
				{
				// Start of zero spaced data
				pos = ii;
				dataSize = 2;
				}
			else
				{
				dataSize += 2;
				}
			ii++;
			}
		else
			{
			// End of zero spaced data
			if (dataSize > 2)
				{
				SplitPlainDataWithZeroSpacedL(aOffset, pos, dataSize);
				return ETrue;
				}
			dataSize = 0;
			}
		}
	return EFalse;
	}
*/


void CResEntry::MatchSelfL(TInt aRes,CResComp* aResComp)
	{
	// Find duplicates within resource
	TInt count = iComponent->Count();
	for (TInt ii=0; ii<count; ii++)
		{
		TResComponent comp = (*iComponent)[ii];
		if (comp.iType == EResTypePlain)
			{
			if (DoMatchSelfL(comp, ii, aRes, aResComp))
				{
				count = iComponent->Count();
				ii--;
				}
			}
		}
	}




/*
TBool CResEntry::MatchPreviousComponent(TInt aCompIndex, TDesC8& aMatch)
	{
	TInt size = aMatch.Size();
	for (TInt ii=0; ii<aCompIndex; ii++)
		{
		TResComponent comp = (*iComponent)[ii];
		if (comp.iType == EResTypePlain)
			{
			console->Printf(_L("Matching: %d (Length: %d)\n"), aCompIndex, size);
			if (comp.iLength <= size)
				{
				 TPtrC8 buf(comp.iData, comp.iLength);
				 TInt found = buf.Find(aMatch);
				 if (found != KErrNotFound)
					 {
					 console->Printf(_L("Found\n"));
					 return ii;
					 }
				}
			}
		}

	return KErrNotFound;
	}
*/

TBool CResEntry::DoMatchSelfL(TResComponent& aComp, TInt aOffset, TInt aRes, CResComp* aResComp)
	{
	TInt cSize = aComp.iLength;
	TInt matchSize = cSize/2;
	if (matchSize > KMaxDictEntry)
		matchSize = KMaxDictEntry;

	while (matchSize > 1)
		{
		TInt pos=matchSize;
		TInt positions = (cSize-(matchSize*2))+1;
		while (positions)
			{
			TPtr8 match(aComp.iData + pos, matchSize,matchSize);
			TPtr8 left(aComp.iData, pos, pos);
			TInt found = left.Find(match);
			if (found != KErrNotFound)
				{
				TInt token = iDict->AddEntryL(match);
				// Remove both matches and replace with token
				SplitPlainDataWithTokenL(aOffset, token, pos);
				SplitPlainDataWithTokenL(aOffset, token, found);
				return ETrue;
				}
			else
				{
				// Find in all other resources
				TInt find = aResComp->FindInResources(match, aRes);
				if (find != KErrNotFound)
					{
					TInt token = iDict->AddEntryL(match);
					SplitPlainDataWithTokenL(aOffset, token, pos);
					return ETrue;
					}

				}
			positions--;
			pos++;
			}
		matchSize--;
		}
	return EFalse;
	}


	TResType iType;
	TUint8* iData;
	TInt iLength;
	TInt iToken;


void CResEntry::CheckForLongDataStringsL()
	{
	TInt components = iComponent->Count();
	for (TInt ii=0; ii<components; ii++)
		{
		TResComponent comp = (*iComponent)[ii];
		if (comp.iLength > 255)
			{
			TInt length = comp.iLength;
			RDebug::Print(_L("Component Too Long!"));
			TResComponent comp1;
			comp1.iType = comp.iType;
			comp1.iData = comp.iData;
			comp1.iLength = 255;
			comp1.iToken = comp.iToken;

			TResComponent comp2;
			comp2.iType = comp.iType;
			comp2.iData = comp.iData + 255;
			comp2.iLength = length - 255;
			comp2.iToken = comp.iToken;

			iComponent->Delete(ii);
			iComponent->InsertL(ii, comp1);
			iComponent->InsertL(ii+1, comp2);

			components = iComponent->Count();
			}
		}
	}

