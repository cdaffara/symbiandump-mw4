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

#include <f32file.h>
#include <uri8.h>
#include <escapeutils.h>
#include <apmrec.h>
#include <apgcli.h>
#include "ResourceHandle.h"
#include "FileConnection.h"
#include "FileReader.h"
#include "ResourceHandleManagerSymbian.h"
#include "StaticObjectsContainer.h"
#include "ResourceRequest.h"
#include <BrCtlSpecialLoadObserver.h>
#include "BrCtl.h"
#include "DeprecatedString.h"

const TInt KFileReadChunkSize = 23920; // bytes
_LIT8( KResLoaderUCS2, "iso-10646-ucs-2" );
_LIT8( KResLoaderWapWmlc, "application/vnd.wap.wmlc" );


static int s_fileTransactionsCount = 0;

using namespace WebCore;

FileConnection::FileConnection(ResourceHandle* _handle) : MUrlConnection(_handle)
{
    m_fileName = 0;
    m_fileReader = 0;
    s_fileTransactionsCount++;
    m_chunkIndex = 0;
    m_charset = 0;
    m_contentType = 0;
}

FileConnection::~FileConnection()
{
    s_fileTransactionsCount--;
    delete m_fileName;
    delete m_fileReader;
    delete m_charset;
    delete m_contentType;
    m_file.Close();
}

int FileConnection::submit()
{
    TRAPD(error, submitL());
    return error;
}

void FileConnection::submitL()
{
    TPtrC8 urlPtr( m_handle->request().url().des() );
    m_fileName = parseFileNameLC( urlPtr );
    int submited;
    if( m_fileName ) {
        CleanupStack::Pop(); // m_fileName
        submited = m_file.Open( StaticObjectsContainer::instance()->fsSession(), *m_fileName, EFileRead|EFileShareReadersOnly );
        if( submited == KErrNone ) {
            // set content max size info
            TInt fileSize;
            // ignore return value
            m_file.Size( fileSize );
            m_maxSize = fileSize;
            // schedule a file read
            m_fileReader = CFileReader::NewL( m_file, KFileReadChunkSize, this );
            m_fileReader->StartReading();
        }
    }
    else {
        submited = KErrArgument;
    }
    User::LeaveIfError(submited);
}

// -----------------------------------------------------------------------------
// FileConnection::parseFileNameLC
// Translate the file name from a URL to a valid file name in the system.
// -----------------------------------------------------------------------------
//
HBufC* FileConnection::parseFileNameLC(const TDesC8& aUrl )
{
    
	//if url has mix of \ and / chars the parser gets confused in extracting the host and the path.
	HBufC8* url = HBufC8::New( aUrl.Length() );
	url->Des().Copy( aUrl );
	TPtr8 urlPtr( url->Des() );
	for( TInt i = 0; i < urlPtr.Length(); i++ ) {
		if( urlPtr[ i ] == '\\' ) {
			urlPtr[ i ] = '/';
		}
	}
	
	TInt status;
    TUriParser8 parser;
    HBufC8* fileName = NULL;
	status = parser.Parse( urlPtr );
    if( status == KErrNone ) {
        // this must be the drive letter
        TPtrC8 host = parser.Extract( EUriHost );
        TPtrC8 path = parser.Extract( EUriPath );
        // add missing ":"
        fileName = HBufC8::NewLC( (host.Length()?host.Length()+1:0) + path.Length() );
        TPtr8 fileNamePtr( fileName->Des() );
        if( host.Length() == 1 ) {
            // fix c to c:
            fileNamePtr.Copy( host );
            fileNamePtr.Append( _L(":") );
        }
        else {
            // according to symbian uri parser in case of
            // file:///c:\foo.html
            // host: /c:\foo.html
            // path: /c:\foo.html
            // remove: / from the begenning of the path
            if( path.Length() > 2 && path[0] == '/' && path[2] == ':' ) {
                // move pointer from "/c:\" to "c:\"
                path.Set( path.Mid( 1 ) );
            }
        }
        HBufC8* fileUriPath = NULL;
        TRAP_IGNORE(fileUriPath = EscapeUtils::EscapeDecodeL(path));    
        if (fileUriPath) {
            fileNamePtr.Append(*fileUriPath);    
        }
        else {            
            fileNamePtr.Append( path );
        }        
        delete fileUriPath;
        // fix slashes
        for( TInt i = 0; i < fileNamePtr.Length(); i++ ) {
            if( fileNamePtr[ i ] == '/' ) {
                fileNamePtr[ i ] = '\\';
            }
        }
    }
    HBufC* fileName16 = NULL;
    if( fileName ) {
        fileName16 = HBufC::NewL( fileName->Length() );
        fileName16->Des().Copy( *fileName );
        CleanupStack::PopAndDestroy(); // filename
        CleanupStack::PushL( fileName16 );
    }
	delete url;
    return fileName16;
}

void FileConnection::cancel()
{
    if( m_fileReader ) {
        m_fileReader->StopReading();
        complete( KErrCancel ); 
    }
}

