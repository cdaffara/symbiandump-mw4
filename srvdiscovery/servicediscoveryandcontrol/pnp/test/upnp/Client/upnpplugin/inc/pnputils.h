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

#ifndef __PNPUTILS_H_
#define __PNPUTILS_H_
#include <e32base.h>
#include <es_sock.h>
#include <e32des8.h>
#include <e32def.h>
#include <uri8.h>
#include <es_sock.h>
#include <in_sock.h>

/* Utilities class
 @internalComponent
 */
class CUPnPUtils : public CBase
	{

public:
	/*
	 * Converts a RSubConParameterBundle to CSubConParameterBundle and vice-versa
	 */ 
	template <class T1 , class T2>
	static TUint ReplicateToReverseBundle (const T1& aSrcBundle,T2& aTargetBundle);
	
	enum TClientDllPanic
		{
		EPnpServiceDiscoveryHandleNotOpen
		};
	enum TUriType
		{
		EService,
		EDevice,
		EAny
		};
	TUriType iUriType;
	static void ResolveHostAddressL ( RHostResolver& aResolver,const TDesC8& aUri, TInetAddr& aSockAddr );
	static TInt GenericUriValidity ( const TDesC8& aUri ,TUriType aType);

private:
	static const TUint KOffset=3;
	static const TUint KIpv4MaxAddrSize = 20;
	};
	
template <class T1 , class T2>
TUint CUPnPUtils::ReplicateToReverseBundle (const T1& aSrcBundle,T2& aTargetBundle)

	{
	TUint length = aSrcBundle.Length();
	RBuf8 serializedBuffer;
	TUint err;
	if((err=serializedBuffer.Create(length))!=KErrNone)
		return err;
	//Create a serailized version of the cscparambundle and store it in tempdata
	if((err=aSrcBundle.Store(serializedBuffer))!=KErrNone)
		return err;
	//Deserialize tempData to rsubconparambundle	
	if((err=aTargetBundle.Load(serializedBuffer))!=KErrNone)
		return err;
	serializedBuffer.Close();
	return KErrNone;
	}
	


#endif /*PNPUTILS_H_*/
