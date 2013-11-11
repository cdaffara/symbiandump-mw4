/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implements the CUpnpService class
*
*/


#ifndef C_CUPNPSERVICE_H
#define C_CUPNPSERVICE_H

// INCLUDES

#include <e32std.h>
#include <e32des8.h>
#include "upnpaction.h"
#include "upnpstatevariable.h"
#include "upnpdispatchercustomer.h"
#include "upnphttpmessage.h"

// CONSTANTS

_LIT8( KAction,     "action" );
_LIT8( KStateVariable,  "stateVariable" );
_LIT8( KProperty,   "property" );

// FORWARD DECLARATIONS

class CUpnpDevice;
class CUpnpAction;
class CUpnpServiceImplementation;
class CUpnpDispatcher;
class CUpnpHttpMessage;
class CUpnpGenaMessage;
class MUpnpContentDirectoryDataFinder; 

// CLASS DECLARATION

/**
*  SCPD handling.
*  This is a data class containing the data of the service description
*  (SCPD). This data consists of actions, their arguments and state variables.
*
*  @since Series60 2.6
*/
class CUpnpService : public CBase
{
public: // Constructors and destructor

    IMPORT_C TBool IsAdded();
    
    /** Two-phased constructor, called from other NewL functions.
    * Only for internal usage. 
    * @param aDescription Content of the service description.
    * @param aServiceType Type of the service.
    * @param aDevice Parent device.
    */
    IMPORT_C static CUpnpService* NewL( const TDesC8& aDescription, 
                                        const TDesC8& aServiceType, 
                                        CUpnpDevice& aDevice );
                        
    /** 
    * Destructor
    */
    IMPORT_C virtual ~CUpnpService();
        
public: // New functions

  
    /**
    * Sets the value of a state variable. If no such named state variable
    * found, ignore.
    * @since Series60 2.6
    * @param aName Name of the variable (in SCPD).
    * @param aValue Requested value for state variable.
    */
    IMPORT_C void SetStateVariableL( const TDesC8& aName, 
                     const TDesC8& aValue );

    /**
    * Returns the requested state variable
    * @since Series60 2.6
    * @param aName Name of the variable (in SCPD).
    * @return CUpnpStateVariable*. Points to NULL if state variable not found.
    */
    IMPORT_C CUpnpStateVariable* StateVariable( const TDesC8& aName );
    
    /**
    * Get service type of this (local or remote) service
    * @since Series60 2.6
    * @return Service type.
    */
    IMPORT_C const TDesC8& ServiceType();
    
    void SetServiceTypeL( const TDesC8& aSercviceType );
    /**
    * Get message dispatching path. For internal use only. 
    * @since Series60 2.6
    * @return path
    */
    IMPORT_C const TDesC8& Path();
	
    void SetPathL( const TDesC8& aPath );
        
    /**
    * Get parent device of this service. 
    * @since Series60 2.6
    * @return Parent device.
    */
    IMPORT_C CUpnpDevice& Device();
   
    /**
    * For internal use. Get subscription url address of this service.  
    * To make subscriptions use CUpnpControlPoint::SubscribeL( CUpnpService* )
    * @since Series60 2.6
    * @return subscription url address
    */
    IMPORT_C const TPtrC8 SubscriptionUrl();
    
    /**
    * For internal use. Set subscription url address of this service.  
    * To make subscriptions use CUpnpControlPoint::SubscribeL( CUpnpService* )    
    * @param aUrl subscription url address, ownership is passed
    */
    IMPORT_C void SetSubscriptionUrl(HBufC8* aUrl);

    /**
    * For internal use. Get control url address of this service.  
    * @return control url address
    */
    IMPORT_C const TPtrC8 ControlUrl();
        
    /**
    * For internal use. Set control url address of this service.  
    * @param aUrl control url address, ownership is passed
    */
    IMPORT_C void SetControlUrl(HBufC8* aUrl);
    
    /**
    * For internal use. Get service url address of this service.  
    * @return control url address
    */
    IMPORT_C const TPtrC8 ServiceDescriptionUrl();
    
    /**
    * For internal use. Set control url address of this service.  
    * @param aUrl service url address, ownership is passed
    */
    IMPORT_C void SetServiceDescriptionUrl(HBufC8* aUrl);
        
    /**
    * Get IP address of this (local or remote) service. 
    * @since Series60 2.6
    * @return address
    */
    IMPORT_C TInetAddr Address();
    
   
    
    /**
    * Returns an action with the given name. If such action found,
    * the pointer aAction is set to that action. 
    * @since Series60 2.6
    * @param aAction XML element where action is placed.
    * @param aActionName Name of the action.
    * @return ETrue, if action is found.
    */
    IMPORT_C TBool GetAction( CUpnpAction*& aAction, 
                  const TDesC8& aActionName );
    
    /**
    * For internal use. Sets the aActionList pointer to point to
    * list of XML elements describing actions.
    * @since Series60 2.6
    * @param aActionList pointer array containing the action list    
    */
    IMPORT_C void GetActionList( RPointerArray<CUpnpAction>& aActionList );
    
    /**
    * Get the state variable list of this action. See interface of 
    * CUpnpStateVariable to use state variables. 
    * @since Series60 2.6
    * @return Gives list of state variables of service.
    */
    IMPORT_C RPointerArray<CUpnpStateVariable> StateVariableList() const;
    
    IMPORT_C void AddStateVariableL( CUpnpStateVariable* aStateVariable );
    /**
    * Creates and returns a new action. This function is proposed to be
    * used by control points (derived from CUpnpControlPoint).
    * The target IP address and other details are already set in the 
    * CUpnpAction structure, and after setting the parameters it can be 
    * sent using CUpnpServiceImplementation or CUpnpControlPoint interface.
    * @since Series60 2.6
    * @param aActionName Name of the action.
    * @return action object
    */
    IMPORT_C CUpnpAction* CreateActionLC( const TDesC8& aActionName );

   
        
public: // From MUpnpDispatcherCustomer

    
    void SetArgumentTypes();
    
    TBool IsComplete();
    
protected:

    /**
    * C++ default constructor.
    * @param aDevice Parent device.
    */
    IMPORT_C CUpnpService( CUpnpDevice& aDevice );
    
    /**
    * By default Symbian 2nd phase constructor is private.
    * @param aDesciption Content of the service description.
    * @param aServiceType Type of the service.
    */
    IMPORT_C void ConstructL( const TDesC8& aDescription, 
                  const TDesC8& aServiceType );  

public:
    static CUpnpService* NewL(CUpnpDevice* aParent);
      
    void AddActionL( CUpnpAction& aAction );

    void Added();
    
protected: // Data
      
    // State variable list, owned
    RPointerArray<CUpnpStateVariable>           iStateVariables;

    // Action variable list, owned
    RPointerArray<CUpnpAction>                  iActions;

    // Not owned; cannot be NULL
    CUpnpDevice&                                iParentDevice;

    
    // Service type, owned
    HBufC8*                                     iType;

    // Used to identify messages, owned
    HBufC8*                                     iPath;
    // subscription url
    HBufC8*                                     iSubscriptionUrl;
    // control url
    HBufC8*                                     iControlUrl;
    // adding flag    
    TBool                                       iIsAdded;
    // complete flag    
    TBool                                       iComplete;
    // url    
    HBufC8*                                 	iUrl;
    };
    

#endif  // C_CUPNPSERVICE_H

// End of File
