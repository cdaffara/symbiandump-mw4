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
*     This class provides connection/session related information.
*	  Both the requested and the stored AP's are here.
*
*/


#ifndef INTERNET_CONNECTION_MANAGER_H
#define INTERNET_CONNECTION_MANAGER_H

#warning The Connection Manager API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

//FORWARD DECLARATIONS
class CIntConnectionInitiator;
class CApUtils;
class CApDataHandler;
class CCommsDatabase;
class CAknGlobalNote;
class CAknGlobalConfirmationQuery;
class CAknWaitDialog;
class CActiveConnectorSyncWrapper;
class TCommDbConnPref;
class TConnSnapPref;

//INCLUDES
#include <bldvariant.hrh>
#include <e32base.h>
#include <cdbpreftable.h>
#include <f32file.h>
#include <es_sock.h>
#include <in_sock.h>
#include <aknprogressdialog.h>
#include <etel.h>
#include <exterror.h>
#include <mconnection.h>
#include <IdPair.h>
#include <GsmError.h>
#include <etelpckt.h>
#include <etelmm.h>
#include <etelmmerr.h>
#include <VpnAPItem.h>
#include <VpnApEngine.h>
#include <ConnectionObservers.h>
#include <errorui.h>
#include <AknNotifyStd.h>

#include <CentralRepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>

//CONSTS
const TUint32 KPrompt = 0;
const TInt KErrGeneralConnection    = -50000;
const TInt NW_STAT_WPS_ERROR = -26041;
const TInt KMaxStages = 3;

_LIT( KConnectionResourceFileFormatter, "z:%SCONNECTIONMANAGER.RSC" );

const TInt KEtelExtErrIntervalHigh = KErrEtelGsmBase;
const TInt KEtelExtErrIntervalLow = KEtelExtErrIntervalHigh - 1160;
const TInt KGsmErrIntervalHigh = KErrPhoneSpecificGsmBase;
const TInt KGsmErrIntervalLow = KGsmErrIntervalHigh - 500;
const TInt KEtelCoreErrIntervalHigh = KErrEtelCoreBase;
const TInt KEtelCoreErrIntervalLow = KEtelCoreErrIntervalHigh - 64; 
const static TInt KADontShowErrors[] = 
	{ KErrGsmCCUnassignedNumber, 
	  KErrGsmCCNoRouteToDestination, 
      KErrGsmCCChannelUnacceptable,
	  KErrGsmCCOperatorDeterminedBarring, 
      KErrGsmCCUserBusy,
	  KErrGsmCCUserNotResponding, 
      KErrGsmCCUserAlertingNoAnswer,
	  KErrGsmCCNumberChanged,
	  KErrGsmCCDestinationOutOfOrder,
	  KErrGsmCCInvalidNumberFormat, 
	  KErrGsmCCNoChannelAvailable, 
      KErrGsmCCNetworkOutOfOrder,
	  KErrGsmCCTemporaryFailure, 
      KErrGsmCCSwitchingEquipmentCongestion,
	  KErrGsmCCRequestedChannelNotAvailable, 
      KErrGsmCCResourceNotAvailable,
	  KErrGsmCCQualityOfServiceNotAvailable,
	  KErrGsmCCIncomingCallsBarredInCug, 
	  KErrGsmCCRequestedFacilityNotImplemented,
	  KErrGsmCCInvalidCallReferenceValue, 
	  KErrGsmCCUserNotInCug, 
      KErrGsmCCIncompatibleDestination,
	  KErrGsmCCInvalidTransitNetworkSelection,
	  KErrGsmCCSemanticallyIncorrectMessage,
	  KErrGsmCCIncompatibleMessageInProtocolState,
	  KErrGsmCCConditionalIEError,
	  KErrGsmCCUnspecifiedProtocolError,
      KErrGsmCCRequestedFacilityNotSubscribed,
      KErrGsmCCAccessInformationDiscarded,
      KErrGsmCallInProgress,
      KErrGsmNumberBarred,
      KErrGsmInvalidFdn,
      KErrGsmNotAllowed,
      KErrGsmNoService,
      KErrGsmCallServerFail,
      KErrGsmGprsActive,
      KErrGsm0707SimNotInserted,
      KErrGsm0707SIMPuk1Required,
      KErrGsm0707SimPin1Required,
      KErrGsm0707SimPin2Required,
      KErrGsm0707SimFailure,
      KErrGsm0707PhoneToSimLockRequired,
      KErrGsm0707SimWrong,
      KErrArgument,
      KErrGsmNoNumber,
      KErrGsmCCFacilityRejected,
      KErrMMEtelActivationBlockedByCallControlNoText,
      KErrMMEtelActivationBlockedByCallControlWithText
	  };

