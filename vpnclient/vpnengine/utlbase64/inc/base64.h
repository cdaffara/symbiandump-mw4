/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _BASE64_H_
#define _BASE64_H_

#define B64_NOP_CHR '='
#define B64_LINEBREAK '\n'
#define B64_NOP_VAL 254
#define B64_IGN 255
#define MAXLINELEN 76

// encoding table
static const TUint8 Kb64 [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

class TBase64Codec
{
	public:
		// Public interface
		IMPORT_C TInt Base64Encode(const TDesC8 &aRawData, const TDes8 &aEncodedData);
		IMPORT_C TInt Base64Decode(const TDesC8 &aEncodedData, const TDes8 &aDecodedData);
		IMPORT_C HBufC8* Base64EncodeLC(const TDesC8 &aRawData);
		IMPORT_C HBufC8* Base64DecodeLC(const TDesC8 &aEncodedData);
		IMPORT_C TBase64Codec();

	private:	
		TUint8 iD64[256]; /* decoding table */
		void InitBase64(void);
};

#endif

