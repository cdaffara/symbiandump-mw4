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








#ifndef WSOVITOKENCREATIONRESPONSE_H_
#define WSOVITOKENCREATIONRESPONSE_H_

#include <SenBaseFragment.h>
#include "uri8.h"


const TInt KStateParsingTokenInfo = 100;
const TInt KStateParsingUserInfo = 102;

class CWSOviTokenCreationResponse : public CSenBaseFragment
{
public:
/**
 * Function:    NewL
 *
 * Description: Create instance of concrete implementation. Note that ECOM
 *              interface implementations can only have two signatures for
 *              NewL:
 *               - NewL without parameters (used here)
 *               - NewL with TAny* pointer, which may provide some client
 *                 data
 *
 * Returns:    Instance of this class.
 *
 * Note:       The interface, which is abstract base class of this
 *             implementation, also provides NewL method. Normally abstract
 *             classes do not provide NewL, because they cannot create
 *             instances of themselves.
 */
static CWSOviTokenCreationResponse* NewL();
static CWSOviTokenCreationResponse* NewLC();

/**
* Getter for token
* @return token
*/
TPtrC8 Token();

/**
* Getter for tokenSecret
* @return tokenSecret
*/
TPtrC8 TokenSecret();

/**
* Getter for ttl
* @return ttl
*/
TPtrC8 TTL();

/**
* Getter for validUntil
* @return validUntil
*/
TPtrC8 ValidUntil();

/**
* Getter for username
* @return username
*/
TPtrC8 Username();


TBool IsTokenUpdate();
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
	virtual ~CWSOviTokenCreationResponse();
	
protected:
	CWSOviTokenCreationResponse();

	void ConstructL();
private:
	HBufC8* iToken;
    HBufC8* iTokenSecret;
	HBufC8* iTTL;
	HBufC8* iUsername;
	TBool iIsTokenUpdate;
	HBufC8* iValidUntil;
};

#endif /*WSOVITOKENCREATIONRESPONSE_H_*/