const static TInt KNumsOfDontShowErrors = sizeof(KADontShowErrors) / sizeof(TInt);

//these errors will not be converted to a general one!
const static TInt KAExcludeFromConverting[] =
	{
	KErrGprsServicesNotAllowed, KErrGprsAndNonGprsServicesNotAllowed,
	KErrGprsMSIdentityCannotBeDerivedByTheNetwork, KErrGprsMSImplicitlyDetached,
	KErrGprsMSCTemporarilyNotReachable, KErrGprsLlcOrSndcpFailure,
	KErrGprsInsufficientResources, KErrGprsMissingorUnknownAPN, 
	KErrGprsUnknownPDPAddress, KErrGprsUserAuthenticationFailure,
	KErrGprsActivationRejectedByGGSN, 
	KErrGprsNSAPIAlreadyUsed, KErrGprsRegularContextDeactivation,
	KErrGprsQoSNotAccepted, KErrGprsReactivationRequested,
	KErrGprsFeatureNotSupported,
    KErrEtelCallNotActive,
    KErrGsmMMNetworkFailure,
    KErrGprsActivationRejected,
    KErrGsmMMServiceOptionNotSubscribed,
    KErrGsmMMServiceOptionTemporaryOutOfOrder,
    KErrPacketDataTsyMaxPdpContextsReached
	};

const static TInt KNumsOfExcludeFromConverting = sizeof(KAExcludeFromConverting) / sizeof(TInt);

class MOffLineObserver
    {
    public:

        virtual void OfflineModeChanged( TBool aEnabled ) = 0;
    };

