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
#include <e32std.h>

class GfxMath
{
private:

class TGfxInt64
{
public:
	TInt32	l;									/* low part			*/
	TInt32 h;									/* high part		*/
} ;

public:
	static TInt svgScalarSin( TInt r1 );
	static TInt svgScalarCos( TInt r1 );
	static TInt svgScalarTan( TInt r1 );
private:
	static TInt32 hgAbs32( TInt32 x );
	static TUint32 hgMulu64h (TUint32 a, TUint32 b);
	static TGfxInt64 hgMulu64 (TUint32 a, TUint32 b);
	static TGfxInt64 hgMul64 (TInt32 a, TInt32 b);
	static TGfxInt64 hgMadd64 (TGfxInt64 a, TInt32 b, TInt32 c);
	static TGfxInt64 hgSet64(TInt32 hh,TInt32 ll);
	static TInt32 hgLsl64h_0_31 (const TGfxInt64	a, TInt32	sh);
	static TInt svgiRCos( TInt32 _x );
	static TGfxInt64 hgAdd64 (const TGfxInt64 a, const TGfxInt64 b);
	static TInt32 hgMul64h (TInt32 a, TInt32 b);
	static TInt32 hgGet64h (const GfxMath::TGfxInt64 a);
};// class GfxMath
