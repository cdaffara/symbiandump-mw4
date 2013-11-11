/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class creates a progress dialog, which shows the progress of an upload
*     
*
*/


// INCLUDES
#include "BrowserUploadProgressNote.h"
#include "BrowserDialogsProviderObserver.h"

#include <BrowserDialogsProvider.rsg>
#include <StringLoader.h>

// ----------------------------------------------------------------------------
// CBrowserUploadProgressNote::CBrowserUploadProgressNote
// ----------------------------------------------------------------------------
//
CBrowserUploadProgressNote::CBrowserUploadProgressNote( 
                            TInt aMaxValue,
                            MBrowserDialogsProviderObserver* aObserver,
                            CEikDialog** aSelfPtr ) :
    CAknProgressDialog( aSelfPtr, ETrue ), // should use EFalse, but doesn't work!
    iMaxValue( aMaxValue ),
    iObserver( aObserver )
    {
    }

// ----------------------------------------------------------------------------
// CBrowserUploadProgressNote::~CBrowserUploadProgressNote
// ----------------------------------------------------------------------------
//
CBrowserUploadProgressNote::~CBrowserUploadProgressNote( )
    {
	delete iStrUploaded;
	delete iStrUnit;
    }

// ----------------------------------------------------------------------------
// CBrowserUploadProgressNote::NewL
// ----------------------------------------------------------------------------
//
CBrowserUploadProgressNote* CBrowserUploadProgressNote::NewL( 
                            TInt aMaxValue,
                            MBrowserDialogsProviderObserver* aObserver,
                            CEikDialog** aSelfPtr )
    {
    CBrowserUploadProgressNote* self = NewLC( aMaxValue, aObserver, aSelfPtr );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CBrowserUploadProgressNote::NewLC
// ----------------------------------------------------------------------------
//
CBrowserUploadProgressNote* CBrowserUploadProgressNote::NewLC( 
                            TInt aMaxValue,
                            MBrowserDialogsProviderObserver* aObserver,
                            CEikDialog** aSelfPtr )
    {	
    CBrowserUploadProgressNote* self = 
                            new ( ELeave ) CBrowserUploadProgressNote( 
                            aMaxValue, aObserver, aSelfPtr );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;	
    }

// ----------------------------------------------------------------------------
// CBrowserUploadProgressNote::UpdateL
// ----------------------------------------------------------------------------
//
void CBrowserUploadProgressNote::UpdateL( TInt aChunkSize )
	{ 
    iUploaded+=aChunkSize; // Set the uploaded size.

    // ...Length() + 20 because if we substitute the uploaded, 
    // and the max upload strings into the
    // "Uploaded:\ %U / %U" string, there should be enough space in the buffer.
    // The max length of the substituted strings is not more than 8 characters, 
    // so 20 characters should be enough, and won`t cause errors, if the size 
    // of the substituted strings slightly changes.
	TInt length = iStrUploaded->Des( ).Length() + iStrUnit->Des().Length() + 20;

    // The final string that will be displayed on the progress note.
	HBufC* progressNoteText = HBufC::NewLC( length );

    // Temporary buffer for formatting.
    HBufC* tempProgressNoteText = HBufC::NewLC( length );

    // Temporary pointer needed, for using StringLoader::Format.
    TPtr progressNoteTextPtr = progressNoteText->Des();

	// If we are showing data in kB Units.
	if ( iMaxValue < 1000 )
		{
        // Convert the uploaded, and max upload size to string.
        TBuf<3> strMax,strUploaded;
        strMax.AppendNum( iMaxValue );
        strUploaded.AppendNum( iUploaded );
        
        // Substitute the first index in the string from .loc file.
        StringLoader::Format( progressNoteTextPtr,iStrUploaded->Des(),
                                                                0,strUploaded );

        // Substitute the second index in the string from .loc file.
        tempProgressNoteText->Des( ).Copy( progressNoteTextPtr ); // Copy to a temporary buffer.
        // Format and put the fully formatted string into progressNoteText
        StringLoader::Format( progressNoteTextPtr,tempProgressNoteText->Des(),
                                                                    1,strMax );
        }
	// If we are showing data in MB Units.
	else 
		{
        // Convert the uploaded, and max upload size to MB units.
		HBufC* strMax = DivisionL( iMaxValue,1000 );
        CleanupStack::PushL( strMax );
		HBufC* strUploaded = DivisionL( iUploaded,1000 );
        CleanupStack::PushL( strUploaded );

        // Substitute the first index in the string from .loc file.
        StringLoader::Format( progressNoteTextPtr,iStrUploaded->Des(),
                                                        0,strUploaded->Des() );

        // Substitute the second index in the string from .loc file.
        tempProgressNoteText->Des().Copy( progressNoteTextPtr ); // Copy to a temporary buffer.
        // Format and put the fully formatted string into progressNoteText
        StringLoader::Format( progressNoteTextPtr,tempProgressNoteText->Des(),
                                                            1,strMax->Des() );

		CleanupStack::PopAndDestroy( 2 ); // strMax, struploaded
		}

    progressNoteText->Des().Append( iStrUnit->Des() ); // Append the unit.        
	/*iProgressDlg->*/SetTextL( progressNoteText->Des() ); // Set the text of the note.
	iProgressInfo->SetAndDraw( iUploaded ); 
    CleanupStack::PopAndDestroy( 2 ); //progressNoteText, tempProgressNoteText
	}


// ----------------------------------------------------------------------------
// CBrowserUploadProgressNote::ConstructL
// ----------------------------------------------------------------------------
//
void CBrowserUploadProgressNote::ConstructL( )
    {
    // TO DO: Param should be EFalse, but it fails....
	/*iProgressDlg = new( ELeave ) CAknProgressDialog( 
                        ( REINTERPRET_CAST ( CEikDialog**, &iProgressDlg) ), ETrue );*/
	
    iStrUploaded = StringLoader::LoadL( 
                                R_QTN_BROWSER_UPLOAD_PROGRESSNOTE_UPLOADED );

    // If "kB" unit is needed...
	if ( iMaxValue<1000 )
		{
		iStrUnit = CCoeEnv::Static()->AllocReadResourceL( 
                                R_QTN_BROWSER_UPLOAD_PROGRESSNOTE_UNIT_KBYTE );
		}
	else
		{
		iStrUnit = CCoeEnv::Static()->AllocReadResourceL( 
                                R_QTN_BROWSER_UPLOAD_PROGRESSNOTE_UNIT_MB );
		}

	PrepareLC( R_UPLOAD_PROGRESSNOTE );
    
    iProgressInfo = GetProgressInfoL( );
	iProgressInfo->SetFinalValue( iMaxValue );
	
	// Set callback for when dialog is dismissed, handled in DialogDismissedL()
	SetCallback( this );
	
	// Show the dialog
	RunLD();
    }

// ----------------------------------------------------------------------------
// CBrowserUploadProgressNote::DivisionL
// ----------------------------------------------------------------------------
//
HBufC* CBrowserUploadProgressNote::DivisionL( TInt aNumerator,TInt aDenominator )
	{
    TLocale locale;
	TInt reminder = aNumerator%aDenominator;
    // Temporary buffer for the result of the division.
	HBufC* tempString = HBufC::NewLC( 50 );

    // Copy the integer part of the division.
	tempString->Des().AppendNum( aNumerator / aDenominator );
    //Copy the decimal separator.
    tempString->Des().Append( locale.DecimalSeparator() );
 
    // Copy the fraction part, we need 3 decimal fractions.
    // Example: 2.00 or 3.50...
    for ( TInt i=0;i<3;i++ )
        {
        TInt newReminder = ( reminder * 10 ) % aDenominator;
        reminder = ( reminder * 10 ) / aDenominator;
        tempString->Des().AppendNum( reminder );
        reminder = newReminder;
        }

    HBufC* returnString = HBufC::NewLC( 4 );

    // Only 3 numbers needed in the string. So, if the integer part is bigger
    // than 99.9, we chop the reminder part...
    // Example 100.342 will be 100 , 158.654 will be 158, 
    // because of the specification.

    //If the integer value is bigger than 99, then chop...
    if ( ( TChar )tempString->Des()[3] == locale.DecimalSeparator() )
        {
        returnString->Des().Copy( tempString->Des().Left( 3 ) );
        }
    // Or simply copy to the result...
    else
        {
        returnString->Des().Copy( tempString->Des().Left( 4 ) );
        }

    CleanupStack::Pop(); // returnString
    CleanupStack::PopAndDestroy(); // tempString
    return returnString;
	}

// ----------------------------------------------------------------------------
// CBrowserUploadProgressNote::DialogDismissedL
// ----------------------------------------------------------------------------
//
void CBrowserUploadProgressNote::DialogDismissedL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyCancel )
        {
        if ( iObserver )
            {
            // Indicate to observer that the dialog is now dismissed
            iObserver->ReportDialogEventL( 
                MBrowserDialogsProviderObserver::EUploadProgress, KErrCancel );
            }
        }
    }

//  END OF FILE
