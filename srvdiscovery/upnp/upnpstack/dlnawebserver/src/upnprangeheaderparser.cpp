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
* Description:  Declares CUpnpRangeHeaderParser class.
*
*/

#include "upnphttpservertransactioncreator.h"
#include "upnphttpmessage.h"
#include "upnprangeheaderparser.h"
#include "upnphttpservertransaction.h"
#include "upnphttpservertransaction.h"

// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::NewL
// Factory method.
// ---------------------------------------------------------------------------
//
CUpnpRangeHeaderParser* CUpnpRangeHeaderParser::NewL(
    CUpnpHttpServerTransaction &aTransaction, TInt& aStartPos, TInt& aEndPos )
    {
    CUpnpRangeHeaderParser* self = 
        CUpnpRangeHeaderParser::NewLC( aTransaction, aStartPos, aEndPos );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::NewLC
// Factory method.
// ---------------------------------------------------------------------------
//
CUpnpRangeHeaderParser* CUpnpRangeHeaderParser::NewLC(
    CUpnpHttpServerTransaction &aTransaction, TInt& aStartPos, TInt& aEndPos )
    {
    CUpnpRangeHeaderParser* self = 
        new( ELeave ) CUpnpRangeHeaderParser( aTransaction, aStartPos, aEndPos );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::CUpnpRangeHeaderParser
// C++ constructor.
// ---------------------------------------------------------------------------
//
CUpnpRangeHeaderParser::CUpnpRangeHeaderParser(
    CUpnpHttpServerTransaction &aTransaction, TInt& aStartPos, TInt& aEndPos )
    :iTransaction(aTransaction), iStartPos( aStartPos ), iEndPos( aEndPos )
    {
    }

// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::ConstructL
// Two-phased constructor
// ---------------------------------------------------------------------------
//    
void CUpnpRangeHeaderParser::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::~CUpnpRangeHeaderParser
// Destructor.
// ---------------------------------------------------------------------------
//
CUpnpRangeHeaderParser::~CUpnpRangeHeaderParser()
    {
    }

// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::ParseRangeL
// Function returns parsinfg status.
// ---------------------------------------------------------------------------
//
TInt CUpnpRangeHeaderParser::ParseRangeL( CUpnpHttpMessage* aMsg, TInt aFileSize )
    {
    //checks if Range header exist and is correct
    TInt rangeStatus = CheckRangeHeaderSyntax( aMsg );
    if ( rangeStatus != EHttpPartialContent )
        {
        return rangeStatus;
        }
    
    //checks Range values    
    rangeStatus = CheckRangeValues( aFileSize );
    if ( rangeStatus < 0 )
    {
        return rangeStatus;
    }
    
    // creates response if Range header occurs
    if ( rangeStatus==EHttpPartialContent )
        {
        //HTTP/1.1 206 Partial Content
        CreatePartialContentResponseL( aFileSize, rangeStatus ); 
        }
    else if ( rangeStatus==EHttpNoContent )
        {
        //HTTP/1.1 204 No Content
        CreateNoContentResponseL( aFileSize, rangeStatus ); //HTTP/1.1 204 No Content
        }

    return rangeStatus;
    }

// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::CheckRangeHeaderSyntax
// Checks range header syntax.
// ---------------------------------------------------------------------------
//
TInt CUpnpRangeHeaderParser::CheckRangeHeaderSyntax( CUpnpHttpMessage* aMsg )
    {
    // checks if range header exist
    TBool rangeHeaderExist = EFalse;
    aMsg->IsHeader( (TDesC8&) UpnpHTTP::KHdrRange, rangeHeaderExist );
    if( !rangeHeaderExist )
        {
        return EHttpOk; //Range header doesn't exist
        }
    
    // checks if Range value exist    
    if( !(aMsg->GetHeaderValue( (TDesC8&) UpnpHTTP::KHdrRange ).Length() > 0 ) )
        {
        return -EHttpBadRequest; //Range value doesn't exist
        }
    
    // checks syntax of Range header
    if( !CUpnpRangeHeaderParser::ParseRangeHeader( aMsg->GetHeaderValue(
        (TDesC8&) UpnpHTTP::KHdrRange ), iStartPos, iEndPos ) )
        {
        return -EHttpBadRequest; //error in syntax
        }            
     
     return EHttpPartialContent; //Range header is proper
    }
    
// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::CheckRangeValue
// Checks range values.
// ---------------------------------------------------------------------------
//    
TInt CUpnpRangeHeaderParser::CheckRangeValues( TInt aFileSize )
    { 
    
    // empty file
    if ( aFileSize == 0 )
        {
        if ( ( iStartPos == 0 ) || ( iStartPos == iEndPos ) 
             || ( iStartPos > 0 && iEndPos == KErrNotFound ) 
             || ( iStartPos == KErrNotFound && iEndPos > 0 ) )
            {
            return EHttpNoContent;
            }
        else
            {
            return -EHttpBadRequest;
            }
        }
     else
        {
        // final bytes request
        // e.g. Range: bytes=-70 
        if ( ( iStartPos == KErrNotFound && iEndPos > 0 && iEndPos <= aFileSize ) )
            {
            iStartPos = aFileSize - iEndPos; 
            iEndPos = aFileSize-1;
            return EHttpPartialContent;
            }
        // bad posision dependece
        // e.g. Range: bytes=70-20 
        // e.g requested final bytes bigger than file size
        else if ( ( iStartPos == KErrNotFound && iEndPos == 0 ) 
                  || ( iStartPos == KErrNotFound && iEndPos > 0  && iEndPos > aFileSize )
                  || ( iStartPos > iEndPos && iEndPos != KErrNotFound ) )   
            {
            return -EHttpBadRequest;
            }
        // start position larger than file size     
        else if ( ( iStartPos >= aFileSize 
                  && ( iStartPos <= iEndPos || iEndPos == KErrNotFound ) ) )
            {
            return -EHttpRequestedRangeNotSatisfiable;
            }
        }
        
        // updates end position if it is out of file size
        if ( iEndPos == KErrNotFound || iEndPos > aFileSize-1 )
            {
            iEndPos = aFileSize-1;
            }
            
        return EHttpPartialContent; 
    }
     
// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::AppendContentLengthToResponseL
// Appends Content-Length header to response.
// ---------------------------------------------------------------------------
//            
void CUpnpRangeHeaderParser::AppendContentLengthToResponseL( 
    TInt aFileSize, TInt rangeStatus )
    {
    TInt size( 0 );
    if ( rangeStatus == EHttpPartialContent )
        {
        size = iEndPos - iStartPos + 1;    
        }
    else
        {
        size = aFileSize;
        }
    iSizeBuffer.Num( size ); 
    iTransaction.AddResponseHeaderL( UpnpHTTP::KHdrContentLength(), iSizeBuffer );
    }
       
// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::AppendBytesToResponseL
// Appends Bytes header to response.
// ---------------------------------------------------------------------------
//         
void CUpnpRangeHeaderParser::AppendBytesToResponseL( TInt aFileSize )
    {
    HBufC8* buffer = HBufC8::NewLC( sizeof(UpnpHTTP::KBytes) +
                         KIntegerOccurance*KMaxIntegerLength + KSlashSpaceAndMinusSize );
    
    //bytes
    buffer->Des().Append( UpnpHTTP::KBytes() );
    buffer->Des().Append( UpnpString::KSpace );
    //x
    iSizeBuffer.Num( iStartPos );
    buffer->Des().Append( iSizeBuffer ); 
    buffer->Des().Append( UpnpString::KMinus );
    //y
    iSizeBuffer.Num( iEndPos ); 
    buffer->Des().Append( iSizeBuffer ); 
    buffer->Des().Append( UpnpString::KSlash );
    //total
    iSizeBuffer.Num( aFileSize );
    buffer->Des().Append( iSizeBuffer );

    iTransaction.AddResponseHeaderL( UpnpHTTP::KHdrContentRange(), *buffer );

    CleanupStack::PopAndDestroy( buffer );
    }

// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::CreatePartialContentResponseL
// Creates response 206 Partial Content.
// ---------------------------------------------------------------------------
//     
void CUpnpRangeHeaderParser::CreatePartialContentResponseL( 
    TInt aFileSize, TInt aHttpCode )
    {
    // append Partial Content To Response - "206 Partial Content"    
    iTransaction.AddResponseHeaderL( KNullDesC8(), UpnpHTTP::KHTTPPartialContent() );
    AppendContentLengthToResponseL( aFileSize, aHttpCode );
    
    AppendBytesToResponseL( aFileSize );
    }

// ---------------------------------------------------------------------------
// CUpnpRangeHeaderParser::CreateNoContentResponseL
// Creates response for 204 No Content.
// ---------------------------------------------------------------------------
//     
void CUpnpRangeHeaderParser::CreateNoContentResponseL( 
    TInt aFileSize, TInt aHttpCode )
    {
    // append No Content To Response - "204 No Content"
    iTransaction.AddResponseHeaderL( KNullDesC8(), UpnpHTTP::KHTTPNoContent() );    
    
    AppendContentLengthToResponseL( aFileSize, aHttpCode );

    // Append No Content Fields To Response;
    iTransaction.AddResponseHeaderL( UpnpHTTP::KHdrPragma(), UpnpHTTP::KNoCache() );
    iTransaction.AddResponseHeaderL( UpnpHTTP::KHdrCacheControl(), UpnpHTTP::KNoCache() );

    
    }

// -----------------------------------------------------------------------------
//  CUpnpHttpSession::ParseRangeHeader
//  Parse range values
// -----------------------------------------------------------------------------
//    
TBool CUpnpRangeHeaderParser::ParseRangeHeader( TDesC8& aRangeHeader, TInt& aStartPos,
                                                TInt& aEndPos )
    {
    // Check header's syntax: "bytes=x-y" where y i optional
    TInt pos( KErrNotFound );

    // 1 string has to contain "="
    pos = aRangeHeader.FindC( UpnpString::KEqual() );
    if( pos == KErrNotFound ) 
        return EFalse;

    // 2 "bytes" has to be at the beginnig
    pos = aRangeHeader.FindC( UpnpHTTP::KBytes() );
    if( pos == KErrNotFound || pos != 0 ) 
        return EFalse;

    // 3 Sets position to after bytes
    pos = UpnpHTTP::KBytes().Length();

    // 4 If there any space or tab after "bytes" - move pos after it
    CUpnpRangeHeaderParser::MovePosition( aRangeHeader, pos );

    // 5 "=" - has to be at this position (after "bytes" + spaces or tabs)
    if( aRangeHeader[ pos ] != UpnpString::KEqual()[0] )
        return EFalse;

    // 6 Sets position to after "="
    pos++;

    // 7 If there any space or tab after "=" - move pos after it
    CUpnpRangeHeaderParser::MovePosition( aRangeHeader, pos );
    
    // 8 extract x-y. -1 stands for '=' length
    TPtrC8 byteRange = aRangeHeader.Right( aRangeHeader.Length() - pos );

    // 9 There can't be any comas because multi-range is not allowed
    if( byteRange.Find( UpnpString::KComa() ) != KErrNotFound ) 
        return EFalse;

    // 10 "-" delimiter must occure and it cant't be first char, because notation as follows: "-y"  is not allowed
    pos = byteRange.Find( UpnpString::KMinus() );
    if( pos == KErrNotFound ) 
        return EFalse;
    
    // 11 Checks if it is a final bytes request
    // e.g. Range: bytes= -20
    if( pos == 0 )
        {
        // If there any space or tab after "-" - move pos after it
        CUpnpRangeHeaderParser::MovePosition( byteRange.Right( byteRange.Length()-1 ), pos );
        // if pos equal 0 should be 1 to avoid "-" in getting number from string operation
        pos = pos == 0 ? 1 : pos;
        TLex8 endMinus( byteRange.Right( byteRange.Length() - pos  ) );
        endMinus.SkipSpace();
        TInt error = endMinus.Val( aEndPos );
        if ( !CUpnpRangeHeaderParser::HandleConversionException( endMinus, aEndPos, error ) )
            {
            return EFalse;
            }
            
        // We have to check if something else than space or tab leaves after conversion - unless for example 11a will be correct but it is not
        if ( CUpnpRangeHeaderParser::HasImproperChars( endMinus ) )
            return EFalse;
         
        aStartPos = KErrNotFound;
        return ETrue;
        }

    // 12 All looks fine, so now parse it and get x and y
    TLex8 start( byteRange.Left( pos ) );
    start.SkipSpace();

    // 13 If conversion fails - return error
    TInt error = start.Val( aStartPos );
    if ( !CUpnpRangeHeaderParser::HandleConversionException( start, aStartPos, error ) )
        {
        return EFalse;
        }
    // 14 We have to check if something else than space or tab leaves after conversion - unless for example 11a will be correct but it is not
    if ( CUpnpRangeHeaderParser::HasImproperChars( start ) )
            return EFalse;

    // y is optional
    if( ( byteRange.Length() - pos - 1 ) > 0 )
        {
        TLex8 end( byteRange.Right( byteRange.Length() - pos - 1 ) );
        end.SkipSpace();
        
        error = end.Val( aEndPos );
        if ( !CUpnpRangeHeaderParser::HandleConversionException( end, aEndPos, error ) )
            {
            return EFalse;
            }
            
        // We have to check if something else than space or tab leaves after conversion - unless for example 11a will be correct but it is not
        if ( CUpnpRangeHeaderParser::HasImproperChars( end ) )
            return EFalse;
        
        } 
    else
        {
        aEndPos = KErrNotFound; 
        } 

    return ETrue;
    } 

// -----------------------------------------------------------------------------
//  CUpnpRangeHeaderParser::HasImproperChars
//  Checks number format
// -----------------------------------------------------------------------------
//  
TBool CUpnpRangeHeaderParser::HasImproperChars( const TLex8& aNumber )
    {
    TPtrC8 rem_ptr = aNumber.Remainder();
    for( TInt index = 0; index < rem_ptr.Length(); index++ )
        {
        if( rem_ptr[ index ] != UpnpString::KSpace()[0] &&
            rem_ptr[ index ] != UpnpString::KTab()[0] )
            return ETrue;
        }
    return EFalse;    
    }
    
// -----------------------------------------------------------------------------
//  CUpnpRangeHeaderParser::MovePosition
//  Moves parser position to first not white space
// -----------------------------------------------------------------------------
//  
void CUpnpRangeHeaderParser::MovePosition( const TDesC8& aString, TInt& aPos )
    {
    while ( aPos < aString.Length() && 
            ( aString[ aPos ] == UpnpString::KSpace()[0] || aString[ aPos ] ==
                                                            UpnpString::KTab()[0] ) 
          ) 
        {
        aPos++;
        }  
    }    

// -----------------------------------------------------------------------------
//  CUpnpRangeHeaderParser::HandleConversionException
//  Handles conversion exceptions
// -----------------------------------------------------------------------------
// 
TBool CUpnpRangeHeaderParser::HandleConversionException( TLex8& aNumber, TInt& aRangePos,
                                                         TInt aError )
    {
    if( aError == KErrOverflow )
    {
        aRangePos = KMaxTInt;
        while ( aNumber.Peek().IsDigit() )
            {
            aNumber.Inc();
            }        
    }        
    else if ( aError != KErrNone )
        {
        return EFalse;
        }
    
    return ETrue;
    }
    
// End Of File

