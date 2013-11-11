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

#ifndef __CUPNPREGISTERNOTIFY_H_
#define __CUPNPREGISTERNOTIFY_H_
#include <e32base.h>

#include <mpnpobserver.h>
#include "cupnpelementarray.h"

/* Used to register for specific advertisements
 @internalComponent
 */

class CUPnPRegisterNotify : public CBase
{
public:
	void SubmitRequestL(const RPnPParameterBundle& aServiceInfo);
	void NotifyResultsL(TUint32 aFamily, RPnPParameterBundleBase& aBundle);
	~CUPnPRegisterNotify();
	void CancelNotifyAnnouncementL( const RPnPParameterBundle& aServiceInfo );
	CUPnPRegisterNotify(RSubConnection& aSubConnection );
private:
	RSubConnection& 	iSubConnection;
	CUPnPElementArray* 	iElementArray;

};
#endif /*CUPNPREGISTERNOTIFY_H_*/
