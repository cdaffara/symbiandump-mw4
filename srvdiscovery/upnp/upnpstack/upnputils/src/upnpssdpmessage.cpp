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
* Description:  Represents SSDP message
*
*/


// INCLUDE FILES
#include "upnpssdpmessage.h"
#include "upnpcons.h"
#include "upnplist.h"


// LITERALS
using namespace UpnpSSDP;
using namespace UpnpHTTP;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::CUpnpSsdpMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpSsdpMessage::CUpnpSsdpMessage()
    {
    }


// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpMessage::ConstructL(TRequestType aRequestType)
    {

    switch ( aRequestType )
        {
        case ERequestNotify:
            AddPairL( KMethodNotify, KNullDesC8 );
            AddPairL( KHdrLocation, KNullDesC8 );
            AddPairL( KHdrHost, KDefaultHost );
            AddPairL( UpnpSSDP::KHdrServer, KNullDesC8 );
            AddPairL( KHdrNotificationType,(TDesC8&) KNotificationAlive );
            AddPairL( KHdrUuid, KNullDesC8 );
            AddPairL( UpnpSSDP::KHdrCacheControl, KNullDesC8 );
            AddPairL( KHdrSearchTarget, KNullDesC8 );
            break;
        case ERequestByeBye:
            AddPairL( KMethodNotify, KNullDesC8 );
            AddPairL( KHdrHost, KDefaultHost );
            AddPairL( KHdrSearchTarget, KNullDesC8 );
            AddPairL( KHdrNotificationType, KNotificationByebye );
            AddPairL( KHdrUuid, KNullDesC8 );
            
            break;
        case ERequestSearch:
            AddPairL( KMethodMSearch, KNullDesC8 );
            AddPairL( KHdrHost, KDefaultHost );
            AddPairL( KHdrMan, KNotificationDiscover );
            AddPairL( KHdrMx, KNullDesC8 );
            AddPairL( KHdrSt, KNullDesC8 );
            break;
        case ERequestSearchResponse:
            {
            AddPairL( UpnpHTTP::KHTTPOk, KNullDesC8 );
            AddPairL( UpnpSSDP::KHdrCacheControl, KNullDesC8 );
            AddPairL( UpnpHTTP::KHdrDate, KNullDesC8 );
            AddPairL( UpnpHTTP::KHdrExtWithoutColon, KNullDesC8 );
            AddPairL( KHdrLocation, KNullDesC8 );
            AddPairL( UpnpSSDP::KHdrServer, KNullDesC8 );
            AddPairL( KHdrSt, KNullDesC8 );
            AddPairL( KHdrUuid, KNullDesC8 );
            
            TTime now;
            now.UniversalTime();
            
            this->SetMessageDateL( now );
            }
            break;
        default:
            break;
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSsdpMessage* CUpnpSsdpMessage::NewL( TInetAddr& aAddr, 
                                  		   TRequestType aRequestType )
    {
    CUpnpSsdpMessage* self = new (ELeave) CUpnpSsdpMessage();
    CleanupStack::PushL(self );
    self->HttpConstructL( aAddr );
    self->ConstructL(aRequestType );
    CleanupStack::Pop(self );
    
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSsdpMessage* CUpnpSsdpMessage::NewL( TRequestType aRequestType )
    {
    CUpnpSsdpMessage* self = new (ELeave) CUpnpSsdpMessage();
    CleanupStack::PushL(self );
    
    TInetAddr tempAddr = TInetAddr(INET_ADDR(0,0,0,0),0 );
    self->HttpConstructL( tempAddr );
    self->ConstructL( aRequestType );

    CleanupStack::Pop( self );
    
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSsdpMessage* CUpnpSsdpMessage::NewL(TDesC8& aBuffer, TInetAddr& aAddr)
    {
    CUpnpSsdpMessage* self = new (ELeave) CUpnpSsdpMessage();
    CleanupStack::PushL(self);
    aAddr.SetFamily(KAfInet);
    self->HttpConstructL( aAddr );
    self->ParseL(aBuffer);
    CleanupStack::Pop(self);
    
    return self;
    }

// -----------------------------------------------------------------------------
// CCSsdpMessage::~CUpnpSsdpMessage
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpSsdpMessage::~CUpnpSsdpMessage()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::AddHostL
// Add host.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSsdpMessage::AddHostL(TDesC8& aHost)
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name().CompareF( KHdrHost() )== 0 )
            {
            hdr->SetValueL(aHost);
            return;
            }
        hdr = iHeaderList->Next(hdr);	
        }
    // header not found, adding it.
    AddPairL( KHdrHost(), aHost );
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::AddUsnL
// Add USN.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpMessage::AddUsnL(TDesC8& aUsn)
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name().CompareF( KHdrUuid )== 0 )
            {
            hdr->SetValueL(aUsn);
            return;
            }
        hdr = iHeaderList->Next(hdr);	
        }
    // header not found, adding it.       
    AddPairL( KHdrUuid(), aUsn );        
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::AddCacheControlL
// Add cache control.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpMessage::AddCacheControlL(TDesC8& aMaxAge)
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name().CompareF( UpnpSSDP::KHdrCacheControl )== 0 )
            {
            hdr->SetValueL( (TDesC8&) aMaxAge);
            return;
            }
        hdr = iHeaderList->Next(hdr);	
        }
    // header not found, adding it.        
    AddPairL( UpnpSSDP::KHdrCacheControl(), aMaxAge );
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::AddLocationL
// Add location.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpMessage::AddLocationL(TDesC8& aDescriptionLocation)
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name ().CompareF( KHdrLocation )== 0 )
            {
            hdr->SetValueL(aDescriptionLocation);
            return;
            }
        hdr = iHeaderList->Next(hdr);	
        }
    // header not found, adding it.        
    AddPairL( KHdrLocation(), aDescriptionLocation );        
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::AddSearchTargetStringL
// Add search string.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpMessage::AddSearchTargetStringL(TDesC8& aSt)
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name ().CompareF( KHdrSt )== 0 )
            {
            hdr->SetValueL(aSt);
            return;
            }
        hdr = iHeaderList->Next(hdr);	
        }
    // header not found, adding it.        
    AddPairL( KHdrSt(), aSt );        
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::AddServerL
// Add server.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpMessage::AddServerL(TDesC8& aServerType)
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name ().CompareF( UpnpSSDP::KHdrServer )== 0 )
            {
            hdr->SetValueL(aServerType);
            return;
            }
        hdr = iHeaderList->Next(hdr);	
        }
    // header not found, adding it.        
    AddPairL( UpnpSSDP::KHdrServer(), aServerType );            
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::AddNotificationTypeL
// Add notification type.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpMessage::AddNotificationTypeL(TDesC8& aNT)
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name ().CompareF( KHdrSearchTarget )== 0 )
            {
            hdr->SetValueL(aNT);
            return;
            }
        hdr = iHeaderList->Next(hdr);	
        }
    // header not found, adding it.        
    AddPairL( KHdrSearchTarget(), aNT );         
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::AddMxL
// Add MX.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSsdpMessage::AddMxL(TDesC8& aMx)
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name ().CompareF( KHdrMx )== 0 )
            {
            hdr->SetValueL(aMx);
            return;
            }
        hdr = iHeaderList->Next(hdr);	
        }
    // header not found, adding it.        
    AddPairL( KHdrMx(), aMx );            
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::SearchTarget
// Search target.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSsdpMessage::SearchTarget()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name().CompareF ( KHdrSt ) == 0 )
            {
            
            if ( hdr->Value().CompareF(KSearchTargetAll) == 0 )
                {
                return ESsdpAll;
                }
            
            else if ( hdr->Value().CompareF(KUPnPRootDevice) == 0 )
                {
                return EUPnPRootDevice;
                }
            else if ( hdr->Value().FindF(KUPnPUuid) == 0 )
                {
                return EDeviceUid;
                }
            else if ( hdr->Value().FindF(KUPnPDeviceSchema) == 0 )
                {
                return EDeviceType;
                }
            else if ( hdr->Value().FindF(KUPnPServiceSchema) == 0 )
                {
                return EServiceType;
                }
            else
                {
                return ESsdpNone;
                }		
            
            }
        hdr = iHeaderList->Next(hdr);	
        }	
    return ESsdpNone;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::IsSsdpAlive
