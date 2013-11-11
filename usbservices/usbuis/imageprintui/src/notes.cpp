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
* Description:  Show all notes on the screen
*
*/


#include <aknViewAppUi.h>
#include <imageprintui.rsg>

#include "imageprintui.hrh"
#include "settingsview.h"
#include <AknWaitDialog.h>
#include <aknnotedialog.h>
#include <aknprogresstimer.h>


#include <e32std.h>
#include <AknProgressDialog.h>
#include <AknInfoPopupNoteController.h>

#include <aknstaticnotedialog.h>
#include <eikprogi.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <eikenv.h>

#include "settingscontainer.h"
#include "imageprintuidebug.h"
#include "imageprintuiappui.h"
#include "notes.h"

const TUint KProgressFinishValue = 100;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNotes* CNotes::NewL(CImagePrintUiAppUi* aPrintUiAppUi)
    {
    FLOG(_L("[IMAGEPRINTUI]\t CNotes::NewL()"));
    CNotes* self = new( ELeave ) CNotes(aPrintUiAppUi);
    FLOG(_L("[IMAGEPRINTUI]\t CNotes::NewL() complete"));
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNotes::~CNotes()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CNotes::Destructor"));
    if(iWaitNote)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CNotes::Delete iWaitNote"));
        delete iWaitNote;
        iWaitNote = NULL;
    	}
    if ( iProgressDialog )
        {
        FLOG(_L("[IMAGEPRINTUI]\t CNotes::Delete iProgressDialog"));
        delete iProgressDialog;
        iProgressDialog = NULL;
        }
    FLOG(_L("[IMAGEPRINTUI]\t CNotes::Destructor complete"));

    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CNotes::CNotes(CImagePrintUiAppUi* aPrintUiAppUi): iPrintUiAppUi(aPrintUiAppUi),
    iDisconnect(EFalse ),
    iCancelling(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNotes::DialogDismissedL(TInt aButtonId )
    {
    FLOG(_L("[IMAGEPRINTUI]\t CNotes::DialogDismissedL BEGIN"));	
	
    // If user pressed cancel, cancel printing
    if(iProgressDialog)
    	{
        if(!iDisconnect)
        	{
        	FLOG(_L("[IMAGEPRINTUI]\t CNotes::DialogMissedL, not disconnect case"));
		    if ( aButtonId == EAknSoftkeyCancel )
		        {
		        FLOG(_L("[IMAGEPRINTUI]\t CNotes::DialogMissedL ,sotfkey cancel iProgressDialog"));	
		        iCancelling = ETrue;
		        iPrintUiAppUi->CancelPrinting();
		        }
        	}
    	}
	if(iWaitNote)
		{
	 	if(!iDisconnect)
        	{
		 	if ( aButtonId == EAknSoftkeyCancel )
		        {
		        FLOG(_L("[IMAGEPRINTUI]\t CNotes::DialogMissedL ,sotfkey cancel iWaitNote"));
		        iPrintUiAppUi->CloseApplication(EFalse);
		        }
        	}
	  	}
    
    FLOG(_L("[IMAGEPRINTUI]\t CNotes::DialogDismissedL END"));	
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNotes::ShowNoteL(TUint aNoteType)
	{
	FLOG(_L("[IMAGEPRINTUI]\t CNotes::ShowNote Start"));
	if(!iWaitNote)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CNotes::Create note"));
		iWaitNote = new (ELeave) CAknWaitDialog(
	              (REINTERPRET_CAST(CEikDialog**,&iWaitNote)), ETrue);
	    FLOG(_L("[IMAGEPRINTUI]\t CNotes::ShowNoteL; Note created"));          
        iWaitNote->SetCallback( this ); 
        FLOG(_L("[IMAGEPRINTUI]\t CNotes::ShowNoteL; Callback set"));   
        iWaitNote->ExecuteLD(aNoteType);
        FLOG(_L("[IMAGEPRINTUI]\t CNotes::ShowNoteL; ExecuteLD passed")); 
    	} 	
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CNotes::StopShowNote(TUint /*aNoteType*/)
	{
	FLOG(_L("[IMAGEPRINTUI]\t CNotes::Stop note"));
	delete iWaitNote;
	iWaitNote = NULL;
	}
	


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNotes::StartPrintingNotesL()
    {
    FLOG(_L("[IMAGEPRINTUI]>>> CNotes::StartPrintingNotesL")); 
	PrepareShowProgressNoteL();
    FLOG(_L("[IMAGEPRINTUI]<<< CNotes::StartPrintingNotesL")); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CNotes::PrepareShowProgressNoteL()
	{
	FLOG(_L("[IMAGEPRINTUI]>>> CNotes::PrepareShowProgressNoteL")); 
	HBufC* txt;
	txt = StringLoader::LoadLC( R_QTN_PRINT_PROGRESS_NOTE_TITLE );
            
	if(!iProgressDialog)
    	{
    	iProgressDialog = new (ELeave) CAknProgressDialog(
						(reinterpret_cast<CEikDialog**> (&iProgressDialog)));				
    	}
    	
    FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrepareShowProgressNoteL; PrepareLC next"));	
   	iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
	CEikProgressInfo* progressBar = iProgressDialog->GetProgressInfoL(); // not taking ownership
	FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrepareShowProgressNoteL; progressinfo got"));	
	
    progressBar->SetFinalValue( KProgressFinishValue );
    iProgressDialog->SetTextL( *txt );
    iProgressDialog->SetCallback( this );
    FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrepareShowProgressNoteL; RunLD next"));  
    iProgressDialog->RunLD();
    CleanupStack::PopAndDestroy( txt );  
	FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrepareShowProgressNoteL")); 
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CNotes::PrintProgress(TUint aProgress )
    {
    FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrintProgress start "));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CNotes PrintProgress aProgress value  is %d"), aProgress ));	
    TInt err = KErrNone;
    TRAP( err, PrintProgressL( aProgress ) );
    if ( err != KErrNone )
        {
        FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CNotes PrintProgress err value  is %d"), err ));	
        TRAP_IGNORE( CNotes::ShowErrorMsgL( err ) );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNotes::SetCancelState(TBool aCancel)
	{
	iCancelling = aCancel;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CNotes::SetDisconnectState(TBool aDisconnect)
	{
	iDisconnect = aDisconnect;
	}


// ---------------------------------------------------------------------------
// Called to update the print progress 
// ---------------------------------------------------------------------------
//
void CNotes::PrintProgressL(TUint aProgress )
    {
    FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrintProgressL start; "));
	if( !iCancelling )
		{
		if ( !iProgressDialog )
        	{
        	FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrintProgressL call PrepareShowProgressNoteL "));
			PrepareShowProgressNoteL();
        	}
    	CEikProgressInfo* progressBar = iProgressDialog->GetProgressInfoL(); // not taking ownership
    	FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrintProgressL; Update progress"));
    	progressBar->SetAndDraw( aProgress );
    	if(aProgress == KProgressFinishValue )
    		{
    		FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrintProgressL; normal printing finish"));
    		PrintFinished();
    		}
		}
    FLOG(_L("[IMAGEPRINTUI]>>> CNotes::PrintProgressL END "));
    }	

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	    
void CNotes::ShowErrorMsgL(TUint aErrCode )
	{
	if ( aErrCode != KErrNone )
        {
        FLOG(_L("[IMAGEPRINTUI]>>> CNotes:Show err note "));
        HBufC* str = StringLoader::LoadLC( aErrCode  );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote;
        note->ExecuteLD( *str );
        CleanupStack::PopAndDestroy( str );  // str
        }
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CNotes::ShowInfoNoteL(TInt aResourceId )
    {
    HBufC* str = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
    note->ExecuteLD( *str );
    CleanupStack::PopAndDestroy( str);  // str
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//			
void CNotes::PrintFinished()
    {
    FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrintFinished; START"));
    if ( iProgressDialog )
        {
        FLOG(_L("[IMAGEPRINTUI]<<< CNotes::PrintFinished, delete progress note"));
        delete iProgressDialog;
        iProgressDialog = NULL;
        iCancelling = EFalse;
        }
    }

//End of File


