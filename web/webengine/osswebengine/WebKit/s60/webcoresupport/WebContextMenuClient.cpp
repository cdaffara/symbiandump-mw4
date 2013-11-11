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
* Description:   Implementation of WebContextMenuClient
*
*/

#include "config.h"
#include "WebContextMenuClient.h"
#include "ContextMenu.h"
#include "PlatformString.h"
#include "DebugStream.h"

using namespace WebCore;

void WebContextMenuClient::contextMenuDestroyed()
{
    delete this;
}

PlatformMenuDescription WebContextMenuClient::getCustomMenuFromDefaultItems(ContextMenu*)   { notImplemented(); return 0; }
void WebContextMenuClient::contextMenuItemSelected(ContextMenuItem*, const ContextMenu*)    { notImplemented(); }
void WebContextMenuClient::downloadURL(const KURL& url)                                     { notImplemented(); }
void WebContextMenuClient::searchWithGoogle(const Frame*)                                   { notImplemented(); }
void WebContextMenuClient::lookUpInDictionary(Frame*)                                       { notImplemented(); }
void WebContextMenuClient::speak(const String&)                                             { notImplemented(); }
void WebContextMenuClient::stopSpeaking()                                                   { notImplemented(); }



