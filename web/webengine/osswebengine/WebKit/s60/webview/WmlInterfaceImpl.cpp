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
* Description:   Handle special load events such as network connection, deal with non-http or non-html requests
*
*/


//  INCLUDES

#include "WmlInterfaceImpl.h"
#include "WebView.h"
#include "WebFrame.h"
#include "PlatformString.h"
#include "HttpSessionManager.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"

// FORWARD DECLARATIONS
class CWmlContentInterface;

using namespace WebCore;
// -----------------------------------------------------------------------------
// WKWmlInterfaceImpl::WKWmlInterfaceImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//  
CWmlInterfaceImpl::CWmlInterfaceImpl(CBrCtl& aBrCtl):m_brCtl(aBrCtl)                                            
{
}

// -----------------------------------------------------------------------------
// WKWmlInterfaceImpl::WKResetWmlServiceOptions
// Set the ResetWmlServiceOptions.
// -----------------------------------------------------------------------------
  


void CWmlInterfaceImpl::resetWmlServiceOptions()
{
 m_brCtl.resetWmlServiceOptions();
}


// -----------------------------------------------------------------------------
// WKWmlInterfaceImpl::WKSetWKWmlServiceOptionsAddItemL
// Set the ResetWmlServiceOptions.
// -----------------------------------------------------------------------------

void CWmlInterfaceImpl::setWmlServiceOptionsAddItemL(
              const TText* aText, TUint32 aElemID, TInt aElType)
{
 m_brCtl.wmlServiceOptionsAddItemL(aText,aElemID,aElType);
}


// -----------------------------------------------------------------------------
// WKWmlInterfaceImpl::WKSetWKUpdateSoftkeys()
// Set the WKUpdateSoftkeys.
// -----------------------------------------------------------------------------

void CWmlInterfaceImpl::setUpdateSoftkeys()
{
 m_brCtl.updateDefaultSoftkeys();
}


// -----------------------------------------------------------------------------
// CWmlInterfaceImpl::setWKPostUrlL()
// Set the WKPostUrlL.
// -----------------------------------------------------------------------------

void CWmlInterfaceImpl::setPostUrlL(TDesC& aUrl, TDesC8& aPostData, TDesC& aContentType)
{
 m_brCtl.postUrlL(aUrl,aPostData,aContentType);
}

// -----------------------------------------------------------------------------
// CWmlInterfaceImpl::WKFileNameToUrlLC()
// Convert the FileNameToUrl.
// -----------------------------------------------------------------------------

HBufC* CWmlInterfaceImpl::fileNameToUrlLC(const TDesC& aFileName)
{
 return ( m_brCtl.fileNameToUrlLC( aFileName ) );
}
// -----------------------------------------------------------------------------
// WKWmlInterfaceImpl::WKScrollingProviderNotifyLayoutChange
// Set the NotifyLayoutChange.
// -----------------------------------------------------------------------------

void CWmlInterfaceImpl::scrollingProviderNotifyLayoutChange(TBrCtlLayout aLayout)
{
 //(m_brCtl.BrCtlScrollingProvider())->NotifyLayoutChange(aLayout);
}


// -----------------------------------------------------------------------------
// WKWmlInterfaceImpl::WKSetWKWmlServiceOptionsAddItemL
// Set the UpdateVScrollBarL.
// -----------------------------------------------------------------------------


void CWmlInterfaceImpl::setScrollingProviderUpdateVScrollBarL( TInt aDocumentHeight,
                                    TInt aDisplayHeight,
                                    TInt aDisplayPosY)
{
 //(m_brCtl.BrCtlScrollingProvider())->UpdateVScrollBarL(aDocumentHeight,aDisplayHeight,aDisplayPosY);
}



// -----------------------------------------------------------------------------
// WKWmlInterfaceImpl::WKSetWKWmlServiceOptionsAddItemL
// Set the UpdateHScrollBarL.
// -----------------------------------------------------------------------------


void CWmlInterfaceImpl::setScrollingProviderUpdateHScrollBarL( TInt aDocumentWidth,
                                TInt aDisplayWidth,
                                TInt aDisplayPosX )
{
 //(m_brCtl.BrCtlScrollingProvider())->UpdateHScrollBarL(aDocumentWidth,aDisplayWidth,aDisplayPosX);
}


void CWmlInterfaceImpl::updateScrollbars(int documentHeight, int displayHeight, int displayPosY,
            int documentWidth, int displayWidth, int displayPosX)
{
    m_brCtl.updateScrollbars(documentHeight, displayHeight, displayPosY, documentWidth, displayWidth, displayPosX);
}

// -----------------------------------------------------------------------------
// CWmlInterfaceImpl::WKObjectInfoCreateL
// Create object of type CWKObjectInfo(..) 
// -----------------------------------------------------------------------------

CBrCtlObjectInfo* CWmlInterfaceImpl::objectInfoCreateL()
{
 CBrCtlObjectInfo* objectInfo = new(ELeave)CBrCtlObjectInfo();
 return objectInfo;
}

// -----------------------------------------------------------------------------
// CWmlInterfaceImpl::WKWmlResourceLoadListenerSetUrlL
// Create object of type CTempFile(..) 
// -----------------------------------------------------------------------------

void CWmlInterfaceImpl::resourceLoadListenerSetUrlL(const TDesC& aUrl)
{
//not implemented
}

// -----------------------------------------------------------------------------
// CWmlInterfaceImpl::WKWmlResourceLoadListenerLoadWmlResourceL
// Create object of type CWKObjectInfo(..) 
// -----------------------------------------------------------------------------

void CWmlInterfaceImpl::resourceLoadListenerLoadWmlResourceL(const TDesC& aURL)
{
//not implemented
}

// -----------------------------------------------------------------------------
// CWmlInterfaceImpl::WKWmlIsUrlInCache
// Returns true if the url is in http cache
// -----------------------------------------------------------------------------
TBool CWmlInterfaceImpl::wmlIsUrlInCache(TDesC& aUrl)
{
  return m_brCtl.IsUrlInCache(aUrl);
}


// -----------------------------------------------------------------------------
// CWmlInterfaceImpl::WKWmlLoadUrlL
// Returns 
// -----------------------------------------------------------------------------
void CWmlInterfaceImpl::wmlLoadUrlL( const TDesC& url, TDesC& referrer, TInt /*aApId*/,
                               TBrCtlDefs::TBrCtlCacheMode cachemode)
{    
    if (url.Ptr() == NULL || url.Length() == 0)
        User::Leave(KErrArgument);
    
    HttpSessionManager* httpSessionMgr = WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    HBufC8* url8 = HBufC8::NewLC( url.Length() );
    url8->Des().Copy( url );
    if(httpSessionMgr->refererEnabled()){ 
        m_brCtl.webView()->mainFrame()->loadURL( *url8, (TInt)cachemode, String(referrer) );
    }
    else{
        m_brCtl.webView()->mainFrame()->loadURL( *url8, (TInt)cachemode, String() );
    }
    CleanupStack::PopAndDestroy();
}


//Destructor
CWmlInterfaceImpl::~CWmlInterfaceImpl()
{
}

void CWmlInterfaceImpl::loadResource(TDesC8& url,  CWmlContentInterface* content )
{
    m_brCtl.loadResource(url, content);
}
// End of File

