/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Base64 encoder/decoder
*
*/



#include <eikenv.h>
#include "base64.h"

////////////////////////////////////////////////////////////////////////////////////////
//	Constructor
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TBase64Codec::TBase64Codec()
{
	// Initialize base64
	InitBase64();
}

////////////////////////////////////////////////////////////////////////////////////////
//	Initialize decoding array
////////////////////////////////////////////////////////////////////////////////////////
void TBase64Codec::InitBase64(void)
{
	TUint8 i;
	Mem::Fill(iD64, sizeof(iD64), B64_IGN);
	for (i=0;i<sizeof(Kb64);i++)
	{
		iD64[Kb64[i]]=i;
	}
	iD64[B64_NOP_CHR]=B64_NOP_VAL;
}

////////////////////////////////////////////////////////////////////////////////////////
//	Base64 encoding function using buffers allocated by the caller
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt TBase64Codec::Base64Encode(const TDesC8 &aRawData, const TDes8 &aEncodedData)
{
	TInt i;
	TInt j;
	TInt datalen;
	TUint8 block[3];
	const TUint8 *fromPtr;
	TUint8 *toPtr;
	TUint count = 0;
	TInt lengthIncludingLinebreaks;
	TInt lengthWithoutLinebreaks;


	i=0;
	fromPtr = aRawData.Ptr();
	toPtr = CONST_CAST(TUint8*, aEncodedData.Ptr());
	datalen = aRawData.Size();
	lengthWithoutLinebreaks = ((datalen + 3)/ 3) * 4;
	lengthIncludingLinebreaks = lengthWithoutLinebreaks + (lengthWithoutLinebreaks) / MAXLINELEN - ((lengthWithoutLinebreaks % MAXLINELEN)?0:1); 
	if(aEncodedData.MaxLength() < lengthIncludingLinebreaks)
		return KErrArgument;

	for (j=0;j<datalen;j+=3)
	{
		/* 11111100>>2 */
		/* 00000011<<4 + 11110000>>4 */
		/*               00001111<<2 + 11000000>>6 */
		/*                             00111111    */
		Mem::FillZ(block, 3);
		Mem::Copy(block, &fromPtr[j], datalen-j<3?datalen-j:3);
		toPtr[i++] = Kb64[(block[0]&0xfc)>>2]; 
		toPtr[i++] = Kb64[((block[0]&0x03)<<4)+(block[1]>>4)];
		if (datalen-j>1)
		{
			toPtr[i++] = Kb64[((block[1]&0x0f)<<2)+(block[2]>>6)]; 
		}
		else
		{
			toPtr[i++] = B64_NOP_CHR;
		}
		if (datalen-j>2)
		{
			toPtr[i++]=Kb64[block[2]&0x3f]; 
		}
		else
		{
			toPtr[i++] = B64_NOP_CHR;
		}
		count += 4;
		// Add linebreaks every 76th characters
		// Don't add it after the last full length line
		if(count >= MAXLINELEN && j < datalen - 3)
		{
			toPtr[i++] = B64_LINEBREAK;
			count = 0;
		}
	}
	
	CONST_CAST(TDes8 &, aEncodedData).SetLength(i);
	return KErrNone;
}

////////////////////////////////////////////////////////////////////////////////////////
//	Base64 decoding function using buffers allocated by the caller
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt TBase64Codec::Base64Decode(const TDesC8 &aEncodedData, const TDes8 &aDecodedData)
{
	TInt i;
	TInt j;
	TInt k;
	TInt blocklen;
	TInt datalen;
	const TUint8 *fromPtr;
	TUint8 *toPtr;
	
	TUint8 c = 0;
	TUint8 block[4];

	InitBase64();
	
	i=0;
	j=0;
	fromPtr = aEncodedData.Ptr();
	toPtr = CONST_CAST(TUint8*, aDecodedData.Ptr());
	datalen = aEncodedData.Size();
	if(aDecodedData.MaxLength() < (datalen / 4) * 3)
		return KErrArgument;
	
	while ((datalen > 0) && (j <= datalen))
	{
		blocklen=0;
		Mem::Fill(block,'\0',sizeof(block));
		for (k=0;k<4;k++) /* skip non base64 characters */
		{
			while (j++ <= datalen && (c=iD64[*fromPtr++])==B64_IGN) {;}
			if (j<=datalen)
			{
				block[k]=c;
				if (c!=B64_NOP_VAL)
				{
					blocklen++;
				}
				else
				{
					block[k]=0;
				}
			}
			else
			{
				block[k]=0;
			}
		}
		if (blocklen)
		{
			toPtr[i+0] = STATIC_CAST(TUint8, (block[0]<<2) + (block[1]>>4));

			if(blocklen > 1)
				toPtr[i+1] = STATIC_CAST(TUint8, ((block[1]&0x0f)<<4) + (block[2]>>2));

			if(blocklen > 2)
				toPtr[i+2] = STATIC_CAST(TUint8, ((block[2]&0x03)<<6) + (block[3]));

			i+=blocklen-1;
		}
	}
	CONST_CAST(TDes8 &, aDecodedData).SetLength(i);
	return KErrNone;
}

////////////////////////////////////////////////////////////////////////////////////////
//	Base64 encoding function, buffer allocated by the function and returned to the caller
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C HBufC8* TBase64Codec::Base64EncodeLC(const TDesC8 &aRawData)
{
	HBufC8 *outBuf = NULL;
	TInt rCode;
	TUint lengthIncludingLinebreaks;
	TUint lengthWithoutLinebreaks;

	lengthWithoutLinebreaks =((aRawData.Length() + 3)/ 3) * 4;
	lengthIncludingLinebreaks = lengthWithoutLinebreaks + (lengthWithoutLinebreaks) / MAXLINELEN - ((lengthWithoutLinebreaks % MAXLINELEN)?0:1); 
	outBuf = HBufC8::NewLC(lengthIncludingLinebreaks);
	TPtr8 outDes = outBuf->Des();
	rCode = Base64Encode(aRawData, outDes);
	if(rCode != KErrNone)
	{
		CleanupStack::Pop();
		delete outBuf;
		outBuf = NULL;
	}
	return (outBuf);
}

////////////////////////////////////////////////////////////////////////////////////////
//	Base64 decoding function, buffer allocated by the function and returned to the caller
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C HBufC8* TBase64Codec::Base64DecodeLC(const TDesC8 &aEncodedData)
{
	HBufC8 *outBuf = NULL;
	TInt rCode;
	
	outBuf = HBufC8::NewLC(((aEncodedData.Length() / 4) * 3));
	TPtr8 outDes = outBuf->Des();
	rCode = Base64Decode(aEncodedData, outDes);
	if(rCode != KErrNone)
	{
		CleanupStack::Pop();
		delete outBuf;
		outBuf = NULL;
	}
	return (outBuf);
}

