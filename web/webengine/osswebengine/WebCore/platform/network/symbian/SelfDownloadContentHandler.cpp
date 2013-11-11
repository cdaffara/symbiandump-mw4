/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "BrCtl.h"
#include "SelfDownloadContentHandler.h"
#include <BrCtlSpecialLoadObserver.h>
#include <BrCtlDefs.h>
#include <apmstd.h>
#include <eikenv.h>
#include "ResourceRequest.h"
#include "ResourceResponse.h"
#include "WebFrame.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include "HttpSessionManager.h"
#include "httpfiltercommonstringsext.h"
#include <uri8.h>
#include <http/rhttptransaction.h>
#include <http/mhttpdatasupplier.h>
#include <http/rhttpsession.h>

// CONSTANTS
_LIT(KTempFilePath, "c:\\system\\temp\\browser\\");

// -----------------------------------------------------------------------------
// CTempFile::CTempFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTempFile::CTempFile( void )
    {
    }

// -----------------------------------------------------------------------------
// CTempFile::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTempFile::ConstructL(HBufC* aFileName)
    {
    User::LeaveIfError( iFsSession.Connect() );

    // Make any directories in KTempFilePath that do not yet exist
    iFsSession.MkDirAll( KTempFilePath );

    TInt err = aFileName == NULL ? KErrGeneral : KErrNone;

    // Create (or replace) our new file with the filename passed in
    if ( aFileName != NULL )
        {
        iFileName = HBufC::NewL( aFileName->Length() + KTempFilePath().Length() );
        iFileName->Des().Copy( KTempFilePath );
        iFileName->Des().Append( *aFileName );
        err = iFile.Replace( iFsSession, *iFileName, EFileWrite );
        }

    // Create a temp file, if Replace() failed, or aFileName=NULL
    if ( err != KErrNone )
        {
        delete iFileName;
        iFileName = NULL;
        TFileName tempName;
        User::LeaveIfError( iFile.Temp( iFsSession, KTempFilePath, tempName, EFileWrite ) );
        iFileName = tempName.AllocL();
        }
    }
     
