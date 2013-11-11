/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares HttpServer class.
*
*/


// INCLUDE FILES
#include <libc/limits.h>
#include "upnphttpbuffer.h"
#include "upnphttpserver.h"
#include "upnpcons.h"
#include "upnphttpsession.h"
#include "upnphttpfileaccess.h"
#include "upnphttpfiletransferreader.h"
#include "upnphttpchunkparser.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "upnpcons.h"
#include "httperr.h"
// CONSTANTS

const TInt KMaxBufferLength = 10240;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpBuffer* CUpnpHttpBuffer::NewL( CUpnpHttpSession* aSession, TBool aToBeSavedInFile )
	{
LOGS("CUpnpHttpBuffer::NewL(CUpnpHttpSession*, TBool)");
	CUpnpHttpBuffer* self = new (ELeave) CUpnpHttpBuffer( aSession, aToBeSavedInFile );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
	return self;
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::~CUpnpHttpBuffer
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpHttpBuffer::~CUpnpHttpBuffer()
	{
LOGS("CUpnpHttpBuffer::~CUpnpHttpBuffer()");

	iBuffer.Close();
    delete iChunkParser;
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::CUpnpHttpBuffer
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpBuffer::CUpnpHttpBuffer( CUpnpHttpSession* aSession, TBool aToBeSavedInFile )
	{
	iContentLength = -1;
	iSession = aSession;
	iToFile = aToBeSavedInFile;
	iContentLengthCounter = 0;
	iErrorCodeCheckedForSaving = EFalse;
	iOkToSaveToFile = EFalse;
	iFileReadBufferSize = aSession->HttpServer()->FileReadBufferSize();
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
void CUpnpHttpBuffer::ConstructL()
	{
	iBuffer.CreateL( 0 );
	iChunkParser = CUpnpHttpChunkParser::NewL();
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::AppendHeadersL
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpBuffer::AppendHeadersL( TDesC8& aBuffer, TBool aToBeSavedInFile )
{
	TInt PosOfDoubleLineFeed ( KErrNotFound );
	
	LOGS("CUpnpHttpBuffer::AppendL(TDesC8&)");
	
	if ( !IsHeaderReady() )
		{
		 iToFile = aToBeSavedInFile;
		// header not yet ready. Checking if it gets ready with this received buffer
		PosOfDoubleLineFeed = aBuffer.FindF( UpnpString::KDoubleLineFeed );
		
//trying to find double linefeed by spliiitng line
		
			if (iBuffer.Length() > 0)
				{
				if( !iBuffer.Right(UpnpString::KLineFeedLength).CompareF( UpnpString::KLineFeed) 
					&& !aBuffer.FindF(UpnpString::KLineFeed) )
					{
					PosOfDoubleLineFeed = iBuffer.Length() - UpnpString::KLineFeedLength;
					}				
				else if	(!iBuffer.Right(UpnpString::KCRLength).CompareF(UpnpString::KCR) 
					&& !aBuffer.FindF(UpnpString::KLFCRLF))				
					{
					PosOfDoubleLineFeed = iBuffer.Length() - UpnpString::KCRLength;
					}
				else if	(!iBuffer.Right(UpnpString::KCRLFCRLength).CompareF(UpnpString::KCRLFCR)  
					&& !aBuffer.FindF(UpnpString::KLinefeed))				
					{
					PosOfDoubleLineFeed = iBuffer.Length() - UpnpString::KCRLFCRLength;
					}
				//if double linefeed there are not found by splitting line
				//may be it (all double linefeed) is in ne line	
				else if ( PosOfDoubleLineFeed != KErrNotFound )
					{
					PosOfDoubleLineFeed += 	iBuffer.Length();			
					}
				}
		
		if ( PosOfDoubleLineFeed == KErrNotFound )
			{
			// double linefeed not found, so only adding unfinished headers to 
			// iBuffer
			
			if (iBuffer.Length() <= KMaxBufferLength )    //10kb
				{
				    iBuffer.ReAllocL( iBuffer.Length() + aBuffer.Length() );
					iBuffer.Append( aBuffer );
				}
			
			}
		else
			{
			//  Double linefeed found. Appending headers and then checking if 
			// response is OK.
			TInt afterDoubleLineFeed = PosOfDoubleLineFeed + UpnpString::KDoubleLineFeedLength - iBuffer.Length();
			iBuffer.ReAllocL(PosOfDoubleLineFeed + UpnpString::KDoubleLineFeedLength);
			iBuffer.Append( aBuffer.Left(afterDoubleLineFeed));

            // is headers checked
    		if( !iErrorCodeCheckedForSaving )
    		    {
    		    TInt messageError = MessageErrorCode( iBuffer );
    		    TPtrC8 method = Method();
    		    TPtrC8 target = Target();
    		    
    		    if(method == KHttpPost ) 
    		        {
    		        messageError = EHttpOk;
    		        }
    		    
    		    _LIT8( KControl, "/control");
    		    
    		    if( method.FindC( KHttpPost() ) == 0 && 
    		        method.Length() == KHttpPost().Length() 
    		        )
    		        {
        		    TInt lastSlash = target.LocateReverse('/');
        		    if( lastSlash > KErrNotFound )
        		        {
            		    target.Set( target.Mid( lastSlash ) );
            		    }
                    }
    		    
    		    if ( messageError == EHttpOk || messageError == EHttpPartialContent )
    		        {
                    iOkToSaveToFile = ETrue;
    		        }
    		        
    		    // if EXPORTING, then saving to file
    		    else if( method.FindC( KHttpPost() ) == 0 &&
                            method.Length() == KHttpPost().Length() &&
                            target.FindC( KControl ) != 0 )
                    {
                    iOkToSaveToFile = ETrue;
    		        }
    		    else
    		        {
    		        iOkToSaveToFile = EFalse;
    		        CUpnpHttpFileAccess* temp = iSession->FileAccess();
    		        // Delete only downloaded file if it is not complete.
       		        if( temp && temp->Download() )
	       		        {
                        temp->DeleteFile();
	                    // no delete, not owned
	        		    temp = NULL;
	       		        }
    		        }
    		    // error code checked, no need to check it again.
    	        iErrorCodeCheckedForSaving = ETrue;
    		    }

    		return afterDoubleLineFeed;
			}
		}

	return PosOfDoubleLineFeed;
}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::AppendL
// This function appends the aBuffer data at the end of this HTTPBuffer.
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpBuffer::AppendL( TDesC8& aBuffer, TBool aToBeSavedInFile)
	{
	TInt retVal ( KErrNone );
	
	LOGS("CUpnpHttpBuffer::AppendL(TDesC8&)");
	
	if ( !IsHeaderReady() )
		{
		TInt PosOfDoubleLineFeed = AppendHeadersL( aBuffer, aToBeSavedInFile );

		if ( PosOfDoubleLineFeed > KErrNotFound )
		{
			TInt afterDoubleLineFeed = PosOfDoubleLineFeed + UpnpString::KDoubleLineFeedLength - iBuffer.Length();
    		TPtrC8 AppendPointer( aBuffer.Right( aBuffer.Length() - PosOfDoubleLineFeed ) );
			//for file upload, data copied to the file transfer reader
			if(iToFile && iOkToSaveToFile 
				&& iSession->FileTransferReader() && iSession->FileTransferReader()->Activated())
				{
				if(AppendPointer.Length() > 0)
					iSession->FileTransferReader()->AppendL(AppendPointer);
				}
			else if ( IsChunked() )
				{
				iPos =  iBuffer.Length();											
				retVal = ParseL( AppendPointer );
				}
			else
				{
				iBuffer.ReAllocL( iBuffer.Length() + aBuffer.Length() );
				iBuffer.Append( AppendPointer );				
				}
			}
		}
	else
		{
		if( !iErrorCodeCheckedForSaving )
		    {
		    TInt messageError = MessageErrorCode( iBuffer );
		    if ( messageError == EHttpOk || messageError == EHttpPartialContent )
		        {
                iOkToSaveToFile = ETrue;
		        }
		    else
		        {
		        iOkToSaveToFile = EFalse;
   		        CUpnpHttpFileAccess* temp = iSession->FileAccess();
   		        if( temp )
   		            {
    		        temp->DeleteFile();
                    // no delete, not owned
    		        temp = NULL;
    		        }
		        }
		    // error code checked, no need to check it again.
	        iErrorCodeCheckedForSaving = ETrue;
		    }
		
		//for file upload, data copied to the file transfer reader
		if(iToFile && iOkToSaveToFile &&
			iSession->FileTransferReader() && iSession->FileTransferReader()->Activated())
			{
			if(aBuffer.Length() > 0)
				iSession->FileTransferReader()->AppendL(aBuffer);
			return retVal;
			}
			    
		if ( IsChunked() )
			{
			retVal = ParseL( aBuffer );
			}
		else
			{
			iBuffer.ReAllocL( iBuffer.Length() + aBuffer.Length() );
			iBuffer.Append( aBuffer );
			}
	    }
	return retVal;
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::ParseL
// Decodes chunked messages.
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpBuffer::ParseL( TDesC8& aBuffer )
    {
    iBuffer.ReAllocL( iBuffer.Length() + aBuffer.Length() );
    iBuffer.Append( aBuffer );
    return Parse( iBuffer, iPos ) < 0 ? KErrHttpUnknownParseState: KErrNone;                  
    }

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::Parse
// Decodes chunked-encoded buffer
// -----------------------------------------------------------------------------
//	
TInt CUpnpHttpBuffer::Parse(TDes8& aBuffer, TInt& aPos)
    {    
    if( aPos >= aBuffer.Length() )
        {
        iChunkParser->Reset();
        return KErrNone;
        }
    return iChunkParser->Parse( aBuffer, aPos );		
    }

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::IsReady
// Returns ETrue, if buffer is ready to be converted to HTTPMessage 
// else returns EFalse.
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpBuffer::IsReadyL()
	{
LOGS("CUpnpHttpBuffer::IsReady()");
	if ( IsHeaderReady() )
		{
		//Assuming here that HTTP GET -requests may not have a body.
		//So if a HTTP GET request arrives and it has a CONTENT-TYPE or a 
		//CONTENT-LENGHT header we may still serve the file
		TPtrC8 method = Method();
		if ( method.FindC( KHttpGet() ) == 0 )
		    {
		    return ETrue;
		    }
		if ( HasBody() )
			{
			return IsBodyReadyL();
			}
		else 
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::IsHeaderReady
// Returns ETrue, if header part of HTTPMessage is ready, else returns EFalse.
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpBuffer::IsHeaderReady()
	{
LOGS("CUpnpHttpBuffer::IsHeaderReady()");
	if ( iBuffer.Find( UpnpString::KDoubleLineFeed() ) > KErrNotFound )
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::IsBodyReadyL
// Returns ETrue, if body part of HTTPMessage is ready, else returns EFalse.
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpBuffer::IsBodyReadyL()
	{
LOGS("CUpnpHttpBuffer::IsBodyReadyL()");
	TInt bodyStart = iBuffer.FindF( UpnpString::KDoubleLineFeed );
	TInt contLength = ContentLengthL();
	if ( IsChunked() )
		{
		return ChunkTransferFinished();
		}
	else
		{
		if ( iHasContentType )
			{	
			TPtrC8 body = iBuffer.Mid( bodyStart + 
                UpnpString::KDoubleLineFeed().Length() );
			return body.Find( UpnpString::KDoubleLineFeed() ) != KErrNotFound;
			}
		}

	TPtrC8 tempPtr;
	if ( iToFile && iOkToSaveToFile )
		{
		if ( iContentLengthCounter >= contLength )
			{
			return !(IsChunked() && !ChunkTransferFinished());
			}
		else
			{
			return EFalse;		
			}
		}
	else
		{
		tempPtr.Set( iBuffer.Mid( bodyStart + UpnpString::KDoubleLineFeedLength ) );		
		if (  tempPtr.Length() >= contLength )
			{
			return !(IsChunked() && !ChunkTransferFinished());
			}
		else
			{
			return EFalse;
			}
		}
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::BytesInBuffer
// Returns the length of entire buffer.
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpBuffer::BytesInBuffer()
	{
LOGS("CUpnpHttpBuffer::BytesInBuffer()");
	return iBuffer.Length();
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::RemoveL
// This function deletes aLength bytes from the beginning of HTTPBuffer.
// -----------------------------------------------------------------------------
//
void CUpnpHttpBuffer::RemoveL( TInt aLength )
	{
LOGS1("CUpnpHttpBuffer::RemoveL(%i)", aLength);
	if ( aLength <= iBuffer.Length() )
		{
		iBuffer.Delete( 0, aLength );
		}
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::ChunkTransferFinished
// Returns ETrue, if chunk encoded message is finished.
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpBuffer::ChunkTransferFinished()
	{
LOGS("CUpnpHttpBuffer::ChunkTransferFinished()");
	return iChunkParser->IsFinished();
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::HasBody
// Returns ETrue, if message has body, else returns EFalse.
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpBuffer::HasBody()
	{
LOGS("CUpnpHttpBuffer::HasBody()");
	if( IsChunked() )
		{
		return ETrue;
		}
TInt endOfHeader = iBuffer.Find( UpnpString::KDoubleLineFeed );
if (endOfHeader>0)
	{TPtrC8 tmp = iBuffer.Mid(0, endOfHeader);
	if ( tmp.FindC( UpnpHTTP::KContentLength() ) > -1 )
		{
		iHasContentType = EFalse;
		return ETrue;
		}
	else if ( tmp.FindC( UpnpHTTP::KContentType() ) > -1)
		{
		iHasContentType = ETrue;
		return ETrue;
		}
	else 
		{
		iHasContentType = EFalse;
		return EFalse;
		}
	}
else
	{
	iHasContentType = EFalse;
	return EFalse; //no end of the header, not possible to judge if there is a body	
	}
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::IsChunked
// Returns ETrue, if message's header contains "Transfer-encoding: 
// Chunked" -header.
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpBuffer::IsChunked()
	{
	LOGS("CUpnpHttpBuffer::IsChunked()");	
	TInt endOfHeader = iBuffer.Find( UpnpString::KDoubleLineFeed );	
		
	// to check only first 2k of data or whole portion with all headers 
	// if they are bigger	
	TPtrC8 TranferCodingPointer;
	if (endOfHeader <= UpnpSSDP::KMaxMessageLength)
		TranferCodingPointer.Set( iBuffer.Left( endOfHeader + UpnpString::KLineFeedLength ) ); 
	else 
		TranferCodingPointer.Set( iBuffer.Left( UpnpSSDP::KMaxMessageLength ) ); 

	TInt TransferEncodingIndex = 
        TranferCodingPointer.FindC( UpnpHTTP::TransferEncoding );

	if ( TransferEncodingIndex == KErrNotFound ) 
		{
		return EFalse;
		}
	else
		{
		TInt correct = 0;
		TInt transferHeaderLength = 0;
		correct = TransferEncodingIndex;
		transferHeaderLength = UpnpHTTP::TransferEncoding().Length();

		TPtrC8 tempPtr = iBuffer.Mid( correct + transferHeaderLength, 
            iBuffer.Length() - correct - transferHeaderLength );
	
		TInt LineFeedAfterTransferCoding = tempPtr.FindF( UpnpString::KLineFeed );

		TPtrC8 codingPtr = tempPtr.Left( LineFeedAfterTransferCoding );
		if ( codingPtr.FindC( UpnpHTTP::Chunked ) != KErrNotFound )
			{
			return ETrue;
			}
		else
			{
			return EFalse;
			}
		}
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::Headers
// Returns Headers of the first message in the buffer.
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpHttpBuffer::Headers()
	{
LOGS("CUpnpHttpBuffer::Headers()");
	TInt EndOfHeaders = iBuffer.FindC( UpnpString::KDoubleLineFeed );
	TPtrC8 headers = iBuffer.Left( EndOfHeaders + UpnpString::KDoubleLineFeedLength  );	
	return headers;
	}


// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::L
// Returns the content of buffer.
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpHttpBuffer::Content()
	{
LOGS("CUpnpHttpBuffer::Content()");
	TInt PosOfContLength = iBuffer.FindC( UpnpHTTP::KHdrContentLength() );
	TInt contLen = 0;
	
	if ( PosOfContLength > 0 )
		{
		TRAPD(error, contLen = ContentLengthL());
		if (error)
			{
			contLen = 0;
			}
		}
	else if ( iHasContentType )
		{
		return TPtrC8( iBuffer );
		}
	else if( IsChunked() )
		{
		TInt StartOfMessage = iBuffer.FindC( UpnpString::KDoubleLineFeed );
		contLen = iBuffer.Length() - StartOfMessage;	
		}
	/*else
		{
		TInt StartOfMessage = iBuffer.FindC( UpnpString::KDoubleLineFeed );
		contLen = iBuffer.Des().Length() - StartOfMessage;
		}*/	

	TInt endOfHeader = iBuffer.Find( UpnpString::KDoubleLineFeed );
	TInt length = 0;
	// Probably not all headers were sent
	
	if( endOfHeader == KErrNotFound  )
		{
		length = iBuffer.Length();
		}
	else
		{
		length = endOfHeader + contLen + UpnpString::KDoubleLineFeedLength;	
		}

    if( length < 0 ) // in case of overflow of length value
        {
        length = INT_MAX;
        }

	TPtrC8 message = iBuffer.Left( length );
	return message;
	}	

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::ContentLengthL
// Returns the content length of first message in the buffer.
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpBuffer::ContentLengthL()
	{
	LOGS("CUpnpHttpBuffer::ContentLengthL()");
	
	TInt endOfHeader = iBuffer.Find( UpnpString::KDoubleLineFeed );	
    User::LeaveIfError(endOfHeader);
	TPtrC8 findptr;
	// to check only first 2k of data or whole portion with all headers 
	// if they are bigger	
	if (endOfHeader >= UpnpSSDP::KMaxMessageLength)
		findptr.Set( iBuffer.Left( endOfHeader + UpnpString::KLineFeedLength ) ); 
	else 
		findptr.Set( iBuffer.Left( UpnpSSDP::KMaxMessageLength ) ); 						

	TInt contentLengthIndex = findptr.FindC( UpnpHTTP::KHdrContentLength() );

	TInt lengthOfContentLength = UpnpHTTP::KHdrContentLength().Length() +
        UpnpString::KColon().Length();
	
	if ( contentLengthIndex <= 0 ) 
		{
	return KErrNotFound;
		}

	TPtrC8 rest;
	rest.Set( findptr );
	rest.Set( rest.Mid( contentLengthIndex + lengthOfContentLength ) );

	TInt lineEnd = rest.Find( UpnpString::KLineFeed );
	if ( lineEnd < 0 )
		{
		return KErrNotFound;
		}
	
	// LWS and spanning headers
	TInt indexLWS = 0;
	TPtrC8 clRest = rest;
	TInt contentLengthValueEnd = lineEnd;
	
	while( ETrue )
	    {
	    indexLWS = CUpnpHttpMessage::FindLWS( clRest );
	    if( indexLWS == lineEnd )
	        {	     
	        clRest.Set( clRest.Mid( lineEnd + UpnpString::KLineFeed().Length() ) );
	        lineEnd = clRest.Find( UpnpString::KLineFeed );
	        contentLengthValueEnd += lineEnd + UpnpString::KLineFeed().Length();
	        }
	    else
	        {
	        break;
	        }
	    }			
    
	TPtrC8 length = rest.Left( contentLengthValueEnd );
    
    if( !length.Compare( KNullDesC8() ) )
        return -1;
    
    // Unfold header
    HBufC8* clValueBuf = length.AllocLC();
    TPtr8 ptrCLValue = clValueBuf->Des();
    
    // Remove CRLFs
    TInt pos;
	while( ( pos = ptrCLValue.FindC( UpnpString::KLineFeed() ) ) != KErrNotFound)
	    {
		ptrCLValue.Delete(pos, UpnpString::KLineFeedLength);
	    }
	
    // Remove spaces
    while( ( pos = ptrCLValue.FindC( UpnpString::KSpace() ) ) != KErrNotFound)
	    {
		ptrCLValue.Delete(pos, 1);
	    }
	
	// Remove tabs
    while( ( pos = ptrCLValue.FindC( UpnpString::KTab() ) ) != KErrNotFound)
	    {
		ptrCLValue.Delete(pos, 1);
	    }
   
	TLex8 lex( ptrCLValue );
	TInt val = 0;
	User::LeaveIfError(lex.Val( val ));
	
	CleanupStack::PopAndDestroy( clValueBuf );

	iContentLength = val;

	return val;
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::UsesConnectionClose
// Returns ETrue, if message's header contains "Connection: close" -header.
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpBuffer::UsesConnectionClose()
	{
LOGS("CUpnpHttpBuffer::UsesConnectionClose()");
	if(IsHeaderReady())
		{
		TPtrC8 headers=Headers();
		TInt posOfConn=headers.FindC(UpnpHTTP::KConnection());
		TInt posOfHttp10 = headers.FindC(KHttp10); 	
		
		if( posOfConn != KErrNotFound )
			{
			TPtrC8 conn=headers.Mid(posOfConn+UpnpHTTP::KConnection().Length() +
                UpnpString::KColon().Length());
			
			TInt posOfLineFeed=conn.FindC(UpnpString::KLineFeed());
			if(posOfLineFeed != KErrNotFound)
				{
				conn.Set(conn.Left(posOfLineFeed));
				
				while((conn.Left(1)).CompareC(UpnpString::KSpace()) == 0)
					{
					conn.Set(conn.Mid(1)); // removing spaces in front of value
					}
					
				if(conn.FindC(UpnpHTTP::KClose()) >= 0)
					{
					return ETrue;
					}
				else
					{
					return EFalse;
					}
				}
			else
				{
				// If rsp = 'HTTP/1.0', we can assume that it contains 'connection:close' header, even if it doesn't!	
		                   if( posOfHttp10 != KErrNotFound )
		                        return ETrue;
				return EFalse;
				}
			}
		else
			{
		         // If rsp = 'HTTP/1.0', we can assume that it contains 'connection:close' header, even if it doesn't!	
		         if( posOfHttp10 != KErrNotFound )
		             return ETrue;
			return EFalse;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::MessageErrorCode
// Checks error code of received message.
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpBuffer::MessageErrorCode( TDesC8& aBuffer )
    {
    if( IsHeaderReady() )
        {
    	TInt posOfNextLine = aBuffer.Find(UpnpString::KLineFeed); 
    	if( posOfNextLine != KErrNotFound )
    		{
    		TPtrC8 spacePtr;
    		spacePtr.Set( aBuffer );
    		
    		// finding first space
    		TInt posOfFirstSpace = spacePtr.Find(UpnpString::KSpace);
            if ( posOfFirstSpace != KErrNotFound )
                {
    		    spacePtr.Set( spacePtr.Left( posOfFirstSpace ) );

                }

    		// back to basic handling
    		spacePtr.Set( aBuffer );
    		
    		if( posOfFirstSpace < posOfNextLine && posOfFirstSpace != KErrNotFound )
    			{
    			// if first space is found, it has to be before linefeed
    			// +1 comes from going over the first space, we are interested
    			// in what comes after it.
    			spacePtr.Set( spacePtr.Mid( posOfFirstSpace  + 1 ) );
    			TInt posOfSecondSpace = spacePtr.Find(UpnpString::KSpace);
    			if( posOfSecondSpace < posOfNextLine && posOfSecondSpace != KErrNotFound )
    				{
    				// check that length of path is longer than zero.
    				if( posOfSecondSpace > 0 )
    					{
                        // now setting spacePtr so that we have the error number in spacePrt
    					spacePtr.Set( spacePtr.Left( posOfSecondSpace ) );

                        // making conversion
          				TLex8 errorValue( spacePtr );
                        TInt messageErrorValue( 0 );                        
        				TInt convError = errorValue.Val( messageErrorValue );
        				
        				// checking that conversion was ok
        				if( convError == KErrNone )
        				    {
        				    return messageErrorValue;
        				    }
        				else
        				    {
        				    return KErrAbort;
        				    }
        				
    					}
    				else
    					{
    					// header too short. Invalid message
    					return KErrAbort;
    					}
    				}
    			else
    				{
    				// no second space found. Invalid message.
    				return KErrAbort;
    				}
    			}
    		else
    			{
    			// no space found before linefeed. Invalid message
    			return KErrAbort;
    			}
     		}
		else
			{
			// no linefeed found in headers. Invalid message
			return KErrAbort;
			}        
        }
    else
        {
        return KErrAbort;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::Method
// returns the method of message
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpHttpBuffer::Method()
    {
    if( IsHeaderReady() )
        {
        TPtrC8 ret;
        ret.Set( Headers() );
        TInt posOfSpace = ret.Locate(' ');
        if( posOfSpace != KErrNotFound )
            {
            ret.Set( ret.Left( posOfSpace ) );
            return ret;
            }
        else
            {
            return KNullDesC8();
            }
        
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::Target
// 
// -----------------------------------------------------------------------------
//    
TPtrC8 CUpnpHttpBuffer::Target()
    {
    if( IsHeaderReady() )
        {
        TPtrC8 ret;
        ret.Set( Headers() );
        TInt posOfSpace = ret.Locate(' ');
        if( posOfSpace != KErrNotFound )
            {
            ret.Set( ret.Mid( posOfSpace +1 ) );
            posOfSpace = ret.Locate(' ');
            if( posOfSpace != KErrNotFound )
                {
                ret.Set( ret.Left( posOfSpace ) );
                return ret;
                }
            else
                {
                return KNullDesC8();
                }
            }
        else
            {
            return KNullDesC8();
            }
        
        }
    else
        {
        return KNullDesC8();
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::IsToFile
// Returns True if message content is going to be saved in file.
// -----------------------------------------------------------------------------
//    
TBool CUpnpHttpBuffer::IsToFile()
	{
	return iToFile;		
	}

// -----------------------------------------------------------------------------
// CUpnpHttpBuffer::SetToFile
// Sets if body of message contained in buffer should be saved in file.
// -----------------------------------------------------------------------------
//        
void CUpnpHttpBuffer::SetToFile( TBool aToFile )
	{
	iToFile = aToFile;			
	}


// End Of File
