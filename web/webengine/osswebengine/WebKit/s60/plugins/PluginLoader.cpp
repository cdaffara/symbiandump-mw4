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
#include "PluginLoader.h"
#include "PluginWin.h"
#include "PluginSkin.h"
#include "UrlRequestInfo.h"
#include "UrlResponseInfo.h"
#include "UrlResponseHeaderInfo.h"
#include "WebCoreUrlResponseInfo.h"
#include "WebCoreFormData.h"
#include "WebKitBridge.h"
#include "WebKitControl.h"
#include "WebKitLoader.h"
#include "PluginHandler.h"

const TInt KHttpStatusNotFound = 404;
const TInt KHttpStatusOk = 200;


// -----------------------------------------------------------------------------
// CPluginLoader::NewL
//
// Initializes a new plugin loader
// -----------------------------------------------------------------------------
//
CPluginLoader* CPluginLoader::NewL(CPluginSkin &aPluginSkin,
                                   CWebKitFrame& aWebKitFrame,
                                   const TDesC8& aBaseUrl)
    {
    CPluginLoader* self = new (ELeave) CPluginLoader(aPluginSkin,aWebKitFrame);
    CleanupStack::PushL( self );
    self->ConstructL(aBaseUrl);
    CleanupStack::Pop();    // self
    return self;
    }


// -----------------------------------------------------------------------------
// CPluginLoader::~CPluginLoader
//
// Destructor for CPluginLoader
// -----------------------------------------------------------------------------
//
CPluginLoader::~CPluginLoader()
    {
    // cancel pending loads
    CancelAllTransactions();
    delete iPluginLoadDataArray;
    delete iBaseUrl;
    }


// -----------------------------------------------------------------------------
// CPluginLoader::LoadPluginContentL
//
// Method to load plugin content, as requested by constructor and other
// webkit-plugin code
// -----------------------------------------------------------------------------
//
void CPluginLoader::LoadPluginContentL( const TDesC8& aUrl, TPluginLoadMode aLoadMode )
    {
    CUrlRequestInfo* requestInfo = CUrlRequestInfo::NewL();
    CleanupClosePushL( *requestInfo );

    // Get the base and relative url resolved by the webcore
    HBufC8* newUrl = CWebCoreFrameBridge::ResolveUrlL( iBaseUrl->Des(), aUrl );
    CleanupStack::PushL( newUrl );
    requestInfo->SetUrlL( newUrl->Des() );
    CleanupStack::PopAndDestroy();    // newUrl

    if ( aLoadMode != ELoadModeTop )
        {
        MContentLoaderInterface* urlLoader;
        if ( iWebKitFrame->WebKitBridge().Loader().LoadPluginL( requestInfo,
                                                                *this,
                                                                urlLoader ) == KErrNone )
            {
            CPluginLoadData* loadData = CPluginLoadData::NewL( requestInfo->TransId() );
            CleanupStack::PushL( loadData );
            loadData->SetLoadMode( aLoadMode );
            loadData->SetUrlLoader( urlLoader );
            loadData->SetHttpStatus( KHttpStatusOk );
            iPluginLoadDataArray->AppendL( *loadData );
            CleanupStack::Pop();// CPluginLoadData
            }
        }
    else    // aLoadMode == ELoadModeTop
        {
        // We received a top-level load request from webkit-plugin (Constructor, etc)
        requestInfo->SetCacheMode( iWebKitFrame->WebKitView().WebKitControl().CacheMode() );
        // zalan 
        // iWebKitFrame->WebKitBridge().Loader().LoadPageL( requestInfo );
        }

    CleanupStack::Pop(); // requestInfo
    requestInfo->Close();
    }


