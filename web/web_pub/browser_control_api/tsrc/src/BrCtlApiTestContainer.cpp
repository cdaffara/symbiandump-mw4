/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CBrCtlApiTestContainer from BrCtlBCTestContainer.h
*
*/

// INCLUDE FILES
#include "BrCtlApiTestContainer.h"

#include <eiklabel.h>  // for example label control
#include <aknview.h>
#include <AknGlobalConfirmationQuery.h>
#include <BrCtlInterface.h>
#include <CHARCONV.H>
#include "BrCtlApiTestObserver.h"
#include <e32std.h>

_LIT8( KContentDataType, "text/html" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBrCtlApiTestContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CBrCtlApiTestContainer::ConstructL(const TRect& aRect)
    {
    iBrCtlApiTestObserver = CBrCtlApiTestObserver::NewL();
    iBrCtlApiTestStateChangeObserver = CBrCtlApiTestStateChangeObserver::NewL( this );
    iBrCtlDialogs = CBrowserDialogsProviderProxy::NewL();
    iCommandBase = TBrCtlDefs::ECommandIdBase;
    CreateWindowL();
    SetRect(aRect);
    ActivateL();
    }

// Destructor
CBrCtlApiTestContainer::~CBrCtlApiTestContainer()
    {

    if (iBrCtlInterface)
        {
        iBrCtlInterface->RemoveLoadEventObserver(iBrCtlApiTestObserver);
        iBrCtlInterface->RemoveStateChangeObserver(iBrCtlApiTestStateChangeObserver);
        delete iBrCtlInterface;
        }

    delete iBrCtlApiTestStateChangeObserver;
    delete iBrCtlApiTestObserver;
    delete iBrCtlDialogs;
    }

void CBrCtlApiTestContainer::CancelLoad()
    {
    iCancelInitDataLoad = ETrue;
    }
    
TInt CBrCtlApiTestContainer::LoadPageL()
    {
    if(iBrCtlInterface)
        {
        iBrCtlInterface->LoadUrlL(KUrlLoadTestHtml2);
        }

     return KErrNone;
     }

// ---------------------------------------------------------
// CBrCtlApiTestContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CBrCtlApiTestContainer::SizeChanged()
    {
    if (iBrCtlInterface)
        {
        iBrCtlInterface->SetRect(Rect());
        }
    }

// ---------------------------------------------------------
// CBrCtlApiTestContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CBrCtlApiTestContainer::CountComponentControls() const
    {
    if (iBrCtlInterface)
        return 1;
    return 0;
    }

// ---------------------------------------------------------
// CBrCtlApiTestContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CBrCtlApiTestContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iBrCtlInterface; // Could be NULL
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CBrCtlApiTestContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CBrCtlApiTestContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    // TODO: Add your drawing code here
    // example code...
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    
    }

// ---------------------------------------------------------
// CBrCtlApiTestContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CBrCtlApiTestContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    // TODO: Add your control event handler code here
    }



// ----------------------------------------------------
// CBrCtlApiTestContainer::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ?implementation_description
// ----------------------------------------------------
//
TKeyResponse CBrCtlApiTestContainer::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CBrCtlApiTestContainer::HandleCommandL(TInt aCommand)
// ?implementation_description
// ----------------------------------------------------
//
TInt CBrCtlApiTestContainer::HandleCommandL( TInt aCommand )
    {
    const TInt KCommandIdBase( 15000 );
    iBrCtlInterface->HandleCommandL( aCommand + KCommandIdBase );
    return KErrNone;
    }

TKeyResponse CBrCtlApiTestContainer::OfferKeyEventL(const TKeyEvent& /* aKeyEvent */,
                        TEventCode /* aType */ )
    {
    return EKeyWasConsumed;
    }

void CBrCtlApiTestContainer::HandleCommandL( 
    TBrCtlDefs::TBrCtlClientCommands /* aCommand */, 
    const CArrayFix<TPtrC>& /* aAttributesNames */,
    const CArrayFix<TPtrC>& /* aAttributeValues */ )
    {
    }


void CBrCtlApiTestContainer::CreateBrowserControlInterfaceL(TInt aBrCtlConfig)
    {
    TPoint base( 0, 0 );
    TPoint extent( 352, 295 );
    TRect rect( base, extent );
    
    iBrCtlInterface = CreateBrowserControlL( this, rect, 
        aBrCtlConfig,
        iCommandBase, iBrCtlApiTestObserver, iBrCtlApiTestObserver,
        iBrCtlApiTestObserver, iBrCtlApiTestObserver, iBrCtlDialogs, NULL, iBrCtlApiTestObserver );

    iBrCtlInterface->MakeVisible( ETrue );
    
    }

