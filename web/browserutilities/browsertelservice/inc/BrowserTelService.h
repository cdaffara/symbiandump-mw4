/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*	  Declaration of class CBrowserTelService.
*
*
*/


// INCLUDE FILES
#ifndef __BROWSERTELSERVICE_H
#define __BROWSERTELSERVICE_H

#include "BrowserTelServiceEtelWatcher.h"

#include <e32def.h> //resolve NULL redefinition warning (conflict with time.h).
#include <e32base.h>
#include <etel.h>
#include <etelmm.h>
#include <f32file.h>
#include <AknProgressDialog.h>
#include <AknWaitDialog.h>
#include <ConnectionObservers.h>
#include <ApEngineConsts.h>
#include <ErrorUI.h> 
#include <bldvariant.hrh>
#include <ConeResLoader.h>

#ifdef __BROWSER_TEL_SERVICES_CALLUI__
// AIW related headers
#include <AiwServiceHandler.h>


#include <aiwdialdata.h>
#include <aiwdialdataext.h>


#endif // __BROWSER_TEL_SERVICES_CALLUI__

#define JAPANESE ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) )

// FORWARD DECLARATIONS
class MBrowserTelServiceObserver;
class CSendAppUi;
class CContactIdArray;
class CPbkContactEngine;
class CPbkContactItem;

// DATA TYPES
enum TBrowserTelServiceState
	{
	EIdle,
	EDialing,
	EDialingDTMF,
	EConnected
	};

enum TBrowserTelServiceError
	{
	ESendDTMFFailed
	};

enum TBrowserTelServiceDlgType
	{
	EConfirmMakeCall,
	EConfirmSendDTMF,
	EConfirmAddToPb,
    ESendingDTMF
	};

enum TMatchType
	{
	EPhoneNumber,
	EEmail
	};

// CONSTANTS

const TUid KUidBrowserApplication = { 0x10008D39 };

// The Central Repository key used to determine how BrowserTelService
// should handle a 'mailto' attribute of an html link element:
//     0 = email;  1 = MMS composer
const TUint32 KBrowserMailtoSendingApp = 0x00000004;

// Return values
const TInt KErrInvocationError = (-200);
const TInt KErrNoCallActive = (-108);	// KErrGsmNoCallActive
const TInt KErrUserNoAnswer = (-107);	// KErrGsmCCUserNoAnswer
const TInt KErrNoService = (-106);		// KErrGsmNoService
const TInt KErrUserBusy = (-105);		// KErrGsmCCUserBusy
const TInt KErrPBPhoneBookFull = (-104);
const TInt KErrPBWriteError = (-103);
const TInt KErrPBNumberTooLong = (-102);
const TInt KErrPBNameTooLong = (-100);
const TInt KErrUnspecified = (-1);

// CLASS DECLARATION

/**
*  Implements telephony services for browser.
*  This class implements required telephony services for multipurpose browser.
*/
class CBrowserTelService : public CActive, 
						   public MBrowserTelServiceEtelWatcherObserver, 
						   public MConnectionStageObserver,
#ifdef __BROWSER_TEL_SERVICES_CALLUI__
                           public MAiwNotifyCallback,
