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
* Description:  CUpnpSubscriberLibraryElement
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <uri8.h> 
#include "upnpsubscriberlibraryelement.h"
#include "upnpgenamessage.h"
#include "upnpstring.h"
#include "upnpcommonupnplits.h"
#include "upnpcommonstructs.h"

// CONSTANTS
_LIT8(KCallbackPrefix, "<http://");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::CUpnpSubscriberLibraryElement
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpSubscriberLibraryElement::CUpnpSubscriberLibraryElement( 
                                    MUpnpTimeoutElementParent& aParent)
                : CUpnpTimeoutElement(aParent)
{

}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibraryElement::ConstructL()
{
    BaseConstructL();
    iSid = HBufC8::NewL(0);
    iCallbackPath = HBufC8::NewL(0);
    iTimeout = HBufC8::NewL(0);
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpSubscriberLibraryElement* CUpnpSubscriberLibraryElement::NewL(
                                    MUpnpTimeoutElementParent& aParent )
{
    CUpnpSubscriberLibraryElement* self = new (ELeave) 
                                        CUpnpSubscriberLibraryElement(aParent);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
}

// Destructor
CUpnpSubscriberLibraryElement::~CUpnpSubscriberLibraryElement()
{
    delete iSid;
    delete iCallbackPath;
    delete iTimeout;
    
    Cancel();
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::Sid
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDesC8& CUpnpSubscriberLibraryElement::Sid()
{
    return *iSid;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::CallbackAddress
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInetAddr& CUpnpSubscriberLibraryElement::CallbackAddress()
{
    return iCallbackAddress;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::CallbackPath
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDesC8& CUpnpSubscriberLibraryElement::CallbackPath()
{
    return *iCallbackPath;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::Seq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CUpnpSubscriberLibraryElement::Seq() const
{
    return iSeq;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::Timeout
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDesC8& CUpnpSubscriberLibraryElement::Timeout()
{
    return *iTimeout;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::AddSubscriberL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpSubscriberLibraryElement::AddSubscriberL( 
                                CUpnpGenaMessage* aMessage)
    {
    if (aMessage == NULL)
        {
        return EUndefined;
        }
    
    // Errors: Missing or invalid CALLBACK
    if (aMessage->Callback().Compare(KNoHeader) == 0)
        {   
        return EPreconditionFailed;
        }
    
    // Errors: Invalid NT
    if (aMessage->Nt().Compare(UpnpGENA::KDefaultNt) != 0)
        {
        return EPreconditionFailed;
        }
    
    TPtrC8 temp;
    TPtrC8 sid(KNullDesC8);
    TPtrC8 timeout;
    
    // only 1 address allowed with this method
    temp.Set( aMessage->Callback() );
    if ( temp.Find( KCallbackPrefix ) != KErrNotFound 
         && temp.Length() < KUrlMaxLength )
        {
        temp.Set( temp.Mid( temp.Find( KLessThan8 ) + 1 ) );
        if ( temp.Find( KGreaterThan8 )  != KErrNotFound )
            {
            temp.Set( temp.Left( temp.Find( KGreaterThan8 ) ) );
            }
        else
            {
            return EPreconditionFailed;
            }
        }
    else
        {
        return EPreconditionFailed;
        }


    TPtrC8 callbackIp;
    TPtrC8 callbackPort;
    TPtrC8 callbackPath;
    
    TUriParser8 uriParser;
    if ( !uriParser.Parse(temp) )
        {
        callbackIp.Set( uriParser.Extract( EUriHost ) );
        if ( callbackIp == KNullDesC8() )
		    	{
		    	return EPreconditionFailed;	
		    	}	
        callbackPort.Set( uriParser.Extract( EUriPort ) );
        if ( callbackPort == KNullDesC8() )
            {
            callbackPort.Set(KDefaultHostPort);            
            }        
        callbackPath.Set( uriParser.Extract( EUriPath ) );       
        }
    else
        {
        // invalid callback
        return EPreconditionFailed;
        }
    
    temp.Set(aMessage->Timeout());
    
    if (temp.Find(KTimeoutPrefix) != KErrNotFound)
        {
        timeout.Set( temp.Mid( temp.Find( KTimeoutPrefix) + 
                        KTimeoutPrefix().Length() ) );
        }
    else
        {
        // timeout according to DLNA recommendation: 300 seconds
        timeout.Set(KDefaultTimeout);
        }
    
    AddInfoL(sid,
             callbackIp,
             callbackPort,
             callbackPath,
             timeout);
             
    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::RenewSubscriberL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpSubscriberLibraryElement::RenewSubscriberL(
                                CUpnpGenaMessage* aMessage )
{
    TPtrC8 temp;
    // Errors: Incompatible headers (SID already found)
    
    if (aMessage == NULL)
    {
        return EUndefined;
    }
    
    if (aMessage->Nt().Compare(KNoHeader) != 0)
    {
        return EBadRequest;
    }
    else if (aMessage->Callback().Compare(KNoHeader) != 0)
    {
        return EBadRequest;
    }
    else
    {
    }
    
    TPtrC8 timeout;
    temp.Set(aMessage->Timeout());
    
    if (temp.Find(KTimeoutPrefix) != KErrNotFound)
    {
        timeout.Set( temp.Mid( temp.Find(KTimeoutPrefix) + 
                        KTimeoutPrefix().Length() ) );
    }
    else
    {
        // timeout according to DLNA recommendation: 300 seconds
        timeout.Set(KDefaultTimeout);
    }
    
    SetTimeoutL(timeout);
    
    return EHttpOk;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::AddInfoL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibraryElement::AddInfoL( const TDesC8& aSid,  
                                                       const TDesC8& aCallbackIp, 
                                                       const TDesC8& aCallbackPort, 
                                                       const TDesC8& aCallbackPath, 
                                                       const TDesC8& aTimeout)
{
    // Adding subscriber information

    SetSidL(aSid);
    SetCallbackAddress(aCallbackIp, aCallbackPort);
    SetCallbackPathL(aCallbackPath);
    SetTimeoutL(aTimeout);
    SetSeq(0);
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::SetSidL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibraryElement::SetSidL(const TDesC8& aSid)
{
    delete iSid;
    iSid = NULL;
    iSid = aSid.AllocL();
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::SetCallbackAddress
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibraryElement::SetCallbackAddress(const TDesC8& aIp, 
                                                            const TDesC8& aPort)
{
    CUpnpHttpMessage::AddrInput(iCallbackAddress, aIp);
    
    TLex8 lex(aPort);
    TInt port;
    lex.Val(port);
    
    iCallbackAddress.SetPort(port);
    iCallbackAddress.SetFamily(KAfInet);
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::SetCallbackPathL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibraryElement::SetCallbackPathL( const TDesC8& aCallbackPath)
{
    delete iCallbackPath;
    iCallbackPath = NULL;
    iCallbackPath = aCallbackPath.AllocL();
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::SetSeq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibraryElement::SetSeq(TUint32 aSeq)
{
    iSeq = aSeq;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::IncreaseSeq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibraryElement::IncreaseSeq()
{
    if (iSeq < KMaxSeq)
    {
        iSeq++;
    }
    else
    {
        iSeq = KMinSeq;
    }
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::SetSessId
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibraryElement::SetSessId(TInt aSessId)
{
    iSessId = aSessId;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::SessId
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpSubscriberLibraryElement::SessId()
{
    return iSessId;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibraryElement::SetTimeoutL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibraryElement::SetTimeoutL(const TDesC8& /*aTimeout*/)
{
    


        // 7.2.22.1 UPnP devices must send events to all properly subscribed 
        // UPnP control points.  
        // The device must enforce a subscription TIMEOUT value of 5 minutes.
    SetTimeout( KDefaultTimeoutValue );
    SetRenew(CUpnpTimeoutElement::EOnce);
    
    // set text-formed timeout
    delete iTimeout;
    iTimeout = NULL;
    iTimeout = KDefaultTimeout().AllocL();

}

//  End of File
