/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Empty view for UI
*
*/


#include <aknViewAppUi.h>
#include <imageprintui.rsg>

#include "emptyview.h"
#include "emptycontainer.h"
#include "imageprintuidebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//  constructor
// ---------------------------------------------------------------------------
//
CEmptyView::CEmptyView()
    {
    }


// ---------------------------------------------------------------------------
//  ConstructL
// ---------------------------------------------------------------------------
//
void CEmptyView::ConstructL()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::ConstructL()"));
    BaseConstructL( R_EMPTY_VIEW );
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::ConstructL() complete"));
    }


// ---------------------------------------------------------------------------
//  NewL
// ---------------------------------------------------------------------------
//
CEmptyView* CEmptyView::NewL()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::NewL()"));
    CEmptyView* self = CEmptyView::NewLC();
    CleanupStack::Pop( self );
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::NewL() complete"));
    return self;
    }


// ---------------------------------------------------------------------------
//  NewLC
// ---------------------------------------------------------------------------
//
CEmptyView* CEmptyView::NewLC()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::NewLC()"));
    CEmptyView* self = new( ELeave ) CEmptyView;
    CleanupStack::PushL( self );
    self->ConstructL();
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::NewLC() complete"));
    return self;
    }


// ---------------------------------------------------------------------------
//  destructor
// ---------------------------------------------------------------------------
//
CEmptyView::~CEmptyView()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::Destructor"));
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::Destructor complete"));

    }


// ---------------------------------------------------------------------------
// Returns the id of the view
// ---------------------------------------------------------------------------
//
TUid CEmptyView::Id() const
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::Id"));
    return KImagePrintEmptyViewId;
    }


// ---------------------------------------------------------------------------
// Forwards all the commands to appUi
// ---------------------------------------------------------------------------
//
void CEmptyView::HandleCommandL(
    TInt aCommand )
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::HandleCommandL"));
    AppUi()->HandleCommandL( aCommand );
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::HandleCommandL complete"));
    }

// ---------------------------------------------------------------------------
// Creates a container and puts it to stack
// ---------------------------------------------------------------------------
//
void CEmptyView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,
   TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {       
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::DoActivateL"));

	iContainer = CEmptyContainer::NewL( ClientRect() );
	iContainer->SetMopParent( this );
	AppUi()->AddToStackL( *this, iContainer );
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::DoActivateL complete"));
    }


// ---------------------------------------------------------------------------
// Deletes container 
// ---------------------------------------------------------------------------
//
void CEmptyView::DoDeactivate()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::DoDeactivate()"));
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
	    iContainer = NULL;
        }
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyView::DoDeactivate() complete"));

    }

//End of File
