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
 * This source contains utility routines used by server and client
 * 
 */

#include <s32strm.h>
#include "smfutils.h"

namespace SmfUtils
	{
	void ExternalizeDesL(const TDesC8& aDes, RWriteStream& aStream)
		{
		TInt length = aDes.Length();
		aStream.WriteInt32L(length);

		if (length > 0)
			{
			aStream << aDes;
			}
		}

	TInt InternalizeDesL(RBuf8& aDes, RReadStream& aStream)
		{
		TInt length = aStream.ReadInt32L();
		aDes.Close();

		if (length > 0)
			{
			aDes.CreateL(aStream, length);
			}
		else
			{
			aDes.CreateL(KNullDesC8);
			}

		return length;
		}

	void ExternalizeDesL(const TDesC16& aDes, RWriteStream& aStream)
		{
		TInt length = aDes.Length();
		aStream.WriteInt32L(length);

		if (length > 0)
			{
			aStream << aDes;
			}
		}

	HBufC16* InternalizeDesL(RReadStream& aStream)
		{
		TInt length = aStream.ReadInt32L();
		if (length)
			{
			HBufC* buf = HBufC::NewL(aStream, length);
			return buf;
			}
		else
			return NULL;
		}
	
	TInt InternalizeDesL( RBuf16& aDes, RReadStream& aStream )
	    {
	    TInt length = aStream.ReadInt32L();
	    
	    aDes.Close();
	    
	    if( length > 0 )
	        {
	        aDes.CreateL( aStream, length );
	        }
	    else
	        {
	        aDes.CreateL( KNullDesC16 );
	        }

	    return length;
	    }
	
	void ExternalizeInt64L(const TInt64& aInt, RWriteStream& aStream)
		{
		TInt32 low = I64LOW( aInt );
		TInt32 high = I64HIGH( aInt );
		aStream.WriteInt32L(low);
		aStream.WriteInt32L(high);
		}

	void InternalizeInt64L(TInt64& aInt, RReadStream& aStream)
		{
		TInt32 low = aStream.ReadInt32L();
		TInt32 high = aStream.ReadInt32L();
		aInt = MAKE_TINT64(high, low);
		}

	}