HBufC8* CBrCtlApiTestContainer::ReadFileLC( const TDesC& aFileName )
    {
    RFs rfs;
    RFile file;
    User::LeaveIfError( rfs.Connect() );
    CleanupClosePushL( rfs );
    User::LeaveIfError( file.Open( rfs, aFileName, EFileRead ) );
    CleanupClosePushL( file );
    TInt size;
    User::LeaveIfError( file.Size( size ) );
    HBufC8* buf = HBufC8::NewLC( size );
    TPtr8 bufPtr( buf->Des() );
    User::LeaveIfError( file.Read( bufPtr ) );
    CleanupStack::Pop(); // buf
    CleanupStack::PopAndDestroy( 2 ); // file, rfs
    CleanupStack::PushL( buf );
    return buf;
    }

TInt CBrCtlApiTestContainer::LoadDataL()
    {
    HBufC8* data = ReadFileLC( KUrlLoadTestHtml );
    TDataType dataType( KContentDataType() );
    TUid uid;
    uid.iUid = KCharacterSetIdentifierIso88591;
    TRAPD( err, iBrCtlInterface->LoadDataL( KUrlLoadTestHtml, *data, dataType, uid ) );

    CleanupStack::PopAndDestroy( data );

    return err;
    }

TInt CBrCtlApiTestContainer::ClearCacheTest()
    {
    const TUint result = iBrCtlInterface->ClearCache();
    
    return KErrNone;
    }

TBool CBrCtlApiTestContainer::IsUrlInCacheTest( TDesC16& aUrl )
    {
    const TBool result = iBrCtlInterface->IsUrlInCache( aUrl ); // ( TDesC16 & )KUrlLoadTestHtml() );

    return result;
    }

TInt CBrCtlApiTestContainer::ClearItemInCacheTest()
    {
    TInt result = iBrCtlInterface->ClearItemInCache( (TDesC16 &) KUrlLoadTestHtml() );

    if ( ( result == KErrNotFound ) || ( result == KErrNone ) )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }

TInt CBrCtlApiTestContainer::PageInfoLC( TBrCtlDefs::TBrCtlPageInfo aType )
    {
    iBrCtlInterface->LoadFileL( KUrlLoadTestHtml );

    TRAPD( err, 
        HBufC* url = iBrCtlInterface->PageInfoLC( aType ); 
        CleanupStack::PopAndDestroy( url ); 
        )

    return err;
    }

TInt CBrCtlApiTestContainer::CertInfo()
    {
    const TCertInfo* certInfo = iBrCtlInterface->CertInfo();

    if ( !certInfo )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }

TInt CBrCtlApiTestContainer::NavigationAvailableTest()
    {
    const TBool nav = iBrCtlInterface->NavigationAvailable( 
        TBrCtlDefs::ENavigationBack );

    if ( nav == 0 || nav == 1 )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }

TInt CBrCtlApiTestContainer::GetBitmapData()
    {
    
    TRAPD( err, iBrCtlInterface->LoadUrlL(KUrlLoadTestHtml2) );
    
    //waits for browser control to load the page
    User::After(4000000);

    CGulIcon* icon = iBrCtlInterface->GetBitmapData( KUrlLoadTestHtml, TBrCtlDefs::EBitmapFavicon );
    icon = iBrCtlInterface->GetBitmapData( KUrlLoadTestHtml, TBrCtlDefs::EBitmapThumbnail );	
    return err;
    }

TInt CBrCtlApiTestContainer::SaveToFileL()
    {
    HBufC8* data = ReadFileLC( KUrlLoadTestHtml );
    TDataType dataType( KContentDataType() );
    TUid uid;
    uid.iUid = KCharacterSetIdentifierIso88591;
    TRAPD( err, iBrCtlInterface->LoadDataL( KUrlLoadTestHtml, *data, dataType, uid ) );

    TUint16 NoOfBytesSaved =  iBrCtlInterface->SaveToFileL( KUrlLoadTestHtml );

    CleanupStack::PopAndDestroy( data ); 
    return err;
    }