// -----------------------------------------------------------------------------
// CPluginLoader::LoadPluginContentL
//
// Method to load additional plugin content, as requested by the plugin
// -----------------------------------------------------------------------------
//
void CPluginLoader::LoadPluginContentL( const TDesC8& aUrl,
                                        TUrlLoadMethod aMethod,
                                        TPluginLoadMode aLoadMode,
                                        const TPtr8& aBody, TInt &aTrId )
    {

    _LIT8(KJavaScript, "javascript:");

    // Pass javascript urls to the ecmascript engine instead...
    if ( aUrl.FindF( KJavaScript ) == 0 )
        {
        HBufC8* newUrl = CWebCoreFrameBridge::ResolveUrlL(iBaseUrl->Des(), aUrl);
        CleanupStack::PushL( newUrl );
        iWebKitFrame->WebKitBridge().WebCoreBridge().ExecuteScript( *newUrl );
        CleanupStack::PopAndDestroy( newUrl );
        }

    CUrlRequestInfo* requestInfo = CUrlRequestInfo::NewL();
    CleanupClosePushL( *requestInfo );

    // Get the base and relative url resolved by the webcore
    HBufC8* newUrl = CWebCoreFrameBridge::ResolveUrlL( iBaseUrl->Des(), aUrl );
    CleanupStack::PushL( newUrl );
    requestInfo->SetUrlL( newUrl->Des() );
    CleanupStack::PopAndDestroy();    // newUrl

    requestInfo->SetLoadMethod( aMethod );

    if ( aMethod == EUrlPost && aBody.Length() > 0 )
       {
       RArray<TWebCoreFormDataItem> postDataItems;
       TWebCoreFormDataItem webCoreFormData( aBody );
       postDataItems.Append( webCoreFormData );
       requestInfo->SetPostDataL( postDataItems );
       postDataItems.Close();
       }

    if ( aLoadMode == ELoadModePlugin )
        {
        // To redirect the response back here, set the listener as *this
        MContentLoaderInterface* urlLoader;
        requestInfo->SetTopLevel( EFalse );
        if ( iWebKitFrame->WebKitBridge().Loader().LoadPluginL( requestInfo,
                                                                *this,
                                                                urlLoader ) == KErrNone )
            {
            CPluginLoadData* loadData = CPluginLoadData::NewL( requestInfo->TransId() );

            CleanupStack::PushL( loadData );
            loadData->SetLoadMode( aLoadMode );
            loadData->SetUrlLoader( urlLoader );
            loadData->SetHttpStatus( KHttpStatusOk );
            iPluginLoadDataArray->AppendL( *loadData );
            CleanupStack::Pop(); // loadData
            }
        }
    else    // aLoadMode == ELoadModeTop
        {
        // We have received a top-level load request from a plugin (see NPN_GetUrl).
        // We currently only support window target=_Top, see NpnImplementation.cpp.
        // Dispatch to WebKitLoader, so that it is the listener.
        requestInfo->SetTopLevel( ETrue );
        requestInfo->SetCacheMode( iWebKitFrame->WebKitView().WebKitControl().CacheMode() );
        // zalan
        //iWebKitFrame->WebKitBridge().Loader().LoadPageL( requestInfo );
        }

    aTrId = requestInfo->TransId();
    CleanupStack::PopAndDestroy(); // requestInfo

    }


