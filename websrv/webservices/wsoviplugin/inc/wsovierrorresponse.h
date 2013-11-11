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
* Description: Header declaration
*
*/








#ifndef WSOVIERRORRESPONSE_H_
#define WSOVIERRORRESPONSE_H_

#include <SenBaseFragment.h>



class CWSOviErrorResponse : public CSenBaseFragment
{
public:
    static CWSOviErrorResponse* NewL();
    static CWSOviErrorResponse* NewLC();
    
    TPtrC8 Code();
    
    TPtrC8 Text();

private:
// From CBaseFragment
virtual void StartElementL(const TDesC8& aNsUri,
                           const TDesC8& aLocalName,
                           const TDesC8& aQName,
                           const RAttributeArray& aAttributes);

virtual void EndElementL(const TDesC8& aNsUri,
                         const TDesC8& aLocalName,
                         const TDesC8& aQName);
public:
	virtual ~CWSOviErrorResponse();
	
protected:
	CWSOviErrorResponse();

	void ConstructL();
private:
	HBufC8* iCode;
    HBufC8* iText;

};

#endif /*WSOVIErrorRESPONSE_H_*/

