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


//  INCLUDES
#include <Browser_platform_variant.hrh>
#include "WmlDispatcher.h"

#ifndef BRDO_WML_DISABLED_FF
#include "WmlEngineInterface.h"
#endif 

#include "WmlResourceLoaderClient.h"

#include "StaticObjectsContainer.h"
#include "WebCursor.h"

#include "Frame.h"
#include "FrameLoader.h"
#include "DocumentLoader.h"
#include "ProgressTracker.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"
#include "WmlResourceLoader.h"

#include "WebKitLogger.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// -----------------------------------------------------------------------------
// CWmlDispatcher::NewL
// Public Class Method
// Two-phased constructor.
// -----------------------------------------------------------------------------
CWmlDispatcher* CWmlDispatcher::NewL(CBrCtl* aBrCtl, WebCore::Frame* frame)
{
  
  CWmlDispatcher* self = new (ELeave) CWmlDispatcher(aBrCtl, frame);
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop(); // self
  return self;
}
  
// -----------------------------------------------------------------------------
// CWmlDispatcher::ConstructL
// Private Class Method
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CWmlDispatcher::ConstructL()
{
}
    
// -----------------------------------------------------------------------------
//   C++ default constructor.
// -----------------------------------------------------------------------------
CWmlDispatcher::CWmlDispatcher(CBrCtl* aBrCtl, WebCore::Frame* frame): m_brCtl(aBrCtl), m_frame(frame)
{
}

// -----------------------------------------------------------------------------
//   C++ destructor.
// -----------------------------------------------------------------------------  
CWmlDispatcher::~CWmlDispatcher( )
{
    delete m_url;
    delete m_contentType;
    delete m_charset;
    delete m_contentFile;
}


// -----------------------------------------------------------------------------
// CWmlDispatcher::HeadersL
// -----------------------------------------------------------------------------
TInt CWmlDispatcher::HeadersL(TInt aTransactionId, WebCore::ResourceResponse& aResponseInfo)
{  	
   TInt status(KErrNone);

#ifndef BRDO_WML_DISABLED_FF
    // cleanup content file, parameter list and data type from prior response (if any)
    delete m_contentFile;
    m_contentFile = NULL;
    delete m_contentType;
    m_contentType = NULL;
    delete m_charset;
    m_charset = NULL;
    delete m_url;
    m_url = NULL;

    m_error = 0;
    m_contentFile = CTempFile::NewL();
    m_loading = ETrue;
    
    // set response attributes
    m_contentType = aResponseInfo.mimeType().des().AllocL();
    m_charset = aResponseInfo.textEncodingName().des().AllocL();

    TPtrC myUrl = aResponseInfo.url().url().des();
    m_url = HBufC::NewL(myUrl.Length() + 1);
    m_url->Des().Copy(myUrl);
    m_url->Des().ZeroTerminate();

    //If wmlEngine.dll is not loaded then it is time to load the dll.
    if( !m_brCtl->wmlEngineInterface() ){
        m_brCtl->LoadDllWmlEngineL();
    }

    if (! m_brCtl->wmlMode()) {
        m_brCtl->MakeVisible(EFalse);  // turn off previous view if necessary
    }
    m_brCtl->setWmlMode(ETrue);
    
    //send the certificate info
    m_brCtl->wmlEngineInterface()->SetCertInfo(m_brCtl->CertInfo());
    
#else
    status = EFalse; 
#endif    
    return status;
}

//
// -----------------------------------------------------------------------------
// CWmlDispatcher::ProcessResponseL
// 
// 
// -----------------------------------------------------------------------------
//
void CWmlDispatcher::ResponseL( const char* data, int length )
{
#ifndef BRDO_WML_DISABLED_FF
    if(0 == m_error){
    // write incoming data to the content file
    const TPtrC8 response((unsigned char *)data, length);
    m_contentFile->WriteL( response );
    }
#endif     
}

//
// -----------------------------------------------------------------------------
// CWmlDispatcher::ResponseCompleteL
// 
// 
// -----------------------------------------------------------------------------
//
void CWmlDispatcher::CompleteL(TInt /*aTransactionId*/, TInt /*aError*/  )
{
#ifndef BRDO_WML_DISABLED_FF
    if(0 == m_error && m_loading){
    m_contentFile->CloseL();
    LoadFileL(m_contentFile->Name() , *m_url, *m_contentType);
    MakeWmlVisible(ETrue);
    m_loading = EFalse;
    }
#endif     
}

//
// -----------------------------------------------------------------------------
// CWmlDispatcher::HandleError
// 
// 
// -----------------------------------------------------------------------------
//
void CWmlDispatcher::HandleError( TInt /*aTransactionId*/, TInt aError )
{
#ifndef BRDO_WML_DISABLED_FF
    m_error = aError;
#endif     
}

// -----------------------------------------------------------------------------
// void CWmlDispatcher::LoadFileL
// Loads the specified file
// -----------------------------------------------------------------------------
void CWmlDispatcher::LoadFileL(const TDesC& aFileName, const TDesC& aUrl, const TDesC& aContType)
{
#ifndef BRDO_WML_DISABLED_FF
    if (aFileName.Ptr() == NULL || aFileName.Length() == 0){
        User::Leave(KErrArgument);
    }
    AddUrlToFileL(aFileName, aUrl, aContType);
    HBufC* urlName = m_brCtl->fileNameToUrlLC(aFileName);
    LoadUrlL( *urlName);
    CleanupStack::PopAndDestroy(urlName);
#endif     
}

