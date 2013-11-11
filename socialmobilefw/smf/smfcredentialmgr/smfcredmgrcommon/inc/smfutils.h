/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Lasse Laasonen, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Description:
 * This header contains utility routines used by server and client
 */

#ifndef SMFUTILS_H_
#define SMFUTILS_H_

#include <e32base.h>

class RWriteStream;
class RReadStream;

/**
 * Utility to serialize data
 */
namespace SmfUtils
	{
	/**
	 * Method to externalize
	 * @param aDes descriptor to be externalized
	 * @param aStream The Write Stream to be filled.
	 */
	void ExternalizeDesL(const TDesC8& aDes, RWriteStream& aStream);

	/**
	 * Method to internalize 
	 * @param aDes descriptor to be internalized
	 * @param aStream the stream to read from.
	 */
	TInt InternalizeDesL(RBuf8& aDes, RReadStream& aStream);

	/**
	 * Method to externalize
	 * @param aDes descriptor to be externalized
	 * @param aStream The Write Stream to be filled.
	 */
	void ExternalizeDesL(const TDesC16& aDes, RWriteStream& aStream);

	/**
	 * Method to internalize 
	 * @param aStream the stream to read from.
	 */
	HBufC16* InternalizeDesL(RReadStream& aStream);

	TInt InternalizeDesL( RBuf16& aDes, RReadStream& aStream );
	/**
	 * Method to externalize 
	 * @param aInt integer to be externalized
	 * @param aStream The Write Stream to be filled.
	 */
	void ExternalizeInt64L(const TInt64& aInt, RWriteStream& aStream);

	/**
	 * Method to internalize 
	 * @param aInt integer to be internalized
	 * @param aStream the stream to read from.
	 */
	void InternalizeInt64L(TInt64& aInt, RReadStream& aStream);
	}

#endif /* SMFUTILS_H_ */