TInt CBrCtlApiTestContainer::OkToExit()
    {
    const TBool res = iBrCtlInterface->OkToExit();

    if ( res )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }
    
TInt CBrCtlApiTestContainer::FindKeyword( TPtrC aHtml, TPtrC aKeyword, TInt aMatch)
    {
    TRAP_IGNORE( iBrCtlInterface->LoadFileL( KUrlLoadTestHtml2 ) );

    TInt result = iBrCtlInterface->FindKeyword( aKeyword );

    if ( result == TBrCtlDefs::EFindNoMatches && aMatch == EFindNoMatchesExpected )
        {
        return KErrNone;
        }
    else if ( result == TBrCtlDefs::EFindMatch && ( aMatch == EFindMatchExpected || aMatch == EFindMoreMatchesExpected ) )
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

TInt CBrCtlApiTestContainer::FindKeywordAgain( TInt aMatch )
    {
    TBrCtlDefs::TBrCtlFindResponse result = iBrCtlInterface->FindKeywordAgain( ETrue );

    if ( result == TBrCtlDefs::EFindNoMatches && ( aMatch == EFindNoMatchesExpected || aMatch == EFindMatchExpected ) )
        {
        return KErrNone;
        }
    else if ( result == TBrCtlDefs::EFindMatch && aMatch == EFindMoreMatchesExpected )
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

TInt CBrCtlApiTestContainer::SetFocus()
    {
    iBrCtlInterface->SetFocus( ETrue, EDrawNow );
    
    return KErrNone;
    }

TInt CBrCtlApiTestContainer::OfferKeyEvent()
    {
    TKeyEvent key;
    key.iCode = 0;
    key.iScanCode = 0;
    key.iModifiers = 0;
    key.iRepeats = 0;
    
    TRAPD( err, iBrCtlInterface->OfferKeyEventL( key, EEventNull ) );
    
    return err;
    }

TInt CBrCtlApiTestContainer::BrCtlMinimumSize()
    {
    const TSize testSize = iBrCtlInterface->MinimumSize();
    
    if ( testSize.iWidth > 0 && testSize.iHeight > 0 )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }

TInt CBrCtlApiTestContainer::BrCtlSubscribeToMenuItemsL()
    {
    TRAPD( err, iBrCtlInterface->LoadUrlL( KUrlLoadTestHtml2 ) );

	const RPointerArray<TBrCtlSubscribeTo> &ptr = iBrCtlInterface->SubscribeToMenuItemsL();

    return err;
    }    
    
TInt CBrCtlApiTestContainer::BrCtlContentSize()
    {
    TBool testPassed = EFalse;
    TSize testSize;
    
    if ( iBrCtlInterface )
        {
        testSize = iBrCtlInterface->ContentSize();

        if ( testSize.iWidth == 0 && testSize.iHeight == 0 )
            {
            testPassed = ETrue;
            }
        }
    
    return testPassed;
    }
    
TInt CBrCtlApiTestContainer::SetBrowserSettingL()
    {
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsAutoLoadImages, 0 );
        
    return KErrNone;
    }

TInt CBrCtlApiTestContainer::BrowserSettingL( TUint& aSetting )
    {
    // value returned
    aSetting = iBrCtlInterface->BrowserSettingL( TBrCtlDefs::ESettingsAutoLoadImages );
    
    return KErrNone;
    }

TInt CBrCtlApiTestContainer::FocusedImageL()
    {
    iBrCtlInterface->LoadFileL( KUrlLoadTestHtml );

    iBrCtlInterface->SetFocus( ETrue, EDrawNow );

    TBrCtlImageCarrier* res = iBrCtlInterface->FocusedImageLC();
    CleanupStack::PopAndDestroy( res );

    if ( res )
        {
        return KErrNone; 
        }
    else 
        {
        return KErrGeneral;
        }        
    }

TInt CBrCtlApiTestContainer::AddCommandObserverL()
    {
    iBrCtlInterface->AddCommandObserverL( this );
    return KErrNone;
    }

TInt CBrCtlApiTestContainer::RemoveCommandObserver()
    {
    iBrCtlInterface->RemoveCommandObserver( this );
    return KErrNone;
    }

TInt CBrCtlApiTestContainer::ImageCountL()
    {
    return iBrCtlInterface->ImageCountL();
    }

TInt CBrCtlApiTestContainer::BrCtlFocusedElementTypeL()
    {
    
    iBrCtlInterface->LoadUrlL( KUrlLoadTestHtml2 );
    
    iBrCtlInterface->SetFocus( ETrue, EDrawNow );

    iBrCtlInterface->FocusedElementType();

    return KErrNone;
    }
    
TInt CBrCtlApiTestContainer::BrCtlSetParam()
    {
    _LIT( KContentType, "text/html");
    TRAPD( err, iBrCtlInterface->SetParamL( TBrCtlDefs::EParamsSelfDownoadableTypes, KContentType ) )

    return err;
    }

TInt CBrCtlApiTestContainer::LoadSavedPageL()
    {
    _LIT( KUid, "savedpage://4" );
    _LIT( KSavedUrl, "c:\\BrCtlTest\\loadtest.saved" );

    HBufC8* databuf = ReadFileLC( KSavedUrl );
    HBufC16* bigbuf = HBufC::NewL( databuf->Des().Length() );

    bigbuf->Des().Copy( databuf->Des() );
    
    iBrCtlInterface->LoadSavedPageL( KUid, *bigbuf );

    delete bigbuf;
    CleanupStack::PopAndDestroy( databuf ); 

    return KErrNone;    
    }

TInt CBrCtlApiTestContainer::SetSelfDownloadContentTypes()
    {
    TRAPD( err, iBrCtlInterface->SetSelfDownloadContentTypesL( (TDesC16&)KContentDataType() ) );

    return err;
    }

TInt CBrCtlApiTestContainer::ZoomLevels()
    {
    RArray<TUint>* zoom = iBrCtlInterface->ZoomLevels();

    if ( zoom->Count() > 0 )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }

TInt CBrCtlApiTestContainer::AddOptionMenuItems()
    {
    TRAPD( err, iBrCtlInterface->AddOptionMenuItemsL( *iMenuPane, 0, 0 ) );

    return err;
    }

TInt CBrCtlApiTestContainer::BrCtlInitLoadDataL()
    {
    HBufC8* data = ReadFileLC( KUrlLoadTestHtml );
    TDataType dataType( KContentDataType() );
    MBrCtlDataLoadConsumer* brCtlDataLoadConsumer = NULL;
    TUid uid;
    uid.iUid = KCharacterSetIdentifierIso88591;
    TRAPD( err, iBrCtlInterface->InitLoadDataL( KUrlLoadTestHtml, dataType, uid, data->Length(),
        this, &brCtlDataLoadConsumer ) );

    if ( brCtlDataLoadConsumer )
        {
        brCtlDataLoadConsumer->HandleNextDataChunk( *data );
        if ( !iCancelInitDataLoad )
            {
            brCtlDataLoadConsumer->HandleLoadComplete();
            }
        }
        
    iCancelInitDataLoad = EFalse;

    CleanupStack::PopAndDestroy( data );
    
    return err;
    }

TInt CBrCtlApiTestContainer::HandleDownloadCommand()
    {
    TBool testPassed = EFalse;

    if ( iBrCtlInterface )
        {
        TRAPD( err, iBrCtlInterface->HandleDownloadCommandL( 0, TBrCtlDefs::EDownloadCmdMarkAsProgressive ) );

            if ( err == KErrNone )
            {
            testPassed = ETrue;
            }
        }

    if ( testPassed )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }   

TInt CBrCtlApiTestContainer::VersionInfoL()
  {

  _LIT(KName, "S60 browser");
  _LIT(KVersion, "5.0");
  _LIT(KBuild, "0516");

  HBufC* verinfo = iBrCtlInterface->VersionInfoLC( TBrCtlDefs::EVersionInfoName );

  if(!verinfo)
    {
    return KErrGeneral;
    }
  if (verinfo->Compare(KName()) != 0)
    {
    CleanupStack::PopAndDestroy( verinfo ); 
    return KErrGeneral;
    }
  CleanupStack::PopAndDestroy( verinfo ); 
  verinfo = 0;
  verinfo = iBrCtlInterface->VersionInfoLC( TBrCtlDefs::EVersionInfoVersion );	
  if(!verinfo)
    {
  	return KErrGeneral;
    }
  if(verinfo->Compare(KVersion()) != 0)
    {
    CleanupStack::PopAndDestroy( verinfo ); 
    return KErrGeneral;
    }
  CleanupStack::PopAndDestroy( verinfo ); 
  verinfo = 0;
  verinfo = iBrCtlInterface->VersionInfoLC( TBrCtlDefs::EVersionInfoBuild );
  if(!verinfo)
    {
   	return KErrGeneral;
    }
  if(verinfo->Compare(KBuild()) != 0)
    {
    CleanupStack::PopAndDestroy( verinfo ); 
    return KErrGeneral;
    }
  CleanupStack::PopAndDestroy( verinfo ); 
  return KErrNone;
  }
TInt CBrCtlApiTestContainer::WmlOptionMenuItemsL()
    {

    iBrCtlInterface->WMLOptionMenuItemsL();
    return KErrNone;

    }

TInt CBrCtlApiTestContainer::BrCtlPostL()
    {
    
    RFs rfs;
    RFile file;
    User::LeaveIfError( rfs.Connect() );
    CleanupClosePushL( rfs );
    User::LeaveIfError( file.Open( rfs, KUrlLoadTestHtml(), EFileShareReadersOnly ) );
    CleanupClosePushL( file );    
    
    int size;
    User::LeaveIfError( file.Size( size ) );
    HBufC8* data = NULL;
    data = HBufC8::NewLC( size );
    TPtr8 dataPtr( data->Des() );
    User::LeaveIfError( file.Read( dataPtr ) );

    //parse url
    HBufC* url16 = NULL;
    int pos = dataPtr.Find( _L8("\r\n") );                   
    if ( pos != KErrNotFound )
        {
        url16 = HBufC::NewLC( pos );                
        url16->Des().Copy( dataPtr.Mid( 0, pos ) ); 
        }
    else 
        {
        CleanupStack::PushL( url16 );        
        }
    
    //parse content type
    HBufC8* contenttype = NULL;
    int pos2 = dataPtr.Mid( pos+2 ).Find( _L8("\r\n") );                   
    if ( pos2 != KErrNotFound )
        {
        contenttype = HBufC8::NewLC( pos2 );                
        contenttype->Des().Copy( dataPtr.Mid( pos+2, pos2 ) );             
        }
    else 
        {
        CleanupStack::PushL( contenttype );        
        }

    //parse post data
    HBufC8* postdata = NULL;
    int pos3 = dataPtr.Mid( pos+2+pos2+2 ).Find( _L8("\r\n") );                   
    if ( pos3 != KErrNotFound )
        {
        postdata = HBufC8::NewLC( pos3 );                
        postdata->Des().Copy( dataPtr.Mid( pos+2+pos2+2, pos3 ) );             
        }
    else 
        {
        CleanupStack::PushL( postdata );        
        }

    //parse boundry
    HBufC8* boundry = NULL;
    int pos4 = dataPtr.Mid( pos+2+pos2+2+pos3+2 ).Find( _L8("\r\n") );                   
    if ( pos4 != KErrNotFound )
        {
        boundry = HBufC8::NewLC( pos4 );                
        boundry->Des().Copy( dataPtr.Mid(pos+2+pos2+2+pos3+2 , pos4) );             
        }
    else 
        {
        CleanupStack::PushL( boundry );        
        }
    
    TAny* any = NULL;
    
    TRAPD( error, iBrCtlInterface->PostUrlL ( *url16, 
                                *contenttype, 
                                *postdata, 
                                boundry, 
                                any ) );
	
/*	HBufC* urlTest = NULL;

    TRAP( error, iBrCtlInterface->PostUrlL ( *urlTest, 
                                *contenttype, 
                                *postdata, 
                                boundry, 
                                any ) );
                                
     HBufC8* contenttypeTest = NULL;                          
                                
     TRAP( error, iBrCtlInterface->PostUrlL ( *url16, 
                                *contenttypeTest, 
                                *postdata, 
                                boundry, 
                                any ) );                           
*/

    CleanupStack::PopAndDestroy( boundry );
    CleanupStack::PopAndDestroy( postdata );
    CleanupStack::PopAndDestroy( contenttype );
    CleanupStack::PopAndDestroy( url16 );    
    CleanupStack::PopAndDestroy( data );                        
    CleanupStack::PopAndDestroy( 2 ); // file, rfs        

    return error;
    }
     
// Added for Widget Extension Test
TInt CBrCtlApiTestContainer::CreateBrCtlInterfaceAndWidgetExtensionL()
    {
    TPoint base( 0, 0 );
    TPoint extent( 352, 295 );
    TRect rect( base, extent );
    
	iBrCtlInterface = CreateBrowserControlL(
        this,
        rect,
        TBrCtlDefs::ECapabilityLoadHttpFw |
        TBrCtlDefs::ECapabilityCursorNavigation|
        TBrCtlDefs::ECapabilityWebKitLite |
        TBrCtlDefs::ECapabilityClientResolveEmbeddedURL,
        TBrCtlDefs::ECommandIdBase,
        iBrCtlApiTestObserver, /* softkeys */
        iBrCtlApiTestObserver, /* link resolver */
        iBrCtlApiTestObserver, /* special load observer */
        iBrCtlApiTestObserver, /* layout observer */
        iBrCtlDialogs,  //iWidgetUiDialogsProviderProxy,
        iBrCtlApiTestObserver, /* window observer */
        iBrCtlApiTestObserver /* download observer */
        ); 
        
    iBrCtlInterface->AddLoadEventObserverL( iBrCtlApiTestObserver );
    iBrCtlInterface->AddStateChangeObserverL( iBrCtlApiTestStateChangeObserver );
    iBrCtlInterface->SetComponentsToInheritVisibility();
    
    iWidgetExtension = iBrCtlInterface->CreateWidgetExtensionL( *this );
    
    if ( iWidgetExtension )
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }

    }

