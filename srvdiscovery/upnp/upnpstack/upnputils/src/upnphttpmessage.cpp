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
* Description:  HTTP header
*
*/


// INCLUDES
#include <e32math.h>
#include <f32file.h>
#include <e32std.h>

#include <e32property.h>

#include "upnplist.h"

#include "upnphttpmessage.h"
#include "upnpcommonupnplits.h"
#include "upnpcons.h"
#include "upnpstring.h"
#include "upnpcustomlog.h"
#include "upnpmessageobserver.h"

const TInt KHeadersMaxSixe = 8192;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* CUpnpHttpMessage::NewL()
    {
    CUpnpHttpMessage* self = new (ELeave) CUpnpHttpMessage();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }

// -----------------------------------------------------------------------------
// CCHttpMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* CUpnpHttpMessage::NewL(const TInetAddr& aAddr)
    {
    return NewL(aAddr, NewSessionIdL());
    }

// -----------------------------------------------------------------------------
// CCHttpMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* CUpnpHttpMessage::NewL(TInetAddr aAddr, TInt aSessionId)
    {
    CUpnpHttpMessage* self = new (ELeave) CUpnpHttpMessage();
    CleanupStack::PushL(self);
    
    
    self->ConstructL(aAddr, aSessionId);
    CleanupStack::Pop(self);
    
    return self;
    }

// -----------------------------------------------------------------------------
// CCHttpMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* CUpnpHttpMessage::NewL(TDesC8& aMessageBuffer, TInetAddr aSender)
    {
    
    return NewL(aMessageBuffer, aSender, NewSessionIdL());
    }