// -----------------------------------------------------------------------------
// CTempFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTempFile* CTempFile::NewL(HBufC* aFileName)
    {
    CTempFile* self = new(ELeave) CTempFile(  );
    CleanupStack::PushL( self );
    self->ConstructL(aFileName);
    CleanupStack::Pop( );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CTempFile Destructor
// Public Class Method
// -----------------------------------------------------------------------------
CTempFile::~CTempFile( void )
{
    iFile.Close();
    if(iFileName) {
        iFsSession.Delete( *iFileName );
    }
    iFsSession.Close();
    delete iFileName;
}

// -----------------------------------------------------------------------------
// CTempFile::Write
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTempFile::WriteL(
    const TDesC8& aFileData )
{
    return iFile.Write( aFileData );
}

// -----------------------------------------------------------------------------
// CTempFile::Close
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTempFile::CloseL()
{
    iFile.Close();
}

// -----------------------------------------------------------------------------
// CTempFile::Name
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CTempFile::Name() const
{
    return *iFileName;
}

using namespace WebCore;

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::SelfDownloadContentHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
SelfDownloadContentHandler::SelfDownloadContentHandler(
    WebFrame* webFrame)
    : m_WebFrame(webFrame),
      m_ContentFile(NULL),
      m_ContentType(NULL),
      m_Charset(NULL),
      m_ReqUrl(NULL)
{
	StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->ResetOutstandingSelfDl();
}

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void SelfDownloadContentHandler::ConstructL(
    WebFrame* webFrame,
    TDesC& selfDownloadTypes)
{
    m_SpecialLoadObserver = control(m_WebFrame)->brCtlSpecialLoadObserver();
    m_SelfDlMimeTypes = HBufC::NewL(selfDownloadTypes.Length());
    m_SelfDlMimeTypes->Des().Copy(selfDownloadTypes);
}

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
SelfDownloadContentHandler* SelfDownloadContentHandler::NewL(
    WebFrame* webFrame,
    TDesC& selfDownloadTypes)
{
    SelfDownloadContentHandler* self = new(ELeave) SelfDownloadContentHandler(webFrame);
    CleanupStack::PushL(self);
    self->ConstructL(webFrame, selfDownloadTypes);
    CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
SelfDownloadContentHandler::~SelfDownloadContentHandler()
{
    delete m_ContentFile;
    m_ContentFile = NULL;
    delete m_ContentType;
    m_ContentType = NULL;
    delete m_Charset;
    m_Charset = NULL;
    delete m_ReqUrl;
    m_ReqUrl = NULL;
    delete m_SelfDlMimeTypes;
    m_SelfDlMimeTypes = NULL;
}

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::IsSupported
// -----------------------------------------------------------------------------
TInt SelfDownloadContentHandler::IsSupported(
    const ResourceRequest& request,
    const ResourceResponse& response,
    RHTTPTransaction& httpTransaction)
{
    TPtrC respContType(response.mimeType().des());
    if(IsSelfDownloadContent(respContType)) {
        return KErrNone;
    } else {
        // todo: check to see if we can do content recognition; if we can then
        // return KErrNotReady so that we get called again later with
        // response data - then return either KErrNone (supported) or
        // KErrNotSupported if recognized content type is not supported.
        /* for now, just... */ return KErrNotSupported;
    }
}

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::MHFRunL
// -----------------------------------------------------------------------------
void SelfDownloadContentHandler::MHFRunL(
    RHTTPTransaction httpTransaction,
    const THTTPEvent& httpEvent)
{
    switch( httpEvent.iStatus ) {
        case THTTPEvent::EGotResponseHeaders: {
            // we should not get called with EGotResponseHeaders event, since the decision
            // to pass response to this class is made by WebCore, and flow to this class
            // is via the HandleResponseHeadersL() call
            User::Leave(KErrNotSupported);
        }
        case THTTPEvent::EGotResponseBodyData: {
            HandleResponseBodyL(httpTransaction);
            break;
        }
        case THTTPEvent::EResponseComplete: {
            // do not mix it up with the ESucceeded
            // The transaction's response is complete. An incoming event.
            break;
        }
        case THTTPEvent::ESucceeded: {
            ResponseCompleteL(httpTransaction, KErrNone);
            break;
        }
        case THTTPEvent::ERequestComplete: {
            // request is all set
            break;
        }
        case THTTPEvent::EFailed:
        case THTTPEvent::EUnrecoverableError:
        case KErrAbort:
        default: {
            HandleError(httpEvent.iStatus);
            break;
        }
    }
}

TInt SelfDownloadContentHandler::MHFRunError(
    TInt aError,
    RHTTPTransaction aTransaction,
    const THTTPEvent &aEvent)
{
    TInt status(aError);
    __DEBUGGER();
    return status;
}

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::ProcessHeadersL
//
//
// -----------------------------------------------------------------------------
//
TInt SelfDownloadContentHandler::HandleResponseHeadersL(
    const ResourceRequest& request,
    const ResourceResponse& response,
    RHTTPTransaction& httpTransaction)
{
    TInt status( KErrNone );
    // cleanup content file, parameter list and data type
    // from prior response (if any)
    delete m_ContentFile;
    m_ContentFile = NULL;
    delete m_ContentType;
    m_ContentType = NULL;
    delete m_Charset;
    m_Charset = NULL;
    delete m_ReqUrl;
    m_ReqUrl = NULL;

    // ensure that certain needed values are in place:
    __ASSERT_DEBUG(m_WebFrame,
        User::Panic(_L("SelfDownloadContentHandler"), KErrArgument) );
    const TDesC8& reqUrl = response.url().des();
	
    __ASSERT_DEBUG(reqUrl.Length() > 0,
        User::Panic(_L("SelfDownloadContentHandler"), KErrArgument) );
    const TDesC& charset = response.textEncodingName().des();

    const TDesC& contentType = response.mimeType().des();
    __ASSERT_DEBUG( contentType.Length() > 0,
        User::Panic( _L("DefaultContentHandler"), KErrArgument) );
    //
    TUriParser8 parser;
    HBufC* fileName = NULL;
    TInt pStatus = parser.Parse(reqUrl);
    if ( pStatus == KErrNone ) {
        TPtrC8 path = parser.Extract(EUriPath);
        TInt index = path.LocateReverse('/');
        if ( index >= 0 ) {
            // Get the filename and extension
            path.Set( path.Right( path.Length() - index - 1  ) );
			fileName = HBufC::NewL(path.Length());
            fileName->Des().Copy(path);
        }
    }
    CleanupStack::PushL(fileName);
    m_ContentFile = CTempFile::NewL(fileName);
    CleanupStack::PopAndDestroy(fileName);
    //        
    // Save these for handling completed download
    m_ContentType = contentType.AllocL();
    m_Charset = charset.Alloc();
    m_ReqUrl = HBufC::NewL(reqUrl.Length());
    m_ReqUrl->Des().Copy(reqUrl);
    return status;
}

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::ProcessResponseL
// -----------------------------------------------------------------------------
TInt SelfDownloadContentHandler::HandleResponseBodyL(
    RHTTPTransaction& httpTransaction)
{
    TInt status(KErrNone);

    // ensure that content file exists
    __ASSERT_DEBUG(m_ContentFile, User::Panic( _L("SelfDownloadContentHandler"), KErrGeneral));
    // write incoming data to the content file
    MHTTPDataSupplier* body = httpTransaction.Response().Body();
    TPtrC8 chunkPtr;
    body->GetNextDataPart(chunkPtr);
    if (chunkPtr.Length()) {
        status = m_ContentFile->WriteL(chunkPtr);
    }
    body->ReleaseData();
    // ensure that we successfully wrote data to content file
    __ASSERT_DEBUG(status == KErrNone, User::Panic( _L("SelfDownloadContentHandler"), KErrGeneral));
    return status;
}

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::ResponseCompleteL
// -----------------------------------------------------------------------------
TInt SelfDownloadContentHandler::ResponseCompleteL(RHTTPTransaction httpTransaction,
    TInt error)
{
    TInt status(error);
    // ensure that content file name exists
    __ASSERT_DEBUG(m_ContentFile->Name().Ptr(), User::Panic( _L("SelfDownloadContentHandler"), KErrArgument) );
    m_ContentFile->CloseL();

    // send data to the host application
    if(m_SpecialLoadObserver) {
        RArray<TUint>* typeArray = new (ELeave) RArray<TUint>(4);
        CleanupStack::PushL( typeArray );

        CDesCArrayFlat* desArray = new (ELeave) CDesCArrayFlat(4);
        CleanupStack::PushL(desArray);
        //
        typeArray->Append(EParamRequestUrl);
        desArray->AppendL(m_ReqUrl->Des()); 
        typeArray->Append(EParamCharset);
        desArray->AppendL(m_Charset->Des());
        typeArray->Append(EParamReceivedContentType);
        desArray->AppendL(m_ContentType->Des());
        typeArray->Append(EParamLocalFileName);
        desArray->AppendL(m_ContentFile->Name());
        //
        m_SpecialLoadObserver->HandleDownloadL(typeArray, desArray);
        //
        typeArray->Close();
        desArray->Reset();
		CleanupStack::PopAndDestroy(2);//desArray, typeArray,
        // todo: do not delete content file yet - needed by CDocumentHandler
        // can we ask SpecialLoadObserver to clean up when handling app
        // returns and file no longer needed?
		
		// remove the selfDownloadCallbackStr property
        RHTTPTransactionPropertySet propSet = httpTransaction.PropertySet();
		RStringPool strPool = httpTransaction.Session().StringPool();

		propSet.RemoveProperty( strPool.StringF(HttpFilterCommonStringsExt::ESelfDownloadCallback,
                                    HttpFilterCommonStringsExt::GetTable()) );
    }
	StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->ResetOutstandingSelfDl();
    return status;
}

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::HandleError
//
//
// -----------------------------------------------------------------------------
//
void SelfDownloadContentHandler::HandleError(
    /* TInt aTransactionId,*/
    TInt error)
{
    // todo: Complete should be called and cleanup occurs there.
}

// -----------------------------------------------------------------------------
// CDefaultContentHandler::IsSelfDownloadContentL
//
// -----------------------------------------------------------------------------
//
TBool SelfDownloadContentHandler::IsSelfDownloadContent(
    TDesC& contentType)
{
    // Check if the Host Application included this MIME type in the self-download
    // content types
    TInt index(m_SelfDlMimeTypes->FindF(contentType));
    return (index != KErrNotFound);
    }

// -----------------------------------------------------------------------------
// SelfDownloadContentHandler::ReinitializeSpecialLoadObserver
// -----------------------------------------------------------------------------
void SelfDownloadContentHandler::ReinitializeSpecialLoadObserver(WebFrame* webFrame)
{
    //Reinitializes the m_SpecialLoadObserver other wise this will take the old refrence because 
    //SelfDownloadContentHandler is static object.
    m_SpecialLoadObserver = control(webFrame)->brCtlSpecialLoadObserver();
}

/*TBool SelfDownloadContentHandler::RecognizeContentTypeL(
    TPtr aContentType)
{
    TDataRecognitionResult dataType;
    RApaLsSession apaSession;
    User::LeaveIfError(apaSession.Connect());
    //
	// we need a buffer to hold data from the file
	TInt bufferSize = 0;
    TInt seekPosition = 0;
	apaSession.GetMaxDataBufSize(bufferSize);
	HBufC8* buffer = HBufC8::NewLC(bufferSize);
	TPtr8 buf = buffer->Des();
	RFile file;
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    //
    if(file.Open(fs, iContentFile->Name(), EFileShareAny | EFileRead ) != KErrNone) {
        return EFalse;
    }
    file.Seek(ESeekStart, seekPosition);
	file.Read(buf);
    file.Seek(ESeekStart, seekPosition);
    TInt ret = apaSession.RecognizeData(iContentFile->Name(), buf, dataType);
    apaSession.Close();
	CleanupStack::PopAndDestroy(2); //fs, buffer
    //
    if( ret == KErrNone &&
        (dataType.iConfidence == CApaDataRecognizerType::ECertain) ||
        (dataType.iConfidence == CApaDataRecognizerType::EProbable)) {
        // if Content-Type found - save it to header info
        aContentType.Copy( dataType.iDataType.Des() );
        return ETrue;
    } else {
        return EFalse;
//    }
}*/

//  End of File
