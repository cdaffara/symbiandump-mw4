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

//INCLUDES
#include <Browser_platform_variant.hrh>
#include "../../bidi.h"
#include "PlatformString.h"
#include <centralrepository.h>
#include <BrowserUiSDKCRKeys.h>
#include "PluginSkin.h"
#include "PluginWin.h"
#include "PluginHandler.h"
#include "PluginStreamLoaderClient.h"
#include "PluginStream.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebView.h"
#include "WebPageFullScreenHandler.h"
#include "BrCtl.h"
#include "WebCoreGraphicsContext.h"
#include "StaticObjectsContainer.h"
#include "BrCtlDefs.h"
#include "SettingsContainer.h"
#include <uri8.h>
#include <StringLoader.h>
#include <Webkit.rsg>
#include "Frame.h"
#include "FrameLoader.h"
#include "DocumentLoader.h"
#include "ResourceRequest.h"
#include "Widget.h"
#include "PlatformScrollbar.h"

#include <ApEngineConsts.h>
#include <uri8.h>
#include <InternetConnectionManager.h>
#include <es_enum.h>
#include <TextEncoding.h>
#include "CString.h"
#include "WidgetExtension.h"
#include <WidgetRegistryClient.h>

// CONSTANTS
using namespace WebCore;
// scheme for RTSP url
_LIT8(KRtsp, "rtsp");
_LIT(KAccessPtId, "accesspointid");
_LIT(KContentType, "contenttype");
_LIT(KContentTypeFlash, "flash");
_LIT(KContentTypeUnknown, "application/octet-stream");
_LIT(KRequestContentType, "Content-Type:");
_LIT(KRequestEOF, "\r\n");
_LIT(KRequestEOH, "\r\n\r\n");

// The default placeholder width, matches the value set in html4.css
const TUint KPlaceholderWidth( 100 );
const TUint KPlaceholderHeight( 50 );

// The Padding between object and border, matches the value in html4.css
const TUint KHtml4cssPadding( 1 );

// The border is grow/shrink by this much when the focus changes
const TUint KBorderWidthUnfocus( 0 );
const TUint KBorderWidthFocus( KHtml4cssPadding );

_LIT(KBrowserMode,"BrowserMode");
_LIT(KWidgetMode,"WidgetMode");
_LIT(KUnknownMode, "UnKnownMode");
_LIT(KApplicationId,"ApplicationId");
_LIT(KSecMgrScriptSession,"SecMgrScriptSession");
_LIT(KAllowNetworkAccess,"AllowNetworkAccess");


#define KArraySize 3

const TInt32 KWidgetApplicationId =  0x10282822;
const TInt32 KBrowserApplicationId = 0x10008D39;

//MACROS

//DATATYPES
// LOCAL FUNCTION PROTOTYPES
TInt RunScriptCb( TAny* aPtr );
// ----------------------------------------------------------------------------
// makeAbsoluteUrlL()
// return an absolute url that results from refUrl being resolved against 
// baseUrl.
// ----------------------------------------------------------------------------
HBufC8* makeAbsoluteUrlL(const TDesC8& baseUrl, const TDesC8& refUrl)
{
    TUriParser8 baseUrlparser;
    baseUrlparser.Parse(baseUrl); 
    TUriParser8 refUrlparser;
    refUrlparser.Parse(refUrl); 

    CUri8* absoluteUrl = CUri8::ResolveL(baseUrlparser, refUrlparser);
    CleanupStack::PushL(absoluteUrl);

    HBufC8* absoluteUrlBuf = HBufC8::NewL(absoluteUrl->Uri().UriDes().Length());
    TPtr8 absoluteUrlDes = absoluteUrlBuf->Des();
    absoluteUrlDes.Copy(absoluteUrl->Uri().UriDes());

    CleanupStack::PopAndDestroy(absoluteUrl);

    return absoluteUrlBuf;
}

// ----------------------------------------------------------------------------
// PluginSkin::NewL()
// Two-phased constructor for PluginSkin
// ----------------------------------------------------------------------------
PluginSkin* PluginSkin::NewL( WebFrame& frame,
                                     const TPtrC8& url,
                                     const Vector<WebCore::String>& attributesNames,
                                     const Vector<WebCore::String>& attributeValues,
                                     const TPtrC8& baseUrl,
                                     const WebCore::String& mimeType,
                                     TBool canInteract )
    {
    PluginSkin* self = NULL;
    
    //Check if the flash plugin is enabled or disabled.
    self = new (ELeave) PluginSkin( frame );
    CleanupStack::PushL( self );
    self->ConstructL( mimeType, attributesNames, attributeValues,
                      url, baseUrl, canInteract );
    CleanupStack::Pop();    // self
    return self;
    }

