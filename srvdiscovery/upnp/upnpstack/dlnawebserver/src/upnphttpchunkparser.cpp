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
* Description:  Declares chunk parsing class.
*
*/


// INCLUDE FILES
#include "upnphttpchunkparser.h"
#include "upnpcons.h"


// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpChunkParser* CUpnpHttpChunkParser::NewL()
	{

	CUpnpHttpChunkParser* self = new (ELeave) CUpnpHttpChunkParser();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
	return self;
	}

// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::~CUpnpHttpChunkParser
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpHttpChunkParser::~CUpnpHttpChunkParser()
	{
	}

// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::CUpnpHttpChunkParser
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpChunkParser::CUpnpHttpChunkParser()
:iContext(EUnknown),
 iChunkSize(KErrNotFound),
 iError (KErrNone)
	{
	}

// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
void CUpnpHttpChunkParser::ConstructL()
	{
	}
   
// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::Reset
// Checks if decoding ended
// -----------------------------------------------------------------------------
//
void CUpnpHttpChunkParser::Reset()
    {
    iContext = EUnknown;
    iError = KErrNone;
    iChunkSize = KErrNotFound;	
    }

// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::SetErrorState
// -----------------------------------------------------------------------------
//
void CUpnpHttpChunkParser::SetErrorState()
    {
    iError = EHttpRequestTimeout;
    iContext = EError;
    }				    
	
// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::Parse
// Decoding the chunked-encoded buffer
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpChunkParser::Parse(TDes8& aBuffer, TInt& aPos)
    {		
    TBool interrupt = EFalse;
    while(!interrupt)
        {
        switch(iContext)
            {
            case EUnknown:
                iContext = EHeader;
                break;
            case EHeader:
                interrupt = ParseHeader(aBuffer,aPos);
                break;
            case ELastChunk:
                interrupt = ParseLastChunk(aBuffer,aPos);
                break;	
            case EBody:
                interrupt = ParseBody(aBuffer,aPos);
                break;
            case EEndChunkBody:
                iContext = EHeader;
                break;
            case EExtension:
                interrupt = ParseExtension(aBuffer,aPos);
                break;
            case ETrailer:
                interrupt = ParseTrailer(aBuffer,aPos);
                break;
            case EError:			
                return iError;
            case EFinish:
                return KErrNone;
            default:
                return KErrUnknown;
            }
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::ParseL
// Decoding the chunked-encoded buffer
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpChunkParser::ParseBody( TDes8& aBuffer, TInt& aPos )
	{	
    if ( IsEmpty( aBuffer, aPos ) )
        {	    
        return ETrue;
        }

    TPtrC8 pointer( aBuffer.Right( aBuffer.Length() - aPos ) );	

    if ( iBytesAppended + pointer.Length() >= iChunkSize )
        {	
        aPos += ( iChunkSize - iBytesAppended );		
        pointer.Set( aBuffer.Right( aBuffer.Length() - aPos ) );
        TInt lineFeed = pointer.FindF(UpnpString::KLineFeed());		
        //linefeed found
        if ( !lineFeed )
            {			
            aBuffer.Delete( aPos, UpnpString::KLineFeed().Length() );
            iContext = EEndChunkBody;	
            return ( aPos == aBuffer.Length() );
            }
        //linefeed not found, end of buffer	
        else if ( lineFeed == KErrNotFound && 
                ( ( aPos + UpnpString::KLineFeed().Length() ) > aBuffer.Length() )
                )
            {			
            iBytesAppended = iChunkSize; 				
            return ETrue;
            }
        //size doesn't comply with the header value
        iError = KErrGeneral;	
        iContext = EError;	
        return EFalse;	
        }
    else
        {
        iBytesAppended += pointer.Length();
        aPos = aBuffer.Length();
        return ETrue;
        }		
    }
// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::ParseL
// Decoding the chunked-encoded buffer
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpChunkParser::ParseHeader( TDes8& aBuffer, TInt& aPos )
	{
    if ( IsEmpty( aBuffer, aPos ) )
        {	    
        return ETrue;
        }
    iChunkSize = KErrNotFound;
    iBytesAppended = 0;
    TPtrC8 pointer(NULL,0);
    //if '\r\n' exists
    TInt lineFeed = aBuffer.Right( aBuffer.Length() - aPos ).FindF( UpnpString::KLineFeed );
    //if ';' exists
    TInt semiColon = aBuffer.Right( aBuffer.Length() - aPos ).FindF( UpnpString::KSemiColon );
    //semicolon ignored if occurs after the linefeed
    if ( semiColon !=KErrNotFound && lineFeed !=KErrNotFound && lineFeed<semiColon )
        {
        semiColon = KErrNotFound;
        }
    if ( semiColon !=KErrNotFound )
        {		
        pointer.Set(aBuffer.Right( aBuffer.Length() - aPos ).Mid( 0,semiColon ) );		
        }
    else if ( lineFeed !=KErrNotFound )
        {
        pointer.Set( aBuffer.Right( aBuffer.Length() - aPos ).Mid( 0,lineFeed ) );		
        }
    else
        {
        pointer.Set( aBuffer.Right( aBuffer.Length() - aPos ) );		
        }

    TLex8 lex( pointer );	
    //skip white spaces	
    lex.SkipSpace();
    TUint size;
    //neither semicolon nor linefeed found
    if ( lineFeed == KErrNotFound && semiColon == KErrNotFound )
        {
        //remember the num of cut spaces
        TInt len = lex.Offset();
        if ( !lex.Eos() )
            {
            //check the chunk header size for the limit
            TInt error = lex.Val( size, EHex );			
            if ( error!= KErrNone || size > KMaxTInt )	
                {
                //size too big
                iError = ( error ) ? error: EHttpInsufficientStorage;
                iContext = EError;
                return EFalse;
                }			
            }

        aBuffer.Delete( aPos,len );
        return ETrue;
        }
			
    //get size	
    TInt error = lex.Val( size, EHex );
    if ( error!= KErrNone || size > KMaxTInt )
        {
        //unexpected characters or size too big
        iError = ( error ) ? error: EHttpInsufficientStorage ;
        iContext = EError;
        return EFalse;
        }	
    iChunkSize = size;	
    //skip white spaces	
    lex.SkipSpace();	
    if ( !lex.Eos() )
        {
        //unexpected characters
        iError = KErrGeneral;
        iContext = EError;
        return EFalse;
        }
    if ( lineFeed != KErrNotFound )
        {
        //the whole chunk header is parsed
        aBuffer.Delete( aPos, lineFeed + UpnpString::KLineFeed().Length() );
        //chunk size == 0
        if ( !iChunkSize )
            {        
            iContext = ELastChunk;
            }
        else
            {        
            iContext = EBody;
            }
        return ( aPos == aBuffer.Length() );
        }
    else if ( semiColon != KErrNotFound )
        {
        //extension found, 
        //one character left - possible linefeed
        if ( aPos<aBuffer.Length() )
            {            
            aBuffer.Delete( aPos, aBuffer.Length() - aPos - 1 );		
            }
        iContext = EExtension;
        return ETrue;	
        }
        
    iError = KErrUnknown;	
    iContext = EError;	
    return EFalse;		    	
    }
// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::ParseL
// Decoding the chunked-encoded buffer
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpChunkParser::ParseExtension( TDes8& aBuffer, TInt& aPos )
    {	
    if ( IsEmpty( aBuffer, aPos ) )
        {	    
        return ETrue;	
        }

    //if '\r\n' exists
    TInt lineFeed = aBuffer.Right( aBuffer.Length() - aPos ).FindF( UpnpString::KLineFeed );
    if ( lineFeed != KErrNotFound )
        {
        aBuffer.Delete( aPos, lineFeed + UpnpString::KLineFeed().Length() );		
        if ( !iChunkSize )
            {
            iContext = ETrailer;
            return EFalse;		
            }
        else
            {
            iContext = EBody;
            return EFalse;
            }		
        }
    else
        {
        //one character left - possible linefeed
        if ( aPos + 1 < aBuffer.Length() )
            {            
            aBuffer.Delete( aPos, aBuffer.Length() - aPos - 1 );
            }
        return ETrue;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::ParseL
// Decoding the chunked-encoded buffer
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpChunkParser::ParseLastChunk( TDes8& aBuffer, TInt& aPos )
    {		
    //if empty or only one character present
    if ( (aBuffer.Length() - aPos) < UpnpString::KLineFeedLength )
        {	    
        return ETrue;
        }
    //if '\r\n' exists
    TInt lineFeed = aBuffer.Right( aBuffer.Length() - aPos ).FindF( UpnpString::KLineFeed );
    if ( !lineFeed )
        {
        aBuffer.Delete( aPos, lineFeed + UpnpString::KLineFeed().Length() );		
        iContext = EFinish;
        return ETrue;		
        }
    else
        {
        iContext = ETrailer;
        return EFalse;		
        }		

    }	
// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::ParseL
// Decoding the chunked-encoded buffer
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpChunkParser::ParseTrailer( TDes8& aBuffer, TInt& aPos )
	{		
    if ( IsEmpty( aBuffer, aPos ) )
        {	    
        return ETrue;
        }
    //if double '\r\n' exists
    TInt doublelineFeed = aBuffer.Right( aBuffer.Length() - aPos ).FindF( UpnpString::KDoubleLineFeed );
    if (doublelineFeed != KErrNotFound ) 
        {
        aBuffer.Delete( aPos, doublelineFeed + UpnpString::KDoubleLineFeed().Length() );		
        iContext = EFinish;
        return ETrue;	
        }
    else
        {
        //waiting,3 characters reserved for double linefeed
        if ( aPos + UpnpString::KCRLFCRLength < aBuffer.Length() )
            {            
            aBuffer.Delete( aPos, aBuffer.Length() - aPos - UpnpString::KCRLFCRLength );
            }
        return ETrue;	
        }
    }
		
// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::IsEmpty
// Decoding the chunked-encoded buffer
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpChunkParser::IsEmpty( TDes8& aBuffer, TInt aPos )
    {
    return ( aPos >= aBuffer.Length() );
    }   

// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::IsFinished
// Checks if decoding ended
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpChunkParser::IsFinished()
    {
    return ( iContext==EFinish );
    }   


// End Of File
