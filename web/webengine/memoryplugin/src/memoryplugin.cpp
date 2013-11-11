/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of OOM memory plugin. Registers Browser UID with
*				 the framework to receive OOM notifications.
*
*/


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <oommonitorplugin.h>

/*
 * Note:  Even though we create this plugin to deal with memory events from the system for handling OOM,
 * there is no code further down the line that actually handles them.  We totally ignore these events.
 * This is because the system has too high of an OOM threshold.
 * I.e. the system may only have 6m left and think it's out of memory, however, the browser can still render
 * many pages in only 6m.  So, these system events are ignored and the browser handles OOM with its own mechanism.
 * (See OOMStopper and OOMHandler)
 */
TAny* CreateBrowserOOMPlugin()
	{
	const TUid KBrowserAppUid = { 0x10008d39 };
	return CAppOomMonitorPlugin::NewL(KBrowserAppUid);
	}

const TImplementationProxy ImplementationTable[] =
	{
	{{0x10281f93}, ::CreateBrowserOOMPlugin}
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
	return ImplementationTable;
	}