// -----------------------------------------------------------------------------
// CPluginLoader::Headers
// From MContentListener
//
// This method is called in two circumstances:
// One is during construction of the Plugin and it's initial load request.
// The initial load request returns here when the content arrives. This method
// then needs to create the PluginWin (plugin) in reponse to the first request
// for content.
// The second is that Plugins can request additional content. This method can
// be called numerous times in response to plugin's request (see NPN_GetURL).
//
// This method needs to handle the initial creation of the PluginWin for the first
// request for content. But also handle the additional calls for content correctly.
// For example, a Flash plugin might be loaded because the html page has an
// object tag with content of .swf. Then that same flash plugin can request
// additonal content that is .swf or .xml.
// -----------------------------------------------------------------------------
TInt CPluginLoader::HeadersL( TInt aTransactionId,
                              CUrlResponseInfo& aResponse )
    {
  CUrlResponseHeaderInfo& headerInfo = aResponse.HeaderInfo();

    iWebKitFrame->WebKitBridge().Loader().IncomingContentInfo( aTransactionId,
                                                               ELoadStarted,
                                                               EFalse,
                                                               aResponse );

    CPluginLoadData* pluginLoadData = GetPluginLoadData( aTransactionId );

    // Check the returned http status code
    TUint32 httpStatus( headerInfo.HttpStatus() );
    switch ( httpStatus )
        {
        // If we have a successful code, continue
        case KHttpStatusOk:
            {
            break;
            }

        // If we have an http error code, return noting that plugin content
        // not supported
        default:
            {
            if( !pluginLoadData )
                {
                // If no content returned, just show the place holder
                iPluginSkin->InvalidContent();
                }
            else
                {
                pluginLoadData->SetHttpStatus( KHttpStatusNotFound );
                }
            return KErrNone;
            }
        }   // end of switch

    // Find the plugin load mode
    TPluginLoadMode loadMode = ELoadModeNone;
    if ( pluginLoadData )
        {
        loadMode = pluginLoadData->LoadMode();
        }

    // Get plugin handler, so we can find a plugin handle for a plugin that
    // supports the incoming content (by mimetype or extension)
    CPluginHandler& pluginHandler = iWebKitFrame->WebKitView().WebKitControl().PluginHandler();
    TInt handle = pluginHandler.FindPlugin( headerInfo.ContentType() );
    if ( handle == KErrNotFound )
        {
        handle = pluginHandler.FindPluginByExtension( headerInfo.ResponseUrl() );
        }

    if ( handle != KErrNotFound )
        {
        // We found a plugin that understands the content, so let's create the
        // PluginWin. This method determines if we keep the current PluginWin
        // or create a new PluginWin.
        iPluginSkin->CreatePluginWinL( headerInfo.ContentType(),
                                       handle, loadMode );
        }

    // At this point, we need a valid PluginWin (aka Plugin) to be created, or
    // use the already existing plugin, to handle the incoming content.  The
    // content type, or extension is only used to create the initial instance of
    // plugin. Note, a plugin can request additional content that is not of the
    // same type, or extension as the initial content, so we must let that content
    // be passed to the existing plugin that requested it.
    CPluginWin* pluginWin = iPluginSkin->PluginWin();
    if ( pluginWin )
        {
        // Do we have a PluginWin and save as file (soundstart)
        if (loadMode == ELoadModeSaveAsFile)
            {
            SaveResponseHeader( headerInfo.TransId(),
                                headerInfo );
            }
        TRAPD( err, pluginWin->CreateStreamL( headerInfo.ResponseUrl(),
                                              headerInfo.ContentType(),
                                              headerInfo.TransId(),
                                              headerInfo.ContentLength()) );
        if ( err != KErrNone )
           {
           iPluginSkin->HandlePluginError( err );
           CancelAllTransactions();
           }
        }
    else
        {
        // If the plugin is not supported, just show the place holder
        iPluginSkin->InvalidContent();
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPluginSkin::ResponseL
// From MContentListener
// -----------------------------------------------------------------------------
void CPluginLoader::ResponseL( TInt aTransactionId, CUrlResponseInfo& aResponse )
    {
    // Call our super class
    iWebKitFrame->WebKitBridge().Loader().IncomingContentInfo( aTransactionId,
                                                               EMoreContent,
                                                               ETrue, aResponse );

    // Do not write to the plugin if the load request was bad
    CPluginLoadData* pluginLoadData = GetPluginLoadData( aTransactionId );
    if ( pluginLoadData && pluginLoadData->HttpStatus() == KHttpStatusNotFound )
        {
        return;
        }

    // Pass the data to the plugin. A plugin should make its own copy of the data
    CPluginWin* pluginWin = iPluginSkin->PluginWin();
    if ( pluginWin )
        {
        TRAPD( err, pluginWin->WriteStreamL( aTransactionId, aResponse.Body() ) );
        if ( err != KErrNone )
           {
           iPluginSkin->HandlePluginError( err );
           CancelAllTransactions();
           }
        }
    }

// -----------------------------------------------------------------------------
// CPluginSkin::Complete
// From MContentListener
// -----------------------------------------------------------------------------
void CPluginLoader::Complete(
    TInt aTransactionId,
  CUrlResponseInfo& aResponse,
    TInt aError )
    {

    iWebKitFrame->WebKitBridge().Loader().IncomingContentInfo( aTransactionId, ELoadComplete, ETrue, aResponse );

    CPluginLoadData* pluginLoadData = GetPluginLoadData(aTransactionId);
    //Donot write to the plugin if the load request was bad
    if(pluginLoadData && pluginLoadData->HttpStatus() == KHttpStatusNotFound)
        {
        RemovePluginLoadData( aTransactionId );
        return;
        }

    CPluginWin* pluginWin = iPluginSkin->PluginWin();
    if (aError == KErrNone && pluginWin)
       {
        if ( GetLoadMode(aTransactionId) == ELoadModeSaveAsFile )
            {
            // Soundstart case, don't play and delete content (aka DestroyStream)
            SaveCompleteError(aTransactionId, aError);
            }
        else
            {
            // Plays content and then deletes it
            pluginWin->DestroyStream(aTransactionId, aError);
            }
        }

    iWebKitFrame->WebKitBridge().Loader().IncomingContentInfo( aTransactionId, ELoadComplete, ETrue, aResponse );

    if ( GetLoadMode(aTransactionId) != ELoadModeSaveAsFile )
        {
        // If not soundstart case, the content was deleted (by DestroyStream),
        // so we can delete CPluginLoadData. If soundstart, the content is
        // still around, so keep CPluginLoadData so we can find it.
        RemovePluginLoadData( aTransactionId );
        }
    }

// -----------------------------------------------------------------------------
// CPluginSkin::HandleError
// From MContentListener
// -----------------------------------------------------------------------------
void CPluginLoader::HandleError( TInt /*aTransactionId*/,  TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CPluginLoader::CPluginLoader
//
// Constructor for CPluginLoader
// -----------------------------------------------------------------------------
//
CPluginLoader::CPluginLoader(CPluginSkin &aPluginSkin,CWebKitFrame& aWebKitFrame):
    iWebKitFrame(&aWebKitFrame),
    iPluginSkin(&aPluginSkin)
    {
    }


// -----------------------------------------------------------------------------
// CPluginLoader::~CPluginLoader
//
// Symbian two phase constructor
// -----------------------------------------------------------------------------
//
void CPluginLoader::ConstructL(const TDesC8& aBaseUrl)
    {
    iBaseUrl = HBufC8::NewL(aBaseUrl.Length());
    iBaseUrl->Des().Copy(aBaseUrl);
    iPluginLoadDataArray = new(ELeave) CArrayFixFlat<CPluginLoadData>( 2 );
    }

// -----------------------------------------------------------------------------
// CPluginLoader::GetTransactionId
// Public method
// Method to return transaction Id, using the request url
// -----------------------------------------------------------------------------
TInt CPluginLoader::GetTransactionId( const TDesC8& aRequestUrl )
  {
  for (TInt i = 0;i < iPluginLoadDataArray->Count(); i++)
    {
    CPluginLoadData& loadData = iPluginLoadDataArray->At(i);
    if (loadData.RequestUrl())
      {
      if (aRequestUrl.Compare(*loadData.RequestUrl()) == 0)
        {
        return loadData.TransactionId();
        }
      }
    }
  return KErrNotFound;
  }

// -----------------------------------------------------------------------------
// CPluginLoader::GetCompleteError
// Public method
// Method to return complete error code for a transaction
// -----------------------------------------------------------------------------
TInt CPluginLoader::GetCompleteError( TInt aTrId )
    {
  for (TInt i = 0;i < iPluginLoadDataArray->Count(); i++)
      {
      CPluginLoadData& loadData = iPluginLoadDataArray->At(i);
    if (loadData.TransactionId() == aTrId)
        {
      return loadData.CompleteError();
        }
      }
  return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CPluginLoader::GetLoadData
// Public method
// Method to return load data for a transaction
// -----------------------------------------------------------------------------
CPluginLoadData* CPluginLoader::GetPluginLoadData(TInt aTrId)
    {
    for (TInt i = 0;i < iPluginLoadDataArray->Count(); i++)
        {
        if (iPluginLoadDataArray->At(i).TransactionId() == aTrId)
            {
            return &(iPluginLoadDataArray->At(i));
            }
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// CPluginLoader::RemovePluginLoadData
//
// -----------------------------------------------------------------------------
void CPluginLoader::RemovePluginLoadData(
    TInt aTransactionId )
    {
    for (TInt i = 0;i < iPluginLoadDataArray->Count(); i++)
        {
        if (iPluginLoadDataArray->At(i).TransactionId() == aTransactionId)
            {
            iPluginLoadDataArray->Delete( i );
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPluginLoader::CancelAllTransactions
//
// -----------------------------------------------------------------------------
void CPluginLoader::CancelAllTransactions()
    {
    // cancel pending loads
    TRAP_IGNORE(
        for ( TInt i = 0; i < iPluginLoadDataArray->Count(); i++)
              {
              iPluginLoadDataArray->At(i).UrlLoader()->CancelL( iPluginLoadDataArray->At(i).TransactionId() );
              }
        );
    }

// -----------------------------------------------------------------------------
// CPluginLoader::GetLoadMode
// Public method
// Method to return load mode for a transaction
// -----------------------------------------------------------------------------
TPluginLoadMode CPluginLoader::GetLoadMode(TInt aTrId)
    {
  for (TInt i = 0;i < iPluginLoadDataArray->Count(); i++)
      {
      CPluginLoadData& loadData = iPluginLoadDataArray->At(i);
    if (loadData.TransactionId() == aTrId)
        {
      return loadData.LoadMode();
        }
      }
  return ELoadModeNone;
    }

// -----------------------------------------------------------------------------
// CPluginLoader::SaveCompleteError
// Save the request complete error code for a transaction. Use Transaction Id
// as the index into the PluginLoadDataArray.
// -----------------------------------------------------------------------------
TBool CPluginLoader::SaveCompleteError(
    TInt aTrId,
    TInt aError)
    {
  for (TInt i = 0;i < iPluginLoadDataArray->Count(); i++)
      {
      CPluginLoadData& loadData = iPluginLoadDataArray->At(i);
    if (loadData.TransactionId() == aTrId)
        {
        loadData.SetCompleteError(aError);
      return ETrue;
        }
      }

  return EFalse;
    }

// -----------------------------------------------------------------------------
// CPluginLoader::SaveResponseHeader
// Save the response header for a transaction. Use Transaction Id
// as the index into the PluginLoadDataArray.
// -----------------------------------------------------------------------------
TBool CPluginLoader::SaveResponseHeader( TInt aTrId,
                                         CUrlResponseHeaderInfo& aResponseHeader )
    {
    for (TInt i = 0;i < iPluginLoadDataArray->Count(); i++)
        {
        CPluginLoadData& loadData = iPluginLoadDataArray->At(i);
        if (loadData.TransactionId() == aTrId)
            {
            TRAPD( error, loadData.SetRequestUrlL(aResponseHeader.RequestUrl()) );
            return error == KErrNone;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPluginLoadData - A helper class for CPluginLoader
// -----------------------------------------------------------------------------
// CPluginLoadData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPluginLoadData* CPluginLoadData::NewL(TInt aTrId)
    {
    CPluginLoadData* self = new( ELeave ) CPluginLoadData(aTrId);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CPluginLoadData::CPluginLoadData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPluginLoadData::CPluginLoadData(TInt aTrId)
    : iTransId(aTrId),
      iCompleteError(-1)
    {
    }

// -----------------------------------------------------------------------------
// CPluginLoadData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPluginLoadData::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CPluginLoadData::~CPluginLoadData
// Deconstructor.
// -----------------------------------------------------------------------------
//
CPluginLoadData::~CPluginLoadData()
    {
    delete iRequestUrl;
    iRequestUrl = NULL;
    }

// -----------------------------------------------------------------------------
// CPluginLoadData::SetHttpStatus
// Public method
// Save the http staus
// -----------------------------------------------------------------------------
//
void CPluginLoadData::SetHttpStatus(TInt aHttpStatus)
{
  iHttpStatus = aHttpStatus;
}

// -----------------------------------------------------------------------------
// CPluginLoadData::SetLoadMode
// Public method
// Save the load mode.
// -----------------------------------------------------------------------------
//
void CPluginLoadData::SetLoadMode(TPluginLoadMode aLoadMode)
    {
    iLoadMode = aLoadMode;
    }

// -----------------------------------------------------------------------------
// CPluginLoadData::SetCompleteError
// Public method
// Save the completion error from response complete
// -----------------------------------------------------------------------------
//
void CPluginLoadData::SetCompleteError(TInt aCompleteError)
    {
    iCompleteError = aCompleteError;
    }

// -----------------------------------------------------------------------------
// CPluginLoadData::SetRequestUrlL
// Public method
// Save the request url. We take ownership of the url.
// -----------------------------------------------------------------------------
//
void CPluginLoadData::SetRequestUrlL(const TDesC8& aRequestUrl)
    {
    if (iRequestUrl)
        {
        delete iRequestUrl;
        iRequestUrl = NULL;
        }

    iRequestUrl = aRequestUrl.AllocL();
    }

// -----------------------------------------------------------------------------
// CPluginLoadData::SetUrlLoader()
// Public method
// Save the url loader
// -----------------------------------------------------------------------------
//
void CPluginLoadData::SetUrlLoader(MContentLoaderInterface* aUrlLoader)
    {
    iUrlLoader = aUrlLoader;
    }
