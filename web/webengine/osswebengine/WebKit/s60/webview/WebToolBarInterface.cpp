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


// INCLUDE FILES
#include "config.h"

#include <e32std.h>
#include "StaticObjectsContainer.h"
#include "WebIconDatabase.h"
#include "WebToolBarInterface.h"
#include "SettingsContainer.h"

#include "KURL.h"
#include "BrCtl.h"
#include "Frame.h"
#include "FrameLoader.h"

#include "WebView.h"
#include "WebCursor.h"
#include "WebFrame.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
using namespace WebCore;

// ============================ MEMBER FUNCTIONS ===============================


WebToolBarInterface::WebToolBarInterface(WebView* webView)    
{
    m_webView = webView;
}

WebToolBarInterface::~WebToolBarInterface()
{
}

TPoint WebToolBarInterface::CursorPosition()
{
    return StaticObjectsContainer::instance()->webCursor()->position();
}

void WebToolBarInterface::SetAndUpdateCursorPosition(
    const TPoint& aPoint)
{
    StaticObjectsContainer::instance()->webCursor()->setPosition(aPoint);
    StaticObjectsContainer::instance()->webCursor()->cursorUpdate(ETrue);
}

const RPointerArray<TBrCtlSubscribeTo>& WebToolBarInterface::SubscribeToMenuItemsL() 
{ 
    return m_webView->brCtl()->SubscribeToMenuItemsL();
}

CCoeControl& WebToolBarInterface::View() 
{ 
    return *m_webView; 
}

CCoeControl& WebToolBarInterface::CCoeControlParent() 
{ 
    return *(m_webView->brCtl()->CCoeControlParent()); 
}

void WebToolBarInterface::CloseToolBarL() 
{ 
    m_webView->closeToolBarL(); 
}

CGulIcon* WebToolBarInterface::GetFaviconL(const TDesC& aUrl)
{ 

    HBufC8* url = HBufC8::NewLC(aUrl.Length());
    url->Des().Copy(aUrl);
    CGulIcon* icon = StaticObjectsContainer::instance()->sharedIconDatabase()->iconForPageURL(*url);
    CleanupStack::PopAndDestroy(); // url
    return icon;
    
}

void WebToolBarInterface::SendCommandsToClient(
    TBrCtlDefs::TBrCtlClientCommands aCommand,
    const CArrayFix<TPtrC>& aAttributesNames,
    const CArrayFix<TPtrC>& aAttributeValues)
{
    WebView* wv = m_webView;
    wv->closeToolBarL();
    wv->brCtl()->sendCommandsToClient(aCommand,aAttributesNames,aAttributeValues);
}

void WebToolBarInterface::LoadUrlL( const TDesC& aUrl, TBrCtlDefs::TBrCtlCacheMode aCacheMode )
{ 
    HBufC8* url = HBufC8::NewLC(aUrl.Length());
    url->Des().Copy(aUrl);
    m_webView->mainFrame()->loadURL( *url, (TInt)aCacheMode, String() );
    CleanupStack::PopAndDestroy(); // url    
}


HBufC* WebToolBarInterface::ResolveUrlL(
    const TDesC& aUrl)
{
    KURL url(core(m_webView->mainFrame())->loader()->baseURL(),DeprecatedString::fromDes(aUrl));
    HBufC* newUrl = HBufC::NewL( url.des().Length() );
    newUrl->Des().Copy( url.des() );
    return newUrl;    
}

void WebToolBarInterface::OpenPageViewL()
{
    WebView* wv = m_webView;
    wv->closeToolBarL();
    wv->openPageViewL();
}

TUint WebToolBarInterface::GetBrowserSettingL(TUint aSetting)
{    
   return m_webView->brCtl()->settings()->brctlSetting(static_cast<TBrCtlDefs::TBrCtlSettings>(aSetting));
}

void WebToolBarInterface::HandleHistoryCommandL(
    TBrCtlDefs::TBrCtlCommands aCommand)
{
    WebView* wv = m_webView;
    wv->closeToolBarL();
    wv->brCtl()->HandleCommandL((TInt)TBrCtlDefs::ECommandIdBase + (TInt)aCommand);
}

void WebToolBarInterface::ViewImagesL()
{
    if (m_webView && m_webView->brCtl()) {
    
        m_webView->brCtl()->HandleCommandL( (TInt)TBrCtlDefs::ECommandShowImages +
                                            (TInt)TBrCtlDefs::ECommandIdBase );
    }                                            
}

TBool WebToolBarInterface::wmlMode()
{
    if (m_webView && m_webView->brCtl()) {
        return m_webView->brCtl()->wmlMode();
    }
    return EFalse;
}

//End of file
