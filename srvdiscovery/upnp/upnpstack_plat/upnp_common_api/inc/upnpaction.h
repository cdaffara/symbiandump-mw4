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
* Description:  Declares the CUpnpAction class
*
*/


#ifndef C_CUPNPACTION_H
#define C_CUPNPACTION_H

// INCLUDES

#include <e32base.h>
#include <in_sock.h>
#include "upnperrors.h"

// FORWARD DECLARATIONS

class CUpnpSoapMessage;
class CUpnpService;
class CUpnpArgument;

// CLASS DECLARATION

/**
*  Used to make a common body for actions. \
*  This class is only used when an action is sent or received. \
*  This class makes a common body for actions, which can then be modified by user. \
* 
*  @since Series60 2.6
*/
class CUpnpAction : public CBase
{ 
public: // Constructors and destructor                  
                    
    /**
    * Two-phased constructor for building from a XML node.
    * Adds the arguments and their default values according to the XML node.
    */
    IMPORT_C static CUpnpAction* NewLC( CUpnpAction& aAction, 
                                        CUpnpService& aParentService );
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CUpnpAction();
    
public: // New functions
        
    /**
    * Sets error code
    * @since Series S60 3.1
    * @param aError
    */
    IMPORT_C void SetError( TInt aError );
    
    /**
    * Get service
    * @since Series60 2.6
    * @return Parent service.
    */
    IMPORT_C CUpnpService& Service();
    
    /**
    * Getter.
    * @since Series60 2.6
    * @return Error code
    */
    IMPORT_C TInt Error() const;
    
    /**
    * Get session ID
    * @since Series60 2.6
    * @return Session ID.
    */
    IMPORT_C TInt SessionId() const;
    
    void SetSessionId(TInt aId);
     
    /**
    * Get argument tyoe
    * @since Series60 2.6
    * @param aArgumentName Name of the argument.
    * @return Type of the argument.
    */
    IMPORT_C TInt ArgumentType( const TDesC8& aArgumentName );
    
    /**
    * Get argument list
    * @since Series60 2.6
    * @return List of the argumets.
    */
    IMPORT_C RPointerArray<CUpnpArgument>& ArgumentList();
    
    /**
    * Get action name
    * @since Series60 2.6
    * @return Name of the action
    */
    IMPORT_C TDesC8& Name();
    
    /**
    * Modifies an existing argument of the action.
    * @since Series60 2.6
    * @param aName the name of the argument.
    * @param aValue the value of the argument.
    * @return status of operation.
    */
    IMPORT_C TInt SetArgumentL( const TDesC8& aName, const TDesC8& aValue );
    
    /**
    * Modifies an existing argument of the action.
    * @since Series60 2.6
    * @param aName Name of the argument.
    * @return KErrNone if set succeeds, KErrBadName if not.
    */
    IMPORT_C const TDesC8& ArgumentValue( const TDesC8& aName );
    
    /**
    * Returns the argument.
    * @since Series60 2.6
    * @param aName Name of the argument.
    * @return argument
    */
    IMPORT_C CUpnpArgument* Argument( const TDesC8& aName );
    
    /**
    * Get destination address
    * @since Series60 2.6
    * @return Destination address of the action.
    */
    IMPORT_C TInetAddr DestinationAddr() const;
    
    /**
    * Set destination address
    * @since Series60 2.6
    * @param aAddr Destination address of the action.
    */
    IMPORT_C void SetDestinationAddr( const TInetAddr & aAddr );
    
    /**
    * Set destination path
    * @since Series60 2.6
    * @param aDestination Destination path of the action (destination service).
    */
    IMPORT_C void SetDestinationPathL( const TDesC8& aDestination );
    
    /**
    * Get destination path
    * @since Series60 2.6
    * @return Destination path of the action.
    */
    IMPORT_C const TDesC8& DestinationPath();
    
    /**
    * Set sender address
    * @since Series60 2.6
    * @param aSender Source address of the action.
    */
    IMPORT_C void SetSender( const TInetAddr & aSender );
    
    /**
    * Get source address of sender address
    * @since Series60 2.6
    * @return Source address of the action.
    */
    IMPORT_C TInetAddr& Sender();
    
    /**
    * Get service type
    * @since Series60 2.6
    * @return Service type.
    */
    IMPORT_C TDesC8& ServiceType();

    /**
    * Returns whether an action is local, comes from local control point.
    * @since Series60 2.6
    * @return status.
    */
    IMPORT_C TBool Local() const;        
    
    /**
    * Sets the value of iLocal.
    * @since Series60 2.6
    * @param aLocal new value of the iLocal parameter
    */
    IMPORT_C void SetLocal(TBool aLocal);        
    
public:
    /**
     * Construct action ONLY with specified name (not completely valid object)
     * for TEST use only.
     */
    IMPORT_C static CUpnpAction* NewL(const TDesC8& aActionName);
    
public: // for internal use (construct action from xml description)

  static CUpnpAction* NewL(CUpnpService& aParentService);
       
  void SetNameL(const TDesC8& aName);
       
  void AddArgumentL( CUpnpArgument& aAction );

  void  ConstructL();    
    
private: // Constructors
    
    /**
    * C++ default constructor.
    * @param aParentService Parent service.
    */
    CUpnpAction( CUpnpService& aParentService );
    
    /**
    * C++ default constructor.
    */
    CUpnpAction();   
    
    /**
    * By default Symbian 2nd phase constructor is private.
    * Adds the arguments and their default values according to the XML node
    */
    void ConstructL( CUpnpAction& aAction );
    
private: // Data
     
     // Session ID
     TInt                            iSessionId;
     
     // Error code
     TInt                            iError;

     // indicate if action is local, comes from local control point
     TInt                            iLocal;
     
     // Name of the action
     HBufC8*                         iName;
     
     // Source address
     TInetAddr                       iSender;
     
     // Destination service
     HBufC8*                         iDestinationPath;
            
     // Destination address
     TInetAddr                       iDestinationAddr;
     
     // Arguments of the action
     RPointerArray<CUpnpArgument>    iArguments;
     
     // Parent service
     CUpnpService&                   iParentService; 
};

#endif // C_CUPNPACTION_H

// End of File
