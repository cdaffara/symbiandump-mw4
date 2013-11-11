/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#include "config.h"
#include "History.h"

#include "DeprecatedString.h"
#include "WebCoreHistory.h"

#include "HistoryInterface.h"
#include "HistoryController.h"
#include "BrCtl.h"
#include "StaticObjectsContainer.h"
namespace WebCore {

bool historyContains(const DeprecatedString& s)
    {
	// go through all available windows.
	bool ret = false;
    const Vector<CBrCtl*>& ctrls = StaticObjectsContainer::instance()->activeBrowserControls();
		for (int i=0; i<ctrls.size(); ++i) {
			if (ctrls[i]->historyHandler()->historyController()) 
				 ret = ctrls[i]->historyHandler()->historyController()->containsItemForURL(s.des());
			if (ret) break;
		} //for
	return ret;
    }
}
