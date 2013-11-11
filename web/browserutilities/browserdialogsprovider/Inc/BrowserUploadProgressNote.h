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


#ifndef BROWSERUPLOADPROGRESSNOTE_H
#define BROWSERUPLOADPROGRESSNOTE_H

#include <eikprogi.h>
#include <AknProgressDialog.h>

class CEikDialog;
class CEikProgressInfo;
class MBrowserDialogsProviderObserver;

class CBrowserUploadProgressNote : public CAknProgressDialog,
                                   public MProgressDialogCallback
    {
    public:
		/**
		* Constructor
		* @param aMaxValue The maximal value of the progress.
		* @param aObserver Observers of the dialog
        * @param aSelfPtr  self pointer
        */
		CBrowserUploadProgressNote( TInt aMaxValue,
		                        MBrowserDialogsProviderObserver* aObserver,
		                        CEikDialog** aSelfPtr );

        /**
        * Destructor.
        */      
        virtual ~CBrowserUploadProgressNote();

		/**
        * Two-phased constructor. Leaves on failure. 
        * @param aMaxValue The maximal value of the progress.
        * @param aObserver Observers of the dialog
        * @param aSelfPtr  self pointer
        * @return The constructed progress note.
        */
	    static CBrowserUploadProgressNote* NewL( TInt aMaxValue,
	                            MBrowserDialogsProviderObserver* aObserver,
	                            CEikDialog** aSelfPtr );

		/**
        * Two-phased constructor. Leaves on failure. 
		* Places the instance on the cleanup stack.
        * @param aMaxValue The maximal value of the progress.
        * @param aObserver Observers of the dialog
        * @param aSelfPtr  self pointer
        * @return The constructed progress note.
        */
		static CBrowserUploadProgressNote* NewLC( TInt aMaxValue,
		                        MBrowserDialogsProviderObserver* aObserver,
		                        CEikDialog** aSelfPtr );

        /**
        * Updates the progress bar by a given value.
		* @param aChunkSize The size of chunk of data uploaded this time.
        */      
	    void UpdateL( TInt aChunkSize );
	    
    public: // from MProgressDialogCallback
        
        /**
		* Callback which is called when a dialog has beed dismissed
        * Requires registration with SetCallback( MProgressDialogCallback* )
        * 
		*/
        void DialogDismissedL( TInt aButtonId );
        
	private:
        /**
        * Two-phased constructor. Leaves on failure. 
        */      
        void ConstructL( );

		/**
        * Divides the given parameters, and returns the result
        * as a formatted string.
        * @param aNumerator The numerator of the division.
        * @param aDenominator The denominator of the division.
        * @return A pointer to the created descriptor on the heap.
        *
        * The calling program must destroy the heap descriptor when it is no 
        * longer needed.
        */      
		HBufC* DivisionL( TInt aNumerator, TInt aDenominator );


    private: 
        /**
        *  The sum of sizes for all uploaded fields (to be uploaded).
        */      
		TInt iMaxValue;

        /**
        * The currently uploaded size of data.
        */      
        TInt iUploaded;

		/**
		* Handle of the progress note.
		*/
	    //CAknProgressDialog* iProgressDlg;

		/**
		* Handle to the progress bar of the progress note.
		*/
	    CEikProgressInfo* iProgressInfo;

		/**
		* Handle to the "Uploaded:\n %U / %U " decriptor in resource.
		*/
		HBufC*  iStrUploaded;

		/**
		* Handle to the "kB" / "MB" decriptor in resource.
		*/
		HBufC*  iStrUnit;

        /**
        * Dialog Observer Pointer
        */				
		MBrowserDialogsProviderObserver* iObserver;
    };

#endif