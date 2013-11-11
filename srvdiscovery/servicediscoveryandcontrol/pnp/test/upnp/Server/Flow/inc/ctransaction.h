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
// @file
// @internalComponent
// 
//

#ifndef __CTRANSACTION_H_
#define __CTRANSACTION_H_

// System Includes
#include <http/framework/cheadercodec.h>
#include <rmemchunk.h>

// Local Includes
#include "CRequest.h"
#include "CResponse.h"

 	
class CTransaction : public CBase
	{
public:
	static CTransaction* NewL ( CHeaderCodec& aCodec, RStringPool& aStringPool );	
	virtual ~CTransaction ( );
	
	CRequest* Request ( ) const;
	CResponse* Response ( ) const;	
	
	inline void AddBodyPart ( RMBufChain& aData )
		{
		if ( !aData.IsEmpty () )
			iBodyParts.Append ( aData );
		}
	
	inline void AddBodyPart ( RMemChunk& aMemChunk )
		{
		if ( !aMemChunk.IsEmpty () )
			iMemParts.Append ( aMemChunk );
		}
	
	inline void FreeMemChunk ( )
		{
		if ( !iMemParts.IsEmpty () )
			iMemParts.Free();
		}

	inline void RemoveAllBodyParts ( ) 
		{
		if ( iBodyParts.IsEmpty ( ) )
			{
			return;
			}
			
		while ( iBodyParts.NumBufs ( ) > 0 )
			{
			iBodyParts.Remove ( );
			if ( iBodyParts.IsEmpty ( ) )
				{
				break;
				}
			}		
		}
		
	inline RMBufChain& BodyParts ( )
		{
		return iBodyParts;
		}

	inline RMemChunk& MemParts ( )
		{
		return iMemParts;
		}

	inline void RemoveBodyPart ()
		{
		RMBuf* mBuf = iBodyParts.Remove ();	 // Remove the first body part
		__ASSERT_DEBUG ( mBuf, User::Invariant() );
		if ( iDataLeft != KErrUnknown )
			iDataLeft -= mBuf->Length ();
		}
	
	inline void GetBodyPart ( TPtrC8& aData )
		{
		RMBuf* buf = iBodyParts.First ();
		aData.Set ( buf->Ptr (), buf->Length () );		
		}
	
	inline TBool IsLastBodyPart () const
		{
		if ( iDataLeft != KErrUnknown )
			{
			return ( ( iDataLeft - iBodyParts.First()->Length() ) == 0)	? ETrue : EFalse;			
			}
		return Complete();
		}
	
	inline void SetDataLeft ( TInt aDataLeft )
		{
		iDataLeft = aDataLeft;			
		}

	inline void SetComplete ()
		{
		iComplete = ETrue;		
		}
	
	inline TBool Complete() const
		{
		return iComplete;	
		}
	
	inline TInt DataLeft () const
		{
		return iDataLeft;	
		}
		
	inline void Reset ()
		{
		iDataLeft = KErrUnknown;
		iRequest->Reset ();
		iResponse->Reset ();
		if ( !iBodyParts.IsEmpty () )
			iBodyParts.Free ();
		iComplete = EFalse;
		}
	
	inline TBool CloseNeeded () const
		{
		if ( iDataLeft != KErrUnknown )
			{
			if ( iBodyParts.IsEmpty () )
				{
				return iDataLeft != 0 ? ETrue : EFalse;
				}
			else
				{
				return ( ( iDataLeft - iBodyParts.Length() ) != 0 ) ? ETrue : EFalse;
				}
			}
				
		return !Complete ();	
		}
	
protected:
	CTransaction ( );
	void ConstructL ( CHeaderCodec& aCodec, RStringPool& aStringPool );
	
protected:
	TInt		iDataLeft;
	CRequest* 	iRequest;
	CResponse*	iResponse;
	RMBufChain	iBodyParts;
	RMemChunk	iMemParts;
	TBool		iComplete : 1;	
	};


#endif // CTRANSACTION_H