TInt CBrCtlApiTestContainer::HistoryLoad1L()
    {
    TInt err( KErrNone );
    if(iBrCtlInterface)
        {
        TRAP(err, iBrCtlInterface->LoadUrlL(KUrlHistoryHandlerHtml1));
        }

    return err;
    }

TInt CBrCtlApiTestContainer::HistoryLoad2L()
    {
    TInt err( KErrNone );
    if(iBrCtlInterface)
        {
        TRAP(err, iBrCtlInterface->LoadUrlL(KUrlHistoryHandlerHtml2));
        }

    return err;
    }

TInt CBrCtlApiTestContainer::HistoryLoad3L()
    {
    TInt err( KErrNone );
    if(iBrCtlInterface)
        {
        TRAP(err, iBrCtlInterface->LoadUrlL(KUrlHistoryHandlerHtml3));
        }

    return err;
    }

TInt CBrCtlApiTestContainer::WMLLoad1L()
    {
    TInt err( KErrNone );
    if(iBrCtlInterface)
        {
        TRAP(err, iBrCtlInterface->LoadUrlL(KUrlLoadTestWml1));
        }

    return err;
    }

TInt CBrCtlApiTestContainer::WMLLoad2L()
    {
    TInt err( KErrNone );
    if(iBrCtlInterface)
        {
        TRAP(err, iBrCtlInterface->LoadUrlL(KUrlLoadTestWml2));
        }

    return err;
    }