// ----------------------------------------------------------------------------
// PluginSkin::PluginSkin()
// Default constructor for PluginSkin
// ----------------------------------------------------------------------------
PluginSkin::PluginSkin( WebFrame& frame )
    : m_pluginwin(0),
      m_frame( &frame ),
      m_visible( EFalse ),
      m_pluginSupported( EFalse ),
      m_pluginClosed(false),
      m_canInteract( EFalse ),
      m_rect(TRect(0,0,0,0)),
      m_ref(1),
      m_handle(-1),
      m_instance(0),    
      m_pluginfuncs(0),
      m_resized(false),
      m_oldRect(TRect(0,0,0,0)),
      m_oldViewport(TRect(0,0,0,0))
  {
  }


void PluginSkin::addWidgetAttributesL()
{
    const TDesC& SecMgrScriptSession = KSecMgrScriptSession();
    RWidgetRegistryClientSession widgetregistry;
    TInt ret = widgetregistry.Connect();
    if ( ret != KErrNone && ret != KErrAlreadyExists ) { 
        User::Leave( ret );
    }
    else {
        CleanupClosePushL( widgetregistry );
    }
    
    CBrCtl*   brCtl = control(this->frame());
    WebView*  view = brCtl->webView();
    CWidgetExtension* wdgtExt = view->widgetExtension();

    if(wdgtExt){

#if defined(BRDO_LIW_FF)
        void* scriptSession =  wdgtExt->getSecuritySession();
        if ( !scriptSession ) {
            User::Leave( KErrGeneral );
        }
        
        NPN_GenericElement ScriptSession(SecMgrScriptSession,scriptSession);
        iGenericElementArray->AppendL(ScriptSession);           
#endif

        TInt uid = wdgtExt->GetWidgetId();         
        CWidgetPropertyValue* AccessValue = widgetregistry.GetWidgetPropertyValueL(TUid::Uid(uid), EAllowNetworkAccess ); 
        TInt networkAccess = *AccessValue;
        const TDesC& allowNetworkAccess = KAllowNetworkAccess();
        NPN_GenericElement NetworkAccess(allowNetworkAccess,networkAccess);
        iGenericElementArray->AppendL(NetworkAccess);
    }
    User::LeaveIfError(widgetregistry.Disconnect());
    CleanupStack::PopAndDestroy(); //widgetregistry
}

const TDesC& PluginSkin::GetExecutionMode()
{
    TUid uid  = RProcess().SecureId();
    if( uid.iUid == KWidgetApplicationId ){
        return KWidgetMode;
    }
    else if( uid.iUid == KBrowserApplicationId ){
        return KBrowserMode;
    }   
    return KUnknownMode;
 }
 
 void PluginSkin::setupGenericElementArrrayL()
 {
    iGenericElementArray = new (ELeave) RArray<NPN_GenericElement> (KArraySize);

    const TDesC& appId = KApplicationId();
    const TDesC& executionMode = GetExecutionMode();

    NPN_GenericElement ExecutionModeElement(appId,executionMode);
    iGenericElementArray->AppendL(ExecutionModeElement);

    if(0 == executionMode.Compare(KWidgetMode())) {
        addWidgetAttributesL();
    }
        
 }
 
// ----------------------------------------------------------------------------
// PluginSkin::ConstructL()
// Second phase constructor for PluginSkin
// ----------------------------------------------------------------------------
void PluginSkin::ConstructL( const WebCore::String& mimeType,
                             const Vector<WebCore::String>& attributesNames,
                             const Vector<WebCore::String>& attributeValues,
                             const TPtrC8& url,
                             const TPtrC8& baseUrl,
                             TBool canInteract )
    {
    // Get the canned object images
    m_cannedimg = WebCore::StaticObjectsContainer::instance()->webCannedImages()->getImage( WebCannedImages::EImageObject );

    TInt value( 0 );
    CRepository* rep = CRepository::NewL( KCRUidBrowser );
    rep->Get( KBrowserUsBuild, value );
    delete rep;

    if ( !value || canInteract )
        {
        m_canInteract = ETrue;
        }
        
    // Create the plugin if supported by platform
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    TPtrC mimeTypePtr;
    if (mimeType.length() > 0) {
        mimeTypePtr.Set(mimeType.des());
        m_pluginSupported = pluginHandler->isSupported( mimeTypePtr, url );
    }
    else {
        HBufC* ptr = pluginHandler->pluginMimeByExtention(url);
        if (ptr) {
            mimeTypePtr.Set(ptr->Des());
        }
        m_pluginSupported = ((ptr != NULL) && (mimeTypePtr.Length() > 0));
    }

    if ( m_pluginSupported ) {
        
        m_attributeNames = new (ELeave) CDesCArrayFlat(attributesNames.size());
        m_attributeValues = new (ELeave) CDesCArrayFlat(attributeValues.size());

        for (TInt i = 0; i < attributesNames.size(); i++)
                { m_attributeNames->AppendL(attributesNames[i].des());}
        for (TInt i = 0; i < attributeValues.size(); i++)
                { m_attributeValues->AppendL(attributeValues[i].des());}

        m_attributeNames->AppendL( KContentType() );
        m_attributeValues->AppendL( mimeType.des());
        
        // Append the browser access point id.
        // The access point id could be used by plugins to make open up a connection.
        m_attributeNames->AppendL( KAccessPtId );
        // Convert int value to string and append it
        TBuf16<4> value;
        value.Format( _L("%d"), m_frame->frameView()->topView()->accessPointId() );
        m_attributeValues->AppendL( value );      
        
        setupGenericElementArrrayL();

        if(url.Length())
            m_url = url.AllocL();
        bool loadPluginContent = m_frame->frameView()->topView()->brCtl()->settings()->brctlSetting(TBrCtlDefs::ESettingsAutoLoadImages);
        if((mimeTypePtr.Find(KContentTypeFlash) != KErrNotFound)) {
            loadPluginContent = loadPluginContent && !m_frame->frameView()->topView()->brCtl()->settings()->brctlSetting(TBrCtlDefs::ESettingsDisableFlash);
            m_flashContent = ETrue;
        }
        else {
            m_flashContent = EFalse;           
        }
        if (loadPluginContent) {
            if (isBrowserScheme(url)) {            
                NetscapePlugInStreamLoaderClient* pluginloader = NetscapePlugInStreamLoaderClient::NewL(url, this, core(m_frame));
                if (pluginloader) {
                    pluginloader->start();                            
                }                                                
            }
            else {
                TPtrC8 urlPtr(url);
                HBufC8* mime = NULL;
                mime = HBufC8::NewLC(mimeTypePtr.Length());
                mime->Des().Copy(mimeTypePtr);
                createPluginWinL(urlPtr, mimeTypePtr);
                loadPluginL(*mime);
                CleanupStack::PopAndDestroy(); // mime
            }
        }                
    }   // end of if (iIsPluginSupported)
    else {
        if(url.Length())
            m_url = url.AllocL();
    }

    // register itself
    MemoryManager::AddCollector(this);
    iJavascriptTimer = NULL;
}

