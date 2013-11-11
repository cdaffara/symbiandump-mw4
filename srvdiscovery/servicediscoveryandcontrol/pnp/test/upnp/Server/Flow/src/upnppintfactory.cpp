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

#include "upnppint.h"

using namespace ESock;
// =====================================================================================
// UPnP Protocol-Interface Factory

CUPnPProtocolIntfFactory* CUPnPProtocolIntfFactory::NewL(TUid aFactoryId, CProtocolIntfFactoryContainer& aParentContainer)
	{
	CUPnPProtocolIntfFactory* factory = new (ELeave) CUPnPProtocolIntfFactory(aFactoryId, aParentContainer);
	//Note!!!! UPnP PINT Factory is not a Plugin,so make self construct that adds this factory to its
	//parent container, which is not the case with other factory that were created as Plugin's
	CleanupStack::PushL(factory);
    factory->ConstructL();
    CleanupStack::Pop(factory);
	return factory;
	}

CUPnPProtocolIntfFactory::CUPnPProtocolIntfFactory(TUid aFactoryId,
												   CProtocolIntfFactoryContainer& aParentContainer)
												: CProtocolIntfFactoryBase(aFactoryId, aParentContainer)
	{
	}

CProtocolIntfBase* CUPnPProtocolIntfFactory::DoCreateProtocolIntfL(TFactoryQueryBase& aQuery)
	{	
	const TDefaultProtocolIntfFactoryQuery& query = static_cast<const TDefaultProtocolIntfFactoryQuery&>(aQuery);
	return CUPnPProtocolIntfBase::NewL(*this, query.iCprId);
	}

