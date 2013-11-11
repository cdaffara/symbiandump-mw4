/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the CWidgetUiDialogsProviderProxy class.
*
*/


// INCLUDE Files

// User includes
#include "WidgetUiDialogsProviderProxy.h"
#include "WidgetUiWindowManager.h"
#include "WidgetUiWindow.h"
#include "WidgetUiWindowView.h"

// System includes
#include <BrowserDialogsProviderObserver.h>
#include <BrowserDialogsProvider.h>

// CONSTANTS

//-----------------------------------------------------------------------------
// CWidgetUiDialogsProviderProxy::CWidgetUiDialogsProviderProxy
//-----------------------------------------------------------------------------
CWidgetUiDialogsProviderProxy::CWidgetUiDialogsProviderProxy(
						    CBrowserDialogsProvider& aDialogsProvider,
						    MBrowserDialogsProviderObserver* aDialogsObserver,
						    CWidgetUiWindow& aWidgetWindow )
    : iDialogsProvider( aDialogsProvider ),
      iDialogsObserver( aDialogsObserver ),
      iWidgetWindow( aWidgetWindow ),
      iCancelWaitingDialogs( EFalse )
	{
	}


//-----------------------------------------------------------------------------
// CWidgetUiDialogsProviderProxy::~CWidgetUiDialogsProviderProxy
//-----------------------------------------------------------------------------
CWidgetUiDialogsProviderProxy::~CWidgetUiDialogsProviderProxy()
	{
	}


//-----------------------------------------------------------------------------
//	CWidgetUiDialogsProviderProxy* CWidgetUiDialogsProviderProxy::NewL
//-----------------------------------------------------------------------------
CWidgetUiDialogsProviderProxy* CWidgetUiDialogsProviderProxy::NewL(
						    CBrowserDialogsProvider& aDialogsProvider,
						    MBrowserDialogsProviderObserver* aDialogsObserver,
						    CWidgetUiWindow& aWidgetWindow  )
	{
	CWidgetUiDialogsProviderProxy* self = new (ELeave)
	        CWidgetUiDialogsProviderProxy(   aDialogsProvider,
	                                        aDialogsObserver,
	                                        aWidgetWindow );

	CleanupStack::PushL( self );
	self->ConstructL( );
	CleanupStack::Pop( self );   // self
	return self;
	}

//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::ConstructL
//
//-----------------------------------------------------------------------------
void CWidgetUiDialogsProviderProxy::ConstructL()
	{
	}


//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::DialogNotifyErrorL
//
//-----------------------------------------------------------------------------
void CWidgetUiDialogsProviderProxy::DialogNotifyErrorL( TInt aErrCode )
	{
	HandleDialogRequestL();

	// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
    	{
    	iDialogsProvider.DialogNotifyErrorL( aErrCode );
    	if ( iDialogsObserver )
    	    {
    	    iDialogsObserver->ReportDialogEventL(
                                MBrowserDialogsProviderObserver::ENotifyError,
        	                            aErrCode );
    	    }
        }
	}


//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::DialogNotifyHttpErrorL
//-----------------------------------------------------------------------------
void CWidgetUiDialogsProviderProxy::DialogNotifyHttpErrorL(
								            TInt aErrCode, const TDesC& aUri )
	{
	HandleDialogRequestL();

	// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
    	{
    	iDialogsProvider.DialogNotifyHttpErrorL( aErrCode, aUri );
    	if ( iDialogsObserver )
    	    {
    	    iDialogsObserver->ReportDialogEventL(
    	                    MBrowserDialogsProviderObserver::ENotifyHttpError,
                            NULL );
    	    }
    	}
	}


//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::DialogFileSelectLC
//-----------------------------------------------------------------------------
TBool CWidgetUiDialogsProviderProxy::DialogFileSelectLC(
                                                    const TDesC& aStartPath,
													const TDesC& aRootPath,
													HBufC*& aSelectedFileName )
	{
	TBool retVal( EFalse );

	HandleDialogRequestL();

	// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
    	{
    	retVal = iDialogsProvider.DialogFileSelectLC( aStartPath,
    	                                                    aRootPath,
    	                                                    aSelectedFileName );
    	}

	return retVal;
	}

//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::DialogSelectOptionL
//-----------------------------------------------------------------------------
TBool CWidgetUiDialogsProviderProxy::DialogSelectOptionL(
								const TDesC& aTitle,
								TBrCtlSelectOptionType aBrCtlSelectOptionType,
								CArrayFix<TBrCtlSelectOptionData>& aOptions )
	{
	TBool retVal( EFalse );

	HandleDialogRequestL();

	// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
	    {
	    retVal = iDialogsProvider.DialogSelectOptionL(
	                                                aTitle,
	                                                aBrCtlSelectOptionType,
	                                                aOptions );
	    }

	return retVal;
	}


//-----------------------------------------------------------------------------
// CWidgetUiDialogsProviderProxy::DialogUserAuthenticationLC
//-----------------------------------------------------------------------------
TBool CWidgetUiDialogsProviderProxy::DialogUserAuthenticationLC(
												const TDesC& aUrl,
												const TDesC& aRealm,
												const TDesC& aDefaultUserName,
												HBufC*& aReturnedUserName,
												HBufC*& aReturnedPasswd,
												TBool aBasicAuthentication )
	{
	TBool retVal( EFalse );

	HandleDialogRequestL();

		// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
	    {
	    retVal = iDialogsProvider.DialogUserAuthenticationLC(
	                                                aUrl,
	                                                aRealm,
	                                                aDefaultUserName,
	                                                aReturnedUserName,
	                                                aReturnedPasswd,
	                                                aBasicAuthentication );
	    if ( iDialogsObserver )
	        {
            iDialogsObserver->ReportDialogEventL(
                        MBrowserDialogsProviderObserver::EUserAuthentication,
                        ( TInt ) retVal );
	        }
	    }

	return retVal;
	}