void FileConnection::download(WebCore::ResourceHandle* handle,
                              const WebCore::ResourceRequest& request,
                              const WebCore::ResourceResponse& response)
{
    // stop reading and close the file
    m_fileReader->StopReading();
    m_cancelled = true;
    m_file.Close();
    // send data to the host application
    MBrCtlSpecialLoadObserver* loadObserver = StaticObjectsContainer::instance()->brctl()->brCtlSpecialLoadObserver();
    if(loadObserver) {
        RArray<TUint> typeArray;
        CDesCArrayFlat* desArray = new (ELeave) CDesCArrayFlat(4);
        CleanupStack::PushL(desArray);
        //
        HBufC* url = HBufC::NewLC(handle->request().url().url().length());
        url->Des().Copy(handle->request().url().des());
        typeArray.Append(EParamRequestUrl);
        desArray->AppendL(*url);

        typeArray.Append(EParamCharset);
        desArray->AppendL(*m_charset);

        typeArray.Append(EParamReceivedContentType);
        desArray->AppendL(*m_contentType);

        typeArray.Append(EParamLocalFileName);
        desArray->AppendL(*m_fileName);
        //
        loadObserver->HandleDownloadL(&typeArray, desArray);
        //
        typeArray.Close();
        desArray->Reset();
        CleanupStack::PopAndDestroy(2); // desArray, url 
    }
}

// -----------------------------------------------------------------------------
// FileConnection::contentTypeL
// Determine the content type of the file.
// -----------------------------------------------------------------------------
HBufC8* FileConnection::contentTypeL()
{
    TPtrC8 contentTypePtr;
    HBufC8* contentType = NULL;
    TDataRecognitionResult dataType;
    RApaLsSession apaSession;
    TInt ret;
    
    User::LeaveIfError( apaSession.Connect() );
    // Ask the application architecture to find the file type
    TPtrC8 chunkPtr;
    
    m_fileReader->GetChunkBuffer( chunkPtr );
    ret = apaSession.RecognizeData( m_fileName->Des(), chunkPtr, dataType );
    apaSession.Close();
    
    if( ret == KErrNone &&
        ( dataType.iConfidence == CApaDataRecognizerType::ECertain ) ||
        ( dataType.iConfidence == CApaDataRecognizerType::EProbable ) ) {
        // If the file type was found, try to match it to a known file type
        contentTypePtr.Set( dataType.iDataType.Des8() );
    }
    else {
        // extensions
        _LIT( KCssExt, ".css" );
        _LIT(KWbmpExt, ".wbmp");
        _LIT(KEcmaScriptExt, ".es");
        _LIT(KJavaScriptExt, ".js");
        
        TPtrC extPtr( m_fileName->Right( m_fileName->Length() -  m_fileName->LocateReverse('.') ) );
        
        if( extPtr.CompareF( KCssExt() ) == 0 ) {
            contentTypePtr.Set( _L8( "text/css" ) );
        }
        else if( extPtr.CompareF( KWbmpExt() ) == 0 ) {
            contentTypePtr.Set( _L8( "image/vnd.wap.wbmp" ) );
        }
        else if( extPtr.CompareF( KEcmaScriptExt() ) == 0 ||
            extPtr.CompareF( KJavaScriptExt() ) == 0 ) {
            contentTypePtr.Set( _L8( "text/ecmascript" ) );
        }

        // todo plugin is missing
        // Check if it is a supported plugin
        // CPluginHandler* pluginHandler = CPluginHandler::GetSingleton();
        // TUint16* mimeType16 = pluginHandler->GetPluginMimeTypeL(iFileName);
    }
    if( contentTypePtr.Length() ) {
        contentType = HBufC8::NewL( contentTypePtr.Length() );
        contentType->Des().Copy( contentTypePtr );
    }
    return contentType;
}

// -----------------------------------------------------------------------------
// FileConnection::contentEncoding
// Determine the content encoding of the file.
// -----------------------------------------------------------------------------
//
TPtrC8 FileConnection::contentEncoding(const TDesC8& aContentTypeString ) const
{
    // Assume Latin-1 for xhtml and html. ucs2 for any other
    TPtrC8 charset( KNullDesC8 );

    TPtrC8 httpAppString( KResLoaderWapWmlc );
    TPtrC8 contentString( aContentTypeString );

    // Is the contentType a HTTP_application_vnd_wap_wmlc_string
    TUint index = contentString.FindF( httpAppString );
    if( index == 0 ) {
        // This is a HTTP_application_vnd_wap_wmlc_string
        charset.Set( KResLoaderUCS2 );
    }
    return charset;
}

void FileConnection::response()
{
    TRAPD(error, responseL());
    if (error) {
        complete(error);
    }        
}

void FileConnection::responseL()
{
    if (m_chunkIndex == 0 ) {
        HBufC8* contentType = contentTypeL();

        if( contentType ) {
            m_contentType = HBufC::NewL(contentType->Length());
            m_contentType->Des().Copy(*contentType);

            TPtrC8 contentEncodingPtr( contentEncoding( contentType->Des() ) );
            ResourceResponse response(m_handle->request().url().des(), contentType->Des(), m_maxSize, contentEncodingPtr, String() );


            const TDesC& charset = response.textEncodingName().des();
            m_charset = charset.AllocL();
            response.setHTTPStatusCode(200);

            CResourceHandleManager::self()->receivedResponse(m_handle, response, this);
        }
        
       delete contentType;
    }
    // If it is not Browser content, reading the file is canceled in FileConnection::download().
    if (!m_cancelled) {
        TPtrC8 chunkPtr;
        m_fileReader->GetChunkBuffer( chunkPtr );    
        m_chunkIndex++;
        CResourceHandleManager::self()->receivedData(m_handle, chunkPtr, m_maxSize, this);
    }
}

void FileConnection::complete(int error)
{
    CResourceHandleManager::self()->receivedFinished(m_handle, error, this);
    derefHandle();
}


// end of file
