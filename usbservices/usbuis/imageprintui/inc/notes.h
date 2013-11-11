/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for notes
*
*/

#ifndef CNOTES_H
#define CNOTES_H


#include <aknview.h>
#include <AknProgressDialog.h>
#include <AknWaitDialog.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MProgressDialogCallback; 
class CEikProgressInfo;
class CImagePrintUiAppUi;



/**
* Class for showing different notes
*/
class CNotes :  public CBase, public MProgressDialogCallback
    {

public:

   
    /**
     * Factory method NewL
     * @param aPrintUiAppUi the pointer to AppUi class
     * @return   the new object 
     */
    static CNotes* NewL(CImagePrintUiAppUi* aPrintUiAppUi);

   /**
    * Destructor.
    */ 
    virtual ~CNotes();
    
    
    /**
     * Shows all string based info notes
     * @return none
     */  
    void ShowInfoNoteL(TInt aResourceId );
    /**
     * Shows all string based wait notes
     * @return none
     */  
    void ShowNoteL(TUint aNoteType);
    
    /**
     * Shows print progress note
     * @return none
     */  
    void ShowProgressNoteL();
    
    /**
     * Informs printing progress
     * @return none
     */  
    void PrintProgress(TUint aProgress );
    
    /**
     * Informs printing progrees 
     * @return none
     */  
    void PrintProgressL(TUint aProgress );
    
    /**
     * Prepare print progress note
     * @return none
     */  
    void PrepareShowProgressNoteL();
    
    /**
     * Shows all string based error messages
     * @return none
     */  
    void ShowErrorMsgL(TUint aErr );
    
    /**
     * Stop shows all string based notes
     * @return none
     */  
    void StopShowNote(TUint aNoteType);
    
    /**
     * Start show printing note
     * @return none
     */  
    void StartPrintingNotesL();
    
    /**
     * Finish print progress note
     * @return none
     */  
    void PrintFinished();
    
    /**
     * Sat cancel state
     * @return none
     */  
    void SetCancelState(TBool aCancel);
    
    /**
     * Set disconnect state
     * @return none
     */  
    void SetDisconnectState(TBool aDisconnect);
	    
  
public: 

    /**
    * From MProgressDialogCallback 
    * Get's called when a dialog is dismissed.        
    * @param aButtonId Id of the pressed button.
    * @return None.
    */
    void DialogDismissedL( TInt aButtonId );

private:

    /**
     * C++ default constructor
     * @param aPrintUiAppUi the pointer to AppUi class 
     */     
     CNotes(CImagePrintUiAppUi* aPrintUiAppUi );
     
private: // data

    /**
     * Wait dialog for telling the user to connect the cable
     * Own
     */    
    CAknWaitDialog* iWaitNote;
    
    CAknProgressDialog* iProgressDialog;
    CImagePrintUiAppUi* iPrintUiAppUi;
    TBool iDisconnect;
    TBool iCancelling;
 
    };

#endif // CNOTES_H