void PluginSkin::addPluginStream(PluginStream* stream)
{
    m_streams.add(stream);
}

void PluginSkin::removePluginStream(PluginStream* stream)
{
    m_streams.remove(stream);
}

// ----------------------------------------------------------------------------
// PluginSkin::~PluginSkin()
// PluginSkin class Destructor
// ----------------------------------------------------------------------------
PluginSkin::~PluginSkin()
    {
    MemoryManager::RemoveCollector(this);
    Close();
    
    RFs& rfs = StaticObjectsContainer::instance()->fsSession();
    for(TInt i=0; i < m_tempFilesArray.Count(); i++)
        {
          rfs.Delete(m_tempFilesArray[i]->Des());
        }
    
    m_tempFilesArray.ResetAndDestroy();
    if (iGenericElementArray){
        iGenericElementArray->Close();
        delete iGenericElementArray;
	}
    }

void PluginSkin::Close()
{    
    // close any pending stream
    Vector<PluginStream*> streams;
    for (HashSet<PluginStream*>::iterator it = m_streams.begin(); it != m_streams.end(); ++it) {
        streams.append(*it);
    }    
    for (int i=0; i<streams.size(); ++i) {
        streams[i]->close();
    }
    m_streams.clear();

    if (m_instance && m_pluginfuncs && m_pluginfuncs->destroy) {        
        m_pluginfuncs->destroy(m_instance, NULL);
    }    
    
    if (m_handle >= 0) {
        PluginHandler* pluginhandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
        if ( pluginhandler) {
            pluginhandler->unloadPlugin( m_handle );
            pluginhandler->removePluginObject(this);
            if (this == pluginhandler->activePlugin()) {
                pluginhandler->setActivePlugin(NULL);
            }
        }
        m_handle = -1;
    }
    
    User::Free(m_instance); m_instance = 0;
    delete m_pluginwin; m_pluginwin = 0;
    delete m_attributeNames; m_attributeNames = 0;
    delete m_attributeValues; m_attributeValues = 0;
    delete m_url; m_url = 0;
    delete iJavascriptTimer; iJavascriptTimer = 0;
    m_pluginfuncs = 0;
    m_pluginSupported = EFalse;
    m_pluginClosed = true;
 }


// ----------------------------------------------------------------------------
// PluginSkin::pluginFocusChanged()
// PluginSkin function to show/hide fullscreen sprite
// ----------------------------------------------------------------------------
void PluginSkin::pluginFocusChanged(TBool focus) 
{ 
    if (control(m_frame)->webView()->pageFullScreenHandler() && 
        !control(m_frame)->webView()->pageFullScreenHandler()->isFullScreenMode()) 
            return;
    
    if (focus && !m_flashContent)
        {
        TRAP_IGNORE(
        control(m_frame)->webView()->LeaveFullscreenBrowsingL();
        control(m_frame)->webView()->notifyFullscreenModeChangeL( EFalse );
        );
        }
    else if (focus)
        {
        TRAP_IGNORE(
        control(m_frame)->webView()->pageFullScreenHandler()->showEscBtnL();
        );
        }
    else 
        {
        TRAP_IGNORE(
        control(m_frame)->webView()->pageFullScreenHandler()->hideEscBtnL();
        );
        }
}


