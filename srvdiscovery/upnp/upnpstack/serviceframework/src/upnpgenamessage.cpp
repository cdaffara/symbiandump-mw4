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
* Description:  Implements the CUpnpGenaMessage class.
*
*/


// INCLUDE FILES
#include "upnpgenamessage.h"
#include "upnphttpheader.h"
#include "upnpcommonupnplits.h"
#include "upnplist.h"

using namespace UpnpGENA;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::CUpnpGenaMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage::CUpnpGenaMessage():CUpnpHttpMessage()
{

}

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* CUpnpGenaMessage::NewL( const TInetAddr& aAddr )
{
    CUpnpGenaMessage* self = CUpnpGenaMessage::NewLC(aAddr);    
    CleanupStack::Pop(self);    
    return self;        
}

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* CUpnpGenaMessage::NewLC( const TInetAddr& aAddr )
{
    CUpnpGenaMessage* self = new (ELeave) CUpnpGenaMessage();
    CleanupStack::PushL(self);
    self->ConstructL(aAddr);
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::~CUpnpGenaMessage
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage::~CUpnpGenaMessage()
{

}

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpGenaMessage::ConstructL( TInetAddr aSender) 
{
    CUpnpHttpMessage::ConstructL(aSender, NewSessionIdL());
}
        
// -----------------------------------------------------------------------------
// CUpnpGenaMessage::Nt
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpGenaMessage::Nt()
{
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
    {
        if ( hdr->Name().CompareF( UpnpSSDP::KHdrSearchTarget ) == 0 )  
        {
            return hdr->Value();
        }
        hdr = iHeaderList->Next( hdr );
    }
    
    return (TDesC8&) KNoHeader;
}

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::Nts
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpGenaMessage::Nts()
{
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
    {
        if ( hdr->Name().CompareF( UpnpSSDP::KHdrNotificationType ) == 0 )      // NTS
        {
            return hdr->Value();
        }
        hdr = iHeaderList->Next( hdr );
    }
    
    return (TDesC8&) KNoHeader;
}

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::Sid
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpGenaMessage::Sid()
{
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
    {
        if ( hdr->Name().CompareF( UpnpGENA::KSid ) == 0 )
        {
            return hdr->Value();
        }
        hdr = iHeaderList->Next( hdr );
    }
    
    return (TDesC8&) KNoHeader;
}

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::Timeout
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpGenaMessage::Timeout()
{
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
    {
        if ( hdr->Name().CompareF( UpnpGENA::KTimeout) == 0 )
        {
            return hdr->Value();
        }
        hdr = iHeaderList->Next( hdr );
    }
    
    return (TDesC8&) KNoHeader;
}

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::Callback
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpGenaMessage::Callback()
{
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
    {
        if ( hdr->Name().CompareF( KCallback ) == 0 )
        {
            return hdr->Value();
        }
        hdr = iHeaderList->Next( hdr );
    }
    
    return (TDesC8&) KNoHeader;
}

// -----------------------------------------------------------------------------
// CUpnpGenaMessage::Seq
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpGenaMessage::Seq()
{
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
    {
        if ( hdr->Name().CompareF( KSeq ) == 0 )
        {
            return hdr->Value();
        }
        hdr = iHeaderList->Next( hdr );
     }
    
    return (TDesC8&) KNoHeader;
}

//  End of File 
