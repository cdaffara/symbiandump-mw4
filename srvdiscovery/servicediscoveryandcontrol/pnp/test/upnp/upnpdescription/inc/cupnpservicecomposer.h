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

#ifndef __CUPNPSERVICECOMPOSER_H_
#define __CUPNPSERVICECOMPOSER_H_

#include "cpnpserviceparam.h"
#include "cstringpoolmanager.h"

/**
CUPnPServiceXmlComposer is used to compose the parsed xml device description file back into a compatible format
@publishedPartner
@prototype
*/
// Composes the parsed xml file
class CUPnPServiceXmlComposer: public CBase
    {
public:
	static CUPnPServiceXmlComposer* NewL( const RStringPool& aStringPool);
	~CUPnPServiceXmlComposer();
	void ComposeServiceXmlL(const CUPnPServiceDescription *aServDescObj, RBuf8 &aXmlData);

private:
	// Helper functions
	CUPnPServiceXmlComposer( const RStringPool& aStringPool );
	const TStringTable& GetTable();
    void AppendLiteralL(TInt aTag, RBuf8& aBuf, TInt& aCount, TBool aIsStart );
    void AppendLiteralL(const TDesC8& aLiteral, RBuf8& aBuf, TInt& aCount);
    void AppendLiteralL(const RString& aString, RBuf8& aBuf, TInt& count, TBool aIsStart );
    void AppendActionL(const CUPnPAction* aUPnPAction, RBuf8 &aXmlData, TInt &aCount);	
    void AppendStateVariableL(const CUPnPStateVariable* aUPnPStateVariable, RBuf8 &aXmlData, TInt &aCount);
   
private:
    const RStringPool&			iStringPool;
    TInt 				iError;

    };
#endif /*CUPNPSERVICECOMPOSER_H_*/
