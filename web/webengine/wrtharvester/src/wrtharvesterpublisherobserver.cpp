/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


// INCLUDE FILES

#include "wrtharvesterpublisherobserver.h"
#include "wrtharvester.h"

#include "wrtharvesterconst.h"

#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>

_LIT8( KCPContentInterface, "IContentPublishing" );
_LIT8( KCPService, "Service.ContentPublishing" );

// =============================================================================
// --------------------------------------------------------------------------
// Cleanup utility for RPointerArray
// --------------------------------------------------------------------------
//
static void DoCleanup( TAny* aPtr )
    {
    __ASSERT_DEBUG( aPtr, User::Invariant() );
    static_cast< RCriteriaArray* >( aPtr )->ResetAndDestroy();
    }

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CWrtHarvesterPublisherObserver::CWrtHarvesterPublisherObserver( CWrtHarvester* aHarvester )
    : iHarvester( aHarvester )
	{
	}

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CWrtHarvesterPublisherObserver* CWrtHarvesterPublisherObserver::NewLC( 
        const TDesC& aBundleId, CWrtHarvester* aHarvester )
    {
    CWrtHarvesterPublisherObserver* self( 
        new( ELeave ) CWrtHarvesterPublisherObserver( aHarvester ) );
        
    CleanupStack::PushL( self );
    self->ConstructL( aBundleId );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWrtHarvesterPublisherObserver::~CWrtHarvesterPublisherObserver()
    {
    ReleaseLiw();
    delete iBundleId;
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CWrtHarvesterPublisherObserver::ConstructL( const TDesC& aBundleId )
    {
    iBundleId = aBundleId.AllocL();
    InitLiwL();
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPublisherObserver::HandleNotifyL
//
// Handles Published content notification
// ---------------------------------------------------------------------------
//
TInt CWrtHarvesterPublisherObserver::HandleNotifyL( 
    TInt /*aErrorCode */, 
    TInt /*aEventId*/,
    CLiwGenericParamList& aEventParamList, 
    const CLiwGenericParamList& /*aInParamList*/ )
    {
    TInt pos(0);

    aEventParamList.FindFirst(pos, KChangeInfo);
    if (pos != KErrNotFound)
        {
        // Get list of maps
        TLiwVariant variant = (aEventParamList)[pos].Value();
        variant.PushL();
        const CLiwList* changeMapsList = variant.AsList();
        
        TPtrC publisher;
        TPtrC operation;
        TPtrC8 trigger;

        // Iter through list content
        for (TInt i = 0; i < changeMapsList->Count(); ++i)
            {
             if ( changeMapsList->AtL( i, variant )) 
                 {
                 const CLiwMap* map  = variant.AsMap();
                 
                 // Check what triggered a notification
                 variant.Reset();
                 if ( map->FindL( KOperation, variant ))
                     {
                     operation.Set( variant.AsDes()); 
                     }
                 if ( operation == KExecute )
                     {
                     variant.Reset();
                     if( map->FindL( KActionTrigger, variant ))
                         {
                         trigger.Set( variant.AsData()); 
                         }
                     TLiwVariant pubVariant;
                     if( map->FindL( KContentId, pubVariant) )
                         {
                         publisher.Set( pubVariant.AsDes());
                         }
                     pubVariant.PushL();
                     if( publisher.Length() && trigger.Length())
                         {
                         iHarvester->HandlePublisherNotificationL( publisher, trigger );
                         }
                     CleanupStack::PopAndDestroy( &pubVariant );
                     }
                 }
            }
        CleanupStack::PopAndDestroy( &variant );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPublisherObserver::RegisterL
//
// Register for notifications
// ---------------------------------------------------------------------------
//
void CWrtHarvesterPublisherObserver::RegisterL( CLiwDefaultMap* aFilter )
    {
    CLiwGenericParamList* inParamList( CLiwGenericParamList::NewLC() );
    CLiwGenericParamList* outParamList( CLiwGenericParamList::NewLC() );
    
    // Fill in input list for RequestNotification command
    inParamList->AppendL(TLiwGenericParam(KType,TLiwVariant( KPubData )));
    inParamList->AppendL(TLiwGenericParam(KFilter ,TLiwVariant( aFilter )));
        
    iInterface->ExecuteCmdL( KRequestNotification,  *inParamList,
                						 *outParamList, 0, this );
    
    CleanupStack::PopAndDestroy( outParamList );
    CleanupStack::PopAndDestroy( inParamList ); 
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPublisherObserver::Name
// ---------------------------------------------------------------------------
//
const TDesC& CWrtHarvesterPublisherObserver::BundleId()
    {
    return *iBundleId;
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPublisherObserver::ReleaseL
//
// Sing off to notification
// ---------------------------------------------------------------------------
//
void CWrtHarvesterPublisherObserver::ReleaseL()
    {
    if( iInterface )
        {
        CLiwGenericParamList* inParamList = CLiwGenericParamList::NewL();
        CleanupStack::PushL( inParamList );
        CLiwGenericParamList* outParamList = CLiwGenericParamList::NewL();
        CleanupStack::PushL( outParamList );
        
        iInterface->ExecuteCmdL( KRequestNotification, *inParamList, *outParamList,
                								 KLiwOptCancel, this );

        CleanupStack::PopAndDestroy( outParamList );
        CleanupStack::PopAndDestroy( inParamList );
        }
    }


// ------------------------------------------------------------------------
// CWrtHarvesterPublisherObserver::InitLiwL
//
// Init LIW interface.
// ------------------------------------------------------------------------
void CWrtHarvesterPublisherObserver::InitLiwL()
    {
    if ( !iLiwHandler )
        {
        TInt error( KErrNone );
        CLiwGenericParamList* in( NULL );
        CLiwGenericParamList* out( NULL );
        CLiwCriteriaItem* criterion( NULL );

        RCriteriaArray array( 1 );
        TCleanupItem cleanup( DoCleanup, &array );
        CleanupStack::PushL( cleanup );

        // Do the basic initialization.
        iLiwHandler = CLiwServiceHandler::NewL();

        // Get iCPSIface.
        in = CLiwGenericParamList::NewLC();
        out = CLiwGenericParamList::NewLC();

        criterion = CLiwCriteriaItem::NewL( 
            KLiwCmdAsStr,
            KCPContentInterface, 
            KCPService );
            
        error = array.Append( criterion );
        if ( error )
            {
            delete criterion; 
            criterion = NULL;
            User::Leave( error );
            }

        criterion->SetServiceClass( TUid::Uid( KLiwClassBase ) );
            
        iLiwHandler->AttachL( array );
        iLiwHandler->ExecuteServiceCmdL( *criterion, *in, *out ); 

        out->FindFirst( error, KCPContentInterface );
        User::LeaveIfError( error );

        iInterface = (*out)[ error ].Value().AsInterface();

        CleanupStack::PopAndDestroy( 3 );
        }
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPublisherObserver::ReleaseLiw
// ---------------------------------------------------------------------------
//
void CWrtHarvesterPublisherObserver::ReleaseLiw()
    {    
    if ( iInterface )
        {
        iInterface->Close();
        iInterface = NULL;
        }
        
    delete iLiwHandler;
    iLiwHandler = NULL;
    }
    
 //  End of File

