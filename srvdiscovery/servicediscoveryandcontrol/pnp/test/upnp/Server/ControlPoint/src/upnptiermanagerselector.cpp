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

#include <metadatabase.h>
#include <commsdattypesv1_1.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <es_connpref.h>


#include "upnptiermanagerselector.h"
#include "upnptiermanagerfactory.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPTmTag KESockTierTag
#endif

using namespace ESock;
using namespace Meta;
using namespace CommsDat;
	
CUPnPProviderSelector::CUPnPProviderSelector(const SMetaData& aSelectionPreferences)
	: ASimpleSelectorBase(aSelectionPreferences)
	{
	}
	
void CUPnPProviderSelector::SelectL(ESock::ISelectionNotify& aSelectionNotify)
	{
	ASSERT(iDbs);
	ASSERT(iTierRecord);
	
	TUint32 defaultAccessPoint = iTierRecord->iDefaultAccessPoint;

	// This selector deals only with the AccessPoint aware system!
	ASSERT(0 != defaultAccessPoint);

	if (iSelectionPrefs.IsEmpty())
		{
		aSelectionNotify.SelectComplete(this, FindOrCreateProviderL(defaultAccessPoint));
		aSelectionNotify.SelectComplete(this, NULL);
		
		return;
		}		
		
	const TConnPref& prefs = iSelectionPrefs.Prefs();
	switch (prefs.ExtensionId())
		{
		case TConnPref::EConnPrefSnap:
			{
			TUint accessPoint = static_cast<const TConnSnapPref&>(prefs).Snap();
			aSelectionNotify.SelectComplete(this, FindOrCreateProviderL(accessPoint));
			aSelectionNotify.SelectComplete(this, NULL);		
			}			
			break;
		
		/* Note! As of now UPnP Tier is not having multiple access points to open
		case TConnPref::EConnPrefIdList:
			{
			const TConnIdList& list = static_cast<const TConnIdList&>(prefs);
			TInt count = list.Count();
			for (TInt i = 0; i < count; i++)
				{
				aSelectionNotify.SelectComplete(this,FindOrCreateProviderL(list.Get(i)));
				}
			aSelectionNotify.SelectComplete(this,NULL);
			}
			break;
		*/
		default:
			User::Leave(KErrArgument);
		}	
	}

MProviderSelector* TUPnPProviderSelectorFactory::NewSelectorL(const SMetaData& aSelectionPreferences)
	{
	ASSERT(aSelectionPreferences.IsTypeOf(TSelectionPrefs::TypeId()));
	CMDBSession* dbs = CMDBSession::NewLC(KCDVersion1_2);
	ASimpleSelectorBase* selector = new (ELeave) CUPnPProviderSelector(aSelectionPreferences);
	CleanupStack::PushL(selector);
	ASSERT(NULL == selector->iTierRecord);
	selector->iTierRecord = TierManagerUtils::LoadTierRecordL(TUid::Uid(CUPnPTierManagerFactory::iUid), *dbs);
	CleanupStack::Pop(selector);
	CleanupStack::Pop(dbs);
	ASSERT(NULL == selector->iDbs);
	selector->iDbs = dbs;
	
	return selector;
	}
