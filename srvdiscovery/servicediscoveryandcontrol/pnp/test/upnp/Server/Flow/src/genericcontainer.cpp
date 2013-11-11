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
// System Includes
// 
//

#include<ss_glob.h>
#include <ecom/ecom.h>
#include <appprotintf/app_protintf_msgs.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_factory.h>

//Local Includes
#include "genericcontainer.h"


using namespace ESock;
using namespace Messages;


CAppProtIntfFactoryContainer* CAppProtIntfFactoryContainer::NewL ( )
	{
	CAppProtIntfFactoryContainer *factContainer = new ( ELeave ) CAppProtIntfFactoryContainer (  );
	CleanupStack::PushL(factContainer);
	factContainer->ConstructL();
	CleanupStack::Pop(factContainer);
	return factContainer;
	}

CAppProtIntfFactoryContainer::CAppProtIntfFactoryContainer ( )
	{
	
	}

void CAppProtIntfFactoryContainer::ConstructL()
	{
	TUid destroyUid;
	if(!iAppProtIntfFactory)
		{
		iAppProtIntfFactory = reinterpret_cast<CFactory*>( REComSession::CreateImplementationL(TUid::Uid( KAppProtIntfFactoryUid ), destroyUid ));
		iAppProtIntfFactory->iDestroyUid = destroyUid;	
		}
	}

void CAppProtIntfFactoryContainer::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	CFactoryObject* myFactoryObject = NULL;	
	const TCFFactory::TFindOrCreatePeer& msg = message_cast<const TCFFactory::TFindOrCreatePeer> ( aMessage );
	if ( iAppProtIntfFactory )
		{
		myFactoryObject = iAppProtIntfFactory->FindOrCreateFactoryObjectL ( *(msg.iQuery) );
		}

	RClientInterface::OpenPostMessageClose ( NodeId (), aSender, TCFFactory::TPeerFoundOrCreated ( myFactoryObject->Id () ).CRef() ); 
	}

CFactory::CFactory()
	{
	
	}

CFactory::~CFactory ()
	{
	if (iDestroyUid.iUid)
		{
		REComSession::DestroyedImplementation(iDestroyUid);
		}
	iObjectsList.ResetAndDestroy();
	}
	
CFactoryObject* CFactory::FindFactoryObject ( Factories::MFactoryQuery& aFactoryQuery )
	{
	const TAppProtIntfQuery& query = static_cast < const TAppProtIntfQuery& > ( aFactoryQuery );
	TInt count = iObjectsList.Count();
	CFactoryObject* obj = NULL;
	for ( TInt ind = 0; ind < count; ind++ )
		{
		if (iObjectsList[ind]->ProtocolType() == query.iProtocol )
    		obj = iObjectsList[ind];
		}
	return obj;
	}
	
CFactoryObject* CFactory::FindOrCreateFactoryObjectL ( Factories::MFactoryQuery& aFactoryQuery )
	{
	CFactoryObject* myFactoryObj = FindFactoryObject ( aFactoryQuery );
	return myFactoryObj ? myFactoryObj : CreateFactoryObjectL ( aFactoryQuery );
	}
	
CFactoryObject* CFactory::CreateFactoryObjectL ( Factories::MFactoryQuery& aFactoryQuery )
	{
	CFactoryObject* myFactoryObj = DoCreateFactoryObjectL ( aFactoryQuery );	
	CleanupStack::PushL ( myFactoryObj );
	iObjectsList.Append(myFactoryObj);
	CleanupStack::Pop ( myFactoryObj );
	return myFactoryObj;	
	}
	
CFactoryObject::CFactoryObject( TInt aProtocolType) : iProtocolFlag(aProtocolType)
	{
	}
	
TInt CFactoryObject::ProtocolType()
	{
	return iProtocolFlag;
	}

CFactoryObject::~CFactoryObject()
	{
		
	}

