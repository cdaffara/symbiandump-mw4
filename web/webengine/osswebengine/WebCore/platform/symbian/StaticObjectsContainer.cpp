/*
 * Copyright (C) 2006 Nokia, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include <../bidi.h>            // work around for multiple bidi.h files
#include "StaticObjectsContainer.h"
#include "BrCtl.h"
#include "AtomicString.h"
#include "PlatformFontCache.h"
#include "PictographSymbian.h"
#include "FontCache.h"
#include "FormFillController.h"
#include "ResourceLoaderDelegate.h"
#include "WebKitLogger.h"
#include "BrCtlDefs.h"
#include "WebIconDatabase.h"
#include "WebSurface.h"
#include "WebCursor.h"
#include "PluginHandler.h"
#include "WebCannedImages.h"
#include "OOMHandler.h"
#include "OOMStopper.h"
#include "SharedTimer.h"
#include "TextEncoding.h"
#include "TextEncodingRegistry.h"
#include "cssstyleselector.h"
#include "RenderStyle.h"
#include "Page.h"
#include "Cache.h"
#include "StreamingTextCodecSymbian.h"
#include "HTMLNames.h"
#include "XMLNames.h"
#include "MediaFeatureNames.h"
#include "EventNames.h"
#include "FontCache.h"
#include "RenderThemeSymbian.h"
#include "QualifiedName.h"
#include "XMLTokenizer.h"
#include "Document.h"
#include "StyleElement.h"
#include "bidi.h"
#include "RenderBox.h"
#include "FontCache.h"
#include "MimeTypeRegistry.h"
#include "ResourceHandleManagerSymbian.h"
#include "TextBreakIteratorSymbian.h"
#include "ImageSymbian.h"
#include "HTMLElementFactory.h"
#include <eikenv.h>

#include "WidgetEngineBridge.h"
#if defined(BRDO_LIW_FF)
#include "DeviceBridge.h"
#endif

const TInt KLowResolutionDpi = 130;
const TInt KMediumResolutionDpi = 200;

namespace WebCore {

StaticObjectsContainer* StaticObjectsContainer::gInstance = 0;

StaticObjectsContainer::StaticObjectsContainer() : 
     m_fontCache(0)
    ,m_formFillController(0)
    ,m_pictograph(NULL)
    ,m_resourceLoaderDelegate(NULL)
    ,m_stream(NULL)
    ,m_icondatabase(NULL)
    ,m_rendertarget(NULL)
    ,m_cursor(NULL)
    ,m_cannedimg(NULL)
    ,m_pluginhandler(NULL)
    ,m_refcount(0)
    ,m_capabilities(0)
    ,m_oomHandler(0)
    ,m_oomStopper(0)
    ,m_fullScreenMode(false)
    ,m_pluginFullscreen(false)
    ,m_symbianTheme(NULL)
{
    // Check the device resolution
    CEikonEnv* eikEnv = CEikonEnv::Static();
    if( eikEnv ) {
        CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();

        TUint dpi = screenDev.VerticalTwipsToPixels(KTwipsPerInch);

        if( dpi <= KLowResolutionDpi ) {
            m_screenRes = ELowScreenResolution;
        }
        else if( dpi > KLowResolutionDpi && dpi <= KMediumResolutionDpi ) {
            m_screenRes = EMediumScreenResolution;
        }
        else {
            m_screenRes = EHighScreenResolution;
        }
    }
    m_oomHandler = new OOMHandler();
    m_oomStopper = new OOMStopper();
    initSharedTimer();
}

StaticObjectsContainer::~StaticObjectsContainer()
{
    // Run KJS collector to cleanup any remaining references
    // This must be run before Cache::deleteStaticCache to properly free resources
    KJS::Collector::collect();
    delete m_oomHandler;
    delete m_oomStopper;
    FontCache::deleteFontDataCache();
    delete m_fontCache;
    delete m_formFillController;
    delete m_pictograph;
    delete m_resourceLoaderDelegate;
    delete m_stream;
    delete m_icondatabase;
    delete m_rendertarget;
    delete m_cannedimg;
    delete m_cursor;
    delete m_pluginhandler;
    delete m_symbianTheme;
    gInstance = NULL;
    deletePageStaticData();

    CSSStyleSelector::deleteDefaultStyle();
    Cache::deleteStaticCache();
    TextCodecSymbian::deleteStatAvailCharsets();

	cleanupChangedDocuments();
	mappedAttributeCleaner();
	cleanupMidpoints();
	cleanOverridSizeMap();
	cleanupFontDataCache();
	cleanupMimeTypes();
	cleanupHandleManager();
	cleanupFuncMap();
	cleanupIconFileName();
	cleanupIterators();

	XMLTokenizer::cleanupXMLStringParser();
    shutdownSharedTimer();
    m_widgetLibrary.Close();
#if defined(BRDO_LIW_FF)
    m_deviceLibrary.Close();
#endif
    CloseSTDLIB();
}

StaticObjectsContainer* StaticObjectsContainer::instance()
{
    if( !gInstance )
        gInstance = new (ELeave) StaticObjectsContainer;
    return gInstance;
}

PictographSymbian* StaticObjectsContainer::pictograph()
{
    if (!m_pictograph)
        {        
        m_pictograph = new PictographSymbian();
        }
    return m_pictograph;
}

PlatformFontCache* StaticObjectsContainer::fontCache()
{
    if( !m_fontCache )
        {
        m_fontCache = new PlatformFontCache();
        // tot:fixme. find out why platforminit is not called by the core
        FontCache::platformInit();
        }

    return m_fontCache;
}

FormFillController* StaticObjectsContainer::formFillController()
{
    if(!m_formFillController)
        m_formFillController = new FormFillController();

    return m_formFillController;
}

RFs& StaticObjectsContainer::fsSession()
{
    return CEikonEnv::Static()->FsSession();
}

ResourceLoaderDelegate* StaticObjectsContainer::resourceLoaderDelegate()
{
    if (!m_resourceLoaderDelegate) {
        m_resourceLoaderDelegate = new ResourceLoaderDelegate;
    }
    return m_resourceLoaderDelegate; 
}

LogStream& StaticObjectsContainer::logStream()
{
    if(!m_stream)
        m_stream = new LogStream();
    return *m_stream;
}


WebIconDatabase* StaticObjectsContainer::sharedIconDatabase()
{
    if (!m_icondatabase) {
        m_icondatabase = new WebIconDatabase();
        m_icondatabase->openSharedDatabase();
    }
    return m_icondatabase;
}

WebSurface* StaticObjectsContainer::webSurface()
{
    if (!m_rendertarget) {
        m_rendertarget = WebSurface::NewL();
    }
    return m_rendertarget;
}

WebCursor* StaticObjectsContainer::webCursor()
{
    if (!m_cursor) {
        m_cursor = WebCursor::NewL();
    }
    return m_cursor;
}

WebCannedImages* StaticObjectsContainer::webCannedImages()
{
    if (!m_cannedimg) {
        m_cannedimg = WebCannedImages::NewL();
    }
    return m_cannedimg;
}

PluginHandler* StaticObjectsContainer::pluginHandler()
{
    if (!m_pluginhandler) {
        //tot:fixme 
        TBool enablePlugin = ETrue;
        m_pluginhandler = PluginHandler::NewL(enablePlugin);
    }
    return m_pluginhandler;
}

#if defined(BRDO_LIW_FF)
RLibrary& StaticObjectsContainer::getDeviceBridgeLibL()
{
    if( !m_deviceLibrary.Handle() ) {
        _LIT( KDeviceDLLName, "jsdevice.dll" );
        User::LeaveIfError( m_deviceLibrary.Load(KDeviceDLLName) );
    }
    return m_deviceLibrary;
}
#endif 

RLibrary& StaticObjectsContainer::getWidgetEngineBridgeLibL()
{
    if( !m_widgetLibrary.Handle() ) {
        _LIT( KBrowserWidgetEngineName, "widgetengine.dll" );
        User::LeaveIfError( m_widgetLibrary.Load(KBrowserWidgetEngineName) );
    }
    return m_widgetLibrary;    
}

CBrCtl* StaticObjectsContainer::brctl() const
{
    if (m_activeBrCtls.size() > 0)
        return m_activeBrCtls[0];
    return NULL;        
}

const Vector<CBrCtl*>& StaticObjectsContainer::activeBrowserControls() const
{
    return m_activeBrCtls;
}

void StaticObjectsContainer::ref(CBrCtl& brctl)              
{ 
    m_refcount++; 
    m_activeBrCtls.append(&brctl);
}

void StaticObjectsContainer::deref(CBrCtl& brctl)            
{ 
    for (int i = 0; i < m_activeBrCtls.size(); i++) {
        if (m_activeBrCtls[i] == &brctl) {
            m_activeBrCtls.remove(i);
            break;
        }
    }
    if( --m_refcount==0 ) 
        delete this; 
}

void StaticObjectsContainer::setFullScreenMode(bool mode)
{
    m_fullScreenMode = mode;
}

bool StaticObjectsContainer::fullScreenMode(void){
    return m_fullScreenMode;
}

RenderTheme* StaticObjectsContainer::theme()
{
    if(!m_symbianTheme)
        m_symbianTheme = new RenderThemeSymbian();
    return m_symbianTheme;
}
}
// END OF FILE
