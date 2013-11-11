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

#ifndef __PNPEXTPARAMETERFAMILY_H__
#define __PNPEXTPARAMETERFAMILY_H__

#include <es_sock.h>
#include <elements/metadata.h>
#include <comms-infras/es_parameterfamily.h>
#include <comms-infras/es_parameterbundle.h>

/** Container of parameter sets.
@internalAll
*/
class RUPnPParameterFamily : public RParameterFamily
	{
	public:
		inline void DeleteParamSetL ( TInt aIndex );
	};

#include <upnp/pnpextparameterfamily.inl>

#endif //__PNPEXTPARAMETERFAMILY_H__