#endif // __BROWSER_TEL_SERVICES_CALLUI__                           
						   public MProgressDialogCallback
	{
	public:		// Constructors and destructor			
		/**
        * Destructor.
        */
		IMPORT_C ~CBrowserTelService();

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aObserver The observer for CBrowserTelService.
        * Defaults to NULL.
        * @return The constructed CBrowserTelService object.
        */		
		IMPORT_C static CBrowserTelService* NewL( MBrowserTelServiceObserver* 
													aObserver = NULL );
		IMPORT_C static CBrowserTelService* NewLC( MBrowserTelServiceObserver* 
													aObserver = NULL );

        /**
        * Parses given phone number. If specified number contains DTMF 
		* sequence it will be
        * extracted from the phone number.
        * @param aNumber. Phone number with possible DTMF sequence.
        * @return The extracted DTMF sequence
        */
        IMPORT_C HBufC* ParsePhoneNumberL( TDes* aNumber );

	public:		// New functions
		/**
        * Call the given phone number synchronously.
        * @param aNumber The phone number to be called.
        * @return Typically KErrNone or another of the system-wide error codes.
        *
        * NOTE: If the specified phone number contains DTMF sequence, it will 
		* be extracted from the number. After the call has been established 
		* the dtmf sequence will be send immediately. This function will 
		* return after the dtmf sending has 
        * been handled.
        */	
		IMPORT_C TInt MakeCall( TDesC& aNumber, TBool aSendDTMFAlwaysConfirm );

		/**
        * Call the given phone number asynchronously.
        * @param aStatus Request status.
		* @param aNumber The phone number to be called.
        * @return Typically KErrNone or another of the system-wide error codes.
        *
        * NOTE: If the specified phone number contains DTMF sequence then it 
		* will be extracted from the number. After the call has been 
		* established the dtmf sequence will be sent immediately.
        */	
		IMPORT_C void MakeCall( TRequestStatus& aStatus, TDesC& aNumber, 
							    TBool aSendDTMFAlwaysConfirm );	

		/**
        * Cancel the request set with 
		* MakeCallL(TRequestStatus& aStatus,TDesC& aNumber)
        */
		IMPORT_C void CancelMakeCall();
		
		/**
        * Send given DTMF sequence (needs an active call) synchronously.
        * @param aNumber DTMF number.
        * @return Typically KErrNone or another of the system-wide error codes.
        */
		IMPORT_C TInt SendDTMF( TDesC& aNumber, TBool aSendDTMFAlwaysConfirm );	

		/**
        * Send given DTMF sequence (needs an active call) asynchronously.
        * @param aStatus Request status.
		* @param aNumber DTMF number.
        * @return Typically KErrNone or another of the system-wide error codes.
        */	
		IMPORT_C void SendDTMF( TRequestStatus& aStatus,TDesC& aNumber, 
							    TBool aSendDTMFAlwaysConfirm );
		
		/**
        * Cancel the request set with SendDTMFL().
        */
		IMPORT_C void CancelSendDTMFL();

		/**
        * Create a new phonebook entry. Draw up a dialog. Fill its names, 
		* and phonenumber fields with the given values.
        * @param aNumber Phone number
		* @param aName Name
		* @param aEmail Email address
        * @return always KErrNone
        */	
		IMPORT_C TInt AddPBEntryL( TDesC& aNumber, 
								   TDesC& aName, 
								   TDesC& aEmail );

		/**
        * Current state of the engine
        * @return TBrowserTelServiceState
        */	
		IMPORT_C TBrowserTelServiceState State() const; 

		/**
        * Add observer. Observers are notified whenever 
		* the engine state changes.
        * @param aObserver The Observer.
        */
		IMPORT_C void AddObserver( MBrowserTelServiceObserver* aObserver );

		/**
        * Remove observer.
        * @param aObserver The Observer.
        */
		IMPORT_C void RemoveObserver( MBrowserTelServiceObserver* aObserver ); 

		/**
        * Create the message and open up the email editor as ready to send.
        * @param aToRecipients To recipients.
		* @param aCcRecipients Cc recipients.
		* @param aParams Other parameters ??.
		* @param aBody Body text.
        * @return Typically KErrNone or another of the system-wide error codes.
        */	
		IMPORT_C TInt SendEmailMessageL( const TDesC& aToRecipients,
										 const TDesC& aCcRecipients, 
										 const TDesC& /*aParams*/,
										 const TDesC& aBody );
		/**
        * Create the message and open up the email editor as ready to send.
        * @param aToRecipients To recipients.
		* @param aCcRecipients Cc recipients.
		* @param aParams Other parameters ??.
		* @param aBody Body text.
		* @param aLaunchEmbedded information to launch embedded mode or not
        * @return Typically KErrNone or another of the system-wide error codes.
        */	
		IMPORT_C TInt SendEmailMessageL( const TDesC& aToRecipients,	
										 const TDesC& aCcRecipients,	
										 const TDesC& aParams,
										 const TDesC& aBody,
										 TBool aLaunchEmbedded );

		/**
		* Check for active CSD/HSCSD connection and disconnect if active.
		*/
		IMPORT_C void DisconnectActiveCSDConnectionL();

        /**
        * Displays specified type of dialog.
        * @param aDynamicText. Typically a phone number, 
		* dtmf string or email address.
        * @param aDlgType. Dialog type.
        * @return 0 if cancelled otherwise non zero.
        */
		IMPORT_C TInt ShowDialogL( const TDesC& aDynamicText, 
								   const TBrowserTelServiceDlgType& aDlgType );
								   
		
		/**
        * Call the given phone number synchronously.
        * @param aNumber The phone number to be called.
        * @return Typically KErrNone or another of the system-wide error codes.
        *
        * NOTE: If the specified phone number contains DTMF sequence, it will 
		* be extracted from the number. After the call has been established 
		* the dtmf sequence will be send immediately. This function will 
		* return after the dtmf sending has 
        * been handled.
        */	
		IMPORT_C TInt MakeVOIPCall( TDesC& aNumber, TBool aSendDTMFAlwaysConfirm );
		

	protected:	// Functions from base classes
		/**
		* From CActive
		*/
		void DoCancel();
		void RunL();

        /**
        * From MProgressDialogCallback
        *
        * Get's called when a dialog is dismissed.
        */
        virtual void DialogDismissedL( TInt aButtonId );

#ifdef __BROWSER_TEL_SERVICES_CALLUI__

        /**
        * From MAiwNotifyCallback
        */
        TInt HandleNotifyL( TInt aCmdId,
                            TInt aEventId,
                            CAiwGenericParamList& aEventParamList,
                            const CAiwGenericParamList& aInParamList );

#endif // __BROWSER_TEL_SERVICES_CALLUI__

	private:	// New functions
	    /**
        * C++ default constructor.
        */
		CBrowserTelService();
		
		/**
        * Second phase constructor. Leaves on failure.
        * @param aObserver The observer for CBrowserTelService. 
        */
		void ConstructL( MBrowserTelServiceObserver* aObserver );

		/**
        * Set the internal state of CBrowserTelService.
        * @param aState The state.
        */		
		void SetState( TBrowserTelServiceState aState );

		/**
        * Set the internal state to idle and notify observers.
		* Just a convenience function.
        */
		void SetStateIdleAndNotifyObservers();	

		/**
        * Notify observers of the current state of CBrowserTelService.
        */
		void NotifyObservers();

		/**
        * Notify observers of error that occurred
        * @param aError The error. 
        */
		void NotifyObservers( TBrowserTelServiceError aError );

        /**
        * Initialization before DoMakeCallL
		* @param aNumber The phone number to be called.
        * @return Typically KErrNone or another of the system-wide error codes.
        */
        void MakeCallL( TDesC& aNumber, TBool aSendDTMFAlwaysConfirm, TBool voipCall = EFalse );

        /**
        * Called if anything went wrong in MakeCall
        */
        void CleanupBuffers();

		/**
        * Make phone call.
        */
		void DoMakeCallL( TDesC& aNumber, TBool voipCall = 0 );

		/**
        * Leave version of exported SendDTMF function.
        */	
		void SendDTMFL( TDesC& aNumber, TBool aSendDTMFAlwaysConfirm );	

		/**
        * Leave version of exported SendDTMF function.
        */	
		void SendDTMFL( TRequestStatus& aStatus,TDesC& aNumber, 
							    TBool aSendDTMFAlwaysConfirm );

		/**
        * Send DTMF sequence.
        * @param aNumber The DTMF number. 
        */
		void DoSendDTMFL( TDesC& aNumber );

		/**
		* Validates the given phone number. Rips all the "-","(",")", 
		* and white spaces off. If the given string has any 
		* alphabetical(excluding w and p) characters, 
		* then NULL will be returned.
        * @param aNumber The phone number.
		* @return If the given string has any alphabetical characters, 
		* then NULL will be returned.
		* Otherwise the validated phone number.
        */
		HBufC* ValidateNumberL( TDesC& aNumber );

        /**
        * Looks for DTMF sequence from a specified phone number.
        * @param aNumber The phone number that might contain a DTMF sequence
        * @return NULL if DTMF sequence couldn't be found. 
		* Otherwise the DTMF number.
        * NOTE: The DTMF sequence will be ripped off from 
		* the given phone number if found.
        */
        HBufC* FindAndRipDTMFSequenceL( TDes* aNumber );

        /**
        * Looks for tel URI parameters from a specified phone number.
        * @param aNumber The phone number that might contain URI parameters
        * @return HBufC containing URI parameters, if parameters couldn't 
        * be found the HBufC with zero len is returned
        * NOTE: The parameters will be ripped off from the given phone 
        * number if found.
        */        
        HBufC* FindAndRipURIParametersL( TDes& aNumber );
        
        
        /**
        * Looks for voip parameters from a specified phone number.
        * @param aNumber The phone number that might contain voip parameters
        * NOTE: The parameters will be ripped off from the given phone 
        * number if found.
        */        
        void FindAndRipVoipParametersL( TDes& aNumber );

        /**
        * Validates the given dtmf number. Characters [p w 0...9 * / ,] 
		* are allowed, with the exception that only one "w" or "/" 
		* (wait) in succession is allowed.
        * @param aNumber The dtmf number do be validated.
        * NOTE: Removes all invalid characters.
        */
        void ValidateDTMFNumberL( TDes* aNumber );

        /**
        * Parses given string into sequences. Sequences are determined 
		* by [ w / p , ] characters.
        * Each sequence begins with [ w / p , ] character.
        */
        CArrayPtrFlat<HBufC>* ParseDTMFSequencesL( const TDesC* aDTMFString );

        /**
        * Parses given string to sequences.
        */
        CArrayPtrFlat<HBufC>* ParseDTMFSequencesForDlgL(
								const TDesC* aDTMFString );

        /**
        * Rips off [ w / p , ] character.
        * NOTE: Assumes that the wait/pause char is the first character of 
		* the given string. If the first character is not a wait/pause 
		* char then this function does nothing.
        */
        TChar RipWaitChar( TDes* aNumber );

        /**
        * Analyses given number whether the DTMF confirmation query 
		* should be displayed.
        * @param aNumber
        * @return ETrue if dialog should be displayed.
        * NOTE: The given string will be modified if a wait 
		* char [/ w p ,]is found. The
        * wait char is deleted from the aNumber.
        */
        TBool DisplaySendDTMFDialog( TDes* aNumber );

        /**
        * Seeks given number in phone book.
        * @param aPhoneNumber The phone number to look for.
        * @return owner's name of the number, 
		* if not found then NULL will be returned.
        */
		HBufC* SearchPbForMatchL( const TDesC& aPhoneNumber );
		CContactIdArray* SearchPbForMatchLC( CPbkContactEngine& aPbkEngine,
                                             const TDesC& aToMatch,
											 const TMatchType& aMatchType );

		/**
        * Closes all open etel sessions.
        */
		void CloseAllSessions();

		/**
        * Internal error handling.
        * @param aError The error.
        */
		void HandleError( TInt aError );

		static TInt IdleCallback( TAny* aTelService );
		void DoIdleCallback();

        /**
        * 
        * @return EFalse if nothing to handle, otherwise ETrue (processing).
        */
        TBool HandleInternalSendDTMFL();

        /**
        * Brings Browser to the foreground.
        */
        void BringBrowserToForeground();

		/**
        * Check if there are activate voice call on.
        */
		void CheckIsThereActivateVoiceCallOnL();
		
		/**
        * Check from DTMF number is there any illeagal char's.
		* aNumber DTMF sequence for check.
		* Return ETrue if everything OK and EFalse if illeagal char
		* in DTMF sequence.
        */
		void CheckDTMFNumberL( TDesC& aNumber );

        /**
        * Search for existing contact item, or create a new empty one.
        * @param aPbkEngine phonebook engine
        * @param aNumber Phone number
		* @param aEmail Email address
        * @param aNewContact ETrue if new contact item created
        * @return existing/new contact item 
        */
        CPbkContactItem* SearchPBItemLC( CPbkContactEngine& aPbkEngine,
                                         TDesC& aNumber, 
                                         TDesC& aEmail,
                                         TBool& aNewContact );                                       

        /**
        * Set contact item's field. If field doesn't exist in the contact 
        * item, a new one is added.
        * @param aField field to set
        * @param phonebook engine
        * @param aContactItem contact item of which field is set
        * @param aFieldValue contact item field value
        * @param index of field to focus initially in contact editor dialog
        */
        void SetPBEntryFieldL( TInt aField,
                               CPbkContactEngine* aPbkEngine,
                               CPbkContactItem* aContactItem,
                               const TPtrC& aFieldValue,
                               TInt& aFormIndex );
                      
        /**
        * Parse recipients received in aRecipients.
        * Recipients are separaret by commas.
        * @param aRecipients Recipient list. Items separated by commas.
        * @return array of recipients
        */
        CPtrC16Array* ParseRecipientsLC( const TDesC& aRecipients );
        /**
        * Looks for postd from a specified phone number.
        * @param aNumber The phone number that might contain a postd parameter
        * @return NULL if postd parameter couldn't be found. 
		* Otherwise the DTMF number.
        */
        HBufC* FindPostdParmL( TDes& aNumber );

	private:	// Functions from base classes	
		/** 
		* From MBrowserTelServiceEtelWatcherObserver
		*/
		void BrowserTelServiceEtelWatcherEvent( RCall::TStatus& aStatus );	

		/** 
		* From MConnectionStageObserver
		*/
		void ConnectionStageAchievedL();

        /**
        * @return the bearer type of connection
        */
        TApBearerType ConnectionTypeL( TUint aApId );

    private:

        void ErrorHandler( TInt aErrorId );

        /**
        * Create the message and open up the mms editor as ready to send.
        * @param aToRecipients To recipients.
		* @param aCcRecipients Cc recipients.
		* @param aParams Other parameters ??.
		* @param aBody Body text.
		* @param aLaunchEmbedded information to launch embedded mode or not
        * @return Typically KErrNone or another of the system-wide error codes.
        */	
        TInt SendMmsMessageL( const TDesC& aToRecipients,
							  const TDesC& aCcRecipients, 
                              const TDesC& aParams,
							  const TDesC& aBody,
							  TBool aLaunchEmbedded );

#ifdef __BROWSER_TEL_SERVICES_CALLUI__

        /**
        * Show confirmation query before sending dtmf when CallUi is used.
		* @param aDTMF DTMF sequence
        * @return TBool User esponse.
        */
       TBool SendDMFConfQueryL( const TDesC& aDTMF );

#endif // __BROWSER_TEL_SERVICES_CALLUI__
    
	private:	// Data
        CArrayPtrFlat<HBufC>* iSendableDTMFNumbers;
        CArrayPtrFlat<HBufC>* iDlgDTMFNumbers;
		RPointerArray<MBrowserTelServiceObserver> iObserverList;
		HBufC* iDTMFNumber;
		TBrowserTelServiceState iState;
		RTelServer iRTelServer;
		RPhone iRPhone;
		//RAdvGsmPhone iRAdvGsmPhone;
        RMobilePhone iRAdvGsmPhone;
		RLine iRLine;
		RCall iRCall;
		RCall::TStatus iCallStatus;
		CBrowserTelServiceEtelWatcher* iWatcher;
		TBool iSynch;
		TInt iRetVal;
		CActiveSchedulerWait iWait;
		TRequestStatus* iUserRequestStatus;
		TBool iAgentDisconnected;
        CAknWaitDialog* iWaitDlg;
        TBool iInternalDTMFWaitStarted;
        TBool iConfirmedOnce;
        TBool iDTMFAlwaysConfirm;
		CIdle* iIdle;
        CErrorUI* iErrorUi;
        
        TFileName                   iResourceFileName; ///< Temp storage for the file name.
        RConeResourceLoader         iResourceLoader; ///< Resource loader.
        TBool                       iResourceOpened; ///< Indicate if the resource is opened.

#ifdef __BROWSER_TEL_SERVICES_CALLUI__

		// AIW related members
        CAiwServiceHandler* iServiceHandler;

        // DialData.
        CAiwDialData*      iDialData;

#endif // __BROWSER_TEL_SERVICES_CALLUI__
        
	};

/**
* This class defines a protocol for observing CBrowserTelService stages.
* You should inherit from this if you want to observe 
* CBrowserTelService stages.
*/
class MBrowserTelServiceObserver
	{
	public:	// New functions
		/**
        * Notification of the state change.
        * @param aEvent The new state.
        */
		virtual void BrowserTelServiceEvent( 
								TBrowserTelServiceState aEvent ) = 0;

		/**
        * Notification of the error that occurred.
        * @param aError The error.
        */
		virtual void BrowserTelServiceError(
								TBrowserTelServiceError aError ) = 0;
	};

#endif // __BROWSERTELSERVICE_H