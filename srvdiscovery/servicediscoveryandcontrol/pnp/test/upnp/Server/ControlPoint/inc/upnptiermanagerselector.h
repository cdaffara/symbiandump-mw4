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


#ifndef __UPNPTIERMANAGERSELECTOR_H
#define __UPNPTIERMANAGERSELECTOR_H

#include <comms-infras/simpleselectorbase.h>

class TUPnPProviderSelectorFactory
	{
public:
	static ESock::MProviderSelector* NewSelectorL(const Meta::SMetaData& aSelectionPreferences);
	};
	
class CUPnPProviderSelector : public CBase, public ASimpleSelectorBase
	{
public:
	explicit CUPnPProviderSelector(const Meta::SMetaData& aSelectionPreferences);

private:
	virtual void SelectL(ESock::ISelectionNotify& aSelectionNotify);
	};

#endif // UPNPTIERMANAGERSELECTOR_H