// ----------------------------------------------------------------------------
// PluginSkin::Draw
// From MWebCoreObjectWidget
//
// The "focus" border width of this widget is adjusted to show when it has
// focus.  The border width is grown to KBorderWidthFocus. The default padding
// (set in Html4.css) is set up to handle the focus border growing into it.
// ----------------------------------------------------------------------------
void PluginSkin::draw( WebCoreGraphicsContext& context,
                        const TRect& /*rect*/ )
    {
    if(!control(m_frame)->webView()->IsVisible())
        return;

    TRect newRect(m_rect);
    TInt z(context.view().scalingFactor());
    if (z != 100 )
        {
        newRect = m_frame->frameView()->toViewCoords(m_rect);
        }
    CFbsBitGc* gc = &context.gc();
    gc->SetPenStyle( CGraphicsContext::ENullPen );
    gc->SetBrushStyle( CGraphicsContext::ENullBrush );

    CFbsBitmap* bitmap = m_cannedimg.m_img;
    TSize bmpSize( bitmap->SizeInPixels() );
    if ( !m_pluginwin && newRect.Height() >= bmpSize.iHeight && newRect.Width() >= bmpSize.iWidth )
        {
        // The inner rect is big enough, draw the placeholder image
        TPoint bitmapStartPoint( newRect.Center() );
        bitmapStartPoint -= TPoint(bmpSize.iWidth/2,bmpSize.iHeight/2);
        gc->BitBltMasked( bitmapStartPoint, m_cannedimg.m_img,
                        bmpSize, m_cannedimg.m_msk, ETrue );
        }
    if(m_pluginwin)
        {
        positionChanged();
        // Force the control to be redrawn.
        m_pluginwin->refreshPlugin(*gc,newRect);
        }
    }

// ----------------------------------------------------------------------------
// PluginSkin::IsVisible
// From MWebCoreObjectWidget
// ----------------------------------------------------------------------------
TBool PluginSkin::isVisible() const
    {
    return m_visible;
    }

// ----------------------------------------------------------------------------
// PluginSkin::SizeHint
// From MWebCoreObjectWidget
// ----------------------------------------------------------------------------
TSize PluginSkin::sizeHint() const
    {
    return TSize( KPlaceholderWidth, KPlaceholderHeight );
    }

// ----------------------------------------------------------------------------
// PluginSkin::MakeVisible
// From MWebCoreObjectWidget
// ----------------------------------------------------------------------------
void PluginSkin::makeVisible( TBool visible )
    {
    if ( m_pluginwin )
        {
        m_visible = visible;
        if(m_active)
            deActivate();
        m_pluginwin->makeVisible(visible);
        }
    }

// ----------------------------------------------------------------------------
// PluginSkin::SetFocus
// From MWebCoreObjectWidget
// ----------------------------------------------------------------------------
void PluginSkin::setFocus( TBool focus )
   {
   m_focused = focus;
   }

// ----------------------------------------------------------------------------
// PluginSkin::deActivate
// Invoked when the user selects the control
// ----------------------------------------------------------------------------
void PluginSkin::deActivate()
    {
    TBool consumed(EFalse);
    if (m_pluginwin )
        {
        TRAP_IGNORE( m_pluginwin->processEventL(EEventDeactivate, consumed));
        }

    if (consumed)
        {
        m_active = EFalse;
        m_frame->frameView()->topView()->setFocusedElementType(TBrCtlDefs::EElementObjectBox);
        // Set right soft key
        m_frame->frameView()->topView()->brCtl()->updateDefaultSoftkeys();
        }
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    pluginHandler->setPluginToActivate(NULL);
    pluginHandler->setActivePlugin(NULL);
    }

// ----------------------------------------------------------------------------
// PluginSkin::activate
// From MWebCoreObjectWidget
// Invoked when the user selects the right key (Cancel)
// ----------------------------------------------------------------------------
void PluginSkin::activate()
{
    if (m_pluginClosed)
        return;

    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    TBool consumed(EFalse);
    if(m_pluginwin)
    {
        if(m_canInteract)
            {
            TRAP_IGNORE( m_pluginwin->processEventL(EEventActivate, consumed));
            if (consumed)
                {
                m_active = ETrue;
                m_frame->frameView()->topView()->setFocusedElementType(TBrCtlDefs::EElementActivatedObjectBox);
                pluginHandler->setActivePlugin(this);
                pluginHandler->setPluginToActivate(NULL);
                }
            // Set right soft key
            m_frame->frameView()->topView()->brCtl()->updateDefaultSoftkeys();
            }
        else
            {
            m_active = ETrue;
            pluginHandler->setActivePlugin(this);
            TRAP_IGNORE( m_frame->frameView()->topView()->openPluginPlayerL(m_pluginwin));
            consumed = ETrue;
            m_active = EFalse;
            pluginHandler->setActivePlugin(NULL);
            }
    } else {
        if ( m_pluginSupported ) {
            NetscapePlugInStreamLoaderClient* pluginloader = NULL;
            TRAP_IGNORE( pluginloader = NetscapePlugInStreamLoaderClient::NewL(*m_url, this, core(m_frame)));
            if (pluginloader) {
                pluginloader->start();                            
            }                                                            
        }
        else {
            if (m_url) {
                // let the download manager handle this. non-native top level load
                mainFrame(m_frame)->loadURL(*m_url, TBrCtlDefs::ECacheModeNoCache, String());
            }
        }
    }
}

