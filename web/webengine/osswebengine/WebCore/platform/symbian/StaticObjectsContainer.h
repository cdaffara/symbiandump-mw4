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

#ifndef __STATICOBJECTCONTAINER_H__
#define __STATICOBJECTCONTAINER_H__

#include <Browser_platform_variant.hrh>
#include <e32base.h>
#include <e32std.h>
#include <wtf/Noncopyable.h>
#include <wtf/HashMap.h>
#include <../bidi.h>
#include <eikenv.h>

class ResourceLoaderDelegate;
class LogStream;
class WebIconDatabase;
class WebSurface;
class WebCursor;
class PluginHandler;
class WebCannedImages;
class CBrCtl;
class MWidgetEngineBridge;
class MDeviceBridge;


namespace WebCore {
class RenderThemeSymbian;
class RenderTheme;
typedef enum
    {
    ELowScreenResolution,
    EMediumScreenResolution,
    EHighScreenResolution
    } TScreenResolution;

class PlatformFontCache;
class FormFillController;
class PictographSymbian;
class OOMHandler;
class OOMStopper;

class StaticObjectsContainer : public Noncopyable 
{
public:
    static StaticObjectsContainer* instance();

    PlatformFontCache* fontCache();
    FormFillController* formFillController();
    RFs& fsSession();
    PictographSymbian* pictograph();
    TScreenResolution screenResolution() const { return m_screenRes; }
    ResourceLoaderDelegate* resourceLoaderDelegate();
    WebIconDatabase* sharedIconDatabase();
    LogStream& logStream();
    WebSurface* webSurface();
    WebCursor* webCursor();
    WebCannedImages* webCannedImages();
    PluginHandler* pluginHandler();
    void setPluginFullscreen(bool val) { m_pluginFullscreen = val; }
    bool isPluginFullscreen() { return m_pluginFullscreen; }
#if defined(BRDO_LIW_FF)
    RLibrary& getDeviceBridgeLibL();
#endif     
    RLibrary& getWidgetEngineBridgeLibL();
    RenderTheme* theme();
    
    virtual ~StaticObjectsContainer();

    void ref(CBrCtl& brctl);
    void deref(CBrCtl& brctl);
    void setCapabilities (unsigned int capabilities)    {m_capabilities = capabilities;}
    unsigned int capabilities() {return m_capabilities;}
    CBrCtl* brctl() const;
    const Vector<CBrCtl*>& activeBrowserControls() const;
    void setFullScreenMode(bool mode);
    bool fullScreenMode(void);
    
private:
    static StaticObjectsContainer* gInstance;
    StaticObjectsContainer();

    PlatformFontCache*      m_fontCache;
    FormFillController*     m_formFillController;
    TScreenResolution       m_screenRes;
    PictographSymbian*      m_pictograph;
    ResourceLoaderDelegate* m_resourceLoaderDelegate;
    LogStream*              m_stream;
    WebIconDatabase*        m_icondatabase;
    WebSurface*             m_rendertarget;
    WebCursor*              m_cursor;
    WebCannedImages*        m_cannedimg;
    PluginHandler*          m_pluginhandler;
    int                     m_refcount;
    unsigned int            m_capabilities;
    Vector<CBrCtl*>         m_activeBrCtls;
    OOMHandler*             m_oomHandler;
    OOMStopper*             m_oomStopper;
    bool                    m_fullScreenMode;
    bool                    m_pluginFullscreen;
    RLibrary                m_widgetLibrary;
#if defined(BRDO_LIW_FF)
    RLibrary                m_deviceLibrary;
#endif
		RenderThemeSymbian*			m_symbianTheme;
};

}

#endif //!__STATICOBJECTCONTAINER_H__