// -----------------------------------------------------------------------------
// void CWmlDispatcher::LoadUrlL
// Loads the specified url
// -----------------------------------------------------------------------------
void CWmlDispatcher::LoadUrlL( const TDesC& aUrl )
{
#ifndef BRDO_WML_DISABLED_FF
    m_brCtl->wmlEngineInterface()->WmlLoadUrlL( aUrl );
#endif     
}
   
        
// -----------------------------------------------------------------------------
// CWmlDispatcher::AddUrlToFile
// Adds the url to the file
// -----------------------------------------------------------------------------
//    
void CWmlDispatcher::AddUrlToFileL(const TDesC& aFileName, const TDesC& aUrl, const TDesC& aContType)
{
#ifndef BRDO_WML_DISABLED_FF
    RFs fsSession;
    RFile contentFile;
    TInt fileSize;
    User::LeaveIfError(fsSession.Connect());
    User::LeaveIfError(contentFile.Open(fsSession,aFileName,EFileShareExclusive|EFileWrite));
  
    contentFile.Size(fileSize);
    _LIT8(KSeparator,"*");
    contentFile.Write(fileSize, KSeparator);
    
    TInt urlPtrLen = aUrl.Length() + 1;
    TUint8 *urlPtr = new(ELeave)TUint8[urlPtrLen];
	   CleanupStack::PushL( urlPtr );
    TPtr8 ptr(urlPtr, (urlPtrLen));
    ptr.Copy(aUrl);
    ptr.ZeroTerminate();
    TPtrC8 urlPtr8(ptr);
    
    contentFile.Size(fileSize);
    contentFile.Write(fileSize, urlPtr8);
  
    //add charset to the file if it has a charset
    if(m_charset != NULL){ 
        TDesC& aCharset = *m_charset;
        contentFile.Size(fileSize);
        _LIT8(KCharsetSeparator,"$");
        contentFile.Write(fileSize, KCharsetSeparator);

        TInt charsetPtrLen = aCharset.Length() + 1;
        TUint8 *charsetPtr = new(ELeave)TUint8[charsetPtrLen];  
        TPtr8 csptr(charsetPtr, (charsetPtrLen));
        csptr.Copy(aCharset);
        csptr.ZeroTerminate();
        TPtrC8 charsetPtr8(csptr);
        contentFile.Size(fileSize);
        contentFile.Write(fileSize, charsetPtr8);

        if(charsetPtr)
            delete charsetPtr;
    }

  //add content type
  _LIT(KContentTypeT,"text/vnd.wap.wml");
  _LIT(KContentTypeTC,"text/vnd.wap.wmlc");
  _LIT(KContentWMLSType,"application/vnd.wap.wmlscriptc");
  _LIT(KContentWMLACType,"application/vnd.wap.wmlc");

  contentFile.Size(fileSize);
  if( aContType.Compare(KContentTypeT) == 0 ){
    _LIT8(KContType,"0");
    contentFile.Write(fileSize, KContType); 
  }
  else if( aContType.Compare(KContentTypeTC) == 0 ){
    _LIT8(KContType,"1");
    contentFile.Write(fileSize, KContType); 
  }
  else if( aContType.Compare(KContentWMLSType) == 0 ){
    _LIT8(KContType,"2");
    contentFile.Write(fileSize, KContType); 
  }
  else if( aContType.Compare(KContentWMLACType) == 0 ){
    _LIT8(KContType,"3");
    contentFile.Write(fileSize, KContType); 
  }

  CleanupStack::PopAndDestroy(urlPtr);

  contentFile.Close();
  fsSession.Close();
#endif // BRDO_WML_DISABLED_FF  
}

    
//-------------------------------------------------------------------------------
// MakeWmlVisible
//
//-------------------------------------------------------------------------------    
void CWmlDispatcher::MakeWmlVisible(bool visible)
{
#ifndef BRDO_WML_DISABLED_FF
    if (m_brCtl->wmlMode()){
        m_brCtl->MakeVisible(visible);
        m_brCtl->SetFocus(visible);
        m_brCtl->DrawNow();
        WebCursor* cursor = WebCore::StaticObjectsContainer::instance()->webCursor();
        cursor->setCursorVisible(!visible);
    }
#endif   
}    

//-------------------------------------------------------------------------------
// loadResource
//
//-------------------------------------------------------------------------------    
void CWmlDispatcher::loadResource(TDesC8& url,  CWmlContentInterface* content )
{
#ifndef BRDO_WML_DISABLED_FF
    //this object get's deleted by the WmlStreamLoader. 
    WmlResourceLoaderClient* wmlclient = WmlResourceLoaderClient::CreateResourceLoaderClient(this);
    if (wmlclient){
        wmlclient->start(url,m_frame);        
    }        
#endif     
}

//-------------------------------------------------------------------------------
// WmlParameters
//
//-------------------------------------------------------------------------------    
void CWmlDispatcher::WmlParameters( const char* data, int length, TDesC& contentType, TDesC& charset, TInt httpStatus, TInt chunkIndex, TDesC& url  )
{
#ifndef BRDO_WML_DISABLED_FF
    m_brCtl->wmlEngineInterface()->WmlParameters( data, length, contentType, charset, httpStatus, chunkIndex, url);    
#endif     
}


// End of File


