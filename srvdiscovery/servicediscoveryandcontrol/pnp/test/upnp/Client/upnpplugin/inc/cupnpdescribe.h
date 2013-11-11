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

#ifndef __CUPNPDESCRIBE_H_
#define __CUPNPDESCRIBE_H_
#include <e32base.h>

#include <upnpparamset.h>
#include <pnpparameterbundle.h>

#include "cupnpelementarray.h"
/* This class is used to send describe requests to the service point.Also the
   responses are conveyed back to the client code.
   @internalComponent
 */
class CUPnPDescribe: public CBase
	{
public:
	CUPnPDescribe( RSubConnection& aSubConnection ,RHostResolver& aResolver);
	void SubmitRequestL( const RPnPParameterBundle& aServiceInfo );
	void NotifyResultsL(RPnPParameterBundleBase& aBundle);
	void CancelDescribeL( const RPnPParameterBundle& aServiceInfo );
	~CUPnPDescribe();

private:
	RSubConnection& iSubConnection;
	RHostResolver&  iResolver;
	CUPnPElementArray* iElementArray;

	};
#endif /*CUPNPDESCRIBE_H_*/