// ----------------------------------------------------------------------------
// PluginSkin::Rect
// From MWebCoreObjectWidget
// ----------------------------------------------------------------------------
TRect PluginSkin::rect() const
    {
    return m_rect;
    }

// ----------------------------------------------------------------------------
// PluginSkin::SetRect
// From MWebCoreObjectWidget
// ----------------------------------------------------------------------------
void PluginSkin::setRect(const TRect& rect)
    {
    m_rect = rect;
    }

// ----------------------------------------------------------------------------
// PluginSkin::SetFont
// From MWebCoreObjectWidget
// ----------------------------------------------------------------------------
void PluginSkin::setFont(CFont* font)
    {
    // Use the font set by our widget, when graphicsContext.UseFont() is called
    // the previous font is cleaned up.
    m_font = font;
    }


// -----------------------------------------------------------------------------
// PluginSkin::IsFocusable
//
// -----------------------------------------------------------------------------
//
TBool PluginSkin::isFocusable() const
    {
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// PluginSkin::loadPlugin
//
// Load the plugin.
// -----------------------------------------------------------------------------
void PluginSkin::loadPluginL( const TDesC8& mimetype )
{
 
    if (!m_instance) {
        
        NPError error( NPERR_NO_ERROR );
        
        m_instance = (NPP) User::AllocL(sizeof(NPP_t));
        m_instance->ndata = pluginWin();
        m_instance->pdata = NULL;

        if (m_instance) {
            
            PluginHandler* pluginhandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
            if ( pluginhandler ) {
                pluginhandler->loadPluginL( m_handle, &m_pluginfuncs );
        
                if (m_pluginfuncs && m_pluginfuncs->newp) {        

                    error = m_pluginfuncs->newp( TPtrC8(mimetype), 
                                                    m_instance, 
                                                    NP_EMBED,
                                                    m_attributeNames,
                                                    m_attributeValues, 
                                                    NULL );
                                                                      
                }
                if (m_pluginwin) {
                    m_pluginwin->ConstructL(*(control(m_frame)->webView()));
            }
        }
        }

        switch ( error ) {
            case NPERR_OUT_OF_MEMORY_ERROR: {
                User::Leave( KErrNoMemory );
                break;
            }
            case NPERR_GENERIC_ERROR: {
                User::Leave( KErrNotSupported );
                break;
            }
        }    
        
        if (m_pluginwin)
        m_pluginwin->SetExtent( TPoint(0,0), TSize(0,0) );
            
    }

}

// -----------------------------------------------------------------------------
// PluginSkin::CreatePluginWinL
//
// Initializes a new PluginWin
// -----------------------------------------------------------------------------
//
void PluginSkin::createPluginWinL(TDesC8& url, TDesC& mimetype)
{
    if (m_pluginwin)
        return;
    // The aLoadMode is not currently used.  It would be used for checking the
    // load mode and determining if the plugin should be deleted and recreated.
    // Get plugin handler, so we can find a plugin handle for a plugin that
    // supports the incoming content (by mimetype or extension)
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();     
    m_handle = pluginHandler->findPlugin( mimetype );        
    if ( m_handle == KErrNotFound ) { 
        TUriParser8 parser;   
        if ( parser.Parse(url) == KErrNone ) {
            m_handle = pluginHandler->findPluginByExtension( parser.Extract(EUriPath ) );        	
        }   
    }
    if (m_handle == KErrNotFound)
        User::Leave(KErrNotSupported);
    //Set the cliprect to plugwin
    TRect clipRect = TRect(m_rect.iTl, m_frame->frameView()->toDocCoords(m_rect.Size()));
    clipRect.Intersection(m_rect);
    setClipRect(clipRect);
    // Create the PluginWin, if it doesn't exist,
    m_pluginSupported = true;
    m_pluginwin = PluginWin::NewL(this, *m_frame->frameView()->topView());
    pluginHandler->storePluginObject(this);
    m_frame->frameView()->invalidateRect(m_rect, EFalse);    
}



// -----------------------------------------------------------------------------
// PluginSkin::PositionChanged
// From MViewFocusObserver
// Callback from the view when the position changes
// -----------------------------------------------------------------------------
void PluginSkin::positionChanged()
{
    // If the plugin content hasnt arrived yet or plugin is invalid, then return immediately
    if ( !m_pluginwin ) {
        return;
    }
    TPoint tl = m_frame->frameView()->frameCoordsInViewCoords(m_rect.iTl);
    TPoint br = m_frame->frameView()->frameCoordsInViewCoords(m_rect.iBr);      
    TRect rect = TRect(tl, br);
    TRect newViewport = m_frame->frameView()->topView()->DocumentViewport();
    
    if (m_oldRect != rect || m_oldViewport != newViewport) {
        m_oldRect = rect;
        m_oldViewport = newViewport;
        TRect clipRect(frameVisibleRect());
        clipRect.Intersection(m_rect);
        setClipRect(clipRect);
        setPluginWinClipedRect();
    }
}
  
//-------------------------------------------------------------------------------
// RunScriptCb
// C-style TCallback function
//-------------------------------------------------------------------------------
TInt RunScriptCb(TAny* aPtr )
{
    static_cast<PluginSkin*>(aPtr)->RunScript();
    return EFalse;
}


// -----------------------------------------------------------------------------
// PluginSkin::forceRedraw
//
//
// -----------------------------------------------------------------------------
void PluginSkin::forceRedraw(bool drawNow)
{
    if (m_frame && m_frame->frameView())
        m_frame->frameView()->invalidateRect(m_rect, drawNow);
}

// -----------------------------------------------------------------------------
// PluginSkin::RunScript
//
// Runs java script, possibly asynchronously
// -----------------------------------------------------------------------------
//
TBool PluginSkin::RunScript()
{
    _LIT8(KJs, "javascript:");
    
    for(int i=0;i<m_JSUrls.Count();i++) {
       TPtr16 temp = ((HBufC*)m_JSUrls[i])->Des(); 
	   core(mainFrame(m_frame))->loader()->executeScript(String(temp.Mid(KJs().Length())));
    }
    m_JSUrls.ResetAndDestroy();
    return EFalse;
}

//callbacks from NpnImplementation  
int PluginSkin::getRequestL(const TDesC8& url, bool notify, void* notifydata,const TDesC* aWindowType)
{
    TPluginLoadMode loadmode = GetLoadMode(aWindowType);

    if (url.Ptr() == NULL ) {                        
        return KErrArgument;
    }

    _LIT8(KJs, "javascript:");
    if ((url.Length() > KJs().Length() ) &&(url.Left(KJs().Length()).FindF(KJs) == 0)) {
        HBufC* pBuffer = HBufC::NewL(url.Length());
	    TPtr16 temp = pBuffer->Des(); 
	    temp.Copy(url);
		m_JSUrls.Append(pBuffer);
		if(!iJavascriptTimer){
		   iJavascriptTimer = CIdle::NewL(CActive::EPriorityLow);	
		}
		if( iJavascriptTimer && !iJavascriptTimer->IsActive() ){
    	   iJavascriptTimer->Start(TCallBack(&RunScriptCb, this));
         }
        return KErrNone;
     }

    // make sure it is an absolute URL
    HBufC8* absoluteUrl = makeAbsoluteUrlL(*m_url, url); 
    CleanupStack::PushL(absoluteUrl);

    if (loadmode == ELoadModePlugin ) {    
        
        if (m_instance && m_pluginfuncs) {
        
            NetscapePlugInStreamLoaderClient* pluginloader = NetscapePlugInStreamLoaderClient::NewL(url, this, core(m_frame), notifydata);
            if (pluginloader) {
                pluginloader->start();                            

                if ( notify ) {
                    HBufC* url16 = HBufC::NewLC( url.Length() );
                    url16->Des().Copy( url );
                    m_pluginfuncs->urlnotify( m_instance, *url16, NPRES_DONE, notifydata );        
                    CleanupStack::PopAndDestroy(url16);
                }
                
            }                
        }                 
    }
    else {    
        HBufC* windowType = HBufC::NewLC(aWindowType->Length());
        windowType->Des().Copy(*aWindowType);
        frame()->loadURL(*absoluteUrl,TBrCtlDefs::ECacheModeNormal,String(),&String(*windowType));
        CleanupStack::PopAndDestroy(windowType);
    }

    CleanupStack::PopAndDestroy(absoluteUrl);

    return KErrNone;
}

int PluginSkin::postRequestL(const TDesC8& url,const TDesC& buffer, bool fromfile, bool notify, void* notifydata,const TDesC* aWindowType)
{
    // make sure it is an absolute URL
    HBufC8* absoluteUrl = makeAbsoluteUrlL(*m_url, url);     
    CleanupStack::PushL(absoluteUrl);
    TPluginLoadMode loadmode = GetLoadMode(aWindowType);
 
    ResourceRequest request = (KURL(*absoluteUrl));

    request.setMainLoad(true);
    request.setCachePolicy(UseProtocolCachePolicy);
    request.setHTTPMethod("POST");      
    
    int contenttype_pos = buffer.Find(KRequestContentType());    
    if (contenttype_pos != KErrNotFound) {
        contenttype_pos += KRequestContentType().Length();
        int contenttype_length = buffer.Mid(contenttype_pos).Find(KRequestEOF());
        
        if (contenttype_length != KErrNotFound) {                                    
            request.setHTTPContentType(buffer.Mid(contenttype_pos,contenttype_length));                                
        } else {
            request.setHTTPContentType(KContentTypeUnknown());                                
        }
            
    } else {
        request.setHTTPContentType(KContentTypeUnknown());                                        
    }
    
    
    if ( fromfile ) {

        RFs& rfs = StaticObjectsContainer::instance()->fsSession();
        RFile  file;
        
        User::LeaveIfError(file.Open( rfs, buffer, EFileRead));
        CleanupClosePushL(file);
        
        // Get the file size and allocate a buffer
        TInt size(0);
        User::LeaveIfError( file.Size(size) );
        HBufC8* body = HBufC8::NewLC(size);        
        TPtr8 tptr8(body->Des());
        
        User::LeaveIfError(file.Read(tptr8));       
        FormData* fd = new (ELeave) FormData(body->Ptr(),body->Length());                                          
        request.setHTTPBody(fd);                            
        CleanupStack::PopAndDestroy(2); // file, body
    }    
    else{        
        
        int start_content = buffer.Find(KRequestEOH());    
        start_content =  (start_content != KErrNotFound) ? start_content+ KRequestEOH().Length() : 0;                
        
        HBufC* body = HBufC::NewLC(buffer.Mid(start_content).Length()+1);                
        body->Des().Copy(buffer.Mid(start_content));
        TextEncoding *ecoder = new TextEncoding(core(mainFrame(m_frame))->loader()->encoding());
        CString decoded_body = ecoder->encode(body->Des().PtrZ(),body->Length());
        FormData* fd = new (ELeave) FormData(decoded_body.data(),decoded_body.length());                                               
        request.setHTTPBody(fd);                                              
        CleanupStack::PopAndDestroy(); // body
    }
                
        
    if (loadmode == ELoadModePlugin ) {    
                        
        if (m_instance && m_pluginfuncs) {
            NetscapePlugInStreamLoaderClient* pluginloader = NetscapePlugInStreamLoaderClient::NewL(request, this, core(m_frame), notifydata);
            if (pluginloader) {
                pluginloader->start();                            

                if ( notify ) {
                    HBufC* url16 = HBufC::NewLC( url.Length() );
                    url16->Des().Copy( url );
                    m_pluginfuncs->urlnotify( m_instance, *url16, NPRES_DONE, notifydata );        
                    CleanupStack::PopAndDestroy(url16);
                }
                
            }                
        }                 
    }
    else {
        HBufC* windowType = HBufC::NewLC(aWindowType->Length());
        windowType->Des().Copy(*aWindowType);
        frame()->loadRequest(request,&String(*windowType));
        CleanupStack::PopAndDestroy(windowType);                   
    }
    

    CleanupStack::PopAndDestroy(absoluteUrl);

    return KErrNone;
}


bool PluginSkin::isInteractive()
{
        
    if (m_instance && m_pluginfuncs) {
        
        bool isNotInteractive = false; 
        NPError err = m_pluginfuncs->getvalue(m_instance, NPPVpluginInteractiveBool, &isNotInteractive);
        if( err == NPERR_NO_ERROR) {
            return !isNotInteractive;
        }

    }
    
    return false;
}

void PluginSkin::openInViewerL()
{
    if(m_pluginwin)
        {
        m_frame->frameView()->topView()->mainFrame()->loadURL(*m_url, TBrCtlDefs::ECacheModeNormal, String());
        }
}
    
/*
// -----------------------------------------------------------------------------
// PluginSkin::HandlePluginError
//
// handles error conditions
// -----------------------------------------------------------------------------
//
void PluginSkin::handlePluginError( TInt error )
    {
    if ( error != KErrNone )
        {
        m_pluginSupported = EFalse;
        delete m_pluginwin;
        m_pluginwin = 0;
        }
    }

*/

void* PluginSkin::pluginScriptableObject()
{
    //
    if (m_pluginfuncs && m_pluginfuncs->getvalue) {
        void *value = 0;
        NPError npErr = m_pluginfuncs->getvalue( m_instance, NPPVpluginScriptableNPObject, (void *)&value);
        if (npErr == NPERR_NO_ERROR) {
            return value;
        }
    }
    return (void *)0;
}

TBool PluginSkin::isBrowserScheme(const TPtrC8& url)
{
    TBool supported(EFalse);
    TUriParser8 parser;
    if( parser.Parse( url ) == KErrNone ) {
        TPtrC8 scheme = parser.Extract( EUriScheme );
        if (scheme.CompareF( _L8("http" ) ) == 0 || scheme.CompareF( _L8("https" ) ) == 0 
            || scheme.Length() == 1 || scheme.CompareF( _L8("file") ) == 0 || scheme.CompareF( _L8("data") ) == 0) {
            supported = ETrue;
        }
    }
    return supported;
}

void PluginSkin::resized()
{
    m_resized = true;
    m_frame->frameView()->topView()->forceLayoutAndResize(m_frame);
}

void PluginSkin::setClipRect(TRect rect) 
{
    m_pluginWinClipRect = rect;
}

void PluginSkin::setPluginWinClipedRect() 
{
    TRect fullRect(getPluginWinRect());
    TRect clipRect(getClipRect());
    TRect frameRect(m_frame->frameView()->rect());
    TRect viewRect = control(m_frame)->webView()->Rect();
    TBool isPageViewMode = control(m_frame)->webView()->inPageViewMode();
    WebFrame* pf = m_frame;
    TPoint p = frameRect.iTl;

    if (m_frame->parentFrame()) {
        pf = m_frame->parentFrame();
        p = pf->frameView()->frameCoordsInViewCoords(frameRect.iTl); 
    }
    TSize  sz = pf->frameView()->toViewCoords(frameRect.Size());
    TRect frameRectInViewCoord = TRect(p, sz);
    TBool isPluginVisible = frameRectInViewCoord.Intersects(fullRect); 
    TBool isFrameVisible = m_frame->frameView()->isVisible() && 
                           frameRectInViewCoord.Intersects(viewRect);
      
    if (m_pluginwin) {
        m_pluginwin->makeVisible( isFrameVisible && !isPageViewMode && isPluginVisible);
        if (!m_pluginwin->isPluginInFullscreen()) {
            clipRect.Intersection(fullRect);
            m_pluginwin->SetRect(clipRect);
        }
    }
}

TRect PluginSkin::getClipRect() const
{
    TPoint pt = m_frame->frameView()->convertContentToView(m_pluginWinClipRect.iTl);
    TRect clipRect(pt, m_frame->frameView()->toViewCoords(m_pluginWinClipRect.Size()));
    clipRect.Intersection(control(m_frame)->webView()->Rect());
    return clipRect;
}

TRect PluginSkin::getPluginWinRect() const
{
    TPoint pt = m_frame->frameView()->convertContentToView(m_rect.iTl);
    TRect rect(pt, m_frame->frameView()->toViewCoords(m_rect.Size()));
    return rect;
}

TRect PluginSkin::frameVisibleRect() const
{
    TRect rect = m_frame->frameView()->visibleRect();
    if (m_frame->isIframe()) {
        if (m_frame->frameView()->vScrollbar() && m_frame->frameView()->vScrollbar()->isEnabled()) {
            rect.SetWidth(rect.Width() - m_frame->frameView()->vScrollbar()->width());
        }
        if (m_frame->frameView()->hScrollbar() && m_frame->frameView()->hScrollbar()->isEnabled()) {
            rect.SetHeight(rect.Height() - m_frame->frameView()->hScrollbar()->height());
        }
    }
    return rect;
}


TUint PluginSkin::Collect(TUint aRequired)
{
    // out of memory, we need to remove the plugin win and all streams associating with it.
    Close();
    return 0;
}

TInt PluginSkin::handleNetworkAccess() const
{
    TInt conn = 0,sockHandle = 0;
    TBool connNeeded;
    TApBearerType type;
    TInt apId = -1;
            
    TRAP_IGNORE(m_frame->frameView()->topView()->brCtl()->brCtlSpecialLoadObserver()->NetworkConnectionNeededL(&conn,&sockHandle,&connNeeded,&type));
    
    if ( conn )
        {
        RConnection* connPtr = REINTERPRET_CAST( RConnection*, conn );
        TConnectionInfoBuf connInfoBuf;
        TUint conns( 0 );
        connPtr->EnumerateConnections( conns );
        if ( conns > 0  && connPtr->GetConnectionInfo(1, connInfoBuf) == KErrNone )
            {
            // set the access point id 
            apId = connInfoBuf().iIapId;
            }
        }
    
    return apId;
}

TPluginLoadMode PluginSkin::GetLoadMode(const TDesC* aWindowType)
{
    TWindowType windowType = GetWindowType(aWindowType);

    switch (windowType) {
        
        case EWindowTypePlugin:     // Return the load response to the plugin
            return ELoadModePlugin;

        case EWindowTypeBlank:      // New window is opened.
        case EWindowTypeNew:
            return ELoadModeNew;

        case EWindowTypeCurrent:    // Replace the plugin with a new document
        case EWindowTypeSelf:
            return ELoadModeSelf;

        case EWindowTypeParent:     // Replace the page with a new top level document
            return ELoadModeParent;
            
    	case EWindowTypeTop:
    	    return ELoadModeTop;

        default:
            return ELoadModeNone;
    }        
}


TWindowType PluginSkin::GetWindowType(const TDesC* aWindowType)
{

    if ((aWindowType == NULL) || 
        (aWindowType->Ptr() == NULL) ||
        (aWindowType->Length() == 0)) {
        
        return EWindowTypePlugin;
    }

    TPtrC windowTypePtr(*aWindowType);

    if (windowTypePtr.FindF(KBlank) == 0) {
        return EWindowTypeBlank;
    }
    
    if (windowTypePtr.FindF(KNew) == 0) {
        return EWindowTypeNew;
    }
    
    if (windowTypePtr.FindF(KSelf) == 0) {
        return EWindowTypeSelf;
    }
    
    if (windowTypePtr.FindF(KCurrent) == 0) {
        return EWindowTypeCurrent;
    }
        
    if (windowTypePtr.FindF(KParent) == 0) {
        return EWindowTypeParent;
    }
    
    if (windowTypePtr.FindF(KTop) == 0) {
        return EWindowTypeTop;
    }

    return EWindowTypeUnknown;
}
