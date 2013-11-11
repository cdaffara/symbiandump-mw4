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

#ifndef __CUPNPDEVICECOMPOSER_H_
#define __CUPNPDEVICECOMPOSER_H_

#include <e32cons.h>
#include <e32debug.h>
#include "cpnpdeviceparam.h"
#include "cstringpoolmanager.h"
/**
CUPnPDeviceXmlComposer is used to compose the parsed xml device description file back into a compatible format
@publishedPartner
@prototype
*/

class CUPnPDeviceXmlComposer: public CBase
    {
public:
	static CUPnPDeviceXmlComposer* NewL( const RStringPool& aStringPool );
	~CUPnPDeviceXmlComposer();
	void ComposeDeviceXmlL(const CUPnPDeviceDescription *aDeviceDescObj, RBuf8 &aXmlData);


private:
	CUPnPDeviceXmlComposer(  const RStringPool& aStringPool );
		
	// Helper functions to construct the XML
	void AppendDeviceXmlL(const CUPnPDevice* aDevObj, RBuf8 &aXmlData, TInt &aCount);
	void AppendDeviceAttributesL(const CUPnPDevice*	aDeviceAttribute, RBuf8 &aXmlData, TInt &aCount);
	void AppendDeviceIconL(const CUPnPIconInfo* aIconInfo, RBuf8 &aXmlData, TInt &aCount);
	void AppendServiceL(const CUPnPServiceInfo* aServInfo, RBuf8 &aXmlData, TInt &aCount);
	void AppendLiteralL(TInt aTag, RBuf8& aBuf, TInt& aCount, TBool aIsStart = EFalse);
	void AppendLiteralL(const TDesC8& aLiteral, RBuf8& aBuf, TInt& aCount);
	void AppendLiteralL(const RString& aString, RBuf8& aBuf, TInt& count, TBool aIsStart );
	const TStringTable& GetTable();
	
    TInt 					iError;
    const RStringPool&		iStringPool;
  
    };

#endif /*CUPNPDEVICECOMPOSER_H_*/
