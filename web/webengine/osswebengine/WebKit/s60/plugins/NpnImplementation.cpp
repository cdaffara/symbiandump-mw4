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
* Description:  Browser's Netscape API
*
*/

#include "config.h"
#include "../../bidi.h"
#include "HttpDefs.h"
#include "NpnImplementation.h"
#include "PluginWin.h"
#include "PluginSkin.h"
#include <CUserAgent.h>
#include <Element.h>
#include <HTMLPlugInElement.h>
#include <HTMLNames.h>

using namespace WebCore::HTMLNames;



// -----------------------------------------------------------------------------
// NpnUrlLoader
//
// Requests the associated pluginInst to load the given url.
// -----------------------------------------------------------------------------
//
NPError NpnUrlLoader(NPP aInstance, const TDesC& aUrl, TUint8 aMethod,
                     const TDesC* aWindowType, const TDesC& aBuf,
                     TBool aFromFile, TBool aNotify, void* aNotifyData)
{
      
    PluginWin* pluginWin = (PluginWin* )aInstance->ndata;
    if ( !pluginWin ) {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    TInt status = KErrNone;
    
    // Make the load request
        // convert to 8 bit
        HBufC8* url = HBufC8::NewLC(aUrl.Length());
        url->Des().Copy(aUrl);    
        if (aMethod==EUrlGet) {
            TRAP_IGNORE(status = pluginWin->pluginSkin()->getRequestL(*url, aNotify, aNotifyData, aWindowType));
        }
        else if (aMethod==EUrlPost) {
            TRAP_IGNORE(status = pluginWin->pluginSkin()->postRequestL(*url, aBuf, aFromFile, aNotify, aNotifyData, aWindowType));
        }

        CleanupStack::PopAndDestroy(url);

    // Convert to NPError
    switch (status) {
        case KErrNone:
            return NPERR_NO_ERROR;

        case KErrNoMemory:
            return NPERR_OUT_OF_MEMORY_ERROR;

        default:
            return NPERR_GENERIC_ERROR;
    }
    
}

// -----------------------------------------------------------------------------
// NpnGetUrl
//
// Requests the associated pluginInst to load the given url.
// -----------------------------------------------------------------------------
//
NPError NpnGetUrl(NPP aInstance, const TDesC& aUrl, const TDesC* aWindowType)
{
    TPtrC nullPtr(NULL, 0);
    return NpnUrlLoader(aInstance, aUrl, EUrlGet, aWindowType, nullPtr,
                        EFalse, EFalse, NULL);
}

// -----------------------------------------------------------------------------
// NpnPostUrl
//
// Requests the associated pluginInst to POST to the given url.
// -----------------------------------------------------------------------------
//
NPError NpnPostUrl(NPP aInstance, const TDesC& aUrl, const TDesC* aWindowType,
                    const TDesC& aBuf, NPBool aFile)
{
    return NpnUrlLoader(aInstance, aUrl, EUrlPost, aWindowType, aBuf,
                        aFile, EFalse, NULL);
}

// -----------------------------------------------------------------------------
// NpnRequestRead
//
// Unsupported.
// -----------------------------------------------------------------------------
//
NPError NpnRequestRead(NPStream* /*aStream*/, NPByteRange* /*aRangeList*/)
{
    // This function is not supported
    return NPERR_GENERIC_ERROR;
}

// -----------------------------------------------------------------------------
// NpnNewStream
//
// Unsupported.
// -----------------------------------------------------------------------------
//
NPError NpnNewStream(NPP /*aInstance*/, NPMIMEType /*aType*/,
                      const TDesC* /*aWindowType*/, NPStream** /*aStream*/)
{
    // This function is not supported.
    // It is specifically intended to create a new Browser window
    return NPERR_GENERIC_ERROR;
}

// -----------------------------------------------------------------------------
// NpnWrite
//
// Unsupported.
// -----------------------------------------------------------------------------
//
TInt32 NpnWrite(NPP /*aInstance*/, NPStream* /*aStream*/,
                TInt32 /*aLen*/, void* /*aBuffer*/)
{
    // This function is not supported.
    return NPERR_GENERIC_ERROR;
}

// -----------------------------------------------------------------------------
// NpnDestroyStream
//
// Unsupported.
// -----------------------------------------------------------------------------
//
NPError NpnDestroyStream(NPP /*aInstance*/, NPStream* /*aStream*/,
                        NPReason /*aReason*/)
{
    // This function is not supported.
    return NPERR_GENERIC_ERROR;
}

// -----------------------------------------------------------------------------
// NpnStatus
//
// Allows a plugin to report its current status to the associated PluginInst.
// -----------------------------------------------------------------------------
//
void NpnStatus(NPP aInstance, const TDesC& aMessage)
{
    // This function is not supported.    
}

// -----------------------------------------------------------------------------
// NpnUAgent
//
// Returns the user agent of the Series 60 Browser.
// -----------------------------------------------------------------------------
//
const TDesC* NpnUAgent(NPP /*aInstance*/)
{
    // Get User Agent String    
    CUserAgent* usrAgnt = CUserAgent::NewL();
    CleanupStack::PushL( usrAgnt );

    HBufC8* userAgent8 = usrAgnt->UserAgentL();
    CleanupStack::PushL( userAgent8 );

    HBufC* userAgent = HBufC::NewL(userAgent8->Length());
    userAgent->Des().Copy(userAgent8->Des());


    CleanupStack::PopAndDestroy(2);

    return userAgent;    
}

// -----------------------------------------------------------------------------
// NpnMemAlloc
//
// Allocates a chunk of memory on the behalf of the plugin.
// -----------------------------------------------------------------------------
//
 void* NpnMemAlloc(uint32 aSize)
{
    if (aSize) {
        return User::Alloc(aSize);
    }
    
    return NULL;
}

// -----------------------------------------------------------------------------
// NpnMemFree
//
// Deallocates a chunk of memory on the behalf of the plugin.
// -----------------------------------------------------------------------------
//
 void NpnMemFree(void* aPtr)
{
    User::Free(aPtr);
}

// -----------------------------------------------------------------------------
// NpnMemFlush
//
// Unsupported.
// -----------------------------------------------------------------------------
//
uint32 NpnMemFlush(uint32 /*aSize*/)
{
    // This function is not supported
    return NPERR_GENERIC_ERROR;
}

// -----------------------------------------------------------------------------
// NpnReloadPlugins
//
// Causes the PluginHandler to scan for new plugins and reloads the current
// page if aReloadPages is true.
// -----------------------------------------------------------------------------
//
 void NpnReloadPlugins(NPBool /*aReloadPages*/)
{
    //This is currently not supported since CPluginHandler is non static
}

// -----------------------------------------------------------------------------
// NpnGetJavaEnv
//
// Unsupported.
// -----------------------------------------------------------------------------
//
JRIEnv* NpnGetJavaEnv()
{
    // This function is not supported
    return NULL;
}

// -----------------------------------------------------------------------------
// NpnGetJavaPeer
//
// Unsupported.
// -----------------------------------------------------------------------------
//
jref NpnGetJavaPeer(NPP /*aInstance*/)
{
    // This function is not supported
    return NULL;
}

// -----------------------------------------------------------------------------
// NpnGetUrlNotify
//
// Requests the associated PluginInst to load the given url.  The plugin is
// notified when the request completes.
// -----------------------------------------------------------------------------
//
NPError NpnGetUrlNotify(NPP aInstance, const TDesC& aUrl,
                        const TDesC* aWindowType, void* aNotifyData)
{
    TPtrC nullPtr(NULL, 0);
    return NpnUrlLoader(aInstance, aUrl, EUrlGet, aWindowType, nullPtr,
                        EFalse, ETrue, aNotifyData);
                        
}

// -----------------------------------------------------------------------------
// NpnPostUrlNotify
//
// Requests the associated PluginInst to POST to the given url.  The plugin is
// notified when the request completes.
// -----------------------------------------------------------------------------
//
 NPError NpnPostUrlNotify(NPP aInstance, const TDesC& aUrl,
                          const TDesC* aWindowType, const TDesC& aBuf,
                          NPBool aFile, void* aNotifyData)
{
    return NpnUrlLoader(aInstance, aUrl, EUrlPost, aWindowType, aBuf,
                        aFile, ETrue, aNotifyData);
}

// -----------------------------------------------------------------------------
// NpnGetValue
//
// Query the associated PluginInst for information.
// -----------------------------------------------------------------------------
//
NPError NpnGetValue(NPP aInstance, NPNVariable aVariable, void *aRetValue)
{
    TInt err = NPERR_NO_ERROR;
    
    switch (aVariable) {
        
        case NPNVjavascriptEnabledBool: // Tells whether JavaScript is enabled; true=JavaScript enabled, false=not enabled
        // NEEDS IMPLEMENTATION
        //  *((TBool*) aRetValue) = NW_Settings_GetEcmaScriptEnabled();
        break;

        case NPNVnetscapeWindow: {         

            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            if (pluginWin) {
                NPWindow* npWindow = (NPWindow*)aRetValue;
                TRect rect = pluginWin->Rect();

                npWindow->x = rect.iTl.iX;
                npWindow->y = rect.iTl.iY;
                npWindow->width = rect.Width();
                npWindow->height = rect.Height();
                npWindow->type = pluginWin->Windowed() ? NPWindowTypeWindow : NPWindowTypeDrawable;
                npWindow->window = pluginWin->Windowed() ? NULL : (MPluginAdapter*) pluginWin;
            
                NPRect clipRect = {0,0,0,0};
                npWindow->clipRect = clipRect;
            }
        }
        break;
        
        case NPNVPluginElementNPObject: {
		PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
        WebCore::Element* pluginElement;
        if (pluginWin) {
        	pluginElement = pluginWin->pluginSkin()->getElement();
        }
            
        NPObject* pluginScriptObject = 0;
        if (pluginElement->hasTagName(appletTag) || pluginElement->hasTagName(embedTag) || pluginElement->hasTagName(objectTag))
			pluginScriptObject = static_cast<WebCore::HTMLPlugInElement*>(pluginElement)->getNPObject();
            
        if (pluginScriptObject)
        	_NPN_RetainObject(pluginScriptObject);

        void** v = (void**)aRetValue;
        *v = pluginScriptObject;
        }
        break;
        
        case NPNVWindowNPObject: {
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            NPObject* windowObject = pluginWin->windowScriptNPObject();
            if (windowObject) {
                void **v = (void **)aRetValue;
                *v = windowObject;
            }
            else {
                err = NPERR_GENERIC_ERROR;
            }
            break;
        }
        case NPNVDOMWindow:
        case NPNVxDisplay:          // Unix only: Returns the current Display
        case NPNVxtAppContext:      // Unix only: Returns the application's XtAppContext
        case NPNVasdEnabledBool:    // Tells whether SmartUpdate (former name: ASD) is enabled;
                                    // true=SmartUpdate enabled, false=not enabled
        case NPNVisOfflineBool:     // Tells whether offline mode is enabled;
                                    // true=offline mode enabled, false=not enabled
            
        case NPNNetworkAccess:
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            TInt apId = -1;
            if (pluginWin) {
                apId = pluginWin->pluginSkin()->handleNetworkAccess();
            }
            *((TInt*) aRetValue) = apId;
            break;
            
       case NPNVGenericParameter: {   
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            if (pluginWin) {
                void **v = (void **)aRetValue;
                *v = pluginWin->pluginSkin()->genericElementArray();
            }
            else {
                err = NPERR_GENERIC_ERROR;
            }
        }
        break;  // for code consistency
        
       case NPNVSupportsWindowless:{
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
               if (pluginWin) {
                   *((TBool*) aRetValue) = ETrue;
               }
               else {
                   err = NPERR_GENERIC_ERROR;
               }
       }
       break;		   
        default:
            {
            *((TBool*) aRetValue) = EFalse;
            err = NPERR_INVALID_PARAM;
            }
            break;
    }   // end of switch
    
    return err;

}

// -----------------------------------------------------------------------------
// NpnSetValue
//
// Set a value on the associated PluginInst
// -----------------------------------------------------------------------------
//
NPError NpnSetValue(NPP aInstance, NPPVariable aVariable, void* aSetValue)
{
    switch (aVariable) {
        // Set a new plugin window size. This will resize the plugin rect,
        // which requires a re-layout of the dom (render) tree.
        case NPPVpluginWindowSize: {
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            if (pluginWin) {
                NPWindow* npWindow = (NPWindow*)aSetValue;
                if (npWindow) {
                    TRect newRect(npWindow->x, npWindow->y, npWindow->width, npWindow->height);
                    pluginWin->resizePluginRect(newRect);
                }
            }
        }
        break;
        case NPPVpluginWindowBool:
          {
          PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
          if (pluginWin) {
              NPBool* isWindowed = (NPBool*)aSetValue;
              if (isWindowed)
                  pluginWin->setWindowedPlugin(*isWindowed);
              }
          
          }
        break;          
        case NPPVpluginTransparentBool:
            {
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            if (pluginWin) {
                NPBool* isTransparent = (NPBool*)aSetValue;
                if (isTransparent)
                    pluginWin->setTransparentPlugin(*isTransparent);
            }          
            }
        break;
        case NPPVPluginFocusPosition:
            {
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            if (pluginWin) {
                TPoint* focusPoint = (TPoint*) aSetValue;
                if (focusPoint) {
                    pluginWin->moveWindow(*focusPoint);
                }
            }
        }
        break;
        case NPPVPluginPointerEvent:
            {
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            if (pluginWin) {
                TPointerEvent* event = (TPointerEvent*)aSetValue;
                if (event) {
                    pluginWin->HandlePointerEventFromPluginL(*event);
                }
            }
            break;
            }
        case NPPVPluginDeactivate:
            {
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            if (pluginWin) {
                TPoint* cursorPos = static_cast<TPoint*>(aSetValue);
                pluginWin->pluginDeactivate(*cursorPos);
            }
        }
        break;
        case NPPVpluginFullScreenBool:
            {
            NPBool* isFullScreen = (NPBool*)aSetValue;
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            pluginWin->ToggleScreenMode(*isFullScreen);
            break;
            }
        case NPPVPluginZoom:
            {
            PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
            if (pluginWin) {
                TPoint* focusPoint = (TPoint*) aSetValue;
                if (focusPoint) {
                    pluginWin->adaptiveZoom(*focusPoint);
                   }
               }
            }
        default:
        break;
    
    }   // end of switch
      
    return NPERR_NO_ERROR;
}

// -----------------------------------------------------------------------------
// NpnInvalidateRect
//
// Unsupported.
// -----------------------------------------------------------------------------
//
void NpnInvalidateRect(NPP aInstance, NPRect * /*aRect*/)
{
    PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
    if( pluginWin )
        pluginWin->forceRedraw(false);
}

// -----------------------------------------------------------------------------
// NpnInvalidateRegion
//
// Unsupported.
// -----------------------------------------------------------------------------
//
void NpnInvalidateRegion(NPP /*aInstance*/, NPRegion /*aRegion*/)
{
// This function is not supported
}

// -----------------------------------------------------------------------------
// NpnForceRedraw
//
// Unsupported.
// -----------------------------------------------------------------------------
//
void NpnForceRedraw(NPP aInstance)
{
    PluginWin* pluginWin = (PluginWin*)aInstance->ndata;
    if( pluginWin )
        pluginWin->forceRedraw(true);
}



