// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CUPNPDESCRIPTIONPARSER_H_
#define __CUPNPDESCRIPTIONPARSER_H_

#include <cstringpoolmanager.h>
#include "cpnpserviceparam.h"
#include "cpnpdeviceparam.h"

class CUPnPDescriptionParser : public CBase
	{

public:
	enum TUPnPDescriptionType
		{
		EService,
		EDevice
		};
public:
	IMPORT_C static CUPnPDescriptionParser* NewL(const RStringPool& aStringPool , TUPnPDescriptionType aType );
	IMPORT_C ~CUPnPDescriptionParser();
    IMPORT_C TAny* ParseDescriptionBufL(const TDesC8 &aDocument );
      
private:
	CUPnPDescriptionParser( const RStringPool& aStringPool ,  TUPnPDescriptionType aType);
	
	const RStringPool&			iStringPool;
	TUPnPDescriptionType		iDescriptionType;
	CUPnPServiceDescription*	iServiceDescObj;
	CUPnPDeviceDescription*		iDeviceDescObj;
	};
#endif /*CUPNPDESCRIPTIONPARSER_H_*/
