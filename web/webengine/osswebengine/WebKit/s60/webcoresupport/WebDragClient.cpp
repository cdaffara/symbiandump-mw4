/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WebDragClient
*
*/

#include "config.h"
#include "WebDragClient.h"
#include "PlatformString.h"
#include "DragImage.h"
#include "DebugStream.h"

using namespace WebCore;

void WebDragClient::willPerformDragDestinationAction(DragDestinationAction, DragData*)          { notImplemented(); }
void WebDragClient::willPerformDragSourceAction(DragSourceAction, const IntPoint&, Clipboard*)  { notImplemented(); }
DragDestinationAction WebDragClient::actionMaskForDrag(DragData*)                               { notImplemented(); return DragDestinationAction(); }
DragSourceAction WebDragClient::dragSourceActionMaskForPoint(const IntPoint& windowPoint)       { notImplemented(); return DragSourceAction(); }

void WebDragClient::startDrag(DragImageRef dragImage, const IntPoint& dragImageOrigin, const IntPoint& eventPos, Clipboard*, Frame*, bool linkDrag) { notImplemented(); }
DragImageRef WebDragClient::createDragImageForLink(KURL&, const String& label, Frame*)          { notImplemented(); return 0; }

void WebDragClient::dragControllerDestroyed()
{
    delete this;
}

