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



#ifndef C_IKECALIST_H
#define C_IKECALIST_H

#include <e32base.h>


class CIkeCaElem;
/**
 *  CA List
 *  List of CAs
 *
 *  @lib IkeCert
 *  @since S60 v3.0
 */
class CIkeCaList : public CArrayPtrFlat<CIkeCaElem>
    {
public:
	IMPORT_C CIkeCaList(TInt aGranularity);
	~CIkeCaList();
	CIkeCaElem* FindCaElem(const TDesC8& aKeyIdentifier) const;
    };

#endif // ? C_IKECALIST_H
