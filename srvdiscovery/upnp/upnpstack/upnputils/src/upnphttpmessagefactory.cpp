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
* Description:  HTTP message factory
*
*/


// INCLUDES
#include "upnphttpmessagefactory.h"
#include "upnpstring.h"
#include "upnpcons.h"
#include "upnpcommonupnplits.h"

_LIT8(KUpnpBodyMsgError1,"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n<html><head>\n<title>");
_LIT8(KUpnpBodyMsgError2,"</title>\n</head><body>\n<h1>");
_LIT8(KUpnpBodyMsgError3,"</h1>\n<hr />\n</body></html>");
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpGetL
// HTTP GET
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpGetL( TInetAddr& aDestination,
                                                      const TDesC8& aPath )
    {
    CUpnpHttpMessage* msg = CUpnpHttpMessage::NewL(aDestination);
    CleanupStack::PushL(msg);
    msg->SetDestinationPathL( aPath );

    TInt headerLen = 0;

    headerLen += KHttpGet().Length();
    headerLen += UpnpString::KSpace().Length();
    headerLen += UpnpString::KSlash().Length();
    headerLen += aPath.Length();
    headerLen += KHttp11().Length();

    HBufC8* header = HBufC8::NewLC(headerLen);

    header->Des().Zero();
    header->Des().Append(KHttpGet());
    header->Des().Append(UpnpString::KSpace());
    if (aPath.Length() >= UpnpString::KSlash().Length())
        {
        if (aPath.Find(UpnpString::KSlash()) != 0)
            {
            header->Des().Append(UpnpString::KSlash());
            }
        }
    else
        {
        header->Des().Append(UpnpString::KSlash());
        }
    header->Des().Append(aPath);
    header->Des().Append(KHttp11);

    msg->AddPairL(*header, KNullDesC8());

    SetHostHeaderL( msg, aDestination );

    msg->AddPairL( UpnpHTTP::KHdrCacheControl(), UpnpHTTP::KNoCache());

    CleanupStack::PopAndDestroy(); // header
    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpGetL
// HTTP GET
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpGetL( TInetAddr& aDestination,
                                                      const TDesC8& aHeadPath,
                                                      const TDesC8& aTailPath )
    {
    HBufC8 *wholePath = HBufC8::NewLC(aHeadPath.Length() + aTailPath.Length());
    wholePath->Des().Append(aHeadPath);
    wholePath->Des().Append(aTailPath);
    CUpnpHttpMessage* msg = HttpGetL(aDestination, *wholePath);
    CleanupStack::PopAndDestroy(wholePath);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpGetL
// HTTP GET
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpGetL(const TDesC8& aUrl)
    {
    TPtrC8 PathPtr;
    PathPtr.Set(aUrl);
    TInt PosOfDoubleSlash = PathPtr.FindC(UpnpString::KDoubleSlash());
    TInt PosOfSlash=0;
    if(PosOfDoubleSlash==KErrNotFound)
        {
        // no "//" found, so we assume that protocol (http://, ftp://)
        // is not used in front of aPath
        PosOfSlash=PathPtr.Find(UpnpString::KSlash());
        if(PosOfSlash != 0)
            {
            PathPtr.Set(PathPtr.Mid(PosOfSlash+1));
            }
        }
    else
        {
        // DoubleSlash found, so we have to get next slash after doubleSlash.
        PathPtr.Set(PathPtr.Mid( PosOfDoubleSlash+2 ));
        PosOfSlash=PathPtr.Find(UpnpString::KSlash());
        if(PosOfSlash != KErrNotFound)
            {
            PathPtr.Set(PathPtr.Mid(PosOfSlash));
            }
        }

    if(PathPtr.Length() == 0)
        {
        PathPtr.Set(aUrl.Mid( PosOfSlash+PosOfDoubleSlash+2 ));
        }

    TPtrC8 addressPtr;
    addressPtr.Set(aUrl);
    if(PosOfDoubleSlash != KErrNotFound)
        {
        addressPtr.Set(addressPtr.Mid(PosOfDoubleSlash+2));
        }

    PosOfSlash=0;
    PosOfSlash=addressPtr.Find(UpnpString::KSlash());
    if(PosOfSlash != KErrNotFound)
        {
        addressPtr.Set(addressPtr.Left(PosOfSlash));
        }

    TInetAddr addr;

    TInt port=KDefaultHttpPortNumber; // default port of http.

    TInt PosOfColon=addressPtr.Find(UpnpString::KColon());
    if(PosOfColon != KErrNotFound)
        {

        // port is given in url, so we use that one.
        TPtrC8 portPtr;
        portPtr.Set(addressPtr);
        portPtr.Set(addressPtr.Mid(PosOfColon+1));

        TLex8 lex = TLex8(portPtr);
        TInt error = lex.Val(port);
        if( error == KErrNone )
            {
            addressPtr.Set(addressPtr.Left(PosOfColon));
            }
        }

    addr.SetPort(port);

    CUpnpHttpMessage::AddrInput(addr, addressPtr);

    CUpnpHttpMessage* msg =  HttpGetL(addr, PathPtr);

    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpGetL
// HTTP GET
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpGetL(
                        const TDesC8& aDestination,
                        const TDesC8& aPath )
    {

    TPtrC8 tempPtr(aPath);
    TInt PosOfDoubleSlash = tempPtr.FindC(UpnpString::KDoubleSlash());
    TInt PosOfSlash=0;
    if(PosOfDoubleSlash==KErrNotFound)
        {
        // no "//" found, so we assume that protocol (http://, ftp://)
        // is not used in front of aPath
        PosOfSlash=tempPtr.Find( UpnpString::KSlash() );
        if(PosOfSlash != 0)
            {
            tempPtr.Set(tempPtr.Mid(PosOfSlash+1));
            }
        }
    else
        {
        // DoubleSlash found, so we have to get next slash after doubleSlash.
        tempPtr.Set(tempPtr.Mid( PosOfDoubleSlash+2 ));
        PosOfSlash=tempPtr.Find( UpnpString::KSlash() );
        tempPtr.Set(tempPtr.Mid(PosOfSlash));
        }

    if(tempPtr.Length() == 0)
        {
        tempPtr.Set(aPath.Mid( PosOfSlash+PosOfDoubleSlash+2 ));
        }

    TInt port(KDefaultHttpPortNumber);

    RPointerArray<TPtrC8> array;
    CleanupClosePushL(array);
    UpnpString::CutToPiecesL(aDestination, ':', array);

    if( array.Count() == 0 )
        {
        CleanupStack::PopAndDestroy( &array ); //array
        return NULL;
        }

    //if the port is defined. otherwise use 80 as a default
    if(array.Count() > 1)
        {
        UpnpString::StringToInt(  *(array)[1], &port );
        }

    TPtrC8 temp = *(array)[0];
    CleanupStack::Pop(); //array
    array.ResetAndDestroy();

    TInetAddr* addr = new (ELeave) TInetAddr();
    CleanupStack::PushL( addr );

    TInt slash = temp.Find( UpnpString::KSlash() );

    if(slash != KErrNotFound)
        {
        temp.Set(temp.Left(slash));
        }

    CUpnpHttpMessage::AddrInput(*addr, temp);
    addr->SetPort(port);

    CUpnpHttpMessage* msg =  HttpGetL(*addr, tempPtr);

    CleanupStack::PopAndDestroy( addr );

    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpHeadL
// HTTP HEAD
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpHeadL(const TDesC8& aUrl)
    {
    TPtrC8 PathPtr;
    PathPtr.Set(aUrl);
    TInt PosOfDoubleSlash = PathPtr.FindC(UpnpString::KDoubleSlash());
    TInt PosOfSlash=0;
    if(PosOfDoubleSlash==KErrNotFound)
        {
        // no "//" found, so we assume that protocol (http://, ftp://)
        // is not used in front of aPath
        PosOfSlash=PathPtr.Find(UpnpString::KSlash());
        if(PosOfSlash != 0)
            {
            PathPtr.Set(PathPtr.Mid(PosOfSlash+1));
            }
        }
    else
        {
        // DoubleSlash found, so we have to get next slash after doubleSlash.
        PathPtr.Set(PathPtr.Mid( PosOfDoubleSlash+2 ));
        PosOfSlash=PathPtr.Find(UpnpString::KSlash());
        if(PosOfSlash != KErrNotFound)
            {
            PathPtr.Set(PathPtr.Mid(PosOfSlash));
            }
        }

    if(PathPtr.Length() == 0)
        {
        PathPtr.Set(aUrl.Mid( PosOfSlash+PosOfDoubleSlash+2 ));
        }

    TPtrC8 addressPtr;
    addressPtr.Set(aUrl);
    if(PosOfDoubleSlash != KErrNotFound)
        {
        addressPtr.Set(addressPtr.Mid(PosOfDoubleSlash+2));
        }

    PosOfSlash=0;
    PosOfSlash=addressPtr.Find(UpnpString::KSlash());
    if(PosOfSlash != KErrNotFound)
        {
        addressPtr.Set(addressPtr.Left(PosOfSlash));
        }

    TInetAddr addr;

    TInt port=KDefaultHttpPortNumber; // default port of http.

    TInt PosOfColon=addressPtr.Find(UpnpString::KColon());
    if(PosOfColon != KErrNotFound)
        {

        // port is given in url, so we use that one.
        TPtrC8 portPtr;
        portPtr.Set(addressPtr);
        portPtr.Set(addressPtr.Mid(PosOfColon+1));

        TLex8 lex = TLex8(portPtr);
        TInt error = lex.Val(port);
        if( error == KErrNone )
            {
            addressPtr.Set(addressPtr.Left(PosOfColon));
            }
        }

    addr.SetPort(port);

    CUpnpHttpMessage::AddrInput(addr, addressPtr);

    CUpnpHttpMessage* msg =  HttpHeadL(addr, PathPtr);

    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpHeadL
// HTTP HEAD
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpHeadL(
                        const TDesC8& aDestination,
                        const TDesC8& aPath )
    {

    TPtrC8 tempPtr(aPath);
    TInt PosOfDoubleSlash = tempPtr.FindC(UpnpString::KDoubleSlash());
    TInt PosOfSlash=0;
    if(PosOfDoubleSlash==KErrNotFound)
        {
        // no "//" found, so we assume that protocol (http://, ftp://)
        // is not used in front of aPath
        PosOfSlash=tempPtr.Find( UpnpString::KSlash() );
        if(PosOfSlash != 0)
            {
            tempPtr.Set(tempPtr.Mid(PosOfSlash+1));
            }
        }
    else
        {
        // DoubleSlash found, so we have to get next slash after doubleSlash.
        tempPtr.Set(tempPtr.Mid( PosOfDoubleSlash+2 ));
        PosOfSlash=tempPtr.Find( UpnpString::KSlash() );
        tempPtr.Set(tempPtr.Mid(PosOfSlash));
        }

    if(tempPtr.Length() == 0)
        {
        tempPtr.Set(aPath.Mid( PosOfSlash+PosOfDoubleSlash+2 ));
        }

    RPointerArray<TPtrC8> array;
    CleanupClosePushL(array);
    UpnpString::CutToPiecesL(aDestination, ':', array);
        if( array.Count() == 0 )
        {
        CleanupStack::PopAndDestroy( &array ); //array
        return NULL;
        }
    TInt port( KDefaultHttpPortNumber);
    //if the port is defined. otherwise use 80 as a default
    if(array.Count() > 1)
        {
        UpnpString::StringToInt(  *(array)[1], &port );
        }
    TPtrC8 temp = *(array)[0];
    CleanupStack::Pop(); //array
    array.ResetAndDestroy();

    TInetAddr* addr = new (ELeave) TInetAddr();
    CleanupStack::PushL( addr );

    TInt slash = temp.Find( UpnpString::KSlash() );

    if(slash != KErrNotFound)
        {
        temp.Set(temp.Left(slash));
        }

    CUpnpHttpMessage::AddrInput(*addr, temp);
    addr->SetPort(port);

    CUpnpHttpMessage* msg =  HttpHeadL(*addr, tempPtr);

    CleanupStack::PopAndDestroy( addr );

    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpHeadL
// HTTP HEAD
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpHeadL( TInetAddr& aDestination,
                                                      const TDesC8& aPath )
    {
    CUpnpHttpMessage* msg = CUpnpHttpMessage::NewL(aDestination);
    CleanupStack::PushL(msg);
    msg->SetDestinationPathL( aPath );

    TInt headerLen = 0;

    headerLen += KHttpHead().Length();
    headerLen += UpnpString::KSpace().Length();
    headerLen += UpnpString::KSlash().Length();
    headerLen += aPath.Length();
    headerLen += KHttp11().Length();

    HBufC8* header = HBufC8::NewLC(headerLen);

    header->Des().Zero();
    header->Des().Append(KHttpHead());
    header->Des().Append(UpnpString::KSpace());
    if (aPath.Length() >= UpnpString::KSlash().Length())
        {
        if (aPath.Find(UpnpString::KSlash()) != 0)
            {
            header->Des().Append(UpnpString::KSlash());
            }
        }
    else
        {
        header->Des().Append(UpnpString::KSlash());
        }
    header->Des().Append(aPath);
    header->Des().Append(KHttp11);

    msg->AddPairL(*header, KNullDesC8());

    SetHostHeaderL( msg, aDestination );

    CleanupStack::PopAndDestroy(); // header
    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpPostL
// HTTP POST
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpPostL(const TDesC8& aUrl)
    {
    TPtrC8 PathPtr;
    PathPtr.Set(aUrl);
    TInt PosOfDoubleSlash = PathPtr.FindC(UpnpString::KDoubleSlash());
    TInt PosOfSlash=0;
    if(PosOfDoubleSlash==KErrNotFound)
        {
        // no "//" found, so we assume that protocol (http://, ftp://)
        // is not used in front of aPath
        PosOfSlash=PathPtr.Find(UpnpString::KSlash());
        if(PosOfSlash != 0)
            {
            PathPtr.Set(PathPtr.Mid(PosOfSlash+1));
            }
        }
    else
        {
        // DoubleSlash found, so we have to get next slash after doubleSlash.
        PathPtr.Set(PathPtr.Mid( PosOfDoubleSlash+2 ));
        PosOfSlash=PathPtr.Find(UpnpString::KSlash());
        if(PosOfSlash != KErrNotFound)
            {
            PathPtr.Set(PathPtr.Mid(PosOfSlash));
            }
        }

    if(PathPtr.Length() == 0)
        {
        PathPtr.Set(aUrl.Mid( PosOfSlash+PosOfDoubleSlash+2 ));
        }

    TPtrC8 addressPtr;
    addressPtr.Set(aUrl);
    if(PosOfDoubleSlash != KErrNotFound)
        {
        addressPtr.Set(addressPtr.Mid(PosOfDoubleSlash+2));
        }

    PosOfSlash=0;
    PosOfSlash=addressPtr.Find(UpnpString::KSlash());
    if(PosOfSlash != KErrNotFound)
        {
        addressPtr.Set(addressPtr.Left(PosOfSlash));
        }

    TInetAddr addr;
    HBufC* addrBuf;
    TInt port=KDefaultHttpPortNumber; // default port of http

    TInt PosOfColon=addressPtr.Find(UpnpString::KColon());
    if(PosOfColon != KErrNotFound)
        {
        // port is given in url, so we use that one.
        TPtrC8 portPtr;
        portPtr.Set(addressPtr);
        portPtr.Set(addressPtr.Mid(PosOfColon+1));

        TLex8 lex = TLex8(portPtr);
        lex.Val(port);

        addressPtr.Set(addressPtr.Left(PosOfColon));
        }

    addr.SetPort(port);

    addrBuf=UpnpString::ToUnicodeL(addressPtr);

    CUpnpHttpMessage::AddrInput(addr, addressPtr);

    delete addrBuf;
    addrBuf=NULL;

    CUpnpHttpMessage* msg =  HttpPostL(addr, PathPtr);

    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpPostL
// HTTP POST
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpPostL(
                            const TDesC8& aDestination,
                            const TDesC8& aPath )
    {
    RPointerArray<TPtrC8> array;
    CleanupClosePushL(array);
    UpnpString::CutToPiecesL(aDestination, ':',array);
    TInetAddr addr;
    if(array.Count() > 1)
        {
        TInt port;
        if((array)[1]->Length() >0)
            {
            TLex8 lex = TLex8( *(array)[1] );
            lex.Val(port);
            }
        else
            {
            CleanupStack::Pop(); //array
            array.ResetAndDestroy();
            return NULL;
            }

        if((array)[0]->Length() > 0)
            {
            CUpnpHttpMessage::AddrInput(addr, *(array)[0]);
            addr.SetPort( port);
            }
        else
            {
            CleanupStack::Pop(); //array
            array.ResetAndDestroy();
            return NULL;
            }
        }
    else
        {
        CleanupStack::Pop(); //array
        array.ResetAndDestroy();
        return NULL;
        }
    CleanupStack::Pop();
    array.ResetAndDestroy();
    return HttpPostL(addr, aPath);
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpPostL
// HTTP POST
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpPostL(
                            const TInetAddr& aDestination,
                            const TDesC8& aPath )
    {
    CUpnpHttpMessage* msg = CUpnpHttpMessage::NewL(aDestination);
    CleanupStack::PushL(msg);
    msg->SetDestinationPathL( aPath );
    HBufC8* temp = HBufC8::NewLC( KHttpPost().Length() +
                                  UpnpString::KSpace().Length() +
                                  aPath.Length() +
                                  KHttp11().Length() );

    temp->Des().Zero();
    temp->Des().Append(KHttpPost());
    temp->Des().Append(UpnpString::KSpace());
    temp->Des().Append(aPath);
    temp->Des().Append(KHttp11);
    msg->AddPairL(*temp, KNullDesC8());

    SetHostHeaderL( msg, aDestination );

    msg->AddPairL(UpnpHTTP::KConnection(), UpnpHTTP::KClose());
    CleanupStack::PopAndDestroy(temp);
    CleanupStack::Pop();
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpResponseOkL
// HTTP response OK
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpResponseOkL(
                                        CUpnpHttpMessage* aMessage )
    {
    CUpnpHttpMessage* msg = CUpnpHttpMessage::NewL(aMessage->Sender(),
        aMessage->SessionId());
    CleanupStack::PushL(msg);

    msg->AddPairL(KHttp11Ok(), KNullDesC8());


    msg->SetDestinationPathL(aMessage->SenderPath());
    msg->SetSenderPathL(aMessage->DestinationPath());

    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpResponseOkL
// HTTP response OK
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpResponseOkL(
                                    const TInetAddr& aDestination )
    {
    CUpnpHttpMessage* msg = CUpnpHttpMessage::NewL(aDestination);
    CleanupStack::PushL(msg);

    msg->AddPairL(KHttp11Ok(), KNullDesC8());

    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::Http11Response
// HTTP response
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::Http11ResponseL(
                                    CUpnpHttpMessage *aMessage,
                                    TInt aStatus )
    {
    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::Http11ResponseL( aMessage->Sender(), aStatus );
    CleanupStack::PushL(msg);

    msg->SetSessionId( aMessage->SessionId() );
    msg->SetDestinationPathL(aMessage->SenderPath());
    msg->SetSenderPathL(aMessage->DestinationPath());

    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::Http11Response
// HTTP response
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::Http11ResponseL(
                                    const TInetAddr& aDestination,
                                    TInt aStatus )
    {
    CUpnpHttpMessage* msg = CUpnpHttpMessage::NewL(aDestination);
    CleanupStack::PushL(msg);
    THttpStatusCode status = (THttpStatusCode) aStatus;

    TBuf8<KMaxIntegerLength> num;
    num.Num( aStatus );

    HBufC8* tempBuf;
    tempBuf=HBufC8::NewLC( KHttp11WithoutSpace().Length() + KSpace().Length() +
                            num.Length() + KSpace().Length() +
                            (CUpnpHttpMessage::HttpError(status)).Length() );
    tempBuf->Des().Zero();
    tempBuf->Des().Append(KHttp11WithoutSpace());
    tempBuf->Des().Append( KSpace() );
    tempBuf->Des().Append( num );
    tempBuf->Des().Append( KSpace );
    tempBuf->Des().Append(CUpnpHttpMessage::HttpError(status));


    msg->AddPairL((TDesC8&) *tempBuf, (TDesC8&) KNullDesC8());

    CleanupStack::PopAndDestroy(tempBuf);
    tempBuf=NULL;

    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpResponseErrorL
// HTTP response ERROR
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpResponseErrorL(
                                    CUpnpHttpMessage *aMessage,
                                    TInt aError )
    {
    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL(
        aMessage->Sender(), aError );
    CleanupStack::PushL(msg);

    msg->SetSessionId( aMessage->SessionId() );
    msg->SetDestinationPathL(aMessage->SenderPath());
    msg->SetSenderPathL(aMessage->DestinationPath());

    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::HttpResponseErrorL
// HTTP response ERROR
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::HttpResponseErrorL(
                            const TInetAddr& aDestination,
                            TInt aError )
    {
    CUpnpHttpMessage* msg = CUpnpHttpMessage::NewL(aDestination);
    CleanupStack::PushL(msg);
    THttpStatusCode error = (THttpStatusCode) aError;

    HBufC8* tempBuf;
    tempBuf=HBufC8::NewLC((CUpnpHttpMessage::HttpError(error)).Length()+KMaxIntegerLength);
    tempBuf->Des().Zero();

    tempBuf->Des().Num(aError);
    tempBuf->Des().Append( KSpace );
    tempBuf->Des().Append(CUpnpHttpMessage::HttpError(error));

    HBufC8* tempBuf2;
    tempBuf2=HBufC8::NewLC(tempBuf->Des().Length()+KHttp11().Length()+1);
    tempBuf2->Des().Zero();
    tempBuf2->Des().Append(KHttp11WithoutSpace());
    tempBuf2->Des().Append(UpnpString::KSpace());
    tempBuf2->Des().Append(*tempBuf);

    msg->AddPairL((TDesC8&) *tempBuf2, (TDesC8&) KNullDesC8());

    CleanupStack::PopAndDestroy(tempBuf2);
    tempBuf2=NULL;

    HBufC8* BodyBuf;
    BodyBuf=HBufC8::NewLC( KUpnpBodyMsgError1().Length() +
                           KUpnpBodyMsgError2().Length() +
                           KUpnpBodyMsgError3().Length() +
                           ( tempBuf->Length() * 2 ) );
    BodyBuf->Des().Zero();
    BodyBuf->Des().Append(KUpnpBodyMsgError1());
    BodyBuf->Des().Append(*tempBuf);
    BodyBuf->Des().Append(KUpnpBodyMsgError2());
    BodyBuf->Des().Append(CUpnpHttpMessage::HttpError(error));
    BodyBuf->Des().Append(KUpnpBodyMsgError3());

    msg->SetBodyL( *BodyBuf);

    CleanupStack::PopAndDestroy(BodyBuf);
    CleanupStack::PopAndDestroy(tempBuf);
    tempBuf=NULL;
    BodyBuf=NULL;

    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::UpnpResponseErrorL
// UPnP response ERROR
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::UpnpResponseErrorL(
                            CUpnpHttpMessage* aMessage,
                            TUpnpErrorCode aError )
    {
    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::UpnpResponseErrorL(
        aMessage->Sender(), aError);
    CleanupStack::PushL(msg);

    msg->SetSessionId(aMessage->SessionId());
    msg->SetDestinationPathL(aMessage->SenderPath());
    msg->SetSenderPathL(aMessage->DestinationPath());

    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::UpnpResponseErrorL
// UPnP response ERROR
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* RUpnpHttpMessageFactory::UpnpResponseErrorL(
                            const TInetAddr& aDestination,
                            TUpnpErrorCode aError )
    {
    CUpnpHttpMessage* msg = CUpnpHttpMessage::NewL(aDestination);
    CleanupStack::PushL(msg);

    TBuf8<KMaxIntegerLength> num;
    num.Num((TInt)aError);

    HBufC8* tempBuf;
    tempBuf=HBufC8::NewLC((CUpnpHttpMessage::UpnpError(aError)).Length()
                            +KMaxIntegerLength+KHttp11().Length()+1);
    tempBuf->Des().Zero();
    tempBuf->Des().Append(KHttp11WithoutSpace());
    tempBuf->Des().Append(UpnpString::KSpace());
    tempBuf->Des().Append(num);
    tempBuf->Des().Append( KSpace );
    tempBuf->Des().Append(CUpnpHttpMessage::UpnpError(aError));

    msg->AddPairL((TDesC8&) *tempBuf, (TDesC8&) KNullDesC8());

    CleanupStack::PopAndDestroy(tempBuf);
    tempBuf=NULL;

    CleanupStack::Pop(msg);
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpHttpMessageFactory::SetHostHeaderL
// Sets Host HTTP header to given message basing on given destinstion address.
// -----------------------------------------------------------------------------
//
void RUpnpHttpMessageFactory::SetHostHeaderL( CUpnpHttpMessage* aMessage, const TInetAddr& aDestination )
    {

    TInt port = aDestination.Port();
    TBuf8<UpnpString::KShortStringLength> addrBuf;
    TBuf8<UpnpString::KMaxTUintLength> portBuf;
    portBuf.Num( port );

    CUpnpHttpMessage::AddrOutput(aDestination, addrBuf);

    HBufC8* hostBuf=HBufC8::NewLC(addrBuf.Length() + KColon().Length() + portBuf.Length());
    TPtr8 hostPtr = hostBuf->Des();
    hostPtr.Append( addrBuf );
    hostPtr.Append( KColon() );
    hostPtr.Append( portBuf );

    aMessage->AddPairL(UpnpSSDP::KHdrHost(),*hostBuf);

    CleanupStack::PopAndDestroy( hostBuf );
    }

//  End of File

