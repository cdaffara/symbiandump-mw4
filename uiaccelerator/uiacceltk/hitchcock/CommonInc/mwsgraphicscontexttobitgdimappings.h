/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef MWSGRAPHICSCONTEXTTOBITGDIMAPPINGS_H
#define MWSGRAPHICSCONTEXTTOBITGDIMAPPINGS_H

#include <gdi.h>
#include <graphics/wsgraphicscontext.h>

class MWsGraphicsContextToBitGdiMappings
	{
public:
	static CGraphicsContext::TFillRule Convert(MWsGraphicsContext::TFillRule aFillRule) { return (CGraphicsContext::TFillRule)aFillRule; }
	static CGraphicsContext::TPenStyle Convert(MWsGraphicsContext::TPenStyle aPenStyle) { return (CGraphicsContext::TPenStyle)aPenStyle; }
	static CGraphicsContext::TBrushStyle Convert(MWsGraphicsContext::TBrushStyle aBrushStyle) { return (CGraphicsContext::TBrushStyle)aBrushStyle; }
	static CGraphicsContext::TTextAlign Convert(MWsGraphicsContext::TTextAlign aTextAlign) { return (CGraphicsContext::TTextAlign)aTextAlign; }
	static TFontUnderline Convert(MWsGraphicsContext::TFontUnderline aFontUnderline) { return (TFontUnderline)aFontUnderline; }
	static TFontStrikethrough Convert(MWsGraphicsContext::TFontStrikethrough aFontStrikethrough) { return (TFontStrikethrough)aFontStrikethrough; }
	static CGraphicsContext::TDrawMode LossyConvert(MWsGraphicsContext::TDrawMode aDrawMode)
		{
		if(aDrawMode == MWsGraphicsContext::EDrawModePEN)
			return CGraphicsContext::EDrawModePEN;
		if(aDrawMode == MWsGraphicsContext::EDrawModeWriteAlpha)
			return CGraphicsContext::EDrawModeWriteAlpha;
		return CGraphicsContext::EDrawModePEN;
		}
	static TInt LossyConvertPenSize(const TSize& aSize) { return aSize.iWidth; }
	
	};


template<class T>
class TArrayWrapper : public TArray<T>
	{
public:
	TArrayWrapper(const T* aArray, TInt aCount);
private:
	static TInt Count(const CBase* aPtr);
	static const TAny* At(const CBase* aPtr, TInt aIndex);
private:
	const T* iArray;
	const TInt iCount;
	};

template<class T>
TArrayWrapper<T>::TArrayWrapper(const T* aArray, TInt aCount)
	: TArray<T>(TArrayWrapper::Count, TArrayWrapper::At, reinterpret_cast<const CBase*>(this)), iArray(aArray), iCount(aCount)
	{
	//reinterpret_cast above since this class doesn't derive from CBase but TArray is
	//only silly requiring CBase as opposed to TAny, so this is safe
	ASSERT(iArray);
	}

template<class T>
TInt TArrayWrapper<T>::Count(const CBase* aPtr)
	{
	//reinterpret_cast since this class doesn't derive from CBase but TArray is
	//only silly requiring CBase as opposed to TAny, so this is safe
	const TArrayWrapper* self = reinterpret_cast<const TArrayWrapper*>(aPtr);
	return self->iCount;
	}

template<class T>
const TAny* TArrayWrapper<T>::At(const CBase* aPtr, TInt aIndex)
	{
	//reinterpret_cast since this class doesn't derive from CBase but TArray is
	//only silly requiring CBase as opposed to TAny, so this is safe
	const TArrayWrapper* self = reinterpret_cast<const TArrayWrapper*>(aPtr);
	if(!Rng(0, aIndex, self->iCount - 1))
		User::Panic(_L("USER"), 130); //out of bounds (RArray/RPointerArray)
	return self->iArray + aIndex;
	}


#endif //MWSGRAPHICSCONTEXTTOBITGDIMAPPINGS_H