// -----------------------------------------------------------------------------
// CCHttpMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* CUpnpHttpMessage::NewL(TDesC8& aMessageBuffer, const TInetAddr& aSender, TInt aSessionId)
    {
    CUpnpHttpMessage* self = NewL(aSender, aSessionId);
    CleanupStack::PushL(self);
    self->ParseL(aMessageBuffer);
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::~CUpnpHttpMessage
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage::~CUpnpHttpMessage()
    {
    if (iHeaderList)
        {
        iHeaderList->DeleteObjects();
        delete iHeaderList;
        }

    if( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        }

    if ( iOutUri )
        {
        delete iOutUri;
        }

    delete iDestinationPath;


    delete iSenderPath;


    delete iTextBody;


    delete iOutFilename;


    delete iInFilename;

    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::CUpnpHttpMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage::CUpnpHttpMessage()
    {
    iSessionPriority = EPriorityNormal;
    
    iOverwriteExisting = EFalse;
    
    // Parameters connected with downloading range of requested content.
    // This mechanism if disabled by default an the whole file is requested.
    iOffset = 0;
    iLength = 0; 
    iSaveAtOffset = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::ConstructL()
    {
    iHeaderList = new (ELeave) CUpnpHttpHeaderList;

    
    iTextBody = HBufC8::NewL(0);

    iDestinationPath = HBufC8::NewL(0);
    iSenderPath = HBufC8::NewL(0);

    iInFilename = HBufC8::NewL(0);
    iOutFilename = HBufC8::NewL(0);
    //by default EFalse, meaning ignored
    iClientRequest = EFalse;
    //TCP-session's 0 means the value will be ignored unless changed later on!
    iTcpTimeout = 0;
    iInternalError = KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::ConstructL( TInetAddr& aSender, TInt aSessionId)
    {
    iDestinationPath = HBufC8::NewL(0);

    iSenderPath = HBufC8::NewL(0);

    iInFilename = HBufC8::NewL(0);
    
    iOutFilename = HBufC8::NewL(0);
    
    iHeaderList = new (ELeave) CUpnpHttpHeaderList;

    iTextBody = HBufC8::NewL(0);
    
    iRemoteHost = aSender;
    iSessionId = aSessionId;
    //by default EFalse, meaning ignored
    iClientRequest = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::ParseL
// Parses one row of headers.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::ParseL(TDesC8& aBuffer)
    {

    // position where the header ends
    TInt endOfHeader=0;
    
    // position of first \r\n
    TInt posOfNextLine = 0;
    
    // position of first space
    TInt posOfFirstSpace = 0;

    // position of second space
    TInt posOfSecondSpace = 0;

    // Boolean value that is used to check if message is
    // request or response. If message is response, protocol
    // version information is at the beginning of first row
    // (HTTP/1.1 200 OK)
    // In request, protocol version is at the end.
    // (GET / HTTP/1.1)
    TBool isResponse = EFalse;

// Handling of header part
    
    // trying to find Head/body separator
    endOfHeader = aBuffer.Find(UpnpString::KDoubleLineFeed);
    if( endOfHeader == KErrNotFound)
        {
        User::Leave(KErrAbort);
        }

    posOfNextLine = aBuffer.Find(UpnpString::KLineFeed); 
    if( posOfNextLine != KErrNotFound )
        {
        TPtrC8 spacePtr;
        spacePtr.Set( aBuffer );
        
        // finding first space
        posOfFirstSpace = spacePtr.Find(UpnpString::KSpace);
        if ( posOfFirstSpace != KErrNotFound )
            {
            spacePtr.Set( spacePtr.Left( posOfFirstSpace ) );

            // if first word of message is HTTP/1.1 or HTTP/1.0
            // message is response
            if( ( spacePtr.Find( KHttp11WithoutSpace() ) == 0 && 
                  posOfFirstSpace == KHttp11WithoutSpace().Length() ) 
                ||
                ( spacePtr.Find( KHttp10() ) == 0 && 
                  posOfFirstSpace == KHttp10().Length() ) )
                {
                isResponse = ETrue;
                }
            else
                {
                isResponse = EFalse;            
                }
            }
        else
            {
            isResponse = EFalse;            
            }

        // back to basic handling
        spacePtr.Set( aBuffer );
        
        if( posOfFirstSpace < posOfNextLine && posOfFirstSpace != KErrNotFound )
            {
            // if first space is found, it has to be before linefeed
            // +1 comes from going over the first space, we are interested
            // in what comes after it.
            spacePtr.Set( spacePtr.Mid( posOfFirstSpace  + 1 ) );
            posOfSecondSpace = spacePtr.Find(UpnpString::KSpace);
            if( posOfSecondSpace < posOfNextLine && posOfSecondSpace != KErrNotFound )
                {
                // check that length of path is longer than zero.
                if( posOfSecondSpace > 0 )
                    {
                    // Now check that Protocol version is HTTP/1.0 
                    // or HTTP/1.1. 
                    
                    // If message is response, version is defined in first 
                    // parameter of the message. This has been checked before,
                    // so if response, no check needed.
                    spacePtr.Set( spacePtr.Mid( posOfSecondSpace+1 ) );
                    posOfNextLine = spacePtr.FindC( UpnpString::KLineFeed() );
                    spacePtr.Set( spacePtr.Left( posOfNextLine ) );
                    
                    if( !isResponse )
                        {
                        //Check protocol's version                        
                        if (spacePtr.Match( KHttpVersionPattern() ) == 0 )
                            {    
                            //get only version numbers    
                            TPtrC8 version = spacePtr.Mid(5);
                            TLex8 lex(version);
                            TUint major;                            
                            if(lex.Val(major)!= KErrNone || lex.Peek()!='.')
                                {
                                {
                                // (protocol NOT HTTP/1.0 or HTTP/1.1. Invalid message)
                                User::Leave(KErrAbort);        
                                }
                                }
                            lex.Inc();                                                        
                            TUint minor;
                            if (lex.Val(minor) !=KErrNone)
                                {
                                // (protocol NOT HTTP/1.x. Invalid message)
                                User::Leave(KErrAbort);    
                                }                                
                            lex.SkipSpace();
                            if (!lex.Eos())
                                {
                                // (protocol NOT HTTP/1.x. Invalid message)
                                User::Leave(KErrAbort);    
                                }
                            //only 1.1 and 1.0 supported
                            if(major != 1 || (minor>1))
                                {
                                // (HTTP Version not supported)
                                User::Leave(-EHttpVersionNotSupported);    
                                }
                            
                            }
                        else
                            {
                            // (protocol NOT HTTP/1.0 or HTTP/1.1. Invalid message)
                            User::Leave(KErrAbort);                                
                            }
                        }
        
                    }
                else
                    {
                    // header too short. Invalid message
                    User::Leave(KErrAbort);
                    }
                }
            else
                {
                // no second space found. Invalid message.
                User::Leave(KErrAbort);
                }
            }
        else
            {
            // no space found before linefeed. Invalid message
            User::Leave(KErrAbort);
            }
        }
    else
        {
        // no linefeed found. Invalid message
        User::Leave(KErrAbort);
        }

    if ( endOfHeader != KErrNotFound )
        {
        TPtrC8 tempHeaderPointer;
        TPtrC8 finderIndexPointer;
        tempHeaderPointer.Set(aBuffer.Left(endOfHeader));
        
        
        // if header lenght is longer that 8k,cut headers after one that cross the line
        if (endOfHeader > KHeadersMaxSixe)
            {
            TInt afterLimit = tempHeaderPointer.Mid(KHeadersMaxSixe).Find(UpnpString::KLineFeed);
            if (afterLimit != KErrNotFound)
                {
                tempHeaderPointer.Set(tempHeaderPointer.Left(KHeadersMaxSixe + afterLimit));
                }
            }    
        TInt index=0, indexLWS=0, indexCRLF=0;
        TBool FirstLine=ETrue;
        
        while (index != KErrNotFound)
            {
            // locate end-of-line, parse each line separately
        
            //RFC822 3.2 field-name  =  1*<any CHAR, excluding CTLs, SPACE, and ":">
            index = tempHeaderPointer.Find(UpnpString::KLineFeed);
            indexLWS = FindLWS(tempHeaderPointer);
            finderIndexPointer.Set(tempHeaderPointer);
            indexCRLF = index;
            //the length of the string for the end-line marker CRLF
            TInt headerCRLF = 2;
            while (indexCRLF == indexLWS && indexCRLF != KErrNotFound)
            {
                //3 below =the length of the LWS string
                finderIndexPointer.Set(finderIndexPointer.Right(finderIndexPointer.Length()-(indexCRLF+3)));
                indexCRLF = finderIndexPointer.Find(UpnpString::KLineFeed);
                if (indexCRLF == KErrNotFound)
                {
                    //3 below =the length of the LWS string
                    index += finderIndexPointer.Length() + 3; 
                    indexLWS = finderIndexPointer.Length();
                    headerCRLF = 0;
                }
                else
                {
                    index += indexCRLF+3;    
                    indexLWS = FindLWS(finderIndexPointer);
                }
                
                
            }
            if(index != KErrNotFound)
                {
                TPtrC8 oneHeaderRow;
                oneHeaderRow.Set(tempHeaderPointer.Left(index));

                // To check if is the first line of message (e.g. GET / HTTP 1.1 etc.)
                if (FirstLine)
                    {
                    ParseHeaderRowL( (TDesC8&) oneHeaderRow, ETrue);
                    FirstLine=EFalse;
                    }
                else 
                    {
                    ParseHeaderRowL(oneHeaderRow, EFalse);
                    }

                // To delete one header row + "\r\n" = length+2 to prepare for next row.
                if (headerCRLF)
                    {
                    tempHeaderPointer.Set(tempHeaderPointer.Right(tempHeaderPointer.Length()-(index+headerCRLF)));    
                    
                    }
                    else
                    {
                        index = KErrNotFound;
                    }
                }
            }
            
        if (FirstLine)
            {
            ParseHeaderRowL(tempHeaderPointer, ETrue);
            }
        else
            {
            ParseHeaderRowL(tempHeaderPointer, EFalse);
            }
        }
    // check if all HTTP/1 requests have HOST header
    if(!isResponse && RequestHTTPVersion() == KHttp11WithoutSpace() )
        {
        TBool exists;
        IsHeader(UpnpSSDP::KHdrHost(), exists);
        if (!exists) 
            User::Leave(KErrAbort);    
        }
    
    // Handle body:
    
    if(iTextBody)
        {
        delete iTextBody;
        iTextBody=NULL;
        }
    
    iTextBody=HBufC8::NewL(aBuffer.Length() - endOfHeader);
    iTextBody->Des().Zero();
    
    // Append rest of message to iTextBody. Leave out string "\r\n\r\n".
    iTextBody->Des().Append(aBuffer.Right(aBuffer.Length() - (endOfHeader + 4)));
    }
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::ParseHeaderRowL
// Parses one row of headers.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::FindLWS(const TDesC8& aText)
{
    TInt indexSP = aText.Find(UpnpString::KCRLFSP);
    TInt indexHT = aText.Find(UpnpString::KCRLFHT);
    
    
    if (indexSP == KErrNotFound) 
        return indexHT;
    else if    (indexHT == KErrNotFound) 
        return indexSP;
    else if(indexSP < indexHT) 
        return indexSP;
    else
        return indexHT;
    
}

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::ParseHeaderRowL
// Parses one row of headers.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::ParseHeaderRowL(const TDesC8& aBuffer, TBool aFirstLine)
    {
    TInt FirstColonPosition=0;

    // locate first colon. This separates header's name and value.

    FirstColonPosition = aBuffer.Find(UpnpString::KColon());
    if(aFirstLine)
        {
        AddPairL(aBuffer, KSpace8());
        }
    else
        {
        if (FirstColonPosition == KErrNotFound )
            {
                User::Leave(KErrAbort);
            }
        else 
            {    
            HBufC8* TrimBuffer;
            TrimBuffer=HBufC8::NewLC(aBuffer.Length() - (FirstColonPosition + 1));
            TrimBuffer->Des().Zero();
            
            TrimBuffer->Des().Append(aBuffer.Right(aBuffer.Length() - (FirstColonPosition + 1)));
            TrimBuffer->Des().Trim();
            UnFoldHeader(TrimBuffer);

            TPtrC8 ptr = aBuffer.Left(FirstColonPosition);
            
            // if there are spaces, ignore them.
            TBool spaceFound = ETrue;

            while( spaceFound )
                {
                if(ptr.Find(KSpace()) == 0)
                    {
                    ptr.Set(ptr.Mid(1));
                    }
                else
                    {
                    spaceFound = EFalse;
                    }
                }

            AddPairL(ptr, *TrimBuffer);
            
            CleanupStack::PopAndDestroy();    
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::TimerEventL
// Timer expired. For SSDP messages
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::TimerEventL( CUpnpNotifyTimer* /*aTimer*/ )
    {
    // Cancel timer if exists.
    if( iTimer )
        {
        if( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        }
    iMessageObserver->MessageExpiredL( this );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetMessageTimeoutL
// Set message timer timeout for SSDP messages.
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt CUpnpHttpMessage::SetMessageTimeoutL( MUpnpMessageObserver* aObserver, const TInt aTimeoutValue )
    {
    // check that request is valid.
    if( !aObserver )
        {
        return KErrNotFound;
        }
        
    iMessageObserver = aObserver;

    if( iTimer )
        {
        if( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        }
    else
        {
        // setup timer;
        iTimer = CUpnpNotifyTimer::NewL( this );
        }
    iTimer->After( aTimeoutValue );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::CancelMessageTimeout
// Set message timer timeout.
// -----------------------------------------------------------------------------
//   
EXPORT_C void CUpnpHttpMessage::CancelMessageTimeout()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        // Deleting timer will also null the observer in the timer.
        delete iTimer;
        iTimer = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::AddPairL
// Adds a Headerrow in this message.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::AddPairL(const TDesC8& aName, const TDesC8& aValue)
    {
    // first checking if given header already exists in the message

    CUpnpHttpHeader* chk = iHeaderList->First();
    while( chk != NULL )
        {
        if( chk->Name().CompareC( aName ) == 0 )
            {
            chk->SetValueL( aValue );
            // no delete to chk, it is owned by iHeaderList.
            return;
            }
        chk = iHeaderList->Next( chk );
        }
    
    // header name not found, adding it.
        
    CUpnpHttpHeader* hdr = CUpnpHttpHeader::NewL(aName, aValue);
    CleanupStack::PushL(hdr);
    iHeaderList->AddL(hdr);
    CleanupStack::Pop(hdr);
    }
    
    
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::AddPairL
// Adds a Headerrow in this message.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::RemovePairL(const TDesC8& aName)
    {
    // first checking if given header already exists in the message

    CUpnpHttpHeader* chk = iHeaderList->First();
    while( chk != NULL )
        {
        if( chk->Name().CompareC( aName ) == 0 )
            {
            // no delete to chk, it is owned by iHeaderList.
            iHeaderList->Remove(chk);
            delete chk;
            return;
            }
        chk = iHeaderList->Next( chk );
        }
    
    }

    

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetBodyL
// Sets the body of message.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetBodyL(const TDesC8& aBody)
    {
    delete iTextBody;
    iTextBody = NULL;
    iTextBody = aBody.AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Body
// Getter for the body of message.
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpHttpMessage::Body()
    {
    return *iTextBody;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::HeadersToStringL
// Returns message as HBufC8.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpHttpMessage::HeadersToStringL()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();

    HBufC8* headerBuf = NULL;
    headerBuf = HBufC8::NewL(0);
    headerBuf->Des().SetLength(0);
    TBool firstLine = ETrue;
    
    while (hdr != NULL)
        {
        CleanupStack::PushL( headerBuf );
        
        HBufC8* tempHeader = NULL;
    
        tempHeader=HBufC8::NewLC( hdr->Name().Length() +
                                    hdr->Value().Length() +
                                    4 ); // 4 for colon, space and linefeed
        tempHeader->Des().Zero();        
        tempHeader->Des().Append(hdr->Name());


        //first line is Request line (Request-Line   = Method SP Request-URI SP HTTP-Version CRLF)
        //block is needed for URI manipualtion, first line is ALSO added
        //-- can not be folded, DLNA says directly "Each HTTP header line"


        TInt posOfSpace = tempHeader->Des().Find(UpnpString::KSpace());
            TPtrC8 ptr;
            if (posOfSpace != KErrNotFound)
                {
                ptr.Set( 
                tempHeader->Des().Right( tempHeader->Des().Length() - 
                                            (posOfSpace + 1)  )
                    );
                }
            
            TInt posOfSecondSpace = ptr.Find( UpnpString::KSpace() );
            TPtrC8 ptr2;
            
            if (posOfSecondSpace != KErrNotFound)            
                ptr2.Set( ptr.Left( posOfSecondSpace ) );

            HBufC8* url = ptr2.AllocLC();

            
        tempHeader->Des().Replace( posOfSpace+1,
                                          url->Des().Length(),
                                          *url );

            CleanupStack::PopAndDestroy( url );
            url = NULL;
            
/*------if Content-Length is not at the beginning (can be some same string in value) so copy it, otherwise will be added and calcualted at the end----*/

        if(hdr->Name().FindC(UpnpHTTP::KHdrContentLength()) != 0)

            {
            if (hdr->Value().Length() > 0 && !firstLine)
                {
                tempHeader->Des().Append(UpnpString::KColon());
                tempHeader->Des().Append(UpnpString::KSpace());
                tempHeader->Des().Append(hdr->Value());
                }
            else 
                {
                if (firstLine)
                    {
                    firstLine = EFalse;
                    }
                else
                    {
                    tempHeader->Des().Append(UpnpString::KColon());
                    }
                }
            //folding, we pass with lineFeed
            FoldHeaderL(tempHeader);                
            tempHeader->Des().Append(UpnpString::KLineFeed());
            }

        HBufC8* tempBuf;
        tempBuf = HBufC8::NewLC( headerBuf->Des().Length() +
                                 tempHeader->Des().Length() );
        tempBuf->Des().Zero();
        tempBuf->Des().Append( *headerBuf );
        

        if(hdr->Name().FindC(UpnpHTTP::KHdrContentLength()) != 0)
            {
            tempBuf->Des().Append(*tempHeader);
            }
        
        CleanupStack::Pop( tempBuf );
        CleanupStack::PopAndDestroy( tempHeader ); 
        CleanupStack::PopAndDestroy( headerBuf ); 
        
        tempHeader = NULL;
        headerBuf = NULL;

        headerBuf = tempBuf;
        tempBuf = NULL;
        
        hdr = iHeaderList->Next(hdr);    
        }
    
    AppendContentLengthToHeadersL(headerBuf);
    return headerBuf;
}
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::AppendContentLengthToHeadersL
// Appends ContentLength when headers are dumped to string (using public method HeadersToStringL).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::AppendContentLengthToHeadersL(HBufC8*& aHeaders){

/*------open file which keep message--------*/
    TInt bodyLength( 0 );
    CleanupStack::PushL( aHeaders );    


    if(iOutFilename->Length() > 0)
        {
        RFs fs;
        CleanupClosePushL(fs);
        RFile file;
        CleanupClosePushL(file);

        User::LeaveIfError( fs.Connect() );
        
        HBufC16* outFilename16 = UpnpString::ToUnicodeL( *iOutFilename);
        CleanupStack::PushL(outFilename16);
        
        User::LeaveIfError( file.Open(fs,*outFilename16,  EFileRead|EFileShareAny ) );

        TInt Filesize( 0 );
        User::LeaveIfError( file.Size( Filesize ) );
        
        CleanupStack::PopAndDestroy( outFilename16 );
        
        CleanupStack::PopAndDestroy();  //file.Close();
        outFilename16 = NULL;
        
        CleanupStack::PopAndDestroy(); //fs.Close();
                
        bodyLength = Filesize;
        }
    else
        {
        iTextBody->Des().Trim();
        bodyLength=iTextBody->Des().Length();
        }

    TBuf8<20> tempBuf;
    tempBuf.Num( bodyLength );

    HBufC8* tempBuf2;
    tempBuf2=HBufC8::NewL( aHeaders->Des().Length() +
                            tempBuf.Length() +
                            UpnpHTTP::KHdrContentLength().Length() +
                            2 ); // +2 from colon and space
    tempBuf2->Des().Append( *aHeaders);
    
    CleanupStack::Pop(aHeaders);
    delete aHeaders;
    aHeaders=NULL;

    tempBuf2->Des().Append(UpnpHTTP::KHdrContentLength());
    tempBuf2->Des().Append(UpnpString::KColon());
    tempBuf2->Des().Append(UpnpString::KSpace());
    tempBuf2->Des().Append(tempBuf);

    aHeaders=tempBuf2;
    tempBuf2=NULL;

    }
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::FoldHeaderL
// Folds long header fields (without last CRLF)  - "MUST limit 998".
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::FoldHeaderL(HBufC8*& aHeader){
HBufC8* result = NULL;
TInt length = aHeader->Length();

    if(length > KLineLengthLimit)
        {
        //Body can be only folded, so 1st we check if name is not too long
        if (aHeader->FindC(UpnpString::KColon()) > KLineLengthLimit-1)
            {
            User::Leave(KErrOverflow);
            #ifdef _DEBUG
            LOGS("[CUpnpHttpMessage::FoldHeaderL] name of header > KLineLengthLimit");
            #endif //_DEBUG
            }
        else
            {
            //RFC 2822 3.2.3 (check if any FWS already exist and remove them) comment in DLNA doesnt exist so there are not processing)
            UnFoldHeader(aHeader);
            aHeader->Des().TrimAll();

            //now proper folding
            //new length is estimated,
            //its faster way to estiamte max number (worst possible case) than calculate strict number
            //how estimation is calculate: HighLevel break (but not SP) in worst case will be in every half -1 in every maxLength string
            
            
            result = HBufC8::NewL(length + length/KLineLengthLimit*2*3 + 2);//CRLFSP + LineFeed
            CleanupStack::PushL(result);
            TInt x1 = 0, x2 = 0;
            TBool last = EFalse;
            while(ETrue)
                {
                length = aHeader->Length();
                x2 = x1 + KLineLengthLimit;
                if (x2 >= length) 
                    {
                    x2 = length-1;
                    last = ETrue;
                    }
                else
                    {
                    TBool foundBreak = EFalse;
                    for (TInt j=0; j<KHigherLevelSintaticBreaks().Length(); j++)
                        {
                        x2 = x1 + KLineLengthLimit;
                        while (x2 > 0 && !foundBreak)
                            {
                            if (aHeader->Des()[x2] == KHigherLevelSintaticBreaks()[j])
                                {
                                foundBreak = ETrue;
                                }
                            else
                                {
                                x2--;
                                }
                                
                            }
                        if (foundBreak) break;
                        }
                    if (!foundBreak)
                        {
                        User::Leave(KErrOverflow);
                        #ifdef _DEBUG
                        LOGS("[CUpnpHttpMessage::FoldHeaderL] value of header > KLineLengthLimit");
                        #endif //_DEBUG
                        }
                    }
                //we will append in different way whether SP or other breaks
                if (aHeader->Des()[x2] == UpnpString::KSpace()[0] || 
                    aHeader->Des()[x2] == UpnpString::KTab()[0])
                    {
                    result->Des().Append(aHeader->Des().Mid(x1, x2));
                    if (!last)
                        {
                        result->Des().Append(UpnpString::KLineFeed());
                        }
                    }
                else
                    {
                    result->Des().Append(aHeader->Des().Mid(x1, x2+1));
                    if (!last)
                        {
                        result->Des().Append(UpnpString::KLineFeed());
                        result->Des().Append(UpnpString::KSpace());//additional space, HTTP req, also break reversibility (folding and unfolding will give new string)
                        }
                    }
                aHeader->Des().Delete(x1, x2-x1);// from, length
                if (last)
                    {
                    break;
                    }
                
                }
            CleanupStack::Pop(result);
            CleanupStack::PopAndDestroy(aHeader);
            aHeader = result;
            CleanupStack::PushL(aHeader);
            }
        }
    
 
}
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::ToStringL
// Unfolds header by simply removing any CRLF that is immediately followed by WSP.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::UnFoldHeader(HBufC8*& aHeader){
    TPtr8 ptr = aHeader->Des();
    TInt pos = ptr.FindC(UpnpString::KLineFeed());
    while(pos != KErrNotFound)
    {
        ptr.Delete(pos, 2);
        pos = ptr.FindC(UpnpString::KLineFeed());
    }
}

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::ToStringL
// Returns message as HBufC8.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpHttpMessage::ToStringL()
    {
    HBufC8* buffer;
    buffer=HeadersToStringL();
    CleanupStack::PushL(buffer);

    HBufC8* returnBuf;
    //4 added to the length of the buffer becasue of the additional 2 line feeds
    returnBuf=HBufC8::NewL(buffer->Des().Length()+iTextBody->Des().Length()+4);
    
    returnBuf->Des().Append(*buffer);
    CleanupStack::PopAndDestroy(buffer);
    buffer=NULL;
    //2 line feeds appended
    returnBuf->Des().Append(UpnpString::KLineFeed);
    returnBuf->Des().Append(UpnpString::KLineFeed);
    
    returnBuf->Des().Append(*iTextBody);
    
    return returnBuf;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetMessageDateL
// Sets the date of message.
// -----------------------------------------------------------------------------
//

EXPORT_C void CUpnpHttpMessage::SetMessageDateL(const TTime& aTime)
{   
    HBufC8* timebuf = NULL;
    timebuf = UpnpString::GetDateLC(aTime);
    
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    TBool HeaderFound=EFalse;
    
    while (hdr != NULL)
        {
        if (hdr->Name() == UpnpHTTP::KHdrDate() )
            {
            HeaderFound = ETrue;
            hdr->SetValueL(*timebuf);
            }
        hdr = iHeaderList->Next(hdr);    
        }
    
    if(!HeaderFound)
        {
        AddPairL(UpnpHTTP::KHdrDate(),*timebuf);
        }
    CleanupStack::PopAndDestroy(timebuf);
}

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Sender
// Returns the IP address of the sender of the message.
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr& CUpnpHttpMessage::Sender()
    {
    return iRemoteHost;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Receiver
// Returns the IP address of the receiver of the message.
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr& CUpnpHttpMessage::Receiver()
    {
    return iRemoteHost;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetDestinationPathL
// Sets the DestinationPath of the message.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetDestinationPathL(const TDesC8& aDestinationPath)
    {
    if(iDestinationPath)
        {
        delete iDestinationPath;
        iDestinationPath = NULL;
        }
        
    iDestinationPath = aDestinationPath.AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::DestinationPath
// Returns the DestinationPath of the message.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CUpnpHttpMessage::DestinationPath()
    {
    if(iDestinationPath->Length()>0)
        {
        return iDestinationPath->Des();
        }
    else
        {
        return (TDesC8&) KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetSenderPathL
// Sets the SourcePath of the message.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetSenderPathL(const TDesC8& aSenderPath)
    {
    if(iSenderPath)
        {
        delete iSenderPath;
        iSenderPath = NULL;
        }
        
    iSenderPath = aSenderPath.AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SenderPath
// Returns the SenderPath of the message.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CUpnpHttpMessage::SenderPath()
    {
    if(iSenderPath->Length()>0)
        {
        return iSenderPath->Des();
        }
    else
        {
        return (TDesC8&) KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SenderPathFromHeader
// Returns the SenderPath from Headers.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpHttpMessage::SenderPathFromHeader()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();

    if (hdr != NULL)
        {
        TInt FirstSpacePosition=hdr->Name().Find(KSpace);
            
        if (FirstSpacePosition != KErrNotFound)
            {
            TPtrC8 tempPtr;
            tempPtr.Set(hdr->Name().Right(hdr->Name().Length() - (FirstSpacePosition + 1)));
            TInt SecondSpacePosition=tempPtr.Find(UpnpString::KSpace());
            if (SecondSpacePosition != KErrNotFound)
                {
                TPtrC8 tempPtr2;
                tempPtr2.Set(tempPtr.Left(SecondSpacePosition));
                return tempPtr2;
                }
            else
                return TPtrC8((unsigned char*)"", 0);
    
            }
        else
            return TPtrC8((unsigned char*)"", 0);
        
        }

    return TPtrC8((unsigned char*)"", 0);
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetPendingRequest
// Sets the private member iPendingRequest.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetPendingRequest(TAny* aPointer)
    {
    iPendingRequest=aPointer;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::PendingRequest
// Returns iPendingRequest pointer.
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* CUpnpHttpMessage::PendingRequest()
    {
    return iPendingRequest;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetInFilenameL
// Sets the filename where to save incoming content.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetInFilenameL(const TDesC8& aFilename, TBool aOverwriteExisting)
    {
    if ( iInFilename )
        {
        delete iInFilename;
        iInFilename = NULL;
        }
    
    iInFilename=HBufC8::NewL(aFilename.Length());
    iInFilename->Des().Zero();
    iInFilename->Des().Append(aFilename);    

    iOverwriteExisting  = aOverwriteExisting;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::InFilename
// Returns the name of file where to save incoming content.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CUpnpHttpMessage::InFilename()
    {
    if(iInFilename->Length()>0)
        {
        return iInFilename->Des();
        }
    else
        {
        return (TDesC8&) KNullDesC8();
        }
    }


// -----------------------------------------------------------------------------
// CUpnpHttpMessage::OverwriteExisting
// Returns if the existing file should be overwritten with the requested content.
// If not then the new file with name: aFilename_serialNumber will be created.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::OverwriteExisting()
    {
    return iOverwriteExisting;        
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetRangeL
// Sets the range of remote filename which will be requested.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetRangeL(TInt aOffset, TInt aLength, TBool aSaveAtOffset)
    {
    iOffset = aOffset;
    iLength = aLength;
    iSaveAtOffset = aSaveAtOffset;    

    
    TPtrC8 method = Method();
    // Check method, because sending POST with range is not supported.
    if( method.Find( KHttpGet ) == 0 || method.Find( KHttpHead ) == 0)
        {
        // If one of those values are different from default then it means that 
        // the Range header should be added.
        if( iOffset != 0 || iLength != 0 ) 
            {
            TBuf8<KMaxIntegerLength> num;
            HBufC8* rangeValue = HBufC8::NewLC(    UpnpHTTP::KBytes().Length() +
                                                UpnpString::KEqual().Length() +
                                                KMaxIntegerLength + 
                                                   UpnpString::KMinus().Length() +
                                                   KMaxIntegerLength );
            rangeValue->Des().Zero();
            
            rangeValue->Des().Append( UpnpHTTP::KBytes() );
            rangeValue->Des().Append( UpnpString::KEqual() );
            
            num.Num( iOffset );
            rangeValue->Des().Append( num );
            rangeValue->Des().Append( UpnpString::KMinus() );
            
            // If length == 0 and offset is specified then it should looks: offset-
            if( iLength )
                {
                // In the Range header the second value specyfies last byte position
                // so there must be done simple transformation last-byte-pos = offset+lenght-1.
                // There is 1 subtracted because we inlucde the byte indicated by offset
                TInt lastBytePos = iOffset + iLength - 1; 
                num.Num( lastBytePos );
                rangeValue->Des().Append( num );
                }
            
            AddPairL( UpnpHTTP::KHdrRange(), *rangeValue );
                                               
            CleanupStack::PopAndDestroy( rangeValue );
            }
        else if( iOffset == 0 && iLength == 0 && !iSaveAtOffset ) 
            {
            // Set values indicate that there should be no Range header, so remove it.
            CUpnpHttpHeader* hdr = iHeaderList->First();
    
            while ( hdr != NULL )
                {
                if( hdr->Name().Length() == UpnpHTTP::KHdrRange().Length() )
                    {
                    if (hdr->Name().FindC( UpnpHTTP::KHdrRange() ) == 0)
                        {
                        // Header was found so remove it and exit function.
                        iHeaderList->Remove( hdr );
                        delete hdr;
                        return;
                        }
                    }
                hdr = iHeaderList->Next( hdr );
                }
            }
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Offset
// Returns the offset of the file that should be requested from server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::Offset()
    {
    return iOffset;    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Length
// Returns the length of file content to be requested.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::Length()
    {
    return iLength;        
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::InFilename
// Return if the requested offset of the remote file should be saved at physical offset of
// the local file.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::SaveAtOffset()
    {
    return iSaveAtOffset;        
    }
    
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetOutFilenameL
// Sets the filename which will be used as message body.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetOutFilenameL(const TDesC8& aFilename)
    {

    if(iOutFilename)
        {
        delete iOutFilename;
        iOutFilename=NULL;
        }
    
    iOutFilename=HBufC8::NewL(aFilename.Length());
    iOutFilename->Des().Zero();
    iOutFilename->Des().Append(aFilename);    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::OutFilename
// Returns the name of file which will be set as body of message to send.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CUpnpHttpMessage::OutFilename()
    {
    if(iOutFilename->Length()>0)
        {
        return iOutFilename->Des();
        }
    else
        {
        return (TDesC8&) KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::GetHeaderValue
// Returns value of the header.
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpHttpMessage::GetHeaderValue(const TDesC8& aHeaderName)
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while (hdr != NULL)
        {
        if(hdr->Name().Length() == aHeaderName.Length())
            {
            if (hdr->Name().FindC(aHeaderName) == 0)
                {
                return hdr->Value();
                }
            }
        hdr = iHeaderList->Next(hdr);
        }
    
    return (TDesC8&) KNullDesC8();
    }

// -----------------------------------------------------------------------------
//    Check if header exists, also return value of header if exists
//    @param aHeaderName Name of the header.
//    @param aExist True if header exist, false if not.
//    @result Value of the header (if exist, otherwise KNullDesC).
// -----------------------------------------------------------------------------

EXPORT_C TDesC8& CUpnpHttpMessage::IsHeader(const TDesC8& aHeaderName, TBool& aExist)
{ 
    CUpnpHttpHeader* hdr = iHeaderList->First();
    aExist = EFalse;
    while (hdr != NULL)
        {
        if(hdr->Name().Length() == aHeaderName.Length())
            {
            if (hdr->Name().FindC(aHeaderName) == 0)
                {
                aExist = ETrue;
                return hdr->Value();
                }
            }
        hdr = iHeaderList->Next(hdr);
        }
    return ( TDesC8& )KNullDesC8();
}


// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetSessionId
// Sets messages Session id.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetSessionId(TInt aId)
    {
    iSessionId = aId;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SessionId
// Returns the SessionId of the message.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::SessionId() const
    {
    return iSessionId;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::NewSessionIdL
// Creates new, unused Session id value.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::NewSessionIdL()
    {
    TInt id;
    TInt r;
    TBool newID=EFalse;

    RSemaphore accessSemaphore;        
    
    r=accessSemaphore.OpenGlobal(KSessionIdAccessSemaphoreName());
    //if failed to open
    if(r!=KErrNone)
        {
        User::LeaveIfError (accessSemaphore.CreateGlobal(KSessionIdAccessSemaphoreName(), 0) );
        accessSemaphore.Signal();
        }
    
            
          
    accessSemaphore.Wait();
    
    // Use Publish and Subscribe to generate new session ID. 
    RProperty idCounter;
    
    // Attach to the idCounter, property defined in MessageHandler
    r = idCounter.Attach(KUPnPUtilsCat,EUPnPUtilsCounter);
    if (r == KErrNone) 
        {
        r = idCounter.Get(id);
        }

    if (r == KErrNone) 
        {
        // Increment id and save
        id++;
        r=idCounter.Set(id);
        if (r==KErrNone) 
            {
            newID = ETrue; 
            }
        }

    accessSemaphore.Signal();
    accessSemaphore.Close();       

    // Close the idCounter property
    idCounter.Close();      
    
    
    if(!newID)
        User::Leave(r);

    return id;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SessionIdMatch
// Compares if two messages have same session id.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::SessionIdMatch(const CUpnpHttpMessage& aFirst, const CUpnpHttpMessage& aSecond)
    {
    return aFirst.iSessionId == aSecond.iSessionId;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::BodyLength
// Returns length of message body.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::BodyLength() const
    {
    return iTextBody->Length();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::DevicePath
// Returns the service used from http message, for example MediaServer:1 
// from Mediaserver:1/ContentDirectory.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpHttpMessage::DevicePath()
    {

    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    if (hdr != NULL)
        {
        TInt FirstSlashIndex = hdr->Name().Find(UpnpString::KSlash());
        if (FirstSlashIndex == KErrNotFound)
            {
            return KNullDesC8();
            }
        TPtrC8 tempPtr;
        tempPtr.Set(hdr->Name().Right(hdr->Name().Length() - (FirstSlashIndex + 1)));
        TInt SecondSlashIndex = tempPtr.Find(UpnpString::KSlash());
        if (SecondSlashIndex == KErrNotFound)
            {
            return KNullDesC8();
            }
        TPtrC8 tempPtr2;
        tempPtr2.Set(tempPtr.Left(SecondSlashIndex));
        return tempPtr2;
        }
    
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::ServicePath
// Returns the service used from http message, for example ContentDirectory 
// from Mediaserver:1/ContentDirectory.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpHttpMessage::ServicePath() const
    {
    TInt firstSlashIndex(-1);
    
    TInt httpSchema  = iDestinationPath->Find( UpnpHTTP::KHTTPUrl() );
    if( httpSchema != KErrNotFound)
    {
        TPtrC8 schemaPtr( iDestinationPath->Mid( UpnpHTTP::KHTTPUrl().Length() ) );    
        TInt relativePath = schemaPtr.Find( UpnpString::KSlash() );        
        firstSlashIndex = relativePath + UpnpHTTP::KHTTPUrl().Length() ;
    }    
    else
    {
        firstSlashIndex = iDestinationPath->Find(UpnpString::KSlash());
    }
    
    if(firstSlashIndex == KErrNotFound)
        return KNullDesC8();    
    
    TPtrC8 devicePtr(iDestinationPath->Right(iDestinationPath->Length() - (firstSlashIndex + 1)));

    TInt secondSlashIndex = devicePtr.Find(UpnpString::KSlash());
    TPtrC8 servicePtr(devicePtr.Right(devicePtr.Length() - (secondSlashIndex + 1)));
    TInt third = servicePtr.Find(UpnpString::KSlash());
    TInt sp = servicePtr.Find(UpnpString::KSpace());

    if (third != KErrNotFound && ((third < sp && sp != KErrNotFound) || sp == KErrNotFound))
        {
        return servicePtr.Left(third);
        }
    else
        {
        TInt space = servicePtr.Find(UpnpString::KSpace());
        if (space != KErrNotFound)
            {
            return servicePtr.Left(space);
            }
        return servicePtr;
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::IsSoap
// Checks if message is SOAP message.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::IsSoap() const
    {
    
    TInt envelope = iTextBody->Find(KEnvelope());
    if( envelope == KErrNotFound )
        {
        CUpnpHttpHeader* hdr = iHeaderList->First();
    
        while (hdr != NULL)
            {
            if ( hdr->Value().FindC( UpnpSSDP::KUPnPServiceSchema() ) != KErrNotFound && hdr->Name().FindC( KSoapAction() ) != KErrNotFound )
                {
                return ETrue;
                }

            hdr = iHeaderList->Next(hdr);
            }
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::IsGena
// Checks if message is GENA message.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::IsGena()
    {

    if (Method().CompareC(UpnpGENA::KGenaSubscribe()) == 0)
        {
        return ETrue;
        }
    else if (Method().CompareC(UpnpGENA::KGenaUnSubscribe()) == 0)
        {
        return ETrue;
        }
    else if (Method().CompareC(UpnpGENA::KGenaNotify()) == 0)
        {
        return ETrue;
        }

    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while (hdr != NULL)
        {
        if (hdr->Name().CompareC(UpnpGENA::KSid()) == 0)
            {
            return ETrue;
            }

        hdr = iHeaderList->Next(hdr);
        }    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Method
// Returns the method of message, like GET, POST etc.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpHttpMessage::Method()
    {
    TBool ValueFound=EFalse;
    CUpnpHttpHeader* hdr = iHeaderList->First();

    TPtrC8 returnPtr;
    
    if (hdr != NULL)
        {
        TInt index = hdr->Name().Find(UpnpString::KSpace());
        if    (index!= KErrNotFound)
        {
        returnPtr.Set(hdr->Name().Left(index));
        ValueFound=ETrue;    
        }
        
        }
    
    if(!ValueFound)
        {
        returnPtr.Set(KNullDesC8);
        }
    return returnPtr;
    }
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::HTTPVersion
//Returns version of HTTP, like HTTP/1.1, HTTP/1.0 etc.
//-----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpHttpMessage::RequestHTTPVersion()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    TPtrC8 returnPtr;

    if (hdr != NULL)
        {
        TInt index = hdr->Name().LocateReverse(UpnpString::KSpace()[0]);
        if (index >= 0)
            {
                returnPtr.Set(hdr->Name().Right(hdr->Name().Length()-index-1));    
            }
            else
            {
                returnPtr.Set(KNullDesC8);
            }
        }
        else
        {
            returnPtr.Set(KNullDesC8);
        }
    return returnPtr;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::HeaderList
// Returns a pointer to this message's headerlist.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpHeaderList* CUpnpHttpMessage::HeaderList()
    {
    return iHeaderList;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetType
// Sets the type of message. Used by ControlPoint
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetType(THTTPMsgType aType)
    {
    iType = aType;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Type
// Returns the Http message type, used by controlpoint.
// -----------------------------------------------------------------------------
//
EXPORT_C THTTPMsgType CUpnpHttpMessage::Type() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::UpnpError
// Returns textual presentation of the given error.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpHttpMessage::UpnpError(TUpnpErrorCode aError)
    {
    switch(aError)    
        {    
        case EHttpOk:
            return KHttpOk();
            
        case EBadRequest:
            return KBadRequest();
        case EInvalidAction:
            return KInvalidAction();
        case EInvalidArgs:
            return KInvalidArgs();
        case EInvalidVar:
            return KInvalidVar();
        case EPreconditionFailed:
            return KPreconditionFailed();
            
        case EInternalServerError:
            return KInternalServerError();
        case EActionFailed:
            return KActionFailed();
            
        // 600-699 Common action errors:    
        case EArgumentValue:
            return KArgumentValue();
        case EArgumentRange:
            return KArgumentRange();
        case ENotImplemented:
            return KNotImplemented();
        case EOutOfMemory:
            return KOutOfMemory();
        case EHumanIntervention:
            return KHumanIntervention();
        case EStringTooLong:
            return KStringTooLong();
        case ENotAuthorized:
            return KNotAuthorized();
        case ESignatureFailure:
            return KSignatureFailure();
        case ESignatureMissing:
            return KSignatureMissing();
        case ENotEncrypted:
            return KNotEncrypted();
        case EInvalidSequence:
            return KInvalidSequence();
        case EInvalidUrl:
            return KInvalidUrl();
        case ENoSession:
            return KNoSession();
            
        // 700-799 Action specific errors:    
        case ENoSuchObject:
            return KNoSuchObject();
        case EInvalidCurrentTag:
            return KInvalidCurrentTag();
        case EInvalidNewTag:
            return KInvalidNewTag();
        case ERequiredTag:
            return KRequiredTag();
        case EReadOnlyTag:
            return KReadOnlyTag();
        case EParameterMismatch:
            return KParameterMismatch();
        case EInvalidSearch:
            return KInvalidSearch();
        case EInvalidSort:
            return KInvalidSort();
        case ENoContainer:
            return KNoContainer();
        case ERestrictedObject:
            return KRestrictedObject();
        case EBadMetadata:
            return KBadMetadata();
        case ERestrictedParentObject:
            return KRestrictedParentObject();
        case ENoSourceResource:
            return KNoSourceResource();
        case ESourceAccess:
            return KSourceAccess();
        case ETransferBusy:
            return KTransferBusy();
        case ENoFileTransfer:
            return KNoFileTransfer();
        case ENoDestinationResource:
            return KNoDestinationResource();
        case EDestinationAccess:
            return KDestinationAccess();
        case ECannotProcess:
            return KCannotProcess();
        default:
            return KNullDesC8();
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::UpnpErrorL
// Returns textual presentation of the given error.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpHttpMessage::UpnpErrorL(TUpnpErrorCode aError)
    {
    return UpnpError(aError).AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::HttpError
// Returns the textual error message of given error code.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpHttpMessage::HttpError(THttpStatusCode aError)
    {
    switch(aError)    
        {    
            // 100
        case EHttpContinue:
            return KHttpContinue();
        case EHttpSwitchingProtocols:
            return KHttpSwitchingProtocols();
            
            // 200
        case EHttp200Ok:
            return KHttpOk();
        case EHttpCreated:
            return KHttpCreated();
        case EHttpAccepted:
            return KHttpAccepted();
        case EHttpNonAuthoritative:
            return KHttpNonAutohorative();
        case EHttpNoContent:
            return KHttpNoContent();
        case EHttpResetContent:
            return KHttpResetContent();
        case EHttpPartialContent:
            return KHttpPartialContent();
        
            // 300

        case EHttpMultipleChoises:
            return     KHttpMultipleChoises();
        case EHttpMovedPermanently:
            return KHttpMovedPermanently();
        case EHttpFound:
            return KHttpFound();
        case EHttpSeeOther:
            return KHttpSeeOther();
        case EHttpNotModified:
            return KHttpNotModified();
        case EHttpUseProxy:
            return KHttpUseProxy();
        case EHttpTemporaryRedirect:
            return KHttpTemporaryRedirect();
            
            // 400
            
        case EHttpBadRequest:
            return KHttpBadRequest();
        case EHttpUnAuthorized:
            return KHttpUnauthorized();
        case EHttpPaymentRequired:
            return KHttpPaymentRequired();
        case EHttpForbidden:
            return KHttpForbidden();
        case EHttpNotFound:
            return KHttpNotFound();
        case EHttpMethodNotAllowed:
            return KHttpMethodNotAllowed();
        case EHttpNotAcceptable:
            return KHttpNotAcceptable();
        case EHttpProxyAuthenticationRequired:
            return KHttpProxyAuthenticationRequired();
            
        case EHttpRequestTimeout:
            return KHttpRequestTimeout();
        case EHttpConflict:
            return KHttpConflict();
        case EHttpGone:
            return KHttpGone();
        case EHttpLengthRequired:
            return KHttpLengthRequired();
        case EHttpPreconditionFailed:
            return KHttpPreconditionFailed();
        case EHttpEntityTooLarge:
            return KHttpEntityTooLarge();
        case EHttpUriTooLong:
            return KHttpUriTooLarge();
        case EHttpUnsupportedMediaType:
            return KHttpUnsupprotedMediaType();
        case EHttpRequestedRangeNotSatisfiable:
            return KHttpRequestedRangeNotSatisfiable();
        case EHttpExpectationFailed:
            return KHttpExpectationFailed();

            // 500                
        case EHttpInternalServerError:
            return KHttpInternalServerError();
        case EHttpNotImplemented:
            return KHttpNotImplemented();
        case EHttpBadGateway:
            return KHttpBadGateway();
        case EHttpServiceUnavailable:
            return KHttpServiceUnavailable();
        case EHttpGatewayTimeout:
            return KHttpGatewayTimeout();
        case EHttpVersionNotSupported:
            return KHttpVersionNotSupported();
        case EHttpInsufficientStorage:
            return KHttpInsufficientStorage();
            
        default:
            return KNullDesC8();
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Error
// Returns the error code of the message.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::Error()
    {
    if(iHeaderList && iHeaderList->Count())
        {
        CUpnpHttpHeader* hdr = iHeaderList->First();

        if (hdr != NULL)
            {            
            TDesC8& first = hdr->Name();
        
            if(first.Length() == 0)
                {
                return KErrNotFound;
                }

            TInt space1 = first.Find(KSpace8());

            if (space1 != KErrNotFound)
                {
                TPtrC8 right = first.Mid(space1+1);
                TInt space2 = right.Find(KSpace8());

                if (space2 != KErrNotFound)
                    {
                    TLex8 lex(right.Left(space2));

                    TInt error;
                    lex.Val(error);

                    return error;                
                    }
                }
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Is3xx
// Returns True if message is a 3xx response
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::Is3xx()
    { TInt err = Error();
        return (err>=300 && err<400);
    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Is1xx
// Returns True if message is a 1xx response
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::Is1xx()
    { TInt err = Error();
        return (err>=100 && err<200);
    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Is2xx
// Returns True if message is a 2xx response
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::Is2xx()
    { TInt err = Error();
        return (err>=200 && err<300);
    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::AddrOutput
// Sets the aBuf as string presentation of aAddr
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::AddrOutput(const TInetAddr& aAddr, TDes8& aBuf)
    {
    aBuf.SetLength(0);
    if (aAddr.Family() == KAfInet || aAddr.Family() == KAfInet6)
        {
        TUint32 a = aAddr.Address();
        aBuf.Format(_L8("%d.%d.%d.%d"), a>>24, (a>>16)&0xff, (a>>8)&0xff, a&0xff);
        }
    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::AddrInput
// Sets the TInetAddr object value to given aBuf.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::AddrInput(TInetAddr& aAddr, const TDesC8& aBuf)
    {
    
    TPtrC8 ptr(aBuf);
    TInt addr[4] = {0, 0, 0, 0};
    for (TInt i=0; i<4; i++)
        {
        TInt posOfDot=ptr.Find(UpnpString::KDot8);
        if(posOfDot != KErrNotFound)
            {
            TLex8 lex(ptr.Left(posOfDot));
            lex.Val(addr[i]);    
            ptr.Set(ptr.Mid(posOfDot+1));
            }
        else
            {
            if(i==3)
                {
                TLex8 lex(ptr);
                lex.Val(addr[i]);                
                }
            }

        }    
    aAddr.SetAddress((addr[0]<<24) | (addr[1]<<16) | (addr[2]<<8) | addr[3]);
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::RetryCounter
// Returns the current iRetryCounter value.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::RetryCounter()
    {
    return iRetryCounter;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetRetryCounter
// Sets the retry counter value
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetRetryCounter(TInt aValue)
    {
    iRetryCounter=aValue;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetHttpPriority
// Sets the priority on session sending this message
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetHttpPriority( TThreadPriority aPriority )
    {
    iSessionPriority = aPriority;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::HttpPriority
// Returns the priority of this message
// -----------------------------------------------------------------------------
//
EXPORT_C TThreadPriority CUpnpHttpMessage::HttpPriority()
    {
    return iSessionPriority;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetOutUri
// Sets the uri to find the local file.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetOutUriL( const TDesC8& aUri )
    {
    if( iOutUri )
        {
        delete iOutUri;
        iOutUri = NULL;
        }
    iOutUri = HBufC8::NewL( aUri.Length() );
    iOutUri->Des().Zero();
    iOutUri->Des().Append( aUri );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::OutUri
// Returns the value of OutUri.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpHttpMessage::OutUri()
    {
    if( iOutUri )
        {
        return *iOutUri;
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetLocal
// Sets if message is a local requests
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetLocal( TBool aLocal )
    {
    iLocal = aLocal;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Local
// Returns if message is a local requests.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::Local()
    {
    return iLocal;
    }
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetClientRequest
// Sets if message is a client request
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetClientRequest( TBool aClientRequest )
    {
    iClientRequest = aClientRequest;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Local
// Returns if message is a client request.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpHttpMessage::ClientRequest()
    {
    return iClientRequest;
    }
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetTimeout
// Sets the value for a TCP timeout (valid for cleint sessions)
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetTcpTimeout( TInt aTimeout )
    {
    iTcpTimeout = aTimeout;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessage::Timeout
// Returns the value of TCP timeout
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::TcpTimeout()
    {
    return iTcpTimeout;
    }
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetInternalError
// Sets internal error code
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpMessage::SetInternalError(TInt aErrorCode)
    {
    iInternalError = aErrorCode;
    }    
// -----------------------------------------------------------------------------
// CUpnpHttpMessage::SetInternalError
// Sets internal error code
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpMessage::InternalError()
    {
    return iInternalError;
    }        
//  End of File
