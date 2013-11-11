/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation of class CRoapAppView.   
*      
*
*/


// INCLUDE FILES

#include <barsread.h>
#include <eiklabel.h>
#include <avkon.rsg>
#include <RoapApp.rsg>
#include "RoapAppView.h"
#include "RoapAppLogger.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapAppView::CRoapAppView()
// ---------------------------------------------------------
//
CRoapAppView* CRoapAppView::NewL
( const TRect& aRect, MObjectProvider* aMopParent )
    {
    CRoapAppView* view = new (ELeave) CRoapAppView( aMopParent );
    CleanupStack::PushL( view );
    view->ConstructL( aRect );
    CleanupStack::Pop( view );
    return view;
    }

// ---------------------------------------------------------
// CRoapAppView::~CRoapAppView()
// ---------------------------------------------------------
//
CRoapAppView::~CRoapAppView()
    {
    delete iLabel;
    delete iBgContext;
    }
    
//----------------------------------------------------------
// CRoapAppView::MopSupplyObject()
//----------------------------------------------------------
//
TTypeUid::Ptr CRoapAppView::MopSupplyObject(TTypeUid aId)
    {
    if (iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject(aId);
    }

// ---------------------------------------------------------
// CRoapAppView::SizeChanged()
// ---------------------------------------------------------
//
void CRoapAppView::SizeChanged()
    {
    iLabel->SetRect( Rect() );
    iLabel->DrawNow();
    if(iBgContext)
        {
        iBgContext->SetRect(Rect());
        if ( &Window() )
            {
            iBgContext->SetParentPos( PositionRelativeToScreen() );
            }
        }
     }

// ---------------------------------------------------------
// CRoapAppView::CountComponentControls()
// ---------------------------------------------------------
//
TInt CRoapAppView::CountComponentControls() const
    {
    return 1;
    }

// ---------------------------------------------------------
// CRoapAppView::ComponentControl()
// ---------------------------------------------------------
//
CCoeControl* CRoapAppView::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iLabel;
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------
// CRoapAppView::OfferKeyEventL()
// ---------------------------------------------------------
//
TKeyResponse CRoapAppView::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    return iLabel->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CRoapAppView::Draw()
// ---------------------------------------------------------
//
void CRoapAppView::Draw( const TRect& aRect ) const
    {
    // Get the standard graphics context 
    CWindowGc& gc = SystemGc();
   
    // Redraw the background using the default skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, aRect );

    gc.Clear( aRect );
    }

// ---------------------------------------------------------
// CRoapAppView::CRoapAppView()
// ---------------------------------------------------------
//
CRoapAppView::CRoapAppView( MObjectProvider* aMopParent )
    {
    SetMopParent( aMopParent );
    }

// ---------------------------------------------------------
// CRoapAppView:ConstructL()
// ---------------------------------------------------------
//
void CRoapAppView::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    iLabel = new (ELeave) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    TResourceReader rr;
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,aRect,ETrue);
    iCoeEnv->CreateResourceReaderLC( rr, R_ROAP_APP_LABEL );
    iLabel->ConstructFromResourceL( rr );
    CleanupStack::PopAndDestroy();  // rr
    iLabel->ActivateL();
    SetRect( aRect );
    ActivateL();
    }
