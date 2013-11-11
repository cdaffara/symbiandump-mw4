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
* Description:  Produces SOAP messages
*
*/



// INCLUDE FILES
#include "upnpsoapmessagefactory.h"
#include "upnphttpmessagefactory.h"
#include "upnpsoapmessage.h"
#include "upnpaction.h"
#include "upnpargument.h"
#include "upnperrors.h"
#include "upnpserviceimplementation.h"
#include "upnpdevice.h"
#include "upnpstring.h"
#include "upnpcommonupnplits.h"
#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"
#include "upnplist.h"
#include "upnpcons.h"

// CONSTANTS
_LIT8(KSoapEnvelopeStart, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n<s:Body>\n<u:");
_LIT8(KSoapServiceNsStart, " xmlns:u=\"urn:schemas-upnp-org:service:");
_LIT8(KSoapServiceNsEnd, "\">\n");
_LIT8(KActionNameEnd, "</u:");
_LIT8(KSoapEnvelopeEnd, ">\n</s:Body>\n</s:Envelope>");
_LIT8(KSoapActionStart, "\"urn:schemas-upnp-org:service:");
_LIT8(KCrash, "#");
_LIT8(KOpenBracket, "<");
_LIT8(KCloseBracket, ">");
_LIT8(KLinefeed, "\n");
_LIT8(KResponse, "Response");

_LIT(KSoapErrorStart, 
     "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n<s:Body>\n<s:Fault>\n<faultcode>s:Client</faultcode>\n<faultstring>UPnPError</faultstring>\n<detail>\n<UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\">\n<errorCode>");

_LIT(KSoapErrorBetween,
     "</errorCode>\n<errorDescription>");
_LIT(KSoapErrorEnd,
     "</errorDescription>\n</UPnPError>\n</detail>\n</s:Fault>\n</s:Body>\n</s:Envelope>");

static const TInt KTagCharsLength = 7;
static const TInt KExtraCharsLength = 2;
static const TInt KMaxErrorCodeLength = 20;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RUpnpSoapMessageFactory::SoapRequest
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSoapMessage* RUpnpSoapMessageFactory::SoapRequestL(CUpnpAction* aAction)
    {
    LOGS("RUpnpSoapMessageFactory::SoapRequest(CUpnpAction*)");
    
    if (aAction == NULL)
    {
        return NULL;
    }
    const TDesC8& actName = aAction->Name();
    HBufC8* servTypeB = NULL;
    TInt posS(KErrNotFound);
    TInt posD(KErrNotFound);
    
    const TDesC8& servType = aAction->ServiceType();
    TInt serviceVersion(KErrNotFound);
    if(servType.Length())
        {
        posS = servType.LocateReverse(':');
        if (posS != KErrNotFound)
            {
            TLex8 number(servType.Right(servType.Length() - posS - 1));
            if(number.Val(serviceVersion) != KErrNone)
                {
                serviceVersion = KErrNotFound;
                }
            }
        }
    
    const TDesC8& devType = aAction->Service().Device().DeviceType();
    TInt deviceVersion(KErrNotFound);
    
    if(devType.Length())
        {
        posD = devType.LocateReverse(':');
        if (posD != KErrNotFound)
            {
            TLex8 number(devType.Right(devType.Length() - posD - 1));
            if(number.Val(deviceVersion) != KErrNone)
                {
                deviceVersion = KErrNotFound;
                }
            }
        }    
    
    if(deviceVersion != KErrNotFound && serviceVersion != KErrNotFound)
        {
        if(deviceVersion < serviceVersion)
            {
            servTypeB = HBufC8::NewLC(servType.Length() + devType.Length() - posD);
            TPtr8 buf = servTypeB->Des();
            buf.Append(servType.Left(posS + 1));
            buf.Append(devType.Right(devType.Length() - posD - 1));
            }
        else
            {
            servTypeB = servType.AllocLC();
            }           
        }
    else
        {
        servTypeB = servType.AllocLC();
        }
    
    RPointerArray<CUpnpArgument>& args = aAction->ArgumentList();
    
    TInt argLength = 0;
    
    TInt i=0;
    for(i=0; i<args.Count(); i++)
    {
        argLength += KTagCharsLength; 
        argLength += args[i]->Name().Length() * 2;
        argLength += args[i]->Value().Length();
    }
    
    argLength += KSoapEnvelopeStart().Length();
    argLength += KSoapServiceNsStart().Length();
    argLength += KSoapServiceNsEnd().Length();
    argLength += KActionNameEnd().Length();
    argLength += KSoapEnvelopeEnd().Length();
    
    argLength += actName.Length()*2;
    argLength += servTypeB->Length();
    
    HBufC8* buffer = HBufC8::NewLC(argLength);
    TPtr8 ptr = buffer->Des();
    
    ptr.Append(KSoapEnvelopeStart());
    ptr.Append(actName);
    ptr.Append(KSoapServiceNsStart());
    ptr.Append(*servTypeB);
    ptr.Append(KSoapServiceNsEnd());
    
    for (i=0; i<args.Count(); i++)
    {
        if (args[i]->Direction() == EIn)
        {
            const TDesC8& name = args[i]->Name();
            const TDesC8& value = args[i]->Value();
            
            ptr.Append(KOpenBracket());
            ptr.Append(name);
            ptr.Append(KCloseBracket());
            ptr.Append(value);
            ptr.Append(KOpenBracket());
            ptr.Append(UpnpString::KSlash());
            ptr.Append(name);
            ptr.Append(KCloseBracket());
            ptr.Append(KLinefeed());
            
        }
    }
    ptr.Append(KActionNameEnd());
    ptr.Append(actName);
    ptr.Append(KSoapEnvelopeEnd());
    
    
    CUpnpSoapMessage* self = NULL;
    
    TInetAddr dest = aAction->DestinationAddr();
    dest.SetFamily(KAfInet);
    
    TPtrC8 destPath = aAction->DestinationPath();
    if (destPath.Length() > KHttp().Length() && 
        destPath.Left( KHttp().Length() ) == KHttp())
    {
        self = static_cast<CUpnpSoapMessage*>(RUpnpHttpMessageFactory::HttpPostL(
            destPath));
    }
    else if (destPath.Find(UpnpString::KSlash()) != 0)
    {
        HBufC8* longPath = HBufC8::NewLC(destPath.Length() +1);
        longPath->Des().Append(UpnpString::KSlash());
        longPath->Des().Append(destPath);
        self = static_cast<CUpnpSoapMessage*>(RUpnpHttpMessageFactory::HttpPostL(
                                                        dest, *longPath));
        
        CleanupStack::PopAndDestroy( longPath );
    }
    else
    {
        self = static_cast<CUpnpSoapMessage*>(RUpnpHttpMessageFactory::HttpPostL(
                                                        dest, destPath));
    }
    
    CleanupStack::PushL(self);
    CUpnpHttpHeaderList* hdrList=self->HeaderList();
    
    CUpnpHttpHeader* hdr = hdrList->First();
    
    while ( hdr )
    {
     TBool flag = EFalse;
        if(hdr->Name().Length() == UpnpSSDP::KHdrHost().Length())
        {
            if (hdr->Name().FindC(UpnpSSDP::KHdrHost()) == 0)
            {
				CUpnpHttpHeader* temp = hdr;
				hdr = hdrList->Next(hdr);
				flag = ETrue;
				hdrList->Remove(temp);
                delete temp;
				
            }
        }
		if(!flag)
			hdr = hdrList->Next(hdr);
    }
    
    HBufC8* host = UpnpString::InetToStringL( self->Sender() );
    CleanupStack::PushL( host );
    self->AddPairL( UpnpSSDP::KHdrHost(), *host );
    CleanupStack::PopAndDestroy( host );
    
    self->SetSessionId(aAction->SessionId());
    self->SetBodyL(*buffer);
    self->AddPairL(UpnpHTTP::KHdrContentType(), UpnpGENA::KTextXmlUtf8());
    
    
    HBufC8* soapAction = HBufC8::NewLC(
                   KSoapActionStart().Length() + 
                   aAction->Name().Length() + 
                   servTypeB->Length() + 
                   KExtraCharsLength);
    
    TPtr8 soapPtr = soapAction->Des();
    
    // soapPtr = "urn:schemas-upnp-org:service:
    soapPtr.Append(KSoapActionStart());
    // soapPtr = "urn:schemas-upnp-org:service:serviceType:v
    soapPtr.Append(*servTypeB);
    // soapPtr = "urn:schemas-upnp-org:service:serviceType:v#
    soapPtr.Append(KCrash());
    // soapPtr = "urn:schemas-upnp-org:service:serviceType:v#actionName
    soapPtr.Append(aAction->Name());
    // soapPtr = "urn:schemas-upnp-org:service:serviceType:v#actionName"
    soapPtr.Append(UpnpString::KInvComma());
    
    self->AddPairL(KSoapAction(), soapPtr);
    
    self->SetLocal(aAction->Local());
    CleanupStack::PopAndDestroy(soapAction);
    CleanupStack::Pop(self);
    CleanupStack::PopAndDestroy(buffer);
    
    CleanupStack::PopAndDestroy(servTypeB);
    
    return self;
}

// -----------------------------------------------------------------------------
// RUpnpSoapMessageFactory::SoapResponseL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSoapMessage* RUpnpSoapMessageFactory::SoapResponseL(CUpnpAction* aAction)
{
    LOGS("RUpnpSoapMessageFactory::SoapResponseL(CUpnpAction*)");
    if (aAction == NULL)
    {
        return NULL;
    }
    const TDesC8& actName = aAction->Name();
    const TDesC8& servType = aAction->ServiceType();
    
    RPointerArray<CUpnpArgument>& args = aAction->ArgumentList();
    
    TInt argLength = 0;
    
    TInt i=0;
    for(i=0; i<args.Count(); i++)
    {
        argLength += KTagCharsLength;
        
        argLength += args[i]->Name().Length() * 2;
        argLength += args[i]->Value().Length();
    }
    
    argLength += (2 * KResponse().Length()); 
    
    argLength += KSoapEnvelopeStart().Length();
    argLength += KSoapServiceNsStart().Length();
    argLength += KSoapServiceNsEnd().Length();
    argLength += KActionNameEnd().Length();
    argLength += KSoapEnvelopeEnd().Length();
    
    argLength += actName.Length() * 2;
    argLength += servType.Length();
    
    HBufC8* buffer = HBufC8::NewLC(argLength);
    TPtr8 ptr = buffer->Des();
    
    ptr.Append(KSoapEnvelopeStart());
    ptr.Append(actName);
    ptr.Append(KResponse());
    ptr.Append(KSoapServiceNsStart());
    ptr.Append(servType);
    ptr.Append(KSoapServiceNsEnd());
    
    for (i=0; i<args.Count(); i++)
    {
        if (args[i]->Direction() == EOut)
        {
            TDesC8& name = args[i]->Name();
            TDesC8& value = args[i]->Value();
            
            ptr.Append(KOpenBracket());
            ptr.Append(name);
            ptr.Append(KCloseBracket());
            ptr.Append(value);
            ptr.Append(KOpenBracket());
            ptr.Append(UpnpString::KSlash());
            ptr.Append(name);
            ptr.Append(KCloseBracket());
            ptr.Append(KLinefeed());
            
        }
    }
    ptr.Append(KActionNameEnd());
    ptr.Append(actName);
    ptr.Append(KResponse());
    ptr.Append(KSoapEnvelopeEnd());
    
    CUpnpSoapMessage* self = static_cast<CUpnpSoapMessage*>(
                    RUpnpHttpMessageFactory::HttpResponseOkL(aAction->Sender()));
    CleanupStack::PushL(self);
    
    self->SetSessionId(aAction->SessionId());
    
    self->SetBodyL(*buffer);
    
    self->AddPairL(UpnpHTTP::KHdrContentType(), UpnpGENA::KTextXmlUtf8());

    self->AddPairL(UpnpHTTP::KHdrExtWithoutColon(), KNullDesC8);
    
    self->SetLocal(aAction->Local());
      
    CleanupStack::Pop(self);
    CleanupStack::PopAndDestroy(buffer);
    
    return self;
}

// -----------------------------------------------------------------------------
// RUpnpSoapMessageFactory::SoapResponseL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSoapMessage* RUpnpSoapMessageFactory::SoapResponseL( CUpnpAction* aAction, TUpnpErrorCode aCode)
{
    LOGS("RUpnpSoapMessageFactory::SoapResponseL(CUpnpAction*, TUpnpErrorCode)");
    
    HBufC8* errorString = CUpnpSoapMessage::UpnpErrorL(aCode);
    CleanupStack::PushL(errorString);
    CUpnpSoapMessage* self = RUpnpSoapMessageFactory::SoapResponseL(aAction, aCode, *errorString);       
    CleanupStack::PopAndDestroy(errorString);    
    return self;   
}

EXPORT_C CUpnpSoapMessage* RUpnpSoapMessageFactory::SoapResponseL( CUpnpAction* aAction, 
                                                                   TUpnpErrorCode aCode, 
                                                                   const TDesC8& aDescription)
{
    LOGS("RUpnpSoapMessageFactory::SoapResponseL(CUpnpAction*, TUpnpErrorCode,TDesC8&)");
    
    if (aAction == NULL)
    {
        return NULL;
    }
    TBuf8<KMaxErrorCodeLength> errorCode;
    TInt code = aCode;
    errorCode.Num(code);
    
    TInt length = 0;
    length += KSoapErrorStart().Length();
    length += KSoapErrorBetween().Length();
    length += KSoapErrorEnd().Length();
    length += errorCode.Length();
    
    length += aDescription.Length();
    
    HBufC8* buffer = HBufC8::NewL(length);
    TPtr8 ptr = buffer->Des();
    
    ptr.Append(KSoapErrorStart());
    ptr.Append(errorCode);
    ptr.Append(KSoapErrorBetween());
    ptr.Append(aDescription);
    ptr.Append(KSoapErrorEnd());
    
    CleanupStack::PushL(buffer);
    
    CUpnpSoapMessage* self = static_cast<CUpnpSoapMessage*>(
        RUpnpHttpMessageFactory::HttpResponseErrorL(aAction->Sender(), 500));
    
    CleanupStack::PushL(self);
    self->SetSessionId(aAction->SessionId());
    
    self->SetBodyL(*buffer);
    self->AddPairL(UpnpHTTP::KHdrContentType(), UpnpGENA::KTextXmlUtf8());
    self->AddPairL(UpnpHTTP::KHdrExtWithoutColon(), KNullDesC8);
    
    self->SetLocal(aAction->Local());

        CleanupStack::Pop(self);
    CleanupStack::PopAndDestroy(buffer);
    
    return self;
}
                                                           
// -----------------------------------------------------------------------------
// RUpnpSoapMessageFactory::SoapResponseL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSoapMessage* RUpnpSoapMessageFactory::SoapResponseL( 
                                            CUpnpSoapMessage* aMessage,
                                            TUpnpErrorCode aCode )
{
    if (aMessage == NULL)
    {
        return NULL;
    }
    TBuf8<KMaxErrorCodeLength> errorCode;
    TInt code = aCode;
    errorCode.Num(code);
    
    TInt length = 0;
    length += KSoapErrorStart().Length();
    length += KSoapErrorBetween().Length();
    length += KSoapErrorEnd().Length();
    length += errorCode.Length();
    
    HBufC8* errorString = CUpnpSoapMessage::UpnpErrorL(aCode);
    CleanupStack::PushL(errorString);
    
    length += errorString->Length();
    
    HBufC8* buffer = HBufC8::NewLC(length);
    TPtr8 ptr = buffer->Des();
    
    ptr.Append(KSoapErrorStart());
    ptr.Append(errorCode);
    ptr.Append(KSoapErrorBetween());
    ptr.Append(*errorString);
    ptr.Append(KSoapErrorEnd());
    
    CUpnpSoapMessage* self = static_cast<CUpnpSoapMessage*>(
            RUpnpHttpMessageFactory::HttpResponseErrorL(aMessage->Sender(), 500));
    
    self->SetSessionId(aMessage->SessionId());
    
    self->SetBodyL(*buffer);
    self->AddPairL(UpnpHTTP::KHdrContentType(), UpnpGENA::KTextXmlUtf8());
    self->AddPairL(UpnpHTTP::KHdrExtWithoutColon(), KNullDesC8);

    self->SetLocal(aMessage->Local());
  
    CleanupStack::PopAndDestroy(2);
    
    return self;
}

//  End of File 
