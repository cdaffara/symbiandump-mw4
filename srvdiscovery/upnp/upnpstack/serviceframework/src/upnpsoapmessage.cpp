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
* Description:  Represents SOAP message
*
*/


// INCLUDE FILES
#include "upnpsoapmessage.h"
#include "upnpstring.h"
#include "upnplist.h"

// CONSTANTS
_LIT8(KCross, "#");
_LIT8(KService, ":service:");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpSoapMessage::CUpnpSoapMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpSoapMessage::CUpnpSoapMessage():CUpnpHttpMessage()
{

}

// -----------------------------------------------------------------------------
// CUpnpSoapMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSoapMessage* CUpnpSoapMessage::NewL( TDesC8& aBuffer, 
                                                   const TInetAddr& aAddr)
{
    
    CUpnpSoapMessage* self = new (ELeave) CUpnpSoapMessage();
    CleanupStack::PushL(self);
    self->ConstructL(aAddr);
    self->ParseL(aBuffer);
    CleanupStack::Pop(self);
        
    return self;
}

// Destructor
EXPORT_C CUpnpSoapMessage::~CUpnpSoapMessage()
{

}

// -----------------------------------------------------------------------------
// CUpnpSoapMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpSoapMessage::ConstructL( TInetAddr aSender) 
{
    CUpnpHttpMessage::ConstructL(aSender, NewSessionIdL());
}
    
// -----------------------------------------------------------------------------
// CUpnpSoapMessage::SoapAction
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpSoapMessage::SoapAction()
{
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
    {
        if ( hdr->Name().CompareF( KSoapAction() ) == 0 )
        {
            return hdr->Value();
        }
        
        hdr = iHeaderList->Next( hdr );
    }
    
    return KNullDesC8();
}

// -----------------------------------------------------------------------------
// CUpnpSoapMessage::ActionName
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpSoapMessage::ActionName()
{
    const TPtrC8 soap = SoapAction();
    
    if (soap.Length() > 0)
    {
        TInt crossIndex = soap.Find( KCross() );
        
        if ( 0 <= crossIndex )
        {
            return soap.Mid(crossIndex + 1, soap.Length() - (crossIndex + 2));
        }
    }
    // If soap is zero or KCross is not found, return KNullDesC8
    return KNullDesC8();
}

// -----------------------------------------------------------------------------
// CUpnpSoapMessage::ServiceType
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpSoapMessage::ServiceType()
{
    const TPtrC8 soap = SoapAction();
    
    if (soap.Length() > 0)
    {
        TInt servStart = soap.Find(KService());
        
        TInt servEnd = soap.Find(KCross());
        if( ( servStart < 0 ) ||
            ( servEnd   < 0 ) )
        {
            return KNullDesC8();
        }

        servStart += KService().Length();
        
        TInt length = (servEnd - servStart);
        
        return soap.Mid(servStart, length);
    }
    
    return KNullDesC8();
}

//  End of File