TInt CBrCtlApiTestContainer::WMLLoad3L()
    {
    TInt err( KErrNone );
    if(iBrCtlInterface)
        {
        TRAP(err, iBrCtlInterface->LoadUrlL(KUrlLoadTestWml3));
        }

    return err;
    }

// Dummy function definitions for the MWidgetCallback functions
TBool CBrCtlApiTestContainer::DialogMimeFileSelectLC(HBufC*& /* aSelectedFileName */,
                         const TDesC& /* aMimeType */) 
	{
		
	}

/**
* Called to show or hide softkeys
* @since 3.1
* @param aVisible ETrue to show softkeys, EFalse when full screen is needed
*/
void CBrCtlApiTestContainer::SetSoftkeysVisible(TBool /* aVisible */) 
	{
		
	}

/**
* Called to change the display orientation to landscape
* @since 3.1
*/
void CBrCtlApiTestContainer::SetDisplayMode(TBrCtlDefs::TBrCtlOrientation /* aOrientation */) 
	{
		
	}

//Reserved for future use
TInt CBrCtlApiTestContainer::Reserved_1(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
TInt CBrCtlApiTestContainer::Reserved_2(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
TInt CBrCtlApiTestContainer::Reserved_3(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
TInt CBrCtlApiTestContainer::Reserved_4(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
TInt CBrCtlApiTestContainer::Reserved_5(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
TInt CBrCtlApiTestContainer::Reserved_6(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
TInt CBrCtlApiTestContainer::Reserved_7(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
TInt CBrCtlApiTestContainer::Reserved_8(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
TInt CBrCtlApiTestContainer::Reserved_9(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
TInt CBrCtlApiTestContainer::Reserved_10(TAny*& a0, TAny* a1, TAny* a2) 
	{
		
	}
    
// End of File  
