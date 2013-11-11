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

#ifndef __GENERIC_CONTAINER_H_
#define __GENERIC_CONTAINER_H_

#include <comms-infras/ss_protflow.h> 
#include <comms-infras/ss_nodemessages.h> 

const TInt KAppProtIntfFactoryUid = 0x2000D05B; // Put in common place

class CFactory;
class CAppProtIntfFactoryContainer : public CBase, public Messages::ASimpleNodeIdBase
	{
public:
	static CAppProtIntfFactoryContainer* NewL ( );
	
private:
	virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	CAppProtIntfFactoryContainer (  );
	void ConstructL();
private:
	CFactory *iAppProtIntfFactory;
	};


class CFactoryObject;
class CFactory : public CBase
	{
	friend class CAppProtIntfFactoryContainer;
public:
	CFactoryObject* FindFactoryObject ( Factories::MFactoryQuery& aFactoryQuery );
	CFactoryObject* FindOrCreateFactoryObjectL ( Factories::MFactoryQuery& aFactoryQuery );
	CFactoryObject* CreateFactoryObjectL ( Factories::MFactoryQuery& aFactoryQuery );

protected:
	CFactory ();
	~CFactory ();
	virtual CFactoryObject* DoCreateFactoryObjectL ( Factories::MFactoryQuery& aFactoryQuery ) =0;
private:
	RPointerArray<CFactoryObject> iObjectsList;

public:
	TUid iDestroyUid;
	};
	
class CFactoryObject : public CBase, public Messages::ASimpleNodeIdBase
	{
public:
	TInt ProtocolType();
	virtual ~CFactoryObject();
protected:
	CFactoryObject(TInt aProtocolType);
private:
	TInt iProtocolFlag;
	};


#endif //__GENERIC_CONTAINER_H_
