/*
    This file is part of the KDE libraries

    Copyright (C) 1998 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2001 Dirk Mueller (mueller@kde.org)
    Copyright (C) 2002 Waldo Bastian (bastian@kde.org)
    Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

    This class provides all functionality needed for loading images, style sheets and html
    pages from the web. It has a memory cache for these objects.
*/

#include "config.h"
#include "DocLoader.h"

#include "Cache.h"
#include "CachedCSSStyleSheet.h"
#include "CachedImage.h"
#include "CachedScript.h"
#include "CachedXSLStyleSheet.h"
#include "Document.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "loader.h"

namespace WebCore {

DocLoader::DocLoader(Frame *frame, Document* doc)
    : m_cache(cache())
    , m_cachePolicy(CachePolicyVerify)
    , m_frame(frame)
    , m_doc(doc)
    , m_requestCount(0)
    , m_autoLoadImages(true)
    , m_loadInProgress(false)
    , m_allowStaleResources(false)
{
    m_cache->addDocLoader(this);
}

DocLoader::~DocLoader()
{
#if PRELOAD_SCANNER_ENABLED
    clearPreloads();
#endif
    HashMap<String, CachedResource*>::iterator end = m_docResources.end();
    for (HashMap<String, CachedResource*>::iterator it = m_docResources.begin(); it != end; ++it)
        it->second->setDocLoader(0);
    m_cache->removeDocLoader(this);
	m_reloadedURLs.clear();
}

void DocLoader::checkForReload(const KURL& fullURL)
{
    if (m_allowStaleResources)
        return; //Don't reload resources while pasting
    if (m_cachePolicy == CachePolicyVerify) {
       if (!m_reloadedURLs.contains(fullURL.url())) {
          CachedResource* existing = cache()->resourceForURL(fullURL.url());
#if PRELOAD_SCANNER_ENABLED
          if (existing && existing->isExpired() && !existing->isPreloaded()) {
#else
          if (existing && existing->isExpired()) {
#endif
             cache()->remove(existing);
             m_reloadedURLs.add(fullURL.url());
          }
       }
    } else if ((m_cachePolicy == CachePolicyReload) || (m_cachePolicy == CachePolicyRefresh)) {
       if (!m_reloadedURLs.contains(fullURL.url())) {
          CachedResource* existing = cache()->resourceForURL(fullURL.url());
#if PRELOAD_SCANNER_ENABLED
           if (existing && !existing->isPreloaded()) {
#else
          if (existing)
#endif
             cache()->remove(existing);
          m_reloadedURLs.add(fullURL.url());
#if PRELOAD_SCANNER_ENABLED
       }
#endif
       }
    }
}

CachedImage* DocLoader::requestImage(const String& url)
{
    CachedImage* resource = static_cast<CachedImage*>(requestResource(CachedResource::ImageResource, url, String()));
    if (autoLoadImages() && resource && resource->stillNeedsLoad()) {
        resource->setLoading(true);
        cache()->loader()->load(this, resource, true);
    }
    return resource;
}

CachedCSSStyleSheet* DocLoader::requestCSSStyleSheet(const String& url, const String& charset)
{
	return static_cast<CachedCSSStyleSheet*>(requestResource(CachedResource::CSSStyleSheet, url, charset));
}

CachedCSSStyleSheet* DocLoader::requestUserCSSStyleSheet(const String& url, const String& charset)
{
    return cache()->requestUserCSSStyleSheet(this, url, charset);
}

/* DocLoader::requestScript
 *
 * If we've been given the charset, just pass it on.
 * Otherwise, get it from the enclosing frame.
 *
 * Notes:
 *
 *   o We do this because some oddly-programmed web pages
 *     and/or servers deliver JavaScript that doesn't bear
 *     any obvious character set selection but the intended
 *     character set isn't the ISO-Latin-1 that will later
 *     be defaulted-in.
 *
 *   o We don't conditionalize this code for S60/Symbian
 *     because it should be either benign or even helpful
 *     to the other platforms as well.
 *
 */

CachedScript* DocLoader::requestScript(const String& url, const String& charset)
{

    if (!!charset)
        return static_cast<CachedScript*>(requestResource(CachedResource::Script, url, charset));
    else
        return static_cast<CachedScript*>(requestResource(CachedResource::Script, url, frame()->loader()->encoding() ));
}

#if ENABLE(XSLT)
CachedXSLStyleSheet* DocLoader::requestXSLStyleSheet(const String& url)
{
    return static_cast<CachedXSLStyleSheet*>(requestResource(CachedResource::XSLStyleSheet, url, String()));
}
#endif

#if ENABLE(XBL)
CachedXBLDocument* DocLoader::requestXBLDocument(const String& url)
{
    return static_cast<CachedXSLStyleSheet*>(requestResource(CachedResource::XBL, url, String()));
}
#endif

#if PRELOAD_SCANNER_ENABLED
CachedResource* DocLoader::requestResource(CachedResource::Type type, const String& url, const String& charset, bool isPreload)
#else
CachedResource* DocLoader::requestResource(CachedResource::Type type, const String& url, const String& charset)
#endif
{
    KURL fullURL = m_doc->completeURL(url.deprecatedString());
    
    if (cache()->disabled()) {
        HashMap<String, CachedResource*>::iterator it = m_docResources.find(fullURL.url());
        
        if (it != m_docResources.end()) {
            it->second->setDocLoader(0);
            m_docResources.remove(it);
        }
    }
                                                          
    if (m_frame && m_frame->loader()->isReloading())
        setCachePolicy(CachePolicyReload);

    checkForReload(fullURL);

#if PRELOAD_SCANNER_ENABLED
    CachedResource* resource = cache()->requestResource(this, type, fullURL, charset, isPreload);
#else
    CachedResource* resource = cache()->requestResource(this, type, fullURL, charset);
#endif
    if (resource) {
        m_docResources.set(resource->url(), resource);
        checkCacheObjectStatus(resource);
    }
    return resource;
}

void DocLoader::setAutoLoadImages(bool enable)
{
    if (enable == m_autoLoadImages)
        return;

    m_autoLoadImages = enable;

    if (!m_autoLoadImages)
        return;

    HashMap<String, CachedResource*>::iterator end = m_docResources.end();
    for (HashMap<String, CachedResource*>::iterator it = m_docResources.begin(); it != end; ++it) {
        CachedResource* resource = it->second;
        if (resource->type() == CachedResource::ImageResource) {
            CachedImage* image = const_cast<CachedImage*>(static_cast<const CachedImage *>(resource));

            CachedResource::Status status = image->status();
            if (status != CachedResource::Unknown)
                continue;

            cache()->loader()->load(this, image, true);
        }
    }
}

void DocLoader::setCachePolicy(CachePolicy cachePolicy)
{
    m_cachePolicy = cachePolicy;
}

void DocLoader::removeCachedResource(CachedResource* resource) const
{
    m_docResources.remove(resource->url());
}

void DocLoader::setLoadInProgress(bool load)
{
    m_loadInProgress = load;
    if (!load && m_frame)
        m_frame->loader()->loadDone();
}

void DocLoader::checkCacheObjectStatus(CachedResource* resource)
{
    // Return from the function for objects that we didn't load from the cache.
    if (!resource)
        return;
    switch (resource->status()) {
        case CachedResource::Cached:
            break;
        case CachedResource::NotCached:
        case CachedResource::Unknown:
        case CachedResource::New:
        case CachedResource::Pending:
            return;
    }
    
    // Notify the caller that we "loaded".
    if (!m_frame || m_frame->loader()->haveToldBridgeAboutLoad(resource->url()))
        return;
    
    ResourceRequest request(resource->url());
    const ResourceResponse& response = resource->response();
    SharedBuffer* data = resource->data();
    
    if (resource->sendResourceLoadCallbacks()) {
        // FIXME: If the WebKit client changes or cancels the request, WebCore does not respect this and continues the load.
        m_frame->loader()->loadedResourceFromMemoryCache(request, response, data ? data->size() : 0);
    }
    m_frame->loader()->didTellBridgeAboutLoad(resource->url());
}

void DocLoader::incrementRequestCount()
{
    ++m_requestCount;
}

void DocLoader::decrementRequestCount()
{
    --m_requestCount;
    ASSERT(m_requestCount > -1);
}

int DocLoader::requestCount()
{
    if (loadInProgress())
         return m_requestCount + 1;
    return m_requestCount;
}

#if PRELOAD_SCANNER_ENABLED
#define PRELOAD_DEBUG 0
void DocLoader::preload(CachedResource::Type type, const String& url)
{
    String encoding = (type == CachedResource::Script || type == CachedResource::CSSStyleSheet) ? m_doc->frame()->loader()->encoding() : String();
    CachedResource* resource = requestResource(type, url, encoding, true);
    if (!resource || m_preloads.contains(resource))
        return;
    resource->increasePreloadCount();
    m_preloads.add(resource);
#if PRELOAD_DEBUG
    printf("PRELOADING %s\n",  resource->url().latin1().data());
#endif
}

void DocLoader::clearPreloads()
{
#if PRELOAD_DEBUG
    printPreloadStats();
#endif
    ListHashSet<CachedResource*>::iterator end = m_preloads.end();
    for (ListHashSet<CachedResource*>::iterator it = m_preloads.begin(); it != end; ++it) {
        CachedResource* res = *it;
        res->decreasePreloadCount();
        if (res->canDelete()) 
        	{
      		 if(res->inCache())
      		 	cache()->remove(res);
      		 else
	            delete res;  
        	} 
        else if (res->preloadResult() == CachedResource::PreloadNotReferenced) 
            cache()->remove(res);
       
    }
    m_preloads.clear();
}

#if PRELOAD_DEBUG
void DocLoader::printPreloadStats()
{
    unsigned scripts = 0;
    unsigned scriptMisses = 0;
    unsigned stylesheets = 0;
    unsigned stylesheetMisses = 0;
    unsigned images = 0;
    unsigned imageMisses = 0;
    ListHashSet<CachedResource*>::iterator end = m_preloads.end();
    for (ListHashSet<CachedResource*>::iterator it = m_preloads.begin(); it != end; ++it) {
        CachedResource* res = *it;
        if (res->preloadResult() == CachedResource::PreloadNotReferenced)
            printf("!! UNREFERENCED PRELOAD %s\n", res->url().latin1().data());
        else if (res->preloadResult() == CachedResource::PreloadReferencedWhileComplete)
            printf("HIT COMPLETE PRELOAD %s\n", res->url().latin1().data());
        else if (res->preloadResult() == CachedResource::PreloadReferencedWhileLoading)
            printf("HIT LOADING PRELOAD %s\n", res->url().latin1().data());
        
        if (res->type() == CachedResource::Script) {
            scripts++;
            if (res->preloadResult() < CachedResource::PreloadReferencedWhileLoading)
                scriptMisses++;
        } else if (res->type() == CachedResource::CSSStyleSheet) {
            stylesheets++;
            if (res->preloadResult() < CachedResource::PreloadReferencedWhileLoading)
                stylesheetMisses++;
        } else {
            images++;
            if (res->preloadResult() < CachedResource::PreloadReferencedWhileLoading)
                imageMisses++;
        }
        
        if (res->errorOccurred())
            cache()->remove(res);
        
        res->decreasePreloadCount();
    }
    m_preloads.clear();
    
    if (scripts)
        printf("SCRIPTS: %d (%d hits, hit rate %d%%)\n", scripts, scripts - scriptMisses, (scripts - scriptMisses) * 100 / scripts);
    if (stylesheets)
        printf("STYLESHEETS: %d (%d hits, hit rate %d%%)\n", stylesheets, stylesheets - stylesheetMisses, (stylesheets - stylesheetMisses) * 100 / stylesheets);
    if (images)
        printf("IMAGES:  %d (%d hits, hit rate %d%%)\n", images, images - imageMisses, (images - imageMisses) * 100 / images);
}
#endif
#endif
}
