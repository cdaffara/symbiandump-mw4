/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CUpnpRemotableAppStore class declaration
*
*/

#ifndef __UPNPREMOTABLEAPPSTORE_H__
#define __UPNPREMOTABLEAPPSTORE_H__

#include "upnpremotableapp.h"

/**
* Container class for all the remotable applications which have been registered 
* with the terminal mode service. It provides an easy access to the list of remotable apps
*/
class CUpnpRemotableAppStore : public CBase
    {
public:
    static CUpnpRemotableAppStore* NewL();
	void AddRemotableAppL( CUpnpRemotableApp* aApp );
	TInt RemoveRemotableApp( TUint aAppId );
	~CUpnpRemotableAppStore();
	CUpnpRemotableApp& FetchRemotableApp( TInt aAppIndex )const;
	const RArray<TUint>& AppIdArray()const;

private:
	CUpnpRemotableAppStore();
	
private:
	RPointerArray<CUpnpRemotableApp>   iRemotableAppArray;
	RArray<TUint>                      iAppIdArray;
    };


#endif  // __UPNPREMOTABLEAPPSTORE_H__
