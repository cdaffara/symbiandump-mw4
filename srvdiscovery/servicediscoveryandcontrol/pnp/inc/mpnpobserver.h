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
// PnPObserver.h
// 
//

#ifndef __MPNPOBSERVER_H__
#define __MPNPOBSERVER_H__

#include <e32def.h>
#include <e32base.h>

class RPnPParameterBundleBase;
/**
Observer class which has a callback function to return the responses. Client must implement to handle the responses.
@publishedPartner
@prototype
*/
class MPnPObserver
	{
public:
	/**
	Callback method which returns the responses.It can be success or fail and is contained in the response parameter bundle
	@param aServiceEventInfo Parameter bundle containg the response.
	*/
	virtual void OnPnPEventL ( RPnPParameterBundleBase& aServiceEventInfo) = 0;
	/**
	Callback method which handles the errors of submitting requests. In case this method is returned, the client should check
	all the values passed in the corresponding bundle because it'll be called only in case of invalid bundle or low memory condition
	Note that unlike OnPnPEventL ,this is not a callback method and returns synchronously in case of error.
	@param aError error code.
	*/
	virtual void OnPnPError (TInt aError) = 0;
	};

#endif  /*__MPNPOBSERVER_H__*/