//This class is responsible for the logic that lies in connection handling
NONSHARABLE_CLASS(  CInternetConnectionManager ): public CBase, 
                                  public MProgressDialogCallback, 
                                  public MConnection,
                                  public MConnectionMultiStageObserver,
                                  public MOffLineObserver
	{
	public:
		/**
		*Symbian OS 2 phased constructor
		*@param aSilentMode: Set it to true if you don't want any userinteraction
		*/
		IMPORT_C static CInternetConnectionManager* NewL( TBool aSilentMode= EFalse );

		
		/**
		*Symbian OS 2 phased constructor
		*@param aSilentMode: Set it to true if you don't want any userinteraction
		*/
		IMPORT_C static CInternetConnectionManager* NewL( CCommsDatabase* aCommsDb, TBool aSilentMode= EFalse );

		/**
		*Symbian OS 2 phased constructor
		*@param aSilentMode: Set it to true if you don't want any userinteraction
		*/
		IMPORT_C static CInternetConnectionManager* NewLC( TBool aSilentMode= EFalse );
	
		/**
		*Symbian OS 2 phased constructor
		*@param aSilentMode: Set it to true if you don't want any userinteraction
		*/
		IMPORT_C static CInternetConnectionManager* NewLC( CCommsDatabase* aCommsDb, TBool aSilentMode= EFalse );
	
	public:
		/**
		*Connects to the network
		*It can leave with a lot of errors coming from underlying components
		*If you don't want to handle all the leaves separately TRAP it 
		*and if error occured (other than OOM) call ShowGeneralConnectionErrorL
		*@param aIAPId1 the first AP id to try the connection with, if 0 the user will be prompted
		*@param aIAPId2 the second AP id to try the connection with, if 0 the user will be prompted
		*@return KErrNone if no error occured, KErrGeneral if any connection error occured
		*/
		IMPORT_C TInt ConnectL( TUint32 aIAPId1, TUint32 aIAPId2 );
		
		/**
		*Connects to the network
		*It can leave with a lot of errors coming from underlying components
		*If you don't want to handle all the leaves separately TRAP it 
		*and if error occured (other than OOM) call ShowGeneralConnectionErrorL
		*@param aIAPId the AP id to try the connection with, if 0 the user will be prompted
		*@return KErrNone if no error occured, KErrGeneral if any connection error occured
		*/
		IMPORT_C TInt ConnectL( TUint32 aIAPId );
			
		/**
		*Call this function to show a localised general error
		*It can be used after a trapped call to ConnectL
		*/
		IMPORT_C void ShowGeneralConnectionErrorL();

		/**
		*Use this function to stop the connection
		*/
		IMPORT_C void StopConnectionL();

		/**
		*Use this function to find out if the old already existing connection is used
		*or a new connection is established
		*/
		IMPORT_C TBool NewConnectionEstablished(  ) const;

        /**
        * Change Internet Accespoint ( Change Connection )
        * @return
        */
        IMPORT_C void ChangeIapL( TConManChangeConn& aCangeConn,
                                  TUint32& aNewAp );

        /**
        * Ask Internet Accespoint from dialog
        * @return KErrNone if the user selected an Iap or an error code
        */
        IMPORT_C TInt AskIap( TUint32& aNewAp  );
        
        /**
        * Shows the connection changed dialog.
        * @return none.
        */
        IMPORT_C void ShowConnectionChangedDlg();
	
	public:

		/**
		*Destructor
		*/
		virtual ~CInternetConnectionManager();	

	public: //MProgressDialogCallback

		/**
		*This is called when the Connection Wait Dialog is dismissed
		*@param aButtonId The button that was used to dismiss the dialog
		*/
		void DialogDismissedL( TInt aButtonId );

	protected:

		/**
		*first phase constructor
		*/
		CInternetConnectionManager( TBool aSilentMode= EFalse );
		
		/**
		*first phase constructor
		*/
		CInternetConnectionManager( CCommsDatabase* aCommsDb, TBool aSilentMode= EFalse );	

	private:

		enum TWaitIconType { EWaitNoIcon, EWaitDataIcon, EWaitGPRSIcon };	

	private:

		/*Does the connection with no check for existing one
		*It does not display any errors
		*@param aPref1 the first preference to try
		*@param aPref2 the second preference to try
		*@return KErrNone or KErrGeneral
		*/
		TInt ConnectWithoutCheckL ( TUint32 aIAPId1, TUint32 aIAPId2 );
		
		/*Does the connection
		*It does not display any errors
		*@param aPref1 the first preference to try
		*@param aPref2 the second preference to try
		*@return KErrNone or KErrGeneral
		*/
		TInt ConnectWithoutCheckL( TUint32 aIAPId1 );
		
		//Can be called after a ConnectWithoutCheckL
		void DisplayConnErrorL();
			
		/*Does the connection
		*@param aPref1 the first preference to try
		*@param aPref2 the second preference to try
		*@return KErrNone or KErrGeneral
		*/
		TInt ConnectL
			(
            TCommDbConnPref& aPref1,
            TCommDbConnPref& aPref2
			);
		
		/**
		*Does the connection
		*@param aPref1 the preference to try
		*@return KErrNone or KErrGeneral
		*/
		TInt ConnectL
			(
            TCommDbConnPref& aPref1
			);

		/**
		*Check network availability
		*@param aGPRSAvailable on return it is ETrue if there is GPRS coverage
		*@return EFalse if there is no network
		*/	
		TBool CheckNetworkL( TBool& aGPRSAvailable );
				
		/**
		*Return the bearer type of the given AP.
		*@param aIAPId the IAPId
		*/
		TApBearerType BearerTypeL( TUint32 aIAPId );

		/**
		*returns the name of the specified IAP
		*@param aIAPId the id of the IAP
		*@return the name of the AP
		*/
		HBufC* APNameLC( TUint32 aIAPId ) const;

		/**
		*Returns the whether WLAN is supported in off-line mode. 
		**/
		TBool CheckForWLAN();

	private:
		
		/**
		*Do change iap.
		*/        
        TBool DoChangeIapL( TUint32& aNewAp );

	private:

	        /**
		*second phase constructor
		*/
		void ConstructL();

                /**
		* pops connection note while it is in connecting mode
		* @param aLabel
		* @param aIcon
		*/
		void StartWaitDialogL( HBufC* aLabel, TWaitIconType aIcon );
		
		/**
		*Pops up query note with the default icon and OK Cancel CBA pair
		*@param aTextResId
		*@return the button id pressed on dialog dismissal
		*/
		TInt ShowConfQueryNoteL(TInt aTextResId );

		/**
		*Pops up information note with the default icon
		*@param aTextResId
		*/
		void ShowInformationNoteL( TInt aTextResId );

		/**
		*Pops up error note with the default icon
		*@param aTextResId
		*@param aContext The context of the error, need for special cases. by default
		*       ECtxAutomatic        
		*/
		void ShowErrorNoteL( 
		    TInt aErrorStat,
		    CTextResolver::TErrorContext aContext = CTextResolver::ECtxAutomatic );

		
		/**
		*Utility function to decide whether an element is in an int array
		*/
		TBool InIntArray( TInt aUpperBound, const TInt* aArray, TInt aElement );
 				
		/**
		*Utility functions that are used to establish a connection with a snap Id
		*/
 		TInt ConnectWithSnapId(TUint32 aRequestedSnapId);
 		
 		TInt ConnectSnapWithoutCheckL(TUint32 aRequestedSnapId);
		 
		TInt ConnectSnapL	(TConnSnapPref& aPref1);
 
		/**
		*Pops up global note (of specified note type) with the default icon
		*@param aNoteType, the note type
		*@param aTextResId
		*/
		void ShowGlobalNoteL( TAknGlobalNoteType aNoteType, TInt aTextResId );
 
    public: // from MConnection

	    /**
	    *Sets the requested AP later it may not be the one to be used
	    *@param aRequestedAPs the requested AP idpair 
	    */
	    IMPORT_C void SetRequestedAPs( TIdPair aRequestedAPs );
	    
	    /**
	    *Sets RequestedAP
	    *@param aRequestedAP the requested AP id
	    */
	    IMPORT_C void SetRequestedAP( TUint32 aRequestedAPId , TBool aDefault = EFalse );
	    
	    IMPORT_C TAPValidity RequestedAPValidityL();
	    
	    /**
	    *A query function to find out whether there is a connection which
	    *was matches with the "current" connection parameters
	    *@return ETrue if the condition above is satisfied EFalse otherwise.
	    */
	    IMPORT_C TBool Connected();
	    
	    /**
	    *Call this for obtaining the AP for the current connection
	    *@return the AP for the latest connection
	    * NULL if there isn't any
	    */
	    IMPORT_C const CApAccessPointItem* CurrentAccessPoint() const;

	    /**
	    *Returns the currently (or the last used) AP id
	    *@return the currently (or the last used) AP id
	    */
	    IMPORT_C TUint32 CurrentAPId() const;
	    
	    /**
	    *Call this the find out the session security mode for the current AP
	    *@retun the security mode for the current AP
	    */
	    IMPORT_C TBool CurrentSessionSecure() const;
	    
	    /**
	    *Call this the find out the connection type for the current AP
	    *@retun the conection type for the current AP
	    */
	    IMPORT_C TBool CurrentSessionConnectionOriented() const;
	    
	    /**
	    *Call this the get the gateway address to be used
	    *@retun the address of the gateway associated with the current AP, space for zero terminator should also be allocated
	    */
	    IMPORT_C HBufC* CurrentGatewayLC() const;
	    
		    
	    /**
	    *Call this the get the start page to be used
	    *@return the address of the start page associated with the current AP, space for zero terminator should also be allocated
	    */
	    IMPORT_C HBufC* CurrentStartPageLC() const;
	    
	    /**
	    *Call this function to get the name of the current Wap AP, space for zero terminator should also allocated
	    *return 
	    */
	    IMPORT_C HBufC* CurrentApNameLC() const;

	    /**
	    *Call this the get the current connection speed to be used
	    *@return the the connection speed 
	    */
	    IMPORT_C TApCallSpeed CurrentConnectionSpeed() const;
	    
	    /**
	    *Call this the get the current bearer to be used
	    *@return the bearer type associated with the current AP
	    */
	    IMPORT_C TApBearerType CurrentBearerTypeL() const;

	    /**
	    *It closes the connection and sets the manager to offline mode
	    */
	    IMPORT_C void Disconnect();
	    
	    /**
	    *Sets an observer on the AP changes
	    *@param aObserver reference to the the AP change observer 
	    */
	    IMPORT_C void SetApChangeObserver( MApChangeObserver& aObserver );
	    
	    /**
	    *Removes the AP change observer
	    */
	    IMPORT_C void UnsetApChangeObserver();

	    /**
	    *Starts the connection process
	    *@param aDisableConnNeeded if set to ETrue the initial dialog indicating that there is no connection will not appear
	    *@param aIgnoreSecureAttribute if set to ETrue the security of the AP will not be taken into account when making 
		                                making decisions about which AP to use the requested or the active
	    */
	    IMPORT_C TInt StartConnectionL( TBool aDisableConnNeeded = EFalse, TBool aIgnoreSecureAttribute = EFalse );

        /**
        * Returns pointer to the currently used AP.
        * @return pointer to the currently used AP. Can be NULL.
        */
        IMPORT_C CApAccessPointItem* CurrentAP();

	    /**
        *Updates the current AP according to the specified base AP, it is possible to update only the WAP part
        *of the AP, and thus create a mixed AP for the current connection (and the session about to be established)
        *@param aBaseAP the base AP according to which the current AP will be updated
        *@param aOnlyWAPPart if set to ETrue only the WAP part of the current AP will be updated. otherewise the whole AP
        */
        void UpdateCurrentAPL( CApAccessPointItem& aBaseAP, TBool aOnlyWAPpart );

        /**
        * Returns the requested id pair.
        * @return requested id pair
        */
        TIdPair& RequestedIdPair();

        /**
        * Returns TRUE if there is no second preference.
        * @return TRUE if there is no second preference
        */
        TBool NoSecondPreference() const;

        /**
        * Set if there is second preference
        * @param aNoSecondPreference TRUE if no second preference
        */
        void SetNoSecondPreference( TBool aNoSecondPreference );

        /**
        * Return TRUE if silent mode set.
        * @return TRUE if silent mode set.
        */
        TBool SilentMode() const;

        /**
        * Returns pointer to CApUtils object.
        * @return pointer to CApUtils object. Always valid.
        */
        CApUtils* ApUtils();

        MApChangeObserver* ApObserver();

        /**
        *Returns the RConnection object used to create connection.
        *@return RConnection object
        */
        IMPORT_C RConnection& Connection();

        /**
        *Return name of the connection created.
        *Ownership of name is handed over.
        *@return name of the connection
        */
        IMPORT_C TName* ConnectionNameL();

        /**
        * Return socket server
        * @return socket server
        */
        IMPORT_C RSocketServ& SocketServer();
        /**
				*Functions that are used to store the connection type and the snap ID
				*/
        IMPORT_C void SetConnectionType( CMManager::TCmSettingSelectionMode aConnectionType );
	    
	    	IMPORT_C void SetRequestedSnap (TUint32 aRequestedSnapId);

        /**
        * Return ETrue if we are in a 3G network
        * @return ETrue if we are in a 3G network
        */
        TBool Check3GNetworkL();

        /**
        *Checks if there is an active (HS)CSD connection in the system
        *@return TRUE if there is
        */
        EXPORT_C TBool IsThereActiveHSCSDConnectionL();

        /**
        * Checks if there is an active voice call in the system
        * @return ETrue is there's active voice call
        */
        TBool IsThereActiveVoiceCall();

        void SetInternalError( TInt aInternalError );

    public: // Functions from base classes

        virtual void OfflineModeChanged( TBool aEnabled );

        TBool IsConnectionAllowedL( TBool aDisplayError );

    protected:

        void StartConnectionObservingL();
        void StopConnectionObserving();
        void ConnectionStageAchievedL(TInt);

        CApAccessPointItem* APItemFromAPIdLC( TUint32 aAPId );
        CApAccessPointItem* APItemFromIAPIdLC( TUint32 aIAPId );
        CApDataHandler* ApDataHandler();
        TBool CheckIfAlreadyConnected( TUint32& aNewIapId );

        void CheckVPNL( TUint32 aNewIap, TConManChangeConn& aChangeConn );        
        void UpdateEasyWlanFlagL();
        
        void DoShowConnectionChangedDlgL();
        
        void InitializeL();

	private:

    	CApAccessPointItem* iCurrentAP;
	    CApDataHandler* iApDataHandler;
		CApUtils* iApUtils;
        CVpnApItem* iVpnItem;
        CVpnApEngine* iVpnEngine;
		CCommsDatabase* iCommsDb;		
		CAknGlobalNote* iNote;
		RFs iRFs;
        RSocketServ iServ;
        RConnection iConnection;
		TInt iResOffset;

		TBool iSilentMode;
		TBool iCommsDbOwned;
		TBool iNewConnectionEstablished;
		TBool iAlreadyInUse;
		TInt iInternalError;
		TInt iLastConnectionStage;
		CAknNoteDialog* iNoteDialog;
		CActiveConnectorSyncWrapper* iSyncConnector;

        MApChangeObserver* iApObserver;
        TUint32 iConnId;
        TIdPair iRequestedAPIds;//< It stores the requested WAP AP ids (two prefs)
        
        CMManager::TCmSettingSelectionMode iConnectionType;
        TUint32 iRequestedSnapId;      
        
	    TBool iNoSecondPreference;
    	TBool iAlreadyConfirmed;    // not used anymore
        CConnectionStageNotifierWCB*   iStageNotifier;
        TBool iConnected;
        TBool iCalledFromProcessFinish;
        CAknWaitDialog* iWaitDialog;
        CRepository* iRepository;
        TBool iInitialized;
        TBool iEasyWlan;

        HBufC* iConnName; // Temp fix for CDMA

        RTelServer  iTelServer;
        RMobilePhone    iMobilePhone;
        TInt iSatges[KMaxStages];
	};

#endif// End of File
