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

#ifndef __UPNPSPSUBSCRIBEINFOCONTAINER_H_
#define __UPNPSPSUBSCRIBEINFOCONTAINER_H_

#include "upnpspsubscribeinfo.h"

class CUPnPSubscribeInfoContainer : public CBase
	{
	private:
	mutable RBuf8 iEventData;
	mutable RPointerArray<CUPnPSubscribeInfoElement>	iSubscribeInfoElementArr;
	
	public:
	// Internal APIs
	inline static CUPnPSubscribeInfoContainer* NewL ( );
	// D'tor
	inline ~CUPnPSubscribeInfoContainer ( );
	inline void SetEventDataL ( const TDesC8& aEventData ) const;
	inline const TDesC8& EventData ( ) const ;
	inline void AppendSubscribeInfoElementL ( CUPnPSubscribeInfoElement* aElement ) const;	
	inline void DeleteSubscribeInfoElement ( CUPnPSubscribeInfoElement* aElement );
	inline CUPnPSubscribeInfoElement* GetSubscribeInfoBySid ( const TDesC8& aSid ) const;
	inline TInt CountOfSubscribeInfoElementArray ( ) const;
	inline CUPnPSubscribeInfoElement* AtSubscribeInfoElementArray ( TInt aIndex ) const;	
	};
	
#include "upnpspsubscribeinfocontainer.inl"	

#endif /*UPNPSPSUBSCRIBEINFOCONTAINER_H_*/