//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::DialogNoteL
//-----------------------------------------------------------------------------
void CWidgetUiDialogsProviderProxy::DialogNoteL( const TDesC& aMessage )
	{
	HandleDialogRequestL();

    // Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
	    {
	    iDialogsProvider.DialogNoteL( aMessage );
	    }
	}


//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::DialogAlertL
//-----------------------------------------------------------------------------
void CWidgetUiDialogsProviderProxy::DialogAlertL( const TDesC& aTitle,
											        const TDesC& aMessage )
	{
	HandleDialogRequestL();

	// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
	    {
	    iDialogsProvider.DialogAlertL( aTitle, aMessage );
	    }
	}


//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::DialogConfirmL
//-----------------------------------------------------------------------------
TBool CWidgetUiDialogsProviderProxy::DialogConfirmL( const TDesC& aTitle,
												    const TDesC& aMessage,
												    const TDesC& aYesMessage,
												    const TDesC& aNoMessage )
	{
	TBool retVal( EFalse );

	HandleDialogRequestL();

		// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
	    {
	    retVal = iDialogsProvider.DialogConfirmL( aTitle, aMessage,
	                                                aYesMessage, aNoMessage );
	    }

    return retVal;
	}

//-----------------------------------------------------------------------------
// CWidgetUiDialogsProviderProxy::DialogPromptLC
//-----------------------------------------------------------------------------
TBool CWidgetUiDialogsProviderProxy::DialogPromptLC( const TDesC& aTitle,
												const TDesC& aMessage,
												const TDesC& aDefaultInput,
												HBufC*& aReturnedInput )
	{
	TBool retVal( EFalse );
	HandleDialogRequestL();

	// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
	    {
	    retVal = iDialogsProvider.DialogPromptLC( aTitle, aMessage,
                                            aDefaultInput, aReturnedInput );
	    }

    return retVal;
	}

//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::DialogDownloadObjectL
//-----------------------------------------------------------------------------
TBool CWidgetUiDialogsProviderProxy::DialogDownloadObjectL(
										CBrCtlObjectInfo* aBrCtlObjectInfo )
	{
	HandleDialogRequestL();
	TBool retVal = iDialogsProvider.DialogDownloadObjectL( aBrCtlObjectInfo );
	return retVal;
	}

//-----------------------------------------------------------------------------
// CWidgetUiDialogsProviderProxy::DialogDisplayPageImagesL
//-----------------------------------------------------------------------------
void CWidgetUiDialogsProviderProxy::DialogDisplayPageImagesL(
							CArrayFixFlat<TBrCtlImageCarrier>& aPageImages )
	{
    HandleDialogRequestL();

	// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
	    {
	    iDialogsProvider.DialogDisplayPageImagesL( aPageImages );
	    }
	}


//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::CancelAll
//-----------------------------------------------------------------------------
//
void CWidgetUiDialogsProviderProxy::CancelAll()
	{
	// Dialogs should be flushed, but not displayed
    iCancelWaitingDialogs = ETrue;
    iDialogsProvider.CancelAll();
	}

//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::DialogMimeFileSelectLC
//-----------------------------------------------------------------------------
TBool CWidgetUiDialogsProviderProxy::DialogMimeFileSelectLC(
													HBufC*& aSelectedFileName,
													const TDesC& aMimeType )
	{
	TBool retVal( EFalse );

	HandleDialogRequestL();

	// Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
    	{
    	retVal = iDialogsProvider.DialogMimeFileSelectLC( aSelectedFileName,
    	                                                  aMimeType );
    	}

	return retVal;
	}

//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::ShowTooltipL
//-----------------------------------------------------------------------------
//
void CWidgetUiDialogsProviderProxy::ShowTooltipL(    const TDesC& aText,
                                                    TInt aDuration,
                                                    TInt aDelay )
    {
    HandleDialogRequestL();
    if ( !iCancelWaitingDialogs )
        {
        iDialogsProvider.ShowTooltipL(   aText, aDuration, aDelay );
        }
    }

//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::UploadProgressNoteL
//-----------------------------------------------------------------------------
//
void CWidgetUiDialogsProviderProxy::UploadProgressNoteL(
                                TInt32 aTotalSize,
                                TInt32 aChunkSize,
                                TBool aIsLastChunk,
                                MBrowserDialogsProviderObserver* aObserver )
    {
    HandleDialogRequestL();

    // Forward dialog to DialogsProvider if it has not been cancelled
	if ( !iCancelWaitingDialogs )
	    {
	    iDialogsProvider.UploadProgressNoteL(   aTotalSize,
	                                            aChunkSize,
	                                            aIsLastChunk,
	                                            aObserver );
	    }

    }

//-----------------------------------------------------------------------------
//  CWidgetUiDialogsProviderProxy::HandleDialogRequestL()
//-----------------------------------------------------------------------------
//
void CWidgetUiDialogsProviderProxy::HandleDialogRequestL()
    {
    // Dialogs for background windows must be blocked
	// Also, if options menu is loaded,  then alert dialogs must be blocked
	iCancelWaitingDialogs = ( iWidgetWindow.WidgetMiniViewState() == EPublishStart
    						||(iWidgetWindow.WindowManager().View())->IsOptionsMenuActivated());
    if ( !iCancelWaitingDialogs)
        iCancelWaitingDialogs = ((iWidgetWindow.WindowManager().ActiveWindow())?
                    (iWidgetWindow.WindowManager().ActiveWindow() != &iWidgetWindow):
                    (ETrue));
    }


//  End of File
