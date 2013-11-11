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

#ifndef __CUPNPDESCRIPTIONCOMPOSER_H_
#define __CUPNPDESCRIPTIONCOMPOSER_H_
#include <e32base.h>
#include <stringpool.h>
class CUPnPServiceXmlComposer;
class CUPnPDeviceXmlComposer;
class CUPnPDescriptionComposer : public CBase
	{
public:
	enum TUPnPDescriptionType
		{
		EService,
		EDevice
		};
public:
	IMPORT_C static CUPnPDescriptionComposer* NewL( const RStringPool& aStringPool);
	IMPORT_C void ComposeDescriptionXmlL( TAny* aDescriptionObj , TUPnPDescriptionType aType , RBuf8 &aXmlData );
	IMPORT_C ~CUPnPDescriptionComposer();

private:
	CUPnPDescriptionComposer( const RStringPool& aStringPool );
	const RStringPool& iStringPool;
	CUPnPServiceXmlComposer* iServiceComposer;
	CUPnPDeviceXmlComposer* iDeviceComposer;
	};
#endif /*CUPNPDESCRIPTIONCOMPOSER_H_*/
