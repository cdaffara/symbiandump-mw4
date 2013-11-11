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
* Description:  CUpnpSubscriberLibrary
*
*/



// INCLUDE FILES
#include "upnpsubscriberlibrary.h"
#include "upnpgenamessage.h"
#include "upnpcommonupnplits.h"
#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"
#include "upnpsubscriberlibraryobserver.h"
#include "upnpsubscriberlibraryelement.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibrary::CUpnpSubscriberLibrary
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpSubscriberLibrary::CUpnpSubscriberLibrary( 
                                      MUpnpSubscriberLibraryObserver& aObserver )
                                            : iObserver( aObserver )
{

}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibrary::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibrary::ConstructL()
{

}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibrary::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpSubscriberLibrary* CUpnpSubscriberLibrary::NewL( 
                                        MUpnpSubscriberLibraryObserver& aObserver )
{
    CUpnpSubscriberLibrary* self = new (ELeave) CUpnpSubscriberLibrary( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
}

// Destructor
CUpnpSubscriberLibrary::~CUpnpSubscriberLibrary()
{
    iElementArray.ResetAndDestroy();
    iElementArray.Close();
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibrary::SubscriberLibrary
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RPointerArray<CUpnpSubscriberLibraryElement> 
                CUpnpSubscriberLibrary::SubscriberLibrary() const
{
    return iElementArray;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibrary::AddInfoL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpSubscriberLibrary::AddInfoL( 
                                    CUpnpGenaMessage* aMessage, 
                                    CUpnpSubscriberLibraryElement** aElement )
{
    if ( !aMessage || !aElement )
    {
        return EUndefined;
    }
    LOGS("CUpnpSubscriberLibrary::AddInfoL");
    

    if ( (   aMessage->Nt().Compare(KNoHeader) != 0 
          || aMessage->Callback().Compare(KNoHeader) != 0 )
          && aMessage->Sid().Compare(KNoHeader) != 0 )
    {
        return EBadRequest;              
    }
        
    TInt index = Find( aMessage->Sid() );
    TUpnpErrorCode error;
    
    if ( index != KErrNotFound )
    {
        // already a subscriber -> renew
        CUpnpSubscriberLibraryElement* element = iElementArray[index];
        *aElement = element;
        error = element->RenewSubscriberL( aMessage );
        
        return error;
    }
    else
    {
        // not found -> new subscriber
        CUpnpSubscriberLibraryElement* element = 
                                CUpnpSubscriberLibraryElement::NewL( *this );
        CleanupStack::PushL( element );
        *aElement = element;
        error = element->AddSubscriberL( aMessage );
        
        if ( error == EHttpOk )
        {
                User::LeaveIfError(iElementArray.Append( *aElement ));
                CleanupStack::Pop( element );
        }
        else
        {
            CleanupStack::PopAndDestroy( element );
            *aElement = NULL;
        }
        
        return error;
    }
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibrary::Find
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpSubscriberLibrary::Find( const TDesC8& aSid )
{
    if ( aSid.Length() > 0 )
    {
        for ( TInt i = 0; i < iElementArray.Count(); i++ )
        {
            if ( aSid.Compare( iElementArray[i]->Sid() ) == 0 )
            {
                return i;
            }
        }
    }
    return KErrNotFound;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibrary::Remove
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpSubscriberLibrary::Remove( const TDesC8& aSid )
{
    LOGS("CUpnpSubscriberLibrary::Remove");
 
    
    TInt result = Find( aSid );
    
    if ( result != KErrNotFound )
    {
        CUpnpSubscriberLibraryElement* elem = iElementArray[result];
        
        iElementArray.Remove( result );
        iElementArray.GranularCompress();
        
        iObserver.SubscriberRemoved( elem , result );

        delete elem;
        LOGS1("  removed, count: %i", iElementArray.Count());
        
        return EHttpOk;
    }
    return EPreconditionFailed;
}

// -----------------------------------------------------------------------------
// CUpnpSubscriberLibrary::TimeoutExpiredL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpSubscriberLibrary::TimeoutExpiredL( CUpnpTimeoutElement* aElement )
{
    
    if ( !aElement )
    {
        return;
    }
    CUpnpSubscriberLibraryElement* elem 
        = static_cast<CUpnpSubscriberLibraryElement*>(aElement);
    
    LOGS("CUpnpSubscriberLibrary::TimeoutExpiredL");
    
    switch ( elem->Renew() )
    {
        case CUpnpTimeoutElement::ERenew:
        {
            LOGS("  is \"Infinite\", renewed");
            
            elem->SetTimeout( KMaxRenewTimeout );
        }
            break;
            
        case CUpnpTimeoutElement::EOnce:
        {       
            for ( TInt i = 0; i < iElementArray.Count(); i++ )
            {
                if ( elem == iElementArray[i] )
                {
                    iElementArray.Remove(i);
                    iElementArray.GranularCompress();
                    
                    iObserver.SubscriberRemoved( elem, i );
                    
                    LOGS1("  expired, count: %i", iElementArray.Count());
                    delete elem;
                }
            }
        }
            break;
            
        default:
            break;
    }
}

//  End of File