// Return wheter SSDP is alive.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSsdpMessage::IsSsdpAlive()
    {
   
    CUpnpHttpHeader* hdr = iHeaderList->First();

    if (IsSsdpResponse())
        {
        return ETrue;
        }
    
    while ( hdr)
        {
        if ( hdr->Name().CompareF ( KHdrNotificationType ) == 0 )
            {
            if ( hdr->Value().CompareF ( KNotificationAlive ) == 0 )
                {
                return ETrue;
                }   
            }
        hdr = iHeaderList->Next(hdr);	
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::IsSsdpByebye
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSsdpMessage::IsSsdpByebye()
    {
    
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr)
        {
        if ( hdr->Name().CompareF( KHdrNotificationType ) == 0 )
            {
            if ( hdr->Value().CompareF( KNotificationByebye ) == 0 )
                {
                return ETrue;
                }
            
            }
        hdr = iHeaderList->Next(hdr);	
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::IsSsdpMSearch
// Return whether SSP search is true or false.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSsdpMessage::IsSsdpMSearch()
    {
    return Method().CompareF( UpnpSSDP::KMSearch() ) == 0;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::IsSsdpResponse
// Return whether .
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSsdpMessage::IsSsdpResponse()
    {  
    return Method().CompareF( KHttp11WithoutSpace() ) == 0;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::IsSearchAll
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSsdpMessage::IsSearchAll()
    {
    
    TBool IsSearch=IsSsdpMSearch();
    
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( ( IsSearch ) && ( hdr->Name().CompareF ( KHdrSt ) == 0 )
                && ( hdr->Value().CompareF ( KSearchTargetAll ) == 0 ) )
            {
            return ETrue;
            }
        
        hdr = iHeaderList->Next(hdr);	
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::IsSearchDevice
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSsdpMessage::IsSearchDevice()
    {
    TBool IsSearch=IsSsdpMSearch();
    
    if (IsSearch)
        {
        CUpnpHttpHeader* hdr = iHeaderList->First();
        
        while ( hdr )
            {
            if ( hdr->Name().CompareF ( KHdrSt ) == 0 )
                {
                if ( hdr->Value().FindF(KUPnPDeviceSchema) == 0 )
                    {
                    return ETrue;
                    }    
                }
                
            hdr = iHeaderList->Next(hdr);	
            }
        }
    
    return EFalse;
    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::IsSearchService
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSsdpMessage::IsSearchService()
    {
    TBool IsSearch=IsSsdpMSearch();
    
    if (IsSearch)
        {
        CUpnpHttpHeader* hdr = iHeaderList->First();
        
        while ( hdr )
            {
            if ( hdr->Name().CompareF ( KHdrSt ) == 0 )
                {
                if ( hdr->Value().FindF( KUPnPServiceSchema ) == 0 )
                    {
                    return ETrue;
                    }		 
                }
        
            hdr = iHeaderList->Next(hdr);	
            }
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::St
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpSsdpMessage::St()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    TBool valuefound=EFalse;
    
    while ( !valuefound && hdr )
        {
        if ( hdr->Name().FindF(KHdrSt()) == 0 && hdr->Name().Length() == KHdrSTLen )
            {
            return hdr->Value();
            }
        hdr = iHeaderList->Next(hdr);	
        }
    return (TDesC8&) KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::Nt
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpSsdpMessage::Nt()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    TBool valuefound=EFalse;
    
    while ( !valuefound && hdr )
        {
        if ( hdr->Name().CompareF( KHdrSearchTarget ) == 0 )
            {
            return hdr->Value();
            }
        hdr = iHeaderList->Next(hdr);	
        }
    
    return (TDesC8&) KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::Usn
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpSsdpMessage::Usn()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    TBool valuefound=EFalse;
    
    while ( !valuefound && hdr )
        {
        if ( hdr->Name().CompareF( KHdrUuid ) == 0 )
            {
            return hdr->Value();
            }
        hdr = iHeaderList->Next(hdr);	
        }
    
    return (TDesC8&) KNullDesC8();
    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::Nts
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpSsdpMessage::Nts()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    TBool valuefound=EFalse;
    
    while ( !valuefound && hdr )
        {
        if ( hdr->Name().CompareF(KHdrNotificationType) == 0 )
            {
            return hdr->Value();
            }
        hdr = iHeaderList->Next(hdr);
        }
    
    return (TDesC8&) KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::ParseL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSsdpMessage::ParseL(TDesC8& aBuffer)
    {
    TInt index = 1,indexLWS=0, indexCRLF=0;
    TBool FirstLine=ETrue;
    TPtrC8 finderIndexPointer;
    TPtrC8 tempHeaderPointer;
    HBufC8* tempHeaderBuffer = aBuffer.AllocLC();
    TInt endOfHeader=0;
    endOfHeader = aBuffer.Find(UpnpString::KDoubleLineFeed);
	if( endOfHeader == KErrNotFound )
	{
		User::Leave(KErrAbort);
	}
    tempHeaderPointer.Set(aBuffer.Left(endOfHeader));
    
    while ( index != KErrNotFound && index != KErrNone )
        {
        //RFC822 3.2 field-name  =  1*<any CHAR, excluding CTLs, SPACE, and ":">
        index = tempHeaderBuffer->Des().Find(UpnpString::KLineFeed);
		indexLWS = FindLWS(tempHeaderPointer);
		finderIndexPointer.Set(tempHeaderPointer);
		indexCRLF = index;
		while (indexCRLF == indexLWS && indexCRLF != KErrNotFound)
		{
			finderIndexPointer.Set(finderIndexPointer.Right(finderIndexPointer.Length()-(indexCRLF+3)));//3 = LWS
			indexCRLF = finderIndexPointer.Find(UpnpString::KLineFeed);
			//+3 - index skips the length of KLineFeed and parses further...
			index += indexCRLF+3;
			indexLWS = FindLWS(finderIndexPointer);
		}

        
        if(index != KErrNotFound && index != KErrNone)
            {
            HBufC8* oneHeaderRow=NULL;
            oneHeaderRow=HBufC8::NewLC(index);
            oneHeaderRow->Des().Zero();
            
            TPtrC8 ptr;
            ptr.Set(tempHeaderBuffer->Des().Left(index));
            
            oneHeaderRow->Des().Append(ptr);
            
            // To check if is the first line of message 
            // (e.g. GET / HTTP 1.1 etc.)
            if (FirstLine)
                {
                ParseHeaderRowL( (TDesC8&) *oneHeaderRow, ETrue);
                FirstLine=EFalse;
                }
            else 
                {
                ParseHeaderRowL(*oneHeaderRow, EFalse);
                }
            
            
            CleanupStack::PopAndDestroy(); // oneHeaderRow
            
            // To delete one header row + "\r\n" = length+2 
            // to prepare for next row.
            tempHeaderBuffer->Des().Delete(0,index+2);
            tempHeaderPointer.Set(tempHeaderBuffer->Des());
            
            
            }
        }
    CleanupStack::PopAndDestroy(); // TempHeaderBuffer
    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::IsSsdpNotify
// Return whether SSDP notify is true or false.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSsdpMessage::IsSsdpNotify()
    {
    return Method().CompareF( UpnpSSDP::KNotify() ) == 0;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::Location
// Return location.
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpSsdpMessage::Location()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    while ( hdr )
        {
        if ( hdr->Name().CompareF(KHdrLocation) == 0 )
            {
            return hdr->Value();
            }
        hdr = iHeaderList->Next(hdr);	
        }
    
    return (TDesC8&) KNullDesC8();	
    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::Uuid
// Return UUID.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtr8 CUpnpSsdpMessage::Uuid()
    {
    TPtrC8 usn = Usn();
    
    TInt sng = usn.Find(UpnpString::KColon);
    TInt dbl = usn.Find(UpnpString::KDblColon);
    
    TPtr8 uuid(NULL, 0, 0);
    if (dbl != KErrNotFound && dbl != sng)
        {
        TPtrC8 temp = usn.Mid(sng + 1, (dbl - sng) - 1);
        uuid.Set((unsigned char*)temp.Ptr(), temp.Length(), temp.Length());
        }
    else
        {
        if (dbl == KErrNotFound)
            {
            TPtrC8 temp = usn.Right((usn.Length() - sng) - 1);
            uuid.Set((unsigned char*)temp.Ptr(), temp.Length(), temp.Length());
            }
        else
            {
            TPtrC8 temp = usn.Left(dbl);
            uuid.Set((unsigned char*)temp.Ptr(), temp.Length(), temp.Length());
            }
        }
    return uuid;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::CacheControl
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSsdpMessage::CacheControl()
    {
    CUpnpHttpHeader* hdr = iHeaderList->First();
    
    TBool valuefound=EFalse;
    
    while ( !valuefound && hdr )
        {
        if ( hdr->Name().CompareF( UpnpSSDP::KHdrCacheControl() ) == KErrNone )
            {
            
            TDesC8& value = hdr->Value();
            
            TInt index = value.FindF(KSsdpMaxAge());
            TInt eqIndex = value.Find(UpnpString::KEqual());
            
            if (index != KErrNotFound && eqIndex!= KErrNotFound && index < eqIndex)
                {
                TLex8 num(value.Right((value.Length() 
                    - UpnpString::KEqual().Length()) - eqIndex));
                num.SkipSpace();
                TInt cc;
                TInt err( num.Val(cc) );
                if ( ( cc < 0 ) || err )
                    {
                    return KDefaultMaxAgeValue;
                    }
                return cc;
                }
            return KDefaultMaxAgeValue;
            }
        hdr = iHeaderList->Next(hdr);	
        }
    return KDefaultMaxAgeValue;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::ToStringL
// Convert headers to string.
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpSsdpMessage::ToStringL()
    {
    HBufC8* buffer=NULL;
    buffer=HeadersToStringL();
    
    CleanupStack::PushL(buffer); 
    
    HBufC8* returnBuf = NULL;
    //+2 bytes reserved for KLineFeed
    returnBuf=HBufC8::NewLC(buffer->Des().Length() + 2);
    
    returnBuf->Des().Append(*buffer);
    returnBuf->Des().Append(UpnpString::KLineFeed());

    CleanupStack::Pop( returnBuf );
    CleanupStack::PopAndDestroy( buffer );

    return returnBuf;
    }
// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::AppendContentLengthToHeadersL
// Overwrites the method from CUpnpHttpMessage. It is empty on purpose!.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSsdpMessage::AppendContentLengthToHeadersL(HBufC8*& /*aHeaders*/)
	{
	}
// -----------------------------------------------------------------------------
// CUpnpSsdpMessage::HttpConstructL
// Constructor of HTTP message.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpMessage::HttpConstructL(TInetAddr& aAddr)
	{
	CUpnpHttpMessage::ConstructL( aAddr, 0 );
	}

//  End of File  
