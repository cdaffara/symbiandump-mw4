/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the CBrowserDialogsProviderProxy class.
*
*/


// INCLUDE Files

// System includes
#include <BrowserDialogsProviderObserver.h>
//#include <BrowserDialogsProvider.h>

// User includes
#include "BrCtlApiTestDialogsProvider.h"


// CONSTANTS

//-----------------------------------------------------------------------------
// CBrowserDialogsProviderProxy::CBrowserDialogsProviderProxy
//-----------------------------------------------------------------------------
CBrowserDialogsProviderProxy::CBrowserDialogsProviderProxy()
	{
	}


//-----------------------------------------------------------------------------
// CBrowserDialogsProviderProxy::~CBrowserDialogsProviderProxy
//-----------------------------------------------------------------------------
CBrowserDialogsProviderProxy::~CBrowserDialogsProviderProxy()
	{
	}


//-----------------------------------------------------------------------------
//	CBrowserDialogsProviderProxy* CBrowserDialogsProviderProxy::NewL
//-----------------------------------------------------------------------------
CBrowserDialogsProviderProxy* CBrowserDialogsProviderProxy::NewL()
	{
	CBrowserDialogsProviderProxy* self = new (ELeave)
	        CBrowserDialogsProviderProxy( );

	CleanupStack::PushL( self );
	self->ConstructL( );
	CleanupStack::Pop( self );   // self
	return self;
	}

//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::ConstructL
//
//-----------------------------------------------------------------------------
void CBrowserDialogsProviderProxy::ConstructL()
	{
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::DialogNotifyErrorL
//
//-----------------------------------------------------------------------------
void CBrowserDialogsProviderProxy::DialogNotifyErrorL( TInt /* aErrCode */ )
	{
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::DialogNotifyHttpErrorL
//-----------------------------------------------------------------------------
void CBrowserDialogsProviderProxy::DialogNotifyHttpErrorL(
								            TInt /* aErrCode */ , const TDesC& /* aUri */ )
	{
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::DialogFileSelectLC
//-----------------------------------------------------------------------------
TBool CBrowserDialogsProviderProxy::DialogFileSelectLC(
                                                    const TDesC& /* aStartPath */,
													const TDesC& /* aRootPath */,
													HBufC*& /* aSelectedFileName */ )
	{
	TBool retVal( EFalse );
	return retVal;
	}

//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::DialogSelectOptionL
//-----------------------------------------------------------------------------
TBool CBrowserDialogsProviderProxy::DialogSelectOptionL(
								const TDesC& /* aTitle */,
								TBrCtlSelectOptionType /* aBrCtlSelectOptionType */,
								CArrayFix<TBrCtlSelectOptionData>& /* aOptions */ )
	{
	TBool retVal( EFalse );

	return retVal;
	}


//-----------------------------------------------------------------------------
// CBrowserDialogsProviderProxy::DialogUserAuthenticationLC
//-----------------------------------------------------------------------------
TBool CBrowserDialogsProviderProxy::DialogUserAuthenticationLC(
												const TDesC& /* aUrl */,
												const TDesC& /* aRealm */,
												const TDesC& /* aDefaultUserName */,
												HBufC*& /* aReturnedUserName */,
												HBufC*& /* aReturnedPasswd */,
												TBool /* aBasicAuthentication */ )
	{
	TBool retVal( EFalse );
	return retVal;
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::DialogNoteL
//-----------------------------------------------------------------------------
void CBrowserDialogsProviderProxy::DialogNoteL( const TDesC& /* aMessage */)
	{
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::DialogAlertL
//-----------------------------------------------------------------------------
void CBrowserDialogsProviderProxy::DialogAlertL( const TDesC& /* aTitle */,
											        const TDesC& /* aMessage */ )
	{
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::DialogConfirmL
//-----------------------------------------------------------------------------
TBool CBrowserDialogsProviderProxy::DialogConfirmL( const TDesC& /* aTitle */,
												    const TDesC& /* aMessage */,
												    const TDesC& /* aYesMessage */,
												    const TDesC& /* aNoMessage */ )
	{
	TBool retVal( EFalse );
    return retVal;
	}

//-----------------------------------------------------------------------------
// CBrowserDialogsProviderProxy::DialogPromptLC
//-----------------------------------------------------------------------------
TBool CBrowserDialogsProviderProxy::DialogPromptLC( const TDesC& /* aTitle */,
												const TDesC& /* aMessage */,
												const TDesC& /* aDefaultInput */,
												HBufC*& /* aReturnedInput */ )
	{
	TBool retVal( EFalse );
    return retVal;
	}

//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::DialogDownloadObjectL
//-----------------------------------------------------------------------------
TBool CBrowserDialogsProviderProxy::DialogDownloadObjectL(
										CBrCtlObjectInfo* /* aBrCtlObjectInfo */ )
	{
	TBool retVal( EFalse );

	return retVal;
	}

//-----------------------------------------------------------------------------
// CBrowserDialogsProviderProxy::DialogDisplayPageImagesL
//-----------------------------------------------------------------------------
void CBrowserDialogsProviderProxy::DialogDisplayPageImagesL(
							CArrayFixFlat<TBrCtlImageCarrier>& /* aPageImages */ )
	{
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::CancelAll
//-----------------------------------------------------------------------------
//
void CBrowserDialogsProviderProxy::CancelAll()
	{
	}
	
//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::DialogMimeFileSelectLC
//-----------------------------------------------------------------------------
TBool CBrowserDialogsProviderProxy::DialogMimeFileSelectLC(
													HBufC*& /* aSelectedFileName */,
													const TDesC& /* aMimeType */ )
	{
	TBool retVal( EFalse );
	return retVal;
	}

//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::ShowTooltipL
//-----------------------------------------------------------------------------
//
void CBrowserDialogsProviderProxy::ShowTooltipL(    const TDesC& /* aText */,
                                                    TInt /* aDuration */,
                                                    TInt /* aDelay */ )
    {
    }

//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::UploadProgressNoteL
//-----------------------------------------------------------------------------
//
void CBrowserDialogsProviderProxy::UploadProgressNoteL(
                                TInt32 /* aTotalSize */,
                                TInt32 /* aChunkSize */,
                                TBool /* aIsLastChunk */,
                                MBrowserDialogsProviderObserver* /* aObserver */ )
    {
    }

//-----------------------------------------------------------------------------
//  CBrowserDialogsProviderProxy::WindowActivated()
//-----------------------------------------------------------------------------
//
void CBrowserDialogsProviderProxy::WindowActivated()
    {
    }

//  End of File
