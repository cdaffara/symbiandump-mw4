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
#include "PlugInInfoStore.h"

#include "StaticObjectsContainer.h"
#include "WebCorePluginHandler.h"
#include "PluginHandler.h"


namespace WebCore {

PluginInfo *PlugInInfoStore::createPluginInfoForPluginAtIndex(unsigned index)
{

    PluginInfo *pluginInfo = new PluginInfo;
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    
    if (pluginInfo && pluginHandler) {
        TWebCorePluginInfo plugin;
        if (pluginHandler->objectAtIndex(index,plugin)) {
            pluginInfo->name = plugin.iName;
            pluginInfo->file = plugin.iFilename;
            pluginInfo->desc = plugin.iPluginDescription;

            for (int i=0; i<plugin.iMimeTypeArray.Count(); i++) {
                HBufC *ptr = plugin.iMimeTypeArray[i];
                TPtrC MIME(ptr->Des());
                MimeClassInfo *mime = new MimeClassInfo;
                if (mime) {                
                    pluginInfo->mimes.append(mime);
                    mime->type = String(MIME).lower();
                    HBufC8* tmp = pluginHandler->extensionsForMIMEType(index,MIME);
                    if (tmp)
                        mime->suffixes = *tmp;
                    else
                        mime->suffixes = String();
                    delete tmp;
                    //
                    HBufC* tmp16 = pluginHandler->descriptionForMIMEType(index,MIME);
                    if (tmp16)
                        mime->desc = *tmp16;                    
                    else
                        mime->desc = String();
                    delete tmp16;
                    mime->plugin = pluginInfo;        
                }
                else {
                // cleanup
                deleteAllValues(pluginInfo->mimes);
                delete pluginInfo;            
                pluginInfo = NULL;
                }
            }
        }
    }    
    return pluginInfo;
}

unsigned PlugInInfoStore::pluginCount() const
{

    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    if (pluginHandler) {
        return pluginHandler->getPluginCount();
    }
    
    return 0;
}

bool PlugInInfoStore::supportsMIMEType(const String& mimeType)
{

    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    if (pluginHandler) {
        return pluginHandler->pluginSupportsMIMEType(mimeType.des());
    }
    
    return false;
}

void refreshPlugins(bool reloadOpenPages)
{
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    if (pluginHandler) {
        return pluginHandler->refreshPlugins(reloadOpenPages);
    }
}

}

