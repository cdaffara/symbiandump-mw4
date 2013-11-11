/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content model for data plug-in.
*
*/


#ifndef WRTHARHESTERCONST_H
#define WRTHARHESTERCONST_H

#include <aicontentmodel.h>
#include "sapidatapluginuids.hrh"

// AI Data Plug-in ECOM implementation UID.
const TInt KImplUidDataPlugin = SAPIDP_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DATAPLUGIN;
const TUid KUidDataPlugin = { KImplUidDataPlugin };

// ================================= CONTENT ===================================

// CPS Constants
_LIT8( KType,          "type");
_LIT( KCpData,         "cp_data");
_LIT( KPubData,        "publisher" );
_LIT8( KFilter,        "filter" );
_LIT8( KDataMap,       "data_map");
_LIT8( KActionTrigger, "action_trigger" );
_LIT( KExecute,        "execute" );
_LIT8( KGetList,       "GetList" );
_LIT8( KResults,       "results" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT8( KChangeInfo, "change_info" );
_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType, "content_type" );
_LIT8( KContentId,   "content_id" );
_LIT8( KOperation, "operation" );
_LIT8( KDeActive, "deactive");
_LIT8( KActive, "active");
_LIT8( KSuspend , "suspend");
_LIT8( KResume, "resume");
_LIT8( KSelected, "selected");
_LIT8( KHSOnline, "online");
_LIT8( KHSOffline, "offline");
_LIT8( KTemplateType, "template_type");
_LIT8( KWidgetName,   "widget_name");
_LIT8( KWidgetInfo,   "widget_info");
_LIT( KTemplatedWidget, "ai3templatedwidget");
_LIT8( KMyActionMap, "action_map" );
_LIT8( KMyItem, "item" );
_LIT8( KMyAdd, "Add" );
_LIT8( KMyDelete, "Delete" );
_LIT8( KMyItemId, "item_id" );
_LIT8( KItemOnlineOffline, "online_offline");
_LIT8( KMenuItems, "menuitems");
_LIT( KTemplateName, "wideimage" );
_LIT( KMyActionName, "data" );
_LIT( KWRTPublisher, "wrt_publisher");

#endif /* WRTHARHESTERCONST_H */
