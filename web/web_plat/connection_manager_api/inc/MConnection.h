/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
*     This interface class defines a protocolconnection/session
*	  handling.
*	
*
*/


#ifndef M_Connection_H
#define M_Connection_H

#warning The Connection Manager API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES

#include <ApEngineConsts.h> // TEMP
#include <CmApplicationSettingsUi.h>

//FORWARD DECLARATIONS
class TIdPair;
class MApChangeObserver;
class CApAccessPointItem;
class RSocketServ;
class RConnection;

enum TAPValidity
	{
	EFirst,
	ESecond,
	EBoth,
	ENeither,
	};

enum TConManChangeConn
	{
	EConManDoNothing,
	EConManCloseAndStart,
	EConManStartAgain,
	};

//This class provides a standerd interface for the WML Browser towards any concrete implementation of a connection manager
class MConnection
	{
public:
	/**
	*Sets the requested AP later it may not be the one to be used
	*@param aRequestedAPs the requested AP idpair 
	*/
	virtual void SetRequestedAPs( TIdPair aRequestedAPs ) = 0;
	
	/**
	*Sets RequestedAP
	*@param aRequestedAP the requested AP id
	*/
	virtual void SetRequestedAP( TUint32 aRequestedAPId , TBool aDefault = EFalse ) = 0;
		
	/**
	*Functions that are used to store the connection type and the snap ID
	*/
    virtual void SetConnectionType( CMManager::TCmSettingSelectionMode aConnectionType ) = 0;
    
    virtual void SetRequestedSnap (TUint32 aRequestedSnapId) = 0;
	
	virtual TAPValidity RequestedAPValidityL() = 0;
	
	/**
	*A query function to find out whether there is a connection which
	*was matches with the "current" connection parameters
	*@return ETrue if the condition above is satisfied EFalse otherwise.
	*/
	virtual TBool Connected() = 0;
	
	/**
	*Call this for obtaining the AP for the current connection
	*@return the AP for the latest connection
	* NULL if there isn't any
	*/
	virtual const CApAccessPointItem* CurrentAccessPoint() const = 0;

	/**
	*Returns the currently (or the last used) AP id
	*@return the currently (or the last used) AP id
	*/
	virtual TUint32 CurrentAPId() const = 0;
	
	/**
	*Call this the find out the session security mode for the current AP
	*@retun the security mode for the current AP
	*/
	virtual TBool CurrentSessionSecure() const = 0;
	
	/**
	*Call this the find out the connection type for the current AP
	*@retun the conection type for the current AP
	*/
	virtual TBool CurrentSessionConnectionOriented() const = 0;
	
	/**
	*Call this the get the gateway address to be used
	*@retun the address of the gateway associated with the current AP, space for zero terminator should also be allocated
	*/
	virtual HBufC* CurrentGatewayLC() const = 0;
	
		
	/**
	*Call this the get the start page to be used
	*@return the address of the start page associated with the current AP, space for zero terminator should also be allocated
	*/
	virtual HBufC* CurrentStartPageLC() const = 0;
	
	/**
	*Call this function to get the name of the current Wap AP, space for zero terminator should also allocated
	*return 
	*/
	virtual HBufC* CurrentApNameLC() const = 0;

	/**
	*Call this the get the current connection speed to be used
	*@return the the connection speed 
	*/
	virtual TApCallSpeed CurrentConnectionSpeed() const = 0;
	
	/**
	*Call this the get the current bearer to be used
	*@return the bearer type associated with the current AP
	*/
	virtual TApBearerType CurrentBearerTypeL() const = 0;

	/**
	*It closes the connection and sets the manager to offline mode
	*/
	virtual void Disconnect() = 0;
	
	/**
	*Sets an observer on the AP changes
	*@param aObserver reference to the the AP change observer 
	*/
	virtual void SetApChangeObserver( MApChangeObserver& aObserver ) = 0;
	
	/**
	*Removes the AP change observer
	*/
	virtual void UnsetApChangeObserver() = 0;


	/**
	*Returns whether a new connection needed to be established or the old one is used.
	*/
	virtual TBool NewConnectionEstablished()  const = 0;

	/**
	*Starts the connection process
	*@param aDisableConnNeeded if set to ETrue the initial dialog indicating that there is no connection will not appear
	*@param aIgnoreSecureAttribute if set to ETrue the security of the AP will not be taken into account when making 
                                                   making decisions about which AP to use the requested or the active
	*/
	virtual TInt StartConnectionL( TBool aDisableConnNeeded = EFalse, TBool aIgnoreSecureAttribute = EFalse ) = 0;
	
    /**
    *Returns the RConnection object used to create connection.
    *@return RConnection object
    */
    virtual RConnection& Connection() = 0;

    /**
    *Return name of the connection created.
    *Ownership of name is handed over.
    *@return name of the connection
    */
    virtual TName* ConnectionNameL() = 0;

    /**
    * Return socket server
    * @return socket server
    */
    virtual RSocketServ& SocketServer() = 0;

    /**
    * Change Internet Accespoint ( Change Connection )
    * @return
    */
    virtual void ChangeIapL( TConManChangeConn& aCangeConn,
                             TUint32& aNewAp ) = 0;

    /**
    * Ask Internet Accespoint from dialog
    * @return KErrNone if the user selected an Iap or and error code
    */
    virtual TInt AskIap( TUint32& aNewAp  ) = 0;

    /**
    * Shows the connection changed dialog.
    * @return none.
    */
    virtual void ShowConnectionChangedDlg() = 0;
    };

#endif
