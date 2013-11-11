/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class containing list of CAs
*
*/



#include "ikecalist.h"
#include "ikecaelem.h"


EXPORT_C CIkeCaList::CIkeCaList(TInt aGranularity)
:CArrayPtrFlat<CIkeCaElem>(aGranularity) 
    {
    }


CIkeCaList::~CIkeCaList() 
    { 
    ResetAndDestroy(); 
    }

CIkeCaElem* CIkeCaList::FindCaElem(const TDesC8& aKeyIdentifier) const
    {
	//
	// Find a CIkeCaElem element using aKeyIdentifier as search argument
	//
	CIkeCaElem* result = NULL;
	TInt i = 0;
	TInt count = Count();
	while ( i < count )
	    {
		CIkeCaElem* caElem = At(i);
		if ( caElem->KeyHash().Compare(aKeyIdentifier) == 0 )
    		{
    		result = caElem;
    		break;	
    		}	
		i ++;
	    }
	return result;
    }	
